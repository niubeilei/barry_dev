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
#include "net/tcp_server.h"

#include "alarm_c/alarm.h"
#include "net/types.h"
#include "net/tcp.h"
#include "net/udp.h"
#include "porting_c/addr.h"
#include "porting_c/sock_len.h"
#include "porting_c/socket.h"
#include "porting_c/ipv6.h"
#include "porting_c/select.h"
#include "porting_c/sleep.h"
#include "porting_c/get_errno.h"
#include "thread_c/thread.h"
#include "util_c/tracer.h"
#include "util_c/buffer.h"
#include "util_c/memory.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


//
// This function creates the server socket and make connection.
// If connection failed, it will try until either the connection is
// established or timer "timerValue" expires.
//
int aos_tcp_server_connect(
		int *sock, 
		const aos_sock_type_e type,
		const u32 local_addr, 
		const u32 local_port, 
		const int local_num_ports,
		const char * const unix_path,
		u16 *local_port_used)
{
	aos_assert_r(sock, -1);
	aos_assert_r(*sock <= 0, -1);
	aos_assert_r(local_num_ports > 0, -1);
	aos_assert_r(local_addr, -1);

	if (type == eAosSockType_Tcp) 
	{
		*sock = socket(AF_INET, SOCK_STREAM, 0);
	}
	else if (type == eAosSockType_Unix)
	{
		*sock = socket(AF_UNIX, SOCK_STREAM, 0);
	}
	aos_assert_r(*sock > 0, -1);

	int bindSuccess = 0;
	if (type == eAosSockType_Tcp)
	{
		int i;
		struct sockaddr_in serv_addr;

		for (i=0; i<local_num_ports; i++)
		{
			//
			// To bind the socket to local_port
			//
			aos_trace("To bind: %d", local_port + i);
			memset((char *) &serv_addr, 0, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_addr.s_addr = local_addr;
			serv_addr.sin_port = htons(local_port+i);
			int on=1;
			setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

			if (bind(*sock, (struct sockaddr *) &serv_addr, 
						sizeof(serv_addr)) != 0)
			{
				// 
				// Failed the binding. If local port is 0, it must be
				// the case that the IP address is not local. Abort.
				//
				if (local_port == 0)
				{
					aos_alarm("Failed to bind: most likely the "
						"local address is invalid: %s", 
						aos_addr_to_str(local_addr));
					return -1;
				}

				//
				// Try the next port
				//
				continue;
			}
			
			//
			// Note that if local_port is 0, it means that we want 
			// the system to assign a port instead of us requesting 
			// the use of a specific port.
			// In this case, we need to retrieve the port number.
			//
			bindSuccess = 1;
			if (local_port == 0)
			{
				aos_sock_len_t len = sizeof(serv_addr); 
				aos_assert_r(!getsockname(*sock, 
					(struct sockaddr *) &serv_addr, &len), -1);
				*local_port_used = serv_addr.sin_port;
			} 
			else
			{
				*local_port_used = local_port + i;
			}
	
			break;
		}
	}
	else if (type == eAosSockType_Unix)
	{
		struct sockaddr_un serv_addr;
		char cmd[100];
		aos_assert_r(unix_path, -1);
		sprintf(cmd, "chmod 777 %s", unix_path);

		unlink(unix_path);

		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strncpy(serv_addr.sun_path, unix_path, 
					sizeof(serv_addr.sun_path)-1);
		
		aos_assert_r(!bind(*sock, (struct sockaddr *) &serv_addr, 
					sizeof(serv_addr)), -1);

		//
		// Add the permission so as to let other access it
		//
		system(cmd);
		bindSuccess = 1;
	}

	if (!bindSuccess)
	{
		// 
		// Failed binding. 
		//
		aos_alarm("Can't bind local address port: %s:%d:%d", 
				aos_addr_to_str(local_addr), local_port, 
				local_num_ports);
		
		close(*sock);
		return -1;
	}

	//
	// Listen to the newly created port. 
	//
	aos_trace("Begin listening: %d", *sock);
	struct linger ling;
	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt(*sock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
	setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	aos_assert_r(!listen(*sock, 50), -1);

	aos_trace("Listen success on: %d", *sock);
	return 0;
}


int aos_tcp_server_close_conn(
		aos_tcp_server_t *server, 
		aos_conn_t *conn)
{
	aos_assert_r(server, -1);

	//
	// When closing a connection,
	// two things need to be done. One is to modify
	// the read_fds and the other is to remove the entry.
	//

	//
	// Modify mReadFds
	//
	// if(server->conn_callback)
	// {
	// 	server->conn_callback(conn, 1);
	// }

	int sock = conn->sock;
	aos_assert_r(sock > 0, -1);
	aos_trace("To close connection: %d", sock);

	aos_assert_r(!aos_tcp_server_close_conn(server, conn), -1);
	conn->mf->disconnect((aos_conn_t *)conn);
	aos_lock(server->lock);
    FD_CLR((size_t)sock, &server->read_fds);
	aos_unlock(server->lock);
    return 0;
}


aos_conn_t *aos_tcp_server_get_conn(
		aos_tcp_server_t *server, 
		const int sock)
{
	//
	// The server keeps a list of all active connections in 
	// server->conns. This function will look up the list 
	// If it is found, the connection is returned.
	// Otherwise, null is returned;
	//
	int i;
	aos_trace("To get conn: %d", socket);
	aos_lock(server->lock);
	for (i=0; i<server->conns_array.noe; i++)
	{
        if (server->conns[i]->sock == sock)
        {
			aos_conn_t *conn = server->conns[i];
			aos_unlock(server->lock);
            return conn;
        }
    }

	//
	// Not found
	//
	aos_unlock(server->lock);
    return 0;
}


//
// Description:
// It assumes that events have been detected and recorded on 
// working_fds. This function searches the entries in 'conns' 
// to see which ones have events. The first one that has
// the event is retrieved. The corresponding flags in 'working_fds' 
// are cleared so that the next time this function is called,
// it will not be found again. Eventually, all responsible
// connections will be processed. If false is returned, it means
// that no one is responsible for the events.
//
// If it returns 1, 'conn' is guaranteed not null. If it returns
// 0, 'conn' is set to 0.
//
// Returns:
// 1 if found. 
// 0 if not found.
//
int aos_tcp_server_get_conn_event(
		aos_tcp_server_t *server, 
		aos_conn_t **conn)
{
	int i;
	aos_conn_t *client;
	aos_lock(server->lock);
	for (i=0; i<server->conns_array.noe; i++)
	{
        client = server->conns[i];
		
		//
		// Check whether the connection is responsible for the events.
		//
		if(FD_ISSET(client->sock, &server->working_fds))
        {
			// Found the connection
			*conn = client;
			aos_unlock(server->lock);
            return 1;
        }
    }

	aos_unlock(server->lock);

	*conn = 0;
    return 0;
}


int aos_tcp_server_add_conn(
		aos_tcp_server_t *server, 
		aos_conn_t *conn)
{
	//
	// When new connection is detected and accepted successfully,
	// an instance of OmnTcpClient is created. This function
	// is called to add the connection to mConnections, to set
	// mReadFds, mFdscnt. It will also check whether the connection
	// is already there. If it is, it is an error. 
	//

	int theSock = conn->sock;

aos_trace("Noe: %d", server->conns_array.noe);

	//
	// Find the index of the element in mConnections that 
	// corresponds to theSock.
	//
    aos_conn_t *c = server->mf->get_conn(server, theSock);

    if (!c)
    {
		//
		// This means that the connection doesn't exist.
		// Set the flag, add the connection to the connection list
		// and return.
		//
		aos_trace("To set sock: %d", theSock);
		aos_lock(server->lock);
        FD_SET(theSock, &server->read_fds); 

aos_trace("fds_cnt: %d", server->fds_cnt);
        if (server->fds_cnt < theSock+1)
        {
aos_trace("Set new fds cnt: %d", server->fds_cnt);
            server->fds_cnt = theSock + 1 ;
        }

		aos_assert_r(!server->conns_array.mf->add_element(
				&server->conns_array, 1, (char **)&server->conns), -1);
		server->conns[server->conns_array.noe++] = conn;
		aos_unlock(server->lock);

		// server->mf->wake_up(server);
aos_trace("conn added: %d", conn->sock);
        return 0;
    }

	aos_alarm("Connection already exist: %s", 
			conn->mf->dump((aos_conn_t *)conn));
    return -1;
}


/*
int aos_tcp_server_wake_up(aos_tcp_server_t *server)
{
aos_trace("To wake up");
	aos_assert_r(server, -1);
	aos_assert_r(server->control_conn1, -1);
	aos_assert_r(server->control_conn2, -1);
	aos_assert_r(!server->control_conn2->mf->send_udp(
		server->control_conn2, 
		"c", 1, server->control_conn1->local_addr, 
		server->control_conn1->local_used_port), -1);
	return 0;
}
*/


void aos_tcp_server_thread_func(aos_thread_t *thread)
{
	int timeout;
	int isEndOfFile;
	aos_conn_t *conn  = 0;
	aos_tcp_server_t *server = 0;

	aos_assert(thread);
	aos_assert(thread->user_data);
	server = (aos_tcp_server_t *)thread->user_data;
	aos_assert(server->magic == AOS_TCP_SERVER_MAGIC);

	aos_trace("Entering tcp_server thread function");
	aos_buffer_t buffer;
	aos_assert(!aos_buffer_init(&buffer));
	aos_assert(!buffer.mf->alloc_memory(&buffer, 1000));
    while (1)
    {
		conn = 0;

		while (server->sock < 0)
		{
			aos_alarm("Server socket bad. Try to reconnect ...");

			if (aos_tcp_server_connect(
					&server->sock, 
					server->type, 
					server->local_addr, 
					server->local_port, 
					server->local_num_ports, 
					server->unix_path, 
					&server->local_port_used))
			{
				aos_alarm("Failed to reconnect. Will sleep one "
					"second and retry ...");
				aos_sleep(1);
				continue;
			}

			// 
			// Connection re-connected
			//
			break;
		}

		aos_trace("Wait on event");
    	if(server->mf->wait_on_event(server, &conn, &timeout))
		{			
			aos_alarm("Failed on wait_on_event");
			continue;		
		}

		if (timeout)
		{
			aos_trace("Timedout");
			continue;
		}

		if (!conn)	
		{
			continue;
		}

		//
		// Found a valid connection that has something to read
		//
		int is_conn_broken;
		if (conn->mf->read((aos_conn_t *)conn, 0, 1000, 
					server->callback, &isEndOfFile, 
					&is_conn_broken))
		{
			// 
			// Either the remote has closed the connection or something
			// wrong with the connection. We will close the connection.
			//
			aos_alarm("Reading failed: %s", 
					conn->mf->dump((aos_conn_t *)conn));
			aos_assert(!server->mf->remove_conn(server, conn->sock));
			conn->mf->disconnect((aos_conn_t *)conn);
		}
		else
		{
			if (isEndOfFile || is_conn_broken)
			{
				// 
				// The other side has finished sending the contents. 
				// Ready to close the connection.
				//
				aos_assert(!server->mf->remove_conn(server, conn->sock));
				aos_assert(!conn->mf->disconnect((aos_conn_t *)conn));
				continue;
			}
		}
	}

	return;
}


int aos_tcp_server_wait_on_event(
		aos_tcp_server_t *server,
		aos_conn_t **conn,
		int *timeout)
{
    //
    // This member function determines whether there is any event on
    // any connection this class manages, or whether there is any
    // new connection request.
    //
    // If there is at least one event, it finds which connId and TCP
    // the event occurs and returns true.
    //
    // Whether there is event or not is indicated by "working_fds", 
	// which was set by the most recent call of "select(...)".
	//
    //
    // If there is no event, it goes to a while(1) loop, which first
    // reset "working_fds" to "read_fds", and then call the 
	// "select(...)".
    // Two things can cause the call return. One is there is at least
    // one event on one of the connections and the other is that there
    // is a new connection request.
    //
    // Once returned from "select(...)", it checks new connection 
	// first. If there is new connection, it creates a new instance 
	// of TCP, and adds the connection.
    //
    // It then checks whether there is any new events. If no, it 
	// goes back to the beginning of the loop. Otherwise, it returns.
    // TCP.
    //

	//
	// It checks whether there is any events on working_fds. This may
	// be the events it received the last time and haven't been 
	// processed yet. If it finds, it gets the connId and conn, 
	// clear the flag, and then return.
	//
	aos_assert_r(server, -1);
	aos_assert_r(conn, -1);
	aos_assert_r(timeout, -1);

	*timeout = 0;
	aos_trace("To get event");
    if (aos_tcp_server_get_conn_event(server, conn) == 1)
    {
		// 
		// Events are found. So we need to clear the flag and return.
		//
		aos_lock(server->lock);
        FD_CLR((size_t)(*conn)->sock, &server->working_fds);
		aos_unlock(server->lock);
		aos_trace("Got event");
        return 0;
    }

	// 
	// This means there is no more event. The following will use 
	// the select(...) to wait on new events. 
	//
    while (1)
    {
		aos_assert_r(server->sock > 0, -1);

		//
		// In this loop, it first go 'select'. When it is waken up,
		// it checks whether the select was successful. If failed,
		// it can be something intermident (EINTR or EINVAL). Or
		// it is possible that the server connection is not okay.
		//
        server->working_fds = server->read_fds;

	    struct timeval timer;
		timer.tv_sec = 100;
		timer.tv_usec = 0;
				
		int ret = aos_socket_select(server->fds_cnt, 
				&server->working_fds, 0, 0, &timer);

		if(ret == 0)
		{
			// timeout
			*timeout = 1;
			return 0;
		}

		if(ret < 0)
        {
			aos_alarm("Failed to select");
            switch (aos_get_errno())
            {
                case EINTR:
                case EINVAL:
                     continue;

                case EBADF:
					 aos_alarm("Bad connections found!");
                     server->mf->check_conns(server);
                     continue;

                default:
					 //
					 // It failed the selection. This should never 
					 // happen.
					 //
					 aos_alarm("Something is wrong in select");
                     return -1; 
            }

        }

		//
        // This means the select is successful. Check if new 
		// connection is present.
		//
        ret = FD_ISSET(server->sock, &server->working_fds);
		if(ret == 1)
        {
			//
			// It means that new connections arrived. 
			//
			aos_trace("New connection detected");
			aos_conn_t *new_conn;
			aos_assert_r(!server->mf->accept_new_conn(server, 
					&new_conn), -1);

			FD_CLR(server->sock, &server->working_fds);
            continue;
        }

		//
		// If it comes to this point, it means there is no more 
		// connection requests. Now it needs to check whether 
		// there are reading requests.
		//
        if (server->mf->get_conn_event(server, conn) == 1)
        {
			//
			// It means that there are reading requests.
			// Clear the bit.
			//
			aos_assert_r(*conn, -1);
			aos_lock(server->lock);

            FD_CLR((*conn)->sock, &(server->working_fds));
			aos_unlock(server->lock);
            return 0;
        }
    }

	aos_should_never_come_here;
    return -1;
}


int aos_tcp_server_check_conns(aos_tcp_server_t *server)
{
    //
    // This function is called when a bad file descriptor is found.
    // This function goes over all the connections. If a connection is
    // bad, it removes it.
    //
	int i;
	aos_assert_r(server, -1);
	aos_assert_r(server->lock, -1);

	aos_lock(server->lock);
	aos_conn_t *conn;
	for (i=0; i<server->conns_array.noe; i++)
    {
		conn = server->conns[i];
        if (conn->mf->is_conn_good((aos_conn_t *)conn) != 1)
        {
			aos_alarm("Bad connection found: %s", 
				conn->mf->dump((aos_conn_t *)conn));
            FD_CLR((size_t)conn->sock, &server->read_fds);
			aos_assert_g(!server->conns_array.mf->del_element(
					&server->conns_array, i, 1, 
					(char **)&server->conns), cleanup);
			i--;
        }
    }
	aos_unlock(server->lock);
	return 0;

cleanup:
	aos_unlock(server->lock);
	return -1;
}


int aos_tcp_server_accept_new_conn(
		aos_tcp_server_t *server,
		aos_conn_t **conn)
{
    // 
    // This function is called to accept a new connection.
	// An instance of aos_tcp_t will be created for
	// this connection.
	// 
	aos_assert_r(server, -1);
	aos_assert_r(conn, -1);
	aos_assert_r(server->callback, -1);

	struct sockaddr_in cli_addr;
	aos_sock_len_t clilen = (aos_sock_len_t)sizeof(cli_addr);
	memset((char *) &cli_addr, 0, sizeof(cli_addr));

    int new_sock = accept(server->sock, 
			(struct sockaddr *) &cli_addr, &clilen);
	aos_assert_r(new_sock > 0, -1);

    u32 remote_addr = aos_retrieve_addr(&cli_addr);
	u16 remote_port = ntohs(cli_addr.sin_port);

	aos_trace("Accepted new sock: %d, %s:%d", 
		   new_sock, 
	   	   aos_addr_to_str(remote_addr), remote_port);

	*conn = (aos_conn_t *)aos_tcp_create(server->local_addr, 
			server->local_port_used, 1,
			remote_addr, remote_port, 1);
	aos_assert_r(*conn, -1);
	(*conn)->sock = new_sock;
	(*conn)->callback = server->callback;
	(*conn)->local_used_port = server->local_port_used;
	(*conn)->remote_used_port = remote_port;
	aos_assert_r(!server->mf->add_conn(server, *conn), -1);
    return 0;
}


int aos_tcp_server_v_connect(aos_tcp_server_t *server)
{
	aos_assert_r(server, -1);
	aos_assert_r(server->local_addr, -1);
	aos_assert_r(server->local_num_ports, -1);

	aos_assert_r(!aos_tcp_server_connect(
			&server->sock, 
			server->type,
			server->local_addr, 
			server->local_port, 
			server->local_num_ports, 
			server->unix_path, 
			&server->local_port_used), -1);

	aos_assert_r(server->sock > 0, -1);
	aos_assert_r(server->fds_cnt == 0, -1);
	aos_lock(server->lock);
    FD_SET(server->sock, &server->read_fds); 
	server->fds_cnt = server->sock+1;
	aos_unlock(server->lock);
	return 0;
}


int aos_tcp_server_disconnect(aos_tcp_server_t *server)
{
	int i;
	aos_assert_r(server, -1);
	aos_assert_r(server->sock > 0, -1);

	//
	// Need to close all member connections
	//
	aos_lock(server->lock);
	for (i=0; i<server->conns_array.noe; i++)
    {
		server->conns[i]->mf->disconnect((aos_conn_t *)
				server->conns[i]);
    }

	aos_unlock(server->lock);
	aos_close_sock(server->sock);
	server->sock = -1;
	return 0;
}


int aos_tcp_server_remove_conn(
		aos_tcp_server_t *server, 
		const int sock)
{
	int i, removed;
	aos_trace("To remove connection: %d", sock);
	aos_assert_r(server, -1);
	aos_assert_r(sock > 0, -1);
	aos_assert_r(server->lock, -1);

	aos_lock(server->lock);
	removed = 0;
	for (i=0; i<server->conns_array.noe; i++)
	{
        if (server->conns[i]->sock == sock)
		{
			aos_assert_r(!server->conns_array.mf->del_element(
					&server->conns_array, i, 1, 
					(char **)&server->conns), -1);
			FD_CLR(sock, &server->read_fds);
			aos_unlock(server->lock);
			aos_trace("Connection: %d removed", sock);
			removed = 1;
			break;
		}
	}

	if (!removed)
	{
		aos_alarm("To remove connect: %d but not found", sock);
	}

	// 
	// Need to find the new fds_cnt
	//
	if (sock == server->fds_cnt-1)
	{
		int max = server->sock + 1;
		for (i=0; i<server->conns_array.noe; i++)
		{
			if (max < server->conns[i]->sock + 1)
			{
				max = server->conns[i]->sock + 1;
			}
		}

		server->fds_cnt = max;
	}
	aos_unlock(server->lock);

    return 0;
}


int aos_tcp_server_start_reading(aos_tcp_server_t *server)
{
	aos_assert_r(server, -1);
	aos_assert_r(server->callback, -1);
	aos_assert_r(!server->thread, -1);
	server->thread = aos_thread_create("no_name", 
			0, aos_tcp_server_thread_func, server, 1, 1, 0);
	aos_assert_r(server->thread, -1);
	return 0;
}	


static aos_tcp_server_mf_t sg_mf = 
{
	aos_tcp_server_close_conn,
	aos_tcp_server_get_conn,
	aos_tcp_server_get_conn_event,
	aos_tcp_server_add_conn,
	aos_tcp_server_wait_on_event,
	aos_tcp_server_check_conns,
	aos_tcp_server_accept_new_conn,
	aos_tcp_server_v_connect,
	aos_tcp_server_disconnect,
	aos_tcp_server_remove_conn,
	aos_tcp_server_start_reading
};


aos_tcp_server_t *aos_tcp_server_create(
		const u32 local_addr, 
		const u16 local_port, 
		const int local_num_ports,
		aos_conn_read_callback_t callback,
		aos_sock_type_e type)
{
	aos_assert_r(local_addr, 0);
	aos_assert_r(local_num_ports > 0, 0);
	aos_assert_r(callback, 0);
	aos_assert_r(aos_sock_type_check(type) == 1, 0);

	aos_tcp_server_t *obj = aos_malloc(sizeof(aos_tcp_server_t));
	aos_assert_r(obj, 0);
	memset(obj, 0, sizeof(aos_tcp_server_t));
	obj->mf = &sg_mf;

	obj->lock = aos_malloc(sizeof(aos_lock_t));
	aos_assert_g(obj->lock, cleanup);
	aos_init_lock(obj->lock);

	aos_assert_g(!aos_dyn_array_init(&obj->conns_array, 
			(char **)&obj->conns, sizeof(aos_conn_t *), 5, 
			AOS_TCP_SERVER_MAX_CONNS), cleanup);
	obj->magic = AOS_TCP_SERVER_MAGIC;
	obj->local_addr = local_addr;
	obj->local_port = local_port;
	obj->local_num_ports = local_num_ports;
	obj->callback = callback;
	obj->type = type;
	FD_ZERO(&obj->read_fds);
	FD_ZERO(&obj->working_fds);

aos_trace("Noe: %d", obj->conns_array.noe);

	return obj;

cleanup:
	if (obj->lock) aos_free(obj->lock);
	aos_free(obj);
	return 0;
}

