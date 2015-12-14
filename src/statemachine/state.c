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
// This is the super class for all states. 
//
// Virtual Member Functions
// 'start'
// This function is called when a state is started. 
//
// 'exiting'
// This function is called when the state logic determines to leave
// the state. Note that this may trigger the state to do something. 
//
// Event Processing
// When it receives an event , all the rules are checked in the 
// order in which they are stored. If a rule processed the event, 
// it may terminate the processing, or let the process continue. 
// At the end, if the event was processed, the state will finish
// and the next state is determined by the last rule that processed
// the event. 
//
// Modification History:
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/state.h"

#include "action/action.h"
#include "alarm/Alarm.h"
#include "event/event.h"
#include "parser/xml_node.h"
#include "rule/rule.h"
#include "statemachine/statemachine.h"
#include "statemachine/state_atomic.h"
#include "util/memory.h"
#include "util/timer.h"
#include "util/gen_data.h"
#include "util/rc_obj.h"
#include "util/tracer.h"
#include "util/magic.h"

AOS_DECLARE_RCOBJ_LOCK;


#define AOS_STATE_DEFAULT_LIFETIMER 10

// 
// Description
// It serializes its part into the xml node 'node'. The function 
// should be called by its subclass. 
//
int aos_state_serialize(
		struct aos_state *state, 
		struct aos_xml_node *node)
{
	// 
	// 	<state>
	// 		<Type>
	// 		<Id>
	// 		<LifeTimer>						default: 10	
	// 		<IgnoreUnknownEvent>			default: 0
	// 		<Actions>	... </Actions>		optional
	// 		<Rules>	... </Rules>			optional
	// 		<Branching>						mandatory
	// 			<Branch>
	// 				<Cond>
	// 				<StateId>
	// 			</Branch>
	// 			...
	// 		</Branching>
	// 	</state>
	//
	int i;
	aos_xml_node_t *child;
	aos_assert_r(state, -1);
	aos_assert_r(node, -1);

	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, "Type", 
			aos_state_2str(state->type), 0, 0), -1);

	// Set <Id>
	aos_assert_r(!node->mf->append_child_int(node, "Id", 
			state->state_id), -1);

	// Set <LifeTimer>
	if (state->life_timer != AOS_STATE_DEFAULT_LIFETIMER)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"LifeTimer", state->life_timer), -1);
	}

	// Set <IgnoreUnknownEvent>
	if (state->ignore_unknown_event)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"IgnoreUnknownEvent", state->ignore_unknown_event), -1);
	}

	// Set <Actions>
	aos_xml_node_t *actions = node->mf->append_child_node(
				node, "Actions");
	aos_assert_r(actions, -1);

	aos_action_t *action, *tt;
	aos_list_for_each_entry_safe(action, tt, &state->actions, link)
	{
		aos_assert_r(!action->mf->serialize(action, actions), -1);
	}

	// Set <Rules>
	if (state->rules)
	{
		child = node->mf->append_child_node(node, "Rules");
		aos_assert_r(child, -1);
		for (i=0; i<state->rules_array.noe; i++)
		{
			aos_rule_t *rule = state->rules[i];
			aos_assert_r(!rule->mf->serialize(rule, child), -1);
		}
	}

	// Set <Branching>
	aos_assert_r(!aos_list_empty(&state->branches), -1);
	aos_xml_node_t *bn = node->mf->append_child_node(
				node, "Branching");
	aos_assert_r(bn, -1);
	aos_state_branch_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &state->branches, link)
	{
		aos_xml_node_t *sub = bn->mf->append_child_node(bn, "Branch");
		aos_assert_r(sub, -1);
		aos_assert_r(!entry->cond->mf->serialize(entry->cond, sub), -1);
		aos_assert_r(!sub->mf->append_child_int(sub, "StateId", entry->state_id), -1);
	}

	return 0;
}


static int aos_state_add_rule(
		aos_state_t *state, 
		aos_rule_t *rule)
{
	aos_assert_r(state, -1);
	aos_assert_r(rule, -1);
	aos_assert_r(!state->rules_array.mf->add_element(
		&state->rules_array, 1, (char **)&state->rules), -1);
	state->rules[state->rules_array.noe++] = rule;
	return 0;
}


int aos_state_clear_branches(aos_state_t *state)
{
	aos_assert_r(state, -1);

	aos_state_branch_t *entry;

	while (!aos_list_empty(&state->branches))
	{
		entry = (aos_state_branch_t *)state->branches.next;
		aos_assert_r(!entry->cond->mf->put(entry->cond), -1);
		aos_list_del(&entry->link);
	}

	return 0;
}


