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
// This class randomly generates a string by randomly selecting a value
// from a value list:
//       [value1, weight1], 
//       [value2, weight2], 
//       ..., 
//       [valuen, weightn]
// The weights control how often a value is selected. 
//
// [min, max] It limits how many values it will pick each time: it will
//       pick at least 'min' number of values and at most 'max' number
//       of values.
// unique: If multiple values are allowed, this attribute indicates
//       whether values can be repeated (unique == 0) or not (unique == 1).
// noe_selector: If multiple values are allowed, this RIG determines
//       how many values to select. 
// selector: This is an RIG that randomly select a value.
// order: If order == 1 and if multiple values are selected, the values
//       must appear in the same order as they are defined by this class.
// separator: If specified and if multiple values are selected, it 
//       defines how to generate 'separators' in between two values.
//       This is a RSG.
// lead_space: If specified, it defines how to generate leading spaces.
//       This is an RSG.
// trail_space: If specified, it defines how to generate trailing
//       spaces. This is an RSG.
//
// Modification History:
// 03/16/2008: Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rsg_enum.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "rvg_c/rig.h"
#include "util_c/memory.h"
#include "util2/value.h"
#include "util_c/buffer.h"
#include "util_c/rc_obj.h"


#define AOS_RSG_ENUM_MAX_REPEAT 10000

// 
// Description
// This function randomly picks a number of values
// (in form of their indexes). If 'ordered' is true, it will sort them. 
//
static int aos_rsg_enum_pick_values(
		aos_rsg_enum_t *rsg, 
		aos_dyn_array_t *value_idxs, 
		int **ptr,
		const int ordered)
{
	// 1. Determine the number of values to pick
	int noe = 1;
	if (rsg->noe_selector)
	{
		aos_assert_r(!rsg->noe_selector->mf->next_int(
				rsg->noe_selector, &noe), -1);
	}

	int i;
	int idx;
	rsg->selector->mf->reset_unique(rsg->selector);
	for (i=0; i<noe; i++)
	{
		aos_assert_r(!rsg->selector->mf->next_int(rsg->selector, &idx), -1);
		if (ordered)
		{
			int j;
			for (j=0; j<value_idxs->noe; j++)
			{
				if (idx >= (*ptr)[j])
				{
					aos_assert_r(!value_idxs->mf->insert(value_idxs, 
							j, 1, (char **)ptr), -1);
					(*ptr)[j] = idx;
					value_idxs->noe++;
					break;
				}
			}

			if (j == value_idxs->noe)
			{
				aos_assert_r(value_idxs->mf->add_element(
					value_idxs, 1, (char **)ptr), -1);
				(*ptr)[value_idxs->noe++] = idx;	
			}
		}
		else
		{
			aos_assert_r(value_idxs->mf->add_element(
					value_idxs, 1, (char **)ptr), -1);
			(*ptr)[value_idxs->noe++] = idx;	
		}
	}

	return 0;
}


// 
// we use an RIG to determine which value to select, 
// this function can be implemented very simple:
// 
// Assume:
//    [value1, weight1], 
//    [value2, weight2], 
//    ...
//    [valuen, weightn]
//
// Create an RIG:
//    [0, 0, weight1], 
//    [1, 1, weigh2], 
//    ..., 
//    [n-1, n-1, weighn]
//
int aos_rsg_enum_next_value(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgEnum, -1);
	aos_rsg_enum_t *self = (aos_rsg_enum_t *)rvg;

	// 1. Generate values. The function below will generate a sequence
	//    of indexes instead of the actual values. These indexes are 
	//    used later on to assemble the final value.
	aos_dyn_array_t value_idxs;
	int *idxs;
	aos_assert_r(!aos_dyn_array_init(&value_idxs, (char **)&idxs, 
			4, 100, AOS_RSG_ENUM_MAX_REPEAT), -1);
	aos_assert_r(!aos_rsg_enum_pick_values(self, &value_idxs, 
			&idxs, self->ordered), -1);

	// 2. Assemble the final value. The final value is stored in 'buffer'. 
	
	// b. Create the leading spaces as needed.
	if (self->lead_spacer)
	{
		aos_assert_r(!self->lead_spacer->mf->next_value(
				(aos_rvg_t *)self->lead_spacer, value), -1);
	}

	// c. Put all the values into the value 
	int i, ii;
	for (i=0; i<value_idxs.noe-1; i++)
	{
		ii = idxs[i];
		aos_assert_r(ii >= 0 && ii < self->white_entries_array.noe, -1);
		aos_assert_r(!value->mf->append_str(value, 
				self->white_entries[ii]->value,
				strlen(self->white_entries[ii]->value)), -1);

		if (self->separator)
		{
			aos_assert_r(!self->separator->mf->next_value(
					(aos_rvg_t *)self->separator, value), -1);
		}
	}

	// d. Add the last value to value.
	ii = idxs[value_idxs.noe-1];
	aos_assert_r(ii >= 0 && ii < self->white_entries_array.noe, -1);
	aos_assert_r(!value->mf->append_str(value, 
				self->white_entries[ii]->value,
				strlen(self->white_entries[ii]->value)), -1);

	// e. Add the trail spaces as needed
	if (self->trail_spacer)
	{
		aos_assert_r(!self->trail_spacer->mf->next_value(
				(aos_rvg_t *)self->trail_spacer, value), -1);
	}

	return 0;
}


