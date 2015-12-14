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
// 02/12/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "conditions_c/cond.h"

#include "alarm_c/alarm.h"
#include "conditions_c/const.h"
#include "parser/xml_node.h"
#include "util_c/memory.h"


int aos_cond_const_serialize(
		aos_cond_t *cond, 
		aos_xml_node_t *parent)
{
	// 
	//	<Cond>
	//		<Type>
	//		...
	//		<Status>0/1</Status>
	//	</Cond>
	//
	aos_assert_r(cond, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(cond->type == eAosCondType_Const, -1);
	aos_cond_const_t *self = (aos_cond_const_t *)cond;

	aos_xml_node_t *node = parent->mf->append_child_node(parent, "Cond");
	aos_assert_r(node, -1);

	aos_assert_r(!aos_cond_serialize(cond, node), -1);

	// Set <Status>
	aos_assert_r(!node->mf->append_child_int(node, "Status", self->status), -1);

	return 0;
}


int aos_cond_const_deserialize(
		aos_cond_t *cond, 
		aos_xml_node_t *node)
{
	aos_assert_r(cond, -1);
	aos_assert_r(node, -1);
	aos_assert_r(cond->type == eAosCondType_Const, -1);
	aos_cond_const_t *self = (aos_cond_const_t *)cond;

	aos_assert_r(!aos_cond_deserialize(cond, node), -1);

	// Retrieve <Status>
	aos_assert_r(!node->mf->first_child_int(node, "Status", &self->status), -1);

	return 0;
}


int aos_cond_const_release_memory(aos_cond_t *cond)
{
	aos_assert_r(!aos_cond_release_memory(cond), -1);
	return 0;
}


int aos_cond_const_destroy(aos_cond_t *cond)
{
	aos_assert_r(!aos_cond_const_release_memory(cond), -1);
	aos_free(cond);
	return 0;
}


int aos_cond_const_evaluate(
		struct aos_cond *cond, 
		struct aos_gen_data *data)
{
	aos_assert_r(cond, -1);
	aos_assert_r(cond->type == eAosCondType_Const, -1);
	aos_cond_const_t *self = (aos_cond_const_t *)cond;
	return self->status;
}


int aos_cond_const_evaluate_by_event(
		struct aos_cond *cond, 
		struct aos_event *event, 
		struct aos_state *state, 
		struct aos_gen_data *data)
{
	aos_assert_r(cond, -1);
	aos_assert_r(cond->type == eAosCondType_Const, -1);
	aos_cond_const_t *self = (aos_cond_const_t *)cond;
	return self->status;
}


static aos_cond_const_mf_t sg_mf = 
{
	aos_cond_hold,
	aos_cond_put,
	aos_cond_const_serialize,
	aos_cond_const_deserialize,
	aos_cond_const_destroy,
	aos_cond_const_release_memory,
	aos_cond_const_evaluate,
	aos_cond_const_evaluate_by_event
};


int aos_cond_const_init(aos_cond_const_t *obj)
{
	aos_assert_r(obj, -1);
	memset(obj, 0, sizeof(aos_cond_const_t));
	obj->mf = &sg_mf;
	obj->type = eAosCondType_Const;

	aos_assert_r(!aos_cond_init((aos_cond_t *)obj), -1);
	return 0;
}


aos_cond_t *aos_cond_const_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_cond_const_t *obj = aos_malloc(sizeof(aos_cond_const_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_cond_const_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_cond_t *)obj, node), cleanup);
	return (aos_cond_t *)obj;

cleanup:
	aos_free(obj);
	return 0;
}

