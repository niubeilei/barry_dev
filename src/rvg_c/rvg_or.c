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
// An RVG-OR is a RVG that contains one or more member rvgs. When 
// generating a new value, it will randomly select a member rvg
// and use that rvg to generate a value. How a member rvg is selected
// is controlled by an RIG. If the RIG is not specified, it uses the
// standard random number generator.
//
// Modification History:
// 03/18/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rvg_or.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "porting_c/mutex.h"
#include "random_c/random_util.h"
#include "rvg_c/rvg_xml.h"
#include "rvg_c/rsg.h"
#include "rvg_c/rig.h"
#include "util2/global_data.h"
#include "util_c/memory.h"
#include "util_c/rc_obj.h"
#include "util_c/strutil.h"
#include "util_c/buffer.h"

#define AOS_RVG_OR_MAX_RSGS 500

int aos_rvg_or_integrity_check(aos_rvg_t *rvg)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rvg_or_serialize(
		aos_rvg_t *rvg, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<A_Name>
	// 			<Type>
	// 			<RVGS>
	// 				<RVG> ... </RVG>
	// 				<RVG> ... </RVG>
	// 				...
	// 			</RVGS>
	// 			<RvgSelector> 	Optional
	// 		</A_Name>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(rvg, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;

	aos_xml_node_t *node = parent->mf->append_child_node(
			node, self->name);
	aos_assert_r(node, -1);

	// Add <RVGS>
	int i;
	for (i=0; i<self->rvgs_array.noe; i++)
	{
		aos_assert_r(!self->rvgs[i]->mf->serialize(
				(aos_rvg_t *)self->rvgs[i], node), -1);
	}


	// Add <RvgSelector>
	if (self->rvg_selector)
	{
		aos_assert_r(!self->rvg_selector->mf->serialize(
				(aos_rvg_t *)self->rvg_selector, node), -1);
	}

	node->mf->put((aos_field_t *)node);
	node = 0;
	return 0;
}


int aos_rvg_or_deserialize(
		aos_rvg_t *rvg, 
		aos_xml_node_t *node)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(node, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;

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
		aos_rvg_t *rvg = aos_rvg_factory_xml(tmp);
		aos_assert_g(rsg, cleanup);
		aos_assert_r(!self->rvgs_array.mf->add_element(
				&self->rvgs_array, 1, (char **)&self->rvgs), -1);
		self->rvgs[self->rvgs_array.noe++] = rvg;
		rsg = 0;
		tmp->mf->put((aos_field_t *)tmp);
		tmp = rn->mf->next_sibling(rn);
	}
	aos_assert_g(self->rvgs_array.noe >= 2, cleanup);

	// Retrieve <RvgSelector>
	aos_xml_node_t *rsn = node->mf->first_named_child(
			node, "RvgSelector");
	if (rsn)
	{
		if (self->rvg_selector) 
		{
			self->rvg_selector->mf->put((aos_rvg_t *)self->rvg_selector);
		}
		self->rvg_selector = (aos_rig_t *)aos_rvg_factory_xml(rsn);
		aos_assert_g(self->rvg_selector, cleanup);
		aos_assert_g(aos_is_rig_type(self->type) == 1, cleanup);
		rsn->mf->put((aos_field_t *)rsn);
	}

	rn->mf->put((aos_field_t *)rn);
	aos_assert_r(!rsg, -1);
	aos_assert_r(!tmp, -1);
	return 0;

cleanup:
	if (tmp) tmp->mf->put((aos_field_t *)tmp);
	if (rsg) rsg->mf->put((aos_rvg_t *)rsg);
	rn->mf->put((aos_field_t *)rn);
	return -1;
}


int aos_rvg_or_release_memory(aos_rvg_t *rvg) 
{
	aos_assert_r(rvg, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_assert_r(!aos_rvg_release_memory((aos_rvg_t *)rvg), -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;

	int i;
	for (i=0; i<self->rvgs_array.noe; i++)
	{
		aos_assert_r(!self->rvgs[i]->mf->put(
				(aos_rvg_t *)self->rvgs[i]), -1);
	}
	aos_assert_r(!self->rvgs_array.mf->release_memory(
			&self->rvgs_array), -1);

	if (self->rvg_selector)
	{
		self->rvg_selector->mf->put((aos_rvg_t *)self->rvg_selector);
	}
	return 0;
}


/*
int aos_rvg_or_next_value_bf(
		aos_rvg_t *rvg, 
		aos_buffer_t *buffer)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;
	
	int idx;
	if (self->rvg_selector)
	{
		aos_assert_r(!self->rvg_selector->mf->next_int(
				self->rvg_selector, &idx), -1);
		aos_assert_r(idx >= 0 && idx < self->rvgs_array.noe, -1);
	}
	else
	{
		idx = aos_next_int(0, self->rvgs_array.noe-1);
	}

	aos_assert_r(!self->rvgs[idx]->mf->next_value_bf(
			self->rvgs[idx], buffer), -1);

	return 0;
}
*/


int aos_rvg_or_next_value(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->mf->integrity_check(rvg) == 1, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;
	
	int idx;
	if (self->rvg_selector)
	{
		aos_assert_r(!self->rvg_selector->mf->next_int(
				self->rvg_selector, &idx), -1);
		aos_assert_r(idx >= 0 && idx < self->rvgs_array.noe, -1);
	}
	else
	{
		idx = aos_next_int(0, self->rvgs_array.noe-1);
	}

	aos_assert_r(!self->rvgs[idx]->mf->next_value(
			self->rvgs[idx], value), -1);

	return 0;
}


static int aos_rvg_or_destroy(aos_rvg_t *rvg)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(!aos_rvg_or_release_memory(rvg), -1);
	aos_free(rvg);
	return 0;
}


static int aos_rvg_or_domain_check(
		aos_rvg_t *rvg, 
		aos_value_t *value)
{
	aos_assert_r(rvg, -1);
	aos_assert_r(value, -1);
	aos_assert_r(rvg->type == eAosRvg_RvgOr, -1);
	aos_rvg_or_t *self = (aos_rvg_or_t *)rvg;

	int i;
	for (i=0; i<self->rvgs_array.noe; i++)
	{
		if (self->rvgs[i]->mf->domain_check(self->rvgs[i], value) == 1)
		{
			return 1;
		}
	}

	return 0;
}


static aos_rvg_or_mf_t sg_mf = 
{
	AOS_RVG_MEMFUNC_INIT,
	aos_rvg_or_integrity_check,
	aos_rvg_or_next_value,
	aos_rvg_or_domain_check,
	aos_rvg_or_serialize,
	aos_rvg_or_deserialize,
	aos_rvg_or_release_memory,
	aos_rvg_or_destroy
};


int aos_rvg_or_init(aos_rvg_or_t *rvg)
{
	aos_assert_r(rvg, -1);
	memset(rvg, 0, sizeof(*rvg));
	rvg->mf = &sg_mf;
	rvg->type = eAosRvg_RvgOr;
	aos_assert_r(!aos_dyn_array_init(&rvg->rvgs_array, 
			(char **)&rvg->rvgs, 4, 3, AOS_RVG_OR_MAX_RSGS), -1);

	return 0;
}


aos_rvg_or_t *aos_rvg_or_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_rvg_or_t *obj = aos_malloc(sizeof(*obj));
	aos_assert_g(!aos_rvg_or_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_rvg_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

