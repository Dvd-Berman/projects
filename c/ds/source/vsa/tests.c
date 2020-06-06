
#include <stdio.h>

#include "tests.h"

static size_t g_error_counter = 0;
char *fname;

/* Write the result of the test str to the file. */
static void WriteToTestFile(const char *str, const char *result);

/*---------------------------------------------------------------------------*/

void OpenTestFile(char *file_name)
{
	FILE* fp  = fopen(file_name, "w");
	fputs("\t\t\t_____ ", fp);
	fputs(file_name, fp);
	fputs(" _____\n\n", fp);
	fname = file_name;
	fclose(fp);
}

/*---------------------------------------------------------------------------*/

void TestResult(void)
{
	if (g_error_counter)
	{
		printf("\n\t____ %lu ERRORS EXISTS _____\n\n", g_error_counter);
	}
	else
	{
		printf("\n\t____ NO ERRORS _____\n\n");
	}	
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void CheckResult(int test, const char *func_name)
{
	if (!test)
	{
		WriteToTestFile(func_name,": \t OOPS\n");
		++g_error_counter;
	}
	else
	{
		WriteToTestFile(func_name,": \t AWESOME!!!\n");
	}	
}
/*---------------------------------------------------------------------------*/

static void WriteToTestFile(const char *func_name, const char *result)
{
	FILE* fp  = fopen(fname, "r+");
	
	fseek(fp, 0, SEEK_END);
	fputs(func_name, fp);
	fseek(fp, 0, SEEK_END);
	fputs(result, fp);
	fclose(fp);
}
