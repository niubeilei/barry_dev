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
#include "SeReqProc/SeRequestProc.h"

#include "Thread/Mutex.h"
#include "SEServer/SeReqProc.h"
#include "Util/HashUtil.h"

extern AosSeRequestProcPtr	sgSeRequestProcs[AosSeReqid::eMax];
static OmnMutex				sgLock;

AosSeRequestProc::AosSeRequestProc(
		const OmnString &name, 
		const AosSeReqid::E id, 
		const bool regflag)
:
mId(id),
mReqidName(name)
{
	if (regflag)
	{
		OmnString errmsg;
		AosSeRequestProcPtr thisptr(this, false);
		if (!registerSeProc(thisptr, errmsg))
		{
			OmnExcept e(__FILE__, __LINE__, errmsg);
			throw e;
		}
	}
}


AosSeRequestProc::~AosSeRequestProc()
{

}


bool
AosSeRequestProc::registerSeProc(
		const AosSeRequestProcPtr &proc, 
		OmnString &errmsg)
{
	sgLock.lock();
	if (!AosSeReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		errmsg = "Incorrect reqid: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgSeRequestProcs[proc->mId])
	{
		sgLock.unlock();
		errmsg = "SeProc already registered: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgSeRequestProcs[proc->mId] = proc;
	bool rslt = AosSeReqid::addName(proc->mReqidName, proc->mId, errmsg);
	sgLock.unlock();
	return rslt;
}


AosSeRequestProcPtr 
AosSeRequestProc::getProc(const OmnString &idstr)
{
	sgLock.lock();
	AosSeReqid::E id = AosSeReqid::toEnum(idstr);
	if (!AosSeReqid::isValid(id)) 
	{
		sgLock.unlock();
		// OmnAlarm << "Unrecognized SerequestID: " << idstr << enderr;
		return 0;
	}

	AosSeRequestProcPtr proc = sgSeRequestProcs[id];
	sgLock.unlock();
	return proc;
}


AosSeRequestProcPtr
AosSeRequestProc::clone(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return 0;
}


bool
AosSeRequestProc::requestFinished()
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosSeRequestProc::requestFailed()
{
	OmnShouldNeverComeHere;
	return false;
}


void 
AosSeRequestProc::setSeReqProc(const AosSeReqProcPtr &p)
{
	mSeReqProc = p;
}
