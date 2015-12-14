////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/list.h"

#include "util_c/memory.h"
#include <stdio.h>

struct test_list
{
	struct aos_list_head list;
	int age;
};

int 
aos_list_tester()
{
	struct aos_list_head theHead;
	AOS_INIT_LIST_HEAD(&theHead);

	struct test_list *data = aos_malloc(sizeof(struct test_list));
	AOS_INIT_LIST_HEAD(&data->list);
	data->age = 1;
	aos_list_add_tail(&data->list, &theHead);

	struct test_list *data1 = aos_malloc(sizeof(struct test_list));
	AOS_INIT_LIST_HEAD(&data1->list);
	data1->age = 2;
	aos_list_add_tail(&data1->list, &theHead); 

	struct test_list *data2 = aos_malloc(sizeof(struct test_list));
	AOS_INIT_LIST_HEAD(&data2->list);
	data2->age = 3;
	aos_list_add_tail(&data2->list, &theHead); 

	struct test_list *data3 = aos_malloc(sizeof(struct test_list));
	AOS_INIT_LIST_HEAD(&data3->list);
	data3->age = 4;
	aos_list_add_tail(&data3->list, &theHead); 

	struct test_list *var1 = 0;
	// aos_list_for_each_entry(var1, var2, &(data->list), list)
	aos_list_for_each_entry(var1, &theHead, list)
	{
		// printf("age: %d, age1: %d\n", var1->age, var2->age);
		printf("age: %d\n", var1->age);
	}
	return 0;
} 

