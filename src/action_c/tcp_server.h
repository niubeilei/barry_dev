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
// 02/25/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_action_tcp_server_h
#define aos_omni_action_tcp_server_h

#include "action_c/net.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"

struct aos_act_tcp_server;

#define AOS_ACT_TCP_SERVER_MEMFUNC_DECL					\

#define AOS_ACT_TCP_SERVER_MEMDATA_DECL					\


typedef struct aos_act_tcp_server_mf
{
	AOS_ACTION_MEMFUNC_DECL;
	AOS_ACT_NET_MEMFUNC_DECL;
	AOS_ACT_TCP_SERVER_MEMFUNC_DECL;
} aos_act_tcp_server_mf_t;

typedef struct aos_act_tcp_server
{
	aos_act_tcp_server_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
	AOS_ACT_NET_MEMDATA_DECL;
	AOS_ACT_TCP_SERVER_MEMDATA_DECL;
} aos_act_tcp_server_t;

extern int aos_act_tcp_server_init(aos_act_tcp_server_t *sm);
extern aos_action_t * aos_act_tcp_server_create_xml(
		struct aos_xml_node *node);

#endif

