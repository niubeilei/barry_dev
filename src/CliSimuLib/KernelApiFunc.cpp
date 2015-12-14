////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelApiFunc.cpp
// Description:
// 	This is a simulation of net/core/sock.c. This file is used as the
//  replacement of 'sock.c' when code is compiled in the KernelSimulate
//  mode. 
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

// #include <KernelSimu/socket.h>
// #include <KernelSimu/sock.h>
// #include <KernelSimu/net.h>
// #include <KernelSimu/uaccess.h>
// #include <KernelSimu/errno_base.h>
// #include <KernelSimu/aosKernelDebug.h>
// #include "KernelUtil/KernelMem.h"
// #include "aos/socket.h"

#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aos/aosCoreComm.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aos_core/aosSockCtl.h"


int AosKernelApi_setSockopt(int level, 
					int optname,
		    		char __user *optval, 
					int optlen)
{
	int ret = 0;
		
  	if((unsigned int)optlen<sizeof(int))
	{
		return aos_alarm("Invalid optlen: %d", optlen);
	}
  	
  	switch(optname) 
  	{
		// 
		// AosKernelExtension_1: Chen Ding, 04/09/2005
		// 
		case AOS_SO_INIT_AOS:
		case AOS_SO_KAPI:
		case 3264:
			 ret = aosSockSetopt(0, optname, optval, optlen);
			 break;

		default:
			aos_alarm("Unrecognized name: %d\n", optname);
		  	ret = -eAosRc_InvalidKernelApiCode;
			break;
  	}
	return ret;
}

