/****************************************
* Author: David Berman					*
* Last Update: 24/11/2019				*
* Data structure: event                 *
* Code File                             *
* ***************************************/

#include <assert.h> /* for assert */
#include <time.h> /*for time()*/
#include <stdlib.h> /* for malloc */

#include "uid.h"
#include "event.h"

enum comparison_between_events
		{EVENT1_WILL_BE_BEFORE_EVENT2 = -1, 
			EVENT1_AND_EVENT2_WILL_HAPPEN_IN_THE_SAME_TIME = 0, 
				EVENT2_WILL_BE_BEFORE_EVENT1 = 1};

/******************************************************************************/
/*------------------------- EventCreate function -----------------------------*/

event_t *EventCreate(int (*op_func)(void *param), size_t interval, void *param)
{
	event_t *event = NULL;
	ilrd_uid_t uid;

	assert(NULL != op_func);
		
	uid = UIDCreate();
	if (!UIDIsSame(uid, UIDGetBad()))
	{
		event = (event_t *)malloc(sizeof(*event));
		if (NULL != event)
		{
			event->op_func = op_func;
			event->next_occurence = time(NULL) + interval;
			event->interval = interval;
			event->op_uid = uid;
			event->param = param;
		}
	}
	
	return event;
}

/******************************************************************************/
/*------------------------- EventDestroy function ----------------------------*/

void EventDestroy(event_t *event)
{
	assert(NULL != event);
	
	free(event);
}

/******************************************************************************/
/*------------------------- EventUpdateNextOccurence function ----------------*/				

void EventUpdateNextOccurence(event_t *event)
{
	assert(NULL != event);
	
	event->next_occurence += event->interval;
}

/******************************************************************************/
/*------------------------- EventGetDelay function ---------------------------*/				

size_t EventGetDelay(event_t *event)
{
	double diff = 0;
	
	assert(NULL != event);
	
	diff = difftime(event->next_occurence, time(NULL));
	
	return ((diff < 0) ? 0 : diff);
}

/******************************************************************************/
/*------------------------- EventExecute function ----------------------------*/				

int EventExecute(event_t *event)
{
	assert(NULL != event);
		
	return event->op_func(event->param);
}

/******************************************************************************/
/*------------------------- EventCompareOccurence function -------------------*/				

int EventCompareOccurence(event_t *event1, event_t *event2)
{
	double subtraction_res_between_events_next_occurences = 0;
	
	assert(NULL != event1);
	assert(NULL != event2);
	
	subtraction_res_between_events_next_occurences = 
			difftime(event1->next_occurence, event2->next_occurence);
			
	if (0 > subtraction_res_between_events_next_occurences)
	{
		return EVENT1_WILL_BE_BEFORE_EVENT2;
	}
	else if (0 < subtraction_res_between_events_next_occurences)
	{
		return EVENT2_WILL_BE_BEFORE_EVENT1;
	}
	
	return EVENT1_AND_EVENT2_WILL_HAPPEN_IN_THE_SAME_TIME;
}

/******************************************************************************/
/*------------------------- EventIsSameUID function --------------------------*/				
			
int EventIsSameUID(event_t *event, ilrd_uid_t uid)
{
	assert(NULL != event);
	
	return UIDIsSame(event->op_uid, uid);
}

/******************************************************************************/
/*------------------------- EventGetUID function -----------------------------*/

ilrd_uid_t EventGetUID(event_t *event)
{
	assert(NULL != event);
	
	return event->op_uid;
}

/******************************************************************************/
/*------------------------- End Of File -------- -----------------------------*/				