int aos_state_deserialize(
		struct aos_state *state, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	char buff[100];
	int buff_len = 100;
	aos_xml_node_t *child = 0;
	aos_assert_r(state, -1);
	aos_assert_r(node, -1);

	// Retrieve <Type>
	buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(node, "Type", 
			buff, &buff_len, 0), -1);
	state->type = aos_state_2enum(buff);
	aos_assert_r(aos_state_check(state->type) == 1, -1);

	// Retrieve <Id>
	aos_assert_r(!node->mf->first_child_int(node, "Id", 
			&state->state_id), -1);

	// Retrieve <LifeTimer>
	int life_timer;
	aos_assert_r(!node->mf->first_child_int_dft(node, "LifeTimer", 
			&life_timer, AOS_STATE_DEFAULT_LIFETIMER), -1);
	aos_assert_r(life_timer > 0, -1);
	state->life_timer = (u32)life_timer;

	// Retrieve <IgnoreUnknownEvent>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"IgnoreUnknownEvent", &state->ignore_unknown_event, 0), -1);

	// Retrieve <Actions>
	child = node->mf->first_named_child(node, "Actions");
	aos_assert_r(child, -1);
	aos_xml_node_t *action = child->mf->first_child(child);
	aos_assert_r(action, -1);
	while (action)
	{
		aos_action_t *a = aos_action_factory(action);
		aos_assert_r(a, -1);
aos_trace("Got action: %x", a);
		a->mf->hold(a);
		aos_list_add_tail(&a->link, &state->actions);
		action = child->mf->next_sibling(child);
	}

	// Retrieve <Rules>
	child = node->mf->first_named_child(node, "Rules");
	if (child)
	{
		aos_xml_node_t *proc = child->mf->first_child(child);
		aos_assert_r(proc, -1);
		while (proc)
		{
			aos_rule_t *a = aos_rule_factory(proc);
			aos_assert_r(a, -1);
			aos_assert_r(!aos_state_add_rule(state, a), -1);
			proc = child->mf->next_sibling(child);
		}
	}

	// Retrieve <Branches>
	child = node->mf->first_named_child(node, "Branches");
	aos_assert_r(child, -1);
	aos_assert_r(!state->mf->clear_branches(state), -1);
	aos_xml_node_t *nn = child->mf->first_child(child);
	while (nn)
	{
		aos_xml_node_t *cn = nn->mf->first_named_child(nn, "Cond");
		aos_assert_r(cn, -1);
		aos_cond_t *cond = aos_cond_factory(cn);
		aos_assert_r(cond, -1);

		int state_id;
		aos_assert_r(!nn->mf->first_child_int(nn, "StateId", &state_id), -1);

		aos_state_branch_t *branch = aos_malloc(sizeof(aos_state_branch_t));
		aos_assert_r(branch, -1);
		AOS_INIT_LIST_HEAD(&branch->link);
		branch->cond = cond;
		branch->state_id = state_id;
		aos_list_add_tail(&branch->link, &state->branches);
		nn = child->mf->next_sibling(child);
	}

	aos_assert_r(!aos_list_empty(&state->branches), -1);

	return 0;
}


int aos_state_release_mem(struct aos_state *state)
{
	aos_assert_r(state, -1);

	aos_action_t *action, *tmp;
	aos_list_for_each_entry_safe(action, tmp, &state->actions, link)
	{
		action->mf->put(action);
	}

	// Release rules_array
	aos_assert_r(!state->rules_array.mf->release_memory(
			&state->rules_array), -1);

	if (state->statemachine)
	{
		state->statemachine->mf->put(state->statemachine);
		state->statemachine = 0;
	}

	return 0;
}


int aos_state_hold(aos_state_t *state)
{
	AOS_RC_OBJ_HOLD(state);
}


int aos_state_put(aos_state_t *state)
{
	AOS_RC_OBJ_PUT(state);
}


static int state_event_callback(
		aos_event_t *event, 
		void *user_data)
{
	aos_assert_r(event, -1);
	aos_assert_r(user_data, -1);
	aos_state_t *state = (aos_state_t *)user_data;
	aos_assert_r(state->magic == AOS_STATE_MAGIC, -1);

	if (state->mf->event_rcvd)
	{
		aos_assert_r(!state->mf->event_rcvd(state, event), -1);
		return 0;
	}

	if (state->status == eAosStStatus_Finished) return 0;

	// Need to check whether all actions finished
	int all_finished = aos_action_run_action_list(&state->actions, 
			state->data, state_event_callback, 
			state, &state->reg_id);

	aos_assert_r(all_finished >= 0, -1);
	if (all_finished == 1)
	{
		return state->mf->finish(state);
	}

	return 0;
}

	
int aos_state_finish(aos_state_t *state)
{
	aos_assert_r(state, -1);
	
	// Cancel the life timer
	if (state->life_timer_id > 0)
	{
		aos_assert_r(!aos_timer_cancel_timer(state->life_timer_id), -1);
		state->life_timer_id = -1;
	}

	state->status = eAosStStatus_Finished;
	aos_trace("State: %d finished", state->state_id);

	aos_assert_r(state->statemachine, -1);
	aos_assert_r(!state->statemachine->mf->state_finished(
				state->statemachine, state), -1);
	return 0;
}


