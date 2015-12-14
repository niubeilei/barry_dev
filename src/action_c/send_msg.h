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
// 02/22/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_action_send_msg_h
#define aos_omni_action_send_msg_h

#include "action_c/net.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"

struct aos_act_send_msg;

#define AOS_ACT_SEND_MSG_MEMFUNC_DECL					\

#define AOS_ACT_SEND_MSG_MEMDATA_DECL					\
	char *					to_clipboard;				\
	char *					from_clipboard;				\
	aos_data_src_e			data_src;					\
	char *					data_name;					\
	char *					data_to_send;				\
	int						data_len

typedef struct aos_act_send_msg_mf
{
	AOS_ACTION_MEMFUNC_DECL;
	AOS_ACT_NET_MEMFUNC_DECL;
	AOS_ACT_SEND_MSG_MEMFUNC_DECL;
} aos_act_send_msg_mf_t;

typedef struct aos_act_send_msg
{
	aos_act_send_msg_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
	AOS_ACT_NET_MEMDATA_DECL;
	AOS_ACT_SEND_MSG_MEMDATA_DECL;
} aos_act_send_msg_t;

extern int aos_act_send_msg_init(aos_act_send_msg_t *sm);
extern aos_action_t * aos_act_send_msg_create_xml(
		struct aos_xml_node *node);

#endif

