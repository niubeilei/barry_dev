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
#include "ShortMsgSvr/ShmReqProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "XmlUtil/XmlTag.h"

extern AosShmReqProcPtr		sgShmReqProcs[AosShmReqid::eMax];
static OmnMutex             sgLock; 


AosShmReqProc::AosShmReqProc(
		const OmnString &name, 
		const AosShmReqid::E id, 
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
		AosShmReqProcPtr thisptr(this, false);
		registerSeProc(thisptr);
	}
}


bool
AosShmReqProc::registerSeProc(const AosShmReqProcPtr &proc)
{
	sgLock.lock();
	if (!AosShmReqid::isValid(proc->mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect reqid: " << proc->mId << enderr;
		return false;
	}

	if (sgShmReqProcs[proc->mId])
	{
		sgLock.unlock();
		OmnAlarm << "SeProc already registered: " << proc->mId << enderr;
		return false;
	}

	sgShmReqProcs[proc->mId] = proc;
	bool rslt = AosShmReqid::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosShmReqProcPtr 
AosShmReqProc::getProc(const OmnString &idstr)
{
	AosShmReqid::E id = AosShmReqid::toEnum(idstr);
	if (!AosShmReqid::isValid(id)) return 0;
	return sgShmReqProcs[id];
}



void
AosShmReqProc::createLog(
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
