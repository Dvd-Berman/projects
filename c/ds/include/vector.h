/********************************
* Author: OL78					*
* Last Update: 04/11/2019		*
* Data structure: Vector		*
* ******************************/

#ifndef __VECTOR_H__
#define __VECTOR_H__

typedef struct vector_struct_t vector_t;

/* Initializes empty vector and returns a pointer to it.					*
 * If fails - returns NULL.													*/
vector_t *VectorCreate(size_t capacity, size_t element_size);

/* Adds new element to the end of the vector.								*
 * If succeeds - returns 0. If fails - returns 1.							*/
int VectorPushBack(vector_t *vector, const void *element);

/* Returns a pointer to the element.										*
 * If element doesn't exists - returns NULL.								*
 * The pointer can be used to set the element value. 						*
 * CAUTION! Exceeding element_size will lead to undefined behavior.			*/
void *VectorGetElementByAddress(const vector_t *vector, size_t index);

/* Deletes the last element. 												*/
void VectorPopBack(vector_t *vector);

/* Returns number of elements in the vector.								*/
size_t VectorSize(const vector_t *vector);

/* Returns the current vector capacity.										*/
size_t VectorCapacity(const vector_t *vector);

/* Change the vector capacity. If the vector size is greater than			*
 * new_capacity, changes the capacity to the vector size.					*
 * If succeeds - returns 0. If fails - returns 1.							*/
int VectorReserve(vector_t *vector, size_t new_capacity);

/* Destroys the vector and deallocates its memory.							*/
void VectorDestroy(vector_t *vector);

#endif /* __VECTOR_H__ */
