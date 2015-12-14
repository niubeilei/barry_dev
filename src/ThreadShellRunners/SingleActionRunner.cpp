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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
/*
#include "ThreadShellRunners/SingleActionRunner.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSourceObj.h"
#include "SEInterfaces/DataBlobObj.h"
#include "ThreadShellRunners/Ptrs.h"
#include "Util/File.h"


AosSingleActionRunner::AosSingleActionRunner(
		const AosDataBlobObjPtr &blob,
		const AosActionObjPtr &action,
		const AosValueRslt &user_data, 
		const int64_t max_errors,
		const AosActionRunnerListener &caller,
		const AosRundataPtr &rdata)
:
mBlob(blob),
mAction(action),
mRundata(rdata),
mUserData(user_data),
mMaxErrors(max_errors),
mCaller(caller),
mStatus(false)
{
	if (!mBlob)
	{
		OmnThrowException("blob is null");
		return;
	}

	if (!mAction)
	{
		OmnThrowException("action is null");
		return;
	}

	OmnThrdShellProcPtr thisptr(this, false);
	OmnThreadShellMgr::getSelf()->proc(thisptr);
}


bool
AosSingleActionRunner::run()
{
	// This function distributes 'mBlob' into the buckets based on 
	// the distribution map. 
	aos_assert_r(mRundata, false);
	if (mBlob) return runBlobAction(mRundata);
	if (mBuffArray) return runBuffArrayAction(mRundata);

	AosSetErroru(mRundata, "internal_error") << enderr;
	if (mCaller) mCaller->actionFinished(false, mUserData, mRundata);
	return false;
}


bool
AosSingleActionRunner::runBlobAction(const AosRundataPtr &rdata)
{
	switch (mRunMode)
	{
	case AosActionObj::eRunOnValue:
		 return runOnValue(rdata);

	case AosActionObj::eRunOnRecord:
	case AosActionObj::eRunOnBlob:

	default:
		 AosSetErrorU(rdata, "invalid_run_mode") << enderr;
		 mStatus = false;
		 return false;
	}

	return true;
}


bool
AosSingleActionRunner::runOnValue(const AosRundataPtr &rdata)
{
	aos_assert_rr(mBlob, rdata, false);
	aos_assert_rr(mAction, rdata, false);

	mBlob->reset();
	AosValueRslt value;
	mNumErrors = 0;
	while (mBlob->nextValue(value, false))
	{
		if (!mAction->run(value, rdata))
		{
			mNumErrors++;
			if (mMaxErrors > 0 && mNumErrors > mMaxErrors)
			{
				mStatus = false;
				return false;
			}
		}
	}

	mStatus = true;
	return true;
}


bool
AosSingleActionRunner::runOnRecord(const AosRundataPtr &rdata)
{
	aos_assert_rr(mBlob, rdata, false);
	aos_assert_rr(mAction, rdata, false);

	mBlob->reset();
	AosDataRecordPtr record = mBlob->cloneDataRecord();
	if (!record)
	{
		AosSetErrorU(rdata, "failed_clone_data_record") << enderr;
		mStatus = false;
		return false;
	}

	mNumErrors = 0;
	while (mBlob->nextRecord(record, false))
	{
		if (!mAction->run(record, rdata))
		{
			mNumErrors++;
			if (mNumErrors > mMaxErrors)
			{
				mStatus = false;
				return false;
			}
		}
	}

	mStatus = true;
	return true;
}


bool		
SingleActionRunner::procFinished()
{
	if (mCaller)
	{
		mCaller->actionFinished(mStatus, mUserData, mRundata);
	}
	return true;
}
*/
