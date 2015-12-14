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
#include "action_c/recv_msg.h"

#include "alarm_c/alarm.h"
#include "event_c/event_mgr.h"
#include "event_c/data_rcvd.h"
#include "event_c/msg_rcvd.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "net/read_thrd_mgr.h"
#include "parser/xml_node.h"
#include "parser/scm_field.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"
#include "util_c/buffer.h"
#include "util_c/tracer.h"
#include "util_c/clipboard.h"
#include "util_c/file.h"


int aos_act_recv_msg_serialize(
		struct aos_action *action, 
		struct aos_xml_node *parent)
{
	// 
	// 	<Parent>
	// 		...
	// 		<Action_SendMsg>
	// 			<Name>
	// 			<Type>
	// 			<LocalAddr>		// Default: 0
	// 			<LocalPort>		// Default: 0
	// 			<LocalNumPorts>	// Default: 1
	// 			<RemoteAddr>	// Default: 0
	// 			<RemotePort>	// Default: 0
	// 			<UnitPath>		// Default: 0
	// 			<Proto>			// Default: 0
	//
	//			<RecvHandler>	// Default: "GenMsgEvent"
	// 			<MsgHandler>	// Default: "DoNothing"
	// 			<Schema>		// Default: null
	// 			<FromClipboard>	// Optional
	// 			<FinishMethod>	// Default: "UponRecvMsg"
	// 			<IsBlocking>	// Default: 1
	// 		</Action_SendMsg>
	// 		...
	// 	</Parent>
	//
	aos_assert_r(action, -1);
	aos_assert_r(parent, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;

	aos_xml_node_t *node = parent->mf->append_child_node(
			parent, "Action_SendMsg");
	aos_assert_r(node, -1);
	aos_assert_r(!aos_act_net_serialize(action, node), -1);

	// Set <RecvHandler>
	if (self->recv_handler != eAosActionType_GenMsgEvent)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
				"RecvHandler", 
				aos_action_type_2str(self->recv_handler), 
				0, 0), -1);
	}

	// Set <Schema>
	if (self->schema)
	{
		aos_xml_node_t *sn = node->mf->append_child_node(
				node, "Schema");
		aos_assert_r(sn, -1);
		aos_assert_r(!self->schema->mf->serialize(
				self->schema, sn), -1);
	}

	// Set <FromClipboard>
	if (self->from_clipboard)
	{
		aos_assert_r(!node->mf->append_child_str(
				node, "FromClipboard", 
				self->from_clipboard, 0, 0), -1);
	}

	// Set <IsBlocking>
	if (!action->blocking)
	{
		aos_assert_r(!node->mf->append_child_int(node, 
			"IsBlocking", action->blocking), -1);
	}

	// Set <ToClipboard>
	if (self->to_clipboard)
	{
		aos_assert_r(!node->mf->append_child_str(
				node, "ToClipboard", 
				self->to_clipboard, 0, 0), -1);
	}
	aos_assert_r(!(self->to_clipboard && self->from_clipboard), -1);

	// Set <MsgHandler>
	if (self->msg_handler != eAosActionType_DoNothing)
	{
		aos_assert_r(
			self->msg_handler == eAosActionType_ToClipboard ||
			self->msg_handler == eAosActionType_WriteToFile ||
			self->msg_handler == eAosActionType_AppendToFile, -1);
		aos_assert_r(self->msg_hd_id, -1);

		// 	<MsgHandler>
		// 		<Type>
		// 		<DataId>
		// 	</MsgHandler>	
		aos_xml_node_t *mm = node->mf->append_child_node(
				node, "MsgHandler");
		aos_assert_r(mm, -1);
		aos_assert_r(!mm->mf->append_child_str(mm, "Type", 
				aos_action_type_2str(self->msg_handler), 0, 0), -1);
		aos_assert_r(!mm->mf->append_child_str(mm, "DataId", 
				self->msg_hd_id, 0, 0), -1);
	}

	// Set <FinishMethod>
	if (action->finish_method != eAosActFinishMd_UponRecvMsg)
	{
		aos_assert_r(!node->mf->append_child_str(node, 
			"FinishMethod", 
			aos_act_finish_md_2str(action->finish_method), 
			0, 0), -1);
	}

	return 0;
}


