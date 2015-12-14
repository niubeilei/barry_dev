////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: socket.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef _ASM_SOCKET_H
#define _ASM_SOCKET_H

// #include <asm/sockios.h>

/* For setsockopt(2) */

#ifndef OMN_PLATFORM_MICROSOFT
#define SOL_SOCKET	1
#define SO_DEBUG				1
#define SO_REUSEADDR			2
#define SO_TYPE					3
#define SO_ERROR				4
#define SO_DONTROUTE			5
#define SO_BROADCAST			6
#define SO_SNDBUF				7
#define SO_RCVBUF				8
#define SO_KEEPALIVE			9
#define SO_OOBINLINE			10
#define SO_LINGER				13
#define SO_BSDCOMPAT			14
#define SO_RCVLOWAT				18
#define SO_SNDLOWAT				19
#define SO_RCVTIMEO				20
#define SO_SNDTIMEO				21
#define SO_ACCEPTCONN			30
#endif

#define SO_NO_CHECK				11
#define SO_PRIORITY				12
/* To add :#define SO_REUSEPORT 15 */
#define SO_PASSCRED				16
#define SO_PEERCRED				17

/* Security levels - as per NRL IPv6 - don't actually do anything */
#define SO_SECURITY_AUTHENTICATION		22
#define SO_SECURITY_ENCRYPTION_TRANSPORT	23
#define SO_SECURITY_ENCRYPTION_NETWORK		24

#define SO_BINDTODEVICE	25

/* Socket filtering */
#define SO_ATTACH_FILTER        26
#define SO_DETACH_FILTER        27

#define SO_PEERNAME		28
#define SO_TIMESTAMP		29
#define SCM_TIMESTAMP		SO_TIMESTAMP


#define SO_PEERSEC		31


//
// AosKernelExtension_1 Chen Ding, 03/30/2005
//
#define AOS_SO_KAPI						80
#define AOS_SO_INIT_AOS					81
// End of AosKernelExtension_1

#endif /* _ASM_SOCKET_H */
