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
#include "net/tcp.h"

#include "alarm_c/alarm.h"
#include "porting_c/socket.h"
#include "porting_c/get_errno.h"
#include "porting_c/get_err_str.h"
#include "porting_c/ipv6.h"
#include "porting_c/addr.h"
#include "porting_c/sock_len.h"
#include "util_c/types.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include "util_c/memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>


//
// Description:
// This function send data to the socket. 
// The data is stored in "data". If it failed
// to write, it return -1. Otherwise, it returns 0.
//
int aos_tcp_send_raw(
		const int sock, 
		const char * const data, 
		const int len, 
		const int is_blocking)
{
    int bytes_left = len;

	aos_assert_r(sock > 0, -1);
	aos_assert_r(data, -1);
	aos_assert_r(len > 0, -1);

	const char * data_to_send = (const char *)data;
    int bytes_written = 0;
    while (bytes_left > 0)
    {
		if(is_blocking)
		{
			bytes_written = aos_write_sock(sock, 
					data_to_send, bytes_left);
		}
		else
		{
			bytes_written = aos_send_to_sock(sock, 
					data_to_send, bytes_left, 0);
		}

		if (bytes_written <= 0)
		{
			//
			// Failed to read. Check the errors and return.
			//
			int ec = aos_get_errno();
   	        if(EAGAIN == ec || EWOULDBLOCK == ec)
   	        {
				continue;
            }
			
			aos_alarm("Failed to write tcp sock: %d, %s",
					sock, aos_get_err_str(ec));

			if (sock > 0)
			{
				aos_close_sock(sock);
			}
			return -1;
		}
     	bytes_left -= bytes_written;
     	data_to_send = data_to_send + bytes_written;
   	}
		
	aos_trace("Sent data. Sock: %d. Length: %d", sock, len);
    return 0;
}


int aos_tcp_send(
		aos_tcp_t *conn, 
		const char * const data, 
		const int len, 
		const int is_blocking)
{
	aos_assert_r(conn, -1);
	int sock = conn->sock;

	return aos_tcp_send_raw(sock, data, len, is_blocking);
}


int aos_tcp_connect(
		int *sock, 
		const u32 local_addr, 
		const u16 local_start_port, 
		const int local_num_ports,
		const u32 remote_addr, 
		const u16 remote_port, 
		const int remote_num_ports,
		u16 *local_port_used, 
		u16 *remote_port_used)
{
	//
	// It establishes the connection. 
	// The caller should make sure that the connection 
	// is not there.  This function will check no more. 
	// The caller should have locked the connection.
	//
	aos_assert_r(sock, -1);

	aos_trace("Create TCP from: %s:%d:%d, to: %s:%d",
			aos_addr_to_str(local_addr), local_start_port, 
			local_num_ports, 
			aos_addr_to_str(remote_addr), remote_port);
		
    //
    // Create the socket
    //
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		aos_alarm("Failed to create socket to: %d", aos_get_errno());
    }

	int bindstatus = 0;
	struct sockaddr_in sockAddr;
	if (local_start_port == 0)
	{
	 	// 
	 	// No need to loop. 
	 	//
	 	memset((char*)&sockAddr, 0, sizeof(sockAddr));

	 	sockAddr.sin_family = AF_INET;
	 	aos_set_addr(&sockAddr, local_addr);
	 	sockAddr.sin_port = 0;

	 	aos_trace("To bind TCP Client to: %d. Local: %s:%d", 
			   *sock, aos_addr_to_str(local_addr), 
			   local_start_port);

 		aos_assert_gm(!bind(*sock, (struct sockaddr*)&sockAddr, 
				sizeof(sockAddr)), cleanup, "Local: %s:%d", 
				aos_addr_to_str(local_addr), local_start_port);
	
 		//
 		// Retrieve the port
 		//
 		struct sockaddr_in sockAdd;
 		memset((char*) &sockAdd, 0, sizeof (sockAdd));
 		aos_sock_len_t addrLen = sizeof(sockAdd);
 		aos_assert_g(!getsockname(*sock, (struct sockaddr*)&sockAdd,
			   	&addrLen), cleanup);
 		*local_port_used = ntohs(sockAdd.sin_port);
		aos_trace("Port used: %d", *local_port_used);
 	}
 	else
 	{
 		// 
 		// Need to bind to a local port. Will loop
 		//
		int i;
 		for (i=0; i<local_num_ports; i++)
 		{
 			//
 			// Bind the socket to the specified local address and port
 			//
 			memset((char*)&sockAddr, 0, sizeof(sockAddr));
 
			sockAddr.sin_family = AF_INET;
	 		aos_set_addr(&sockAddr, local_addr);
 			sockAddr.sin_port = htons(local_start_port+i);
 	
			aos_trace("To bind: %d, %s:%d", *sock, 
					aos_addr_to_str(local_addr), 
					local_start_port);
 
			if (bind(*sock, (struct sockaddr*)&sockAddr, 
						sizeof(sockAddr)) != 0)
 			{
 				continue;
 			}
 		
			aos_trace("Bind success");
 			bindstatus = 1;
 			break;
 		}
				
 		aos_assert_gm(bindstatus, cleanup, "Failed to bind: %s:%d:%d", 
				aos_addr_to_str(local_addr), 
				local_start_port, local_num_ports);
 		
 		//
 		// Bind successful. Retrieve the port used.
 		//
 		aos_sock_len_t addrLen = sizeof(sockAddr);
 		aos_assert_g(!getsockname(*sock, 
				(struct sockaddr*)&sockAddr, &addrLen), cleanup);
 		*local_port_used = ntohs(sockAddr.sin_port);
 		aos_trace("On port = %d", *local_port_used);
	}

 	// 
 	// To connect
 	//
   	struct sockaddr_in servAddr;
   	memset((char *)&servAddr, 0, sizeof(servAddr));
	
   	servAddr.sin_family = AF_INET;
   	aos_set_addr(&servAddr, remote_addr);
   	servAddr.sin_port = htons(remote_port);

   	aos_trace("To connect to %s:%d",
  		aos_addr_to_str(remote_addr), 
		remote_port);

	aos_assert_g(!connect(*sock, (struct sockaddr *)&servAddr, 
				sizeof(servAddr)), cleanup);

	aos_trace("Connect successful: %d to %s:%d", 
			*sock, aos_addr_to_str(remote_addr), remote_port);

	struct linger ling;
   	ling.l_onoff = 0;
   	ling.l_linger = 0;
	setsockopt(*sock, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
   	setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	return 0;

cleanup:
	aos_close_sock(*sock);
	return -1;
}     


