/************************************
* Author: OL78					 	*
* Last Update: 20/11/2019		 	*
* Data structure: Scheduler Event	*
************************************/

#ifndef __EVENT_H__
#define __EVENT_H__

/*----------------------------------------------------------------------------*/

typedef struct event
{
	int (*op_func)(void*);
	time_t next_occurence;
	size_t interval;
	ilrd_uid_t op_uid;
	void *param;

} event_t;

/*----------------------------------------------------------------------------*/

/* Creates a new event and returns a pointer to it. If fails - returns NULL. 
 * Algorithmic complexity: O(1). */
event_t *EventCreate(int (*op_func)(void *param), size_t interval, void *param);

/*----------------------------------------------------------------------------*/

/* Updates next_occurence according to the interval. 
 * Algorithmic complexity: O(1). */
void EventUpdateNextOccurence(event_t *event);

/*----------------------------------------------------------------------------*/

/* Returns the delay to next_occurrence of the event.
 * Algorithmic complexity: O(1). */
size_t EventGetDelay(event_t *event);

/*----------------------------------------------------------------------------*/

/* Executes the op_function of the event. Returns the returned value of the 
 * function (1 if to be executed again / 0 - if not). 
 * Algorithmic complexity: O(1). */
int EventExecute(event_t *event);

/*----------------------------------------------------------------------------*/

/* Destroys current event and frees all allocated memory. 
 * Algorithmic complexity: O(1). */
void EventDestroy(event_t *event);

/*----------------------------------------------------------------------------*/

/* Compares the next_occurance fields of two events. 
 * Returns the difference. 
 * Algorithmic complexity: O(1). */
int EventCompareOccurence(event_t *event1, event_t *event2);

/*----------------------------------------------------------------------------*/

/* Compares op_uid of the event with the provided UID. 
 * If they match - returns 1. Otherwise, returns 0. 
 * Algorithmic complexity: O(1). */
int EventIsSameUID(event_t *event, ilrd_uid_t uid);

/*----------------------------------------------------------------------------*/

/* returns the uid of the event */
ilrd_uid_t EventGetUID(event_t *event);

/*----------------------------------------------------------------------------*/

#endif /* __EVENT_H__ */
