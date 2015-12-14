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
#include "action_c/net.h"

#include "alarm_c/alarm.h"
#include "net/comm_mgr.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "parser/xml_node.h"
#include "util_c/limit.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"


int aos_act_net_serialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// 	<A-Name>
	// 		<Name>
	// 		<Type>
	// 		<LocalAddr>			// Default: 0
	// 		<LocalPort>			// Default: 0
	// 		<LocalNumPorts>		// Default: 1
	// 		<RemoteAddr>		// Default: 0
	// 		<RemotePort>		// Default: 0
	// 		<UnitPath>			// Default: 0
	// 		<Proto>				// Default: 0
	// 		<SendingBlocking>	// Default: 0
	// 	</A-Name>
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(aos_action_type_is_net(action->type) == 1, -1);
	aos_act_net_t *self = (aos_act_net_t *)action;
	aos_assert_r(node, -1);
	aos_assert_r(!aos_action_serialize(action, node), -1);

	// Set <LocalAddr>
	if (self->local_addr)
	{
		aos_assert_r(!node->mf->append_child_addr(node, "LocalAddr", 
				self->local_addr), -1);
	}

	// Set <LocalPort>
	if (self->local_port)
	{
		aos_assert_r(!node->mf->append_child_int(node, "LocalPort", 
				self->local_port), -1);
	}

	// Set <LocalNumPorts>
	if (self->local_num_ports > 1)
	{
		aos_assert_r(!node->mf->append_child_int(node, "LocalNumPorts", 
				self->local_num_ports), -1);
	}

	// Set <RemoteAddr>
	if (self->remote_addr)
	{
		aos_assert_r(!node->mf->append_child_addr(node, "RemoteAddr", 
				self->remote_addr), -1);
	}

	// Set <RemotePort>
	if (self->remote_port)
	{
		aos_assert_r(!node->mf->append_child_int(node, "RemotePort", 
				self->remote_port), -1);
	}

	// Set <RemoteNumPorts>
	if (self->remote_num_ports > 1)
	{
		aos_assert_r(!node->mf->append_child_int(node, "RemoteNumPorts",
				self->remote_num_ports), -1);
	}

	// Set <UnitPath>
	if (self->unix_path)
	{
		aos_assert_r(!node->mf->append_child_str(node, "UnixPath", 
				self->unix_path, 0, 0), -1);
	}

	// Set <Proto>
	if (self->proto > eAosProto_Undefined)
	{
		aos_assert_r(!node->mf->append_child_str(node, "Proto", 
				aos_proto_2str(self->proto), 0, 0), -1);
	}

	// Set <SendBlocking>
	if (self->send_blocking)
	{
		aos_assert_r(!node->mf->append_child_int(node, "SendBlocking", 
				self->send_blocking), -1);
	}

	return 0;
}


int aos_act_net_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(aos_action_type_is_net(action->type) == 1, -1);
	aos_act_net_t *self = (aos_act_net_t *)action;

	aos_assert_r(!aos_action_deserialize(action, node), -1);

	// Retrieve <LocalAddr>
	aos_assert_r(!node->mf->first_child_addr_dft(node, "LocalAddr", 
				&self->local_addr, 0), -1);

	// Retrieve <LocalPort>
	int port;
	aos_assert_r(!node->mf->first_child_int_dft(node, "LocalPort", 
				&port, 0), -1);
	aos_assert_r(port >= 0 && port <= AOS_U16_MAX, -1);
	self->local_port = (u16)port;

	// Retrieve <LocalNumPorts>
	aos_assert_r(!node->mf->first_child_int_dft(node, "LocalNumPorts", 
				&self->local_num_ports, 1), -1);
	aos_assert_r(self->local_num_ports > 0, -1);

	// Retrieve <RemoteAddr>
	aos_assert_r(!node->mf->first_child_addr_dft(node, "RemoteAddr", 
				&self->remote_addr, 0), -1);

	// Retrieve <RemotePort>
	aos_assert_r(!node->mf->first_child_int_dft(node, "RemotePort", 
				&port, 0), -1);
	aos_assert_r(port >= 0 && port <= AOS_U16_MAX, -1);
	self->remote_port = (u16)port;

	// Retrieve <RemoteNumPorts>
	aos_assert_r(!node->mf->first_child_int_dft(node, "RemoteNumPorts", 
				&self->remote_num_ports, 1), -1);

	// Retrieve <Proto>
	char buff[101];
	int buff_len = 101;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, "Proto", 
				buff, &buff_len, "Undefined"), -1);
	self->proto = aos_proto_2enum(buff);
	aos_assert_r(aos_proto_check(self->proto) == 1, -1);

	// Retrieve <UnixSock>
	int len;
	aos_assert_r(!node->mf->first_child_str_dft(node, "UnixPath", 
				&self->unix_path, &len, 0), -1);

	// Retrieve <SendBlocking>
	aos_assert_r(!node->mf->first_child_char_dft(node, "SendBlocking", 
				&self->send_blocking, 0), -1);

	return 0;
}


