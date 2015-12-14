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
// 03/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "action_c/set_value.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "parser/attr.h"
#include "statemachine/statemachine.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"

extern aos_xml_node_t *tmpding;

int aos_act_set_value_serialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// 	<Action>
	// 		<Name>
	// 		<Type>
	// 		<LHS scope=xxx, obj_type=xxx>
	// 		<RHS scope=xxx, obj_type=xxx>
	// 	</Action>
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(node, -1);
	aos_assert_r(action->type == eAosActionType_SetValue, -1);
	aos_act_set_value_t *self = (aos_act_set_value_t *)action;
	aos_assert_r(!aos_action_serialize(action, node), -1);

	// Set <LHS>
	aos_attr1_t attrs[2];
	aos_attr_init(&attrs[0]);
	aos_attr_init(&attrs[1]);

	aos_attr_set(&attrs[0], "scope", aos_scope_2str(self->lhs_scope));
	aos_attr_set(&attrs[1], "obj_type", aos_obj_type_2str(self->lhs_type));

	aos_assert_r(!node->mf->append_child_str(node, "LHS", 
				aos_gen_data_tag_2str(self->lhs_tag), attrs, 2), -1);

	// Set <RHS>
	aos_attr_set(&attrs[0], "scope", aos_scope_2str(self->rhs_scope));
	aos_attr_set(&attrs[1], "obj_type", aos_obj_type_2str(self->rhs_type));

	aos_assert_r(!node->mf->append_child_str(node, "RHS", 
				aos_gen_data_tag_2str(self->rhs_tag), attrs, 2), -1);
	
	return 0;
}


int aos_act_set_value_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(action->type == eAosActionType_SetValue, -1);
	aos_act_set_value_t *self = (aos_act_set_value_t *)action;

	aos_assert_r(!aos_action_deserialize(action, node), -1);

	// Retrieve <LHS>
	char buff[101];
	int buff_len = 101;
	aos_xml_node_t *child;
	aos_assert_r(!node->mf->first_child_str_b(node, "LHS", 
				buff, &buff_len, &child), -1);
	self->lhs_tag = aos_gen_data_tag_2enum(buff);
	aos_assert_r(aos_gen_data_tag_check(self->lhs_tag) == 1, -1);
	aos_attr1_t *attrs = child->attr;
	int num_attrs = child->num_attr;

	int i;
	for (i=0; i<num_attrs; i++)
	{
		if (strcmp(attrs[i].name, "scope") == 0)
		{
			self->lhs_scope = aos_scope_2enum(attrs[i].value);
			aos_assert_r(aos_scope_check(self->lhs_scope) == 1, -1);
		}
		else if (strcmp(attrs[i].name, "obj_type") == 0)
		{
			self->lhs_type = aos_obj_type_2enum(attrs[i].value);
			aos_assert_r(aos_obj_type_check(self->lhs_type) == 1, -1);
		}
	}
	child->mf->put((aos_field_t *)child);

	aos_assert_r(aos_scope_check(self->lhs_scope) == 1, -1);
	aos_assert_r(aos_obj_type_check(self->lhs_type) == 1, -1);

	// Retrieve <RHS>
	buff_len = 101;
	aos_assert_r(!node->mf->first_child_str_b(node, "RHS", 
				buff, &buff_len, &child), -1);
	self->rhs_tag = aos_gen_data_tag_2enum(buff);
	aos_assert_r(aos_gen_data_tag_check(self->rhs_tag) == 1, -1);
	attrs = child->attr;
	num_attrs = child->num_attr;

	for (i=0; i<num_attrs; i++)
	{
		if (strcmp(attrs[i].name, "scope") == 0)
		{
			self->rhs_scope = aos_scope_2enum(attrs[i].value);
			aos_assert_r(aos_scope_check(self->rhs_scope) == 1, -1);
		}
		else if (strcmp(attrs[i].name, "obj_type") == 0)
		{
			self->rhs_type = aos_obj_type_2enum(attrs[i].value);
			aos_assert_r(aos_obj_type_check(self->rhs_type) == 1, -1);
		}
	}
	child->mf->put((aos_field_t *)child);

	aos_assert_r(aos_scope_check(self->rhs_scope) == 1, -1);
	aos_assert_r(aos_obj_type_check(self->rhs_type) == 1, -1);

	return 0;
}


