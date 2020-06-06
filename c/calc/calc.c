/**************************
* Author: David Berman    *
* Last Update : 20/12/19  *
* Project : Calculator    *
* Code File               *
**************************/

#include <stdlib.h> /* for strtod */ 
#include <math.h> /* for pow function */
#include <limits.h> /* for UCHAR_MAX */
#include <assert.h> /* for assert */

#include "calc.h"
#include "stack.h"

#define MAX_NUM_OF_PARENTHESIS 100
#define AUXILARY_STACK_CAPACITY 500
#define NUM_OF_ARITHMETIC_OP 5
#define NUM_OF_OP 6
#define NUM_OF_STATUS 5

typedef enum {NOT_INITIATED, INITIATED}lut_init_flag_e;
typedef enum {PUSH_TO_STACK, ACTIVATE_SHUNTING_YARD, POWER_HANDLER}operations_e; 

/******************************************************************************/
/*------------------------- enum Definition ----------------------------------*/

typedef enum
{
	PLUS_TYPE,
	MINUS_TYPE,
	MULTIPLY_TYPE,
	DIVIDE_TYPE,
	POWER_TYPE,
	OPEN_PAR_TYPE,
	CLOSE_PAR_TYPE,
	NUM_TYPE,
	UNKNOWN_ERROR_TYPE,
	EOF_TYPE,
	NUM_OF_INPUT_TYPES
}input_type_e;

/******************************************************************************/

typedef enum state
{
	WAIT_FOR_NUM_STATE,
	WAIT_FOR_OP_STATE,
	SUCCESS_STATE,
	UNKNOWN_ERROR_STATE,
	INVALID_ERROR_STATE,
	NUM_OF_STATES	
}state_e;   

/******************************************************************************/
/*------------------------- union Definition ---------------------------------*/

typedef union input_value 
{
	double num;
	char operator;
}input_value_u;

/******************************************************************************/
/*------------------------- struct Definition --------------------------------*/

typedef struct token
{
	input_value_u value;
	input_type_e type;
}token_t;

/******************************************************************************/

typedef void (*op_func)(token_t , stack_t *, stack_t *);
typedef token_t (*calc_func)(token_t *, token_t *);

/******************************************************************************/

typedef struct transition
{
	state_e next_state;
	op_func op_func;
}transition_t;


/******************************************************************************/
/*------------------------- Auxilary functions -------------------------------*/

/* Initiation of transition lut */
static void InitTransitionLutIMP(transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES]);
static void InitTransitionLutRowIMP(
	transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES], 
	state_e state,
	transition_t *transition1,
	transition_t *transition2, transition_t *transition3, 
	transition_t *transition4, transition_t *transition5,
	transition_t *transition6, transition_t *transition7, 
	transition_t *transition8, transition_t *transition9,
	transition_t *transition10);
	
/* given a row in lut 
 this function fills the cells in this row with the same transition struct */
static void FillLutRowWithSameTransitionIMP
(transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES],
		 state_e state, transition_t *transition);
	
/******************************************************************************/

/* Checks if input equation is valid according to paranthesis rules,
 if it is returns SUCCESS, 
 if not returns either INTERNAL_ERROR or PARENTHESIS_ERROR */
static calculator_status_e CheckValidPranthesisIMP(const char *arithmetic_expression);

/******************************************************************************/

/* init of auxilary stacks and update status according to the init */
static void InitAuxilaryStacksIMP(stack_t **output_stack, 
				stack_t **operator_stack, calculator_status_e *status);
				
/******************************************************************************/

static token_t GetNextTokenIMP(const char **arithmetic_expression);

/******************************************************************************/
static void InitLutLexerIMP(input_type_e lut[], int length);

/* removes the spaces before next token */
static const char *RemoveSpacesBeforeInputIMP(const char *arithmetic_expression_runner);

/******************************************************************************/
static void InitPrecedenceLutIMP(operations_e precedence_lut[NUM_OF_ARITHMETIC_OP][NUM_OF_OP]);

/******************************************************************************/
static void InitErrorHandler(calculator_status_e error_handler[]);

/******************************************************************************/
					 
static void DoNothingIMP(token_t token, stack_t *output, stack_t *operators);
static void PushOpenParToOperatorsStackIMP(token_t token, stack_t *output, stack_t *operators);
static void PushNumToOutputStackIMP(token_t token, stack_t *output, stack_t *operators);
static void OperationsHandlerIMP
			(token_t token, stack_t *output, stack_t *operators);
