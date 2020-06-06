/****************************************
* Author: David Berman					*
* Last Update: 19/01/2020				*
* Project: Watchdog.		            *
****************************************/

#define _GNU_SOURCE

#include <stdlib.h> /* for setenv */
#include <fcntl.h> /* for O_CREAT | O_EXCL */
#include <semaphore.h> /* for semaphore */
#include <unistd.h> /* for ppid */
#include <sys/stat.h>   /* For mode constants */
#include <assert.h> /* for assert */
#include <errno.h> /* for errors */
#include <stdio.h> /* for perror */
#include <signal.h> /* for SIGTERM */

#include "shared_wd.h"

static e_stop_flag stop_flag = 0;

/******************************************************************************/
/*----------------------------enums ------------------------------------------*/

typedef enum {FAIL = -1, SUCCESS}e_result;


/******************************************************************************/
/*----------------------------Auxilary functions -----------------------------*/

static void StopWatchDog(int sig);

static int StopSchedIMP(void *param);

/******************************************************************************/
/*----------------------------main function----- -----------------------------*/

int main(int argc, char *argv[])
{
	e_result set_env_res = 0;
	e_result sched_res = 0;
	sem_t *thread_sem = NULL;
	pid_t thread_pid = getppid();
	e_result sem_post_res = 0;
	ilrd_uid_t stop_uid = {0};
	struct sigaction wd_signal = {0};
	struct sigaction stop_signal = {0};
	wd_t wd = {0};
	
	
	write(0, "\n\n\nDoberman Is Alive\n\n\n", 23);
	
	(void)argc;
	
	assert(NULL != argv);
	
	wd_signal.sa_handler = &ResetCounter;
	sigaction(SIGUSR1, &wd_signal, NULL);
	stop_signal.sa_handler = &StopWatchDog;
	sigaction(SIGUSR2, &stop_signal, NULL);
	
    set_env_res = setenv("WOOF_WOOF", "I'm Alive", 0);
    if (FAIL == set_env_res)
    {
#ifndef NDEBUG
		perror("set_env error");
#endif		
		kill(0, SIGTERM);
    }
    
    thread_sem = sem_open("thread_sem", 0);
	if (thread_sem == SEM_FAILED)
	{
#ifndef NDEBUG
		perror("thread sem_open error");
#endif		
		kill(0, SIGTERM);
	}
	
	
	wd.prog_name = argv[0];
	wd.argv = argv;
	wd.sem_name = "wd_sem";
	wd.restarted_process_pid = thread_pid;
	sched_res = SchedulerCreateAndAdd(&wd);
	if (FAIL == sched_res)
	{
		write(0, "sched create fail",17); 
		kill(0, SIGTERM);
	}
	
	stop_uid = SchedulerAdd(wd.sched, &StopSchedIMP, 1, &wd);
	if (UIDIsBad(stop_uid))
	{
		SchedulerDestroy(wd.sched);
		kill(getpid(), SIGTERM);
		
		return FAIL;
	}
	
	sem_post_res = sem_post(thread_sem);
	if (FAIL == sem_post_res)
	{
#ifndef NDEBUG
		perror("thread sem_post error");
#endif
	}
	
	
	SchedulerRun(wd.sched);
	
	sem_close(thread_sem);
	
	SchedulerDestroy(wd.sched);
	
	return 0;
}

/******************************************************************************/
/*----------------------------SchedIMP ---------------------------------------*/

static int StopSchedIMP(void *param)
{
	wd_t *wd = param;
	
	assert(NULL != param);
	
	if (TIME_TO_DIE == stop_flag)
	{
		SchedulerStop(wd->sched);
		
		return 0;
	}
		
	return 1;
}	
	 	
/******************************************************************************/
/*----------------------------StopWatchDog -----------------------------------*/

static void StopWatchDog(int sig)
{
	(void)sig;
	
	__sync_lock_test_and_set (&stop_flag, TIME_TO_DIE);
}


