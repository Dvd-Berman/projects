/************************************
* Author: David Berman				*
* Last Update: 18/01/2019		 	*
* Data structure: Shared_wd     	*
************************************/

#ifndef __SHARED_WD_H__
#define __SHARED_WD_H__

#include <pthread.h>

#include "wd.h"
#include "sched.h"

/******************************************************************************/

typedef enum {ALIVE, TIME_TO_DIE}e_stop_flag; 

/******************************************************************************/

struct wd
{
	char *prog_name;
	char **argv;
	char *sem_name;
	pthread_t pthread;
	pid_t restarted_process_pid;
	scheduler_t *sched;
};

/******************************************************************************/


/******************************************************************************/

int SchedulerCreateAndAdd(wd_t *wd);

/******************************************************************************/

pid_t CreateProgram(char *prog_name, char *argv[], char *sem_name);

/******************************************************************************/

void ResetCounter(int sig);


#endif /*__SHARED_WD_H__ */