int aos_rsg_enum_create_incorrect_rvgs(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rsg_enum_serialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *parent)
{
	//
	// 	<EnumRSG>
	// 		<Type>
	//		<WhiteValueList>
	//			<Entry weight="weight">value</Entry>
	//			<Entry weight="weight">value</Entry>
	//			...
	//		</WhiteValueList>
	//		<BlackValueList>
	//			<Entry weight="weight">value</Entry>
	//			<Entry weight="weight">value</Entry>
	//			...
	//		</BlackValueList>
	// 	</EnumRSG>
	// 	...
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgEnum, -1);
	aos_rsg_enum_t *self = (aos_rsg_enum_t *)rvg;

	// aos_xml_node_t *node = parent->mf->append_child_node(
	// 		parent, self->name);
	aos_xml_node_t *node = aos_rsg_serialize(rvg, node);
	aos_assert_r(node, -1);

	// aos_assert_g(!aos_rsg_serialize(rvg, node), cleanup);

	// Add <WhiteValueList>
	aos_assert_g(!aos_rsg_serialize_valuelist(node, 
			self->white_entries, self->white_entries_array.noe, 
			"WhiteValueList"), cleanup);

	// Add <BlackValueList>
	aos_assert_g(!aos_rsg_serialize_valuelist(node, 
			self->black_entries, self->black_entries_array.noe, 
			"BlackValueList"), cleanup);

	node->mf->put((aos_field_t *)node);
	return 0;

cleanup:
	node->mf->put((aos_field_t *)node);
	return -1;
}


int aos_rsg_enum_deserialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *node)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgEnum, -1);
	aos_rsg_enum_t *self = (aos_rsg_enum_t *)rvg;
	aos_assert_r(!aos_rsg_deserialize(rvg, node), -1);

	// Retrieve <WhiteValueList>
	aos_assert_r(!aos_rsg_deserialize_valuelist(node, 
			&self->white_entries, &self->white_entries_array, 
			"WhiteValueList"), -1);

	// Retrieve <BlackValueList>
	aos_assert_r(!aos_rsg_deserialize_valuelist(node, 
			&self->black_entries, &self->black_entries_array, 
			"BlackValueList"), -1);

	return 0;
}


static int aos_rsg_enum_release_memory(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


static int aos_rsg_enum_destroy(aos_rvg_t *rvg)
{
	AOS_DESTROY_OBJ(rvg);
}


static int aos_rsg_enum_integrity_check(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


// 
// Description:
// It checks whether the string in 'value' can be constructed
// through this rsg. 
//
static int aos_rsg_enum_domain_check(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_enum_t *self = (aos_rsg_enum_t *)rvg;

	aos_assert_r(value->type == eAosDataType_string ||
				 value->type == eAosDataType_binary, -1);

	int i;
	for (i=0; i<self->white_entries_array.noe; i++)
	{
		if (strcmp(self->white_entries[i]->value, value->value.string) == 0) return 1;
	}

	return 0;
}


static int aos_rsg_enum_domain_check_len(
			struct aos_rsg *rsg, 
			struct aos_value *value,
			int *idx, 
			const aos_domain_check_e type)
{
	aos_assert_r(rsg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(idx, -1);
	aos_assert_r(rsg->type == eAosRvg_RsgCharset, -1);
	aos_rsg_enum_t *self = (aos_rsg_enum_t *)rsg;

	aos_assert_r(value->type == eAosDataType_string ||
				 value->type == eAosDataType_binary, -1);

	int i;
	int ii = *idx;
	aos_assert_r(ii >= 0 && ii < value->data_size, -1);
	char *data = &value->value.string[*idx];
	for (i=0; i<self->white_entries_array.noe; i++)
	{
		if (strncmp(self->white_entries[i]->value, data, 
				strlen(self->white_entries[i]->value)) == 0) 
		{
			*idx = ii + strlen(self->white_entries[i]->value);
			return 1;
		}
	}

	return 0;
}


static aos_rsg_enum_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rsg_enum_integrity_check,
	aos_rsg_enum_next_value,
	aos_rsg_enum_domain_check,
	aos_rsg_enum_serialize,
	aos_rsg_enum_deserialize,
	aos_rsg_enum_release_memory,
	aos_rsg_enum_destroy,
	aos_rsg_enum_domain_check_len,
};


int aos_rsg_enum_init(aos_rsg_enum_t *obj)
{
	aos_assert_r(obj, -1);
	memset(obj, 0, sizeof(*obj));
	obj->mf = &sg_mf;
	obj->type = eAosRvg_RsgEnum;
	return 0;
}


aos_rsg_enum_t *aos_rsg_enum_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_rsg_enum_t *obj = aos_malloc(sizeof(*obj));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_rsg_enum_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	obj->mf->hold((aos_rvg_t *)obj);
	return obj;

cleanup:
	obj->mf->release_memory((aos_rvg_t *)obj);
	aos_free(obj);
	return 0;
}	

