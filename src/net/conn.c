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
// 02/25/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "net/conn.h"

#include "alarm_c/alarm.h"
#include "porting_c/select.h"
#include "porting_c/socket.h"
#include "porting_c/get_errno.h"
#include "porting_c/ipv6.h"
#include "porting_c/error_msg.h"
#include "porting_c/addr.h"
#include "util_c/rc_obj.h"
#include "util_c/buffer.h"
#include "util_c/tracer.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


AOS_DECLARE_RCOBJ_LOCK;

int aos_conn_integrity_check(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(conn->local_addr, -1);
	aos_assert_r(conn->remote_addr, -1);
	aos_assert_r(conn->local_num_ports, -1);
	aos_assert_r(conn->remote_num_ports, -1);
	return 1;
}


int aos_conn_hold( struct aos_conn *conn)
{
	AOS_RC_OBJ_HOLD(conn);

}


int aos_conn_put( struct aos_conn *conn)
{
	AOS_RC_OBJ_PUT(conn);
}


//
// Description:
// This function determines whether the connection is
// good or not. The way to tell is to use "select(...)".
// For more information on select(...), please refer to 
// its mannual page.
//
// Returns:
// 1 if good.
//
int aos_conn_is_conn_good( struct aos_conn *conn)
{
	aos_assert_r(conn, -1);
	if (conn->sock <= 0)
	{
		return 0;
	}

    fd_set fds;
    FD_ZERO(&fds);
	FD_SET(conn->sock, &fds);

    struct timeval theTime;
    theTime.tv_sec = 0;
    theTime.tv_usec = 10000;
    
    int ret = aos_socket_select(conn->sock+1, &fds, 0, 0, &theTime);    
	if (ret >= 0)
	{
		//
		// Connection is good
		//
		return 1;
	}

	return 0;
}


int aos_conn_read_from(
		const int sock, 
		aos_buffer_t *buffer,
		const int timer_sec, 
		const int timer_usec,
		int *is_timedout, 
		u32 *remote_addr, 
		u16 *remote_port, 
		int *is_conn_broken)
{
	//
	// It reads a message from the connection. If "select()" 
	// fails, which means it is a connection error. It checks the 
	// type of error and may try maximum three times. If after three 
	// times trial, the problem is still there,
	// it is an problem. It reports the problem and returns error codes.
	//
	// Otherwise, if there is nothing in receive, it waits until either 
	// there is something or "timerValue" timeout. If it is timer out, 
	// it returns 0 and 'length' is set to 0. 
	// Otherwise, it returns the contents read.
	//
	// If 'timer_sec' < 0, it waits forever.
	//
	aos_assert_r(buffer, -1);
	aos_assert_r(buffer->buff_len > 0, -1);
	aos_assert_r(buffer->buff, -1);
	aos_assert_r(is_timedout, -1);
	aos_assert_r(is_conn_broken, -1);

	*is_timedout = 0;
	*is_conn_broken = 0;
	buffer->data_len = 0;

	aos_assert_r(sock > 0 || sock < MAX_SOCK_TO_SELECT, -1);

	fd_set fd; 	
	FD_ZERO(&fd);
	FD_SET(sock, &fd);
	int rslt;
	if (timer_sec <= 0)
	{
		aos_trace("To select: no timer: %d", sock);
		rslt = aos_socket_select(sock+1, &fd, 0, 0, 0);
	}
	else
	{
		aos_trace("To select with timer: %d, timer: %d:%d", 
				sock, timer_sec, timer_usec);
		struct timeval timer;
		timer.tv_sec = timer_sec;
		timer.tv_usec = timer_usec;
		rslt = aos_socket_select(sock+1, &fd, 0, 0, &timer);
	}

	if (rslt < 0)
	{
		//
		// It means select failed. 'errCode' is the error code. 
		//
		aos_alarm("Select error: %d", aos_get_errno());
		return -1;
	}

	if (rslt == 0)
	{
		//
		// It means timeout.
		//
		aos_trace("Reading timeout on: %d", sock);
		*is_timedout = 1;
		return 0;
	}

	//
	// Otherwise, there should be something to read
	//
	aos_trace("To read from %d", sock);

	int bytesRead = 0;
	if (remote_addr)
	{
		struct sockaddr_in  addr;
		addr.sin_family = AF_INET;
		int addrLen = sizeof(addr);
		aos_assert_r(remote_port, -1);

		bytesRead = aos_recv_from(sock, buffer->buff, buffer->buff_len, 
			(struct sockaddr *)&addr, &addrLen);

		*remote_addr = aos_retrieve_addr(&addr);
		*remote_port = ntohs(addr.sin_port);
	}
	else
	{
		bytesRead = aos_recv_from(sock, buffer->buff, 
				buffer->buff_len, 0, 0);
	}

	if (bytesRead > 0)
	{
		//
		// Reading successful. 
		//
		buffer->data_len = bytesRead;

		if (remote_addr)
		{
			aos_trace("Read from: sock: %d, from %s:%d", 
				sock, aos_addr_to_str(*remote_addr), *remote_port); 
		}
		else
		{
			aos_trace("Read from sock: %d", sock);
		}
		return 0;
	}

	if (bytesRead == 0)
	{
		//
		// It did not read anything. The remote side has gracefully 
		// closed the connection.
		//
		*is_conn_broken = 1;
		return 0;
	}

	//
	// Reading failed. The error code is returned through "errcode"
	//
	int errcode = aos_get_errno();
	aos_alarm("Reading failed: %d:%d: %s", 
			bytesRead, errcode, aos_read_error(errcode)); 
	return -1;
}


