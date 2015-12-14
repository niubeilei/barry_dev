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
// 02/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/tester/state_tester.h"

#include "alarm/Alarm.h"
#include "parser/parser.h"
#include "parser/xml_node.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "statemachine/state.h"
#include "util/memory.h"
#include <stdio.h>


extern int aos_state_torturer();
extern int aos_state_special_test_cases();

int aos_state_tester()
{
	aos_assert_r(!aos_state_special_test_cases(), -1);
	aos_assert_r(!aos_state_torturer(), -1);
	return 0;
}


int aos_state_special_tc1()
{
	aos_xml_node_t *node = aos_xml_node_create_from_file("tc1.xml");
	aos_assert_r(node, -1);
	node->mf->hold((aos_field_t *)node);
	aos_state_t *state = aos_state_factory(node);
	aos_assert_r(state, -1);
	node->mf->put((aos_field_t *)node);
	return 0;
}


int aos_state_special_test_cases()
{
	aos_assert_r(!aos_state_special_tc1(), -1);
	return 0;
}


int aos_state_torturer()
{
	return 0;
} 

