/****************************************
* Author: David Berman			        *
* Last Update: 23/11/2019		        *
* Data structure: Scheduler           	*
* Tester file.                          *
* **************************************/

#include <stdio.h> /* for printf */
#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <time.h> /* for time variables */

#include "pq.h"
#include "uid.h"
#include "event.h"
#include "sched.h"
#include "tests.h"

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

struct scheduler
{
	pq_t *sched_pq;
	event_t *curr_event;
};

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static void TestSchedulerCreate(void);
static void TestSchedulerSize(scheduler_t *sched, size_t expected);
static void TestSchedulerIsEmpty(scheduler_t *sched, int expected);
static ilrd_uid_t TestSchedulerAdd(scheduler_t *sc, 
			int (*op_func)(void *param), size_t interval, void *param);
static void TestSchedulerRemove(scheduler_t *sched, 
					ilrd_uid_t event_uid, int expected);
static void TestSchedulerClear(scheduler_t *sched);
static int PrintMessage1(void *param);
static int PrintMessage2(void *param);
static int PrintMessage3(void *param);
static int PrintMessage4(void *param);
static int PrintMessage5(void *param);
static int PrintMessage6(void *param);
static int PrintMessage7(void *param);
static int PrintMessage8(void *param);
static int PrintMessage9(void *param);
static int StopScheduler(void *param);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	scheduler_t *sc = {0};
	ilrd_uid_t uid1 = {0};
	
	OpenTestFile("Sched.txt");
	
	/* Testing Create */
	TestSchedulerCreate();
	
	/* Testing sc size */
	sc = SchedulerCreate();
	TestSchedulerSize(sc, 0);
	
	/* Testing is empty */
	TestSchedulerIsEmpty(sc, 1);
	
	/* Testing add */
	uid1 = TestSchedulerAdd(sc, PrintMessage1, 5, NULL);
	TestSchedulerSize(sc, 1);
	
	/* Testing Remove */
	TestSchedulerRemove(sc, uid1, 0);
	TestSchedulerSize(sc, 0);
	
	/* Testing clear */
	TestSchedulerAdd(sc, PrintMessage1, 5, NULL);
	TestSchedulerAdd(sc, PrintMessage2, 5, NULL);
	TestSchedulerAdd(sc, PrintMessage3, 5, NULL);
	TestSchedulerAdd(sc, PrintMessage4, 5, NULL);
	TestSchedulerClear(sc);
	
	/* Testing Run and Stop */
	TestSchedulerAdd(sc, PrintMessage1, 1, NULL);
	TestSchedulerAdd(sc, PrintMessage2, 3, NULL);
	TestSchedulerAdd(sc, PrintMessage3, 4, NULL);
	TestSchedulerAdd(sc, PrintMessage4, 6, NULL);
	TestSchedulerAdd(sc, PrintMessage5, 20, NULL);
	TestSchedulerAdd(sc, PrintMessage6, 24, NULL);
	TestSchedulerAdd(sc, PrintMessage7, 26, NULL);
	TestSchedulerAdd(sc, PrintMessage8, 28, NULL);
	TestSchedulerAdd(sc, PrintMessage9, 53, NULL);
	TestSchedulerAdd(sc, StopScheduler, 55, sc);
	SchedulerRun(sc);
	TestSchedulerSize(sc, 0);
	
	
	SchedulerDestroy(sc);
	
	TestResult();		
	
	return 0;
}

/******************************************************************************/
/*------------------------- TestSchedulerCreate function ---------------------*/

static void TestSchedulerCreate(void)
{
	scheduler_t *sched = SchedulerCreate();
	CheckResult(NULL != sched, "TestSchedulerCreate");
	
	SchedulerDestroy(sched);
}

/******************************************************************************/
/*------------------------- TestSchedulerCreate function ---------------------*/

static void TestSchedulerSize(scheduler_t *sched, size_t expected)
{
	assert(NULL != sched);
	
	CheckResult(SchedulerSize(sched) == expected, "TestSchedulerSize");	
}

/******************************************************************************/
/*------------------------- TestSchedulerIsEmpty function --------------------*/

