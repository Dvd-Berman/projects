/****************************									   
* Author: David Berman		*									   
* Last Update: 05/01/2020	*									   
* Project: DHCP				*									   
****************************/		

#include <stdlib.h>		/* malloc */
#include <assert.h>		/* assert */
#include <math.h>		/* pow */
#include <limits.h>

#include "ip.h"
#include "trie.h"
#include "dhcp.h"

#define NUM_OF_BITS_IN_SUBNET (CHAR_BIT * BYTES_IN_IP)


/******************************************************************************/
/******************************************************************************/
/*----------------------Struct Definitions------------------------------------*/

struct dhcp
{
	trie_t *address_trie;
	unsigned int subnet;
	unsigned int mask;
	size_t num_of_free_addr;
};

typedef struct path_wrap
{
	ip_t ip_num;
	size_t num_of_bits;
}path_wrap_t;

/******************************************************************************/
/******************************************************************************/
/*----------------------Declaration auxiliary functions-----------------------*/


/* allocates the 3 reserved address, returns status */
static int AllocReservedAddressIMP(dhcp_t *dhcp);


/* returns direction to go in trie */
static e_direction TrieNavigationIMP(trie_node_t *node,
					 void *param, size_t level);
					 
/* calculates restricted "broadcast" address */
static unsigned int CalculateBroadCastIMP(dhcp_t *dhcp);
/* caculates subnet */
static unsigned int CalculateSubnetIMP(const unsigned char subnet[BYTES_IN_IP],
				 size_t bits_in_subnet);
/* checks if ip belongs to subnet */
static int IsValidIP(dhcp_t *dhcp, ip_t ip);

/* checks if ip is reserved (one of the 3 restricted addresses */
static int IsReservedIP(dhcp_t *dhcp, ip_t ip);

/******************************************************************************/					 
/******************************************************************************/
/*-------------------------------API functions--------------------------------*/

dhcp_t *DHCPCreate(const unsigned char subnet[BYTES_IN_IP], size_t mask)
{
	dhcp_t *dhcp = NULL;
	
	assert(0 != mask);
	
	dhcp = (dhcp_t *)malloc(sizeof(dhcp_t));
	
	if (NULL != dhcp)
	{
		dhcp->mask = mask;
		dhcp->num_of_free_addr = pow(2, NUM_OF_BITS_IN_SUBNET - mask);
		dhcp->address_trie = TrieCreate();
		dhcp->subnet = CalculateSubnetIMP(subnet, mask); 
		
		if (ALLOC_SUCCESS != AllocReservedAddressIMP(dhcp))
		{
			DHCPDestroy(dhcp);
		}
	}
	
	return dhcp;
}

/******************************************************************************/					 
/******************************************************************************/
/*-------------------------------DHCPAllocIP function-------------------------*/

e_alloc_status DHCPAllocIP(dhcp_t *dhcp, const unsigned char request[BYTES_IN_IP], 
			unsigned char result[BYTES_IN_IP])
{
	ip_t request_ip = 0;
	ip_t found_path = dhcp->subnet;
	e_status trie_status = 0;
	size_t height  = 0;
	
	
	assert(NULL != dhcp);
	assert(NULL != dhcp->address_trie);
	assert(NULL != request);
	assert(NULL != result);
	
	if (TrieIsFull(dhcp->address_trie))
	{
		return ALLOC_ERROR_FULL_SUBNET;
	}
	
	height  = NUM_OF_BITS_IN_SUBNET - dhcp->mask;
	request_ip = IPGetNumFromAddress(request);
	if (0  ==  request_ip)
	{
		trie_status = TrieInsert(dhcp->address_trie, height, &found_path);
		if (FAIL == trie_status)
		{
			return 	ALLOC_ERROR_INTERNAL;
		}
		
		IPGetAddressFromNum(found_path, result);
		
		return ALLOC_SUCCESS;
	}
	
	else
	{
		path_wrap_t param = {0};
		param.num_of_bits = height;
		param.ip_num = request_ip;
		
		trie_status = TrieInsertPath(dhcp->address_trie, &TrieNavigationIMP, &param);
		if (FAIL == trie_status)
		{
			trie_status = TrieInsert(dhcp->address_trie, height, &found_path);
			if (FAIL == trie_status)
			{
				return 	ALLOC_ERROR_INTERNAL;
			}
		
			IPGetAddressFromNum(found_path, result);
			
			return 	ALLOC_SUCCESS_ANOTHER_IP;
		}
		
		IPGetAddressFromNum(request_ip, result);
		
		return ALLOC_SUCCESS; 
	}
}

/******************************************************************************/					 
/******************************************************************************/
/*-------------------------------DHCPFreeIP function--------------------------*/

e_free_status DHCPFreeIP(dhcp_t *dhcp, const unsigned char addr[BYTES_IN_IP])
{
	ip_t ip_to_free = 0;
	path_wrap_t param = {0};
	int status = 0;
	
	assert(NULL != dhcp);
	assert(NULL != dhcp->address_trie);
	assert(NULL != addr);
	
	ip_to_free = IPGetNumFromAddress(addr);
	if (!IsValidIP(dhcp, ip_to_free))
	{
		return FREE_ERROR_INVALID;
	}
	 
	if (IsReservedIP(dhcp, ip_to_free))
	{
		return FREE_ERROR_RESERVED_IP;
	}
	
	param.num_of_bits = NUM_OF_BITS_IN_SUBNET - dhcp->mask;
	param.ip_num = ip_to_free;
	
	status = TrieFreePath(dhcp->address_trie, &TrieNavigationIMP, &param);
	if (FAIL == status)
	{
		return FREE_ERROR_ALREADY_FREE;
	}
	
	return FREE_SUCCESS;  
}

