/****************************									   
* Author: OL78				*									   
* Last Update: 02/01/2020	*									   
* Project: DHCP				*									   
****************************/									   
																				
#ifndef __DHCP_H__															   
#define __DHCP_H__ 

/*----------------------------------------------------------------------------*/

#define BYTES_IN_IP 4

typedef struct dhcp dhcp_t;

typedef enum alloc_status
{
	ALLOC_SUCCESS,
	ALLOC_SUCCESS_ANOTHER_IP,
	ALLOC_ERROR_INTERNAL,
	ALLOC_ERROR_FULL_SUBNET
} e_alloc_status;

typedef enum free_status
{
	FREE_SUCCESS,
	FREE_ERROR_INVALID,
	FREE_ERROR_ALREADY_FREE,
	FREE_ERROR_RESERVED_IP
} e_free_status;

/*----------------------------------------------------------------------------*/

/* creates dhcp, returns NULL if fails  */
dhcp_t *DHCPCreate(const unsigned char subnet[BYTES_IN_IP], size_t mask);

/*----------------------------------------------------------------------------*/

/*destroys dhcp frees all memmory */
void DHCPDestroy(dhcp_t *dhcp);

/*----------------------------------------------------------------------------*/

/* allocates ip according to request, returns ip in result, 
	if request cannot be accommodated 
	allocates different ip, returns allocation status  */
e_alloc_status DHCPAllocIP(dhcp_t *dhcp, const unsigned char request[BYTES_IN_IP], 
			unsigned char result[BYTES_IN_IP]);

/*----------------------------------------------------------------------------*/

/* allocates free ip returns status */
e_free_status DHCPFreeIP(dhcp_t *dhcp, const unsigned char addr[BYTES_IN_IP]);

/*----------------------------------------------------------------------------*/

/* counts free addresses in dhcp */
size_t DHCPCountFree(const dhcp_t *dhcp);

/*----------------------------------------------------------------------------*/

#endif /* __DHCP_H__ */


