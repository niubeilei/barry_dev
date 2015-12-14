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
// 02/15/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "rvg_c/rig.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "random_c/random_util.h"
#include "rvg_c/rig_basic.h"
#include "rvg_c/rig_unique.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include <stdio.h>


int aos_rig_serialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	aos_assert_r(!aos_rvg_serialize(rvg, node), -1);
	return 0;
}


int aos_rig_deserialize(
		struct aos_rvg *rvg, 
		struct aos_xml_node *node)
{
	aos_assert_r(!aos_rvg_deserialize(rvg, node), -1);
	return 0;
}


int aos_rig_integrity_check(
		struct aos_rvg *rvg)
{
	aos_assert_r(aos_rvg_integrity_check(rvg) == 1, -1);
	return 1;
}


// 
// This is a virtual function. By default, it does nothing.
// This will be overridden by rig_unique. 
//
int aos_rig_reset_unique(aos_rig_t *rig)
{
	return 0;
}


int aos_rig_release_memory(aos_rvg_t *rig)
{
	aos_assert_r(!aos_rvg_release_memory(rig), -1);
	return 0;
}


int aos_rig_init(aos_rig_t *rig)
{
	aos_assert_r(rig, -1);
	aos_assert_r(!aos_rvg_init((aos_rvg_t *)rig), -1);
	return 0;
}


aos_rig_t *aos_rig_factory(struct aos_xml_node *node)
{
	char buff[100];
	int buff_len = 100;
	aos_assert_r(node, 0);

	aos_assert_rm(!node->mf->first_child_str_b(
			node, "Type", buff, &buff_len, 0), 0,
			"Expecting the <Type> tag but not found: %s", 
			node->contents);

	if (strcmp(buff, "RigBasic") == 0)
	{
		aos_rig_t *t = (aos_rig_t *)aos_rig_basic_create_xml(node);
		aos_assert_r(t, 0);
		return t;
	}

	if (strcmp(buff, "RigUnique") == 0)
	{
		aos_rig_t *t = (aos_rig_t *)aos_rig_unique_create_xml(node);
		aos_assert_r(t, 0);
		return t;
	}

	aos_alarm("Unrecognized tag: %s. Node: %s", 
			buff, node->mf->dump((aos_field_t *)node));
	return 0;
}