static void ClosedParanthesesHandlerIMP(token_t token, stack_t *output, stack_t *operators);
static void EOFHandlerIMP(token_t token, stack_t *output, stack_t *operators);
static void ActivateShuntingYardIMP(token_t token, stack_t *output, stack_t *operators);
static void MoveOperatorToOutputIMP(stack_t *output, stack_t *operators);
static void	CalculateOutputVariablesIMP(stack_t *output);
static void PowerHandlerIMP(token_t token, stack_t *output, stack_t *operators);
static void InitCalculationLutIMP(calc_func calculation_lut[]);
static token_t CalculatePlusIMP(token_t *top_num, token_t *bottom_num);
static token_t CalculateMinusIMP(token_t *top_num, token_t *bottom_num);
static token_t CalculateMultiplyIMP(token_t *top_num, token_t *bottom_num);
static token_t CalculateDivideIMP(token_t *top_num, token_t *bottom_num);
static token_t CalculatePowerIMP(token_t *top_num, token_t *bottom_num);

/******************************************************************************/
/*------------------------- Calculator function ------------------------------*/

calculator_status_e Calculator(const char *arithmetic_expression, double *result)
{
	static transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES] = {0};
	static calculator_status_e error_handler[NUM_OF_STATUS] = {0};
	token_t token = {0};
	token_t *res_token = NULL;
	transition_t transition = {0};
	calculator_status_e status = UNKNOWN_ERROR;
	state_e state = WAIT_FOR_NUM_STATE;	
	stack_t *output_stack = NULL;
	stack_t *operator_stack = NULL;
	
	assert(NULL != arithmetic_expression);
	
	InitTransitionLutIMP(transition_lut);
	InitErrorHandler(error_handler);
	status = CheckValidPranthesisIMP(arithmetic_expression); 	
	InitAuxilaryStacksIMP(&output_stack, &operator_stack, &status);
	
	if (SUCCESS == status)
	{
		while (EOF_TYPE != token.type)
		{
			token = GetNextTokenIMP(&arithmetic_expression);
			transition = transition_lut[state][token.type];
			state = transition.next_state;
			transition.op_func(token, output_stack, operator_stack);
		}
		
		status = error_handler[state];
		if (!StackIsEmpty(output_stack))
		{
			res_token = StackPeek(output_stack);
			*result = res_token->value.num;
		}
		
		StackDestroy(output_stack);
		StackDestroy(operator_stack); 
	}
	
	return status;	 
}

/******************************************************************************/
/*------------------------- InitErrorHandler ---------------------------------*/

static void InitErrorHandler(calculator_status_e error_handler[])
{
	static lut_init_flag_e init_flag = NOT_INITIATED;
	
	assert (NULL != error_handler);
	
	if (NOT_INITIATED == init_flag)
	{
		error_handler[WAIT_FOR_NUM_STATE] = INVALID_ERROR;
		error_handler[WAIT_FOR_OP_STATE] = INVALID_ERROR;
		error_handler[SUCCESS_STATE] = SUCCESS;
		error_handler[UNKNOWN_ERROR_STATE] = UNKNOWN_ERROR;
		error_handler[INVALID_ERROR_STATE] = INVALID_ERROR;	
		
		init_flag = INITIATED;
	}
}

/******************************************************************************/
/*------------------------- InitAuxilaryStacksIMP ----------------------------*/

static void InitAuxilaryStacksIMP(stack_t **output_stack, 
				stack_t **operator_stack, calculator_status_e *status)
{
	if (SUCCESS == *status)
	{
		*output_stack = StackInit(AUXILARY_STACK_CAPACITY, sizeof(token_t));
		*operator_stack = StackInit(AUXILARY_STACK_CAPACITY, sizeof(token_t));
	
		if (NULL == *output_stack || NULL == *operator_stack)
		{
			*status = INTERNAL_ERROR;
		}
	
		else
		{
			*status = SUCCESS;
		}
	}
}

/******************************************************************************/
/*------------------------- InitTransitionLut --------------------------------*/

