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
// A AND-RVG is a composite RVG. It contains two or more member RVGs.
// When generating a value, it will use the member RVGs to generate
// the values and the final value is assembled by concatenate 
// them all. This means that only RSGs can be the member RVGs.
//   
//
// Modification History:
// 03/17/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rvg_and.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "porting_c/mutex.h"
#include "rvg_c/rvg_xml.h"
#include "rvg_c/rsg.h"
#include "rvg_c/rig.h"
#include "util2/global_data.h"
#include "util_c/memory.h"
#include "util_c/rc_obj.h"
#include "util_c/strutil.h"
#include "util_c/buffer.h"

#define AOS_ANDRVG_MAX_RSGS 500

int aos_rvg_and_integrity_check(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rvg_and_serialize(
		aos_rvg_t *rvg, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<A_Name>
	// 			<RVGS>
	// 				<RVG> ... </RVG>
	// 				<RVG> ... </RVG>
	// 				...
	// 			</RVGS>
	// 		</A_Name>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;

	aos_xml_node_t *node = parent->mf->append_child_node(
			node, self->name);
	aos_assert_r(node, -1);

	// Add <RVGS>
	int i;
	for (i=0; i<self->rsgs_array.noe; i++)
	{
		aos_assert_r(!self->rsgs[i]->mf->serialize(
				(aos_rvg_t *)self->rsgs[i], node), -1);
	}

	return 0;
}


// 
// Currently not doing anything.
//
int aos_rvg_and_deserialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *node)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;

	// Retrieve Name
	if (self->name) aos_free(self->name);
	aos_assert_r(!aos_str_set(&self->name, node->name, 
			strlen(node->name)), -1);

	// Retrieve <RVGS>
	aos_xml_node_t *rn = node->mf->first_named_child(node, "RVGS");
	aos_assert_r(rn, -1);
	aos_xml_node_t *tmp = rn->mf->first_child(rn);
	aos_rsg_t *rsg = 0; 
	while (tmp)
	{	
		aos_rsg_t *rsg = (aos_rsg_t *)aos_rvg_factory_xml(tmp);
		aos_assert_g(rsg, cleanup);
		aos_assert_g(aos_is_rsg_type(rsg->type) == 1, cleanup);
		aos_assert_r(!self->rsgs_array.mf->add_element(
				&self->rsgs_array, 1, (char **)&self->rsgs), -1);
		self->rsgs[self->rsgs_array.noe++] = rsg;
		rsg = 0;
		tmp = rn->mf->next_sibling(rn);
	}

	aos_assert_r(self->rsgs_array.noe >= 2, -1);
	return 0;

cleanup:
	if (tmp) tmp->mf->put((aos_field_t *)tmp);
	if (rsg) rsg->mf->put((aos_rvg_t *)rsg);
	rn->mf->put((aos_field_t *)rn);
	return -1;
}


int aos_rvg_and_release_memory(aos_rvg_t *rvg) 
{
	aos_assert_r(rvg, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_assert_r(!aos_rvg_release_memory((aos_rvg_t *)rvg), -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;

	int i;
	for (i=0; i<self->rsgs_array.noe; i++)
	{
		aos_assert_r(!self->rsgs[i]->mf->put(
				(aos_rvg_t *)self->rsgs[i]), -1);
	}
	aos_assert_r(!self->rsgs_array.mf->release_memory(
			&self->rsgs_array), -1);
	return 0;
}


/*
int aos_rvg_and_next_value_bf(
		aos_rvg_t *rvg, 
		aos_buffer_t *buffer)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;
	
	int i;
	for (i=0; i<self->rsgs_array.noe; i++)
	{
		aos_assert_r(!self->rsgs[i]->mf->next_value_bf(
				(aos_rvg_t *)self->rsgs[i], buffer), -1);
	}

	return 0;
}
*/


int aos_rvg_and_next_value(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;
	
	int i;
	for (i=0; i<self->rsgs_array.noe; i++)
	{
		aos_assert_r(!self->rsgs[i]->mf->next_value(
				(aos_rvg_t *)self->rsgs[i], value), -1);
	}

	return 0;
}


static int aos_rvg_and_destroy(aos_rvg_t *rvg)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(!aos_rvg_and_release_memory(rvg), -1);
	aos_free(rvg);
	return 0;
}


static int aos_rvg_and_domain_check(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgAnd, -1);
	aos_rvg_and_t *self = (aos_rvg_and_t *)rvg;
	
	int i;
	int idx = 0;
	for (i=0; i<self->rsgs_array.noe; i++)
	{
		if (self->rsgs[i]->mf->domain_check_len(self->rsgs[i], 
				value, &idx, self->domain_check_type) != 1)
		{
			return 0;
		}
	}

	if (idx == value->data_size) return 1;

	return 0;
}


static aos_rvg_and_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rvg_and_integrity_check,
	aos_rvg_and_next_value,
	aos_rvg_and_domain_check,
	aos_rvg_and_serialize,
	aos_rvg_and_deserialize,
	aos_rvg_and_release_memory,
	aos_rvg_and_destroy
};


int aos_rvg_and_init(aos_rvg_and_t *rvg)
{
	aos_assert_r(rvg, -1);
	memset(rvg, 0, sizeof(*rvg));
	rvg->mf = &sg_mf;
	rvg->type = eAosRvg_RvgAnd;
	aos_assert_r(!aos_dyn_array_init(&rvg->rsgs_array, 
			(char **)&rvg->rsgs, 4, 3, AOS_ANDRVG_MAX_RSGS), -1);

	return 0;
}


aos_rvg_and_t *aos_rvg_and_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_rvg_and_t *obj = aos_malloc(sizeof(*obj));
	aos_assert_g(!aos_rvg_and_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