int aos_act_recv_msg_deserialize(
		struct aos_action *action, 
		struct aos_xml_node *node)
{
	// 
	// See the comments in serialize(...)
	//
	aos_assert_r(action, -1);
	aos_assert_r(node, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;

	aos_assert_r(!aos_act_net_deserialize(action, node), -1);

	// Retrieve <RecvHandler>
	char buff[101];
	int buff_len = 101;
	aos_assert_r(!node->mf->first_child_str_b_dft(node, 
			"RecvHandler", buff, &buff_len, "GenMsgEvent"), -1);
	self->recv_handler = aos_action_type_2enum(buff);
	aos_assert_r(aos_action_type_check(self->recv_handler) == 1, -1);

	// Retrieve <Schema>
	aos_xml_node_t *sn = node->mf->first_named_child(
			node, "Schema");
	if (sn)
	{
		self->schema = aos_scm_field_factory(sn);
		aos_assert_r(self->schema, -1);
	}

	// If recv_handler is eAosActionType_GenMsgEvent, 
	// schema is mandatory
	aos_assert_t(self->recv_handler == eAosActionType_GenMsgEvent,
			self->schema, -1);

	// Retrieve <FromClipboard>
	aos_assert_r(!node->mf->first_child_str_dft(
				node, "FromClipboard", 
				&self->from_clipboard, 0, 0), -1);

	// Retrieve <ToClipboard>
	aos_assert_r(!node->mf->first_child_str_dft(
				node, "ToClipboard", 
				&self->to_clipboard, 0, 0), -1);

	aos_assert_r(!(self->to_clipboard && self->from_clipboard), -1);

	// Retrieve <MsgHandler>
	aos_xml_node_t *mm = node->mf->first_named_child(
			node, "Msghandler");
	if (!mm)
	{
		self->msg_handler = eAosActionType_DoNothing;
	}
	else
	{
		aos_assert_r(!mm->mf->first_child_str_b(mm, 
				"Type", buff, &buff_len, 0), -1);
		self->msg_handler = aos_action_type_2enum(buff);
		aos_assert_r(
			self->msg_handler == eAosActionType_ToClipboard ||
			self->msg_handler == eAosActionType_WriteToFile ||
			self->msg_handler == eAosActionType_AppendToFile, -1);
		if (self->msg_hd_id) aos_free(self->msg_hd_id);
		int len;
		aos_assert_r(!mm->mf->first_child_str(mm, 
				"DataId", &self->msg_hd_id, &len, 0), -1);
	}

	// Retrieve <FinishMethod>
	buff_len = 101;
	aos_assert_r(!node->mf->first_child_str_b_dft(
			node, "FinishMethod",
			buff, &buff_len, "UponRcvdMsg"), -1);
	action->finish_method = aos_act_finish_md_2enum(buff);
	aos_assert_rm(aos_act_finish_md_check(
			action->finish_method) == 1, -1, 
			"Method: %s", buff);

	// Retrieve <IsBlocking>
	aos_assert_r(!node->mf->first_child_int_dft(node, 
			"IsBlocking", &action->blocking, 1), -1);

	return 0;
}


int aos_act_recv_msg_destroy(struct aos_action *action)
{
	aos_assert_r(action, -1);

	aos_assert_r(!action->mf->release_memory(action), -1);
	aos_free(action);
	return 0;
}


int aos_act_recv_msg_release_memory(struct aos_action *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;

	aos_assert_r(!aos_act_net_release_memory(action), -1);
	if (self->to_clipboard) aos_free(self->to_clipboard);
	if (self->from_clipboard) aos_free(self->from_clipboard);
	return 0;
}


//
// Description:
// The action received something. It will create generate an 
// event.
//
static int aos_act_recv_msg_gen_event(
		 aos_act_recv_msg_t *action,
		 struct aos_conn *conn,
		 struct aos_buffer *buffer,
		 const int is_timedout,
		 const u32 remote_addr,
		 const u16 remote_port)
{
	aos_assert_r(conn, -1);
	aos_assert_t(!is_timedout, buffer, -1);

	aos_event_data_rcvd_t *event = aos_event_data_rcvd_create(
			buffer, remote_addr, remote_port);
	aos_assert_r(event, -1);
	event->match_data = conn->sock;
	aos_assert_g(!aos_add_event((aos_event_t *)event), cleanup);
	return 0;

cleanup:
	event->mf->put((aos_event_t *)event);
	return -1;
}


//
// Description:
// The action received something. It will create a message. If 
// the received can create a complete message, it will determine
// whether it is the right message to receive and whether it
// should stop receiving anymore.
//
static int aos_act_recv_msg_gen_msg_event(
		 aos_act_recv_msg_t *action,
		 struct aos_conn *conn,
		 struct aos_buffer *buffer,
		 const int is_timedout,
		 const u32 remote_addr,
		 const u16 remote_port)
{
	aos_assert_r(conn, -1);
	aos_assert_t(!is_timedout, buffer, -1);
	aos_assert_r(action->schema, -1);

	aos_field_t *msg;
    int ret = action->schema->mf->data_rcvd(
			action->schema, buffer, &msg);

	if (ret == 1)
	{
		// Received is not complete to construct a message. 
		// Need to wait for more receivings.
		return 0;
	}

	if (ret < 0)
	{
		aos_alarm("Received is incorrect: %s", buffer->buff);
		return -1;
	}

	aos_assert_r(msg, -1); 
	
	// A message has been created.
	aos_event_msg_rcvd_t *event = aos_event_msg_rcvd_create(
			msg, conn->local_addr, conn->local_used_port, 
			remote_addr, remote_port);
	aos_assert_r(event, -1);
	event->match_data = conn->sock;
	aos_assert_g(!aos_add_event((aos_event_t *)event), cleanup);

	if (action->finish_method == eAosActFinishMd_UponRecvMsg)
	{
		action->finished = 1;
	}
	return 0;

cleanup:
	event->mf->put((aos_event_t *)event);
	return -1;
}


static int read_callback(
		  	aos_conn_t *conn,
		  	aos_buffer_t *buffer,
		  	const int is_timedout,
			const u32 remote_addr,
			const u16 remote_port)
{
aos_trace("Received data");
	// The action read something. Need to determine what to do.
	aos_assert_g(conn, cleanup);
	aos_act_recv_msg_t *action = 0;
	aos_assert_g(!conn->mf->get_user_data(conn, 
			eAosGenPtrTag_RecvMsgAction, 
			(void **)&action), cleanup);
	aos_assert_g(action, cleanup);
	aos_assert_g(action->magic == AOS_ACTION_MAGIC, cleanup);

	aos_trace("Received something: %d. Length: %d", 
			conn->sock, buffer->data_len);

	switch (action->msg_handler)
	{
	case eAosActionType_ToClipboard:
		 aos_assert_g(action->msg_hd_id, cleanup);
		 aos_assert_g(!aos_clipboard_set_ptr(
				eAosDataType_Buffer, action->msg_hd_id, 
				buffer, 0), cleanup);
		 buffer->mf->hold(buffer);
		 break;

	case eAosActionType_WriteToFile:
		 aos_assert_g(action->msg_hd_id, cleanup);
		 aos_assert_g(!aos_file_write_to_file(action->msg_hd_id, 
				buffer->buff, buffer->data_len), cleanup);
		 break;

	case eAosActionType_AppendToFile:
		 aos_assert_g(action->msg_hd_id, cleanup);
		 aos_assert_g(!aos_file_append_to_file(action->msg_hd_id, 
				buffer->buff, buffer->data_len), cleanup);
		 break;

	case eAosActionType_DoNothing:
		 break;

	default:
		 aos_alarm("Unsupported action: %s", 
				 aos_action_type_2str(action->msg_handler));
		 goto cleanup;
	}

	switch(action->recv_handler)
	{
	case eAosActionType_GenRecvEvent:
		 if (action->finish_method == eAosActFinishMd_UponRecvData)
		 {
			 action->finished = 1;
		 }
		 aos_assert_g(!aos_act_recv_msg_gen_event(action, 
				conn, buffer, is_timedout, 
				remote_addr, remote_port), cleanup);
		 break;

	case eAosActionType_GenMsgEvent:
		 aos_assert_g(!aos_act_recv_msg_gen_msg_event(action, 
				conn, buffer, is_timedout, 
				remote_addr, remote_port), cleanup);
		 break;

	default:
		 aos_alarm("Unhandled action: %d", action->recv_handler);
		 goto cleanup;
	}

	switch (action->finish_method)
	{
	case eAosActFinishMd_UponRecvData:
		 action->finished = 1;
		 break;

	case eAosActFinishMd_UponRecvMsg:
		 // This should have been handled by the function
		 // call 'aos_act_recv_msg_gen_msg_event(...)
		 break;

	default:
		 aos_alarm("Unsupported finish method: %s", 
				aos_act_finish_md_2str(action->finish_method));
		 return -1;
	}

	if (action->mf->is_finished((aos_action_t *)action) == 1)
	{
		aos_assert_g(!action->mf->finish_action(
					(aos_action_t *)action), cleanup);
	}

	return 0;

cleanup:
	buffer->mf->put(buffer);
	return -1;
}


// 
// Description
// This function receives message
//
int aos_act_recv_msg_run(
		aos_action_t *action, 
		aos_gen_data_t *data)
{
	aos_assert_r(action, -1);
	aos_assert_r(data, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;
aos_trace("To receive message: %d", action->finished);
	// Check whether to retrieve the connection from the clipboard
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

	aos_assert_r(self->conn, -1);

	if (self->to_clipboard)
	{
		aos_assert_r(!aos_clipboard_set_ptr(
			eAosDataType_ptr, self->to_clipboard, 
			self->conn, 0), -1);
	}

	// Add the action to the connection
	aos_assert_r(!action->mf->hold(action), -1);
	aos_assert_r(!self->conn->mf->add_user_data(self->conn, 
			eAosGenPtrTag_RecvMsgAction, action), -1);
aos_trace("To add connection to read thread mgr");
	aos_assert_r(!aos_rdthdmgr_add_conn(read_callback, self->conn), -1);
	return 0;
}


int aos_act_recv_msg_need_reg_event(aos_action_t *action)
{
	return 1;
}


int aos_act_recv_msg_is_finished(aos_action_t *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;
	return self->finished;
}

#include "net/read_thrd.h"
extern aos_read_thrd_entry_t *chending;


int aos_act_recv_msg_finish_action(aos_action_t *action)
{
	aos_assert_r(action, -1);
	aos_assert_r(action->type == eAosActionType_RecvMsg, -1);
	aos_act_recv_msg_t *self = (aos_act_recv_msg_t *)action;

if (chending)
{
	aos_trace("In finish_action: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	// Remove the connection from its reading thread manager
	aos_assert_r(!aos_rdthdmgr_del_conn(self->conn), -1);

if (chending)
{
	aos_trace("Deleted connection: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	// Remove the action from the connection
	aos_assert_r(!self->conn->mf->del_user_data(self->conn, 
			eAosGenPtrTag_RecvMsgAction, action), -1);

if (chending)
{
	aos_trace("Deleted user data: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	aos_assert_r(!action->mf->put(action), -1);

if (chending)
{
	aos_trace("Before Creating event: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	// Generate ActionFinished event
	aos_event_t *event;
	aos_assert_r(!aos_event_create(&event, 
			eAosEvent_ActionFinished, action->action_id), -1);

if (chending)
{
	aos_trace("Created Event: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	aos_assert_g(!aos_add_event(event), cleanup);

if (chending)
{
	aos_trace("Added Event: %x:%x:%x:%x",
		chending->link.prev,
		chending->link.next,
		chending->conn,
		chending->callback);
}

	return 0;

cleanup:
	aos_assert_r(!event->mf->destroy(event), -1);
	return -1;
}


static aos_act_recv_msg_mf_t sg_mf = 
{
	aos_action_hold,
	aos_action_put,
	aos_act_recv_msg_serialize,
	aos_act_recv_msg_deserialize,
	aos_act_recv_msg_destroy,
	aos_act_recv_msg_release_memory,
	aos_act_recv_msg_run,
	aos_act_recv_msg_is_finished,
	aos_act_recv_msg_finish_action,
	aos_act_recv_msg_need_reg_event,
	aos_action_is_blocking,
	aos_action_reset,
	aos_act_net_set_conn,
	aos_act_net_send_tcp,
	aos_act_net_send_udp,
	aos_act_net_disconnect
};


int aos_act_recv_msg_init(aos_act_recv_msg_t *act)
{
	aos_assert_r(act, -1);
aos_trace("Create RecvMsg action: %x", act);
	memset(act, 0, sizeof(aos_act_recv_msg_t));
	act->mf = &sg_mf;

	aos_assert_r(!aos_act_net_init((aos_act_net_t *)act), -1);

	act->type = eAosActionType_RecvMsg;
	act->recv_handler = eAosActionType_GenRecvEvent;
	return 0;
}


aos_action_t * aos_act_recv_msg_create()
{
	aos_action_t *act = aos_malloc(sizeof(aos_act_recv_msg_t));
	aos_assert_r(act, 0);

	aos_assert_g(!aos_act_recv_msg_init((aos_act_recv_msg_t *)act), cleanup);
	return act;

cleanup:
	aos_free(act);
	return 0;
}


aos_action_t * aos_act_recv_msg_create_xml(aos_xml_node_t *node)
{
	aos_assert_r(node, 0);
	aos_action_t *act = aos_malloc(sizeof(aos_act_recv_msg_t));
	aos_assert_r(act, 0);

	aos_assert_g(!aos_act_recv_msg_init((aos_act_recv_msg_t *)act), cleanup);
	aos_assert_g(!act->mf->deserialize(act, node), cleanup);
	return act;

cleanup:
	aos_free(act);
	return 0;
}

