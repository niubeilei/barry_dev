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
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeLogClient/Tester/LogReqProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

#include <map>
using namespace std;

static map<AosLogReqid::E, AosLogReqProcPtr>	sgLogReqProcs;
static OmnMutex				sgLock;

AosLogReqProc::AosLogReqProc()
{

}


AosLogReqProc::AosLogReqProc(
		const OmnString &name, 
		const AosLogReqid::E id, 
		const bool regflag)
:
mId(id),
mReqidName(name)
{
	if (regflag)
	{
		OmnString errmsg;
		AosLogReqProcPtr thisptr(this, false);
		if (!registerLogProc(thisptr, errmsg))
		{
			OmnExcept e(__FILE__, __LINE__, errmsg);
			throw e;
		}
	}
}


bool
AosLogReqProc::registerLogProc(
		const AosLogReqProcPtr &proc, 
		OmnString &errmsg)
{
	sgLock.lock();
	if (!AosLogReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		errmsg = "Incorrect reqid: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgLogReqProcs.find(proc->mId) != sgLogReqProcs.end())
	{
		sgLock.unlock();
		errmsg = "LogProc already registered: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLogReqProcs.insert(pair<AosLogReqid::E,AosLogReqProcPtr>(proc->mId, proc));
	bool rslt = AosLogReqid::addName(proc->mReqidName, proc->mId, errmsg);
	sgLock.unlock();
	return rslt;
}


AosLogReqProcPtr 
AosLogReqProc::getProc(const AosLogReqid::E &id)
{
	sgLock.lock();
	if (!AosLogReqid::isValid(id)) 
	{
		sgLock.unlock();
		return 0;
	}

	AosLogReqProcPtr proc = sgLogReqProcs[id];
	sgLock.unlock();
	return proc;
}


int
AosLogReqProc::getProcSize()
{
	return sgLogReqProcs.size(); 
}
