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
#ifndef aos_net_tcp_h
#define aos_net_tcp_h

#include "porting_c/mutex.h"
#include "util_c/types.h"
#include "net/types.h"
#include "net/conn.h"

struct aos_tcp;

typedef int (*aos_tcp_send_t)(
 		struct aos_tcp *conn, 
 		const char * const contents, 
 		const int len, 
		const int is_blocking);

#define AOS_TCP_MEMFUNC_DECL					\
	aos_tcp_send_t				send_tcp

#define AOS_TCP_MEMDATA_DECL					\

typedef struct aos_tcp_mf
{
	AOS_CONN_MEMFUNC_DECL;
	AOS_TCP_MEMFUNC_DECL;
} aos_tcp_mf_t;

typedef struct aos_tcp
{
	aos_tcp_mf_t *mf;

	AOS_CONN_MEMDATA_DECL;
	AOS_TCP_MEMDATA_DECL;
} aos_tcp_t;

#ifdef __cplusplus
extern "C" {
#endif

extern int aos_tcp_send_raw(
		const int sock, 
		const char * const data, 
		const int len, 
		const int is_blocking);

extern int aos_tcp_send(
		struct aos_tcp *conn, 
		const char * const data, 
		const int len, 
		const int is_blocking);

extern int aos_tcp_connect(
		int *sock, 
		const u32 local_addr, 
		const u16 local_start_port, 
		const int local_num_ports,
		const u32 remote_addr, 
		const u16 remote_port, 
		const int remote_num_ports, 
		u16 *local_port_used, 
		u16 *remote_port_used);

extern int aos_tcp_init(aos_tcp_t *tcp);
extern aos_tcp_t *aos_tcp_create(
		const u32 local_addr,
		const u16 local_port,
		const u16 local_num_ports,
		const u32 remote_addr,
		const u16 remote_port,
		const u16 remote_num_ports);

#ifdef __cplusplus
}
#endif


#endif


