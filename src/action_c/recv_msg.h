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
// 02/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef aos_omni_action_recv_msg_h
#define aos_omni_action_recv_msg_h

#include "action_c/net.h"
#include "action_c/types.h"
#include "net/conn.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"

struct aos_act_recv_msg;
struct aos_scm_field;
struct aos_read_thrd_mgr;
struct aos_scm_field;

#define AOS_ACT_RECV_MSG_MEMFUNC_DECL					\

#define AOS_ACT_RECV_MSG_MEMDATA_DECL					\
	aos_action_type_e 			recv_handler;			\
	struct aos_scm_field *		schema;					\
	aos_conn_read_callback_t	callback;				\
	struct aos_read_thrd_mgr *	thrd_mgr;				\
	char *						to_clipboard;			\
	char *						from_clipboard;			\
	aos_action_type_e			msg_handler;			\
	char *						msg_hd_id


typedef struct aos_act_recv_msg_mf
{
	AOS_ACTION_MEMFUNC_DECL;
	AOS_ACT_NET_MEMFUNC_DECL;
	AOS_ACT_RECV_MSG_MEMFUNC_DECL;
} aos_act_recv_msg_mf_t;

typedef struct aos_act_recv_msg
{
	aos_act_recv_msg_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
	AOS_ACT_NET_MEMDATA_DECL;
	AOS_ACT_RECV_MSG_MEMDATA_DECL;
} aos_act_recv_msg_t;

extern int aos_act_recv_msg_init(aos_act_recv_msg_t *sm);
extern aos_action_t * aos_act_recv_msg_create_xml(
		struct aos_xml_node *node);
extern aos_action_t * aos_act_recv_msg_create();
#endif

