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

#include "doubly_ll.h"

node_t* head;

node_t* insert_at_beginning(node_t* head, uint32_t data)
{

	//if no head exists, init this node as head
	if(head == NULL)
	{
		//allocate memory
		info_t* new_node = (info_t*)malloc(sizeof(info_t));
		if(new_node == NULL)
			printf("\nMALLOC FAIL !!!\n");
		
		new_node->data = data;
				
		new_node->list.prev = NULL;
		new_node->list.next = NULL;

		head = &(new_node->list);
	}

	else
	{
		info_t* new_node = (info_t*)malloc(sizeof(info_t));
		if(new_node == NULL)
			printf("\nMALLOC FAIL !!!\n");

		new_node->data = data;

		new_node->list.prev = NULL;
		new_node->list.next = head;

		
		
		head->prev = &(new_node->list); 

		head = &(new_node->list);
	}

	return head;
}

node_t* insert_at_end(node_t* head, uint32_t data)
{
	//if no head exists, init this node as head
	if(head == NULL)
	{
		//allocate memory
		info_t* new_node = (info_t*)malloc(sizeof(info_t));
		if(new_node == NULL)
			printf("\nMALLOC FAIL !!!\n");

		new_node->data = data;
				
		new_node->list.prev = NULL;
		new_node->list.next = NULL;

		head = &(new_node->list);
		
	}

	else
	{
		//allocate memory
		info_t* new_node = (info_t*)malloc(sizeof(info_t));
		if(new_node == NULL)
			printf("\nMALLOC FAIL !!!\n");

		node_t* temp_node;
		temp_node = head;

		//traverse the list till end
		while(temp_node->next != NULL)
			temp_node = temp_node->next;				
		
		temp_node->next = &(new_node->list);
		new_node->list.prev = temp_node;
		new_node->list.next = NULL;

		new_node->data = data;
						
	}
	return head;
	
}

node_t* insert_at_position(node_t* head, uint32_t data, uint32_t index)
{
	size_t len;
	int i = 0;
	//if no head exists, init this node as head
	if(head == NULL)
	{
		if(index == 0)
			head = insert_at_beginning(head, data);
		else
			printf("\nINDEX OUT OF BOUNDS!!!\n");
	}
	
	else
	{
		//allocate memory
		info_t* new_node = (info_t*)malloc(sizeof(info_t));
		if(new_node == NULL)
			printf("\nMALLOC FAIL !!!\n");

		//find number of links in list
		len = size(head);
		if(len == -1)
			printf("\nNO LIST\n");

		if(index == 0)
			head = insert_at_beginning(head, data);
		else if(index == len + 1)
			head = insert_at_end(head, data);
		else
		{
			if((index > len + 1) || (index < 0)){
				printf("\nINDEX OUT OF BOUNDS!!!\n");
				return head;
			}
			else
			{
				node_t* temp_node;
				temp_node = head;

				node_t* stemp;

				//traverse to index
				while(i < (index-1))
				{
					i++;
					temp_node = temp_node->next;	
				}

				stemp = temp_node->next;
				//temp_node->next = &(new_node->list);					

				//new_node->list.prev = temp_node;
				//new_node->list.next = stemp;

				//stemp->prev = &(new_node->list);

				new_node->list.prev = temp_node;				
				new_node->list.next = temp_node->next;

				temp_node->next = &(new_node->list);
				stemp->prev =&(new_node->list); 

				new_node->data = data;				
			}
				
		}
	}
	return head;
}

size_t size(node_t* any_node)
{
	size_t len = 1;
	node_t* temp_node;
	temp_node = any_node;

	if(any_node == NULL)
		len = -1;
	else
	{	
		//backward traverse till head
		while(temp_node != NULL){
			len++;
			temp_node = temp_node->prev;	
		}
		len--;

		//forward traverse till tail
		temp_node = any_node;
		while(temp_node != NULL){
			len++;
			temp_node = temp_node->next;
		}
		len--;
	}
	return len;	
}

node_t* delete_from_beginning(node_t* head)
{
	//if no list	
	if(head == NULL){
		printf("\nNO LIST\n");
		return NULL;
	}

	//if list contains only head	
	else if(head->next == NULL)
	{		
		info_t* node_addr = GET_LIST_CONTAINER(head, info_t, list);
		free(node_addr);		
		return NULL;
	}
	else
	{
		node_t* temp_node;
		temp_node = head->next;
		info_t* node_addr = GET_LIST_CONTAINER(head, info_t, list);
		free(node_addr);

		temp_node->prev = NULL;
		head = temp_node;
		
		return head;
	}
}

