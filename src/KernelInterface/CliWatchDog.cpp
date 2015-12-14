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

#include "KernelInterface_exe/version.h"

#define	WATCHDOG "/usr/local/AOS/Bin/watchdog"

int
AosCli::watchdogSetStatus(OmnString &rslt)
{
	int curPos = 0;
	OmnString status;
	OmnString interval;
	OmnString systemCmd;

	curPos = mFileName.getWord(curPos, status);
	curPos = mFileName.getWord(curPos, interval);

	if (strcmp(status, "on"))
		return -1;
	systemCmd << WATCHDOG << " " << interval; 
	OmnCliSysCmd::doShell(systemCmd, rslt);

	return 1;
}

int
AosCli::watchdogStop(OmnString &rslt)
{
	return 1;
}

#endif

