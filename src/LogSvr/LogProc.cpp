////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogSvr/LogProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t		sgReqidMap;
static AosLogProcPtr	sgLogProcs[AosLogProcId::eMax];
static OmnMutex			sgLock;

AosLogProc::AosLogProc(
		const OmnString &name, 
		const AosLogProcId::E id, 
		const bool regflag)
:
mProcId(id),
mProcName(name)
{
	if (name == "")
	{
		OmnAlarm << "Missing request name: " << id << enderr;
	}

	if (regflag)
	{
		AosLogProcPtr thisptr(this, false);
		registerProc(thisptr);
	}
}


bool
AosLogProc::registerProc(const AosLogProcPtr &proc)
{
	sgLock.lock();
	if (!AosLogProcId::isValid(proc->mProcId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mProcId << enderr;
		return false;
	}

	if (sgLogProcs[proc->mProcId])
	{
		sgLock.unlock();
		OmnAlarm << "SeProc already registered: " << proc->mProcId << enderr;
		return false;
	}

	sgLogProcs[proc->mProcId] = proc;
	bool rslt = AosLogProcId::addName(proc->mProcName, proc->mProcId);
	sgLock.unlock();
	return rslt;
}


AosLogProcPtr 
AosLogProc::getProc(const OmnString &idstr)
{
	AosLogProcId::E id = AosLogProcId::toEnum(idstr);
	if (!AosLogProcId::isValid(id)) return 0;
	return sgLogProcs[id];
}

