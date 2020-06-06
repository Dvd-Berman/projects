/********************************
* Author: David Berman			*
* Last Update: 06/11/2019		*
* Data structure: Vector.	    *
* Code file.                    *
* ******************************/

#include <stdio.h>
#include <stdlib.h> /* xxalloc */
#include <assert.h>
#include <string.h> /* memcpy */

#include "vector.h"

#define GROWTH_FACTOR 2
#define SHRINK_FACTOR 0.75
#define TIME_TO_SHRINK (num_of_elements <= 0.5 * vector->capacity)
#define TIME_TO_GROW (num_of_elements == vector->capacity)

static int ChangeCapacity(vector_t *vector, size_t new_capacity);

typedef unsigned char byte;
     	
/******************************************************************************/
/*------------------------- struct vector definition -------------------------*/

struct vector_struct_t 
{
	byte *start;
	byte *next_spot;
	size_t capacity;
	size_t element_size;
};

/******************************************************************************/
/*------------------------- Create Function ----------------------------------*/

vector_t *VectorCreate(size_t capacity, size_t element_size)
{
	vector_t *vector = NULL;
	
	assert(0 != capacity);
	assert(0 != element_size);
	
	vector = (vector_t *)malloc(sizeof(vector_t));
	if (NULL != vector)
	{
		vector->start = malloc(capacity * element_size);
		if (NULL != vector->start)
		{
			vector->next_spot = vector->start;
			vector->capacity = capacity;
			vector->element_size = element_size;
		}
		else
		{
			free(vector);
			vector = NULL;
		}	
	}
		
	
	return vector;  
}


/******************************************************************************/
/*------------------------- Destroy Function ---------------------------------*/

void VectorDestroy(vector_t *vector)
{
	assert(NULL != vector);
	
	free(vector->start);
	vector->start = NULL;
	vector->next_spot = NULL;
	free(vector);
}

/******************************************************************************/
/*------------------------- GetElement Function ------------------------------*/

void *VectorGetElementByAddress(const vector_t *vector, size_t index)
{
	assert(NULL != vector);
	assert (index < VectorSize(vector));
	
	return 	vector->start + (index * vector->element_size);	
}

/******************************************************************************/
/*------------------------- VectorPushBack Function --------------------------*/

int VectorPushBack(vector_t *vector, const void *element)
{
	int status = 0;
	size_t num_of_elements = 0;
	
	assert(NULL != vector);
	assert(NULL != element);
	
	num_of_elements = VectorSize(vector); 
	if (TIME_TO_GROW)
	{
		status = ChangeCapacity(vector, GROWTH_FACTOR * vector->capacity);
		
		if(1 == status)
		{
			return status; 
		}
				
	}
	memcpy(vector->next_spot , element, vector->element_size);
	vector->next_spot = vector->next_spot + vector->element_size;
		
	return status;
}

/******************************************************************************/
/*------------------------- VectorSize ---------------------------------------*/

size_t VectorSize(const vector_t *vector)
{
	size_t size_of_elements_in_bytes = 0;
	
	assert(NULL != vector);
	assert(0 < vector->element_size);
	
	size_of_elements_in_bytes = vector->next_spot - vector->start;
	
	return (size_of_elements_in_bytes / vector->element_size); 
}


/******************************************************************************/
/*------------------------- VectorPopBack Function ---------------------------*/

void VectorPopBack(vector_t *vector)
{
	size_t num_of_elements = 0;
	
	assert(NULL != vector);
	assert(0 < VectorSize(vector));
	
	num_of_elements = VectorSize(vector);
	if (TIME_TO_SHRINK)
	{
		ChangeCapacity(vector, (SHRINK_FACTOR * vector->capacity) + 1);			
	}
	vector->next_spot = vector->next_spot - vector->element_size;
}


/******************************************************************************/
/*------------------------- VectorReserve ------------------------------------*/

int VectorReserve(vector_t *vector, size_t new_capacity)
{
	size_t num_of_elements = 0;
	int status = 0;
	
	assert(NULL != vector);
	
	num_of_elements = VectorSize(vector);
	
	if (new_capacity < num_of_elements)
	{
		status = ChangeCapacity(vector, num_of_elements);
	}
	
	else if (new_capacity > num_of_elements)
	{
		status = ChangeCapacity(vector, new_capacity);
	}
	
	return status;
}

/******************************************************************************/
/*------------------------- VectorCapacity -----------------------------------*/

size_t VectorCapacity(const vector_t *vector)
{
	assert(NULL != vector);
	
	return vector->capacity;
}

/******************************************************************************/
/*------------------------- ChangeCapacity -----------------------------------*/

static int ChangeCapacity(vector_t *vector, size_t new_capacity)
{	
	size_t num_of_elements = 0;
	void *new_start = NULL;
	
	assert(NULL != vector);
	
	num_of_elements = VectorSize(vector); 
	new_start = realloc(vector->start, new_capacity * vector->element_size);
	if(NULL == new_start)
	{
		return 1;
	}
	
	vector->capacity = new_capacity;
	vector->start = new_start;
	vector->next_spot = vector->start + num_of_elements * vector->element_size;
		
	return 0;
}

/******************************************************************************/