static void InitTransitionLutIMP
		(transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES])
{	
	static lut_init_flag_e init_flag = NOT_INITIATED;
	
	if (NOT_INITIATED == init_flag)
	{
		transition_t unknown_error = {UNKNOWN_ERROR_STATE, &DoNothingIMP};
		transition_t invalid_error = {INVALID_ERROR_STATE, &DoNothingIMP};
		transition_t wait_for_num_with_push_op = {WAIT_FOR_NUM_STATE, &PushOpenParToOperatorsStackIMP};
		transition_t wait_for_num_with_op_check = 
					{WAIT_FOR_NUM_STATE, &OperationsHandlerIMP};
		transition_t wait_for_op_with_push_op = {WAIT_FOR_OP_STATE, &PushNumToOutputStackIMP};
		transition_t wait_for_op_with_close_par_func = 
					{WAIT_FOR_OP_STATE, &ClosedParanthesesHandlerIMP};
		transition_t success = {SUCCESS_STATE, &EOFHandlerIMP};
		
		
		assert(NULL != transition_lut);
		
		/* init row WAIT_FOR_NUM_STATE */
		InitTransitionLutRowIMP(transition_lut, WAIT_FOR_NUM_STATE,
		&invalid_error, &invalid_error, &invalid_error, &invalid_error, &invalid_error, 
		&wait_for_num_with_push_op, &invalid_error, 
		&wait_for_op_with_push_op, &unknown_error, &invalid_error); 
		
		/* init row WAIT_FOR_OP_STATE */
		InitTransitionLutRowIMP(transition_lut, WAIT_FOR_OP_STATE,
		&wait_for_num_with_op_check, &wait_for_num_with_op_check, 
		&wait_for_num_with_op_check, &wait_for_num_with_op_check,
		&wait_for_num_with_op_check,
		&invalid_error,  &wait_for_op_with_close_par_func, 
		&invalid_error, &unknown_error, &success);
		 
		 /* init row SUCCESS_STATE */
		 FillLutRowWithSameTransitionIMP(transition_lut, SUCCESS_STATE, &invalid_error);
		 
		 /* init row UNKNOWN_ERROR_STATE */
		 FillLutRowWithSameTransitionIMP(transition_lut, UNKNOWN_ERROR_STATE,
		 					 &unknown_error);
		 
		 /* init row INVALID_ERROR_STATE */
		 FillLutRowWithSameTransitionIMP(transition_lut, INVALID_ERROR_STATE, &invalid_error);
		 
		 init_flag = INITIATED;
	}
}

/******************************************************************************/
/*------------------------- InitTransitionLutRow -----------------------------*/

static void InitTransitionLutRowIMP(
	transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES], 
	state_e state,
	transition_t *transition1,
	transition_t *transition2, transition_t *transition3, 
	transition_t *transition4, transition_t *transition5,
	transition_t *transition6, transition_t *transition7, 
	transition_t *transition8, transition_t *transition9,
	transition_t *transition10)
{
	assert(NULL != transition1);
	assert(NULL != transition2);
	assert(NULL != transition3);
	assert(NULL != transition4);
	assert(NULL != transition5);
	assert(NULL != transition6);
	assert(NULL != transition7);
	assert(NULL != transition8);
	assert(NULL != transition9);
	assert(NULL != transition10);
	
	transition_lut[state][PLUS_TYPE] = *transition1;
	transition_lut[state][MINUS_TYPE] = *transition2;
	transition_lut[state][MULTIPLY_TYPE] = *transition3;
	transition_lut[state][DIVIDE_TYPE] = *transition4;
	transition_lut[state][POWER_TYPE] = *transition5;
	transition_lut[state][OPEN_PAR_TYPE] = *transition6;
	transition_lut[state][CLOSE_PAR_TYPE] = *transition7;
	transition_lut[state][NUM_TYPE] = *transition8;
	transition_lut[state][UNKNOWN_ERROR_TYPE] = *transition9;
	transition_lut[state][EOF_TYPE] = *transition10;	
}
/******************************************************************************/
/*------------------------- FillLutRowWithSameTransition ---------------------*/

static void FillLutRowWithSameTransitionIMP
(transition_t transition_lut[NUM_OF_STATES][NUM_OF_INPUT_TYPES],
		 state_e state, transition_t *transition)
{
	int column = 0;
	
	assert(NULL != transition_lut);
	
	for (; column < NUM_OF_INPUT_TYPES; ++column)
	{
		transition_lut[state][column] = *transition;
	}  
}

/******************************************************************************/
/*------------------------- CheckValidPranthesis function --------------------*/

