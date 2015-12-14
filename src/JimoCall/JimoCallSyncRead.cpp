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
#include "JimoCall/JimoCallSyncRead.h"

#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"


AosJimoCallSyncRead::AosJimoCallSyncRead(
		AosRundata *rdata, 
		const OmnString &package, 
		const int func, 
		const u64 dist_id, 
		const u32 rep_policy,
		AosClusterObj *cluster)
:
AosJimoCall(rdata, package, func, cluster),
mLock(OmnNew OmnMutex())
{
	mDistId = dist_id;
	mReplicPolicy = rep_policy;
}


bool 
AosJimoCallSyncRead::makeCall(AosRundata *rdata) 
{
	// A read call shall try to read from the master. If it does not respond
	// before the timer expires, it tries one of the backups. 
	//
	// Some calls involve more than one hop, such as DocClient, which requires
	// the caller initiates a request to DocDB frontend, which will hop to 
	// DocStore to actually get the data. This function assumes DocDB knows
	// how to access DocStore:
	//
	//			jimo_call --> DocDB --> DocStore
	
	mStartTime = OmnTime::getTimestamp(); 
	aos_assert_rr(mCluster, rdata, false);
	bool rslt = mCluster->getTargetCubes(rdata, mDistId, mEndpointIDs);
	aos_assert_rr(rslt, rdata, false);

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
	buff->appendInt(0);		// For cube id
	i64 crt_pos = buff->getCrtPos();
	buff->appendU8(0);		// For dupid 
	*/
	i64 dupid_pos = -1;
	AosBuffPtr buff = composeBuffNew(rdata, dupid_pos);
	aos_assert_rr(buff, rdata, false);
	aos_assert_rr(dupid_pos > 0, rdata, false);
	char *data = buff->data();

	AosJimoCallPtr thisptr(this, false);
	mCrtEndpointIdx = 0;

	AosJimoCallWaiter::setWait(*this);

	mLock->lock(); 
	rslt = mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff.getPtr());
	mLock->unlock();


	if (!rslt)                                                         
	{
		//The leader is not available. It should try the next leader. 
		OmnNotImplementedYet;                                          
		return false;                                                  
	}                                                                 


	AosJimoCallWaiter::wait(*this, 60);

	// This is a sync call. It initiates a call and waits for the response. 
	// If it is a negative response or the timer expires, it will try the next
	// one until all the tries are made.
	
/*	while (mCrtEndpointIdx < mEndpointIDs.size())
	{
		// Set the dupid
		data[dupid_pos] = mEndpointIDs[mCrtEndpointIdx].dupid;

		if (mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff))
		{
			AosJimoCallWaiter::wait(*this, 1);

			// The call woke up. It wakes up when:
			// 	1. Time out
			// 	2. Call responded
			if (isCallSuccess())
			{
				logCall(rdata);
				mCluster->eraseJimoCall(rdata, mJimoCallID);
				return true;
			}

			if (isLogicalFail())
			{
				logFailedCall(rdata);
				mCluster->eraseJimoCall(rdata, mJimoCallID);
				AosLogError(rdata, true, "jimo_call_failed")
					<< AosFN("Error Message") << getOmnStr(rdata, AosFN::eErrmsg, "")
					<< AosFN("File") << getOmnStr(rdata, AosFN::eFile, "")
					<< AosFN("Line") << getInt(rdata, AosFN::eLine, -1) << enderr;
				return false;
			}
		}

		logFailedTry(rdata);
		mCrtEndpointIdx++;
	}
	*/

	int tries = 3;
	while (tries--)
	{
		// Set the dupid
		data[dupid_pos] = mEndpointIDs[mCrtEndpointIdx].dupid;

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
		if (isLogicalFail())
		{
			logFailedCall(rdata);
			mCluster->eraseJimoCall(rdata, mJimoCallID);
			AosLogError(rdata, true, "jimo_call_failed")
				<< AosFN("Error Message") << getOmnStr(rdata, AosFN::eErrmsg, "")
				<< AosFN("File") << getOmnStr(rdata, AosFN::eFile, "")
				<< AosFN("Line") << getInt(rdata, AosFN::eLine, -1) << enderr;
			return false;
		}
	}

	// This is a serious problem
	AosLogError(rdata, false, "failed_making_calls") << enderr;
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	mStatus = ePhysicalFail;
	return false;
}


	bool 
AosJimoCallSyncRead::callResponded(
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
AosJimoCallSyncRead::checkTimer(AosRundata *rdata)
{
	// It is a reading sync call. 
	if (OmnGetTimestamp() - mStartTime < mTimerMs) return;

	// It timed out. Trigger the semaphore. 
	aos_assert(mSem);
	mSem->post();
	return;
}

