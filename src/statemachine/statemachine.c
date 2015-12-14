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
#include "statemachine/statemachine.h"

#include "action/action.h"
#include "alarm/Alarm.h"
#include "event/event.h"
#include "parser/xml_node.h"
#include "statemachine/state.h"
#include "thread/thread.h"
#include "util/rc_obj.h"
#include "util/memory.h"
#include "util/gen_data.h"
#include "util/tracer.h"
#include "util/named_var.h"


AOS_DECLARE_RCOBJ_LOCK;


int aos_statemachine_serialize(
		struct aos_statemachine *sm, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Statemachine>
	// 			<Name>				// Optional
	// 			<InitState>
	// 			<States>
	// 				<State>
	// 					...
	// 				</State>
	// 				<State>
	// 				...
	// 				</State>
	// 				...
	// 			</States>
	// 			<Variables>
	// 				<Var>
	// 					<Name>
	// 					<Value>
	// 				</Var>
	// 				...
	// 			</Variables>
	// 			<InitActions>
	// 				<Action>
	// 				<Action>
	// 				...
	// 			</InitActions>
	// 			<ExitActions>
	// 				<Action>
	// 				<Action>
	// 				...
	// 			</ExitActions>
	// 		</Statemachine>
	// 	</Parent>
	//
	aos_assert_r(sm, -1);
	aos_assert_r(parent, -1);

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "Statemachine");
	aos_assert_r(node, -1);

	// Set <Name>
	if (sm->name)
	{
		aos_assert_r(!node->mf->append_child_str(
				node, "Name", sm->name, 0, 0), -1);
	}

	// Set <InitState>
	aos_xml_node_t *init_state = node->mf->append_child_node(
			node, "InitState");
	aos_assert_r(init_state, -1);
	aos_assert_r(!sm->init_state->mf->serialize(sm->init_state, init_state), -1);	
	
	// Append <States>
	aos_xml_node_t *states = node->mf->append_child_node(node, "States");

	aos_state_t *state;
	aos_list_for_each_entry(state, &sm->states, sm_link)
	{
		aos_assert_r(!state->mf->serialize(state, states), -1);
	}

	// Append <Variables>
	if (sm->machine_data->mf->is_empty(sm->machine_data) != 1)
	{
		aos_xml_node_t *vn = node->mf->append_child_node(node, "Variables");
		aos_assert_r(vn, -1);
		aos_assert_r(!sm->machine_data->mf->serialize(
				sm->machine_data, vn), -1);
	}

	// Append <InitActions>
	if (aos_list_empty(&sm->init_actions) != 1)
	{
		aos_xml_node_t *ia = node->mf->append_child_node(node, "InitActions");
		aos_assert_r(ia, -1);
		aos_action_t *action, *tmp;
		aos_list_for_each_entry_safe(action, tmp, &sm->init_actions, link)
		{
			aos_assert_r(!action->mf->serialize(action, ia), -1);
		}
	}

	// Append <ExitActions>
	if (aos_list_empty(&sm->exit_actions) != 1)
	{
		aos_xml_node_t *ia = node->mf->append_child_node(node, "ExitActions");
		aos_assert_r(ia, -1);
		aos_action_t *action, *tmp;
		aos_list_for_each_entry_safe(action, tmp, &sm->exit_actions, link)
		{
			aos_assert_r(!action->mf->serialize(action, ia), -1);
		}
	}

	return 0;
}


int aos_statemachine_deserialize(
		struct aos_statemachine *sm, 
		struct aos_xml_node *node)
{
	aos_assert_r(sm, -1);
	aos_assert_r(node, -1);

	// Retrieve <Name>
	int len;
	aos_assert_r(!node->mf->first_child_str_dft(
			node, "Name", &sm->name, &len, 0), -1);

