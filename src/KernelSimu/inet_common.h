////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: inet_common.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_inet_common_h
#define Omn_aos_KernelSimu_inet_common_h

#ifdef AOS_KERNEL_SIMULATE

struct socket;
struct sockaddr;

extern int	inet_bind(struct socket *sock, struct sockaddr *uaddr, int addr_len);
extern int	inet_stream_connect(struct socket *sock,
				    struct sockaddr * uaddr,
				    int addr_len, int flags);
extern int 	inet_listen(struct socket *sock, int backlog);

#else
#include <asm/atomic.h>
#include <net/inet_common.h>
#endif

#endif

