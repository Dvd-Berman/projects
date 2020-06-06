/**************************
* Author: David Berman    *
* Last Update : 16/12/19  *
* Project : Calculator    *
* Header File             *
**************************/

#ifndef __CALC_H__
#define __CALC_H__

/******************************************************************************/

typedef enum calculator_status
{
	SUCCESS,
	UNKNOWN_ERROR, 
	PARENTHESIS_ERROR,
	INVALID_ERROR,
	INTERNAL_ERROR
}calculator_status_e;


/******************************************************************************/

/* calculates a string of an arithmetic expression according to algebric rules and stores result in result
Includes power, devision, multiplication, plus and minus operations,
returns SUCCESS status when calculation succeeded or other status if failed according to 
reason of failure. Make sure that the string is of a valid expression to avoid failure */ 
calculator_status_e Calculator(const char *arithmetic_expression, double *result);

#endif /* __CALC_H__ */         
