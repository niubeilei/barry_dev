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
// 05/26/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeReqProc/SqlProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosSqlProcPtr	sgSqlProcs[AosSqlReqid::eMax];
static OmnMutex			sgLock;

AosSqlProc::AosSqlProc(
		const OmnString &name, 
		const AosSqlReqid::E id, 
		const bool regflag)
:
mId(id),
mReqidName(name)
{
	if (name == "")
	{
		OmnAlarm << "Missing request name: " << id << enderr;
	}

	if (regflag)
	{
		AosSqlProcPtr thisptr(this, false);
		registerSeProc(thisptr);
	}
}


bool
AosSqlProc::registerSeProc(const AosSqlProcPtr &proc)
{
	sgLock.lock();
	if (!AosSeReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mId << enderr;
		return false;
	}

	if (sgSqlProcs[proc->mId])
	{
		sgLock.unlock();
		OmnAlarm << "SqlProc already registered: " << proc->mId << enderr;
		return false;
	}

	sgSqlProcs[proc->mId] = proc;
	bool rslt = AosSeReqid::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosSqlProcPtr 
AosSqlProc::getProc(const OmnString &idstr)
{
	AosSeReqid::E id = AosSqlReqid::toEnum(idstr);
	if (!AosSeReqid::isValid(id)) return 0;
	return sgSqlProcs[id];
}

