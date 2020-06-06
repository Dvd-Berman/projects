/********************************
* Author: David Berman			*
* Last Update: 28/12/2019		*
* Algorithm: Heapify            *
* header file                   *
* ******************************/

#ifndef __HEAPIFY_H__
#define __HEAPIFY_H__

/******************************************************************************/

void HeapifyUp(void *arr, size_t arr_size, size_t element_size,
		int (*cmp_func)(const void *data1, const void *data2, void *param), 
		void *param, size_t element_to_heapify);
		
/******************************************************************************/

void HeapifyDown(void *arr, size_t arr_size, size_t element_size,                 
		int (*cmp_func)(const void *data1, const void *data2, void *param),     
		void *param, size_t element_to_heapify);
		
/******************************************************************************/

/* should be moved to another file */
void HeapSort(void *arr, size_t arr_size, size_t element_size,                 
		int (*cmp_func)(const void *data1, const void *data2, void *param),     
		void *param);
		
/******************************************************************************/

#endif /* __HEAPIFY_H__ */          
