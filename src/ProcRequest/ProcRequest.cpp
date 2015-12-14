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
// 2011/07/28	Created by Jackie zhao
////////////////////////////////////////////////////////////////////////////
#include "ProcRequest/ProcRequest.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosProcRequestPtr	sgProcRequests[AosProcReqid::eMax];
static OmnMutex				sgLock;

AosProcRequest::AosProcRequest(
		const OmnString &name, 
		const AosProcReqid::E id, 
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
		AosProcRequestPtr thisptr(this, false);
		registerProcRequest(thisptr);
	}
}


bool
AosProcRequest::registerProcRequest(const AosProcRequestPtr &proc)
{
	sgLock.lock();
	if (!AosProcReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mId << enderr;
		return false;
	}

	if (sgProcRequests[proc->mId])
	{
		sgLock.unlock();
		OmnAlarm << "SeProc already registered: " << proc->mId << enderr;
		return false;
	}

	sgProcRequests[proc->mId] = proc;
	bool rslt = AosProcReqid::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosProcRequestPtr 
AosProcRequest::getProc(const OmnString &idstr)
{
	sgLock.lock();
	AosProcReqid::E id = AosProcReqid::toEnum(idstr);
	if (!AosProcReqid::isValid(id)) 
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized SerequestID: " << idstr << enderr;
		return 0;
	}

	AosProcRequestPtr proc = sgProcRequests[id];
	sgLock.unlock();
	return proc;
}

