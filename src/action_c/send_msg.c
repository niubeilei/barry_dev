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
#include "action_c/send_msg.h"

#include "alarm_c/alarm.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "parser/xml_node.h"
#include "util_c/memory.h"
#include "util_c/tracer.h"
#include "util_c/gen_data.h"
#include "util_c/clipboard.h"
#include "util_c/file.h"


int aos_act_send_msg_serialize(
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
	// 			<RemoteAddr>	// Default: 0
	// 			<RemotePort>	// Default: 0
	// 			<UnitPath>		// Default: 0
	// 			<Proto>			// Default: 0
	// 			<ToClipboard>	// Optional
	// 			<FromClipboard>	// Optional
	// 			<DataToSend>	// Optional
	// 			<DataLength>	// Optional
	// 			<FinishMethod>	// Default: Immediate
	// 		</Action_SendMsg>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(action, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_send_msg_t *self = (aos_act_send_msg_t *)action;

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "Action_SendMsg");
	aos_assert_r(node, -1);
	aos_assert_r(!aos_act_net_serialize(action, node), -1);

	// Append <ToClipboard>	
	if (self->to_clipboard)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
				"ToClipboard", self->to_clipboard, 0, 0), -1);
	}

	// Append <FromClipboard>	
	if (self->from_clipboard)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
				"FromClipboard", self->from_clipboard, 0, 0), -1);
	}

	// Append <IsBlocking>
	if (self->blocking)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
				"IsBlocking", self->blocking), -1);
	}

	// 
	// Append <DataSrc>
	// 		      <Type>
	// 		      <Data
	// 		  </DataSrc>
	//
	aos_xml_node_t *srcn = node->mf->append_child_node(
			node, "DataSrc");
	aos_assert_r(srcn, -1);
	aos_assert_r(!srcn->mf->append_child_str(srcn, 
			"Type", aos_data_src_2str(self->data_src), 0, 0), -1);
	switch (self->data_src)
	{
	case eAosDataSrc_File:
	case eAosDataSrc_Clipboard:
		 aos_assert_r(!srcn->mf->append_child_str(srcn, 
				"Data", self->data_name, 0, 0), -1);
		 break;

	case eAosDataSrc_Direct:
		 aos_assert_r(!srcn->mf->append_child_str(srcn, 
				"Data", self->data_to_send, 0, 0), -1);
		 break;
	
	default:
		 aos_alarm("Unrecognized data src: %d", self->data_src);
		 return -1;
	}
		 	 
	// Set <FinishMethod>
	if (action->finish_method != eAosActFinishMd_Immediate)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"FinishMethod", 
			aos_act_finish_md_2str(action->finish_method), 
			0, 0), -1);
	}

	return 0;
}


int aos_act_send_msg_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_send_msg_t *self = (aos_act_send_msg_t *)action;

	aos_assert_r(!aos_act_net_deserialize(action, node), -1);

	// Retrieve <ToClipboard>
	if (self->to_clipboard) aos_free(self->to_clipboard);
	self->to_clipboard = 0;
	aos_assert_r(!node->mf->first_child_str_dft(node, "ToClipboard",
				&self->to_clipboard, 0, 0), -1);

	// Retrieve <FromClipboard>
	if (self->from_clipboard) aos_free(self->from_clipboard);
	self->from_clipboard = 0;
	aos_assert_r(!node->mf->first_child_str_dft(node, 
				"FromClipboard",
				&self->from_clipboard, 0, 0), -1);

	// Retrieve <DataToSend>
	aos_xml_node_t *datan = node->mf->first_named_child(node, 
			"DataToSend");
	aos_assert_r(datan, -1);

	// Retrieve <IsBlocking>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"IsBlocking", &action->blocking, 0), -1);

	// 
	// 	<DataToSend>
	// 		<Type>
	// 		<Data>
	// 	</Data>
	//
	char buff[101];
	int len = 101;
	aos_assert_r(!datan->mf->first_child_str_b(datan, 
				"Type", buff, &len, 0), -1);	
	self->data_src = aos_data_src_2enum(buff);
	aos_assert_r(aos_data_src_check(self->data_src) == 1, -1);
	aos_assert_r(!datan->mf->first_child_str(datan, 
				"Data", &self->data_to_send, 
				&self->data_len, 0), -1);

	if (self->data_src == eAosDataSrc_File)
	{
		self->data_name = self->data_to_send;
		aos_assert_r(!aos_file_read_file(self->data_name, 
				&self->data_to_send, 
				&self->data_len), -1);
	}

	if (self->data_src == eAosDataSrc_Clipboard)
	{
		self->data_name = self->data_to_send;
		self->data_to_send = 0;
		self->data_len = 0;
	}

	// Retrieve <FinishMethod>
	len = 101;
	aos_assert_r(!node->mf->first_child_str_b_dft(
			node, "FinishMethod",
			buff, &len, "Immediate"), -1);
	action->finish_method = aos_act_finish_md_2enum(buff);
	aos_assert_r(aos_act_finish_md_check(action->finish_method) == 1, -1);

	return 0;
}


int aos_act_send_msg_need_reg_event(struct aos_action *action)
{
	return 0;
}


