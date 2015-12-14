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
#include "JimoCall/JimoCallSyncWrite.h"

#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"


AosJimoCallSyncWrite::AosJimoCallSyncWrite(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const u64 dist_id, 
		const u32 rep_policy,
		AosClusterObj *cluster)
:
AosJimoCall(rdata, package, func, cluster),
mLock(OmnNew OmnMutex()),
mNumResponses(0),
mNumPositiveResponses(0),
mCallResponded(false)
{
	mDistId = dist_id;
	mReplicPolicy = rep_policy;
}


AosJimoCallSyncWrite::AosJimoCallSyncWrite(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const OmnString &dist_str, 
		const u32 rep_policy,
		AosClusterObj *cluster)
:
AosJimoCall(rdata, package, func, cluster),
mLock(OmnNew OmnMutex()),
mNumResponses(0),
mNumPositiveResponses(0),
mCallResponded(false)
{
	mDistId = 0; // ???????????????? need fix bug
	mReplicPolicy = rep_policy;
}


bool 
AosJimoCallSyncWrite::makeCall(AosRundata *rdata)
{
	// A write call will modify data on the cubes. It sends 
	// requests to all the cubes at once and waits for enough responses
	// before returning.
	
	mStartTime = OmnTime::getTimestamp(); 
	aos_assert_rr(mCluster, rdata, false);
	bool rslt = mCluster->getTargetCubes(rdata, mDistId, mEndpointIDs);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(mEndpointIDs.size() > 0, rdata, false);

	for (u32 i=0; i<mEndpointIDs.size(); i++)
	{
		mEndpointStatus.push_back(eNotRespondedYet);
	}

	mJimoCallID = mCluster->nextJimoCallID(rdata);
	aos_assert_rr(mJimoCallID > 0, rdata, false);

	/*
	AosBuff *buff = mBSON.getBuffRaw();
	aos_assert_r(buff, false);

	rdata->serializeToBuffForJimoCall(buff);

	buff->appendU64((u64)OmnMsgId::eJimoCallReq);
	buff->appendU64(mJimoCallID);
	buff->appendOmnStr(mPackage);
	buff->appendInt(mFunc);
	*/

	i64 dupid_pos = -1;
	AosBuffPtr buff = composeBuffNew(rdata, dupid_pos);
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(dupid_pos > 0, rdata, false);
	//char *data = buff->data();

	AosJimoCallWaiter::setWait(*this);

	AosJimoCallPtr thisptr(this, false);

	// 2. Make the call to all the endpoints in mEndpointIDs.
	mNumResponses = 0;
	//u32 num_failed = 0;

	//Phil:
	//if without lock, the response coming back in 
	//the middle will destroy request data
	mLock->lock();

	// Chen Ding, 2015/07/07
	// for (u32 i=0; i<mEndpointIDs.size(); i++)
	// {
	// 	bool rslt = mCluster->makeJimoCall(rdata, mEndpointIDs[i], thisptr, buff);
	// 	if (!rslt) num_failed++;
	// }
	
	rslt = mCluster->makeJimoCall(rdata, mEndpointIDs[0].physical_id, thisptr, buff.getPtr());
	mLock->unlock();
	if (!rslt)
	{
		// The leader is not available. It should try the next leader.
		OmnNotImplementedYet;
		return false;
	}

	// if (num_failed >= mEndpointIDs.size())
	// {
	// 	mStatus = ePhysicalFail;
	// 	mCluster->eraseJimoCall(rdata, mJimoCallID);
	// 	logFailedCall(rdata);
	// 	return false;
	// }
	
	// mLock->lock();
	// mNumResponses += num_failed;
	// mLock->unlock();

	// for (u32 i=num_failed; i<mEndpointIDs.size(); i++)
	// {
	//	AosJimoCallWaiter::wait(*this, 30);	
	// }

	AosJimoCallWaiter::wait(*this, 60);

	// This is a sync call. It initiates a call and waits for the response. 
	// If it is a negative response or the timer expires, it will try the next
	// one until all the tries are made.
	int tries = 3;
	while (tries--)
	{
		if (isCallSuccess())
		{
			logCall(rdata);
			mCluster->eraseJimoCall(rdata, mJimoCallID);
			return true;
		}

		OmnString errmsg = getOmnStr(rdata, AosFN::eErrmsg, "");
		if (errmsg == "not_leader")
		{
			// The endpoint is not the leader. 
			int leader_id = getInt(rdata, AosFN::eLeader, -1);
			aos_assert_rr(leader_id >= 0, rdata, false);

			setIdle();
			rslt = mCluster->makeJimoCall(rdata, leader_id, thisptr, buff.getPtr());
			aos_assert_rr(rslt, rdata, false);

			AosJimoCallWaiter::wait(*this, 60);
		}
	}

	// The call failed.
	AosLogError(rdata, false, "failed_making_calls") << enderr;
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	mStatus = ePhysicalFail;
	return true;
}


