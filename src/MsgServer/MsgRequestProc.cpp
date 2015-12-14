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
// 06/15/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/MsgRequestProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosMsgRequestProcPtr			sgMsgRequestProcs[AosMsgReqid::eMax];
static OmnMutex						sgLock;

AosMsgRequestProc::AosMsgRequestProc(
		const OmnString &name, 
		const AosMsgReqid::E id, 
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
		AosMsgRequestProcPtr thisptr(this, false);
		registerMsgProc(thisptr);
	}
}


bool
AosMsgRequestProc::registerMsgProc(const AosMsgRequestProcPtr &proc)
{
	sgLock.lock();
	if (!AosMsgReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mId << enderr;
		return false;
	}

	if (sgMsgRequestProcs[proc->mId])
	{
		sgLock.unlock();
		OmnAlarm << "SeProc already registered: " << proc->mId << enderr;
		return false;
	}

	sgMsgRequestProcs[proc->mId] = proc;
	bool rslt = AosMsgReqid::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosMsgRequestProcPtr 
AosMsgRequestProc::getProc(const OmnString &idstr)
{
	AosMsgReqid::E id = AosMsgReqid::toEnum(idstr);
	if (!AosMsgReqid::isValid(id)) return 0;
	return sgMsgRequestProcs[id];
}
