/****************************************
* Author: David Berman			        *
* Last Update: 3/12/2019		        *
* Data structure: memory mapping    	*
* Code file.                            *
* **************************************/
#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include <stdlib.h>

#include "pq.h"
#define WORD_SIZE sizeof(size_t)

int first_bss_variable = 0;
int second_bss_variable;
int first_data_variable = 5;
int second_data_variable = 5;
static int first_static_global_variable = 5;
static int second_static_global_variable = 5;

/******************************************************************************/
/*------------------------- Struct definition---------------------------------*/

typedef struct address
{
	char *address_name_type;
	void *address;
}address_t;

/******************************************************************************/

typedef struct wrapper_for_cmp_func
	{
		int(*cmp_func)(void *, void *, void *);
		void *param;
	}cmp_wrapper;
	
/******************************************************************************/

struct pq
{
	sortlist_t *sortlist;
	cmp_wrapper *wrapper;	
};

/******************************************************************************/
/*------------------------- Auxilary function --------------------------------*/

static void AddressSegments(void);
static int AddressCmp(void *data1, void *data2, void *param);
static int PrintAddressList(void *data, void *param);
static void PrintDistance(const sortlist_t *list);
static void PrintList(const sortlist_t *list);
static void PrintStaticFunctionAddress1(void);
static void PrintStaticFunctionAddress2(void);
void PrintExternFunctionAddress1(void);
void PrintExternFunctionAddress2(void);


/******************************************************************************/
/*------------------------- main function ------------------------------------*/

int main (void)
{
	
	printf("------------------------------------Text Segment-----------------------------------------\n\n"); 	
	/* Static functions */
	PrintStaticFunctionAddress1();
	PrintStaticFunctionAddress2();
	
	/* Extern Functions */
	PrintExternFunctionAddress1();
	PrintExternFunctionAddress2();
	
	AddressSegments();
	
	return 0;
}

/******************************************************************************/
/*------------------------- AddressSegments function -------------------------*/

static void AddressSegments(void)
{
	int first_local_variable = 5;
	int second_local_variable = 5;
	const int first_const_variable = 5;
	const int second_const_variable = 5;
	int *first_heap_variable = NULL;
	int *second_heap_variable = NULL;
	pq_t *address_pq = NULL;
	void *for_dlopen = NULL;
	void *for_sym = NULL;
	address_t struct_first_local_variable;
	address_t struct_second_local_variable;
	address_t struct_first_const_variable;
	address_t struct_second_const_variable;
	address_t struct_first_heap_variable;
	address_t struct_second_heap_variable;
	address_t struct_first_bss_variable;
	address_t struct_second_bss_variable;
	address_t struct_first_data_variable;
	address_t struct_second_data_variable;
	address_t struct_first_static_global_variable;
	address_t struct_second_static_global_variable;
	address_t struct_shared_library;
	address_t struct_printf;
	
	
	address_pq = PQCreate(AddressCmp, NULL);
	
	printf("------------------------------------Stack Heap and Data Segments-----------------------------------------\n\n"); 	
	
/*------------------------- Stack variables ----------------------------------*/	
	
	struct_first_local_variable.address = (void *)&first_local_variable;
	struct_first_local_variable.address_name_type = "first_local_variable:";
	PQEnqueue(address_pq, &struct_first_local_variable);
	
	struct_second_local_variable.address = (void *)&second_local_variable;
	struct_second_local_variable.address_name_type = "second_local_variable:";
	PQEnqueue(address_pq, &struct_second_local_variable);
	
	struct_first_const_variable.address = (void *)&first_const_variable;
	struct_first_const_variable.address_name_type = "first_const_variable:";
	PQEnqueue(address_pq, &struct_first_const_variable);
	
	
	struct_second_const_variable.address = (void *)&second_const_variable;
	struct_second_const_variable.address_name_type = "second_const_variable:";
	PQEnqueue(address_pq, &struct_second_const_variable);

/*------------------------- Heap variables -----------------------------------*/	

	first_heap_variable = malloc(1024);
	second_heap_variable = malloc(1024 * 1024);
	
	struct_first_heap_variable.address = (void *)first_heap_variable;
	struct_first_heap_variable.address_name_type = "first_heap_variable:";
	PQEnqueue(address_pq, &struct_first_heap_variable);
	
	struct_second_heap_variable.address = (void *)second_heap_variable;
	struct_second_heap_variable.address_name_type = "second_heap_variable:";
	PQEnqueue(address_pq, &struct_second_heap_variable);

/*------------------------- Data variables -----------------------------------*/			
	
	struct_first_bss_variable.address = (void *)&first_bss_variable;
	struct_first_bss_variable.address_name_type = "first_bss_variable:";
	PQEnqueue(address_pq, &struct_first_bss_variable);
	
	struct_second_bss_variable.address = (void *)&second_bss_variable;
	struct_second_bss_variable.address_name_type = "second_bss_variable:";
	PQEnqueue(address_pq, &struct_second_bss_variable);
	
	struct_first_data_variable.address = (void *)&first_data_variable;
	struct_first_data_variable.address_name_type = "first_data_variable:";
	PQEnqueue(address_pq, &struct_first_data_variable);
	
	struct_second_data_variable.address = (void *)&second_data_variable;
	struct_second_data_variable.address_name_type = "second_data_variable:";
	PQEnqueue(address_pq, &struct_second_data_variable);
	
	struct_first_static_global_variable.address = (void *)&first_static_global_variable;
	struct_first_static_global_variable.address_name_type = "first_static_global_variable:";
	PQEnqueue(address_pq, &struct_first_static_global_variable);
	
	struct_second_static_global_variable.address = (void *)&second_static_global_variable;
	struct_second_static_global_variable.address_name_type = "second_static_global_variable:";
	PQEnqueue(address_pq, &struct_second_static_global_variable);

/*------------------------- Shared Library -----------------------------------*/
	
	for_dlopen = dlopen("libc.so.6", RTLD_LAZY);
	struct_shared_library.address = for_dlopen;
	struct_shared_library.address_name_type = "Shared Library:";
	PQEnqueue(address_pq, &struct_shared_library);
	
	for_sym = dlsym(for_dlopen, "printf");
	struct_printf.address = for_sym;
	struct_printf.address_name_type = "printf:";
	PQEnqueue(address_pq, &struct_printf);
				
/*------------------------- Printing -----------------------------------------*/									
	PrintList(address_pq->sortlist);
	PrintDistance(address_pq->sortlist);
	
/*------------------------- Freeing allocations ------------------------------*/			
	PQDestroy(address_pq);
	free(first_heap_variable);
	free(second_heap_variable);
}

