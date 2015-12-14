////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelAPI.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelAPI_KernelApi_h
#define Omn_KernelAPI_KernelApi_h

#include "aos/KernelEnum.h"
#include "aos/KernelApiStruct.h"
#include "Debug/Rslt.h"
#include "KernelSimu/compiler.h"
#include "KernelSimu/sock.h"
#include "Util/String.h"

#ifdef __KERNEL__
extern struct socket sgTestSock;
#include "aos/aosKernelApi.h"
#define aosKernelCli(x, y, z) \
		sock_setsockopt(&sgTestSock, SOL_SOCKET, AOS_SO_KAPI, (y), (z))
#define aosKernelCli1(x, y, z) sock_setsockopt(&sgTestSock, SOL_SOCKET, AOS_SO_INIT_AOS, (y), (z))
#else
#define aosKernelCli(x, y, z) setsockopt((x), SOL_SOCKET, AOS_SO_KAPI, (y), (z))
#define aosKernelCli1(x, y, z) setsockopt((x), SOL_SOCKET, AOS_SO_INIT_AOS, (y), (z))

#endif

typedef int (*AosKernelApiFunc)(int level,
                    int optname,
                    char __user *optval,
                    int optlen);

class OmnKernelApi
{
private:
	static int		mSSock;		// used to communicate with the kernel

public:
	static int			init();
	static int  startTimer(const OmnKernelTimerId timerId, 
								const int value, 
								const long data);
	static int	sendPkt(const int index,
                      			const unsigned long  sendAddr,
                      			const unsigned short sendPort,
                      			const unsigned long  recvAddr,
                      			const unsigned short recvPort,
                      			const unsigned int size,
                      			const int tos,
                      			const char *deviceName);

	static int			skb_put(const int index, const unsigned int size);
	static int			skb_tailroom(const int index);
	static int			skb_reserve(const int index, const unsigned int size);
	static int			skb_push(const int index, const unsigned int size);
	static int			skb_pull(const int index, const unsigned int size);
	static int			showSkb(const int index);
	static void			setSimuFlag(const bool b);

	static OmnString	getKernelApiError(const int code);

	static int 			sendToKernel(char *data, const int size);
#ifdef SG_ERR_STRING
	static void			setErrStr();
	static OmnString	getErr(const int errcode);
#endif
	static void			setKernelApiFunc(AosKernelApiFunc f);
};

#endif

