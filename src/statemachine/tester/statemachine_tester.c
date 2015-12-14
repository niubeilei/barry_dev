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
// 03/04/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/tester/statemachine_tester.h"

#include "alarm/Alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "statemachine/state.h"
#include "statemachine/statemachine.h"
#include "util/memory.h"
#include <stdio.h>


extern int aos_statemachine_torturer();
extern int aos_statemachine_special_test_cases();

int aos_statemachine_tester()
{
	aos_assert_r(!aos_statemachine_special_test_cases(), -1);
	aos_assert_r(!aos_statemachine_torturer(), -1);
	return 0;
}


int aos_statemachine_special_tc1()
{
	aos_xml_node_t *root = aos_xml_node_create_from_file("sm.xml");
	aos_assert_r(root, -1);
	root->mf->hold((aos_field_t *)root);
	aos_xml_node_t *node = root->mf->first_named_child(root, "Statemachine");
	aos_assert_r(node, -1);
	aos_statemachine_t *state = aos_statemachine_create_xml(node);
	aos_assert_r(state, -1);

	aos_assert_r(!state->mf->start(state), -1);

	root->mf->put((aos_field_t *)root);

	while (1)
	{
		aos_sleep(1);
	}
	return 0;
}


int aos_statemachine_special_test_cases()
{
	aos_assert_r(!aos_statemachine_special_tc1(), -1);
	return 0;
}


int aos_statemachine_torturer()
{
	return 0;
} 