static calculator_status_e CheckValidPranthesisIMP(const char *arithmetic_expression)
{
	const char *arithmetic_expression_runner = arithmetic_expression;
	stack_t *stack_par = StackInit(MAX_NUM_OF_PARENTHESIS, sizeof(char));
	calculator_status_e status  = SUCCESS;
	
	assert(NULL != arithmetic_expression);
	
	if (NULL == stack_par)
	{
		status = INTERNAL_ERROR;
	}
	else
	{
		while ('\0' != *arithmetic_expression_runner)
		{
			if ('(' == *arithmetic_expression_runner)
			{
				StackPush(stack_par, arithmetic_expression_runner);
			}
			else if (!StackIsEmpty(stack_par) && ')' == *arithmetic_expression_runner)
			{
				StackPop(stack_par);
			}
			
			++arithmetic_expression_runner;
		}
		
		if (!StackIsEmpty(stack_par))
		{
			status = PARENTHESIS_ERROR;
		}
		else
		{
			status = SUCCESS;
		}
	}
	
	StackDestroy(stack_par);
	
	return status;	
}

/******************************************************************************/
/*------------------------- GetNextToken function ----------------------------*/

static token_t GetNextTokenIMP(const char **arithmetic_expression_runner)
{
	token_t token = {0};
	input_type_e type = UNKNOWN_ERROR_TYPE;
	static input_type_e lut_lexer[UCHAR_MAX] = {0};
	
	assert(NULL != arithmetic_expression_runner);
	assert(NULL != *arithmetic_expression_runner);
	
	InitLutLexerIMP(lut_lexer, UCHAR_MAX);
	*arithmetic_expression_runner = 
		RemoveSpacesBeforeInputIMP(*arithmetic_expression_runner);
	
	type = lut_lexer[(int)**arithmetic_expression_runner];
	token.type = type;
	switch(type)
	{
		case NUM_TYPE:
			token.value.num = strtod(*arithmetic_expression_runner, (char **)arithmetic_expression_runner);
			break;
		
		default:
			token.value.operator = **arithmetic_expression_runner;
			++*arithmetic_expression_runner;
			break;
	}
	
	return token;					
}

/******************************************************************************/
/*------------------------- RemoveSpacesBeforeInput function -----------------*/

static const char *RemoveSpacesBeforeInputIMP(const char *arithmetic_expression_runner)
{
	assert(NULL != arithmetic_expression_runner);
	
	for (; *arithmetic_expression_runner == ' ' ||
			  *arithmetic_expression_runner == '\t';
				 				 ++arithmetic_expression_runner)
	{
		;
	}
	
	return arithmetic_expression_runner;
}

/******************************************************************************/
/*------------------------- InitLutLexer function ----------------------------*/

static void InitLutLexerIMP(input_type_e lut[], int length)
{
	int index = 0;
	static lut_init_flag_e init_flag = NOT_INITIATED;
	
	assert(NULL != lut);
	if (NOT_INITIATED == init_flag)
	{
		for (; index < length; ++index)
		{
			lut[index] = UNKNOWN_ERROR_TYPE;
		}
		
		for (index = '0'; index <= '9'; ++index)
		{
			lut[index] = NUM_TYPE;
		} 
		
		lut['+'] = PLUS_TYPE;
		lut['-'] = MINUS_TYPE;
		lut['*'] = MULTIPLY_TYPE;
		lut['/'] = DIVIDE_TYPE;
		lut['^'] = POWER_TYPE;
		lut['('] = OPEN_PAR_TYPE;
		lut[')'] = CLOSE_PAR_TYPE;
		lut['\0'] = EOF_TYPE;
		
		init_flag = INITIATED;
	}
}

/******************************************************************************/
/*------------------------- OP functions -------------------------------------*/

static void DoNothingIMP(token_t token, stack_t *output, stack_t *operators)
{
	assert(NULL != output);
	assert (NULL != operators);
	
	(void)token;
	(void)output;
	(void)operators;
}

/******************************************************************************/

static void PushNumToOutputStackIMP(token_t token, stack_t *output, stack_t *operators)
{
	assert(NULL != output);
	assert (NULL != operators);
	
	(void)operators;
	StackPush(output, &token);
}

/******************************************************************************/

static void PushOpenParToOperatorsStackIMP(token_t token, stack_t *output, stack_t *operators)
{
	assert(NULL != output);
	assert (NULL != operators);
	
	(void)output;
	StackPush(operators, &token);
}

/******************************************************************************/

