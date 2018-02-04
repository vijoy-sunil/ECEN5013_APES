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

struct info {
	void *data;
	struct list_head list;
};

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

