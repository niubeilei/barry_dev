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
#include "SQLServer/SqlProc.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include "SqlUtil/SqlReqid.h"
#include "SQLServer/ImportData.h"
#include "SQLServer/ExportData.h"

#include "Porting/Sleep.h"
#include "TransServer/TransServer.h"

static AosSqlProcPtr	sgSqlProcs[AosSqlReqid::eMax];
static OmnMutex			sgLock;

static  AosImportData		sgImportData(true);
static  AosExportData		sgExportData(true);

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
		OmnString errmsg = "Missing request name: ";
		errmsg << id;
		OmnExcept e(__FILE__, __LINE__, errmsg);
		throw e;
	}

	if (regflag)
	{
		AosSqlProcPtr thisptr(this, false);
		registerSeProc(thisptr);
	}
}


AosSqlProc::~AosSqlProc()
{
}


bool
AosSqlProc::registerSeProc(const AosSqlProcPtr &proc)
{
	sgLock.lock();
	if (!AosSqlReqid::isValid(proc->mId))
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
	bool rslt = AosSqlReqid::addName(proc->mReqidName, proc->mId);
	sgLock.unlock();
	return rslt;
}


AosSqlProcPtr 
AosSqlProc::getProc(const OmnString &idstr)
{
	sgLock.lock();
	AosSqlReqid::E id = AosSqlReqid::toEnum(idstr);
	if (!AosSqlReqid::isValid(id)) 
	{
		sgLock.unlock();
		return 0;
	}
	AosSqlProcPtr proc = sgSqlProcs[id];
	sgLock.unlock();
	return proc;
}