// 
// Description
// This function is called when the execution of the state
// is aborted (such as the lifetimer expired). This implementation
// serves as the default, which does nothing. 
//
int aos_state_abort_state(aos_state_t *state)
{
	aos_assert_r(state, -1);
	aos_alarm("State: %d execution aborted", state->state_id);
	return 0;
}


// 
// Description
// This function is called when the state's life timer expires. 
// This indicates the state is not good. It needs to abort the
// state. 
//
static int aos_state_timer_func(
		const u32 timer_id, 
		void *data)
{
	aos_assert_r(data, -1);
	aos_state_t *state = (aos_state_t *)data;

	if (state->life_timer_id == -1)
	{
		// This may happen when the state canceled its life
		// timer but at the moment when canceling the life
		// timer, the life timer expired. 
		aos_very_rare("State: %d, Timer: %d", 
				state->state_id, timer_id);

		// Since the state has already canceled the timer, 
		// do nothing.
		return 0;
	}

	aos_assert_r(timer_id == state->life_timer_id, -1);
	aos_warn("State: %d, timer: %d", state->state_id, timer_id);
	state->life_timer_id = -1;
	aos_assert_r(!state->mf->abort_state(state), -1);
	return 0;
}


int aos_state_set_data(
		aos_state_t *state, 
		aos_gen_data_t *data)
{
	aos_assert_r(state, -1);
	
	if (!data) return 0;

	if (!state->data) return aos_gen_data_clone(&state->data, data);
	
	return aos_gen_data_copy(state->data, data);
}


// 
// Description
// The state is started. If the start actions are not empty, 
// it will run those actions. All actions must be successful. 
// If the state_life_timer is set, it will start a timer for it.
//
int aos_state_start(
		struct aos_state *state, 
		struct aos_event *event, 
		aos_gen_data_t *data)
{
	aos_assert_r(state, -1);
	aos_assert_r(state->status == eAosStStatus_Idle, -1);
	aos_action_t *action, *tmp;

	aos_list_for_each_entry_safe(action, tmp, &state->actions, link)
	{
		aos_assert_r(!action->mf->reset(action), -1);
	}

aos_trace("Start State: %d", state->state_id);

	if (data) aos_assert_r(!state->mf->set_data(state, data), -1);

	// Start the actions
	state->reg_id = 0;
	int all_finished = aos_action_run_action_list(&state->actions, 
			state->data, state_event_callback, 
			state, &state->reg_id);
	aos_assert_r(all_finished >= 0, -1);

	if (all_finished == 1)
	{
		state->status = eAosStStatus_Finished;
		// return state->mf->finish(state);
		return 0;
	}

	state->status = eAosStStatus_InProgress;

	// Start the state life timer
	if (state->life_timer <= 0)
	{
		aos_assert_r(!aos_timer_start_timer(
				state->life_timer, &state->life_timer_id, 
				aos_state_timer_func, state), -1);
	}

	return 0;
}


int aos_state_is_finished(aos_state_t *state)
{
	aos_assert_r(state, -1);
	return state->status == eAosStStatus_Finished;
}


int aos_state_init(aos_state_t *sm)
{
aos_trace("To create state: %x", sm);
	aos_assert_r(sm, -1);

	AOS_INIT_LIST_HEAD(&sm->actions);
	AOS_INIT_LIST_HEAD(&sm->sm_link);
	sm->magic = AOS_STATE_MAGIC;
	sm->status = eAosStStatus_Idle;

	// Init rules_array
	aos_assert_r(!aos_dyn_array_init(&sm->rules_array, 
			(char **)&sm->rules, 
			sizeof(aos_rule_t *), 5, 
			AOS_STATE_MAX_RULES), -1);

	AOS_INIT_LIST_HEAD(&sm->branches);
	return 0;
}


aos_state_t *aos_state_factory(struct aos_xml_node *node)
{
	char type[100];
	int type_len = 100;
	aos_state_t *state = 0;
	aos_assert_r(node, 0);

	aos_assert_r(!node->mf->first_child_str_b(node, "Type", 
			type, &type_len, 0), 0);
	if (strcmp(type, "atomic") == 0)
	{
		state = aos_state_atomic_create_xml(node);
		aos_assert_r(state, 0);
		return state;
	}

	aos_alarm("Unrecognized state: %s", node->mf->dump(
				(aos_field_t *)node));
	return 0;
}


int aos_state_is_atomic(const aos_state_e type)
{
	return (type == eAosState_Atomic)?1:0;
}


int aos_state_next_state(
		struct aos_state *state, 
		int *state_id)
{
	aos_state_branch_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &state->branches, link)
	{
		if (entry->cond->mf->evaluate(entry->cond, state->branch_data) == 1)
		{
			*state_id = entry->state_id;
			return 0;
		}
	}

	aos_alarm("Failed to branch: %d", state->state_id);
	return -1;
}

