////////////////////////////////////////////////////////////////////////////
//
// Copyrig_uniqueht (C) 2005
// Packet Engineering, Inc. All rig_uniquehts reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This class randomly generates integers that are guaranteed to be
// unique. Before starting a unique sequence, one should call the 
// member function "reset_unique(...)". 
//
// The algorithm to guarantee uniqueness is:
// 1. When "reset_unique(..)" function is called, it resets
//    the crt_weight to the rig's total weight. 
//
// 2. When generating a new value, it randomly generate an integer
//    in the range [0, crt_weight] Using this integer, it can 
//    determine which range to use, i.e., the last range in 
//    ranges[...] whose accumulated weight is <= the integer. 
//    It then uses the range to generate the integer value. 
//    The range guarantees it will generate unique values.
//
// 3. When a range has generated all its possible values for the
//    current unique cycle, its weight is taken out of the 
//    crt_weight so that the range will not be used in the 
//    next call to generate values. 
//
// IMPORTANT NODES:
// This algorithm is good when the total number of ranges is not
// too many (such as less than 50). Otherwise, it should use an 
// algorithm that is good for many ranges. 
//
// Modification History:
// 03/16/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rig_unique.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "rvg_c/rig_uniquerange.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include <stdio.h>

#define AOS_RIG_UNIQUE_MAX_RANGES 1000

int aos_rig_unique_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<A_Name>
	// 			<Ranges>
	// 				<Range>min, max, weight</Range>
	// 				<Range>min, max, weight</Range>
	// 				...
	// 		</A_Name>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rvg;

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, self->name);
	aos_assert_r(node, -1);

	aos_assert_r(!aos_rvg_serialize(rvg, node), -1);
	
	// Add "<Ranges>"
	aos_xml_node_t *ranges = node->mf->append_child_node(node, "Ranges")    ;
	aos_assert_r(ranges, -1);
	
	int i;
	char buff[100];
	for (i=0; i<self->ranges_array.noe; i++)
	{
		// Add <Range>
		sprintf(buff, "%d, %d, %d", self->ranges[i]->start,
				self->ranges[i]->end, self->ranges[i]->weight);
		aos_assert_r(!ranges->mf->append_child_str(
		        ranges, "Range", buff, 0, 0), -1);
	}
	
	return 0;
}


int aos_rig_unique_add_range(
		aos_rig_t *rig, 
		const int start, 
		const int end, 
		const int weight)
{
	aos_assert_r(rig, -1);
	aos_assert_r(rig->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rig;
	aos_rig_uniquerange_t *range = aos_rig_uniquerange_create(
			start, end, weight);
	aos_assert_r(range, -1);

	aos_assert_g(!self->ranges_array.mf->add_element(
			&self->ranges_array, 1, (char **)&self->ranges), cleanup);
	self->ranges[self->ranges_array.noe++] = range;

	self->total_weight += weight;
	self->first_full_range = self->ranges_array.noe;
	return 0;

cleanup:
	aos_free(range);
	return -1;
}


int aos_rig_unique_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(rvg->type == eAosRvg_RigUnique, -1);
	aos_assert_r(!aos_rig_deserialize(rvg, node), -1);
	return 0;
}


int aos_rig_unique_integrity_check(
		struct aos_rvg *rvg)
{
	aos_assert_r(aos_rig_integrity_check(rvg) == 1, -1);
	return 1;
}


