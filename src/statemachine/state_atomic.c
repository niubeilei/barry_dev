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
// This is an atomic state. An atomic state functions very simple:
//
// 1. When it starts, it may perform some actions. 
// 2. After performed the actions, it may transit to the next 
//    state or wait for something. 
// 3. When it receives something, it checks whether it recognizes
//    the event. If it does, it processes the event and then
//    transits to the next state. If not, it can either ignore 
//    the event or fail the processing.
// 4. Upon exiting, it may perform some actions.
//
// Modification History:
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/state_atomic.h"

#include "alarm/Alarm.h"
#include "event/event.h"
#include "parser/xml_node.h"
#include "rule/rule.h"
#include "statemachine/statemachine.h"
#include "util/rc.h"
#include "util/tracer.h"
#include "util/gen_data.h"
#include "util/memory.h"

// 
// Description
// It serializes its part into the xml node 'node'. The function 
// should be called by its subclass. 
//
int aos_state_atomic_serialize(
		struct aos_state *state, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<state>
	// 			<Type>
	// 			<Id>
	// 			<LifeTimer>
	// 			<StartActions>		optional
	// 				...
	// 			</StartActions>
	// 			<EntranceActions>	optional
	// 				...
	// 			</EntranceActions>
	// 			<ExitActions>		optional
	// 				...
	// 			</ExitActions>
	// 		</state>
	// 		...
	//	</Parent>
	//
	aos_assert_r(state, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(aos_state_is_atomic(state->type) == 1, -1);

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "State");
	aos_assert_r(node, -1);

	aos_assert_r(!aos_state_serialize(state, node), -1);

	return 0;
}


int aos_state_atomic_deserialize(
		struct aos_state *state, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(state, -1);
	aos_assert_r(node, -1);

	aos_assert_r(!aos_state_deserialize(state, node), -1);

	return 0;
}


int aos_state_atomic_release_mem(struct aos_state *state)
{
	aos_assert_r(state, -1);
	aos_assert_r(!aos_state_release_mem(state), -1);

	return 0;
}


int aos_state_atomic_destroy(struct aos_state *state)
{
aos_trace("To delete state: %x", state);
	aos_assert_r(state, -1);
	aos_assert_r(!state->mf->release_memory(state), -1);
	aos_free(state);
	return 0;
}


int aos_state_atomic_start(
		aos_state_t *state, 
		aos_event_t *event, 
		aos_gen_data_t *data)
{
	aos_assert_r(!aos_state_start(state, event, data), -1);
	return 0;
}


int aos_state_atomic_exiting(
		aos_state_t *state, 
		aos_event_t *event, 
		aos_gen_data_t *data)
{
	aos_not_implemented_yet;
	return -1;
}


// 
// Descriptin
// It uses the event to check all the rules. If a rule processed
// the event, it can either terminate the state or continue the
// event processing. At the end, if the event was ever processed, 
// the last rule determines the next state it should go to.
//
int aos_state_atomic_event(
		struct aos_state *state, 
		struct aos_event *event, 
		aos_gen_data_t *data)
{
	aos_assert_r(state, -1);
	aos_assert_r(event, -1);

	/*
	aos_assert_r(state->rules, -1);
	int i, next_state_id = -1;
	int found = 0;
	int finished = 0;
	for (i=0; !finished && i<state->rules_array.noe; i++)
	{
		aos_rule_t *rule = state->rules[i];
		int rslt = rule->mf->event(rule, event, state);
		switch (rslt)
		{
		case eAosRc_Consumed:
			 // Found the match
			 next_state_id = rule->result_int;
			 finished = 1;
			 found = 1;
			 break;

		case eAosRc_Processed:
			 // Match was found and processed, but the processing
			 // should continue.
			 found = 1;
			 next_state_id = rule->result_int;
			 break;

		case eAosRc_NotProcessed:
			 // Not match. Continue the next one
			 break;

		default:
			 aos_alarm("Error in processing event: %d. Event: %s", 
					rslt, event->mf->dump(event));
			 return -1;
		}
	}

	if (!found)
	{
		if (!state->ignore_unknown_event)
		{
			aos_alarm("Received an unknown event: %s", 
					event->mf->dump(event));
			return -1;
		}

		return 0;
	}

	// 
	// The event was processed. 
	//
	aos_statemachine_t *sm = state->statemachine;
	aos_assert_r(sm, -1);

	// Exit the state
	aos_assert_r(!state->mf->exiting(state, event, data), -1);

	// Transit the state
	aos_trace("State: %d transit to: %d", 
				state->state_id, next_state_id);
	aos_assert_r(!sm->mf->trans_state(sm, state->state_id, 
				next_state_id), -1);
	*/
	aos_alarm("Event not processed: %s", 
			aos_event_2str(event->type));
	return 0;
}


static aos_state_atomic_mf_t sg_mf = 
{
	aos_state_hold,
	aos_state_put,
	aos_state_set_data,
	aos_state_clear_branches,
	aos_state_next_state,
	aos_state_atomic_serialize,
	aos_state_atomic_deserialize,
	aos_state_atomic_destroy,
	aos_state_atomic_release_mem,
	aos_state_atomic_start,
	aos_state_finish,
	0,							// Default: event_rcvd = 0
	aos_state_is_finished,
	aos_state_abort_state
};


int aos_state_atomic_init(aos_state_atomic_t *state)
{
	aos_assert_r(state, -1);
	memset(state, 0, sizeof(aos_state_atomic_t));
	state->mf = &sg_mf;
	state->type = eAosState_Atomic;

	aos_assert_r(!aos_state_init((aos_state_t *)state), -1);

	return 0;
}


aos_state_t *aos_state_atomic_create_xml(aos_xml_node_t *node)
{
	aos_state_atomic_t *obj = aos_malloc(sizeof(aos_state_atomic_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_state_atomic_init(obj), cleanup);
	aos_assert_g(!obj->mf->deserialize((aos_state_t *)obj, node), cleanup);
	return (aos_state_t *)obj;

cleanup:
	aos_free(obj);
	return 0;
}


