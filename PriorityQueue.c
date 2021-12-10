// taken from geeks from geeks: https://www.geeksforgeeks.org/priority-queue-using-linked-list/
// C code to implement Priority Queue
// using Linked List
#include <stdio.h>
#include <stdlib.h>

// NodePQ
typedef struct node {
	int data;

	// Lower values indicate higher priority
	int priority;

	struct node* next;


} NodePQ;

// Function to Create A New NodePQ
NodePQ* newNodePQ(int d, int p)
{
	NodePQ* temp = (NodePQ*)malloc(sizeof(NodePQ));
	temp->data = d;
	temp->priority = p;
	temp->next = NULL;

	return temp;
}

// Return the value at head
int peekPQ(NodePQ** head)
{
	return (*head)->data;
}

// Removes the element with the
// highest priority form the list
void popPQ(NodePQ** head)
{
	NodePQ* temp = *head;
	(*head) = (*head)->next;
	free(temp);
}

// Function to pushPQ according to priority
void pushPQ(NodePQ** head, int d, int p)
{
	NodePQ* start = (*head);

	// Create new NodePQ
	NodePQ* temp = newNodePQ(d, p);

	// Special Case: The head of list has lesser
	// priority than new node. So insert new
	// node before head node and change head node.
	if ((*head)->priority > p) {

		// Insert New NodePQ before head
		temp->next = *head;
		(*head) = temp;
	}
	else {

		// Traverse the list and find a
		// position to insert new node
		while (start->next != NULL &&
			start->next->priority < p) {
			start = start->next;
		}

		// Either at the ends of the list
		// or at required position
		temp->next = start->next;
		start->next = temp;
	}
}

// Function to check is list is empty
int isEmptyPQ(NodePQ** head)
{
	return (*head) == NULL;
}

