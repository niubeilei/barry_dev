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
// This is the super class for all RSGs. It does virtually nothing
// now. 
//   
//
// Modification History:
// 02/14/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rsg.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "rvg_c/rig.h"
#include "rvg_c/charset.h"
#include "util_c/memory.h"
#include "util_c/buffer.h"
#include <stdio.h>

aos_xml_node_t * aos_rsg_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	// 
	// 	<RSG>
	// 	</RSG>
	//
	return aos_rvg_serialize(rvg, node);
	return 0;
}


int aos_rsg_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	//
	// 	<RSG>
	// 	</RSG>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);

	aos_assert_r(!aos_rvg_deserialize(rvg, node), -1);
	return 0;
}


int aos_rsg_integrity_check(
		struct aos_rvg *rvg)
{
	aos_assert_r(rvg, -1);
	return 1;
}


// 
// Description:
// This function is shared by its subclasses in serializing 
// themselves. It serializes the either <WhiteValueList> tag
// or <BlackValueList> tag. 
//
int aos_rsg_serialize_valuelist(
		aos_xml_node_t *node,
		aos_rvg_value_entry_t **entries, 
		const int noe, 
		const char * const tagname)
{
	// Add a node
	aos_xml_node_t *vn = node->mf->append_child_node(node, tagname);
	aos_assert_r(vn, -1);	

	int i;
	aos_attr1_t* attrs[1];
	for (i=0; i<noe; i++)
	{
		aos_attr1_t *attr = aos_malloc(sizeof(aos_attr1_t));
		aos_assert_r(attr, -1);
		strcpy(attr->name, "weight");
		attr->value = aos_malloc(30);
		aos_assert_r(attr->value, -1);
		sprintf(attr->value, "%d", entries[i]->weight);
		attrs[0] = attr;
		aos_assert_g(!vn->mf->append_child_str(vn, "Entry", 
					entries[i]->value, attrs, 1), cleanup);
	}
	vn->mf->put((aos_field_t *)vn);
	return 0;

cleanup:
	vn->mf->put((aos_field_t *)vn);
	return -1;
}


// 
// Description:
// This function is shared by its subclasses in deserializing
// either the <WhiteValueList> or <BlackValueList> tags. 
//
// If the tag does not exist, it simply returns 0. 
//
int aos_rsg_deserialize_valuelist(
		aos_xml_node_t *node, 
		aos_rvg_value_entry_t ***entries,
		aos_dyn_array_t *array,
		const char * const tagname)
{
	aos_assert_r(node, -1);
	aos_assert_r(entries, -1);
	aos_assert_r(array, -1);
	aos_assert_r(tagname, -1);

	// Retrieve the tag
	aos_xml_node_t *vn = node->mf->first_named_child(node, tagname);
	if (!vn) return 0;

	aos_xml_node_t *entry = vn->mf->first_child(vn);
	aos_rvg_value_entry_t *ee = 0;
	while (entry)
	{
		int64_t weight;
		ee = aos_rvg_value_entry_create();
		aos_assert_g(ee, cleanup);
		aos_assert_g(!vn->mf->get_attr_int64((aos_field_t *)vn, "weight", &weight), cleanup);
		aos_assert_g(!vn->mf->get_str_b((aos_field_t *)vn, &ee->value), cleanup);
		ee->weight = (int)weight;
		aos_assert_g(!array->mf->add_element(array, 1, (char **)entries), cleanup);
		(*entries)[array->noe++] = ee;
		entry->mf->put((aos_field_t *)entry);
		entry = vn->mf->next_sibling(vn);
	}

	return 0;

cleanup:
	vn->mf->put((aos_field_t *)vn);
	if (ee) 
	{
		if (ee->value) aos_free(ee->value);
		aos_free(ee);
	}
	return -1;
}

/*
// 
// Description
// This function generates a random string and allocates the memory
// for the generated string. The string is returned through the
// parameter 'buffer'. The generated string length is passed back
// through 'len'.
//
// Returns
// 0 if successful
// Errors if fails.
//
int aos_rsg_next_value_bf(
		struct aos_rvg *rvg, 
		char **buffer,
		int *len)
{
	int i;
	aos_assert_r(rvg, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(len, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_Rsg, -1);
	aos_rsg_t *self = (aos_rsg_t *)rvg;

	int length = self->len_rig->mf->next_int(self->len_rig);
	aos_assert_r(length >= 0, -1);
	*buffer = aos_malloc(length+1);
	for (i=0; i<length; i++)
	{
		(*buffer)[i] = self->charset->mf->next_char(self->charset);
	}
	(*buffer)[length] = 0;
	*len = length;
	return 0;
}


// 
// Description
// This function is similar to the above one except that it stores
// the value in 'value'.
//
int aos_rsg_next_value(
		struct aos_rvg *rvg, 
		struct aos_value *value)
{
	int i;
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_Rsg, -1);
	aos_rsg_t *self = (aos_rsg_t *)rvg;

	int length = self->len_rig->mf->next_int(self->len_rig);
	aos_assert_r(length >= 0, -1);
	aos_assert_r(!value->mf->set_str(value, 0, length), -1);
	char *buff = value->memory;
	for (i=0; i<length; i++)
	{
		buff[i] = self->charset->mf->next_char(self->charset);
	}
	buff[length] = 0;
	return 0;
}
*/

