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
// Modification History:
// 02/22/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "net/udp.h"

#include "action_c/action.h"
#include "alarm_c/alarm.h"
#include "porting_c/socket.h"
#include "porting_c/get_errno.h"
#include "porting_c/get_err_str.h"
#include "porting_c/ipv6.h"
#include "porting_c/addr.h"
#include "porting_c/sock_len.h"
#include "porting_c/select.h"
#include "util_c/types.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include "util_c/memory.h"
#include "util_c/gen_data.h"


int aos_udp_connect(
		int *sock, 
		const u32 local_addr, 
		const u16 local_port, 
		const int local_num_ports,
		u16 *local_used_port)
{
	//
	// This function creates a Udp Connection at 'local_addr' 
	// and 'local_port'.  If 'localPort' == 0, the system will 
	// allocate a port (1024-5000), and the port is returned 
	// through 'local_port'. If successful, it returns 0. 
	// Otherwise, it returns an errno. 
	//

	//
	// Create the socket.
	//
	aos_trace("Create UDP socket");
	*sock = socket(AF_INET, SOCK_DGRAM, 0);

	aos_assert_r(*sock, -1);

    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    setsockopt(*sock, SOL_SOCKET, SO_LINGER, 
			(char*) &ling, sizeof (ling));
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char*) NULL, 0);

	//
	// Bind to any local address
	//
	struct sockaddr_in sockAdd;
	memset((char*) &sockAdd, 0, sizeof (sockAdd));
	sockAdd.sin_family      = AF_INET;
	aos_set_addr(&sockAdd, local_addr);
	sockAdd.sin_port        = htons(local_port);
		
	aos_trace("To bind to: %d, %s:%d", 
			*sock, aos_addr_to_str(local_addr), local_port);

	int result = bind(*sock, (struct sockaddr*)&sockAdd, sizeof(sockAdd));
	aos_assert_rm(!result, -1, "Failed to bind: %s. Local addr: %s:%d", 
			aos_get_err_str(aos_get_errno()), aos_addr_to_str(local_addr), 
			local_port);

	aos_trace("Bind successfully: %d", *sock);

	//
	// Retrieve the port
	//
	aos_sock_len_t addrLen = sizeof(sockAdd);
	aos_assert_r(!getsockname(*sock, (struct sockaddr*)&sockAdd, &addrLen), -1);
	*local_used_port = ntohs(sockAdd.sin_port);
	return 0;
}


static int aos_udp_integrity_check(aos_conn_t *conn)
{
	return aos_conn_integrity_check(conn);
}


static int aos_udp_v_connect(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(conn->sock <= 0, -1);
	aos_assert_r(aos_udp_integrity_check(conn) == 1, -1);

	aos_assert_r(!aos_udp_connect(&conn->sock, 
			conn->local_addr, conn->local_port, conn->local_num_ports, 
			&conn->local_used_port), -1);
	return 0;
}


int aos_udp_send(
		const int sock,
		const char * const data,
		const int length,
		const u32 local_addr, 
		const u16 local_port,
		const u32 remote_addr, 
		const u16 remote_port)
{
	//
	// This function writes 'data' to the "remote_addr" and 
	// "remote_port".
	//
	aos_assert_r(data, -1);
	aos_assert_r(sock > 0, -1);
	aos_assert_r(length > 0, -1);
	aos_assert_r(local_addr > 0, -1);
	aos_assert_r(local_port > 0, -1);
	aos_assert_r(remote_addr > 0, -1);
	aos_assert_r(remote_port > 0, -1);

	//
	// Prepare to send
	//
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	aos_set_addr(&address, remote_addr);
	address.sin_port = htons(remote_port);

	aos_trace("To write (%s:%d) to %s:%d", 
			aos_addr_to_str(local_addr), local_port, 
			aos_addr_to_str(remote_addr), remote_port);

	//
	// Call the function to actually send the data
	//
	int bytesLeft = length;
	const char *dataToWrite = (const char *)data;
	while (bytesLeft > 0)
	{
		int bytesSent = aos_send_to(sock, dataToWrite, bytesLeft, 
				(struct sockaddr*) &address, sizeof(address));

		if (bytesSent <= 0)
		{
			//
			// Failed to read. Check the errors and return.
			//
			int errcode = aos_get_errno();
			aos_alarm("Failed to write: %d. Errno: %d. Errmsg: %s. "
				"From: (%s:%d) to: (%s:%d)", 
				sock, errcode, aos_get_err_str(errcode), 
				aos_addr_to_str(local_addr), local_port, 
				aos_addr_to_str(remote_addr), remote_port);
			return -1;
		}

		//
		// Sent successfully, but it doesn't mean that all sent.
		// Check the number and determine what has happened.
		//
		bytesLeft -= bytesSent;
     	dataToWrite = dataToWrite + bytesSent;
    }

	return 0;
}


