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
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterServer/StatProc.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterServer/StatAddCounter.h"
#include "CounterServer/StatAddCounter2.h"
#include "CounterServer/StatSetCounter.h"
#include "CounterUtil/CounterOperations.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util/StrU64Array.h"
#include "Util/U64U64Array.h"
#include "UtilTime/TimeInfo.h"


static AosStatProcPtr	sgStatProcs[AosStatType::eMax];
static OmnMutex			sgLock;

bool AosStatProc::smShowLog = false;


AosStatProc::AosStatProc(
		const u64 &iilid, 
		const AosCounterOperation::E &type)
:
mType(type),
mIILID(iilid),
mMaxDocs(eDftMaxDocs),
mLock(OmnNew OmnMutex())
{
}


AosStatProcPtr
AosStatProc::createStatProc(
		const u64 &iilid, 
		const AosOrder::E order,
		const AosCounterOperation::E &operation,
		const AosRundataPtr &rdata)
{
	switch (operation)
	{
		/*
		case AosCounterOperation::eAddCounter:
			return OmnNew AosStatAddCounter(iilid, order, rdata);

		case AosCounterOperation::eAddCounter2:
			    return OmnNew AosStatAddCounter2(iilid, order, rdata);
				*/

		case AosCounterOperation::eSetCounter:
			return OmnNew AosStatSetCounter(iilid, rdata);

		default:
			break;
	}

	AosSetErrorU(rdata, "unrecognized_counter_operation") << ": " << operation;
	OmnAlarm << rdata->getErrmsg() << enderr;
	return 0;
}

#if 0
AosStatProc::AosStatProc(const AosStatType::E id, const bool regflag)
:
{
	sgLock.lock();
	sgNumObjs++;
	if (sgNumObjs % 10000 == 0) 
	{
		OmnScreen << "StatProc created: " << sgNumObjs << endl;
	}
	sgLock.unlock();

	if (regflag)
	{
		OmnString errmsg;
		AosStatProcPtr thisptr(this, false);
		if (!registerProc(thisptr, errmsg))
		{
			OmnExcept e(__FILE__, __LINE__, errmsg);
			throw e;
		}
	}
}


bool
AosStatProc::registerProc(
		const AosStatProcPtr &proc, 
		OmnString &errmsg)
{
	sgLock.lock();
	if (!AosStatType::isValid(proc->mId))
	{
		sgLock.unlock();
		errmsg = "Incorrect reqid: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgStatProcs[proc->mId])
	{
		sgLock.unlock();
		errmsg = "Proc already registered: ";
		errmsg << proc->mId;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgStatProcs[proc->mId] = proc;
	sgLock.unlock();
	return true;
}


AosStatProcPtr 
AosStatProc::getProc(const OmnString &idstr)
{
	sgLock.lock();
	AosStatType::E id = AosStatType::toEnum(idstr);
	if (!AosStatType::isValid(id)) 
	{
		sgLock.unlock();
		return 0;
	}

	AosStatProcPtr proc = sgStatProcs[id];
	sgLock.unlock();
	return proc;
}
#endif

