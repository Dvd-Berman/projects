/**************************
* Author: David Berman    *
* Last Update : 20/12/19  *
* Project : Calculator    *
* Tester File             *
**************************/

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include "calc.h"
#include "stack.h"
#include "tests.h"

/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

static void TestCalcPhaseOne(void);
static void TestCalcPhaseTwo(void);

/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
		
	OpenTestFile("CALCULATOR");
	
	/* Testing phase 1 */
	TestCalcPhaseOne();
	
	/* Testing phase 2 with power */
	TestCalcPhaseTwo();
	
	TestResult();
	
	printf("Thank you Oved For testing my code!!\n");
	printf("Good Luck against Raananana!\n");

	return 0;	
}

/******************************************************************************/
/*------------------------- TestCalcPhaseOne function ------------------------*/

static void TestCalcPhaseOne(void)
{
	double result = 0;
	char *invalid_expression1 = "777 * 2 )(";
	char *invalid_expression2 = "777 * * ) ";
	char *unknown_chars = "342 * & 2";
	calculator_status_e status = 0;
	char *valid_expression1 = "5 + 7 * 2 - 1 + (888)";
	double expression1_res = 5 + 7 * 2 - 1 + (888);
	char *valid_expression2 = "2 * 5  * 3 - 1";
	double expression2_res = 2 * 5  * 3 - 1;
	char *valid_expression3 = "1 * 2 / 2 * 4  - 1";
	double expression3_res = 1 * 2 / 2 * 4  - 1;
	char *valid_expression4 = "45 + 36 / 12.5 * 40";
	double expression4_res = 45 + 36 / 12.5 * 40;
	char *valid_expression5 = "(200000) - (((300) * 5) / 4)";
	double expression5_res = (200000) - (((300) * 5) / 4); 
	
	status = Calculator(invalid_expression1, &result);
	CheckResult(status == PARENTHESIS_ERROR, "TestCalcPhaseOne1");
	
	status = Calculator(invalid_expression2, &result);
	CheckResult(status == INVALID_ERROR, "TestCalcPhaseOne2");
	
	status = Calculator(unknown_chars, &result);
	CheckResult(status == UNKNOWN_ERROR, "TestCalcPhaseOne3");
	
	status = Calculator(valid_expression1, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseOne4");
	CheckResult(expression1_res == result, "TestResult1");
	
	
	status = Calculator(valid_expression2, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseOne5");
	CheckResult(expression2_res == result, "TestResult2");
	
	status = Calculator(valid_expression3, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseOne6");
	CheckResult(expression3_res == result, "TestResult3");
	
	status = Calculator(valid_expression4, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseOne7");
	CheckResult(expression4_res == result, "TestResult4");
	
	status = Calculator(valid_expression5, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseOne8");
	CheckResult(expression5_res == result, "TestResult5");
}

/******************************************************************************/
/*------------------------- TestCalcPhaseTwo function ------------------------*/

static void TestCalcPhaseTwo(void)
{
	double result = 0;
	char *invalid_expression1 = " (((())) 5 * 7 ^ 8";
	char *invalid_expression2 = "432 * ^ ^ ^  ) ";
	char *unknown_chars = "342 * & $$ 2";
	calculator_status_e status = 0;
	char *valid_expression1 = "2 ^ 2";
	double expression1_res = pow(2,2);
	char *valid_expression2 = "2 ^ 2 ^ 3";
	double expression2_res = pow(2, pow(2, 3));
	char *valid_expression3 = "1 ^ 2 * 5 + 3 * (2 ^ 4 ^ 2)";
	double expression3_res = pow(1, 2) * 5 + 3 * (pow(2, pow(4, 2)));
	char *valid_expression4 = "2 ^ 3 - 5 + (7 - 200 * 50)";
	double expression4_res = pow(2,3) - 5 + (7 - 200 * 50);
	char *valid_expression5 = "10000 / 20 / 5 - 2 ^ 3 ^ 7 ^ 2 ^ 1 * (((300) * 5) / 4)";
	double expression5_res = 10000 / 20 / 5 - pow(2,pow(3,pow(7,pow(2,1)))) * (((300) * 5) / 4); 
	
	status = Calculator(invalid_expression1, &result);
	CheckResult(status == PARENTHESIS_ERROR, "TestCalcPhaseTwo1");
	
	status = Calculator(invalid_expression2, &result);
	CheckResult(status == INVALID_ERROR, "TestCalcPhaseTwo2");
	
	status = Calculator(unknown_chars, &result);
	CheckResult(status == UNKNOWN_ERROR, "TestCalcPhaseTwo3");
	
	status = Calculator(valid_expression1, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseTwo");
	CheckResult(expression1_res == result, "TestResult6");
	
	status = Calculator(valid_expression2, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseTwo");
	CheckResult(expression2_res == result, "TestResult7");
	
	status = Calculator(valid_expression3, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseTwo");
	CheckResult(expression3_res == result, "TestResult8");
	
	status = Calculator(valid_expression4, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseTwo");
	CheckResult(expression4_res == result, "TestResult9");
	
	status = Calculator(valid_expression5, &result);
	CheckResult(status == SUCCESS, "TestCalcPhaseTwo");
	CheckResult(expression5_res == result, "TestResult10");
	
}	
		
/******************************************************************************/
/*------------------------- End Of File --------------------------------------*/
