////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 02/29/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/attr.h"
#include "util_c/memory.h"

#define AOS_ATTR_NAME_MAX_LEN 32
#define AOS_ATTR_VALUE_MAX_LEN 64

#include "alarm_c/alarm.h"
#include <string.h>


int aos_attr_init(aos_attr1_t *attr)
{
	aos_assert_r(attr, -1);
	attr->name[0] = 0;
	attr->value = 0;
	return 0;
}

int aos_attr_copy(aos_attr1_t *to, aos_attr1_t *from)
{
	aos_assert_r(to, -1);
	aos_assert_r(from, -1);
	strcpy(to->name, from->name);
	if (to->value && strlen(to->value) < strlen(from->value))
	{
		aos_free(to->value);
		to->value = aos_malloc(strlen(from->value) + 1);
		aos_assert_r(to->value, -1);
	}
	strcpy(to->value, from->value);
	return 0;
}

int aos_attr_free_all(aos_attr1_t **attr, const int num_attr)
{
	aos_assert_r(attr, -1);
	aos_assert_r(num_attr >= 0, -1);
	int i;
	for (i=0; i<num_attr; i++)
	{
		if (attr[i]->value) aos_free(attr[i]->value);
		aos_free(attr[i]);
	}
	return 0;
}


int aos_attr_set(
		aos_attr1_t *attr, 
		const char * const name, 
		const char * const value)
{
	aos_assert_r(attr, -1);
	aos_assert_r(name, -1);
	aos_assert_r(value, -1);
	aos_assert_r(strlen(name) < AOS_ATTR_NAME_MAX_LEN, -1);

	if (attr->value)
	{
		aos_free(attr->value);
	}
	attr->value = aos_malloc(strlen(value));
	aos_assert_r(attr->value, -1);
	strcpy(attr->name, name);
	strcpy(attr->value, value);
	return 0;
}