int aos_act_net_destroy(struct aos_action *action)
{
	aos_assert_r(action, -1);

	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_act_net_release_memory(struct aos_action *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(!aos_action_release_memory(action), -1);
	aos_assert_r(aos_action_type_is_net(action->type) == 1, -1);
	aos_act_net_t *self = (aos_act_net_t *)action;

	if (self->unix_path) aos_free(self->unix_path);
	self->unix_path = 0;

	if (self->lock) aos_free(self->lock);
	self->lock = 0;

	return 0;
}


int aos_act_net_set_conn(
		struct aos_act_net *action, 
		aos_conn_t *conn)
{
	aos_assert_r(action, -1);
	aos_assert_r(conn,-1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_net_t *self = (aos_act_net_t *)action;
	self->conn = conn;
	return 0;
}


static int aos_act_net_create_udp(aos_act_net_t *action)
{
	aos_assert_r(action, -1);
	if (action->conn && action->conn->sock > 0) return 0;

	aos_assert_r(action->local_addr, -1);
	aos_assert_r(action->local_num_ports > 0, -1);

	if (!action->conn)
	{
		action->conn = (aos_conn_t *)aos_udp_create(
					action->local_addr, action->local_port, 
					action->local_num_ports);
		aos_assert_r(action->conn, -1);
	}

	aos_assert_r(!action->conn->mf->connect(action->conn), -1);		
	return 0;
}


static int aos_act_net_create_tcp(aos_act_net_t *action)
{
	aos_assert_r(action, -1);
	if (action->conn && action->conn->sock > 0) return 0;

	aos_assert_r(action->local_addr, -1);
	aos_assert_r(action->local_num_ports > 0, -1);
	aos_assert_r(action->remote_addr, -1);
	aos_assert_r(action->remote_port, -1);
	aos_assert_r(action->remote_num_ports > 0, -1);

	if (!action->conn)
	{
		action->conn = (aos_conn_t *)aos_tcp_create(
				action->local_addr, action->local_port,
				action->local_num_ports, 
				action->remote_addr, action->remote_port, 
				action->remote_num_ports);
		aos_assert_r(action->conn, -1);
	}

	aos_assert_r(!action->conn->mf->connect(action->conn), -1);
	return 0;
}


int aos_act_net_send_tcp(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len)
{
	aos_assert_r(action, -1);
	aos_assert_r(msg, -1);

	aos_assert_r(len > 0, -1);

	if (!action->conn || action->conn->sock <= 0)
	{
		aos_assert_r(!aos_act_net_create_tcp(action), -1);
	}

	aos_assert_r(!aos_tcp_send_raw(action->conn->sock, msg, len, 
			action->send_blocking), -1);

	return 0;
}


int aos_act_net_send_udp(
		aos_act_net_t *action, 
		const char * const msg, 
		const int len, 
		const u32 remote_addr, 
		const u16 remote_port
		)
{
	aos_assert_r(action, -1);
	aos_assert_r(msg, -1);
	aos_assert_r(len > 0, -1);
	aos_assert_r(remote_addr, -1);
	aos_assert_r(remote_port, -1);

	if (!action->conn || action->conn->sock <= 0)
	{
		aos_assert_r(!aos_act_net_create_udp(action), -1);
	}
	u32 raddr = (remote_addr)?remote_addr:action->remote_addr;
	u16 rport = (remote_port)?remote_port:action->remote_port;
	aos_assert_r(!aos_udp_send(action->conn->sock, msg, len, 
					action->local_addr, action->local_port, 
					raddr, rport), -1);
	return 0;
}


int aos_act_net_connect(aos_act_net_t *action)
{
	aos_assert_r(action, -1);

	if (action->conn && action->conn->sock > 0) return 0;

	switch (action->proto)
	{
	case eAosProto_Tcp: 
		 aos_assert_r(!aos_act_net_create_tcp(action), -1);
		 break;

	case eAosProto_Udp: 
		 aos_assert_r(!aos_act_net_create_udp(action), -1);
		 break;

	case eAosProto_Unix:
		 aos_not_implemented_yet;
		 return -1;

	default:
		 aos_alarm("Unrecognized protocol: %d", action->proto);
		 return -1;
	}

	return 0;
}


/*int aos_act_net_recv(
		aos_act_net_t *action, 
		aos_action_t *callback)
{
	aos_assert_r(action, -1);
	aos_assert_r(action->conn, -1);
	aos_assert_r(action->conn->sock > 0, -1);

	if (callback)
	{
		aos_assert_r(!aos_comm_mgr_recv_msg(action->conn->sock, callback), -1);
	}
	else
	{
		aos_assert_r(action->recv_msg_handler, -1);
		aos_assert_r(!aos_comm_mgr_recv_msg(action->sock, action->recv_msg_handler), -1);
	}

	return 0;
}
*/


int aos_act_net_disconnect(struct aos_act_net *action)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_act_net_init(aos_act_net_t *sm)
{
	return aos_action_init((aos_action_t *)sm);
}

