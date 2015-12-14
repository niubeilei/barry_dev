////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "net/tester/tcp_server_tester.h"

#include "alarm/Alarm.h"
#include "net/tcp_server.h"
#include "porting/addr.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "util/buffer.h"
#include "util/memory.h"
#include "util/tracer.h"
#include <stdio.h>


extern int aos_tcp_server_torturer();
extern int aos_tcp_server_special_test_cases();

int aos_tcp_server_tester()
{
	aos_assert_r(!aos_tcp_server_special_test_cases(), -1);
	aos_assert_r(!aos_tcp_server_torturer(), -1);
	return 0;
}


static int aos_tcp_server_tester_callback(
		struct aos_conn *conn,
		struct aos_buffer *buffer,
		const int is_timedout,
		const u32 remote_addr,
		const u16 remote_port)
{
	aos_trace("Received contents: %d. Length: %d, remote addr:%s:%d", 
		conn->sock, 
		buffer->data_len, 
		aos_addr_to_str(remote_addr), remote_port);
	return 0;
}


int aos_tcp_server_special_tc1()
{
	u32 local_addr;
	aos_assert_r(!aos_addr_to_u32("192.168.197.128", &local_addr), -1);
	aos_tcp_server_t * server = aos_tcp_server_create(
			local_addr,
			5000, 1, aos_tcp_server_tester_callback, 
			eAosSockType_Tcp);	

	aos_assert_r(server, -1);
	aos_assert_r(!server->mf->connect(server), -1);
	aos_assert_r(!server->mf->start_reading(server), -1);

	while (1)
	{
		aos_sleep(1);
	}

	return 0;
}


int aos_tcp_server_special_test_cases()
{
	aos_assert_r(!aos_tcp_server_special_tc1(), -1);
	return 0;
}


int aos_tcp_server_torturer()
{
	return 0;
} 

