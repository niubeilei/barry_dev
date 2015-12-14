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
// A Message is a subclass of Field. In other word, a message is also
// a field, but a field may not be a message. This is because a field
// may contain fields as its member. This is exactly what a message
// is supposed to be. 
//   
// The purposes of having this class is to have an object that is 
// specially designed for messages since messages may be different
// from a field. For instance, many messages contain message level 
// information, and then a list of fields.
//
// This class is intended to be the super class of all messages. 
//
// Modification History:
// 01/30/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/msg.h"

#include "alarm_c/alarm.h"
#include "parser/xml.h"
#include "parser/xml_node.h"
#include "parser/msg.h"
#include "parser/parser.h"
#include "util_c/memory.h"
#include "util_c/rc.h"
#include "util_c/strutil.h"


/*
// 
// Description
// This function serialize a msg into the xml node 'node'.
// Note that this function is normally called by its derived
// function. 
//
int aos_msg_serialize(
		aos_msg_t *msg, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Message>
	// 			<Name>
	// 			<Fields>
	// 				<Field>
	// 					...
	// 				</Field>
	// 				...
	// 				<Field>
	// 					...
	// 				</Field>
	// 			</Fields>
	// 			...
	// 		</Message>
	// 		...
	// 	</Parent>
	//
	int i;
	aos_field_t *field;
	aos_xml_node_t *fn;
	aos_assert_r(msg, -1);
	aos_assert_r(parent, -eAosRc_Error);
	aos_assert_r(msg->name, -1);
	aos_assert_r(msg->nof >= 0, -eAosRc_Error);

	fn = parent->mf->append_child_node(parent, msg->name);
	aos_assert_r(fn, -eAosRc_Error);

	for (i=0; i<msg->nof; i++)
	{
		field = msg->fields[i];
		aos_assert_r(!field->mf->serialize(field, fn), -eAosRc_Error);
	}

	return 0;
}


int aos_msg_deserialize(
		aos_msg_t *msg, 
		aos_xml_node_t *node)
{
	// 
	// 	<Message>
	// 		<Name>
	// 		<Fields>
	// 			<Field>
	// 				...
	// 			</Field>
	// 			...
	// 			<Field>
	// 				...
	// 			</Field>
	// 		</Fields>
	// 		...
	// 	</Message>
	//
	int len;
	aos_xml_node_t *fields, *fn;
	aos_field_t *field;
	aos_assert_r(msg, -eAosRc_Error);
	aos_assert_r(node, -eAosRc_Error);
	aos_assert_r(node->mf->match_label(node, "Message") == 1, 
			-eAosRc_Error);

	if (msg->name) aos_free(msg->name);
	aos_assert_r(!node->mf->first_child_str(node, "Name", 
			&msg->name, &len), -eAosRc_Error);

	fields = node->mf->first_named_child(node, "Fields");
	if (!fields) return 0;

	fn = fields->mf->first_child(node);
	for (; fn; fn = node->mf->next_sibling(node))
	{
		field = aos_field_factory(fn);
		aos_assert_r(field, -eAosRc_Error);
		aos_assert_r(!msg->mf->add_field(msg, field), -eAosRc_Error);
	}
	return 0;
}
*/


// 
// Description
// This function frees all the memory this class allocates. Note that
// it does not free the memory for the message itself. The message 
// itself should be released by its derived classes.
//
int aos_msg_destroy(aos_msg_t *msg)
{
	// 
	// Nothing to do at this moment.
	//
	return 0;
}


// 
// Description
// This function initializes the member data of this class. 
//
int aos_msg_init(aos_msg_t *msg) 
{
	// 
	// Nothing to do at this moment.
	//
	return 0;
}

