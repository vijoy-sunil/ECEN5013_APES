/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 02/03/2018
* DESCRITPTION	: This program implemets a doubly linked list with
		  the following functions -
		  destroy,
		  insert_at_beginning,
		  insert_at_end,
           	  insert_at_position,
		  delete_from_beginning,
		  delete_from_end,
           	  delete_from_position,
		  size
                  
* HEADER FILES	: doubly_ll.h
****************************************************************/

#include "test_ll.h"

node_t* head = NULL;
/*
node_t* insert_at_beginning(node_t* head, uint32_t data)
{

	//if no head exists, init this node as head
	if(head == NULL)
	{
		//allocate memory
		head = (node_t*)malloc(sizeof(node_t*));
	}

	return head;
}
*/

node_t* insert_at_beginning(node_t* head, uint32_t data)
{

	//if no head exists, init this node as head
	if(head == NULL)
	{
		//allocate memory
		head = (node_t*)malloc(sizeof(node_t*));
		if(head == NULL)
			printf("\nMALLOC FAIL !!!\n");

		head->prev = NULL;
		head->next = NULL;
		info_t* new_node_addr = GET_LIST_CONTAINER(head, info_t, list);
		new_node_addr->data = data;
		printf("\nnewnode addr: %p\n", new_node_addr);	
	}

	else
	{
		node_t* new_head =  (node_t*)malloc(sizeof(node_t*));
		if(new_head == NULL)
			printf("\nMALLOC FAIL !!!\n");

		new_head->prev = NULL;
		new_head->next = head;
		info_t* new_node_addr = GET_LIST_CONTAINER(new_head, info_t, list);
		new_node_addr->data = data;
		printf("\nnewnode addr: %p\n", new_node_addr);		
		
		head->prev =new_head; 

		head = new_head;
	}

	return head;
}

int main(void)
{

	printf("\nhead addr: %p\n", head);
	head = insert_at_beginning(head, 16);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 16);	printf("\nhead addr: %p\n", head);

	//info_t* node_addr = GET_LIST_CONTAINER(head, info_t, list);
	free(head);				printf("\nhead addr: %p\n", head);
	

	return 0;	
}
