	// Retrieve <InitState>
	aos_xml_node_t *init_state = node->mf->first_named_child(node, 
			"InitState");
	aos_assert_r(init_state, -1);
	aos_xml_node_t *sn = init_state->mf->first_named_child(
			init_state, "State");
	aos_assert_r(sn, -1);
	aos_assert_r(!sm->mf->set_init_state(sm, aos_state_factory(sn)), -1);

	// Retrieve <States>
	aos_xml_node_t *states = node->mf->first_named_child(node, "States");
	if (states)
	{
		sn = states->mf->first_child(states);
		while (sn)
		{
			aos_state_t *st = aos_state_factory(sn);
			aos_assert_r(st, -1);
			aos_assert_r(!sm->mf->add_state(sm, st), -1);
			sn = states->mf->next_sibling(states);
		}
	}
	// Retrieve <Variables>
	aos_xml_node_t *vn = node->mf->first_named_child(node, "Variables");
	if (vn)
	{
		aos_assert_r(!sm->machine_data->mf->deserialize(
				sm->machine_data, vn), -1);
	}

	// Retrieve <InitActions>
	aos_xml_node_t *ia = node->mf->first_named_child(node, "InitActions");
	if (ia)
	{
		aos_xml_node_t *vv = vn->mf->first_child(ia);
		while (vv)
		{
			aos_action_t *action = aos_action_factory(vv);
			aos_assert_r(action, -1);
			action->mf->hold(action);
			aos_list_add_tail(&action->link, &sm->init_actions);
			vv = ia->mf->next_sibling(ia);
		}
	}

	// Retrieve <ExitActions>
	ia = node->mf->first_named_child(node, "ExitActions");
	if (ia)
	{
		aos_xml_node_t *vv = vn->mf->first_child(ia);
		while (vv)
		{
			aos_action_t *action = aos_action_factory(vv);
			aos_assert_r(action, -1);
			action->mf->hold(action);
			aos_list_add_tail(&action->link, &sm->exit_actions);
			vv = ia->mf->next_sibling(ia);
		}
	}

	return 0;
}


int aos_statemachine_set_init_state(
		aos_statemachine_t *sm, 
		aos_state_t *state)
{
	aos_assert_r(sm, -1);
	aos_assert_r(state, -1);
	if (sm->init_state) aos_assert_r(!sm->init_state->mf->put(sm->init_state), -1);
	sm->init_state = state;
	state->mf->hold(state);

	if (state->statemachine) aos_assert_r(!state->statemachine->mf
			->put(state->statemachine), -1); 
	state->statemachine = sm;
	aos_assert_r(!sm->mf->hold(sm), -1);
	sm->min_ref_count++;
	return 0;
}


int aos_statemachine_add_state(
		aos_statemachine_t *sm, 
		aos_state_t *state)
{
	aos_assert_r(sm, -1);
	aos_assert_r(state, -1);

	aos_list_add_tail(&state->sm_link, &sm->states);
	if (state->statemachine) state->statemachine->mf->put(state->statemachine);
	sm->mf->hold(sm);
	sm->min_ref_count++;
	state->statemachine = sm;
	return 0;
}


int aos_statemachine_add_event(
		aos_statemachine_t *sm, 
		aos_event_t *event)
{
	aos_assert_r(sm, -1);
	aos_assert_r(event, -1);

	aos_lock(sm->lock);
	event->mf->hold(event);
	aos_list_add_tail(&event->link, &sm->events);
	aos_condvar_signal(sm->condvar);
	aos_unlock(sm->lock);
	return 0;
}


int aos_statemachine_release_memory(aos_statemachine_t *sm)
{
	aos_assert_r(sm, -1);
	if (sm->name) aos_free(sm->name);
	sm->name = 0;

	aos_assert_r(!sm->mf->release_paired_obj(sm), -1);

	if (sm->lock) aos_free(sm->lock);
	sm->lock = 0;

	if (sm->condvar) aos_free(sm->condvar);
	sm->condvar = 0;

	if (sm->thread) aos_free(sm->thread);
	sm->thread = 0;
	
	aos_event_t *event, *tmp;
	aos_list_for_each_entry_safe(event, tmp, &sm->events, link)
	{
		aos_list_del(&event->link);
		event->mf->put(event);
	}

	aos_action_t *action, *atmp;
	aos_list_for_each_entry_safe(action, atmp, &sm->init_actions, link)
	{
		aos_list_del(&action->link);
		action->mf->put(action);
	}

	aos_list_for_each_entry_safe(action, atmp, &sm->exit_actions, link)
	{
		aos_list_del(&action->link);
		action->mf->put(action);
	}

	return 0;
}


