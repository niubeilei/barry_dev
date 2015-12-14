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
// Modification History:
// 05/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActDeleteIIL.h"

#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"


AosActDeleteIIL::AosActDeleteIIL(const bool flag)
:
AosSdocAction(AOSACTTYPE_DELETEIIL, AosActionType::eDeleteIIL, flag)
{
}


AosActDeleteIIL::~AosActDeleteIIL()
{
}


AosActionObjPtr
AosActDeleteIIL::clone(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosActDeleteIIL(false);
	}

	catch (const OmnExcept &e)
	{
		AosSetError(rdata, "failed_clone_object") << e.getErrmsg() << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


bool
AosActDeleteIIL::delThrd::run()
{
	return AosDeleteIIL(mIILName, mTrueDelete, mRundata);
}

	
bool
AosActDeleteIIL::delThrd::procFinished()
{
	return true;
}


bool 
AosActDeleteIIL::run(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, false);
	
	OmnString iilname = sdoc->getAttrStr(AOSTAG_IILNAME);
	if (iilname == "")
	{
		AosSetErrorU(rdata, "missing_iilname:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool true_delete = sdoc->getAttrBool(AOSTAG_TREUDELETE, false);

	OmnString mode = sdoc->getAttrStr(AOSTAG_RUNNING_MODE);
	if (mode == "thread")
	{
		OmnThrdShellProcPtr runner = OmnNew delThrd(iilname, true_delete, rdata);
		return addThreadShellProc(runner, rdata);
	}

	// It is not the thread mode. Just run it.
	return AosDeleteIIL(iilname, true_delete, rdata);
}

	
bool
AosActDeleteIIL::run(
		const AosTaskObjPtr &task, 
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task, false);
	mTask = task;
	run(sdoc, rdata);
	AosActionObjPtr thisptr(this, true);
	mTask->actionFinished(thisptr, rdata);
	mTask = 0;
	return true;
}


bool
AosActDeleteIIL::checkConfig(
		const AosXmlTagPtr &def,
		const AosTaskObjPtr &task,
		const AosRundataPtr &rdata)
{
	aos_assert_r(def, false);
	OmnString iilname = def->getAttrStr(AOSTAG_IILNAME);
	if (iilname == "")
	{
		AosSetErrorU(rdata, "missing_iilname:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	return true;
}

