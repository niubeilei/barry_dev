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
// 02/06/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util_c/dyn_array.h"

#include "alarm_c/alarm.h"
#include "util_c/memory.h"


// 
// Description
// Integrity Check includes:
//
static int aos_dyn_array_integrity_check(
		aos_dyn_array_t *array, 
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(array->elem_size > 0, -1);
	aos_assert_t(array->noe > 0, array->buffer, -1);
	aos_assert_r(array->inc_size > 0, -1);
	aos_assert_r(array->max_size == -1 || array->max_size > 0, -1);
	aos_assert_r(array->noe <= array->buff_size, -1);
	aos_assert_t(ptr, *ptr == array->buffer, -1);
	return 1;
}


// 
// Description
// This function prepares the array to add 'noe' number of 
// elements. If successful, this function guarantees that
// it has enough memory to add 'noe' elements to the array.
//
static int aos_dyn_array_add_element(
		struct aos_dyn_array *array, 
		const int noe, 
		char **ptr)
{
	int new_size;
	char *buff;
	aos_assert_r(array, -1);
	aos_assert_r(array->mf->integrity_check(array, ptr) == 1, -1);

	if (array->noe + noe < array->buff_size)
	{
		// 
		// There is enough space. Do nothing
		//
		return 0;
	}

	aos_assert_r(array->max_size == -1 ||
			array->noe + noe <= array->max_size, -1);

	// 
	// Need to grow the array. 
	//
	if (noe <= array->inc_size)
	{
		new_size = array->buff_size + array->inc_size;
	}
	else
	{
		new_size = array->buff_size + noe + array->inc_size;
	}

	if (array->max_size > 0 && new_size > array->max_size)
	{
		new_size = array->max_size;
	}

	buff = aos_malloc(new_size * array->elem_size);
	aos_assert_rm(buff, -1, "Size: %d:%d", new_size, array->elem_size);
	memcpy(buff, array->buffer, array->elem_size * array->noe);
	aos_free(array->buffer);
	array->buffer = buff;
	if (ptr) *ptr = buff;
	array->buff_size = new_size;
	return 0;
}


// 
// Description
// This function inserts 'noe' elements at the position 'pos'. 
// if 'pos' == 'array->noe', it is equivalent to append.
// After spaces are allocated, it will move all the elements
// from the position pos. Or in other word, after the operation,
// the element ptr[pos] becomes ptr[pos+noe]. This results
// in ptr[pos], ptr[pos+1], ..., ptr[pos+noe-1] becoming 
// empty slots. Note that array->noe will be modified accordingly.
//
static int aos_dyn_array_insert(
		struct aos_dyn_array *array, 
		const int pos,
		const int noe, 
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(pos >= 0 && pos <= array->noe, -1);
	aos_assert_r(!aos_dyn_array_add_element(array, noe, ptr), -1);
	
	int new_pos = (pos + noe) * array->elem_size;
	int old_pos = pos * array->elem_size;
	int num_bytes = noe * array->elem_size;
	memmove(&array->buffer[new_pos], &array->buffer[old_pos], num_bytes);
	return 0;
}


// 
// Description
// This function deletes 'noe' number of elements.
//
static int aos_dyn_array_del_element(
		struct aos_dyn_array *array, 
		const int idx,
		const int noe, 
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(array->mf->integrity_check(array, ptr) == 1, -1);
	aos_assert_r(noe > 0 && idx + noe <= array->noe, -1);
	aos_assert_r(idx >= 0 && idx <array->noe, -1);

	memmove(&array->buffer[idx*array->elem_size], 
			&array->buffer[(idx + noe)*array->elem_size], 
			(array->noe - (idx + noe) + 1)*array->elem_size);
	array->noe -= noe;
	return 0;
}


// 
// Description
// This function removes all elements.
//
static int aos_dyn_array_remove_all(
		aos_dyn_array_t *array, 
		char **ptr)
{
	aos_assert_r(array, -1);
	return array->mf->del_element(array, 0, array->noe, ptr);
}


static int aos_dyn_array_add_char(
		aos_dyn_array_t *array, 
		const char c, 
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(!array->mf->add_element(array, 1, ptr), -1);
	array->buffer[array->noe++] = c;
	return 0;
}


static int aos_dyn_array_add_chars(
		aos_dyn_array_t *array, 
		const char c, 
		const int repeat,
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(!array->mf->add_element(array, repeat, ptr), -1);
	memset(&array->buffer[array->noe], c, repeat);
	array->noe += repeat;
	return 0;
}


static int aos_dyn_array_add_str(
		aos_dyn_array_t *array, 
		const char * const str,
		const int len,
		char **ptr)
{
	aos_assert_r(array, -1);
	aos_assert_r(str, -1);
	aos_assert_r(!array->mf->add_element(array, len, ptr), -1);
	memcpy(&array->buffer[array->noe], str, len);
	array->noe += len;
	return 0;
}


static aos_dyn_array_mf_t sg_mf = 
{
	AOS_DYN_ARRAY_MEMFUNC_INIT
};


int aos_dyn_array_init(
		aos_dyn_array_t *array,
		char **ptr, 
		const int elem_size, 
		const int inc_size, 
		const int max_size)
{
	aos_assert_r(elem_size > 0, -1);
	aos_assert_r(inc_size > 0, -1);
	aos_assert_r(max_size == -1 || max_size > 0, -1);
	aos_assert_r(array, -1);

	memset(array, 0, sizeof(aos_dyn_array_t));
	array->mf = &sg_mf;
	array->elem_size = elem_size;
	array->inc_size = inc_size;
	array->max_size = max_size;
	if (ptr) *ptr = array->buffer;
	aos_assert_g(array->mf->integrity_check(array, ptr) == 1, cleanup);
	return 0;

cleanup:
	aos_free(array);
	return -1;
}


int aos_dyn_array_release_memory(aos_dyn_array_t *array) 
{
	aos_assert_r(array, -1);
	if (array->buffer) 
	{
		aos_free(array->buffer);
		array->buffer = 0;
		array->buff_size = 0;
		array->noe = 0;
	}

	return 0;
}


aos_dyn_array_t *aos_dyn_array_create(
		char **ptr,
		const int elem_size, 
		const int inc_size, 
		const int max_size)
{
	aos_dyn_array_t *array = aos_malloc(sizeof(aos_dyn_array_t));
	aos_assert_r(array, 0);
	aos_assert_g(!aos_dyn_array_init(array, ptr, elem_size, inc_size, 
			max_size), cleanup);
	return array;

cleanup:
	aos_free(array);
	return 0;
}