bool 
AosJimoCallSyncWrite::callResponded(
		AosRundata *rdata,
		AosJimoCall &jimo_call)
{
	OmnScreen << "Call responded: " << jimo_call.getJimoCallID() << endl;
	aos_assert_r(mJimoCallID == jimo_call.getJimoCallID(), false);

	mLock->lock();
	mStatus = jimo_call.getStatus();
	mBSON.swap(jimo_call.getBSON());
	mConnBuff = jimo_call.getConnBuff();

	// This is a reading sync call. Trigger the semaphore.
	mSem->post();
	mLock->unlock();
	return true;

	/*
	OmnScreen << "Call responded: " << jimo_call.getJimoCallID() << endl;
	aos_assert_r(mJimoCallID == jimo_call.getJimoCallID(), false);

	// if (mCallResponded) return true;

	//int endpoint_id = jimo_call.getInt(rdata, AosFN::eEndpointID, 1);
	AosEndPointInfo endpoint = jimo_call.getFromEPInfo();
	// int physical_id = 1;//endpoint.mEpId;
	int physical_id = endpoint.mEpId;
	aos_assert_rr(physical_id>= 0, rdata, false);

	// Check whether sufficient cubes have responded.
	mLock->lock();

	if (mCallResponded)
	{
		mLock->unlock();
		return true;
	}

	// Find the endpoint.
	int idx = -1;
	for (u32 i=0; i<mEndpointIDs.size(); i++)
	{
		if (mEndpointIDs[i].physical_id == physical_id) 
		{
			idx = i;
			break;
		}
	}

	if (idx < 0 || (u32)idx >= mEndpointIDs.size())
	{
		mLock->unlock();
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (jimo_call.isLogicalFail())
	{
		mCallResponded = true;
		mStatus = eLogicalFail;
		aos_assert_rl(mSem, mLock, false);
		mSem->post();
		mLock->unlock();
		return true;
	}

	if (mEndpointStatus[idx] != eNotRespondedYet) 
	{
		// This is a duplicated response.
		mLock->unlock();
		return true;
	}

	// Update the status
	mNumResponses++;
	if (jimo_call.isCallSuccess())
	{
		mEndpointStatus[idx] = ePositiveResponse;
		mNumPositiveResponses++;
	}
	else
	{
		mEndpointStatus[idx] = eNegativeResponse;
	}

	// Check whether enough responses have been received.
	// Enough responses means:
	// 	1. mNumPositiveResponses >= number of sync copies, or
	// 	2. mNumResponses >= mEndpointIDs.size()
	if (!enoughResponded()) 
	{
		// Not yet. Need to wait.
		mLock->unlock();
		return true;
	}

	if (jimo_call.isCallSuccess() && mStatus == eCallIdle)
	{
		// Keep the first positive response.
		mStatus = eCallSuccess;
		mBSON.swap(jimo_call.getBSON());
		mConnBuff = jimo_call.getConnBuff();
	}
		
	// Enough responses have been received.
	if (mNumResponses >= mEndpointIDs.size())
	{
		// All have been responded. 
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		logCall(rdata);
	}

	mCallResponded = true;
	aos_assert_rl(mSem, mLock, false);
	mSem->post();
	mLock->unlock();
	return true;
	*/
}


void
AosJimoCallSyncWrite::checkTimer(AosRundata *rdata)
{
	// It is a sync writing call. A writing sync call should
	// have issued a number of calls simultaneously and is
	// waiting on mSem. When the timer expires, it checks 
	// whether it should stop the call. 
	if (mCallResponded) return;

	if (!writeTimerExpired(OmnGetTimestamp()))
	{
		// The write timer has not expired yet. Continue
		// waiting.
		return;
	}

	// The write timer expireed. 
	mLock->lock();
	mCallResponded = true;	

	if (mNumPositiveResponses > 0)
	{
		mStatus = eCallSuccess;
	}
	else
	{
		mStatus = ePhysicalFail;
	}

	aos_assert(mSem);
	mSem->post();
	mLock->unlock();

	if (mNumResponses >= mEndpointIDs.size())
	{
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		logCall(rdata);
		logTimeoutCall(rdata);
	}
}

