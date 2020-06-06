/****************************************
* Author: David Berman					*
* Last Update: 18/01/2020				*
* function: shared Watchdog.		    *
****************************************/

#define _GNU_SOURCE

#define WAITING_TIME 5
#define COUNTER_BARRIER 5

#include <stdio.h> /* for perror */
#include <stdlib.h> /*for malloc, getenv */
#include <assert.h> /* for assert */
#include <unistd.h> /* for fork and such */
#include <errno.h> /* for errors */
#include <pthread.h> /* for pthread_create */
#include <fcntl.h> /* for O_CREAT */       
#include <sys/stat.h> /* for S_IRWXU */ 
#include <signal.h> /* for signals */
#include <semaphore.h> /* for sem funcs*/
#include <time.h> /* for clock_gettime */
#include <sys/wait.h> /* for waitpid */

#include "wd.h"
#include "shared_wd.h"

typedef enum {FAIL = -1, SUCCESS}e_result;

static int g_counter = 0;

/******************************************************************************/
/*----------------------------Auxilary functions------------------------------*/

static int RestartProcessIfNeeded(void *param);

static int SendSignal(void *param);

/******************************************************************************/
/*----------------------------SchedulerCreateAndAdd --------------------------*/

int SchedulerCreateAndAdd(wd_t *wd)
{
	ilrd_uid_t send_signal_uid = {0};
	ilrd_uid_t counter_uid = {0};
	
	wd->sched = SchedulerCreate();
	if (NULL == wd->sched)
	{
		return FAIL;
	}
	
	send_signal_uid = SchedulerAdd(wd->sched, &SendSignal, 1, wd);
	if (UIDIsBad(send_signal_uid))
	{
		SchedulerDestroy(wd->sched);
		return FAIL;
	}
	
	counter_uid = SchedulerAdd(wd->sched, &RestartProcessIfNeeded, 1, wd);
	if (UIDIsBad(counter_uid))
	{
		SchedulerDestroy(wd->sched);
		return FAIL;
	}
	
	return SUCCESS;
}

/******************************************************************************/
/*----------------------------Send Signal Event----- -------------------------*/

static int SendSignal(void *param)
{
	wd_t *wd = param;
	
	assert(NULL != param);
	
	__sync_fetch_and_add (&g_counter, 1);
		
	write(0, "\nsending signal\n", 16);
	
	kill(wd->restarted_process_pid, SIGUSR1);
	
	return 1;	
} 

/******************************************************************************/
/*----------------------------RestartProcessIfNeeded Event--------------------*/

static int RestartProcessIfNeeded(void *param)
{
	wd_t *wd = param;

	assert(NULL != param);
	
	if (COUNTER_BARRIER <= g_counter)
	{
		wd->restarted_process_pid = 
				CreateProgram(wd->prog_name, wd->argv, wd->sem_name);
						
		if (FAIL == wd->restarted_process_pid)
		{
			write(0, "CreateProgram Failed\n", 21);

			kill(getpid(), SIGTERM);
		}
		
		__sync_lock_test_and_set (&g_counter, 0);
	}
		
	return 1;
}

/******************************************************************************/
/*----------------------------CreateProgram ----------------------------------*/

pid_t CreateProgram(char *prog_name, char *argv[], char *sem_name)
{
	pid_t prog_pid = 0;
	sem_t *sem = NULL;
	e_result clock_res = 0;
	struct timespec timeout = {0};

	sem = sem_open(sem_name, O_CREAT, S_IRWXU, 0);
	if (sem == SEM_FAILED)
	{
#ifndef NDEBUG
		perror("sem_open error");		
#endif
		
		return FAIL;
	}
	
	/* initiating waiting time semaphore */
	clock_res = clock_gettime(CLOCK_REALTIME, &timeout);
	if (clock_res !=  SUCCESS)
	{
#ifndef NDEBUG
		perror("thread clock_gettime error");
#endif
			
		return FAIL;
	}
	
	prog_pid = fork();
	if (FAIL == prog_pid)
	{
#ifndef NDEBUG
		perror("fork failed ");
#endif
		
		return FAIL;
	}
	
	if (0 == prog_pid)
	{		
		if (-1 == execvp(prog_name, argv))
		{
			kill(0, SIGTERM);
		} 	 		
	}
	else
	{
		 
		timeout.tv_sec += WAITING_TIME;
		if (SUCCESS != sem_timedwait(sem, &timeout))
		{
	#ifndef NDEBUG
			perror("sem wait error");
	#endif
		
			return FAIL;
		}
		
		sem_close(sem);
		
		return prog_pid;
	}
	
	return SUCCESS;
}

/******************************************************************************/
/*----------------------------ResetCounter -----------------------------------*/

void ResetCounter(int sig)
{
	(void)sig;
	
	write(0, "\nReseting Counter\n", 18);
	
	__sync_lock_test_and_set (&g_counter, 0);
}