node_t* delete_from_end(node_t* head)
{
	//if no list	
	if(head == NULL){
		printf("\nNO LIST\n");
		return NULL;
	}

	//if list contains only head
	else if(head->next == NULL)
	{
		info_t* node_addr = GET_LIST_CONTAINER(head, info_t, list);
		free(node_addr);		
		return NULL;
	}
	else
	{
		node_t* temp_node;
		temp_node = head;
		
		//traverse till end
		while(temp_node->next != NULL){
			temp_node = temp_node->next;
		}	
		
		node_t* secondlast_node = temp_node->prev;
		secondlast_node->next = NULL;
		temp_node->prev = NULL;	

		info_t* node_addr = GET_LIST_CONTAINER(temp_node, info_t, list);
		free(node_addr);				
		return head;
	}
}

node_t* delete_from_position(node_t* head, uint32_t index)
{
	int len, i;
	//if no list	
	if(head == NULL){
		printf("\nNO LIST\n");
		return NULL;
	}
	else
	{
		//find number of links in list
		len = size(head);
		if(len == -1)
			printf("\nNO LIST\n");

		if(index == 0)
			head = delete_from_beginning(head);
		else if(index == len - 1)
			head = delete_from_end(head);
		else
		{
			if((index >= len) || (index < 0)){
				printf("\nINDEX OUT OF BOUNDS!!!\n");
				return head;
			}
			else
			{
				i = 0;
				node_t* temp_node;
				temp_node = head;
 
				node_t* before_node, *after_node;

				//traverse to index
				while(i  < (index))
				{
					temp_node = temp_node->next;
					i++;	
				}

				before_node = temp_node->prev;
				info_t* addr = GET_LIST_CONTAINER(before_node, info_t, list);
				printf("-----%d", addr->data);

				after_node = temp_node->next;
				info_t* asaddr = GET_LIST_CONTAINER(after_node, info_t, list);
				printf("-----%d", asaddr->data);


				before_node->next = after_node;
				after_node->prev = before_node;

				info_t* node_addr = GET_LIST_CONTAINER(temp_node, info_t, list);
				free(node_addr);
				
				return head;	
			}
		}	
	}
}

node_t* destroy(node_t* head)
{
	head = NULL;
	return head;
}

void print_mylist(node_t* head)
{
	node_t* temp_node;
	temp_node = head;
	int i = 0;

	if(temp_node == NULL)
		printf("\nLIST EMPTY!!!\n");

	else{
		//traverse the list till end
		while(temp_node != NULL)
		{
			info_t* curr_node_addr = GET_LIST_CONTAINER(temp_node, info_t, list);
			printf("Element [%d]: %d\n", i, curr_node_addr->data);	
			i++;	
			temp_node = temp_node->next;
		}

		printf("\nList size: %ld\n", size(head));
	}	
}

int main(void)
{

	head = insert_at_beginning(head, 16);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 24);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 32);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 40);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 48);	printf("\nhead addr: %p\n", head);

	head = insert_at_beginning(head, 56);	printf("\nhead addr: %p\n", head);


	head = insert_at_end(head, 64);	printf("\nhead addr: %p\n", head);

	head = insert_at_end(head, 72);	printf("\nhead addr: %p\n", head);

	head = insert_at_position(head, 999, 4);	printf("\nhead addr: %p\n", head);

	print_mylist(head);
	printf("\n------------------------------\n");


	head = delete_from_beginning(head);	printf("\nhead addr: %p\n", head);

	print_mylist(head);
	printf("\n------------------------------\n");

	head = delete_from_end(head);	printf("\nhead addr: %p\n", head);

	print_mylist(head);
	printf("\n------------------------------\n");

	head = delete_from_position(head, 3);	printf("\nhead addr: %p\n", head);

	print_mylist(head);
	printf("\n------------------------------\n");

	head = destroy(head);	printf("\nhead addr: %p\n", head);

	print_mylist(head);
	printf("\n------------------------------\n");

	return 0;	
}
















