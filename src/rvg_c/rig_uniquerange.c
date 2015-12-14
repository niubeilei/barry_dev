////////////////////////////////////////////////////////////////////////////
//
// Copyrig_uniquerangeht (C) 2005
// Packet Engineering, Inc. All rig_uniquerangehts reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class is used to generate unique values for a range [start, end].
// At construction time, it constructs an array:
// 		elements[end-start+1], where elements[i] = i + start, i = 0,...
// The member data 'used_size' indicates the last element in
// 'elements[...]' that is not used yet. 
//
// When generating a new value, it randomly pick an element between 
// [0, unused_size-1]. It then swap the element elements[picked] and
// elements[unused_size-1], and 'unused_size' decrements by one.
//
// When resetting the unique cycle, it sets 'unused_size' = end-start=1.
//
// Modification History:
// 03/16/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rig_uniquerange.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "util_c/strutil.h"
#include "util_c/memory.h"
#include <stdio.h>

#define AOS_rig_uniquerange_MAX_SIZE 10000

int aos_rig_uniquerange_integrity_check(aos_rig_uniquerange_t *entry)
{
	aos_assert_r(entry, -1);
	aos_assert_r(entry->elements, -1);
	aos_assert_r(entry->end >= entry->start, -1);
	aos_assert_r(entry->unused_size >= 0 && 
				 entry->unused_size < entry->end - entry->start + 1, -1);
	aos_assert_r(entry->weight >= 0, -1);
	return 0;
}


int aos_rig_uniquerange_serialize(
		aos_rig_uniquerange_t *entry, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Range>start, end, weight</Range>
	//
	aos_assert_r(entry, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(entry->mf->integrity_check(entry) == 1, -1);

	char buff[100];
	sprintf(buff, "%d, %d, %d", entry->start, entry->end, entry->weight);
	aos_assert_r(!parent->mf->append_child_str(
			parent, "Range", buff, 0, 0), -1);
	return 0;
}


int aos_rig_uniquerange_deserialize(
		aos_rig_uniquerange_t *entry, 
		aos_xml_node_t *node)
{
	aos_assert_r(entry, -1);
	aos_assert_r(node, -1);

	char buff[100];
	int buff_len = 100;
	aos_assert_r(!node->mf->get_str((aos_field_t *)node, buff, &buff_len), -1);
	
	int len = strlen(buff);
	int idx;
	aos_assert_r(!aos_str_get_int(buff, len	, &idx, &entry->start), -1);
	aos_assert_r(!aos_str_get_int(buff, len	, &idx, &entry->end), -1);
	aos_assert_r(!aos_str_get_int(buff, len	, &idx, &entry->weight), -1);
	
	int size = entry->end - entry->start + 1;
	aos_assert_r(size >= 0 && size < AOS_rig_uniquerange_MAX_SIZE, -1);
	aos_assert_r(entry->weight >= 0, -1);

	entry->unused_size = size;
	entry->elements = aos_malloc(sizeof(int) * size);
	aos_assert_g(entry->elements, cleanup);

	int i;
	for (i=0; i<size; i++) entry->elements[i] = i + entry->start;
	return 0;

cleanup:
	aos_free(entry->elements);
	entry->elements = 0;
	return 0;
}


static int aos_rig_uniquerange_reset_unique(aos_rig_uniquerange_t *entry)
{
	aos_assert_r(entry, -1);
	entry->unused_size = entry->end - entry->start + 1;
	return 0;
}


int aos_rig_uniquerange_is_full(aos_rig_uniquerange_t *entry)
{
	aos_assert_r(entry, -1);
	return entry->unused_size == 0;
}


int aos_rig_uniquerange_next_int(
		aos_rig_uniquerange_t *entry, 
		int *value)
{
	aos_assert_r(entry, -1);
	aos_assert_r(value, -1);
	aos_assert_r(entry->unused_size > 0, -1);

	int idx = aos_next_int(0, entry->unused_size-1);
	*value = entry->elements[idx];

	// Put idx-th element into the used portion
	entry->elements[idx] = entry->elements[entry->unused_size-1];
	entry->elements[entry->unused_size-1] = *value;
	
	entry->unused_size--;

	return 0;
}


static int aos_rig_uniquerange_release_memory(aos_rig_uniquerange_t *entry)
{
	aos_assert_r(entry, -1);
	if (entry->elements) aos_free(entry->elements);
	entry->elements = 0;
	return 0;
}


static int aos_rig_uniquerange_destroy(aos_rig_uniquerange_t *entry)
{
	aos_assert_r(!aos_rig_uniquerange_release_memory(entry), -1);
	aos_free(entry);
	return 0;
}


static aos_rig_uniquerange_mf_t sg_mf = 
{
	aos_rig_uniquerange_serialize,
	aos_rig_uniquerange_deserialize,
	aos_rig_uniquerange_integrity_check,
	aos_rig_uniquerange_release_memory,
	aos_rig_uniquerange_destroy,
	aos_rig_uniquerange_is_full, 
	aos_rig_uniquerange_next_int,
	aos_rig_uniquerange_reset_unique
};


int aos_rig_uniquerange_init(aos_rig_uniquerange_t *rig_uniquerange)
{
	aos_assert_r(rig_uniquerange, -1);
	memset(rig_uniquerange, 0, sizeof(aos_rig_uniquerange_t));
	rig_uniquerange->mf = &sg_mf;

	return 0;
}


aos_rig_uniquerange_t *aos_rig_uniquerange_create(
		const int start, 
		const int end, 
		const int weight)
{
	int size = end - start + 1;
	aos_assert_r(size >= 0 && size < AOS_rig_uniquerange_MAX_SIZE, 0);
	aos_assert_r(weight >= 0, 0);

	aos_rig_uniquerange_t *entry = aos_malloc(sizeof(*entry));
	aos_assert_r(entry, 0);
	aos_assert_g(!aos_rig_uniquerange_init(entry), cleanup);

	entry->start = start;
	entry->end = end;
	entry->weight = weight;
	entry->unused_size = size;
	entry->elements = aos_malloc(sizeof(int) * size);
	aos_assert_g(entry->elements, cleanup);

	int i;
	for (i=0; i<size; i++) entry->elements[i] = i + start;
	return entry;

cleanup:
	if (entry->elements) aos_free(entry->elements);
	aos_free(entry);
	return 0;
}


// 
// Description
// It creates an aos_rig_uniquerange_t based on the configuration 'node'.
// The caller must 'hold' the object. 
//
aos_rig_uniquerange_t *aos_rig_uniquerange_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);

	aos_rig_uniquerange_t *obj = aos_malloc(sizeof(aos_rig_uniquerange_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rig_uniquerange_init(obj), cleanup);

	aos_assert_g(!obj->mf->deserialize(obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

