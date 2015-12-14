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
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include "KernelInterface_exe/version.h"

#define CPUMGR_FULLPATH "/usr/local/AOS/Bin/CpuMgr"
#define CPUMGR_PROCNAME "CpuMgr"
#define CPULOG_FULLPATH "/var/log/aos_cpu_log.0"

int
AosCli::cpuMgrSetRecordTime(OmnString &rslt)
{
	int curPos = 0;
	OmnString interval_time;
	OmnString record_time;
	OmnString strCmdLine;
//	curPos = mFileName.getWord(curPos, interval_time);
	curPos = mFileName.getWord(curPos, record_time);

	strCmdLine << CPUMGR_FULLPATH << " 1 " << record_time;
	OmnCliSysCmd::doShell(strCmdLine,rslt);
	return 1;
}

int
AosCli::cpuMgrSetRecordStop(OmnString &rslt)
{
	OmnString strCmdLine;

	strCmdLine << "pkill " << CPUMGR_PROCNAME;
	OmnCliSysCmd::doShell(strCmdLine,rslt);
	return 1;
}

int
AosCli::cpuMgrShowCpu(OmnString &rslt)
{

	int curPos = 0;
	OmnString start_time;
	OmnString end_time;
	OmnString count;
	OmnString strCmdLine;

	curPos = mFileName.getWord(curPos, start_time);
	curPos = mFileName.getWord(curPos, end_time);
	curPos = mFileName.getWord(curPos, count);

	// Load time function 
	// char *strptime(const char *s, const char *format, struct tm *tm)
	return 1;
}

int
AosCli::cpuMgrShowProcess(OmnString &rslt)
{
	int curPos = 0;
	OmnString start_time;
	OmnString end_time;
	OmnString times;
	curPos = mFileName.getWord(curPos, start_time);
	curPos = mFileName.getWord(curPos, end_time);
	curPos = mFileName.getWord(curPos, times);

	return 1;
}

#endif

