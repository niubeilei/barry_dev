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
// 03/11/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_event_msg_rcvd_h
#define aos_event_msg_rcvd_h

#include "event_c/event.h"
#include "util_c/types.h"

struct aos_buffer;

#define AOS_EVENT_MSG_RCVD_MEMFUNC_DECL			\


#define AOS_EVENT_MSG_RCVD_MEMDATA_DECL			\
	struct aos_field *			msg;			\
	u32							local_addr;		\
	u16							local_port;		\
	u32							remote_addr;	\
	u16							remote_port


typedef struct aos_event_msg_rcvd_mf
{
	AOS_EVENT_MEMFUNC_DECL;
	AOS_EVENT_MSG_RCVD_MEMFUNC_DECL;
} aos_event_msg_rcvd_mf_t;


typedef struct aos_event_msg_rcvd
{
	aos_event_msg_rcvd_mf_t *mf;

	AOS_EVENT_MEMDATA_DECL;
	AOS_EVENT_MSG_RCVD_MEMDATA_DECL;
} aos_event_msg_rcvd_t;

extern int aos_event_msg_rcvd_init(aos_event_msg_rcvd_t *event);
extern aos_event_msg_rcvd_t *aos_event_msg_rcvd_create_xml(
		struct aos_xml_node *node);
extern aos_event_msg_rcvd_t *aos_event_msg_rcvd_create(
		struct aos_field *msg, 
		const u32 local_addr, 
		const u16 local_port,
		const u32 remote_addr, 
		const u16 remote_port);
#endif

