////////////////////////////////////////////////////////////////////////////
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
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysConsole/ConsoleProc.h"

#include "Thread/Mutex.h"
#include "SysConsole/CslProcGetCpuUsage.h"

static OmnMutex sgLock;
static bool sgInited = false;

AosConsoleProc::map_t AosConsoleProc::smProcs;

AosConsoleProc::AosConsoleProc(
		const OmnString &info_id, 
		const bool flag)
:
mInfoId(info_id)
{
	if (flag) registerProc();
}


AosConsoleProc::init()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		OmnAlarm << "Already inited" << enderr;
		return false;
	}

	sgProcs.push_back(OmnNew AosCslProcGetCpuUsage(true));
	sgInited = true;
	sgLock.unlock();
	return true;
}


bool
AosConsoleProc::registerProc()
{
	// When this function is called, 'sgLock' should have been 
	// locked!!!!!!!!!!
	aos_assert_r(!sgInited, false);
	mapitr_t itr = smProcs.find(mInfoId);
	aos_assert_r(itr == smProcs.end(), false);
	smProcs[mInfoId] = this;
	return true;
}
#endif
