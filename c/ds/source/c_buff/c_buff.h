/************************************
* Author: OL78						*
* Last Update: 10/11/2019			*
* Data structure: Circular Buffer	*
* **********************************/

#ifndef __C_BUFF_H__
#define __C_BUFF_H__

typedef unsigned char raw_byte; 
typedef struct circ_buffer c_buff_t;

/* Initializes empty buffer and returns a pointer to it.			*
 * If fails - returns NULL.											*
 * CAUTION! bytes_capacity cannot be zero!							*
 * Algorithmic efficiency: O(1).									*/
c_buff_t *CBuffCreate(size_t bytes_capacity); 

/* Destroys the buffer and deallocates its memory.					*
 * Algorithmic efficiency: O(1).									*/
void CBuffDestroy(c_buff_t *buffer);

/* Gets a pointer to a src array of arr_size bytes, and writes		*
 * these bytes to the buffer back.							 		*
 * Returns number of bytes that have been written successfully.		*
 * CAUTION! If the buffer free space is less than arr_size,
 * writes till the buffer is full.									*

 * Algorithmic efficiency: O(n).									*/
size_t CBuffWrite(c_buff_t *buffer, const void *src, 
		size_t src_size);

/* Reads arr_size bytes from buffer front and writes them to dest.  *
 * Removes these bytes from the buffer.								*
 * Returns number of bytes that have been read successfully.		*
 * CAUTION! If the buffer size is less than arr_size,				*
 * reads only buffer_size bytes.									*
 * CAUTION! The dest size should be at least dest_size bytes		*
 * Algorithmic efficiency: O(n).									*/
size_t CBuffRead(c_buff_t *buffer, void *dest, 
		size_t dest_size);

/* If the buffer is empty, returns 1. Otherwise - 0.				*
 * Algorithmic efficiency: O(1).									*/
int CBuffIsEmpty(const c_buff_t *buffer);

/* Returns the buffer free space in bytes (capacity - buffer_size).	*/
 /* Algorithmic efficiency: O(1).									*/
size_t CBuffFreeSpace(const c_buff_t *buffer);

/* Returns the buffer capacity.										*
 * Algorithmic efficiency: O(1).									*/
size_t CBuffCapacity(const c_buff_t *buffer);

#endif /* __C_BUFF_H__ */
