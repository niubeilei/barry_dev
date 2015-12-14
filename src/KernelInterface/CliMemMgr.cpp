////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#include "KernelInterface/Cli.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelInterface/CliCmd.h"
#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>
#include <stdlib.h>
#define	HMD	"/root/AOS/src/DiskMgr/diskmgr"
#define TYPEFILE "/root/type"

#include "KernelInterface_exe/version.h"

int
AosCli::memMgrSetRecordTime(OmnString &rslt)
{
	printf("mem mgr set record time\n");
	return 1;
}

int
AosCli::memMgrSetRecordStop(OmnString &rslt)
{
	printf("mem mgr set record stop\n");
	return 1;
}

int
AosCli::memMgrShowMem(OmnString &rslt)
{
	printf("mem mgr show statistics\n");
	return 1;
}

#endif

