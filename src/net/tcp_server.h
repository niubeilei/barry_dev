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
#ifndef aos_net_tcp_server_h
#define aos_net_tcp_server_h

#include "net/conn.h"
#include "net/types.h"
#include "porting_c/mutex.h"
#include "util_c/types.h"
#include "util_c/dyn_array.h"
#include <sys/types.h>


struct aos_thread;
struct aos_tcp_server;
struct aos_tcp;
struct aos_udp;

#define AOS_TCP_SERVER_MAGIC 162354645
#define AOS_TCP_SERVER_MAX_CONNS 50

typedef int (*aos_tcp_server_close_conn_t)(
		struct aos_tcp_server *server,
		struct aos_conn *conn);

typedef struct aos_conn * (*aos_tcp_server_get_conn_t)(
		struct aos_tcp_server *server,
		const int sock);

typedef int (*aos_tcp_server_get_conn_event_t)(
		struct aos_tcp_server *server,
		struct aos_conn **conn);

typedef int (*aos_tcp_server_add_conn_t)(
		struct aos_tcp_server *server,
		struct aos_conn *conn);

typedef int (*aos_tcp_server_wait_on_event_t)(
		struct aos_tcp_server *server,
		struct aos_conn **conn,
		int *timeout);

typedef int (*aos_tcp_server_check_conns_t)(
		struct aos_tcp_server *server);

typedef int (*aos_tcp_server_accept_new_conn_t)(
		struct aos_tcp_server *server,
		struct aos_conn **conn);

typedef int (*aos_tcp_server_connect_t)(
		struct aos_tcp_server *server);

typedef int (*aos_tcp_server_disconnect_t)(
		struct aos_tcp_server *server);

typedef int (*aos_tcp_server_remove_conn_t)(
		struct aos_tcp_server *server, 
		const int sock);

typedef int (*aos_tcp_server_start_reading_t)(
		struct aos_tcp_server *server);

typedef int (*aos_tcp_server_wake_up_t)(
		struct aos_tcp_server *server);


#define AOS_TCP_SERVER_MEMFUNC_DECL						\
	aos_tcp_server_close_conn_t			close_conn;		\
	aos_tcp_server_get_conn_t			get_conn;		\
	aos_tcp_server_get_conn_event_t		get_conn_event;	\
	aos_tcp_server_add_conn_t			add_conn;		\
	aos_tcp_server_wait_on_event_t		wait_on_event;	\
	aos_tcp_server_check_conns_t		check_conns;	\
	aos_tcp_server_accept_new_conn_t	accept_new_conn;\
	aos_tcp_server_connect_t			connect;		\
	aos_tcp_server_disconnect_t			disconnect;		\
	aos_tcp_server_remove_conn_t		remove_conn;	\
	aos_tcp_server_start_reading_t		start_reading


#define AOS_TCP_SERVER_MEMDATA_DECL					\
	u32							local_addr;			\
	u16							local_port;			\
	int							local_num_ports;	\
	u16							local_port_used;	\
	struct aos_thread *			thread;				\
	aos_conn_read_callback_t 	callback;			\
	fd_set						read_fds;			\
	fd_set						working_fds;		\
	int							sock;				\
	int							magic;				\
	aos_lock_t *				lock;				\
	struct aos_conn **			conns;				\
	aos_dyn_array_t				conns_array;		\
	int							fds_cnt;			\
	aos_sock_type_e				type;				\
	char *						unix_path

typedef struct aos_tcp_server_mf
{
	AOS_TCP_SERVER_MEMFUNC_DECL;
} aos_tcp_server_mf_t;

typedef struct aos_tcp_server
{
	aos_tcp_server_mf_t *mf;

	AOS_TCP_SERVER_MEMDATA_DECL;
} aos_tcp_server_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_tcp_server_connect(
			  	int *sock, 
			  	const aos_sock_type_e type,
			  	const u32 local_addr, 
				const u32 local_port,  
				const int local_num_ports,
				const char * const unix_path, 
				u16 *local_port_used);

extern aos_tcp_server_t *aos_tcp_server_create(
		const u32 local_addr, 
		const u16 local_port, 
		const int local_num_ports,
		aos_conn_read_callback_t callback,
		aos_sock_type_e type);

#ifdef __cplusplus
}
#endif


#endif