int aos_udp_v_read(
		aos_conn_t *conn, 
		const int sec, 
		const int usec,
		aos_conn_read_callback_t callback,
		int *is_eof, 
		int *is_conn_broken)
{
	aos_assert_r(conn, -1);
	aos_assert_r(is_eof, -1);
	aos_assert_r(conn->sock > 0, -1);
	aos_assert_r(conn->callback, -1);

	*is_eof = 0;
	aos_buffer_t *buffer = aos_create_conn_read_buffer();
	aos_assert_r(buffer, -1);
	int is_timedout; 
	u32 remote_addr; 
	u16 remote_port;
	int ret = aos_conn_read_from(conn->sock, buffer, sec, usec, 
			&is_timedout, &remote_addr, &remote_port, is_conn_broken);
	aos_assert_r(!ret, -1);

	if (is_conn_broken) return 0;

	if (callback)
	{
		aos_assert_r(!callback(conn, buffer, is_timedout, 
			remote_addr, remote_port), -1);
	}
	return 0;
}


int aos_udp_v_read_to_buff(
		aos_conn_t *conn, 
		const int sec, 
		const int usec, 
		aos_buffer_t *buffer, 
		u32 *remote_addr, 
		u16 *remote_port,
		int *is_eof, 
		int *is_conn_broken)
{
	aos_assert_r(conn, -1);
	aos_assert_r(is_eof, -1);
	aos_assert_r(conn->sock > 0, -1);
	aos_assert_r(buffer, -1);

	*is_eof = 0;
	int is_timedout; 
	int ret = aos_conn_read_from(conn->sock, buffer, sec, usec, 
			&is_timedout, remote_addr, remote_port, is_conn_broken);
	aos_assert_r(!ret, -1);
	return 0;
}


int aos_udp_send_udp(
		aos_udp_t *conn, 
		const char * const contents, 
		const int len, 
		const u32 remote_addr, 
		const u16 remote_port)
{
	aos_assert_r(conn, -1);
	return aos_udp_send(conn->sock, contents, len, 
			conn->local_addr, conn->local_used_port, 
			remote_addr, remote_port);
}


static int aos_udp_destroy(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(!conn->mf->release_memory, -1);
	aos_free(conn);
	return 0;
}


static int aos_udp_release_memory(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(!aos_conn_release_memory(conn), -1);
	aos_assert_r(conn->type == eAosSockType_Udp, -1);
	
	return 0;
}


static char * aos_udp_dump(aos_conn_t *conn)
{
	aos_assert_r(conn, "Error");
	aos_not_implemented_yet;
	return "Error";
}


static aos_udp_mf_t sg_mf = 
{
	aos_conn_add_user_data,
	aos_conn_del_user_data,
	aos_conn_get_user_data,
	aos_conn_hold,
	aos_conn_put,
	aos_udp_release_memory,
	aos_udp_v_read,
	aos_udp_v_read_to_buff,
	aos_conn_is_conn_good,
	aos_udp_v_connect,
	aos_conn_disconnect,
	aos_udp_integrity_check,
	aos_udp_destroy, 
	aos_udp_dump,
	aos_udp_send_udp
};

int aos_udp_init(aos_udp_t *conn)
{
	aos_assert_r(conn, -1);
	memset(conn, 0, sizeof(aos_udp_t));
	conn->mf = &sg_mf;

	aos_assert_r(!aos_conn_init((aos_conn_t *)conn), -1);
	return 0;
}


// 
// Description
// It creates a aos_udp_t object. It does not connect the sock, 
// nor create the sock. One should call aos_udp_connect(...)
// to actually create and connect the sock.
//
aos_udp_t *aos_udp_create(
		const u32 local_addr, 
		const u16 local_port, 
		const u16 local_num_ports) 
{
	aos_assert_r(local_addr, 0);
	aos_assert_r(local_num_ports > 0, 0);

	aos_udp_t *obj = aos_malloc(sizeof(aos_udp_t));
	aos_assert_r(obj, 0);
	aos_assert_g(!aos_udp_init(obj), cleanup);

	obj->local_addr = local_addr;
	obj->local_port = local_port;
	obj->local_num_ports = local_num_ports;
	obj->type = eAosSockType_Udp;

	return obj;

cleanup:
	aos_free(obj);
	return 0;
}

