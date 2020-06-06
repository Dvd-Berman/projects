/********************************
* Author: David Berman			*
* Last Update: 25/12/2019		*
* Data structure: Hash table    *
* Code File                     *
********************************/ 

#include <stdlib.h> /* for malloc */
#include <assert.h> /* for assert */
#include <math.h> /* for HashSD calculation */

#include "hash.h"
#include "d_list.h"

#define OFFSET(struct, element) ((size_t)(&(((struct *)0)->element)))

typedef enum{SUCCESS, FAILURE}e_status;
typedef enum{FALSE_CASE, TRUE_CASE}e_case;

/******************************************************************************/
/******************************************************************************/
/*--------------------------struct definitions--------------------------------*/

struct hash_table
{
	size_t (*hash_func)(void *, void *);
	void *hash_param;
	int (*is_match)(void *, void *);
	size_t table_size;
	dlist_t *start[1];
};

/******************************************************************************/

typedef struct is_match_wrapper
{
	int (*is_match)(void *, void *);
	void *key;
}is_match_wrapper_t;

/******************************************************************************/
/******************************************************************************/
/*--------------------------Auxilary functions--------------------------------*/

/* creates d_list for all cells in table */
static int InitHashTableIMP(hash_table_t *hash_table); 

/* returns the index to the hash_table */
static size_t GetHashIndexIMP(const hash_table_t *hash_table, void *key);

/* wrapper to is_match */
static int IsMatchWrapperIMP(void *data, void *param);

/* find iter through DlistFind */
static iter_dlist_t FindIterIMP(const hash_table_t *hash_table, void *key, dlist_t *list);

/* returns average between num_of_elements / table size */
static double CalculateAverageForHashSDIMP(const hash_table_t *hash_table);

/* Returns specific list variation */ 
static double HashCellVarianceIMP(const hash_table_t *hash_table,
				 double list_size, double average);
				 
/* Inserts data to front of list, 
	if fails returns if succeeds removes previous data */				 
static void CachingIMP(dlist_t *list, iter_dlist_t found_iter);

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashCreate function-------------------------------*/

hash_table_t *HashCreate(size_t (*hash_func)(void *key, void *hash_param), 
		void *hash_param, 
		int (*is_match)(void *data, void *key), size_t table_size)
{
	hash_table_t *hash_table = NULL;
	
	assert(NULL != hash_func);
	assert(NULL != is_match);
	assert(0 < table_size);
	
	hash_table = malloc(OFFSET(hash_table_t, start) + (table_size * sizeof(dlist_t *)));
	if (NULL != hash_table)
	{
		int init_status = 0;
		
		hash_table->hash_func = hash_func;
		hash_table->hash_param = hash_param;
		hash_table->is_match = is_match;
		hash_table->table_size = table_size;
		
		init_status = InitHashTableIMP(hash_table);
		if (FAILURE == init_status)
		{
			HashDestroy(hash_table);
			return NULL;
		}
	}
	
	return hash_table;
}

/*--------------------------Auxilary function---------------------------------*/

static int InitHashTableIMP(hash_table_t *hash_table)
{
	size_t counter = 0;
	int status = SUCCESS;
	
	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		hash_table->start[counter] = DListCreate();
		if (NULL == hash_table->start[counter])
		{
			status = FAILURE;
			break;
		}
	}
	
	return status;  
}

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashDestroy function------------------------------*/

void HashDestroy(hash_table_t *hash_table)
{	
	size_t counter = 0;
	
	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		if (NULL == hash_table->start[counter])
		{
			break;
		}
		
		DListDestroy(hash_table->start[counter]);
	}
	
	free(hash_table);
	hash_table = NULL;
}

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashInsert function-------------------------------*/

int HashInsert(hash_table_t *hash_table, void *data)
{
	size_t hash_table_index = 0;
	dlist_t *list = NULL;
	
	assert(NULL != hash_table);
	assert(NULL == HashFind(hash_table, data));
	
	hash_table_index = GetHashIndexIMP(hash_table, data);
	list = hash_table->start[hash_table_index];
	
	return DListPushBack(list, data);
}

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashRemove function-------------------------------*/

void HashRemove(hash_table_t *hash_table, void *key)
{
	iter_dlist_t found_iter = {0};
	size_t hash_table_index = 0;
	dlist_t *list = NULL;
	
	assert(NULL != hash_table);
	
	hash_table_index = GetHashIndexIMP(hash_table, key);
	list = hash_table->start[hash_table_index];
	found_iter = FindIterIMP(hash_table, key, list);
	
	if (!(DListIsSameIter(found_iter, DListEnd(list))))
	{
		DListRemove(found_iter);
	}
	
	return;	
}

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashFind function---------------------------------*/

