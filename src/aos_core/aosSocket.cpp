////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosSocket.cpp
// Description:
//	This file contains socket/sock related utility functions.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos_core/aosSocket.h"

#include <KernelSimu/net.h>
#include <KernelSimu/errno.h>
#include <KernelSimu/socket.h>
#include <KernelSimu/inet_common.h>
#include <KernelSimu/in.h>
#include <KernelSimu/tcp.h>
#include <KernelSimu/kconfig.h>
#include <KernelSimu/byteorder.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/fcntl.h>

#include "KernelPorting/CreateSock.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"


#ifdef OMN_UNIX_PLATFORM
#include <asm/fcntl.h>
#endif

#ifdef __KERNEL__
#include "linux/module.h"
#endif

int aosSocket_createAndConnectCli(char *data, unsigned int *length)
{
	// 
	// This is the CLI command function for aosSocketCreate(...)
	//
	int localAddr, localPort, remoteAddr, remotePort;
	int ret = aosKernelApi_getInt4(1, &localAddr, &localPort, &remoteAddr, &remotePort, data);
	struct socket *sock;

	*length = 0;
	if (ret)
	{
		return ret;
	}

	ret = aosSocket_createAndConnect(&sock, localAddr, localPort, remoteAddr, remotePort);
	return ret;
}


// 
// This function simulates the userland function to create a
// TCP socket. If successful, it returns 0 and the socket is tored
// in 'res'. 
//
int aosSocket_createAndConnect(struct socket **res, 
					u32 localAddr, 
					u16 localPort,
					u32 remoteAddr, 
					u16 remotePort)
{
	struct sockaddr_in laddr;
	struct sockaddr_in raddr;
	int ret;

	laddr.sin_family      = AF_INET;
	laddr.sin_addr.s_addr = localAddr;
	laddr.sin_port        = htons(localPort);

	raddr.sin_family      = AF_INET;
	raddr.sin_addr.s_addr = remoteAddr;
	raddr.sin_port        = htons(remotePort);


	// Create the sock
	*res = 0;
	if ((ret = aosSockCreateKern(PF_INET, SOCK_STREAM, IPPROTO_TCP, res)))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}
 
	if ((ret = inet_bind(*res, (struct sockaddr *)&laddr, sizeof(struct sockaddr_in))))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}

	if ((ret = inet_stream_connect(*res, (struct sockaddr *)&raddr, 
			// sizeof(struct sockaddr_in), (*res)->file->f_flags)))
			sizeof(struct sockaddr_in), O_RDWR)))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}

	// Sock created successfully. If it is a userland call, it is important to 
	// call sock_map_fd(socket) to map the socket to a file descriptor. If it is
	// called from kernel, we expect the caller has a way to manage the created
	// socket.
	return 0;
}


// 
// This function creates a listening socket. 
//
int aosSocket_createAndListen(struct socket **res, 
					u32 localAddr, 
					u16 localPort, 
					int backlog)
{
	struct sockaddr_in laddr;
	int ret;

	laddr.sin_family      = AF_INET;
	laddr.sin_addr.s_addr = localAddr;
	laddr.sin_port        = htons(localPort);

printk("To createAndListen\n");

	// Create the sock
	if ((ret = aosSockCreateKern(PF_INET, SOCK_STREAM, IPPROTO_TCP, res)))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}

	if ((ret = inet_bind(*res, (struct sockaddr *)&laddr, sizeof(struct sockaddr_in))))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}

	if ((ret = inet_listen(*res, backlog)))
	{
		return aosAlarmInt(eAosAlarm, ret);
	}

	return 0;
}


int aosSocket_free(struct socket *sk)
{
	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosSocket_free );
EXPORT_SYMBOL( aosSocket_createAndListen );
#endif