/******************************************************************************/
/*------------------------- PrintStaticAddress1 function ---------------------*/

static void PrintStaticFunctionAddress1(void)
{
	printf("first static function: %p\n", (void *)((size_t)&PrintStaticFunctionAddress1));
}

/******************************************************************************/
/*------------------------- PrintStaticAddress2 function ---------------------*/

static void PrintStaticFunctionAddress2(void)
{
	printf("second static function: %p\n", (void *)((size_t)PrintStaticFunctionAddress2));
}

/******************************************************************************/
/*------------------------- PrintExternFunctionAddress1 function -------------*/

void PrintExternFunctionAddress1(void)
{
	printf("first extern function: %p\n", (void *)((size_t)PrintExternFunctionAddress1));
}

/******************************************************************************/
/*------------------------- PrintExternFunctionAddress2 function -------------*/

void PrintExternFunctionAddress2(void)
{
	printf("second extern function: %p\n\n", (void *)((size_t)PrintExternFunctionAddress2));
}

/******************************************************************************/
/*------------------------- AddressCmp function ------------------------------*/

static int AddressCmp(void *data1, void *data2, void *param)
{
	address_t *first_address_struct = (address_t *)data1;
	address_t *second_address_struct = (address_t *)data2;
	long first_address = 0;
	long second_address = 0;
	
	(void)param;
	
	assert(NULL != data1);
	assert(NULL != data2);
	
	first_address = (size_t)(first_address_struct->address);
	second_address = (size_t)(second_address_struct->address);
	
	return (first_address - second_address); 
}

/******************************************************************************/
/*------------------------- PrintIntList function-----------------------------*/

static int PrintAddressList(void *data, void *param)
{
	address_t *address_struct = (address_t *)data;
	
	(void)param;
	
	printf("\n%s\n", ((address_struct->address_name_type)));
	printf("The address is:\t%p\n", ((void *)(size_t)(address_struct->address)));
	printf("address remainder to word_size:\t %lu\n\n", ((size_t)(address_struct->address) % WORD_SIZE));
	
	return 0; 
}


/******************************************************************************/
/*------------------------- PrintList function--------------------------------*/

static void PrintList(const sortlist_t *list)
{
	sortlist_iter_t start_of_list = {NULL};
	sortlist_iter_t end_of_list = {NULL};
	int (*print_func)(void *, void *) = &PrintAddressList;
	
	assert(NULL != list);
	
	start_of_list = SortedListBegin(list);
	end_of_list = SortedListEnd(list);
	
	SortedListForEach(start_of_list, end_of_list, print_func, NULL);
}

/******************************************************************************/
/*------------------------- PrintDistance function----------------------------*/

static void PrintDistance(const sortlist_t *list)
{
	sortlist_iter_t start = {NULL};
	sortlist_iter_t end = {NULL};
	
	assert(NULL != list);
	
	start = SortedListNext(SortedListBegin(list));
	end = SortedListEnd(list);
	
	printf("ADDRESS BY DISTANCE:\n\n");
	
	while (!SortedListIsSameIter(start, end))
	{
		printf("Distance between addresses is:\n%d\n", 
			AddressCmp(SortedListGetData(start), SortedListGetData(SortedListPrev(start)), NULL));
		
		start = SortedListNext(start);
	}	
}

/******************************************************************************/
/*------------------------- End Of File---------------------------------------*/



