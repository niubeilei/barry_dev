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
// A JimoCall defines a Jimo Functoin Call. There are following types
// of Jimo Calls:
// 	1. Reading Sync Calls
// 	2. Reading Async Calls
// 	3. Writing Sync Calls
// 	4. Writing Async Calls
// 	5. Normal Sync Calls
// 	6. Normal Async Calls
//   
//
// Modification History:
// 2015/03/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoCall/JimoCallAsyncNorm.h"

#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"


AosJimoCallAsyncNorm::AosJimoCallAsyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const AosJimoCallerPtr &caller,
		AosClusterObj *cluster)
:
AosJimoCall(rdata, package, func, cluster),
mTimerMs(eDftTimerMs),
mCaller(caller),
mCallTime(0),
mEndpointID(-1),
mLock(OmnNew OmnMutex())
{
}


AosJimoCallAsyncNorm::AosJimoCallAsyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const AosJimoCallerPtr &caller,
		AosClusterObj *cluster,
		const int endpoint_id)
:
AosJimoCall(rdata, package, func, cluster),
mTimerMs(eDftTimerMs),
mCaller(caller),
mCallTime(0),
mEndpointID(endpoint_id),
mLock(OmnNew OmnMutex())
{
}


AosJimoCallAsyncNorm::~AosJimoCallAsyncNorm()
{
	OmnDelete mLock;
}


bool 
AosJimoCallAsyncNorm::makeCall(AosRundata *rdata) 
{
	// It retrieves all the endpoints for this jimocall. 
	mStartTime = OmnTime::getTimestamp(); 
	aos_assert_rr(mCluster, rdata, false);
	bool rslt = mCluster->getTargetCubes(rdata, mEndpointIDs);
	aos_assert_rr(rslt, rdata, false);

	mJimoCallID = mCluster->nextJimoCallID(rdata);
	aos_assert_rr(mJimoCallID > 0, rdata, false);

	AosBuff *buff = mBSON.getBuffRaw();
	aos_assert_r(buff, false);

	rdata->serializeToBuffForJimoCall(buff);

	buff->appendU64((u64)OmnMsgId::eJimoCallReq);
	buff->appendU64(mJimoCallID);
	buff->appendOmnStr(mPackage);
	buff->appendInt(mFunc);

	AosJimoCallPtr thisptr(this, false);
	mCrtEndpointIdx = 0;
	
	if (mEndpointID >= 0)
	{
		buff->appendInt(mEndpointID);
		rslt = mCluster->makeJimoCall(rdata, mEndpointID, thisptr, buff);
		aos_assert_rr(rslt, rdata, false);

		mCluster->eraseJimoCall(rdata, mJimoCallID);
		logCall(rdata);
		return true;
	}

	// This is an async call. It initiates the call
	// and returns immediately.
	while (mCrtEndpointIdx < mEndpointIDs.size())
	{
		OmnScreen << "jimocall id:" << mJimoCallID << " is sending msg to No. " << mCrtEndpointIdx << " endpoint." << endl;
		//rslt = mCluster->makeJimoCall(rdata, mEndpointIDs[mCrtEndpointIdx], thisptr, buff);
		rslt = mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff);
		if (rslt) return true;
		
		logFailedTry(rdata);
		mCrtEndpointIdx++;
	}

	AosLogError(rdata, false, "failed_making_call") << enderr;
	logFailedCall(rdata);
	mCluster->eraseJimoCall(rdata, mJimoCallID);

	return false;
}


bool 
AosJimoCallAsyncNorm::callResponded(
		AosRundata *rdata,
		AosJimoCall &jimo_call)
{
	OmnScreen << "Call responded: " << jimo_call.getJimoCallID() << endl;
	aos_assert_r(mJimoCallID == jimo_call.getJimoCallID(), false);

	mStatus = jimo_call.getStatus();
	mBSON.swap(jimo_call.getBSON());
	mConnBuff = jimo_call.getConnBuff();

	// This is a reading async call. Check whether the call is successful.
	// If yes, call back.
	if (isCallSuccess())
	{
		logCall(rdata);
		mCaller->callFinished(rdata, *this);
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		return true;
	}

	if (isLogicalFail())
	{
		logFailedCall(rdata);
		mCaller->callFinished(rdata, *this);
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		return true;
	}

	// The call failed. Try the next one.
	AosJimoCallPtr thisptr(this, false);
	AosBuff *buff = mBSON.getBuffRaw();
	while (++mCrtEndpointIdx < mEndpointIDs.size())
	{
		OmnScreen << "jimocall id:" << mJimoCallID << " failed, resend msg to No. " << mCrtEndpointIdx << " endpoint." << endl;
		mCallTime = OmnGetTimestamp();
		//if (mCluster->makeJimoCall(rdata, mEndpointIDs[mCrtEndpointIdx], thisptr, buff)) return true;
		if (mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff)) return true;
	}

	// It has tried all the allowed endpoints. 
	logFailedCall(rdata);
	mStatus = ePhysicalFail;
	mCaller->callFinished(rdata, *this);
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	return true;
}


void
AosJimoCallAsyncNorm::checkTimer(AosRundata *rdata)
{
	if (OmnGetTimestamp() - mCallTime < mTimerMs) return;

	// The write timer expireed. 
	logTimeoutCall(rdata);
	mStatus = eCallTimeout;
	mCaller->callFinished(rdata, *this);
	mCluster->eraseJimoCall(rdata, mJimoCallID);
}


void
AosJimoCallAsyncNorm::logTimeoutCall(AosRundata *rdata)
{
	OmnNotImplementedYet;
	return;
}


bool
AosJimoCallAsyncNorm::waitForFinish()
{
	/*
	mLock->lock();
	if (mCallResponded)
	{
		mLock->unlock();
		return true;
	}

	AosJimoCallWaiter::setWait(*this);
	mLock->unlock();
	AosJimoCallWaiter::wait(*this, 60);
	*/
	return true;
}

