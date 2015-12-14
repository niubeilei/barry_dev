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
#include "event_c/event.h"

#include "alarm_c/alarm.h"
#include "event_c/types.h"
#include "event_c/data_rcvd.h"
#include "parser/xml_node.h"
#include "util_c/rc_obj.h"
#include "util_c/memory.h"

AOS_DECLARE_RCOBJ_LOCK;


int aos_event_identity_check(aos_event_t *event)
{
	aos_assert_r(event, -1);
	return event->magic == AOS_EVENT_MAGIC;
}


int aos_event_hold(aos_event_t *event)
{
	AOS_RC_OBJ_HOLD(event);
}

 
int aos_event_put(aos_event_t *event)
{
	AOS_RC_OBJ_PUT(event);
}

 
int aos_event_serialize(
		aos_event_t *event, 
		aos_xml_node_t *node)
{
	// 
	// 	<a_name>
	// 		<Type>			// Mandatory
	// 		<Event_id>		// Default: 0
	// 	</a_name>
	//
	aos_assert_r(event, -1);
	aos_assert_r(node, -1);
	
	// Set <Type>
	aos_assert_r(!node->mf->append_child_str(node, "Type", 
			aos_event_2str(event->type), 0, 0), -1);

	// Set <Name>
	// if (event->name)
	// {
	// 	aos_assert_r(!node->mf->append_child_str(node, "Name", 
	// 		event->name), -1);
	// }

	// Set <event_id>
	aos_assert_r(!node->mf->append_child_int(node, "event_id", 
			event->event_id), -1);

	return 0;
}


int aos_event_deserialize(
		struct aos_event *event, 
		struct aos_xml_node *node)
{
	aos_assert_r(event, -1);
	aos_assert_r(node, -1);

	// Retrieve <Type>
	char buff[100];
	int buff_len = 100;
	aos_assert_r(!node->mf->first_child_str_b(node, 
			"Type", buff, &buff_len, 0), -1);
	event->type = aos_event_2enum(buff);
	aos_assert_r(aos_event_check(event->type) == 1, -1);

	// Retrieve <Name>
	// int name_len = 0;
	// aos_assert_r(!node->mf->first_child_str_dft(node, "Name", 
	// 			&event->name, &name_len, 0), -1);

	// Retrieve <event_id>
	aos_assert_r(!node->mf->first_child_int_dft(node, "event_id", 
				&event->event_id, 0), -1);

	return 0;
}


int aos_event_release_memory(aos_event_t *event)
{
	aos_assert_r(event, -1);
	return 0;
}


int aos_event_destroy(aos_event_t *event)
{
	aos_assert_r(event, -1);
	aos_assert_r(!event->mf->release_memory(event), -1);
	aos_free(event);
	return 0;
}


const char * aos_event_dump(
		struct aos_event *event)
{
	aos_not_implemented_yet;
	return "Not implemented yet";
}


int aos_event_dump_to(
		struct aos_event *event, 
		char *buff, 
		int *idx)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_event_mf_t sg_mf = 
{
	aos_event_hold,
	aos_event_put,
	aos_event_identity_check,
	aos_event_release_memory,
	aos_event_serialize,
	aos_event_deserialize,
	aos_event_destroy,
	aos_event_dump,
	aos_event_dump_to
};

int aos_event_init(aos_event_t *event)
{
	aos_assert_r(event, -1);
	event->mf = &sg_mf;
	event->magic = AOS_EVENT_MAGIC;
	AOS_INIT_LIST_HEAD(&event->link);
	return 0;
}


int aos_event_create(
		aos_event_t **event, 
		const aos_event_e type, 
		const int match_data)
{
	aos_assert_r(event, -1);
	*event = 0;

	aos_event_t *ee = aos_malloc(sizeof(*ee));
	aos_assert_r(ee, -1);
	memset(ee, 0, sizeof(*ee));
	aos_assert_g(!aos_event_init(ee), cleanup);

	ee->type = type;
	ee->match_data = match_data;
	*event = ee;
	return 0;

cleanup:
	*event = 0;
	aos_free(ee);
	return -1;
}


aos_event_t *aos_event_factory(struct aos_xml_node *node)
{
	aos_assert_r(node, 0);

	char buff[100];
	int buff_len = 100;
	aos_event_t *obj = 0;

	aos_assert_r(!node->mf->first_child_str_b(node, "Type", 
				buff, &buff_len, 0), 0);

	if (strcmp(buff, "DataRcvd") == 0)
	{
		obj = (aos_event_t *)aos_event_data_rcvd_create_xml(node);
		aos_assert_r(obj, 0);
		return obj;
	}	
	
	aos_alarm("Unrecognized event: %s", buff);
	return 0;
}


