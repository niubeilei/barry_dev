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
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "action_c/action.h"

#include "action_c/send_msg.h"
#include "action_c/recv_msg.h"
#include "action_c/tcp_server.h"
#include "action_c/set_value.h"
#include "alarm_c/alarm.h"
#include "event_c/event_mgr.h"
#include "parser/xml_node.h"
#include "porting_c/mutex.h"
#include "util_c/rc_obj.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"
#include "util_c/tracer.h"
#include "util_c/global_data.h"

AOS_DECLARE_RCOBJ_LOCK;

static aos_lock_t *	sg_lock = 0;
static int			sg_init_flag = 0;
static u32 			sg_action_id = 101;


static int sg_init()
{
	aos_global_lock();
	if (sg_init_flag) 
	{
		aos_global_unlock();
		return 0;
	}

	sg_lock = aos_malloc(sizeof(*sg_lock));
	aos_init_lock(sg_lock);
	sg_init_flag = 1;
	aos_global_unlock();
	return 0;
}


// 
// Description
// It serializes its part. This function should be called
// by its derived class. 
//
int aos_action_serialize(
		aos_action_t *action, 
		aos_xml_node_t *node)
{
	// 
	// 	<Action>
	// 		<Type>
	// 		<Name>			optional
	// 		<IsBlocking>	default: 1
	// 		...
	// 	</Action>
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);

	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, "Type", 
			aos_action_type_2str(action->type), 0, 0), -1);

	// Set <Name>
	if (action->name)
	{
		aos_assert_r(!node->mf->append_child_str(node, "Name", 
			action->name, 0, 0), -1);
	}

	return 0;
}


int aos_action_deserialize(
		struct aos_action *action, 
		aos_xml_node_t *node)
{
	char buff[101];
	int buff_len = 101;
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);

	// Retrieve <Type>
	buff_len = 101;
	aos_assert_r(!node->mf->first_child_str_b(node, "Type", 
			buff, &buff_len, 0), -1);
	action->type = aos_action_type_2enum(buff); 
	aos_assert_r(aos_action_type_check(action->type) == 1, -1);

	// Retrieve <Name>
	if (node->name) 
	{
aos_trace("To free name: %x:%s", node->name, node->name);
		aos_free(node->name);
	aos_assert_r(!node->mf->first_child_str_dft(node, "Name", 
			&action->name, &buff_len, 0), -1);
if (action->name)
{
aos_trace("Retrieved action name: %x:%d", action->name, action->name);
}
}

	return 0;
}


int aos_action_is_finished(aos_action_t *action)
{
	aos_assert_r(action, -1);
	return action->finished;
}


int aos_action_is_blocking(aos_action_t *action)
{
	aos_assert_r(action, -1);
	return action->blocking;
}


int aos_action_reset(aos_action_t *action)
{
	aos_assert_r(action, -1);
	action->finished = 0;
	return 0;
}


int aos_action_finish_action(aos_action_t *action)
{
	return 0;
}


int aos_action_destroy(struct aos_action *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_action_hold(aos_action_t *action)
{
	AOS_RC_OBJ_HOLD(action);
}


int aos_action_put(aos_action_t *action)
{
	AOS_RC_OBJ_PUT(action);
}


int aos_action_need_reg_event(aos_action_t *action)
{
	return 0;
}


int aos_action_release_memory(struct aos_action *action)
{
	aos_assert_r(action, -1);
	if (action->name) 
	{
		aos_free(action->name);
	}
	action->name = 0;
	return 0;
}


int aos_action_init(aos_action_t *sm)
{
	aos_assert_r(sm, -1);
	sm->magic = AOS_ACTION_MAGIC;
	AOS_INIT_LIST_HEAD(&sm->link);
	if (!sg_init_flag) sg_init();
	aos_lock(sg_lock);
	sm->action_id = sg_action_id++;
	aos_unlock(sg_lock);
	return 0;
}


int aos_action_run_actions(
		aos_action_t **actions, 
		const int noe, 
		aos_gen_data_t *data, 
		aos_event_callback_t callback, 
		void *user_data,
		u32 *reg_id)
{
	int i;
	aos_assert_r(actions, -1);
	aos_assert_r(noe >= 0, -1);

	*reg_id = 0;
	for (i=0; i<noe; i++)
	{
		aos_assert_r(!actions[i]->mf->run(actions[i], data), -1);
		if (actions[i]->mf->need_reg_event(actions[i]) == 1)
		{
			aos_assert_r(!aos_register_event(
					eAosEvent_ActionFinished, callback, 
					actions[i]->action_id, user_data, reg_id), -1);
		}
	}

	return 0;
}


int aos_action_reset_action_list(aos_list_head_t *list)
{
	aos_action_t *action, *tmp;

	aos_list_for_each_entry_safe(action, tmp, list, link)
	{
		aos_assert_r(!action->mf->reset(action), -1);
	}

	return 0;
}


// 
// Description:
// It runs all the actions from the list until an action 
// blocks the subsequent actions from running. If an action
// was already run, it skips to the next one.
//
// If all actions finished, it returns 1. Otherwise, it returns
// 0. If errors, it returns -1.
//
int aos_action_run_action_list(
		aos_list_head_t *list, 
		struct aos_gen_data *data, 
		aos_event_callback_t callback, 
		void *user_data,
		u32 *reg_id)
{
	aos_action_t *action, *tmp;

	int all_finished = 1;
	aos_list_for_each_entry_safe(action, tmp, list, link)
	{
		if (action->mf->is_finished(action) == 1)
		{
			continue;
		}

		if (action->mf->need_reg_event(action) == 1)
		{
aos_trace("To register ActionFinished event: %d", action->action_id);
			aos_assert_r(!aos_register_event(
					eAosEvent_ActionFinished, callback, 
					action->action_id, user_data, reg_id), -1);
		}
		aos_assert_r(!action->mf->run(action, data), -1);

		if (action->mf->is_finished(action) != 1) all_finished = 0;
		if (action->mf->is_blocking(action) == 1) return 0;
	}

	// 
	// All actions have been run
	//
	return all_finished;
}


aos_action_t *aos_action_factory(aos_xml_node_t *node)
{
	char type[101];
	int type_len = 101;
	aos_assert_r(node, 0);
	aos_assert_r(!node->mf->first_child_str_b(
			node, "Type", type, &type_len, 0), 0);

	if (strcmp(type, "SendMsg") == 0)
	{
		return aos_act_send_msg_create_xml(node);
	}

	if (strcmp(type, "RecvMsg") == 0)
	{
		return aos_act_recv_msg_create_xml(node);
	}

	if (strcmp(type, "TcpServer") == 0)
	{
		return aos_act_tcp_server_create_xml(node);
	}

	if (strcmp(type, "SetValue") == 0)
	{
		return (aos_action_t *)aos_act_set_value_create_xml(node);
	}

	aos_alarm("Unrecognized type: %s", type);
	return 0;
}

