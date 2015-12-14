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
#ifndef aos_net_udp_h
#define aos_net_udp_h

#include "porting_c/mutex.h"
#include "util_c/types.h"
#include "net/conn.h"

struct aos_udp;

typedef int (*aos_udp_send_t)(
 		struct aos_udp *conn, 
 		const char * const contents, 
 		const int len, 
 		const u32 remote_addr, 
 		const u16 remote_port);


#define AOS_UDP_MEMFUNC_DECL					\
	aos_udp_send_t				send_udp

#define AOS_UDP_MEMDATA_DECL					\


typedef struct aos_udp_mf
{
	AOS_CONN_MEMFUNC_DECL;
	AOS_UDP_MEMFUNC_DECL;
} aos_udp_mf_t;

typedef struct aos_udp
{
	aos_udp_mf_t *mf;

	AOS_CONN_MEMDATA_DECL;
	AOS_UDP_MEMDATA_DECL;
} aos_udp_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_udp_send(
		const int sock,
		const char * const data,
		const int length,
		const u32 local_addr, 
		const u16 local_port,
		const u32 remote_addr, 
		const u16 remote_port);

int aos_udp_connect(
		int *sock, 
		const u32 local_addr, 
		const u16 local_port, 
		const int local_num_ports,
		u16 *local_used_port);

extern int aos_udp_init(aos_udp_t *udp);
extern aos_udp_t *aos_udp_create(
		const u32 local_addr, 
		const u16 local_port, 
		const u16 local_num_ports); 
#ifdef __cplusplus
}
#endif


#endif