int aos_unix_connect(
		int *sock, 
		const char * const unix_path, 
		const u32 remote_addr, 
		const u16 remote_port)
{
    //
    // Create the socket
    //
    *sock = socket(PF_UNIX, SOCK_STREAM, 0);
	aos_assert_r(*sock > 0, -1);

   	struct sockaddr_un servAddr;
   	memset((char *) &servAddr, 0, sizeof(servAddr));
	
   	servAddr.sun_family = AF_UNIX;
   	strcpy(servAddr.sun_path, unix_path);

	aos_trace("To connect to: %s:%d", 
		aos_addr_to_str(remote_addr), remote_port);

	aos_assert_r(!connect(*sock, (struct sockaddr *)&servAddr, 
			sizeof(servAddr)), -1);

	struct linger ling;
   	ling.l_onoff = 0;
   	ling.l_linger = 0;
	setsockopt(*sock, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
   	setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);
	return 0;
}


static int aos_tcp_integrity_check(aos_conn_t *conn)
{
	return aos_conn_integrity_check(conn);
}


static int aos_tcp_v_connect(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(conn->sock <= 0, -1);
	aos_assert_r(aos_tcp_integrity_check(conn) == 1, -1);

	aos_assert_r(!aos_tcp_connect(&conn->sock, 
			conn->local_addr, conn->local_port, conn->local_num_ports, 
			conn->remote_addr, conn->remote_port, conn->remote_num_ports,
			&conn->local_used_port, 
			&conn->remote_used_port), -1);
	return 0;
}


int aos_tcp_v_read(
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
	aos_assert_r(is_conn_broken, -1);

	*is_eof = 0;
	aos_buffer_t *buffer = aos_create_conn_read_buffer();
	aos_assert_r(buffer, -1);
	int is_timedout; 
	int ret = aos_conn_read_from(conn->sock, buffer, sec, usec, 
			&is_timedout, 0, 0, is_conn_broken);
	aos_assert_r(!ret, -1);

	if (*is_conn_broken) return 0;

	aos_assert_r(callback, -1);
	aos_assert_r(!callback(conn, buffer, is_timedout, 
			conn->remote_addr, conn->remote_used_port), -1);
	return 0;
}


int aos_tcp_v_read_to_buff(
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


int aos_tcp_release_memory(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(!aos_conn_release_memory(conn), -1);
	aos_assert_r(conn->type == eAosSockType_Tcp, -1);
	return 0;
}


int aos_tcp_destroy(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(!conn->mf->release_memory(conn), -1);
	aos_assert_r(conn->type == eAosSockType_Tcp, -1);
	aos_free(conn);
	return 0;
}


char * aos_tcp_dump(aos_conn_t *conn)
{
	aos_not_implemented_yet;
	return "none";
}



static aos_tcp_mf_t sg_mf = 
{
	aos_conn_add_user_data,
	aos_conn_del_user_data,
	aos_conn_get_user_data,
	aos_conn_hold,
	aos_conn_put,
	aos_tcp_release_memory,
	aos_tcp_v_read,
	aos_tcp_v_read_to_buff, 
	aos_conn_is_conn_good,
	aos_tcp_v_connect,
	aos_conn_disconnect,
	aos_tcp_integrity_check,
	aos_tcp_destroy,
	aos_tcp_dump,
	aos_tcp_send
};

int aos_tcp_init(aos_tcp_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(!aos_conn_init((aos_conn_t *)conn), -1);
	return 0;
}

aos_tcp_t *aos_tcp_create(
		const u32 local_addr, 
		const u16 local_port, 
		const u16 local_num_ports, 
		const u32 remote_addr, 
		const u16 remote_port, 
		const u16 remote_num_ports) 
{
	aos_assert_r(local_addr, 0);
	aos_assert_r(remote_addr, 0);
	aos_assert_r(local_num_ports > 0, 0);
	aos_assert_r(remote_num_ports > 0, 0);

	aos_tcp_t *obj = aos_malloc(sizeof(aos_tcp_t));
	aos_assert_r(obj, 0);
	memset(obj, 0, sizeof(aos_tcp_t));
	obj->mf = &sg_mf;
	aos_assert_g(!aos_tcp_init(obj), cleanup);

	obj->local_addr = local_addr;
	obj->local_port = local_port;
	obj->local_num_ports = local_num_ports;
	obj->remote_addr = remote_addr;
	obj->remote_port = remote_port;
	obj->remote_num_ports = remote_num_ports;
	obj->type = eAosSockType_Tcp;

	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


