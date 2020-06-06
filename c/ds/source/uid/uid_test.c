/****************************************
* Author: David Berman					*
* Last Update: 18/11/2019				*
* Data structure: Uid                   *
* Tester File                           *
* ***************************************/

#include <unistd.h> /* for getpid */
#include <time.h> /* for time func */
#include <stdio.h>
#include <assert.h>

#include "uid.h"

/* error counter */
static size_t g_error_counter = 0;
static void CheckResult(int test, const char *func_name);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	ilrd_uid_t test1 = {0};
	ilrd_uid_t test2 = {0};
	ilrd_uid_t bad_id = {0};
	
	test1 = UIDCreate();
	test2 = UIDCreate();
	CheckResult(!UIDIsSame(test1,test2), "TestIsSame");
	
	bad_id = UIDGetBad(); 
	CheckResult(!UIDIsBad(test1), "TestIsBad");
	CheckResult(UIDIsBad(bad_id), "TestIsBad");
	 	
	printf("\nThere were %lu errors\n", g_error_counter);
	printf("Thank you Kewan for testing my code :)\n");
	printf("Hala bil Hamis!!\n");
	
	return 0;
}

/******************************************************************************/
/*------------------------- CheckResult function -----------------------------*/

static void CheckResult(int test, const char *func_name)
{
	assert(NULL != func_name);
	
	if (!test)
	{
		printf("%s: \t", func_name);
		printf("OOPS\n");
		++g_error_counter;
	}	
}









