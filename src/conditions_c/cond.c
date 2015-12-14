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
// 02/12/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "conditions_c/cond.h"

#include "alarm_c/alarm.h"
#include "conditions_c/const.h"
#include "parser/xml_node.h"
#include "util_c/rc_obj.h"


AOS_DECLARE_RCOBJ_LOCK;


int aos_cond_serialize(
		aos_cond_t *cond, 
		aos_xml_node_t *node)
{
	// 
	//	<Cond>
	//		<Type>
	//		...
	//	</Cond>
	//
	aos_assert_r(cond, -1);
	aos_assert_r(node, -1);

	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, "Type", 
			aos_cond_type_2str(cond->type), 0, 0), -1);

	return 0;
}


int aos_cond_deserialize(
		aos_cond_t *cond, 
		aos_xml_node_t *node)
{
	aos_assert_r(cond, -1);
	aos_assert_r(node, -1);

	char type[100];
	int type_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(
				node, "Type", type, &type_len, 0), -1);
	cond->type = aos_cond_type_2enum(type);
	aos_assert_r(aos_cond_type_check(cond->type) == 1, -1);
	return 0;
}


int aos_cond_hold(aos_cond_t *cond)
{
	AOS_RC_OBJ_HOLD(cond);
}


int aos_cond_put(aos_cond_t *cond)
{
	AOS_RC_OBJ_PUT(cond);
}


int aos_cond_release_memory(aos_cond_t *cond)
{
	return 0;
}


int aos_cond_init(aos_cond_t *cond)
{
	return 0;
}


aos_cond_t * aos_cond_factory(struct aos_xml_node *node)
{
	aos_assert_r(node, 0);

	char type[100];
	int type_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(
			node, "Type", type, &type_len, 0), 0);
	if (strcmp(type, "Const") == 0)
	{
		return aos_cond_const_create_xml(node);
	}

	aos_alarm("Unrecognized condition: %s", type);
	return 0;
}