int aos_act_set_value_destroy(aos_action_t *action)
{
	aos_assert_r(action, -1);

	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_act_set_value_release_memory(struct aos_action *action)
{
	aos_assert_r(!aos_action_release_memory(action), -1);
	aos_assert_r(action->type == eAosActionType_SetValue, -1);
	return 0;
}


static int aos_get_value(
		aos_act_set_value_t *action,
		const aos_scope_e scope, 
		const aos_obj_type_e type, 
		const aos_gen_data_tag_e tag, 
		aos_gen_data_t *data,
		char **value, 
		int *alloc_mem)
{
	aos_assert_r(action, -1);
	aos_assert_r(tag, -1);
	aos_assert_r(value, -1);
	aos_assert_r(alloc_mem, -1);

	*alloc_mem = 0;
	switch (scope)
	{
	case eAosScope_CrtStmch:
		 {
		 aos_statemachine_t *sm = 0;
		 aos_value_t *vv = 0;
		 aos_assert_r(!data->mf->get_value(data, 
					 eAosGenData_CrtStmch, &vv), -1);
		 aos_assert_r(!vv->mf->to_ptr(vv, (void **)&sm), -1);
		 aos_assert_r(sm, -1);

		 switch (type)
		 {
		 case eAosObjType_StmchData:
		 	  aos_assert_r(!sm->mf->get_var_str(sm, tag, value), -1);
		 	  *alloc_mem = 0;
		 	  return 0;

		 default:
		 	  aos_alarm("Unrecognized type: %s", aos_obj_type_2str(type));
		 	  return -1;
		 }
		 }
		 break;

	default:
		 aos_alarm("Unrecognized scope: %s", aos_scope_2str(scope));
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_act_set_value_run(
		aos_action_t *action, 
		aos_gen_data_t *data)
{
	aos_assert_r(action, -1);
	aos_assert_r(data,-1);
	aos_assert_r(action->type == eAosActionType_SetValue, -1);
	aos_act_set_value_t *self = (aos_act_set_value_t *)action;

	char *lhs_value = 0;

	// Get the lhs value
	int alloc_mem = 0;
	aos_assert_r(!aos_get_value(self, 
			self->lhs_scope, self->lhs_type, 
			self->lhs_tag, data, &lhs_value, &alloc_mem), -1);

	aos_statemachine_t *sm = 0;
	switch (self->rhs_scope)
	{
	case eAosScope_CrtStmch:
		 {
		 aos_value_t *vv;
		 aos_assert_g(!data->mf->get_value(data, 
					 eAosGenData_CrtStmch, &vv), cleanup);
		 aos_assert_g(!vv->mf->to_ptr(vv, (void **)&sm), cleanup);
		 }
		 break;

	default:
		 aos_alarm("Unrecognized scope: %s", aos_scope_2str(self->rhs_scope));
		 goto cleanup;
	}

	switch (self->rhs_type)
	{
	case eAosObjType_StmchData:
		 aos_assert_r(sm, -1);
		 aos_assert_g(!sm->mf->add_var(sm, 
				self->rhs_tag, lhs_value, 0), cleanup);
		 break;

	default:
		 aos_alarm("Unrecognized type: %s", 
				 aos_obj_type_2str(self->rhs_type));
		 goto cleanup;
	}

	if (alloc_mem) aos_free(lhs_value);
	self->finished = 1;
	return 0;

cleanup:
	if (alloc_mem) aos_free(lhs_value);
	self->finished = 1;
	return -1;
}

static aos_act_set_value_mf_t sg_mf = 
{
	aos_action_hold,
	aos_action_put,
	aos_act_set_value_serialize,
	aos_act_set_value_deserialize,
	aos_act_set_value_destroy,
	aos_act_set_value_release_memory,
	aos_act_set_value_run,
	aos_action_is_finished,
	aos_action_finish_action,
};

int aos_act_set_value_init(aos_act_set_value_t *action)
{
	aos_assert_r(action, -1);
	memset(action, 0, sizeof(aos_act_set_value_t));
	action->mf = &sg_mf;
	action->type = eAosActionType_SetValue;
	aos_assert_r(!aos_action_init((aos_action_t *)action), -1);
	return 0;
}


aos_act_set_value_t *aos_act_set_value_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_act_set_value_t *obj = aos_malloc(sizeof(aos_act_set_value_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_act_set_value_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_action_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

