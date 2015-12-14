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
// 03/03/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "event_c/data_rcvd.h"

#include "alarm_c/alarm.h"
#include "parser/xml_node.h"
#include "porting_c/addr.h"
#include "util_c/memory.h"
#include "util_c/buffer.h"
#include "util_c/global_data.h"


static int aos_event_data_rcvd_serialize(
		struct aos_event *event, 
		struct aos_xml_node *parent)
{
	aos_not_implemented_yet;
	return -1;
}


static int aos_event_data_rcvd_deserialize(
		struct aos_event *event, 
		struct aos_xml_node *node)
{
	aos_not_implemented_yet;
	return -1;
}


static int aos_event_data_rcvd_release_memory(aos_event_t *event)
{
	aos_assert_r(event, -1);
	aos_assert_r(event->type == eAosEvent_DataRcvd, -1);
	aos_assert_r(!aos_event_release_memory(event), -1);
	aos_event_data_rcvd_t *self = (aos_event_data_rcvd_t *)event;

	if (self->buff) self->buff->mf->put(self->buff);
	return 0;
}


static int aos_event_data_rcvd_destroy(
		struct aos_event *event)
{
	aos_assert_r(event, -1);
	aos_assert_r(event->type == eAosEvent_DataRcvd, -1);
	aos_assert_r(!event->mf->release_memory(event), -1);

	aos_free(event);
	return 0;
}


const char * aos_event_data_rcvd_dump(aos_event_t *event)
{
	int buff_len, len, index;
	char * buff = aos_get_global_str1(&buff_len);
	aos_assert_r(buff, "Error");
	aos_assert_r(event, "Error");
	aos_assert_r(event->type == eAosEvent_DataRcvd, "Error");
	aos_event_data_rcvd_t *self = (aos_event_data_rcvd_t *)event;

	// Dump the aos_event_t portion
	len = buff_len;
	aos_assert_r(!aos_event_dump_to(event, buff, &len), 0);
	if (len >= buff_len) return buff;
	
	// Dump itself
	index = len;
	aos_buff_append_line_int(buff, buff_len, &index, "Buffer Len: ", 
			self->buff->data_len);
	aos_buff_append_line_str(buff, buff_len, &index, "Remote Addr: ", 
			aos_addr_to_str(self->remote_addr));
	aos_buff_append_line_int(buff, buff_len, &index, "Remote Port: ", 
			self->remote_port);
	return buff;
}


int aos_event_data_rcvd_dump_to(
		struct aos_event *event, 
		char *buff, 
		int *idx)
{
	aos_not_implemented_yet;
	return -1;
}


static aos_event_data_rcvd_mf_t sg_mf = 
{
	aos_event_hold,
	aos_event_put,
	aos_event_identity_check,
	aos_event_data_rcvd_release_memory,
	aos_event_data_rcvd_serialize,
	aos_event_data_rcvd_deserialize,
	aos_event_data_rcvd_destroy,
	aos_event_data_rcvd_dump,
	aos_event_data_rcvd_dump_to
};


int aos_event_data_rcvd_init(aos_event_data_rcvd_t *event)
{
	aos_assert_r(event, -1);
	memset(event, 0, sizeof(aos_event_data_rcvd_t));
	event->mf = &sg_mf;
	event->type = eAosEvent_DataRcvd;
	aos_assert_r(!aos_event_init((aos_event_t *)event), -1);

	return 0;
}


aos_event_data_rcvd_t *aos_event_data_rcvd_create(
		aos_buffer_t *buff, 
		const u32 remote_addr, 
		const u16 remote_port)
{
	aos_assert_r(buff, 0);
	aos_event_data_rcvd_t *obj = aos_malloc(sizeof(aos_event_data_rcvd_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_event_data_rcvd_init(obj), cleanup);

	buff->mf->hold(buff);
	obj->buff = buff;
	obj->remote_addr = remote_addr;
	obj->remote_port = remote_port;
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


aos_event_data_rcvd_t *aos_event_data_rcvd_create_xml(aos_xml_node_t *node)
{
	aos_event_data_rcvd_t *obj = aos_malloc(sizeof(aos_event_data_rcvd_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_event_data_rcvd_init(obj), cleanup);

	aos_assert_g(obj->mf->deserialize((aos_event_t *)obj, node), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