static void TestSchedulerIsEmpty(scheduler_t *sched, int expected)
{
	assert(NULL != sched);
	
	CheckResult(SchedulerIsEmpty(sched) == expected, "TestSchedulerIsEmpty");	
}

/******************************************************************************/
/*------------------------- TestSchedulerAdd function ------------------------*/

static ilrd_uid_t TestSchedulerAdd(scheduler_t *sc, 
			int (*op_func)(void *param), size_t interval, void *param)
{
	ilrd_uid_t event_uid = {0};
	
	assert(NULL != sc);
	assert(NULL != op_func);
	
	event_uid = SchedulerAdd(sc, op_func, interval, param);
	CheckResult(UIDIsBad(event_uid) == 0, "TestSchedulerAdd");
	
	return event_uid;	
}

/******************************************************************************/
/*------------------------- TestSchedulerRemove function ---------------------*/

static void TestSchedulerRemove(scheduler_t *sched, 
					ilrd_uid_t event_uid, int expected)
{
	int res_remove = 0;
	
	assert(NULL != sched);
	
	res_remove = SchedulerRemove(sched, event_uid);
	CheckResult(res_remove == expected, "TestSchedulerRemove");		
}

/******************************************************************************/
/*------------------------- TestSchedulerClear function ----------------------*/ 

static void TestSchedulerClear(scheduler_t *sched)
{
	assert(NULL != sched);
	
	SchedulerClear(sched);
	
	CheckResult(1 == SchedulerIsEmpty(sched), "TestSchedulerClear");		
}

/******************************************************************************/
/*------------------------- PrintMessage1 function ----------------------------*/

static int PrintMessage1(void *param)
{
	(void)param;
	printf("Thank you Sasha for testing my code ;)\n\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage2 function ----------------------------*/

static int PrintMessage2(void *param)
{
	(void)param;
	printf("A jewish man asks to be converted by the priest \n");
	printf("The priest agrees but asks the jew to commit to" 
		" eating only chicken on friday's dinner\n\n");
		
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage3 function ----------------------------*/

static int PrintMessage3(void *param)
{
	(void)param;
	
	printf("The jew agrees to the priests terms and the priest spills 3 drops"
			" of holly water on his head and says after waiting 2 seconds\n\n");
			
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage4 function ----------------------------*/

static int PrintMessage4(void *param)
{	
	static int repeat = 3;
	
	(void)param;
	printf("You are a Christian!!\n");
	--repeat;
	return (0 < repeat);
}

/******************************************************************************/
/*------------------------- PrintMessage5 function ---------------------------*/

static int PrintMessage5(void *param)
{
	(void)param;
	printf("\nThe Priest went back to the jew's home\n");
	printf(" He noticed that the jew is having fish for dinner\n");
	printf("Angry with the jew's betrayel he paused for 3 seconds\n\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage6 function ---------------------------*/

static int PrintMessage6(void *param)
{
	(void)param;
	printf("Why the hell are you eating fish!?\n"
			"said the priest\n");
	printf("The Jew was shocked and asked for forgiveness\n");
	printf("Then the priest turned his attention towards the fish\n");
	printf("And paused for another 1 second \n\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage7 function ---------------------------*/

static int PrintMessage7(void *param)
{
	static int repeat = 2;
	
	(void)param;
	printf("You are a chicken!!\n");
	--repeat;
	return (0 < repeat);
}

/******************************************************************************/
/*------------------------- PrintMessage8 function ---------------------------*/

static int PrintMessage8(void *param)
{	
	(void)param;
	printf("\nUnfortunately he got stuck between sentences for 26 seconds\n\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- PrintMessage9 function ---------------------------*/

static int PrintMessage9(void *param)
{	
	(void)param;
	printf("\nYou are a CHICKEN!!!\n\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- StopScheduler ------------------------------------*/

static int StopScheduler(void *param)
{
	assert(NULL != param);
	
	SchedulerStop((scheduler_t *)param);
	
	return 0;
}


/******************************************************************************/
/*------------------------- End Of Tester ------------------------------------*/
