/****************************									   
* Author: Ofer Kuperman		*									   
* Last Update: 01/01/2020	*									   
* Data structure: DHCP		*									   
****************************/

#include <stdio.h>
#include <math.h>


#include "dhcp.h"

void TestCreateAndDestroy();
void TestAlloc();
void TestFree();
void TestCountFree();
void Test(int boolean, int test_num);
void TestDavid(void);

int main()
{
	
	TestCreateAndDestroy();
	TestAlloc();
	TestFree();
	/*TestDavid();*/

	/*dhcp_t *dhcp = NULL;
	unsigned char subnet[BYTES_IN_IP] = {0, 0, 0, 0};
	size_t mask = 8;
	unsigned char request[] = {0, 0, 0, 1};
	unsigned char result[] = {200, 10, 0, 0};
	
	dhcp = DHCPCreate(subnet, mask);
	if (NULL != dhcp)
	{
		printf("good\n");
	}
	else
	{
		printf("bad\n");
	}
	
	printf("%d\n", DHCPAllocIP(dhcp, NULL, result));
	printf("%d\t", result[0]);
	printf("%d\t", result[1]);
	printf("%d\t", result[2]);
	printf("%d\t", result[3]);
	printf("%lu\n", DHCPCountFree(dhcp));
	printf("\n%d\n", DHCPFreeIP(dhcp, result));*/
	
	
	/*dhcp_t *dhcp = NULL;
	unsigned char subnet[BYTES_IN_IP] = {200, 10, 0, 0};
	size_t mask = 16;
	
	dhcp = DHCPCreate(subnet, mask);
	if (NULL != dhcp)
	{
		printf("good\n");
	}
	else
	{
		printf("bad\n");
	}
	
	DHCPDestroy(dhcp);*/
	
	
	return 0;

}

void TestDavid()
{
	dhcp_t *dhcp = NULL;
	unsigned char subnet[BYTES_IN_IP] = {200, 10, 5, 0};
	size_t mask = 29;
	unsigned char request[BYTES_IN_IP] = {200, 10, 30, 1};
	unsigned char result[BYTES_IN_IP] = {0, 0, 0, 0};
	int i = 0;
	
	
	dhcp = DHCPCreate(subnet, mask);
	
	printf("First CountFree : %lu \n\n", DHCPCountFree(dhcp));
	DHCPAllocIP(dhcp, request, result);
	
	for (i = 0; i < 4; ++i)
	{
		printf("%d\t", result[i]);
	}
	
	printf("First CountFree : %lu \n\n", DHCPCountFree(dhcp));
	DHCPAllocIP(dhcp, request, result);
	
	for (i = 0; i < 4; ++i)
	{
		printf("%d\t", result[i]);
	}
}

void TestCreateAndDestroy()
{
	dhcp_t *dhcp1 = NULL;
	dhcp_t *dhcp2 = NULL;
	unsigned char subnet[BYTES_IN_IP] = {200, 10, 5, 0};
	size_t mask1 = 27;
	size_t mask2 = 22;
	int test_num = 0;
	
	printf("\n\nTEST CREATE:\n\n");
	
	dhcp1 = DHCPCreate(subnet, mask1);
	Test(NULL != dhcp1, ++test_num);
	Test((pow(2, 5) - 3) == DHCPCountFree(dhcp1), ++test_num);
	
	dhcp2 = DHCPCreate(subnet, mask2);
	Test(NULL != dhcp2, ++test_num);
	Test((pow(2, 10) - 3) == DHCPCountFree(dhcp2), ++test_num);
	
	DHCPDestroy(dhcp1);
	DHCPDestroy(dhcp2);
}

void TestAlloc()
{
	dhcp_t *dhcp = NULL;
	unsigned char subnet[BYTES_IN_IP] = {200, 10, 30, 0};
	size_t mask = 24;
	int test_num = 0;
	int i = 0;
	unsigned char request[BYTES_IN_IP] = {200, 10, 30, 1};
	unsigned char result[BYTES_IN_IP] = {0, 0, 0, 0};
	
	printf("\n\nTEST ALLOC:\n\n");
	
	dhcp = DHCPCreate(subnet, mask);
	
	/* Test regular alloc. */
	Test(0 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(1 == result[3], ++test_num);
	Test((pow(2, 8) - 4) == DHCPCountFree(dhcp), ++test_num);
	
	/* Test alloc exist path. */
	Test(1 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(2 == result[3], ++test_num);
	
	Test((pow(2, 8) - 5) == DHCPCountFree(dhcp), ++test_num);
	
	/* Test alloc reserved path */
	request[3] = 0;
	Test(1 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(3 == result[3], ++test_num);
	Test((pow(2, 8) - 6) == DHCPCountFree(dhcp), ++test_num);
	
	/* Test alloc with non valid request */
	request[0] = 10;
	Test(1 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(4 == result[3], ++test_num);
	Test((pow(2, 8) - 7) == DHCPCountFree(dhcp), ++test_num);
	
	/* Test regular alloc in the middle of the trie */
	request[0] = 200;
	request[1] = 10;
	request[2] = 30;
	request[3] = 120;
	Test(0 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(120 == result[3], ++test_num);
	Test((pow(2, 8) - 8) == DHCPCountFree(dhcp), ++test_num);
	
	/*Test Alloc without request */
	request[0] = 0;
	request[1] = 0;
	request[2] = 0;
	request[3] = 0;
	Test(0 == DHCPAllocIP(dhcp, request, result), ++test_num);
	Test(5 == result[3], ++test_num);
	Test((pow(2, 8) - 9) == DHCPCountFree(dhcp), ++test_num);
	
	DHCPDestroy(dhcp);
}

void TestFree()
{
	dhcp_t *dhcp = NULL;
	unsigned char subnet[BYTES_IN_IP] = {200, 10, 30, 0};
	size_t mask = 28;
	int test_num = 0;
	unsigned char request[BYTES_IN_IP] = {0, 0, 0, 0};
	unsigned char result[BYTES_IN_IP] = {0, 0, 0, 0};
	size_t i = 0;
	unsigned char addr[BYTES_IN_IP] = {200, 10, 30, 4};
	
	printf("\n\nTEST FREE:\n\n");
	
	dhcp = DHCPCreate(subnet, mask);
	
	while (i < 16)
	{
		DHCPAllocIP(dhcp, request, result);
		++i;
	}
	
	Test(0 == DHCPCountFree(dhcp), ++test_num);
	
	/* Test regular free */
	Test(0 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	
	/* Test already free */
	Test(2 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	
	/* Test invalid address */
	addr[0] = 70;
	Test(1 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	
	/* Test free reserved ip */
	addr[0] = 200;
	addr[3] = 0;
	Test(3 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	addr[3] = 15;
	Test(3 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	addr[3] = 14;
	Test(3 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(1 == DHCPCountFree(dhcp), ++test_num);
	
	/* Test regular free */
	addr[3] = 10;
	Test(0 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(2 == DHCPCountFree(dhcp), ++test_num);
	addr[3] = 13;
	Test(0 == DHCPFreeIP(dhcp, addr), ++test_num);
	Test(3 == DHCPCountFree(dhcp), ++test_num);
	
	DHCPDestroy(dhcp);
}

void Test(int boolean, int test_num)
{
	printf("test no.%d\n", test_num);
	if (boolean)
	{
		printf("\tOK\n");
	}
	else
	{
		printf("\tFAIL\n");
	}

}











