static void OperationsHandlerIMP
			(token_t token, stack_t *output, stack_t *operators)
{
	static operations_e precedence_lut[NUM_OF_ARITHMETIC_OP][NUM_OF_OP] = {0};
	token_t *stack_token = NULL; 
	operations_e operation = 0;
	
	assert(NULL != output);
	assert (NULL != operators);
	
	if (StackIsEmpty(operators))
	{
		StackPush(operators, &token);
	}
	else
	{
		stack_token = StackPeek(operators);
		InitPrecedenceLutIMP(precedence_lut);
		operation = precedence_lut[token.type][stack_token->type];
	
		if (PUSH_TO_STACK == operation)
		{
			StackPush(operators, &token);
		}
		else if (ACTIVATE_SHUNTING_YARD == operation)
		{
			ActivateShuntingYardIMP(token, output, operators);
		}
		else
		{
			PowerHandlerIMP(token, output, operators);
		}
	}
}

/******************************************************************************/

static void PowerHandlerIMP(token_t token, stack_t *output, stack_t *operators)
{
	token_t token_in_stack = {0};
	
	assert(NULL != output);
	assert(NULL != operators);
	
	token_in_stack = *(token_t *)StackPeek(operators);
	
	while (!StackIsEmpty(operators) && token_in_stack.type == POWER_TYPE)
	{
		MoveOperatorToOutputIMP(output, operators);
		CalculateOutputVariablesIMP(output);
		if (!StackIsEmpty(operators))
		{
			token_in_stack = *(token_t *)StackPeek(operators);
		}
	}

	StackPush(operators, &token);
}

/******************************************************************************/

static void ActivateShuntingYardIMP(token_t token, stack_t *output, stack_t *operators)
{
	assert (NULL != output);
	assert (NULL != operators);
	
	MoveOperatorToOutputIMP(output, operators);
	CalculateOutputVariablesIMP(output);
	StackPush(operators, &token);
}

/******************************************************************************/

static void MoveOperatorToOutputIMP(stack_t *output, stack_t *operators)
{
	token_t *popped_operator = NULL;
	
	assert (NULL != output);
	assert (NULL != operators);
	
	popped_operator = StackPeek(operators);
	StackPop(operators);
	StackPush(output, popped_operator);
}

/******************************************************************************/

static void	CalculateOutputVariablesIMP(stack_t *output)
{
	static calc_func calculation_lut[NUM_OF_ARITHMETIC_OP] = {0};
	calc_func calc_func = NULL;
	token_t *operator = NULL;
	token_t *top_num = NULL;
	token_t *bottom_num = NULL;
	token_t calculation_result = {0};
	
	assert (NULL != output);
	
	operator = StackPeek(output);
	StackPop(output);
	
	top_num = StackPeek(output);
	StackPop(output);
	
	bottom_num = StackPeek(output);
	StackPop(output);
	
	InitCalculationLutIMP(calculation_lut);
	calc_func = calculation_lut[operator->type];
	calculation_result = calc_func(top_num, bottom_num);
	StackPush(output, &calculation_result);
}

/******************************************************************************/
/*------------------------- InitCalculationLutIMP function -------------------*/

static void InitCalculationLutIMP(calc_func calculation_lut[])
{
	static lut_init_flag_e init_flag = NOT_INITIATED;
	
	assert(NULL !=  calculation_lut);
	
	if (NOT_INITIATED == init_flag)
	{
		calculation_lut[PLUS_TYPE] = &CalculatePlusIMP;
		calculation_lut[MINUS_TYPE] = &CalculateMinusIMP;
		calculation_lut[MULTIPLY_TYPE] = &CalculateMultiplyIMP;
		calculation_lut[DIVIDE_TYPE] = &CalculateDivideIMP;
		calculation_lut[POWER_TYPE] = &CalculatePowerIMP;
		
		init_flag = INITIATED;
	}
}

/******************************************************************************/

static token_t CalculatePlusIMP(token_t *top_num, token_t *bottom_num)
{
	token_t res = {0};
	
	res.value.num = bottom_num->value.num + top_num->value.num; 
	res.type = NUM_TYPE;
	
	return res;
}

/******************************************************************************/

static token_t CalculateMinusIMP(token_t *top_num, token_t *bottom_num)
{
	token_t res = {0};
	
	res.value.num = bottom_num->value.num - top_num->value.num; 
	res.type = NUM_TYPE;
	
	return res;
}

/******************************************************************************/