/******************************************************************************/					 
/******************************************************************************/
/*-------------------------------DHCPDestroy function-------------------------*/

void DHCPDestroy(dhcp_t *dhcp)
{
	assert(NULL != dhcp);
	
	TrieDestroy(dhcp->address_trie);
	dhcp->address_trie = NULL;
	
	free(dhcp);
	dhcp = NULL;
}

/******************************************************************************/					 
/******************************************************************************/
/*-------------------------------DHCPCountFree function-----------------------*/

size_t DHCPCountFree(const dhcp_t *dhcp)
{
	assert(NULL != dhcp);
	
	return TrieCountFree(dhcp->address_trie, NUM_OF_BITS_IN_SUBNET - dhcp->mask);
}

/******************************************************************************/

/*-------------------------------Auxilary functions --------------------------*/					 

static int IsValidIP(dhcp_t *dhcp, ip_t ip)
{
	unsigned int address_space = 0;
	
	assert(NULL != dhcp);
	
	address_space = NUM_OF_BITS_IN_SUBNET - dhcp->mask;

	return (ip >> address_space) == (dhcp->subnet >> address_space); 
}

/******************************************************************************/			

static int IsReservedIP(dhcp_t *dhcp, ip_t ip)
{
	ip_t broadcast = 0;
	ip_t dhcp_server = 0;
	ip_t subnet = 0;
	
	assert(NULL != dhcp);
	
	broadcast = CalculateBroadCastIMP(dhcp);
	dhcp_server = broadcast - 1;
	subnet = dhcp->subnet;
	
	return (ip == broadcast || ip == subnet || ip == dhcp_server);
}

/*-------------------------------AllocReservedAddressIMP----------------------*/

static int AllocReservedAddressIMP(dhcp_t *dhcp)
{
	ip_t broadcast = 0;
	ip_t dhcp_server = 0;
	path_wrap_t subnet_wrap = {0};
	path_wrap_t broadcast_wrap = {0};
	path_wrap_t dhcp_server_wrap = {0};
	unsigned int num_of_bits_in_address = 0;
	
	assert(NULL != dhcp);
	
	num_of_bits_in_address = NUM_OF_BITS_IN_SUBNET - dhcp->mask;
	
	subnet_wrap.ip_num = dhcp->subnet;
	subnet_wrap.num_of_bits = num_of_bits_in_address;
	if (ALLOC_SUCCESS != TrieInsertPath(dhcp->address_trie, &TrieNavigationIMP, &subnet_wrap))
	{
		return ALLOC_ERROR_INTERNAL;
	}
	
	broadcast = CalculateBroadCastIMP(dhcp);
	broadcast_wrap.ip_num = broadcast;
	broadcast_wrap.num_of_bits = num_of_bits_in_address;
	if (ALLOC_SUCCESS != TrieInsertPath(dhcp->address_trie, &TrieNavigationIMP, &broadcast_wrap))
	{
		return ALLOC_ERROR_INTERNAL;
	}
	
	dhcp_server = broadcast - 1;
	dhcp_server_wrap.ip_num = dhcp_server;
	dhcp_server_wrap.num_of_bits = num_of_bits_in_address;
	if (ALLOC_SUCCESS != TrieInsertPath(dhcp->address_trie, &TrieNavigationIMP, &dhcp_server_wrap))
	{
		return ALLOC_ERROR_INTERNAL;
	}
	
	return ALLOC_SUCCESS;
}

/*----------------------------------------------------------------------------*/

static unsigned int CalculateBroadCastIMP(dhcp_t *dhcp)
{
	unsigned int mask = 1;
	unsigned int counter = 0;
	unsigned int broadcast = 0;
	unsigned int num_of_bits_in_address = 0;
	
	assert(NULL != dhcp);
	
	num_of_bits_in_address = NUM_OF_BITS_IN_SUBNET - dhcp->mask;
	broadcast = dhcp->subnet;
/*	broadcast <<= num_of_bits_in_address;*/
	
	for (; counter < num_of_bits_in_address; ++counter)
	{
		broadcast |= mask;
		mask <<= 1;
	}
	
	return broadcast;
}

/*----------------------------------------------------------------------------*/

static unsigned int CalculateSubnetIMP(const unsigned char subnet[BYTES_IN_IP],
				 size_t bits_in_subnet)
{
	ip_t subnet_ip = 0;
	
	assert(NULL != subnet);
	
	subnet_ip = IPGetNumFromAddress(subnet);
	subnet_ip >>= (NUM_OF_BITS_IN_SUBNET - bits_in_subnet);
	subnet_ip <<= (NUM_OF_BITS_IN_SUBNET - bits_in_subnet);
	
	return subnet_ip;
}


/*----------------------------------------------------------------------------*/

static e_direction TrieNavigationIMP(trie_node_t *node,
					 void *param, size_t level)
{	
	unsigned int mask = 1;
	path_wrap_t *path = param;

	assert(NULL != node);
	assert(NULL != param);
	
	if (level == path->num_of_bits)
	{
		return NONE;
	}
	
	mask <<= path->num_of_bits - level - 1;
	if (mask == (mask & path->ip_num))
	{
		return RIGHT;
	}
	
	return LEFT;
}

