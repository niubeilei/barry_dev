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
#include "JimoCall/JimoCallSyncNorm.h"

#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"


AosJimoCallSyncNorm::AosJimoCallSyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		AosClusterObj *cluster)
:
AosJimoCall(rdata, package, func, cluster),
mTimerMs(eDftTimerMs),
mCallTime(0),
mEndpointID(-1)
{
}


AosJimoCallSyncNorm::AosJimoCallSyncNorm(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		AosClusterObj *cluster,
		const int endpoint_id)
:
AosJimoCall(rdata, package, func, cluster),
mTimerMs(eDftTimerMs),
mCallTime(0),
mEndpointID(endpoint_id)
{
}


bool 
AosJimoCallSyncNorm::makeCall(AosRundata *rdata) 
{
	mStartTime = OmnTime::getTimestamp(); 
	aos_assert_rr(mCluster, rdata, false);

	mJimoCallID = mCluster->nextJimoCallID(rdata);
	aos_assert_rr(mJimoCallID > 0, rdata, false);

	i64 dupid_pos = -1;
	AosBuff *buff = composeBuff(rdata, dupid_pos);
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(dupid_pos > 0, rdata, false);

	AosJimoCallWaiter::setWait(*this);

	AosJimoCallPtr thisptr(this, false);

	char *data = buff->data();
	mCrtEndpointIdx = 0;

	if (mEndpointID >= 0)
	{
		data[dupid_pos] = 0;
		bool rslt = mCluster->makeJimoCall(rdata, mEndpointID, thisptr, buff);
		aos_assert_rr(rslt, rdata, false);
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		logCall(rdata);
		return true;
	}
	
	bool rslt = mCluster->getTargetCubes(rdata, mEndpointIDs);
	aos_assert_rr(rslt, rdata, false);

	while (mCrtEndpointIdx < mEndpointIDs.size())
	{
		data[dupid_pos] = mEndpointIDs[mCrtEndpointIdx].dupid;
		// rslt = mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff);
		bool rslt = mCluster->makeJimoCall(rdata, mEndpointIDs[mCrtEndpointIdx].cube_id, thisptr, buff);
		aos_assert_rr(rslt, rdata, false);

		AosJimoCallWaiter::wait(*this, 1);

		// The call woke up. It wakes up when:
		// 	1. Time out
		// 	2. Call responded
		if (isCallSuccess())
		{
			mCluster->eraseJimoCall(rdata, mJimoCallID);
			logCall(rdata);
			return true;
		}

		logFailedTry(rdata);
		mCrtEndpointIdx++;
	}

	// All have tried but failed.
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	AosLogError(rdata, false, "failed_making_call") << enderr;
	logFailedCall(rdata);
	return false;
}


bool 
AosJimoCallSyncNorm::callResponded(
		AosRundata *rdata,
		AosJimoCall &jimo_call)
{
	OmnScreen << "Call responded: " << jimo_call.getJimoCallID() << endl;
	aos_assert_r(mJimoCallID == jimo_call.getJimoCallID(), false);

	mStatus = jimo_call.getStatus();
	mBSON.swap(jimo_call.getBSON());
	mConnBuff = jimo_call.getConnBuff();

	// This is a reading sync call. Trigger the semaphore.
	aos_assert_r(mSem, false);
	mSem->post();
	return true;
}


void
AosJimoCallSyncNorm::checkTimer(AosRundata *rdata)
{
	if (OmnGetTimestamp() - mStartTime < mTimerMs) return;

	// It is a reading sync call. 
	aos_assert(mSem);
	mSem->post();
	return;
}

