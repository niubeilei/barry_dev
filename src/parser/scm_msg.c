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
// 01/31/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "parser/scm_msg.h"

#include "alarm_c/alarm.h"
#include "parser/parser.h"
#include "parser/scm_field.h"
#include "parser/xml_node.h"
#include "util_c/strutil.h"
#include "util_c/memory.h"



// Description
// It releases the memory for 'msg'. The following is released:
// 		a. msg->fields
// 		b. the message itself
// Once this function is called, 'msg' is no longer valid. Do not
// use 'msg' after calling this function.
//
// Returns
// 0 if success
// -1 if errors
//
int aos_scm_msg_destroy(aos_scm_field_t *msg)
{
	// 
	// Nothing to do now.
	//
	return 0;
}


/*
int aos_scm_msg_serialize(aos_scm_msg_t *msg, 
		aos_xml_node_t *parent)
{
	// 
	// 	<Parent>
	// 		<MessageSchema>
	// 			<Name>
	// 			<Fields>
	// 				<Field>...</Field>
	// 				<Field>...</Field>
	// 				...
	// 			</Fields>
	// 		</MessageSchema>
	// 	</Parent>
	//
	int i, ret;
	aos_assert_r(msg, -1);
	aos_assert_r(parent, -1);

	aos_xml_node_t *child = parent->mf->append_child_node(parent, "MessageSchema");
	aos_assert_r(child, -1);

	for (i=0; i<msg->noe; i++)
	{
		ret = msg->fields[i]->mf->serialize(msg->fields[i], child);
		aos_assert_r(!ret, -1);
	}

	return 0;
}


int aos_scm_msg_deserialize(aos_scm_msg_t *msg, 
		aos_xml_node_t *node)
{
	// 
	// 	<MessageSchema>
	// 		<Name>
	// 		<Fields>
	// 			<Field>...</Field>
	// 			<Field>...</Field>
	// 			...
	// 			<Field>...</Field>
	// 		</Fields>
	// 	</MessageSchema>
	//
	int ret, len;
	aos_assert_r(node, -1);
	aos_assert_r(msg, -1);
	if (msg->name) aos_free(msg->name);
	ret = node->mf->first_child_str(node, "Name", &msg->name, &len);
	aos_assert_r(!ret, -1);

	aos_xml_node_t *fields = node->mf->first_named_child(node, "Fields");
	aos_assert_r(fields, -1);
	aos_xml_node_t *fnode = fields->mf->first_child(fields);
	while (fnode)
	{
		aos_scm_field_t *ff = aos_scm_field_factory(fnode);
		aos_assert_r(ff, -1);
		msg->mf->add_field(msg, ff);
		fnode = fields->mf->next_sibling(fields);
	}

	return 0;
}
*/


// 
// Description
// It initializes 'aos_scm_msg_t' portion. The caller should have 
// called memset(msg, 0, ...). 
//
// Returns
// 0 if success. 
// -1 if errors.
//
int aos_scm_msg_init(aos_scm_msg_t *msg) 
{
	// 
	// Currently there is nothing to do.
	//
	return 0;
}


