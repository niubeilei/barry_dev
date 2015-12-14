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
// 07/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ThreadShellRunners/ActionsRunner.h"

#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "Thread/ThrdShellProc.h"
#include "ThreadShellRunners/ActionRunnerListener.h"



AosActionsRunner::AosActionsRunner(
		const int64_t &startPos,
		const int64_t &endPos,
		const AosDataBlobObjPtr &blob,
		const AosDataCacherObjPtr &cacher,
		const AosBuffArrayPtr &buffArray,
		const ProcDataType &type,
		const vector<AosActionObjPtr> &actions,
		const OmnThrdShellProcPtr &caller,
		const AosValueRslt &userData,
		const AosDataRecordObjPtr &record,
		const OmnString &calledClassName,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc(calledClassName),
mDataType(type),
mStartPos(startPos),
mEndPos(endPos),
mBlob(blob),
mCacher(cacher),
mBuffArray(buffArray),
mActions(actions),
mCaller(caller),
mUserData(userData),
mDataRecord(record),
mRundata(rdata)
{
	if (mStartPos < 0)
	{
		OmnThrowException("internal error");
		return;
	}
	
	if (!checkData())
	{
		OmnThrowException("Internal error");
		return;
	}
}


AosActionsRunner::~AosActionsRunner()
{
}



bool 
AosActionsRunner::checkData()
{
	if (mDataType <= eInvalid || mDataType >= eMax) return false;
	
	switch(mDataType)
	{
	case eDataBlob:
		return (!mBlob ||(mBlob->size() > 0));
	
	case eDataCacher:
		return (!mCacher || (mCacher->size() > 0));
	
	case eBuffArray:
		return (!mBuffArray || (mBuffArray->getNumEntries() > 0));
	
	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool 
AosActionsRunner::run()
{
	// This function runs all the actions in 'mActions' in groups 
	// against the input. Actions in the same group are run in parallel, 
	// and actions in different groups are run in sequential. 
	// This is handled by thread shells.
	if (mStartPos < 0)
	{
		AosSetErrorU(mRundata, "internal_error") << enderr;
		return false;
	}

	if (mActions.size() <= 0) return true;
	
	switch (mDataType)
	{
	case eDataBlob:
		return runActions(mBlob, mRundata);

	case eDataCacher:
		return runActions(mCacher, mRundata);
	
	case eBuffArray:
		return runActions(mBuffArray, mRundata);
	
	default:
		OmnAlarm << "Undefined type" << enderr;
		return false;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosActionsRunner::procFinished()
{
	aos_assert_r(mCaller, false);
	return mCaller->procFinished();
}


bool
AosActionsRunner::runActions(
		const AosDataBlobObjPtr &blob,
		const AosRundataPtr &rdata)
{
	aos_assert_r(blob, false);
	if (blob->size() <= 0) return true;

	/*
	mDataRecord->reset(mRundata);
	if (!blob->firstRecordInRange(mStartPos, mDataRecord)) return true;
	aos_assert_r(runActions(mDataRecord, mRundata), false);

	mDataRecord->reset(mRundata);
	while (blob->nextRecordInRange(mDataRecord))
	{
		aos_assert_r(runActions(mDataRecord, mRundata), false);
		mDataRecord->reset(mRundata);
	}
*/
	return true;
}


bool
AosActionsRunner::runActions(
		const AosDataCacherObjPtr &cacher,
		const AosRundataPtr &rdata)
{
	aos_assert_r(cacher, false);
	if (cacher->size() <= 0) return true;
/*	
	mDataRecord->reset(mRundata);
	if (!cacher->firstRecordInRange(mStartPos, mDataRecord)) return true;
	aos_assert_r(runActions(mDataRecord, mRundata), false);

	mDataRecord->reset(mRundata);
	while (cacher->nextRecordInRange(mDataRecord))
	{
		aos_assert_r(runActions(mDataRecord, mRundata), false);
		mDataRecord->reset(mRundata);
	}
*/
	return true;
}


bool
AosActionsRunner::runActions(
		const AosBuffArrayPtr &array,
		const AosRundataPtr &rdata)
{
	aos_assert_r(array, false);
	if (array->getNumEntries() <= 0) return true;
/*	
	array->resetRecordLoop();
	mDataRecord->reset(mRundata);
	while (array->nextValue(mDataRecord))
	{
		aos_assert_r(runActions(mDataRecord, mRundata), false);
		mDataRecord->reset(mRundata);
	}
*/
	return true;
}


bool
AosActionsRunner::runActions(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	if (mActions.size() <= 0) return true;

	for (u32 i=0; i<mActions.size(); i++)
	{
		aos_assert_r(mActions[i]->run(record, rdata), false);
	}

	return true;
}
