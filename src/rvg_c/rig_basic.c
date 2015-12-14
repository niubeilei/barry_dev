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
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rig_basic.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include <stdio.h>


static int aos_rig_basic_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		<Type>
	// 		<Ranges>
	// 			<Range>start, end, weight</Range>
	// 			<Range>start, end, weight</Range>
	//			...
	//		</Ranges>
	// 	</Parent>
	//
	char buff[100];
	int i;
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t*)rvg;

	aos_assert_r(!aos_rig_serialize(rvg, parent), -1);

	// Add "<Ranges>"
	aos_xml_node_t *ranges = parent->mf->append_child_node(
			parent, "Ranges");
	aos_assert_r(ranges, -1);

	for (i=0; i<self->index_array.noe; i++)
	{
		// Add <Range>
		sprintf(buff, "%d, %d, %d", self->ranges[i]->start, 
				self->ranges[i]->end, self->ranges[i]->weight);
		aos_assert_r(!ranges->mf->append_child_str(
			ranges, "Range", buff, 0, 0), -1);
	}
	ranges->mf->put((aos_field_t *)ranges);
	return 0;
}


static int aos_rig_basic_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	char buff[100];
	int buff_len = 100;
	int start, end, weight;

	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(rvg->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t*)rvg;

	// Retrieve "<Name>"
	if (self->name) aos_free(self->name);
	aos_assert_r(!aos_str_set(&self->name, node->name, 
			strlen(node->name)), -1);

	// Retrieve <Ranges>
	aos_xml_node_t *ranges = node->mf->first_named_child(node, "Ranges");
	aos_assert_r(ranges, -1);
	
	aos_field_t *range = (aos_field_t *)ranges->mf->first_child(ranges);
	while (range)
	{
		int idx = 0;
		buff_len = 100;
		aos_assert_r(!range->mf->get_str(range, buff, &buff_len), -1);
		int len = strlen(buff);
		aos_assert_g(!aos_str_get_int(buff, len, &idx, &start), cl);
		aos_assert_g(!aos_str_get_int(buff, len, &idx, &end), cl);
		aos_assert_g(!aos_str_get_int(buff, len, &idx, &weight), cl);
		aos_assert_g(!self->mf->add_range((aos_rig_t *)self, 
					start, end, weight), cl);
		range->mf->put((aos_field_t *)range);
		range = 0;

		aos_assert_g(self->mf->integrity_check(rvg) == 1, cl);
		range = (aos_field_t *)ranges->mf->next_sibling(ranges);
	}

	ranges->mf->put((aos_field_t *)ranges);
	return 0;

cl:
	ranges->mf->put((aos_field_t *)ranges);
	return -1;
}


static int aos_rig_basic_integrity_check(
		struct aos_rvg *rvg)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(rvg->type == eAosRvg_RigBasic, -1);
	aos_assert_r(aos_rvg_integrity_check(rvg) == 1, -1);

	aos_rig_basic_t *self = (aos_rig_basic_t *)rvg;
	aos_assert_r((int)self->index == (int)self->index_array.buffer, -1);
	aos_assert_r((int)self->ranges == (int)self->ranges_array.buffer, -1);
	aos_assert_r(self->index_array.mf->integrity_check(
				&self->index_array, (char **)&self->index) == 1, -1);
	aos_assert_r(self->ranges_array.mf->integrity_check(
				&self->ranges_array, 
				(char **)&self->ranges) == 1, -1);
	aos_assert_r(self->ref_count >= 0, -1);

	return 1;
}


int aos_rig_basic_next_int(aos_rig_t *rig, int *value)
{
	aos_assert_r(rig, -1);
	aos_assert_r(rig->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t *)rig;

	int idx1 = aos_next_int(0, self->index_array.noe-1);
	int idx2 = self->index[idx1];
	aos_assert_r(idx2 >= 0 && idx2 < self->ranges_array.noe, -1);
	*value = aos_next_int(self->ranges[idx2]->start, 
			self->ranges[idx2]->end);
	return 0;
}


