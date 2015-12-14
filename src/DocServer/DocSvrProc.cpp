////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "DocServer/DocSvrProc.h"

#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


extern int gAosLogLevel;
extern AosDocSvrProcPtr     sgProcs[AosDocSvrProcId::eMax];
static OmnMutex				sgLock;


AosDocSvrProc::AosDocSvrProc(
		const OmnString &name, 
		const AosDocSvrProcId::E type, 
		const bool regflag)
:
mName(name),
mType(type)
{
	if (regflag) 
	{
		AosDocSvrProcPtr thisptr(this, false);
		if (!registerProc(thisptr))
		{
			OmnExcept e(__FILE__, __LINE__, "Failed registering DocServerProc");
			throw e;
		}
	}
}


AosDocSvrProc::~AosDocSvrProc()
{
}


bool
AosDocSvrProc::registerProc(const AosDocSvrProcPtr &proc)
{
	sgLock.lock();
	if (!AosDocSvrProcId::isValid(proc->mType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect DocServerProc id: " << proc->mType << enderr;
		return false;
	}

	if (sgProcs[proc->mType])
	{
		sgLock.unlock();
		OmnAlarm << "Smart doc already registered: " << proc->mType << enderr;
		return false;
	}
	sgProcs[proc->mType] = proc;
	if (gAosLogLevel >= 2)	
	{
		OmnScreen << "Start Proc:" << proc->mName << ":" <<  proc->mType << endl;
	}
	bool rslt = AosDocSvrProcId::addName(proc->mName, proc->mType);
	sgLock.unlock();
	return rslt;
}


AosDocSvrProcPtr
AosDocSvrProc::getProc(const OmnString &id)
{
	sgLock.lock();
	AosDocSvrProcId::E type = AosDocSvrProcId::toEnum(id);
	if (gAosLogLevel >= 3)
	{
		OmnScreen  << "Operation: " << id << endl;
	}
	if (!AosDocSvrProcId::isValid(type))
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized DocServer Proc id: " << id << enderr;
		return 0;
	}
	
	AosDocSvrProcPtr proc = sgProcs[type];
	sgLock.unlock();
	aos_assert_r(proc, 0);
	return proc;
}


