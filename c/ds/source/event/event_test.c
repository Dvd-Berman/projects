/****************************************
* Author: David Berman					*
* Last Update: 21/11/2019				*
* Data structure: event                 *
* Tester File                           *
* ***************************************/

#include <stdio.h>
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <time.h>

#include "uid.h"
#include "event.h"
#include "tests.h"

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static int PrintMessage(void *param);
static void TestEventUpdateNextOccurence(void);
static void TestEventGetDelay(void);
static void TestEventIsSameUID(event_t *event);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	event_t *event;
	
	
	OpenTestFile("EVENT.txt");
	
	/* Testing Create And Destroy */
	event = EventCreate(&PrintMessage, 10, NULL);
	CheckResult(NULL != event, "TestEventCreate");
	
	
	/* Testing update of next occurence */
	TestEventUpdateNextOccurence();
	
	/* Testing getdelay */
	TestEventGetDelay();
	
	/* Testing Event Execute */
	EventExecute(event);
	
	/* Test is same */
	TestEventIsSameUID(event);
	
	EventDestroy(event);
	
	TestResult();		
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestEventUpdateOccurance--------------------------*/


static void TestEventUpdateNextOccurence(void)
{
	time_t first_occurence;
	
	event_t *event = EventCreate(&PrintMessage, 10, NULL);
	first_occurence = event->next_occurence;
	
	EventUpdateNextOccurence(event);
	CheckResult(first_occurence != event->next_occurence,
				 "TestEventUpdateNextOccurence");
	
	EventDestroy(event);
}

/******************************************************************************/
/*------------------------- TestEventGetDelay---------------------------------*/

static void TestEventGetDelay(void)
{
	size_t delay = 0;
	
	event_t *event = EventCreate(&PrintMessage, 10, NULL);

	delay = EventGetDelay(event);
	CheckResult(delay == event->interval,
				 "TestEventGetDelay");
	
	EventDestroy(event);
}

/******************************************************************************/
/*------------------------- TestEventIsSameUID--------------------------------*/

static void TestEventIsSameUID(event_t *event)
{
	ilrd_uid_t uid = UIDCreate();
	
	assert(NULL != event);
	
	CheckResult(!UIDIsSame(uid,event->op_uid), "TestEventIsSameUID");
}

/******************************************************************************/
/*------------------------- PrintMessage function ----------------------------*/

static int PrintMessage(void *param)
{
	(void)param;
	printf("Thank you Sasha for testing my code ;)\n");
	
	return 0;
}	
