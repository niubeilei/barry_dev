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
// 03/01/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "action/tester/recv_msg_tester.h"

#include "action/send_msg.h"
#include "action/recv_msg.h"
#include "alarm/Alarm.h"
#include "event/event_mgr.h"
#include "event/event.h"
#include "net/read_thrd_mgr.h"
#include "parser/xml_node.h"
#include "porting/addr.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "util/buffer.h"
#include "util/memory.h"
#include "util/tracer.h"
#include "util/gen_data.h"
#include <stdio.h>


extern int aos_recv_msg_torturer();
extern int aos_recv_msg_special_test_cases();



int aos_recv_msg_tester()
{
	aos_assert_r(!aos_recv_msg_special_test_cases(), -1);
	aos_assert_r(!aos_recv_msg_torturer(), -1);
	return 0;
}


static int read_callback(
		  struct aos_conn *conn,
		  struct aos_buffer *buffer,
		  const int is_timedout,
		  const u32 remote_addr,
		  const u16 remote_port)
{
	aos_trace("Read something from %s:%d. Length: %d. Contents: %s", 
			aos_addr_to_str(remote_addr), remote_port,
			buffer->data_len, 
			buffer->buff);
	buffer->mf->destroy(buffer);
	return 0;
}


static int event_callback(aos_event_t *event, void *user_data)
{
	aos_trace("Received event: %d", event->type);
	aos_assert_r(user_data, -1);
	aos_trace("User data: %x", user_data);
	return 0;
}
	

int aos_recv_msg_special_tc1()
{
	// Create the send_act
	aos_xml_node_t *n = aos_xml_node_create_from_file("sendmsg.xml");
	aos_assert_r(n, -1);
	aos_xml_node_t *node = n->mf->first_named_child(n, "Action_SendMsg");
	aos_assert_r(node, -1);

	aos_act_net_t *send_act = (aos_act_net_t *)aos_act_send_msg_create_xml(node);
	aos_assert_r(send_act, -1);

	// Create the send_data
	aos_gen_data_t send_data;
	aos_gen_data_init(&send_data);

	char buff[10101];
	memset(buff, 'c', 10101);

	aos_assert_r(!send_data.mf->set_str(&send_data, 0, 
				eAosGenData_DataSent, buff, 0), -1);
	aos_assert_r(!send_data.mf->set_int(&send_data, 0, 
				eAosGenData_DataLen, 10101), -1);

	// Create the recv_act
	aos_act_net_t *recv_act = (aos_act_net_t *)aos_act_recv_msg_create();
	aos_assert_r(recv_act, -1);
	
	aos_assert_r(!send_act->mf->connect((aos_act_net_t *)send_act), -1);
	recv_act->local_addr = send_act->local_addr;
	recv_act->local_port = send_act->local_port_used;
	recv_act->local_num_ports = 1;
	recv_act->remote_addr = send_act->remote_addr;
	recv_act->remote_port = send_act->remote_port_used;
	recv_act->remote_num_ports = 1;
	recv_act->conn = send_act->conn;

	// Create read_thrd_mgr
	aos_read_thrd_mgr_t *thrd_mgr = aos_read_thrd_mgr_create();
	aos_assert_r(thrd_mgr, -1);
	
	// Create the recv_data
	aos_gen_data_t recv_data;
	aos_gen_data_init(&recv_data);
	aos_assert_r(!recv_data.mf->set_ptr(&recv_data, 0, 
				eAosGenData_ReadCallback, read_callback), -1);
	aos_assert_r(!recv_data.mf->set_ptr(&recv_data, 1,
				eAosGenData_ReadThrdMgr, thrd_mgr), -1);

	// Register event
	aos_register_event(eAosEventType_DataRcvd, event_callback, 
			send_act->conn->sock, recv_act);

	// Run the recv_act
	aos_assert_r(!recv_act->mf->run((aos_action_t *)recv_act, 
				&recv_data), -1);

	// Run the send_act
	aos_assert_r(!send_act->mf->run((aos_action_t *)send_act, 
				&send_data), -1);

	while (1)
	{
		aos_sleep(1);
	}

	return 0;
}


int aos_recv_msg_special_test_cases()
{
	aos_assert_r(!aos_recv_msg_special_tc1(), -1);
	return 0;
}


int aos_recv_msg_torturer()
{
	return 0;
} 

