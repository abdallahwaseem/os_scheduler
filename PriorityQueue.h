//Taken from https://www.geeksforgeeks.org/queue-linked-list-implementation/
// A C program to demonstrate linked list based implementation of queue
#include <stdio.h>
#include <stdlib.h>

// To create a Queue call struct QueuePQ* Queue_name = createQueuePQ() first 
// then you can can any function 
// A linked list (LL) node to store a queue entry
struct QNodePQ {
	processData key;
    // Lower values indicate higher priority
	int priority;

	struct QNodePQ* next;
};

// The queue, front stores the front node of LL and rear stores the
// last node of LL
struct QueuePQ {
	struct QNodePQ *front, *rear;
};

// A utility function to create a new linked list node.
struct QNodePQ* newNodePQ(processData k, int p)
{
	struct QNodePQ* temp = (struct QNodePQ*)malloc(sizeof(struct QNodePQ));
	temp->key = k;
    temp->priority = p;
	temp->next = NULL;
	return temp;
}

// A utility function to create an empty queue
struct QueuePQ* createQueuePQ()
{
	struct QueuePQ* q = (struct QueuePQ*)malloc(sizeof(struct QueuePQ));
	q->front = q->rear = NULL;
	return q;
}

// The function to add a key k to q
void enQueuePQ(struct QueuePQ* q, processData k, int priority)
{
    struct QNodePQ *start = q->front;
	// Create a new LL node
	struct QNodePQ* temp = newNodePQ(k,priority);

	// If queue is empty, then new node is front and rear both
	if (q->rear == NULL) {
		q->front = q->rear = temp;
		return;
	}
    // Special Case: The head of list has lesser
	// priority than new node. So insert new
	// node before head node and change head node.
	if (q->front->priority > priority)
	{
		// Insert New Node before head
		temp->next = q->front;
		(q)->front = temp;
	}
	else
	{

		// Traverse the list and find a
		// position to insert new node
		while (start->next != NULL &&start->next->priority < priority)
		{
			start = start->next;
		}

		// Either at the ends of the list
		// or at required position
		temp->next = start->next;
		start->next = temp;
        if(temp->next == NULL)
        {
            q->rear = temp;
        }
	}
}

// Function to remove a key from given queue q
void deQueuePQ(struct QueuePQ* q)
{
	// If queue is empty, return NULL.
	if (q->front == NULL)
		return;

	// Store previous front and move front one node ahead
	struct QNodePQ* temp = q->front;

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL
	if (q->front == NULL)
		q->rear = NULL;

	free(temp);
}

processData peekQueuePQ(struct QueuePQ* q)
{
	return q->front->key;
}


int isEmptyQueuePQ(struct QueuePQ* q)
{
	return q->front == NULL;
}


