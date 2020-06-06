/************************************
* Author: David Berman				*
* Last Update: 11/11/2019			*
* Data structure: Circular Buffer.  *
* Code File.                        * 
* **********************************/

#include <stdio.h> /* for size_t */
#include <assert.h> /* for assert */  
#include <stdlib.h> /* for malloc */
#include <string.h> /* for memcpy */

#include "c_buff.h"

#define OFFSET(struct, element) (size_t)(&(((struct *)0)->element)) 

/* Auxilary functions */
static void CBuffMovePosition(size_t *position, size_t steps); 
static void CBuffUpdateSize(size_t *size, size_t update);
static void CBuffWriteAndUpdate(c_buff_t *buffer, const void *src, 
		size_t bytes_to_copy); 
static void CBuffReadAndUpdate(c_buff_t *buffer, void *dest, 
		size_t bytes_to_copy);

/******************************************************************************/
/*------------------------- circ_buffer definition ---------------------------*/

struct circ_buffer 
{
	size_t read_position;
	size_t write_position;
	size_t bytes_capacity;
	size_t bytes_size;
	raw_byte start[1];
};

/******************************************************************************/
/*------------------------- CBuffReadAndUpdate--------------------------------*/

static void CBuffReadAndUpdate(c_buff_t *buffer, void *dest, 
		size_t bytes_to_copy)
{
	assert(NULL != buffer);
	assert(NULL != dest);
	
	memcpy(dest, buffer->start + buffer->read_position, bytes_to_copy);
	CBuffMovePosition(&(buffer->read_position), bytes_to_copy);
	CBuffUpdateSize(&(buffer->bytes_size), -bytes_to_copy);
}

/******************************************************************************/
/*------------------------- CBuffWriteAndUpdate-------------------------------*/

static void CBuffWriteAndUpdate(c_buff_t *buffer, const void *src, 
		size_t bytes_to_copy)
{
	assert(NULL != buffer);
	assert(NULL != src);
	
	memcpy(buffer->start + buffer->write_position, src, bytes_to_copy);
	CBuffMovePosition(&(buffer->write_position), bytes_to_copy);
	CBuffUpdateSize(&(buffer->bytes_size), bytes_to_copy);
}

/******************************************************************************/
/*------------------------- CBuffMovePosition---------------------------------*/

static void CBuffMovePosition(size_t *position, size_t steps) 
{
	assert(NULL != position);
	
	*position += steps;
}

/******************************************************************************/
/*------------------------- CBuffUpdateSize ----------------------------------*/

static void CBuffUpdateSize(size_t *size, size_t update) 
{
	assert(NULL != size);
	
	*size += update; 
}


/******************************************************************************/
/*------------------------- CBuffCreate function -----------------------------*/

c_buff_t *CBuffCreate(size_t bytes_capacity)
{
	c_buff_t *cbuf = NULL;
	
	assert(0 != bytes_capacity);
	
	cbuf = (c_buff_t *)malloc(OFFSET(c_buff_t, start) + bytes_capacity);
	if (NULL != cbuf)
	{
		memset(cbuf->start,0, bytes_capacity);
		cbuf->write_position = 0;
		cbuf->read_position = 0;
		cbuf->bytes_size = 0;
		cbuf->bytes_capacity = bytes_capacity;
	}	
	
	return cbuf;  
}

/******************************************************************************/
/*------------------------- CBuffDestroy function ----------------------------*/

void CBuffDestroy(c_buff_t *buffer)
{	
	assert(NULL != buffer);
	
	free(buffer);
}

/******************************************************************************/
/*------------------------- CBuffWrite function ------------------------------*/

size_t CBuffWrite(c_buff_t *buffer, const void *src, 
		size_t src_size)
{
	size_t bytes_written = 0;
	size_t free_space = CBuffFreeSpace(buffer);
	size_t free_space_till_end = buffer->bytes_capacity - buffer->write_position;
	
	assert(NULL != src);
	assert(NULL != buffer); 
	
	if (0 != free_space)
	{
		if (src_size > free_space)
		{
			src_size = free_space;
		}
		
		bytes_written = src_size;
		
		if (free_space_till_end >= src_size)
		{
			CBuffWriteAndUpdate(buffer, src, src_size); 
		}
		else
		{
			CBuffWriteAndUpdate(buffer, src, free_space_till_end); 
			buffer->write_position = 0;
			CBuffWriteAndUpdate(buffer, (raw_byte *)src + free_space_till_end,
			 		src_size - free_space_till_end); 	
		}
	}
	
	return bytes_written;
}

/******************************************************************************/
/*------------------------- CBuffRead function -------------------------------*/

size_t CBuffRead(c_buff_t *buffer, void *dest, 
		size_t dest_size)
{
	size_t bytes_read = 0;
	size_t free_space_till_end = buffer->bytes_capacity - buffer->read_position;
	
	assert(NULL != dest);
	assert(NULL != buffer); 
	
	if (!CBuffIsEmpty(buffer))
	{
		if (dest_size > buffer->bytes_size)
		{
			dest_size = buffer->bytes_size;
		}
		
		bytes_read = dest_size;
		
		if (free_space_till_end >= dest_size)
		{
			CBuffReadAndUpdate(buffer, dest, dest_size);
		}
		
		else
		{
			CBuffReadAndUpdate(buffer, dest, free_space_till_end);
			buffer->read_position = 0;
			CBuffReadAndUpdate(buffer, (raw_byte *)dest + free_space_till_end,
					 dest_size - free_space_till_end);
		}
	}
	
	return bytes_read;
}

/******************************************************************************/
/*------------------------- CBuffIsEmpty function ----------------------------*/

int CBuffIsEmpty(const c_buff_t *buffer)
{
	assert(NULL != buffer);
	
	return (buffer->bytes_size == 0);
}

/******************************************************************************/
/*------------------------- CBuffFreeSpace function --------------------------*/

size_t CBuffFreeSpace(const c_buff_t *buffer)
{
	assert(NULL != buffer);
	
	return buffer->bytes_capacity - buffer->bytes_size;	
}

/******************************************************************************/
/*------------------------- CBuffCapacity function ---------------------------*/

size_t CBuffCapacity(const c_buff_t *buffer)
{
	assert(NULL != buffer);
	
	return buffer->bytes_capacity;
}

/******************************************************************************/
/*------------------------- End Of File  -------------------------------------*/
