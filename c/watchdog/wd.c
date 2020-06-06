/****************************************
* Author: ol78							*
* Last Update: 15/01/2020				*
* Data structure: Watchdog.				*
****************************************/
#define _GNU_SOURCE

#define WAITING_TIME 5

#include <stdio.h> /* for perror */
#include <assert.h> /* for assert */
#include <stdlib.h> /*for malloc, getenv */
#include <unistd.h> /* for fork and such */
#include <errno.h> /* for errors */
#include <pthread.h> /* for pthread_create */
#include <fcntl.h> /* for O_CREAT */       
#include <sys/stat.h> /* for S_IRWXU */ 
#include <signal.h> /* for signals */
#include <semaphore.h> /* for sem funcs*/
#include <time.h> /* for clock_gettime */

#include "shared_wd.h"

typedef enum {FAIL = -1, SUCCESS} e_result;

static e_stop_flag stop_flag = 0;

/******************************************************************************/
/*----------------------------auxilary functions------------------------------*/

static void *ThreadFunc(void *arg);

static int StopSchedIMP(void *param);

/******************************************************************************/
/*----------------------------MMI function------------------------------------*/

wd_t *MMI(char *argv[])
{
	e_result pthread_res = 0;
	e_result clock_res = 0;
	sem_t *mmi_sem = NULL;
	struct timespec timeout = {0};
	wd_t *wd = NULL;
		
		
	mmi_sem = sem_open("mmi_sem", O_CREAT, S_IRWXU, 0);
	if (mmi_sem == SEM_FAILED)
	{
#ifndef NDEBUG
		perror("mmi sem_open error");
#endif
	
		return NULL;
	}
	
	wd = malloc(sizeof(wd_t));
	if (NULL == wd)
	{
		return NULL;
	}
	wd->argv = argv;
	
	pthread_res = pthread_create(&(wd->pthread), NULL, ThreadFunc, wd);
	if (0 != pthread_res)
	{
#ifndef NDEBUG
		perror("pthread create error");
#endif
		
		free(wd);
		
		return NULL;
	}
	
	clock_res = clock_gettime(CLOCK_REALTIME, &timeout);
	if (clock_res !=  SUCCESS)
	{
#ifndef NDEBUG
		perror("clock_gettime error");
#endif
		
		free(wd);
		
		return NULL;
	}
	
	timeout.tv_sec += WAITING_TIME;
	if (SUCCESS != sem_timedwait(mmi_sem, &timeout))
	{
#ifndef NDEBUG
		perror("mmi_sem wait error");
#endif
		
		free(wd);
		
		return NULL;
	}
	
	sem_close(mmi_sem);

	return wd;
}

/******************************************************************************/
/*----------------------------DNR function------------------------------------*/

int DNR(wd_t *wd)
{	
	int pthread_join_res = 0;
	
	assert(NULL != wd);
	
	__sync_lock_test_and_set (&stop_flag, TIME_TO_DIE);
		
	pthread_join_res = pthread_join(wd->pthread, NULL);
	
	sem_unlink("thread_sem");
	sem_unlink("mmi_sem");
	sem_unlink("wd_sem");
	
	free(wd);	
	
	return pthread_join_res;
}

/*****************************************************************************/
/*----------------------------Thread function--------------------------------*/

static void *ThreadFunc(void *wd_wrapped)
{
	sem_t *mmi_sem = NULL;
	sem_t *wd_sem = NULL; 
	e_result sem_post_res = 0;
	e_result sched_res = 0;
	char *watch_dog_env = NULL;
	struct sigaction thread_signal = {0};
	wd_t *wd = wd_wrapped;
	ilrd_uid_t stop_uid = {0};
	
	thread_signal.sa_handler = &ResetCounter;
	sigaction(SIGUSR1, &thread_signal, NULL);
	
	wd->prog_name = "./process_wd";
	
	write(0, "\n\nChihuahua Is Alive\n\n\n", 23);
	
	/* checking if watchdog exists */
	watch_dog_env = getenv("WOOF_WOOF");
	if (NULL == watch_dog_env)
	{
		wd->restarted_process_pid = CreateProgram("./process_wd",
									 wd->argv, "thread_sem");
		if (FAIL == wd->restarted_process_pid)
		{
			return NULL;	
		}
	}
	else
	{
		
		wd->restarted_process_pid = getppid();
		
		wd_sem = sem_open("wd_sem",O_CREAT, S_IRWXU, 0);
		if (wd_sem == SEM_FAILED)
		{
#ifndef NDEBUG
			perror("wd sem_open error");
#endif
			return NULL;
		}
		
		sem_post_res = sem_post(wd_sem);
		if (FAIL == sem_post_res)
		{
#ifndef NDEBUG
			perror("wd sem_post error");
#endif
			return NULL;
		}
		
	}
	
	wd->prog_name = "./process_wd";
	wd->sem_name = "thread_sem";
	
	sched_res = SchedulerCreateAndAdd(wd);
	if (FAIL == sched_res)
	{
		write(0, "\nFailed to create Sched\n", 24); 
		return NULL;
	}
	
	stop_uid = SchedulerAdd(wd->sched, &StopSchedIMP, 1, wd);
	if (UIDIsBad(stop_uid))
	{ 
		SchedulerDestroy(wd->sched);
		return NULL;
	}
	
	mmi_sem = sem_open("mmi_sem", 0);
	if (mmi_sem == SEM_FAILED)
	{
#ifndef NDEBUG
		perror("mmi sem_open2 error");
#endif
		
		return NULL;
	}
	
	sem_post_res = sem_post(mmi_sem);
	if (FAIL == sem_post_res)
	{
#ifndef NDEBUG
		perror("mmi sem_post error");
#endif
		
		return NULL;
	}
			
	SchedulerRun(wd->sched);
	sem_close(mmi_sem);
	SchedulerDestroy(wd->sched);
	
	return NULL;
}

/******************************************************************************/
/*----------------------------StopSchedIMP -----------------------------------*/

static int StopSchedIMP(void *param)
{
	wd_t *wd = param;
	
	assert(NULL != param);
	
	if (TIME_TO_DIE == stop_flag)
	{
		/*sem_t *stop_sem = 0;
			
		stop_sem = sem_open("stop_sem",O_CREAT, S_IRWXU, 0);
		if (stop_sem == SEM_FAILED)
		{
#ifndef NDEBUG
			perror("stop_sem open failed");
			
			return 1;
#endif
		}*/
				
		kill(wd->restarted_process_pid, SIGUSR2);
		
		SchedulerStop(wd->sched);	
						
		/*while (SUCCESS != sem_trywait(stop_sem))
		{
#ifndef NDEBUG
			perror("\nsem_trywait error");
#endif
			kill(wd->restarted_process_pid, SIGUSR1);
		}*/
		
		/*sem_wait(stop_sem);
		
		sem_close(stop_sem);*/
		
		return 0;
	}
	
	return 1;	 	
}