int aos_conn_release_memory(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	return 0;
}


int aos_conn_disconnect(aos_conn_t *conn)
{
	aos_assert_r(conn, -1);
	aos_assert_r(conn->sock > 0, -1);
	aos_assert_r(aos_conn_integrity_check(conn) == 1, -1);
	aos_assert_r(!aos_close_sock(conn->sock), -1);
	conn->sock = 0;
	return 0;
}


int aos_conn_add_user_data(
		aos_conn_t *conn, 
		aos_gen_ptr_tag_e tag, 
		void *ptr)
{
	aos_assert_r(conn, -1);
	aos_assert_r(aos_gen_ptr_tag_check(tag) == 1, -1);
	
	aos_assert_r(!conn->user_data_array.mf->add_element(
			&conn->user_data_array, 1, (char **)&conn->user_data), -1);
	conn->user_data[conn->user_data_array.noe].tag = tag;
	conn->user_data[conn->user_data_array.noe].ptr = ptr;
	conn->user_data_array.noe++;
	return 0;
}


int aos_conn_del_user_data(
		aos_conn_t *conn, 
		aos_gen_ptr_tag_e tag, 
		void *ptr)
{
	aos_assert_r(conn, -1);
	
	int i;
	for (i=0; i<conn->user_data_array.noe; i++)
	{
		if (conn->user_data[i].tag == tag &&
			conn->user_data[i].ptr == ptr)
		{
			aos_assert_r(!conn->user_data_array.mf->del_element(
					&conn->user_data_array, i, 1, 
					(char **)&conn->user_data), -1);
			return 0;
		}
	}

	aos_alarm("Tag not found: %d", tag);
	return -1;
}


int aos_conn_get_user_data(
		aos_conn_t *conn, 
		aos_gen_ptr_tag_e tag, 
		void **ptr)
{
	aos_assert_r(conn, -1);
	aos_assert_r(ptr, -1);
	
	int i;
	for (i=0; i<conn->user_data_array.noe; i++)
	{
		if (conn->user_data[i].tag == tag)
		{
			*ptr = conn->user_data[i].ptr;
			return 0;
		}
	}

	aos_alarm("Tag not found: %d", tag);
	*ptr = 0;
	return -1;
}


int aos_conn_init(aos_conn_t *conn)
{
	conn->magic = AOS_CONN_MAGIC;
	aos_assert_r(!aos_dyn_array_init(&conn->user_data_array, 
				(char **)&conn->user_data, sizeof(aos_gen_ptr_t), 
				1, AOS_CONN_MAX_USER_DATA), -1);
	return 0;
}

