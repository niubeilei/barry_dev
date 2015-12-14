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
// 06/11/2011	Created by Brian 
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgSvr/ShmHandler.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosShmHandlerPtr		sgShmHandlers[AosShmReqid::eMax];
static OmnMutex             sgLock; 


AosShmHandler::AosShmHandler(
		const OmnString &name, 
		const AosShmHandlerId::E &id, 
		const bool flag)
:
mId(id),
mReqidName(name)
{
	if (name == "")
	{
		OmnAlarm << "Missing request name: " << id << enderr;
	}

	if (flag)
	{
		AosShmHandlerPtr thisptr(this, false);
		registerSeProc(thisptr);
	}
}


AosShmHandler::~AosShmHandler()
{

}


bool
AosShmHandler::registerSeProc(const AosShmHandlerPtr &proc)
{
	sgLock.lock();
	if (!AosShmHandlerId::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mId << enderr;
		return false;
	}

	if (sgShmHandlers[proc->mId])
	{
		sgLock.unlock();
		OmnAlarm << "SeProc already registered: " << proc->mId << enderr;
		return false;
	}

	sgShmHandlers[proc->mId] = proc;
	bool rslt = AosShmHandlerId::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosShmHandlerPtr 
AosShmHandler::getProc(const OmnString &idstr)
{
	AosShmHandlerId::E id = AosShmHandlerId::toEnum(idstr);
	if (!AosShmHandlerId::isValid(id)) return 0;
	return sgShmHandlers[id];
}



void
AosShmHandler::createLog(
		const OmnString &receiver, 
		const OmnString &msg, 
		const AosRundataPtr &rdata)
{
	OmnString status = (rdata->isOk()) ? "success" : "failed";
	OmnString docstr = "<log zky_otype=\"";
	docstr << AOSOTYPE_LOG << "\" zky_stype=\""
		<< AOSSTYPE_SENDSHORTMSG_LOG << "\" status=\""
		<< status << "\">"
		<< "<receiver><![CDATA[" << receiver << "]]></receiver>"
		<< "<msg><![CDATA[" << msg << "]]></msg>";
	if (!rdata->isOk())
	{
		docstr << "<errmsg><![CDATA[" << rdata->getErrmsg() << "]]></errmsg>";
	}
	docstr << "</log>";

	OmnNotImplementedYet;
}
