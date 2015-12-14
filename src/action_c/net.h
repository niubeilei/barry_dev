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
#ifndef aos_omni_action_net_h
#define aos_omni_action_net_h

#include "action_c/action.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"

struct aos_act_net;
struct aos_conn;

typedef int (*aos_act_net_set_conn_t)(
		struct aos_act_net *action, 
		struct aos_conn *conn);

typedef int (*aos_act_net_send_tcp_t)(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len);

typedef int (*aos_act_net_send_udp_t)(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len, 
		const u32 raddr, 
		const u16 rport);

typedef int (*aos_act_net_connect_t)(
 		struct aos_act_net *action);

typedef int (*aos_act_net_disconnect_t)(
		struct aos_act_net *action);

#define AOS_ACT_NET_MEMFUNC_DECL					\
	aos_act_net_set_conn_t		set_conn;			\
	aos_act_net_send_tcp_t		send_tcp;			\
	aos_act_net_send_udp_t		send_udp;			\
	aos_act_net_connect_t		connect;			\
	aos_act_net_disconnect_t	disconnect

#define AOS_ACT_NET_MEMDATA_DECL					\
	struct aos_conn *		conn;					\
	u32						local_addr;				\
	u16						local_port;				\
	int						local_num_ports;		\
	u32						remote_addr;			\
	u16						remote_port;			\
	int						remote_num_ports;		\
	char *					unix_path;				\
	aos_proto_e				proto;					\
	aos_lock_t *			lock;					\
	char					send_blocking;			\
	u16						local_port_used;		\
	u16						remote_port_used;		\
	struct aos_action *		recv_msg_handler


typedef struct aos_act_net_mf
{
	AOS_ACTION_MEMFUNC_DECL;
	AOS_ACT_NET_MEMFUNC_DECL;
} aos_act_net_mf_t;

typedef struct aos_act_net
{
	aos_act_net_mf_t *mf;

	AOS_ACTION_MEMDATA_DECL;
	AOS_ACT_NET_MEMDATA_DECL;
} aos_act_net_t;

extern int aos_act_net_init(aos_act_net_t *sm);

extern int aos_act_net_serialize(
		aos_action_t *action, 
		struct aos_xml_node *node);

extern int aos_act_net_deserialize(
		aos_action_t *action, 
		struct aos_xml_node *node);

extern int aos_act_net_release_memory(struct aos_action *action);

extern int aos_act_net_set_conn(
		struct aos_act_net *action, 
		struct aos_conn *conn);

extern int aos_act_net_send_tcp(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len);

extern int aos_act_net_send_udp(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len, 
		const u32 raddr, 
		const u16 rport);

extern int aos_act_net_connect(struct aos_act_net *action); 

extern int aos_act_net_disconnect(
		struct aos_act_net *action);

#endif

