/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 02/04/2018
* DESCRITPTION	: This program is the unit test for
		  the doubly linked list implemented in 
		  q6/doubly_ll.c
                  
* HEADER FILES	: test_doubly_ll.h
****************************************************************/

#include "test_doubly_ll.h"

static void test_insert_at_beginning_head_is_null(void **state)
{
	assert_non_null(insert_at_beginning(NULL, 32));
}

void test_insert_at_end_head_is_null(void **state)
{
	assert_non_null(insert_at_end(NULL, 32));
}


void test_insert_at_position_head_is_null(void **state)
{
	assert_non_null(insert_at_position(NULL, 32, 0));	
}


void test_delete_from_beginning_head_is_null(void **state)
{
	assert_null(delete_from_beginning(NULL));
}

	
void test_delete_from_end_head_is_null(void **state)
{
	assert_null(delete_from_end(NULL));
}


void test_delete_from_position_head_is_null(void **state)
{
	assert_null(delete_from_position(NULL, 2));
}


void test_size_no_list(void **state)
{
	assert_non_null(size(NULL));
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_insert_at_beginning_head_is_null),
		cmocka_unit_test(test_insert_at_end_head_is_null),
		cmocka_unit_test(test_insert_at_position_head_is_null),

		cmocka_unit_test(test_delete_from_beginning_head_is_null),
		cmocka_unit_test(test_delete_from_end_head_is_null),
		cmocka_unit_test(test_delete_from_position_head_is_null),
		
		cmocka_unit_test(test_size_no_list)		
	};

	return cmocka_run_group_tests(tests,NULL,NULL);
}










