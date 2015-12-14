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
// This is the super class for all content selectors. A content 
// selector defines logic about how to select a message based on
// contents. This is used in parsing messages. When given a 
// buffer and schema, the schema needs to determine which schema
// message to use to parse the message. Which schema message to 
// use can be content dependent. This is the place where a content
// selector is used. 
//   
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/ms.h"

#include "alarm_c/alarm.h"
#include "parser/msg.h"
#include "parser/xml_node.h"
#include "parser/ms_fixed.h"
#include "parser/ms_enum.h"

aos_msg_t *aos_ms_select_msg(
		struct aos_msg_selector *ms, 
		struct aos_omparser *parser)
{
	aos_not_implemented_yet;
	return 0;
}


int aos_ms_serialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return 0;
}


int aos_ms_deserialize(
		struct aos_msg_selector *ms, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return 0;
}


int aos_msg_selector_init(
		aos_msg_selector_t *ms, 
		const aos_msg_selector_type_e type)
{
	aos_assert_r(ms, -1);
	aos_assert_r(!aos_msg_selector_type_check(type), -1);
	ms->type = type;
	return 0;
}


// 
// Description
// This function creates a message selector based on the 
// configuration defined in 'conf'. 
//
aos_msg_selector_t * aos_msg_selector_factory(
		struct aos_xml_node *conf)
{
	// 
	// 	<MessageSelector>
	//		<Name>
	//		...
	//	</MessageSelector>
	//
	aos_assert_r(conf, 0);

	if (conf->mf->match_name((aos_field_t *)conf, "MsgSelector_Fixed") == 1)
	{
		aos_ms_fixed_t *msg = aos_ms_fixed_create_xml(conf);
		aos_assert_r(msg, 0);
		return (aos_msg_selector_t *)msg;
	}

	if (conf->mf->match_name((aos_field_t *)conf, "MsgSelector_Enum") == 1)
	{
		aos_ms_enum_t *msg = aos_ms_enum_create_xml(conf);
		aos_assert_r(msg, 0);
		return (aos_msg_selector_t *)msg;
	}

	aos_alarm("Unrecognized message selector: %s", 
		conf->mf->dump((aos_field_t *)conf));
	return 0;
}