void *HashFind(const hash_table_t *hash_table, void *key)
{
	iter_dlist_t found_iter = {0};
	size_t hash_table_index = 0;
	dlist_t *list = NULL;
	void *data = NULL;
	
	assert(NULL != hash_table);
	
	hash_table_index = GetHashIndexIMP(hash_table, key);
	list = hash_table->start[hash_table_index];
	found_iter = FindIterIMP(hash_table, key, list);
	
	if (DListIsSameIter(found_iter, DListEnd(list)))
	{
		return NULL;
	}
	
	data = DListGetData(found_iter);
	CachingIMP(list, found_iter);
	
	return data;
}

/*--------------------------CachingIMP function-------------------------------*/

static void CachingIMP(dlist_t *list, iter_dlist_t found_iter)
{
	assert(NULL != list);

	if (FAILURE == DListPushFront(list, DListGetData(found_iter)))
	{
		return;
	}
	
	DListRemove(found_iter);
}


/*--------------------------FindIterIMP function------------------------------*/

static iter_dlist_t FindIterIMP(const hash_table_t *hash_table, void *key, dlist_t *list)
{
	is_match_wrapper_t wrapper = {0};
	
	assert(NULL != hash_table);
	
	wrapper.is_match = hash_table->is_match;
	wrapper.key = key;
	
	return DListFind(DListBegin(list), DListEnd(list), &IsMatchWrapperIMP, &wrapper); 
}  

/*--------------------------Auxilary function---------------------------------*/

static size_t GetHashIndexIMP(const hash_table_t *hash_table, void *key)
{
	size_t hash_func_res = 0;
	size_t hash_table_index = 0;
	
	assert(NULL != hash_table);
	
	hash_func_res = hash_table->hash_func(key, hash_table->hash_param);
	hash_table_index = hash_func_res % hash_table->table_size;
	
	return hash_table_index;	
}

/******************************************************************************/

static int IsMatchWrapperIMP(void *data, void *param)
{
	is_match_wrapper_t *unwrapped_param = param;
	
	return !(unwrapped_param->is_match(data, unwrapped_param->key));	
}

/******************************************************************************/
/******************************************************************************/
/*--------------------------HashIsEmpty function------------------------------*/

int HashIsEmpty(const hash_table_t *hash_table)
{
	size_t counter = 0;
	dlist_t *list = NULL;	
	e_case is_empty = TRUE_CASE;
	
	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		list = hash_table->start[counter];
		is_empty = DListIsEmpty(list);
		if (FALSE_CASE == is_empty)
		{
			break;
		}
	}
	
	return is_empty;
}

/******************************************************************************/
/*--------------------------HashLoadFactor function---------------------------*/

double HashLoadFactor(const hash_table_t *hash_table)
{
	size_t counter = 0;
	dlist_t *list = NULL;
	double occupied_cells = 0;
	int is_empty_res = 0;
	
	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		list = hash_table->start[counter];
		is_empty_res = DListIsEmpty(list);
		if (FALSE_CASE == is_empty_res)
		{
			++occupied_cells;
		}
	}
	
	return (occupied_cells / hash_table->table_size);
	
}

/******************************************************************************/
/*--------------------------HashSD function-----------------------------------*/

double HashSD(const hash_table_t *hash_table)
{
	size_t counter = 0;
	dlist_t *list = NULL;
	double list_size = 0;
	double average = 0;
	double variance = 0;
	
	assert(NULL != hash_table);
	
	average = CalculateAverageForHashSDIMP(hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		list = hash_table->start[counter];
		list_size = DListSize(list);
		variance += HashCellVarianceIMP(hash_table, list_size, average);
	}
	
	return sqrt(variance);
}

/*--------------------------Auxilary function---------------------------------*/

static double HashCellVarianceIMP(const hash_table_t *hash_table, 
			double list_size, double average)
{
	assert(NULL != hash_table);
	
	return (pow(list_size - average, 2) / hash_table->table_size); 
}

/*--------------------------Auxilary function---------------------------------*/

static double CalculateAverageForHashSDIMP(const hash_table_t *hash_table)
{
	assert(NULL != hash_table);
	
	return (double)HashSize(hash_table) / hash_table->table_size;
}
/******************************************************************************/
/*--------------------------HashSize function---------------------------------*/

size_t HashSize(const hash_table_t *hash_table)
{
	size_t counter = 0;
	dlist_t *list = NULL;
	size_t num_of_elements = 0;

	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		list = hash_table->start[counter];
		num_of_elements += DListSize(list);
	}
	
	return num_of_elements;
}

/******************************************************************************/
/*--------------------------HashForEach function------------------------------*/

int HashForEach(hash_table_t *hash_table, 
		int (*op_func)(void *data, void *param), void *param)
{
	dlist_t *list = NULL;
	size_t counter = 0;
	int op_res = FAILURE;
		
	assert(NULL != hash_table);
	
	for (; counter < hash_table->table_size; ++counter)
	{
		list = hash_table->start[counter];
		op_res = DListForEach(DListBegin(list), DListEnd(list), op_func, param);
		if (SUCCESS != op_res)
		{
			break;
		}
	}
	
	return op_res;
}
		
/******************************************************************************/
/*--------------------------End Of File---------------------------------------*/