int aos_act_send_msg_destroy(struct aos_action *action)
{
	aos_assert_r(action, -1);

	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_act_send_msg_release_memory(struct aos_action *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(!aos_act_net_release_memory(action), -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_send_msg_t *self = (aos_act_send_msg_t *)action;

	if (self->to_clipboard) aos_free(self->to_clipboard);
	if (self->from_clipboard) aos_free(self->from_clipboard);
	return 0;
}


// 
// Description
// This function assumes the data to be sent is stored in:
// 	data to send: act_data->str[0] 
// 	data length:  act_data->int[0]
//
int aos_act_send_msg_run(
		aos_action_t *action, 
		aos_gen_data_t *data)
{
	u32 raddr;
	u32 rport;

	aos_assert_r(action, -1);
	aos_assert_r(data, -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_send_msg_t *self = (aos_act_send_msg_t *)action;

	switch (self->data_src)
	{
	case eAosDataSrc_File:
	case eAosDataSrc_Direct:
		 aos_assert_r(self->data_to_send, -1);
		 aos_assert_r(self->data_len > 0, -1);
		 break;

	case eAosDataSrc_Clipboard:
		 if (self->data_to_send) break;
		 aos_assert_r(!aos_clipboard_get_str(
				eAosDataType_string, self->data_name, 
				&self->data_to_send, &self->data_len), -1);
		 break;

	default:
		 aos_alarm("Unrecognized data source: %d", self->data_src);
		 return -1;
	}

	if (self->from_clipboard)
	{
		// Retrieve the connection from the clipboard
		aos_assert_r(!aos_clipboard_get_ptr(eAosDataType_ptr, 
				self->from_clipboard, (void **)&self->conn), -1);
		aos_assert_r(self->conn, -1);
		self->local_addr = self->conn->local_addr;
		self->local_port = self->conn->local_port;
		self->local_num_ports = 1;
		self->remote_addr = self->conn->remote_addr;
		self->remote_port = self->conn->remote_port;
		self->remote_num_ports = 1;
		self->local_port_used = self->local_port;
		self->remote_port_used = self->remote_port;
	}

	switch (self->proto)
	{
	case eAosProto_Tcp:
		 aos_trace("To send tcp");
		 aos_assert_r(!self->mf->send_tcp((aos_act_net_t *)self, 
				self->data_to_send, self->data_len), -1);
		 break;

	case eAosProto_Udp:
		 aos_assert_r(data->mf->get_u32(data, 
					 eAosGenData_RemoteAddr, &raddr), -1);
		 aos_assert_r(data->mf->get_u32(data, 
					 eAosGenData_RemotePort, &rport), -1);
		 aos_assert_r(!self->mf->send_udp((aos_act_net_t *)self, 
				self->data_to_send, self->data_len, raddr, rport), -1);
		 break;

	case eAosProto_Unix:
		 aos_not_implemented_yet;
		 return -1;

	default:
		 aos_alarm("Unrecognized protocol: %d", self->proto);
		 return -1;
	}

	if (self->to_clipboard)
	{
		self->conn->mf->hold(self->conn);
		aos_assert_r(!aos_clipboard_set_ptr(
			eAosDataType_ptr, self->to_clipboard, self->conn, 0), -1);
	}

	switch (self->finish_method)
	{
	case eAosActFinishMd_Immediate:
		 self->finished = 1;
		 break;

	default:
		 aos_alarm("Unsupported finish method: %s",
				 aos_act_finish_md_2str(self->finish_method));
		 return -1;
	}

	return 0;
}


int aos_act_send_msg_finish_action(aos_action_t *action)
{
	return 0;
}


int aos_act_send_msg_connect(aos_act_net_t *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(action->type == eAosActionType_SendMsg, -1);
	aos_act_send_msg_t *self = (aos_act_send_msg_t *)action;

	aos_assert_r(!aos_act_net_connect(action), -1);
	if (self->to_clipboard)
	{
		aos_assert_r(!aos_clipboard_set_ptr(
			eAosDataType_ptr, self->to_clipboard, self->conn, 0), -1);
	}

	return 0;
}


static aos_act_send_msg_mf_t sg_mf = 
{
	aos_action_hold,
	aos_action_put,
	aos_act_send_msg_serialize,
	aos_act_send_msg_deserialize,
	aos_act_send_msg_destroy,
	aos_act_send_msg_release_memory,
	aos_act_send_msg_run,
	aos_action_is_finished,
	aos_act_send_msg_finish_action,
	aos_action_need_reg_event,
	aos_action_is_blocking, 
	aos_action_reset, 
	aos_act_net_set_conn,
	aos_act_net_send_tcp,
	aos_act_net_send_udp,
	aos_act_send_msg_connect,
	aos_act_net_disconnect
};


int aos_act_send_msg_init(aos_act_send_msg_t *act)
{
aos_trace("Create SendMsg action: %x", act);
	aos_assert_r(act, -1);
	memset(act, 0, sizeof(aos_act_send_msg_t));
	aos_assert_r(!aos_act_net_init((aos_act_net_t *)act), -1);
	act->mf = &sg_mf;
	act->type = eAosActionType_SendMsg;
	return 0;
}


aos_action_t * aos_act_send_msg_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_act_send_msg_t *act = aos_malloc(sizeof(aos_act_send_msg_t));
	aos_assert_r(act, 0);
	aos_assert_g(!aos_act_send_msg_init(act), cleanup);

	aos_assert_g(!act->mf->deserialize((aos_action_t *)act, node), cleanup);
	return (aos_action_t *)act;

cleanup:
	aos_free(act);
	return 0;
}

