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
#include "action_c/tcp_server.h"

#include "alarm_c/alarm.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "parser/xml_node.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"


int aos_act_tcp_server_serialize(
		struct aos_action *action, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Action_SendMsg>
	// 			<Name>
	// 			<Type>
	// 			...
	// 			<LocalAddr>		// Default: 0
	// 			<LocalPort>		// Default: 0
	// 			<LocalNumPorts>	// Default: 1
	// 			<Proto>			// Default: 0
	// 		</Action_SendMsg>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(action, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(action->type == eAosActionType_TcpServer, -1);

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "Action_SendMsg");
	aos_assert_r(node, -1);
	aos_assert_r(!aos_act_net_serialize(action, node), -1);

	return 0;
}


int aos_act_tcp_server_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(action->type == eAosActionType_TcpServer, -1);

	aos_assert_r(!aos_act_net_deserialize(action, node), -1);

	return 0;
}


int aos_act_tcp_server_destroy(struct aos_action *action)
{
	aos_assert_r(action, -1);

	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_act_tcp_server_release_memory(struct aos_action *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(!aos_act_net_release_memory(action), -1);
	aos_assert_r(action->type == eAosActionType_TcpServer, -1);

	return 0;
}


// 
// Description
// This function assumes the data to be sent is stored in:
// 	data to send: act_data->str[0] 
// 	data length:  act_data->int[0]
//
int aos_act_tcp_server_run(
		aos_action_t *action, 
		aos_gen_data_t *data)
{
	char *data_to_send;
	int len;
	u32 raddr;
	u32 rport;

	aos_assert_r(action, -1);
	aos_assert_r(data, -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_tcp_server_t *self = (aos_act_tcp_server_t *)action;

	aos_assert_r(!data->mf->get_str(data,
			eAosGenData_DataToSend, &data_to_send, 0), -1);
	aos_assert_r(!data->mf->get_int(data,
			eAosGenData_DataLen, &len), -1);
	switch (self->proto)
	{
	case eAosProto_Tcp:
		 aos_assert_r(!self->mf->send_tcp((aos_act_net_t *)self, 
				data_to_send, len), -1);
		 break;

	case eAosProto_Udp:
		 aos_assert_r(data->mf->get_u32(data, eAosGenData_RemoteAddr, &raddr), -1);
		 aos_assert_r(data->mf->get_u32(data, eAosGenData_RemotePort, &rport), -1);
		 aos_assert_r(!self->mf->send_udp((aos_act_net_t *)self, 
				data_to_send, len, raddr, rport), -1);
		 break;

	case eAosProto_Unix:
		 aos_not_implemented_yet;
		 return -1;

	default:
		 aos_alarm("Unrecognized protocol: %d", self->proto);
		 return -1;
	}

	return 0;
}


static int aos_act_tcp_server_send_tcp(
		struct aos_act_net *action, 
		const char * const msg, 
		const int len)
{
	aos_alarm("Should not call this function");
	return -1;
}


static int aos_act_tcp_server_send_udp(
		aos_act_net_t *action, 
		const char * const msg, 
		const int len, 
		const u32 remote_addr, 
		const u16 remote_port)
{
	aos_alarm("Should not call this function");
	return -1;
}


static int aos_act_tcp_server_disconnect(struct aos_act_net *action)
{
	aos_not_implemented_yet;
	return -1;
}


int aos_act_tcp_server_is_finished(aos_action_t *action)
{
	return 0;
}


int aos_act_tcp_server_finish_action(aos_action_t *action)
{
	return 0;
}


int aos_act_tcp_server_need_reg_event(aos_action_t *action)
{
	return 1;
}


static aos_act_tcp_server_mf_t sg_mf = 
{
	aos_action_hold,
	aos_action_put,
	aos_act_tcp_server_serialize,
	aos_act_tcp_server_deserialize,
	aos_act_tcp_server_destroy,
	aos_act_tcp_server_release_memory,
	aos_act_tcp_server_run,
	aos_act_tcp_server_is_finished,
	aos_act_tcp_server_finish_action,
	aos_act_tcp_server_need_reg_event,
	aos_action_is_blocking,
	aos_action_reset,
	aos_act_net_set_conn,
	aos_act_tcp_server_send_tcp,
	aos_act_tcp_server_send_udp,
	aos_act_tcp_server_disconnect
};

int aos_act_tcp_server_init(aos_act_tcp_server_t *sm)
{
	return 0;
}


aos_action_t * aos_act_tcp_server_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_act_tcp_server_t *act = aos_malloc(sizeof(aos_act_tcp_server_t));
	aos_assert_r(act, 0);
	memset(act, 0, sizeof(aos_act_tcp_server_t));
	act->mf = &sg_mf;

	aos_assert_g(!aos_act_net_init((aos_act_net_t *)act), cleanup);
	aos_assert_g(!aos_act_tcp_server_init(act), cleanup);
	aos_assert_g(!act->mf->deserialize((aos_action_t *)act, node), cleanup);
	return (aos_action_t *)act;

cleanup:
	aos_free(act);
	return 0;
}