int aos_rig_unique_reset_unique(aos_rig_t *rig)
{
	aos_assert_r(rig, -1);
	aos_assert_r(rig->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rig;

	self->crt_weight = self->total_weight;
	self->first_full_range = self->ranges_array.noe;

	int i;
	for (i=0; i<self->ranges_array.noe; i++)
	{
		aos_assert_r(self->ranges[i]->mf->reset_unique(self->ranges[i]), -1);
	}

	return 0;
}


int aos_rig_unique_next_int(
		aos_rig_t *rig, 
		int *value)
{
	aos_assert_r(rig, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rig->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rig;
	aos_assert_r(self->crt_weight > 0, -1);
	aos_assert_r(self->first_full_range > 0, -1);

	// Randomly generate an integer
	int weight = aos_next_int(0, self->crt_weight);

	// Determine the range 
	int accu_weights = 0;
	int i;
	for (i=0; i<self->ranges_array.noe; i++)
	{
		accu_weights += self->ranges[i]->weight;
		if (weight <= accu_weights) break;
	}
	aos_assert_r(i < self->ranges_array.noe, -1);
	int range_found = i;
	aos_rig_uniquerange_t *range = self->ranges[range_found];

	// Use the range to generate a value
	aos_assert_r(!range->mf->next_int(range, value), -1);

	if (range->mf->is_full(range) != 1) return 0;

	// The range has generated all its possible values. Time to 
	// take it out of the arrays.
	self->crt_weight -= range->weight;

	// Swap the ranges[...]
	self->ranges[range_found] = self->ranges[self->first_full_range-1];
	self->ranges[self->first_full_range-1] = range;
	self->first_full_range--;

	return 0;
}


/*
int aos_rig_unique_add_range(
		aos_rig_unique_t *rig_unique, 
		const int min, 
		const int max, 
		const int weight)
{
	int i;
	aos_assert_r(rig_unique, -1);
	aos_assert_rm(weight > 0 && weight <= AOS_rig_unique_MAX_WEIGHT, 
			-1, "Weight = %d", weight);
	aos_assert_r(min <= max, -1);

	aos_assert_r(!rig_unique->index_array.mf->add_element(
			&rig_unique->index_array, weight, (char **)&rig_unique->index), -1);
	aos_assert_r(!rig_unique->ranges_array.mf->add_element(
			&rig_unique->ranges_array, 1, (char **)&rig_unique->ranges), -1);
	
	aos_rig_unique_range_t *entry = aos_malloc(sizeof(*entry));
	entry->min = min;
	entry->max = max;
	entry->weight = weight;
	int idx = rig_unique->ranges_array.noe++;
	rig_unique->ranges[idx] = entry;

	for (i=0; i<weight; i++)
	{
		rig_unique->index[rig_unique->index_array.noe++] = idx;
	}

	return 0;
}
*/


/*
// 
// Description
// This function generates a random digit string and allocates the memory
// for the generated string. The string is returned through the
// parameter 'buffer'. The generated string length is passed back
// through 'len'.
//
// Returns
// 0 if successful
// Errors if fails.
//
int aos_rig_unique_next_value_bf(
		struct aos_rvg *rvg, 
		aos_buffer_t *buffer)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rvg;

	int value;
   	aos_assert_r(!self->mf->next_int((aos_rig_t *)self, &value), -1);
	aos_assert_r(!buffer->mf->append_int(buffer, value), -1);
	return 0;
}
*/


// 
// Description
// This function is similar to the above one except that it stores
// the value in 'value'.
//
int aos_rig_unique_next_value(
		struct aos_rvg *rvg, 
		struct aos_value *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rvg;

	int vv;
   	aos_assert_r(!self->mf->next_int((aos_rig_t *)rvg, &vv), -1);
	aos_assert_r(!value->mf->set_int(value, vv), -1);
	return 0;
}


int aos_rig_unique_release_memory(aos_rvg_t *rig)
{
	aos_assert_r(rig, -1);
	aos_assert_r(rig->type == eAosRvg_RigUnique, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t *)rig;

	int i;
	for (i=0; i<self->ranges_array.noe; i++)
	{
		aos_assert_r(!self->ranges[i]->mf->destroy(self->ranges[i]), -1);
	}

	aos_assert_r(self->ranges_array.mf->release_memory(
			&self->ranges_array), -1);
	return 0;
}


int aos_rig_unique_destroy(aos_rvg_t *rig)
{
	aos_assert_r(rig, -1);
	aos_assert_r(!rig->mf->release_memory(rig), -1);
	aos_free(rig);
	return 0;
}


// 
// Description:
// It checks whether 'value' is in this domain, or is in one of the
// ranges defined by this class. If the value is in this rvg, 
// it returns 1. Otherwise, it returns 0. On errors, it returns -1.
//
static int aos_rig_unique_domain_check(
		aos_rvg_t *rig,
		aos_value_t *value)
{
	aos_assert_r(rig, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rig->type == eAosRvg_RigBasic, -1);
	aos_rig_unique_t *self = (aos_rig_unique_t*)rig;

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


static aos_rig_unique_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rig_unique_integrity_check,
	aos_rig_unique_next_value,
	aos_rig_unique_domain_check,
	aos_rig_unique_serialize,
	aos_rig_unique_deserialize, 
	aos_rig_unique_release_memory,
	aos_rig_unique_destroy,
	aos_rig_unique_reset_unique, 
	aos_rig_unique_next_int, 
	aos_rig_unique_add_range
};


int aos_rig_unique_init(aos_rig_unique_t *rig)
{
	aos_assert_r(rig, -1);
	memset(rig, 0, sizeof(aos_rig_unique_t));
	aos_assert_r(!aos_rvg_init((aos_rvg_t *)rig), -1);
	rig->type = eAosRvg_RigUnique;

	rig->mf = &sg_mf;
	aos_assert_r(!aos_dyn_array_init(&rig->ranges_array, 
			(char **)&rig->ranges, 4, 10, 
			AOS_RIG_UNIQUE_MAX_RANGES), -1);

	return 0;
}


// 
// Description
// It creates an aos_rig_unique_t based on the configuration 'node'.
// The caller must 'hold' the object. 
//
aos_rig_unique_t *aos_rig_unique_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);

	aos_rig_unique_t *obj = aos_malloc(sizeof(aos_rig_unique_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rig_unique_init(obj), cleanup);

	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

