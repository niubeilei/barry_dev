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
// "aos_state_msg" is a subclass of "aos_state". This state is 
// message oriented. When entering the state, it may send a message
// out. In the state, it may receive messages. Based on the type of
// messages received, it may leave the state to another or just
// stay in the state. It may start timers to control how soon it 
// should receive events. 
//
// Messages to Send:
// start_msg
// This message is sent when the state starts.
//
// reentrer_msg 
// This message is sent when the state is re-entered.
//
// Modification History:
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/state_msg.h"

#if 0
static int aos_state_msg_serialize(
		aos_state_t *state, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<state_name>
	// 			<Type>
	// 			...
	// 		</state_name>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(state, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(state->type == eAosState_Msg, -1);
	aos_state_mst_t *self = (aos_state_msg_t *)state;

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, state->name);
	aos_assert_r(node, -1);

	aos_assert_r(!aos_state_serialize(state, node), -1);

	return 0;
}


static int aos_state_msg_deserialize(
		aos_state_msg_t *state, 
		aos_xml_node_t *node)
{
	// 
	// 	<Parent>
	// 		...
	// 		<state_name>
	// 			<Type>
	// 			...
	// 		</state_name>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(state, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(state->type == eAosState_Msg, -1);
	aos_state_mst_t *self = (aos_state_msg_t *)state;

	aos_assert_r(!aos_state_deserialize(state, node), -1);

	return 0;
}

static int aos_state_msg_destroy(
		aos_state_t *state)
{
	aos_assert_r(state, -1);
	aos_assert_r(!aos_state_destroy(state), -1);
	aos_state_msg_t *self = (aos_state_t *)state;

	return 0;
}


static int aos_state_msg_release_mem(
		aos_state_t *state)
{
}


static int aos_state_msg_enter(
		aos_state_t *state, 
		aos_event_t *event)
{
}


static int aos_state_msg_exit(
		aos_state_t *state, 
		aos_event_t *event)
{
}


static int aos_state_msg_event(
		aos_state_t *state, 
		aos_event_t *event)
{
}


static aos_state_msg_t sg_mf = 
{
	aos_state_hold,
	aos_state_put,
	aos_state_msg_serialize,
	aos_state_msg_deserialize,
	aos_state_msg_destroy,
	aos_state_msg_release_mem,
	aos_state_msg_enter,
	aos_state_msg_exit,
	aos_state_msg_event
};


int aos_state_msg_init(aos_state_msg_t *sm)
{
	aos_assert_r(sm, -1);

}


aos_state_msg_t *aos_state_msg_create_xml(
		struct aos_xml_node *node)
{
}

#endif