static token_t CalculateMultiplyIMP(token_t *top_num, token_t *bottom_num)
{
	token_t res = {0};
	
	res.value.num = bottom_num->value.num * top_num->value.num; 
	res.type = NUM_TYPE;
	
	return res;
}

/******************************************************************************/

static token_t CalculateDivideIMP(token_t *top_num, token_t *bottom_num)
{
	token_t res = {0};
	
	res.value.num = bottom_num->value.num / top_num->value.num; 
	res.type = NUM_TYPE;
	
	return res;
}

/******************************************************************************/

static token_t CalculatePowerIMP(token_t *top_num, token_t *bottom_num)
{
	token_t res = {0};
	
	res.value.num = pow(bottom_num->value.num, top_num->value.num); 
	res.type = NUM_TYPE;
	
	return res;
}

/******************************************************************************/
/*------------------------- InitPrecedenceLutIMP function --------------------*/

static void InitPrecedenceLutIMP(operations_e precedence_lut[NUM_OF_ARITHMETIC_OP][NUM_OF_OP])
{
	static lut_init_flag_e init_flag = NOT_INITIATED;
	
	assert(NULL != precedence_lut);
	
	if (NOT_INITIATED == init_flag)
	{
		precedence_lut[PLUS_TYPE][PLUS_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[PLUS_TYPE][MINUS_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[PLUS_TYPE][MULTIPLY_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[PLUS_TYPE][DIVIDE_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[PLUS_TYPE][POWER_TYPE] = POWER_HANDLER;
		precedence_lut[PLUS_TYPE][OPEN_PAR_TYPE] = PUSH_TO_STACK;
		
		precedence_lut[MINUS_TYPE][PLUS_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MINUS_TYPE][MINUS_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MINUS_TYPE][MULTIPLY_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MINUS_TYPE][DIVIDE_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MINUS_TYPE][POWER_TYPE] = POWER_HANDLER;
		precedence_lut[MINUS_TYPE][OPEN_PAR_TYPE] = PUSH_TO_STACK;
		
		precedence_lut[MULTIPLY_TYPE][PLUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[MULTIPLY_TYPE][MINUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[MULTIPLY_TYPE][MULTIPLY_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MULTIPLY_TYPE][DIVIDE_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[MULTIPLY_TYPE][POWER_TYPE] = POWER_HANDLER;
		precedence_lut[MULTIPLY_TYPE][OPEN_PAR_TYPE] = PUSH_TO_STACK;
		
		precedence_lut[DIVIDE_TYPE][PLUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[DIVIDE_TYPE][MINUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[DIVIDE_TYPE][MULTIPLY_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[DIVIDE_TYPE][DIVIDE_TYPE] = ACTIVATE_SHUNTING_YARD;
		precedence_lut[DIVIDE_TYPE][POWER_TYPE] = POWER_HANDLER;
		precedence_lut[DIVIDE_TYPE][OPEN_PAR_TYPE] = PUSH_TO_STACK;
		
		precedence_lut[POWER_TYPE][PLUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[POWER_TYPE][MINUS_TYPE] = PUSH_TO_STACK;
		precedence_lut[POWER_TYPE][MULTIPLY_TYPE] = PUSH_TO_STACK;
		precedence_lut[POWER_TYPE][DIVIDE_TYPE] = PUSH_TO_STACK;
		precedence_lut[POWER_TYPE][POWER_TYPE] = PUSH_TO_STACK;
		precedence_lut[POWER_TYPE][OPEN_PAR_TYPE] = PUSH_TO_STACK;
		
		init_flag = INITIATED;
	}
}

/******************************************************************************/

static void ClosedParanthesesHandlerIMP
			(token_t token, stack_t *output, stack_t *operators)
{
	assert(NULL != output);
	assert (NULL != operators);
	
	token = *(token_t *)StackPeek(operators);
	
	while (token.type != OPEN_PAR_TYPE)
	{
		MoveOperatorToOutputIMP(output, operators);
		CalculateOutputVariablesIMP(output);
		token = *(token_t *)StackPeek(operators);
	}

	StackPop(operators);
}

/******************************************************************************/

static void EOFHandlerIMP(token_t token, stack_t *output, stack_t *operators)
{
	assert(NULL != output);
	assert (NULL != operators);
	(void)token;
	
	while (!StackIsEmpty(operators))
	{
		MoveOperatorToOutputIMP(output, operators);
		CalculateOutputVariablesIMP(output);
	}
}

