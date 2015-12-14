////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosConfig.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosConfig.h"

#include <KernelSimu/compiler.h>
#include <KernelSimu/uaccess.h>

#include "aos/aosKernelAlarm.h"
#include "aos/aosKernelApi.h"
#include "aos/KernelApiStruct.h"
#include "aos/aosBridge.h"


int aosConfig_config(char __user *optval, int optlen)
{
	// 
	// The first two bytes in 'optval' is the Kernel ID. 
	// The next byte is a config ID. 
	//
/*
	char buf[3];
	if (optlen < 3)
	{
		return aosAlarmInt(eAosAlarmInvalidOptLen, optlen);
	}
		
	if (copy_from_user(buf, optval, 3))	
	{
		return eAosAlarmCopyFromUserErr;
	}

	switch (buf[2])
	{
	case eAosLoadConfig_core:
		 return aosConfig_loadConfigCore(optval, optlen);

	case eAosSaveConfig_core:
		 return aosConfig_saveConfigCore(optval, optlen);

	// case eAosLoadConfig_ipv4:
	// 	 return aosConfig_ipv4(optval, optlen);
	
	// case eAosSaveConfig_ipv4:
	// 	 return aosConfig_ipv4(optval, optlen);
	
	default:
		 return aosAlarmInt(eAosAlarmUnrecognizedConfigId, buf[2]);
	}
*/

	return 0;
}


int aosConfig_loadConfigCore(char __user *optval, int optlen)
{
/*
	struct aosKernelApi_ConfigCore data;

	if (optlen != sizeof(struct aosKernelApi_ConfigCore))
	{
		return aosAlarmInt2(eAosAlarmOptLenTooShort, 
			optlen, sizeof(struct aosKernelApi_ConfigCore));
	}

	if (copy_from_user(&data, optval, optlen))
	{
		return eAosAlarmCopyFromUserErr;
	}

	return aosBridge_loadConfig(&data);
*/
	return 0;
} 


int aosConfig_saveConfigCore(char __user *optval, int optlen)
{
	/*
	struct aosKernelApi_ConfigCore data;
	int ret;

	if (optlen != sizeof(struct aosKernelApi_ConfigCore))
	{
		return aosAlarmInt2(eAosAlarmOptLenTooShort, 
			optlen, sizeof(struct aosKernelApi_ConfigCore));
	}

	ret = aosBridge_saveConfig(&data);
	if (ret)
	{
		return ret;
	}

	if (copy_to_user(optval, &data, optlen))
	{
		return eAosAlarmCopyToUserErr;
	}

	*/

	return 0;
} 

