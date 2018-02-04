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
		head = (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		head->prev = NULL;
		head->next = NULL;
		info_t* new_node_addr = GET_LIST_CONTAINER(head, info_t, list);
		new_node_addr->data = data;	
	}

	else
	{
		node_t* new_head =  (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		new_head->prev = NULL;
		new_head->next = head;
		info_t* new_node_addr = GET_LIST_CONTAINER(new_head, info_t, list);
		new_node_addr->data = data;	
		
		head->prev =new_head; 

		head = new_head;
	}

	return head;
}

node_t* insert_at_end(node_t* head, uint32_t data)
{
	//if no head exists, init this node as head
	if(head == NULL)
	{
		//allocate memory
		head = (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		head->prev = NULL;
		head->next = NULL;
		info_t* new_node_addr = GET_LIST_CONTAINER(head, info_t, list);
		new_node_addr->data = data;		
	}

	else
	{
		//allocate memory
		node_t* new_tail = (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		node_t* temp_node;
		temp_node = head;

		//traverse the list till end
		while(temp_node->next != NULL)
			temp_node = temp_node->next;				
		
		temp_node->next = new_tail;
		new_tail->prev = temp_node;
		new_tail->next = NULL;

		info_t* new_node_addr = GET_LIST_CONTAINER(new_tail, info_t, list);
		new_node_addr->data = data;
						
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
		//allocate memory
		head = (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		head->prev = NULL;
		head->next = NULL;
		info_t* new_node_addr = GET_LIST_CONTAINER(head, info_t, list);
		new_node_addr->data = data;		
	}
	
	else
	{
		node_t* new_list = (node_t*)malloc(sizeof(node_t*));
		info_t* new_node = (info_t*)malloc(sizeof(info_t*));

		//find number of links in list
		len = size(head);
		if(len == -1)
			printf("\nNO LIST\n");

		if(index == 0)
			head = insert_at_beginning(head, data);
		else if(index == len)
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

				//traverse to index
				while(i != index)
				{
					i++;
					temp_node = temp_node->next;	
				}
				temp_node = temp_node->prev;
				new_list->prev = temp_node;
				new_list->next = temp_node->next;
				temp_node->next = new_list;
				temp_node->next->prev =new_list ;				

				info_t* new_node_addr = GET_LIST_CONTAINER(new_list, info_t, list);
				new_node_addr->data = data;				
			}
				
		}
	}
	return head;
}

size_t size(node_t* any_node)
{
	size_t len;
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

void print_mylist(node_t* head)
{
	node_t* temp_node;
	temp_node = head;
	int i = 0;

	//traverse the list till end
	while(temp_node != NULL)
	{
		info_t* curr_node_addr = GET_LIST_CONTAINER(temp_node, info_t, list);
		printf("Element [%d]: %d\n", i, curr_node_addr->data);	
		i++;	
		temp_node = temp_node->next;
	}	
}

int main(void)
{
	info_t* current_node_addr;

	head = insert_at_beginning(head, 16);

	head = insert_at_beginning(head, 24);

	head = insert_at_beginning(head, 32);

	head = insert_at_end(head, 64);

	head = insert_at_end(head, 72);

	head = insert_at_position(head, 80, 3);

	print_mylist(head);
	return 0;	
}
















