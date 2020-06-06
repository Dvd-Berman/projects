/****************************************
* Author: David Berman			        *
* Last Update: 24/11/2019		        *
* Data structure: Scheduler           	*
* Code file.                            *
* **************************************/

#include <assert.h> /* for assert */
#include <stdlib.h> /* for malloc */
#include <time.h> /* for time variable */
#include <unistd.h> /* for sleep */

#include "pq.h"
#include "uid.h"
#include "event.h"
#include "sched.h"

enum res {SUCCESS, FAILURE};
enum event_execution {STOP_EXECUTION = -1, NO_REPEAT};

#define STOP_NOW -1

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

struct scheduler
{
	pq_t *pq;
	event_t *curr_event;
	int stop_flag;
};

/******************************************************************************/
/*------------------------- Auxilary functions--------------------------------*/

static int CmpEvents(void *event1, void *event2, void *param);
static int IsMatchEvents(const void *event, void *event_id);

/******************************************************************************/
/*------------------------- SchedulerCreate-----------------------------------*/

scheduler_t *SchedulerCreate(void)
{
	scheduler_t *sched = (scheduler_t *)malloc(sizeof(*sched));
	
	if (NULL != sched)
	{
		sched->pq = PQCreate(&CmpEvents, NULL);
		if (NULL != sched->pq)
		{
			sched->curr_event = NULL;
			sched->stop_flag = 0;
		}
		else
		{
			free(sched);
			sched = NULL;
		}
	}
	
	return sched;		
}

/******************************************************************************/
/*------------------------- SchedulerDestroy----------------------------------*/

void SchedulerDestroy(scheduler_t *sched)
{
	assert(NULL != sched);
	assert(NULL != sched->pq);
	
	SchedulerClear(sched);
	PQDestroy(sched->pq);
	sched->pq = NULL;
	
	free(sched);
}

/******************************************************************************/
/*------------------------- SchedulerRun--------------------------------------*/

long SchedulerRun(scheduler_t *sched)
{
	size_t wait_time = 0;
	int execute_res = 0;
	
	assert(NULL != sched);
	
	while (!SchedulerIsEmpty(sched) && STOP_NOW != sched->stop_flag)
	{		
		sched->curr_event =  PQDequeue(sched->pq);
		wait_time = EventGetDelay(sched->curr_event);
		
		while(0 != wait_time)
		{
			wait_time = sleep(wait_time);
		}
		
		execute_res = EventExecute(sched->curr_event);
		if (NULL != sched->curr_event) 
		{
			if (NO_REPEAT != execute_res)
			{	
				EventUpdateNextOccurence(sched->curr_event);
				if (FAILURE == PQEnqueue(sched->pq, sched->curr_event))
				{
					EventDestroy(sched->curr_event);
					
					return STOP_EXECUTION; 
				}
			}
			else
			{
				EventDestroy(sched->curr_event);
			}
		}
		
		sched->curr_event = NULL;
	}	
	
	return SchedulerSize(sched); 
}

/******************************************************************************/
/*------------------------- SchedulerStop-------------------------------------*/

void SchedulerStop(scheduler_t *sched)
{
	assert(NULL != sched);

	sched->stop_flag = STOP_NOW;
}

/******************************************************************************/
/*------------------------- SchedulerDestroy----------------------------------*/

size_t SchedulerSize(const scheduler_t *sched)
{
	size_t size = 0;
	
	assert(NULL != sched);
	
	size = PQSize(sched->pq);
	
	if (NULL != sched->curr_event)
	{
		++size;
	}
	
	return size;
}

/******************************************************************************/
/*------------------------- SchedulerIsEmpty----------------------------------*/

int SchedulerIsEmpty(const scheduler_t *sched)
{
	assert(NULL != sched);
	
	return (PQIsEmpty(sched->pq) && NULL == sched->curr_event);
}

/******************************************************************************/
/*------------------------- SchedulerClear------------------------------------*/

void SchedulerClear(scheduler_t *sched)
{
	event_t *front_event = NULL;
	
	assert(NULL != sched);
	
	if (NULL != sched->curr_event)
	{
		EventDestroy(sched->curr_event);
		sched->curr_event = NULL;
	}
	
	while (!SchedulerIsEmpty(sched))
	{
		front_event = PQDequeue(sched->pq);
		EventDestroy(front_event);
	}
}

/******************************************************************************/
/*------------------------- SchedulerAdd--------------------------------------*/

ilrd_uid_t SchedulerAdd
   (scheduler_t *sc, int (*op_func)(void *param), size_t interval, void *param)
{
	event_t *event_to_be_added = NULL;
	int res_of_enqueue = FAILURE;
	ilrd_uid_t uid = UIDGetBad();
	
	assert(NULL != sc);
	assert(NULL != op_func);
	
	event_to_be_added = EventCreate(op_func, interval, param);
	if (NULL != event_to_be_added)
	{
		res_of_enqueue = PQEnqueue(sc->pq, event_to_be_added);
		if (SUCCESS == res_of_enqueue)
		{
			uid = EventGetUID(event_to_be_added); 	 
		}
		else
		{
			EventDestroy(event_to_be_added);
		}
	}
	
	return uid;
}

/******************************************************************************/
/*------------------------- SchedulerRemove-----------------------------------*/

int SchedulerRemove(scheduler_t *sched, ilrd_uid_t event_id)
{
	void *event = NULL;
	
	assert(NULL != sched);
	
	if (NULL != sched->curr_event && EventIsSameUID(sched->curr_event, event_id))
	{
		EventDestroy(sched->curr_event);
		sched->curr_event = NULL;
		
		return SUCCESS;
	}
	else
	{
		event = PQErase(sched->pq, IsMatchEvents, &event_id);
		if (NULL != event)
		{
			EventDestroy(event);
		
			return SUCCESS;
		}
		
		return FAILURE;
	}	 
}

/******************************************************************************/
/*------------------------- CmpEvents-----------------------------------------*/

static int CmpEvents(void *event1, void *event2, void *param)
{	
	(void)param;
	
	assert(NULL != event1);
	assert(NULL != event2);
	
	return EventCompareOccurence(event1, event2);
}

/******************************************************************************/
/*------------------------- IsMatchEvents-------------------------------------*/

static int IsMatchEvents(const void *event, void *event_id)
{
	assert(NULL != event);
	assert(NULL != event_id);
	
	return EventIsSameUID((event_t *)event, *(ilrd_uid_t *)event_id);
}

/******************************************************************************/
/*------------------------- End Of File---------------------------------------*/
