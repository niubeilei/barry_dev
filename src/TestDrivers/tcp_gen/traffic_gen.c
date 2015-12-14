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
#include "TestDrivers/tcp_gen/traffic_gen.h"

#include "alarm/Alarm.h"
#include "net/tcp_server.h"
#include "net/tcp.h"
#include "net/read_thrd.h"
#include "porting/addr.h"
#include "porting/sleep.h"
#include "rvg/rvg.h"
#include "rvg/rvg_xml.h"
#include "util/buffer.h"
#include "util/memory.h"
#include "util/tracer.h"
#include <stdio.h>

#define AOS_TCP_GEN_MAX_READ_THREADS 50
static aos_read_thrd_t *sg_threads[AOS_TCP_GEN_MAX_READ_THREADS];
static int sg_num_threads = 0;

typedef struct aos_tcp_traffic_gen_rcd
{
	int			bytes_recved;
	int			bytes_expected;
	int			bytes_sent;
	int			bytes_to_send;
	int			repeat;
} aos_tcp_traffic_gen_rcd_t;

#define AOS_TCP_GEN_MAX_SOCK 1024
static aos_tcp_traffic_gen_rcd_t *sg_records[AOS_TCP_GEN_MAX_SOCK+1];

static int aos_tcp_traffic_gen_callback(
		struct aos_conn *conn,
		struct aos_buffer *buffer,
		const int is_timedout,
		const u32 remote_addr,
		const u16 remote_port)
{
	aos_assert_r(conn, -1);
	aos_assert_r(buffer, -1);
	aos_assert_r(buffer->data_len > 0, -1);
	aos_assert_r(conn->type == eAosSockType_Tcp, -1);
	aos_tcp_t *self = (aos_tcp_t *)conn;
	int sock = conn->sock;
	aos_assert_r(sock > 0 && sock <= AOS_TCP_GEN_MAX_SOCK, -1);

	aos_tcp_traffic_gen_rcd_t *record = sg_records[sock];
	aos_assert_r(record, -1);

	record->bytes_recved += buffer->data_len;

	aos_assert_r(!buffer->mf->destroy(buffer), -1);
	if (record->bytes_to_send < record->bytes_sent)
	{
		aos_assert_r(!send_data(conn), -1);
	}
	else
	{
		if (record->bytes_recved >= record->bytes_expected)
		{
			// Finished


	return 0;
}


int aos_tcp_traffic_gen_add_conns(
		const u32 local_addr, 
		const u32 remote_addr, 
		const u16 remote_port, 
		const int num_remote_ports,
		const int num_conns, 
		const int max_size)
{
	int i, j;
	aos_assert_r(local_addr, -1);
	aos_assert_r(num_conns > 0, -1);

	for (i=0; i<num_conns; i++)
	{
		aos_tcp_t *conn = aos_tcp_create(local_addr, 0, 1, 
				remote_addr, remote_port, num_remote_ports);
		aos_assert_r(conn, -1);

		int added = 0;
		for (j=0; j<sg_num_threads; j++)
		{
			if (sg_threads[j]->mf->is_full(sg_threads[j]) != 1)
			{
				aos_assert_r(!sg_threads[j]->mf->add_conn(
					sg_threads[j], (aos_conn_t *)conn, 
					aos_tcp_traffic_gen_callback), -1);
				added = 1;
				break;
			}
		}

		if (!added)
		{
			// 
			// Need to create a new read thread
			//
			aos_read_thrd_t *thrd = aos_read_thrd_create();
			aos_assert_r(thrd, -1);

			thrd->max_entries = max_size;
			aos_assert_r(!thrd->mf->add_conn(thrd, (aos_conn_t *)conn, 
					aos_tcp_traffic_gen_callback), -1);
			sg_threads[sg_num_threads++] = thrd;
			aos_assert_r(!thrd->mf->start(thrd), -1);
		}
	}

	return 0;
}

