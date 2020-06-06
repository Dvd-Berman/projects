/********************************
* Author: OL78					*
* Last Update: 25/12/2019		*
* Data structure: Hash table	*
********************************/ 
		
#ifndef __HASH_H__ 
#define __HASH_H__ 

typedef struct hash_table hash_table_t;

/*----------------------------------------------------------------------------*/

hash_table_t *HashCreate(size_t (*hash_func)(void *key, void *hash_param), 
		void *hash_param, 
		int (*is_match)(void *data, void *key), size_t table_size);

/*----------------------------------------------------------------------------*/

void HashDestroy(hash_table_t *hash_table);

/*----------------------------------------------------------------------------*/

int HashInsert(hash_table_t *hash_table, void *data);

/*----------------------------------------------------------------------------*/

void HashRemove(hash_table_t *hash_table, void *key);

/*----------------------------------------------------------------------------*/

void *HashFind(const hash_table_t *hash_table, void *key);

/*----------------------------------------------------------------------------*/

int HashIsEmpty(const hash_table_t *hash_table);

/*----------------------------------------------------------------------------*/

double HashLoadFactor(const hash_table_t *hash_table);

/*----------------------------------------------------------------------------*/

double HashSD(const hash_table_t *hash_table);

/*----------------------------------------------------------------------------*/

size_t HashSize(const hash_table_t *hash_table);

/*----------------------------------------------------------------------------*/

int HashForEach(hash_table_t *hash_table, 
		int (*op_func)(void *data, void *param), void *param);

/*----------------------------------------------------------------------------*/

#endif /* __HASH_H__ */
