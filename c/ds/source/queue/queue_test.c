/********************************
* Author: David Berman			*
* Last Update: 10/11/2019		*
* Data structure: queue	    	*
* Tester File                   *
* ******************************/

#include <stdio.h>
#include <assert.h> /* assert */

#include "queue.h"
#include "slist_node.h"

int main(void)
{
	int data = 5;
	int res_enqueue = 0;
	int int_data_arr1[5] = {1,2,3,4,5};
	int int_data_arr2[10] = {6,7,8,9,10,11,12,13,14,15};
	int i = 0;
	queue_t *queue2 = QueueCreate();
	queue_t *queue1 = QueueCreate();
	
	res_enqueue = QueueEnqueue(queue2, &data);
	printf("enqueue is %d\n", res_enqueue);
	
	printf("size of queue1 : %lu\n", QueueSize(queue1));
	printf("size of queue2 : %lu\n", QueueSize(queue2));
	QueueDequeue(queue2);
	printf("size of queue2 : %lu\n", QueueSize(queue2));
	QueueEnqueue(queue2, &data);
	printf("peek res is: %d\n",*(int *)QueuePeek(queue2));
	printf("queue1 empty res is : %d\n",QueueIsEmpty(queue1));
	printf("queue2 empty res is : %d\n",QueueIsEmpty(queue2));
	
	for (i = 0; i < 5; ++i)
	{
		QueueEnqueue(queue1, int_data_arr1 + i);
	}
	for (i = 0; i < 10; ++i)
	{
		QueueEnqueue(queue2, int_data_arr2 + i);
	}
	QueueDequeue(queue2);
	printf("size of queue1 : %lu\n", QueueSize(queue1));
	printf("size of queue1 : %lu\n", QueueSize(queue2));
	QueueAppend(queue1, queue2);
	printf("size of queue1 : %lu\n", QueueSize(queue1));
	printf("size of queue2 : %lu\n", QueueSize(queue2));
	QueueDestroy(queue1);
	QueueDestroy(queue2);
	
	
	
	return 0;
}
