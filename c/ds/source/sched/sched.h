/********************************
* Author: OL78					*
* Last Update: 20/11/2019		*
* Data structure: Scheduler		*
* ******************************/


#ifndef __SCHED_H__
#define __SCHED_H__

#include "uid.h"

/*** PAY ATTENTION! Scheduler time resolution is 1 second. ***/

/*---------------------------------------------------------------------------*/

typedef struct scheduler scheduler_t;

/*---------------------------------------------------------------------------*/

/* Creates a new scheduler. If succeeds - returns a pointer to it.
 * If fails - returns NULL. 
 * CAUTION! SchedulerCreate cannot be sent as an event to any "sched"! 
 * Algorithmic efficiency: O(1). */
scheduler_t *SchedulerCreate();

/*---------------------------------------------------------------------------*/

/* Destroys the scheduler. 
 * CAUTION! SchedulerDestroy cannot be sent as an event to current "sched"!
 * Algorithmic efficiency: O(n). */
void SchedulerDestroy(scheduler_t *sched);

/*---------------------------------------------------------------------------*/

/* Adds an event to the schedule. Returns ilrd_uid of the newly created event.
 * If fails to create - returns BAU_UID. 
 * Algorithmic efficiency: O(n). */
ilrd_uid_t SchedulerAdd(scheduler_t *sc, 
			int (*op_func)(void *param), size_t interval, void *param);

/*---------------------------------------------------------------------------*/

/* Removes the event from the schedule. Returns 0 if succeeds.
 * if the event has not been found - returns 1. 
 * CAUTION! This function doesn't remove and event currently running.
 * Algorithmic efficiency: O(n). */
int SchedulerRemove(scheduler_t *sched, ilrd_uid_t event_id);

/*---------------------------------------------------------------------------*/

/* Runs the current scheduler.
 * Return number of events that remain in the scheduler upon stop.
 * If fails - returns -1.
 * SchedulerRun(sched) as event won't have any effect!
 * Algorithmic efficiency: O(n). */
long SchedulerRun(scheduler_t *sched);

/*---------------------------------------------------------------------------*/

/* Stops the operation of current scheduler.
 * CAUTION! SchedulerStop must be an event added to the scheduler in order
 * to stop the running scheduler!
 * Algorithmic efficiency: O(1).  */
void SchedulerStop(scheduler_t *sched);

/*---------------------------------------------------------------------------*/

/* If there are no events in the scheduler - returns 1. Otherwise - returns 0.
 * Algorithmic efficiency: O(1). */
int SchedulerIsEmpty(const scheduler_t *sched);

/*---------------------------------------------------------------------------*/

/* Returns the number of events that remain in the current scheduler,
 * either running or stopped. 
 * Algorithmic efficiency: O(n). */
size_t SchedulerSize(const scheduler_t *sched);

/*---------------------------------------------------------------------------*/

/* Erases all the events from the scheduler. 
 * Pay attention that in this case, memory allocated by the 'current_event' 
 * should be freed! 
 * Algorithmic efficiency: O(n). */
void SchedulerClear(scheduler_t *sched);

/*---------------------------------------------------------------------------*/

#endif /* __SCHED_H__ */