// 
// Description
// The statemachine has finished everything. Time to exit the
// statemachine.
//
int aos_statemachine_finish(
		aos_statemachine_t *sm)
{
	aos_assert_r(sm, -1);
	sm->mf->signal(sm, eAosEvent_ThrdFinished, 0);
	return 0;
}


static int sm_action_callback(
		aos_event_t *event, 
		void *user_data)
{
	aos_assert_r(event, -1);
	aos_assert_r(user_data, -1);
	aos_statemachine_t *sm = (aos_statemachine_t *)user_data;
	aos_assert_r(sm->magic == AOS_STATEMACHINE_MAGIC, -1);

	if (sm->mf->event_rcvd)
	{
		return sm->mf->event_rcvd(sm, event);
	}

	int all_finished;
	switch (sm->status)
	{
	case eAosSmStatus_InitActions:
		 all_finished = aos_action_run_action_list(
				&sm->init_actions, 
				sm->machine_data, 
				sm_action_callback, 
				sm, &sm->reg_id);
		 if (!all_finished) return 0;

		 aos_assert_r(!sm->mf->run_state(sm, sm->init_state), -1);
		 return 0;

	case eAosSmStatus_ExitActions:
		 all_finished = aos_action_run_action_list(
				&sm->exit_actions, 
				sm->machine_data, 
				sm_action_callback, 
				sm, &sm->reg_id);
		 if (!all_finished) return sm->mf->finish(sm);
		 break;

	case eAosSmStatus_InProgress:
		 aos_not_implemented_yet;
		 return -1;

	default:
		 aos_alarm("Unrecognized sm state: %d", sm->status);
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}


int aos_statemachine_start(struct aos_statemachine *sm)
{
	aos_assert_r(sm, -1);
	aos_assert_r(sm->init_state, -1);

	// Run the init_actions
	if (!sm->machine_data)
	{
		sm->machine_data = aos_gen_data_create();
		aos_assert_r(sm->machine_data, -1);
	}

	aos_assert_r(!sm->machine_data->mf->set_ptr(sm->machine_data, 
				eAosGenData_CrtStmch, sm), -1);
	sm->reg_id = 0;
	aos_assert_r(!aos_action_reset_action_list(&sm->init_actions), -1);
	aos_assert_r(!aos_action_reset_action_list(&sm->exit_actions), -1);
	int all_finished = aos_action_run_action_list(
				&sm->init_actions, 
				sm->machine_data, 
				sm_action_callback, 
				sm, &sm->reg_id);
	aos_assert_r(all_finished >= 0, -1);

	if (!all_finished) return 0;

	// Run the init state
	aos_assert_r(!sm->mf->run_state(sm, sm->init_state), -1);
	return 0;
}


int aos_statemachine_hold(aos_statemachine_t *sm)
{
	AOS_RCM_OBJ_HOLD(sm);
}


int aos_statemachine_put(aos_statemachine_t *sm)
{
	AOS_RCM_OBJ_PUT(sm);
}


int aos_statemachine_release_paired_obj(aos_statemachine_t *sm)
{
	aos_assert_r(sm, -1);

	aos_state_t *state, *tmp;
	aos_list_for_each_entry_safe(state, tmp, &sm->states, sm_link)
	{
		state->mf->put(state);
		aos_list_del(&state->sm_link);
	}
	return 0;
}


int aos_statemachine_destroy(aos_statemachine_t *sm)
{
	aos_assert_r(sm, -1);
	aos_assert_r(!sm->mf->release_memory(sm), -1);
	aos_free(sm);
	return 0;
}


int aos_statemachine_msg_rcvd(
		aos_statemachine_t *sm, 
		aos_field_t *msg)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_statemachine_add_var(
		aos_statemachine_t *sm, 
		const aos_gen_data_tag_e tag,
		char * value, 
		const int alloc_mem)
{
	aos_assert_r(sm, -1);
	aos_assert_r(value, -1);
	aos_assert_r(sm->mf->var_exist(sm, tag) != 1, -1);
	aos_assert_r(sm->machine_data, -1);
	aos_assert_r(!sm->machine_data->mf->set_str(sm->machine_data, 
				tag, value, alloc_mem), -1);
	return 0;
}


int aos_statemachine_del_var(
		aos_statemachine_t *sm, 
		const aos_gen_data_tag_e tag)
{
	aos_assert_r(sm, -1);
	aos_not_implemented_yet;
	return -1;
	aos_alarm("Not found: %s", aos_gen_data_tag_2str(tag));
	return -1;
}


// 
// Description:
// It retrieves the named variable. If not found, -1 is returned.
// Otherwise, 0 is returned and the retrieved variable is 
// passed back through 'variable'. 
//
// IMPORTANT: This function calls the 'hold()' function of the
// retrieved variable. If the caller does not need the variable,
// it is IMPORTANT to put the variable.
//
int aos_statemachine_get_var(
		aos_statemachine_t *sm, 
		const aos_gen_data_tag_e tag, 
		aos_value_t **variable)
{
	aos_assert_r(sm, -1);
	aos_assert_r(variable, -1);
	aos_assert_r(sm->machine_data, -1);
	aos_assert_r(!sm->machine_data->mf->get_value(
				sm->machine_data, tag, variable), -1);
	return 0;
}


int aos_statemachine_get_var_str(
		aos_statemachine_t *sm, 
		const aos_gen_data_tag_e tag, 
		char **str)
{
	aos_assert_r(sm, -1);
	aos_assert_r(str, -1);
	aos_assert_r(sm->machine_data, -1);
	int len;
   	aos_assert_r(!sm->machine_data->mf->get_str(
			sm->machine_data, tag, str, &len), -1);
	return 0;
}


int aos_statemachine_var_exist(
		aos_statemachine_t *sm, 
		const aos_gen_data_tag_e tag) 
{
	aos_assert_r(sm, -1);
	aos_assert_r(sm->machine_data, -1);
	return sm->machine_data->mf->data_exist(sm->machine_data, tag);
	return 0;
}


int aos_statemachine_stop(aos_statemachine_t *sm)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_statemachine_trans_state(
		aos_statemachine_t *sm, 
		const int from, 
		const int to)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_statemachine_proc_event(
		aos_statemachine_t *sm, 
		aos_event_t *event)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_statemachine_signal(
		aos_statemachine_t *sm,
		const aos_event_e event_id,
		aos_state_t *state)
{
	aos_assert_r(sm, -1);

	aos_lock(sm->lock);
	sm->event_id = event_id;
	if (sm->crt_state) sm->crt_state->mf->put(sm->crt_state);
	if (state)
	{
		state->mf->hold(state);
		sm->crt_state = state;
	}
	aos_condvar_signal(sm->condvar);
	aos_unlock(sm->lock);
	return 0;
}


aos_state_t *aos_statemachine_get_state(
		aos_statemachine_t *sm, 
		const int state_id)
{
	aos_assert_r(sm, 0);

	aos_state_t *state;
	aos_list_for_each_entry(state, &sm->states, sm_link)
	{
		if (state->state_id == state_id)
		{
			state->mf->hold(state);
			return state;
		}
	}

	return 0;
}


int aos_statemachine_run_state(
		aos_statemachine_t *sm, 
		aos_state_t *state)
{
	aos_assert_r(sm, -1);
	aos_assert_r(state, -1);
	int next_id;

	// 
	// It runs all the states, starting from 'state' until
	// either a state does not finish or state becomes null.
	//
	state->mf->hold(state);
	aos_lock(sm->lock);
	while (state)
	{
		aos_trace("To run state: %d", state->state_id);
		aos_assert_g(!state->mf->start(state, 0, sm->machine_data), cleanup);
		if (state->mf->is_finished(state) != 1)
		{
			if (sm->crt_state) sm->crt_state->mf->put(sm->crt_state);
			sm->crt_state = state;
			aos_unlock(sm->lock);
			return 0;
		}

		aos_trace("State: %d finished", state->state_id);
		aos_assert_g(!state->mf->next_state(state, &next_id), cleanup);
		if (next_id == AOS_STATE_NULL_STATE_ID)
		{
			// 
			// The execution of the statemachine finished.
			//
			int all_finished = aos_action_run_action_list(
					&sm->exit_actions, 
					sm->machine_data, 
					sm_action_callback, sm, &sm->reg_id);
			aos_assert_g(all_finished >= 0, cleanup);

			if (all_finished) aos_assert_g(!sm->mf->finish(sm), cleanup);
			state->mf->put(state);
			if (sm->crt_state) sm->crt_state->mf->put(sm->crt_state);
			aos_unlock(sm->lock);
			return 0;
		}

		state->mf->put(state);
		state = sm->mf->get_state(sm, next_id);
		aos_assert_g(state, cleanup);
	}

	aos_should_never_come_here;
	return -1;

cleanup:
	aos_assert_r(state, -1);
	state->mf->put(state);
	aos_unlock(sm->lock);
	return -1;
}

		
// 
// Description:
// 'state' is the state that just finished its execution. This function
// determiens what to do next.
//
int aos_statemachine_state_finished(
		aos_statemachine_t *sm, 
		aos_state_t *state)
{
	aos_assert_r(state, -1);
	aos_assert_g(sm, cleanup);

	int next_id = 0;
	aos_assert_r(!state->mf->next_state(state, &next_id), -1);
	if (next_id == AOS_STATE_NULL_STATE_ID)
	{
		// 
		// The execution of the statemachine finished.
		//
		sm->status = eAosSmStatus_ExitActions;
		int all_finished = aos_action_run_action_list(
				&sm->init_actions, 
				sm->machine_data, 
				sm_action_callback, 
				sm, &sm->reg_id);

		if (all_finished)
		{
			return sm->mf->finish(sm);
		}
		return 0;
	}

	state = sm->mf->get_state(sm, next_id);
	aos_assert_r(state, -1);
	aos_assert_g(!sm->mf->run_state(sm, state), cleanup);
	state->mf->put(state);
	return 0;

cleanup:
	state->mf->put(state);
	return -1;
}


int aos_statemachine_proc_simple_event(
		aos_statemachine_t *sm, 
		const aos_event_e event_id,
		aos_state_t *state)
{
	aos_assert_r(sm, -1);

	switch (event_id)
	{
	case eAosEvent_StateFinished:
		 aos_assert_r(state, -1);
		 aos_assert_r(!sm->mf->state_finished(sm, state), -1);
		 return 0;

	case eAosEvent_ThrdFinished:
		 aos_assert_r(sm->thread, -1);
		 sm->thread->finished = 1;
		 return 0;

	default:
		 aos_alarm("Unrecognized event: %d:%s", 
				 event_id, aos_event_2str(event_id));
		 return -1;
	}

	aos_should_never_come_here;
	return -1;
}

	
static aos_statemachine_mf_t sg_mf = 
{
	aos_statemachine_hold,
	aos_statemachine_put,
	aos_statemachine_release_paired_obj,
	aos_statemachine_set_init_state,
	aos_statemachine_add_var,
	aos_statemachine_del_var,
	aos_statemachine_get_var,
	aos_statemachine_get_var_str,
	aos_statemachine_var_exist,
	aos_statemachine_release_memory,
	aos_statemachine_add_state,
	aos_statemachine_serialize,
	aos_statemachine_deserialize,
	aos_statemachine_destroy,
	aos_statemachine_msg_rcvd,
	0,								// Default: even_rcvd = 0
	aos_statemachine_start,
	aos_statemachine_stop,
	aos_statemachine_finish,
	aos_statemachine_trans_state,
	aos_statemachine_add_event,
	aos_statemachine_signal,
	aos_statemachine_proc_event,
	aos_statemachine_state_finished,
	aos_statemachine_run_state,
	aos_statemachine_get_state
};


static void thrd_func(aos_thread_t *thrd)
{
	aos_assert(thrd);
	thrd->mf->hold(thrd);
	aos_event_t *event;
	aos_statemachine_t *sm = (aos_statemachine_t *)thrd->user_data;
	aos_assert(sm->magic == AOS_STATEMACHINE_MAGIC);

	while (!thrd->finished)
	{
		aos_lock(sm->lock);

		if (sm->event_id != eAosEvent_Invalid)
		{
			aos_event_e eid = sm->event_id;
			aos_state_t *state = sm->crt_state;
			sm->event_id = eAosEvent_Invalid;
			sm->crt_state = 0;
			aos_unlock(sm->lock);

			aos_statemachine_proc_simple_event(sm, eid, state);
			continue;
		}

		if (aos_list_empty(&sm->events))
		{
			aos_condvar_wait(sm->condvar, sm->lock);
			aos_unlock(sm->lock);
			continue;
		}

		event = (aos_event_t *)sm->events.next;
		aos_assert(event->magic == AOS_EVENT_MAGIC);
		aos_list_del(&event->link);
		aos_unlock(sm->lock);

		aos_assert(!sm->mf->proc_event(sm, event));
		event->mf->put(event);
	}

	thrd->mf->put(thrd);
aos_trace("Statemachine finished");
}


int aos_statemachine_init(aos_statemachine_t *sm)
{
	aos_assert_r(sm, -1);
	memset(sm, 0, sizeof(aos_statemachine_t));
	sm->mf = &sg_mf;
	sm->magic = AOS_STATEMACHINE_MAGIC;
	sm->status = eAosSmStatus_Idle;
	AOS_INIT_LIST_HEAD(&sm->states);
	AOS_INIT_LIST_HEAD(&sm->init_actions);
	AOS_INIT_LIST_HEAD(&sm->exit_actions);
	sm->machine_data = aos_gen_data_create();
	aos_assert_r(sm->machine_data, -1);

	sm->lock = aos_malloc(sizeof(aos_lock_t));
	aos_assert_g(sm->lock, cleanup);
	aos_init_lock(sm->lock);

	sm->condvar = aos_malloc(sizeof(aos_condvar_t));
	aos_assert_g(sm->condvar, cleanup);
	aos_condvar_init(sm->condvar);

	sm->thread = aos_thread_create(sm->name, 0, thrd_func, 
			sm, 0, 1, 0);
	aos_assert_g(sm->thread, cleanup);

	AOS_INIT_LIST_HEAD(&sm->events);
	
	return 0;

cleanup:
	if (sm->thread) aos_free(sm->thread);
	if (sm->condvar) aos_free(sm->condvar);
	if (sm->lock) aos_free(sm->lock);
	if (sm->machine_data) aos_free(sm->machine_data);
	return -1;
}


aos_statemachine_t * aos_statemachine_create_xml(aos_xml_node_t *node)
{
	aos_statemachine_t *sm = aos_malloc(sizeof(aos_statemachine_t));
	aos_assert_r(sm, 0);
	aos_assert_g(!aos_statemachine_init(sm), cleanup);

	aos_assert_g(!sm->mf->deserialize(sm, node), cleanup);
	return sm;

cleanup:
	aos_free(sm);
	return 0;
}