static int aos_rig_basic_add_range(
		aos_rig_t *rig, 
		const int start, 
		const int end, 
		const int weight)
{
	int i;
	aos_assert_r(rig, -1);
	aos_assert_rm(weight > 0 && weight <= AOS_RIG_BASIC_MAX_WEIGHT, 
			-1, "Weight = %d", weight);
	aos_assert_r(start <= end, -1);
	aos_assert_r(rig->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t *)rig;

	aos_assert_r(!self->index_array.mf->add_element(
			&self->index_array, weight, (char **)&self->index), -1);
	aos_assert_r(!self->ranges_array.mf->add_element(
			&self->ranges_array, 1, (char **)&self->ranges), -1);
	
	aos_rig_basic_range_t *entry = aos_malloc(sizeof(*entry));
	entry->start = start;
	entry->end = end;
	entry->weight = weight;
	int idx = self->ranges_array.noe++;
	self->ranges[idx] = entry;

	for (i=0; i<weight; i++)
	{
		self->index[self->index_array.noe++] = idx;
	}

	return 0;
}


// 
// Description
// This function is similar to the above one except that it stores
// the value in 'value'.
//
static int aos_rig_basic_next_value(
		struct aos_rvg *rvg, 
		struct aos_value *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t *)rvg;

	int vv;
   	aos_assert_r(!self->mf->next_int((aos_rig_t *)self, &vv), -1);
	aos_assert_r(!value->mf->set_int(value, vv), -1);
	return 0;
}


static int aos_rig_basic_release_memory(aos_rvg_t *rig)
{
	aos_assert_r(!aos_rvg_release_memory(rig), -1);
	aos_assert_r(rig->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t *)rig;

	int i;
	for (i=0; i<self->ranges_array.noe; i++)
	{
		aos_free(self->ranges[i]);
	}

	aos_assert_r(!self->ranges_array.mf->release_memory(
			&self->ranges_array), -1);
	aos_assert_r(!self->index_array.mf->release_memory(
			&self->index_array), -1);

	return 0;
}


static int aos_rig_basic_destroy(aos_rvg_t *rig)
{
	aos_assert_r(!aos_rig_basic_release_memory(rig), -1);
	aos_free(rig);
	return 0;
}


// 
// Description:
// It checks whether 'value' is in this domain, or is in one of the
// ranges defined by this class. If the value is in this rvg, 
// it returns 1. Otherwise, it returns 0. On errors, it returns -1.
//
static int aos_rig_basic_domain_check(
		aos_rvg_t *rig,
		aos_value_t *value)
{
	aos_assert_r(rig, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rig->type == eAosRvg_RigBasic, -1);
	aos_rig_basic_t *self = (aos_rig_basic_t*)rig;

	int i;
	int vv;
	aos_assert_r(!value->mf->to_int(value, &vv), -1);
	for (i=0; i<self->ranges_array.noe; i++)
	{
		if (vv >= self->ranges[i]->start && 
			vv <= self->ranges[i]->end)
		{
			return 1;
		}
	}

	return 0;
}


static aos_rig_basic_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rig_basic_integrity_check,
	aos_rig_basic_next_value,
	aos_rig_basic_domain_check,
	aos_rig_basic_serialize,
	aos_rig_basic_deserialize, 
	aos_rig_basic_release_memory,
	aos_rig_basic_destroy,
	aos_rig_reset_unique, 
	aos_rig_basic_next_int, 
	aos_rig_basic_add_range
};


int aos_rig_basic_init(aos_rig_basic_t *rig)
{
	aos_assert_r(rig, -1);
	memset(rig, 0, sizeof(aos_rig_basic_t));
	aos_assert_r(!aos_rvg_init((aos_rvg_t *)rig), -1);
	rig->type = eAosRvg_RigBasic;

	rig->mf = &sg_mf;
	aos_assert_r(!aos_dyn_array_init(&rig->index_array, 
			(char **)&rig->index, sizeof(int *), 10, 
			AOS_RIG_BASIC_MAX_RANGES), -1);
	aos_assert_r(!aos_dyn_array_init(&rig->ranges_array, 
			(char **)&rig->ranges, 4, 3, 
			AOS_RIG_BASIC_MAX_RANGES), -1);

	return 0;
}


aos_rig_basic_t *aos_rig_basic_create()
{
	aos_rig_basic_t *obj = aos_malloc(sizeof(aos_rig_basic_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rig_basic_init(obj), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


// 
// Description
// It creates an aos_rig_t based on the configuration 'node'.
// The caller must 'hold' the object. 
//
aos_rig_basic_t *aos_rig_basic_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);

	aos_rig_basic_t *obj = aos_malloc(sizeof(aos_rig_basic_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rig_basic_init(obj), cleanup);

	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

