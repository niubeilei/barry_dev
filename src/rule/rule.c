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
#include "rule/rule.h"

#include "action/action.h"
#include "alarm/Alarm.h"
#include "event/event.h"
#include "parser/xml_node.h"
#include "statemachine/state.h"
#include "util/rc.h"
#include "util/memory.h"
#include "util/magic.h"


int aos_rule_serialize(
		struct aos_rule *proc, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<a_name>
	// 			<Type>
	// 			<Condition>...</Condition>
	// 			<Actions>...</Actions>
	// 		</a_name>
	// 		...
	// 	</Parent>
	aos_not_implemented_yet;
	return -1;
}


int aos_rule_deserialize(
		struct aos_rule *proc, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rule_destroy(struct aos_rule *rule)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rule_hold(struct aos_rule *rule)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_rule_put(struct aos_rule *rule)
{
	aos_not_implemented_yet;
	return -1;
}


const char *aos_rule_dump(struct aos_rule *rule)
{
	aos_not_implemented_yet;
	return 0;
}


int aos_rule_dump_to(
		struct aos_rule *rule, 
		char *buff, 
		int *len)
{
	aos_not_implemented_yet;
	return 0;
}


static int rule_action_callback(
		aos_event_t *event, 
		void *user_data)
{
	aos_assert_r(event, -1);
	aos_assert_r(user_data, -1);
	aos_rule_t *rule = (aos_rule_t *)user_data;
	aos_assert_r(rule->magic == AOS_RULE_MAGIC, -1);

	aos_not_implemented_yet;
	return -1;
}


// 
// Description
// An event is received. This function checks whether it is 
// the event this event proc expects. If not, it returns 
// eAosEventProcRslt_NotProcessed. If yes, it processes it. 
//
// Returns:
// eAosRc_Consumed:
// 		The event was processed by this processor and the 
// 		event shall not be processed by anyone else.
//
// eAosRc_Processed:
// 		The event was processed but the event may be processed
// 		by other processors.
//
// eAosRc_NotProcessed:
// 		The event is not what the proc expects. 
//
// Negative: if errors.
//
int aos_rule_event(
		aos_rule_t *rule, 
		aos_event_t *event, 
		aos_state_t *state)
{
	aos_assert_r(rule, -1);
	aos_assert_r(event, -1);

	int rslt = rule->cond->mf->evaluate_by_event(
			rule->cond, event, state, rule->data);
	aos_assert_r(rslt >= 0, rslt);
	if (rslt == 0)
	{
		// Not what we expected
		return eAosRc_NotProcessed;
	}

	// It is what the proc expects. 
	int all_finished = aos_action_run_actions(rule->actions, 
			rule->actions_array.noe, rule->data, 
			rule_action_callback, rule, &rule->reg_id);
	aos_assert_r(all_finished >= 0, -1);

	return rule->success_rc;
}


static aos_rule_mf_t sg_mf = 
{
	aos_rule_hold,
	aos_rule_put,
	aos_rule_serialize,
	aos_rule_deserialize,
	aos_rule_destroy,
	aos_rule_dump,
	aos_rule_dump_to,
	aos_rule_event
};


int aos_rule_init(aos_rule_t *rule)
{
	aos_assert_r(rule, -1);
	aos_assert_r(!aos_dyn_array_init(&rule->actions_array, 
			(char **)&rule->actions, sizeof(aos_action_t *), 5, 
			AOS_RULE_MAX_ACTIONS), -1);
	rule->magic = AOS_RULE_MAGIC;
	return 0;
}


aos_rule_t *aos_rule_create_xml(struct aos_xml_node *node)
{
	aos_assert_r(node, 0);
	aos_rule_t *rule = aos_malloc(sizeof(aos_rule_t));
	aos_assert_r(rule, 0);
	memset(rule, 0, sizeof(aos_rule_t));
	rule->mf = &sg_mf;

	aos_assert_g(!rule->mf->deserialize(rule, node), cleanup);
	return rule;

cleanup:
	aos_free(rule);
	return 0;
}


aos_rule_t *aos_rule_factory(struct aos_xml_node *node)
{
	return aos_rule_create_xml(node);
}

