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
#include "TestDrivers/tcp_bouncer/bouncer.h"

#include "alarm/Alarm.h"
#include "net/tcp_server.h"
#include "net/tcp.h"
#include "porting/addr.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "util/buffer.h"
#include "util/memory.h"
#include "util/tracer.h"
#include <stdio.h>


static int aos_tcp_bouncer_callback(
		struct aos_conn *conn,
		struct aos_buffer *buffer,
		const int is_timedout,
		const u32 remote_addr,
		const u16 remote_port)
{
aos_trace("Bouncer callback");

	aos_assert_r(conn, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(buffer->data_len > 0, -1);
	aos_assert_r(conn->type == eAosSockType_Tcp, -1);
	aos_tcp_t *self = (aos_tcp_t *)conn;

aos_trace("To send");
	aos_assert_r(!self->mf->send_tcp(self, buffer->buff, 
			buffer->data_len, 0), -1);
aos_trace("sent");
	return 0;
}


int aos_tcp_bouncer_create(
		const u32 local_addr, 
		const u16 local_port, 
		const int num_ports)
{
	aos_tcp_server_t * server = aos_tcp_server_create(
			local_addr, local_port, num_ports, 
			aos_tcp_bouncer_callback, 
			eAosSockType_Tcp);	

	aos_assert_r(server, -1);
	aos_assert_r(!server->mf->connect(server), -1);
	aos_assert_r(!server->mf->start_reading(server), -1);

	return 0;
}

