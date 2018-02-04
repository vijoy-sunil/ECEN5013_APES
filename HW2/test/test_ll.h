/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 01/31/2018
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
                  
* SOURCE FILES	: doubly_ll.c
****************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define offsetof(type, member) ((size_t) &((type *)0)->member)

#define GET_LIST_CONTAINER(ptr, type, member) ({                      \
       const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
       (type *)( (char *)__mptr - offsetof(type,member) );})


typedef struct node {
	struct node *prev;
	struct node *next;
}node_t;

typedef struct info {
	uint32_t data;
	struct node list;
}info_t;


node_t* insert_at_beginning(node_t*, uint32_t);




