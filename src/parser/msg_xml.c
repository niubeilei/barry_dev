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
// An XML message is a subclass of aos_xml_node. 
//
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/msg_xml.h"

#include "alarm_c/alarm.h"
#include "parser/field.h"
#include "util_c/memory.h"


static int aos_msg_xml_destroy(aos_field_t *msg)
{
	aos_assert_r(msg, -1);
	aos_assert_r(msg->type == eAosFieldType_XmlMsg, -1);
	aos_assert_r(!aos_xml_node_cleanup((aos_xml_node_t *)msg), -1);
	aos_free(msg);
	return 0;
}


static int aos_msg_xml_serialize(aos_field_t *msg, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		<XmlMessage>
	// 			...
	// 		</XmlMessage>
	// 	</Parent>
	//
	// 	'parent' points to the parent. 
	//
	aos_assert_r(msg, -1);
	aos_assert_r(parent, -1);

	aos_assert_r(!aos_xml_node_serialize(msg, parent), -1);
	return -1;
}


static int aos_msg_xml_deserialize(aos_field_t *msg, 
		aos_xml_node_t *node)
{
	// 
	// 	<XmlMessage>
	// 		...
	// 	</XmlMessage>
	//
	aos_assert_r(msg, -1);
	aos_assert_r(node, -1);

	aos_assert_r(!aos_xml_node_deserialize(msg, node), -1);
	return 0;
}


static int aos_msg_xml_dump_to(
		aos_field_t *msg, 
		char *buff, 
		int *len)
{
	aos_not_implemented_yet;
	return -1;
}


static const char * aos_msg_xml_dump(aos_field_t *msg)
{
	aos_not_implemented_yet;
	return 0;
}


static aos_msg_xml_mf_t sg_mf = 
{
	AOS_FIELD_MEMFUNC_INIT,
	aos_msg_xml_dump_to,
	aos_msg_xml_dump, 
	aos_msg_xml_destroy, 
	aos_msg_xml_serialize, 
	aos_msg_xml_deserialize,
	aos_msg_xml_integrity_check, 
	aos_msg_xml_parse,
	aos_xml_node_get_value,

	aos_xml_node_first_child,
	aos_xml_node_first_named_child,
	aos_xml_node_next_named_child,
	aos_xml_node_next_sibling,
	aos_xml_node_del_first_named_child,
	aos_xml_node_first_child_int,
	aos_xml_node_first_child_u32,
	aos_xml_node_first_child_int_dft,
	aos_xml_node_first_child_char,
	aos_xml_node_first_child_char_dft,
	aos_xml_node_first_child_str,
	aos_xml_node_first_child_str_b,
	aos_xml_node_first_child_str_dft,
	aos_xml_node_first_child_str_b_dft,
	aos_xml_node_first_child_hex,
	aos_xml_node_first_child_hex_dft,
	aos_xml_node_first_child_addr_dft,
	aos_xml_node_append_child_int,
	aos_xml_node_append_child_hex,
	aos_xml_node_append_child_str,
	aos_xml_node_append_child_bin,
	aos_xml_node_append_child_node
};


aos_msg_xml_t * aos_msg_xml_create(
		const char * const name, 
		const char * const contents, 
		const int len)
{
	aos_assert_r(name, 0);
	aos_assert_r(len >= 0, 0);

	aos_msg_xml_t *msg = aos_malloc(sizeof(aos_msg_xml_t));
	aos_assert_r(msg, 0);

	aos_assert_g(aos_xml_node_init((aos_xml_node_t *)msg, 
			eAosFieldType_XmlMsg, name, contents, len), 
			cleanup);
	msg->mf = &sg_mf;
	msg->mf->hold((aos_field_t *)msg);
	return msg;

cleanup:
	aos_free(msg);
	return 0;
}

