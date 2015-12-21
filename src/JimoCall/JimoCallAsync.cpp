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
#include "JimoCall/JimoCallAsync.h"
#include "API/AosApi.h"
#include "JimoCall/JimoCallDialer.h"
#include "JimoCall/JimoCallWaiter.h"
#include "SEInterfaces/JimoCaller.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/CubeMapObj.h"
#include "Thread/Sem.h"
#include "UtilData/FN.h"
#include "Util1/Timer.h"
#include "Thread/ThreadPool.h"

AosJimoCallAsync::AosJimoCallAsync(
		AosRundata			*rdata,
		const OmnString		&package,
		const i64			cube_id,
		AosClusterObj		*cluster,
		AosJimoCallerPtr	caller,
		const int			iCallTimeoutThreshold,
		const u32			ulRetryTimesThreshold)
:
AosJimoCall(rdata, package, -1, cluster),
mCubeID(cube_id),
mCaller(caller),
mLock(OmnNew OmnMutex()),
mTimerLock(OmnNew OmnMutex()),
mCallSucceeded(false),
mCallTimeoutThreshold(iCallTimeoutThreshold),
mRetryTimesThreshold(ulRetryTimesThreshold),
mTimeoutCount(0),
mRdata(rdata),
mTimerID(0)
{
}


AosJimoCallAsync::AosJimoCallAsync(
		AosRundata				*rdata,
		const OmnString			&package,
		const int				func,
		const i64				cube_id,
		const AosJimoCallerPtr	&caller,
		AosClusterObj			*cluster,
		const int				iCallTimeoutThreshold,
		const u32				ulRetryTimesThreshold)
:
AosJimoCall(rdata, package, func, cluster),
mCubeID(cube_id),
mCaller(caller),
mLock(OmnNew OmnMutex()),
mTimerLock(OmnNew OmnMutex()),
mCallSucceeded(false),
mCallTimeoutThreshold(iCallTimeoutThreshold),
mRetryTimesThreshold(ulRetryTimesThreshold),
mTimeoutCount(0),
mRdata(rdata),
mTimerID(0)
{
}


bool
AosJimoCallAsync::makeCall(AosRundata *rdata)
{
	aos_assert_rr(mCluster, rdata, false);

	bool rslt = mCluster->getTargetCubes(rdata, mCubeID, mEndpointIDs);
	aos_assert_rr(rslt, rdata, false);

	//mCubeID may be -1, meaning unknown cube, initially which means the cluster shall allocate a cube
	mCubeID = mEndpointIDs.begin()->cube_id;

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
	mBuffInitialSend = OmnNew AosBuff(mBSON.getCrtBuff()->data(), mBSON.getCrtBuff()->buffLen(),
			mBSON.getCrtBuff()->dataLen(), true AosMemoryCheckerArgs);
	AosJimoCallPtr thisptr(this, false);
	mCrtEndpointIdx = 0;

	// This is an async call. It initiates the call
	// and returns immediately.
	mStartTime = OmnGetTimestamp();
	while (mCrtEndpointIdx < mEndpointIDs.size())
	{
		int endpoint_id = mEndpointIDs[mCrtEndpointIdx].physical_id;
		OmnScreen << "jimocall id:" << mJimoCallID << " is sending msg to No. " << mCrtEndpointIdx << " endpoint." << endl;

		//Gavin 2015/09/14
		if (rdata->getJPID() == 0)
		{
			OmnAlarm << "invalid_jpid" << enderr;
		}
		if (mCluster->makeJimoCall(rdata, endpoint_id, thisptr, buff.getPtrNoLock()))
		{
			mTimeoutCount = 0;
			if (0 < mRetryTimesThreshold)
			{
				OmnTimerObjPtr thisptr(this, false);
				mTimerLock->lock();
				mTimerID = OmnTimer::getSelf()->startTimer("AsyncJimoCallTimerForInitialTry",
						mCallTimeoutThreshold, 0, thisptr, 0);
				mTimerLock->unlock();
			}
			return true;
		}
		mCrtEndpointIdx++;
	}

	// This is a serious problem
	AosLogError(rdata, false, "failed on making calls") << enderr;
	logFailedCall(rdata);
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	return false;
}


bool
AosJimoCallAsync::callResponded(
		AosRundata *rdata,
		AosJimoCall &jimo_call)
{
	aos_assert_r(mJimoCallID == jimo_call.getJimoCallID(), false);
	OmnScreen << "Call responded, call id: " << jimo_call.getJimoCallID()
			<< " on method:" << getOmnStr(rdata, AosFN::eMethod, "")
			<< " docid:" << getU64(rdata, AosFN::eDocid, 0)
			<< " snap_id:" << getU64(rdata, AosFN::eSnapID, 0) << endl;

	if (mCallSucceeded) return true;
//	mLock->lock();
//	mLock->unlock();

	mTimerLock->lock();
	if (0 != mTimerID)
	{
		OmnTimer::getSelf()->cancelTimer(mTimerID);
	}
	mTimerLock->unlock();
	if (jimo_call.isCallSuccess())
	{
		// Call successful. Need to call back.
		OmnScreen << "Call succeeded, id: " << jimo_call.getJimoCallID() << " at timestamp:" << OmnGetTimestamp() << endl;
		mCallSucceeded = true;
		mStatus = jimo_call.getStatus();
		mBSON.swap(jimo_call.getBSON());
		mConnBuff = jimo_call.getConnBuff();
		mCluster->eraseJimoCall(rdata, mJimoCallID);

		if (mSem)
		{
			mSem->post();
			OmnScreen << "sem posted, id: " << jimo_call.getJimoCallID() << " at timestamp:" << OmnGetTimestamp() << endl;
		}

		if (mCaller)
		{
			mCaller->callFinished(rdata, *this);
			logCall(rdata);
			return true;
		}

		return true;
	}
	mStatus = jimo_call.getStatus();
	// Check whether the leader changed.
	OmnString errmsg = jimo_call.getOmnStr(rdata, AosFN::eErrmsg, "");
	OmnScreen << "Call failed, id: " << jimo_call.getJimoCallID() 
				<< " errmsg responded:'" << errmsg << "'" << endl;
	if (errmsg == "not_leader")
	{
		// The endpoint is not the leader.
		int leader_id = jimo_call.getInt(rdata, AosFN::eLeader, -1);
//		aos_assert_rr(leader_id >= 0, rdata, false);
		OmnScreen << "Responded leader id:" << leader_id << endl;
		bool rslt = mCluster->updateLeadership(rdata, mCubeID, leader_id, mEndpointIDs);
		if (!rslt)
		{
			OmnAlarm << "mCluster->updateLeadership failed, jimocall id:" << jimo_call.getJimoCallID()
					<< " cubeid:" << jimo_call.getCubeID() << " leader id returned:" << leader_id << enderr;
			return false;
		}
		setIdle();
//		AosBuffPtr buff = mBSON.getBuff();
//		aos_assert_r(buff, false);
		AosJimoCallPtr thisptr(this, false);
		mCrtEndpointIdx = 0;

		// This is an async call. It initiates the call
		// and returns immediately.
		mStartTime = OmnGetTimestamp();
		while (mCrtEndpointIdx < mEndpointIDs.size())
		{
			int endpoint_id = mEndpointIDs[mCrtEndpointIdx].physical_id;
			OmnScreen << "jimocall id:" << mJimoCallID << " is resending msg to leader, i.e. No. " << mCrtEndpointIdx << " endpoint." << endl;
			if (mCluster->makeJimoCall(rdata, endpoint_id, thisptr, mBuffInitialSend.getPtrNoLock()))
			{
				if (0 < mRetryTimesThreshold)
				{
					OmnTimerObjPtr thisptr(this, false);
					mTimerLock->lock();
					mTimerID = OmnTimer::getSelf()->startTimer("AsyncJimoCallTimerForLeaderChange", mCallTimeoutThreshold, 0, thisptr, 0);
					mTimerLock->unlock();
				}
				return true;
			}
			mCrtEndpointIdx++;
		}
		return true;
	}

	if (isLogicalFail())
	{
		OmnAlarm << "Logical fail" << enderr;
		mCallSucceeded = true;
		mCaller->callFinished(rdata, *this);
		mCluster->eraseJimoCall(rdata, mJimoCallID);
		logFailedCall(rdata);
		return true;
	}

OmnScreen << "Physical fail" << endl;
	/*
	// The call failed. Try the next one
	AosJimoCallPtr thisptr(this, false);
	AosBuff *buff = mBSON.getBuffRaw();
	while (++mCrtEndpointIdx < mEndpointIDs.size())
	{
		if (mCluster->makeJimoCall(rdata, mCrtEndpointIdx, thisptr, buff))
		{
			mLock->unlock();
			return true;
		}

		mCrtEndpointIdx++;
	}

	// All have tried but failed.
	mCallResponded = true;
	mStatus = ePhysicalFail;
	mLock->unlock();
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	mCaller->callFinished(rdata, *this);
	logFailedCall(rdata);
	*/
return false;
}


void
AosJimoCallAsync::checkTimer(AosRundata *rdata)
{
	// It is a sync writing call. A writing sync call should
	// have issued a number of calls simultaneously and is
	// waiting on mSem. When the timer expires, it checks
	// whether it should stop the call.
	if (OmnGetTimestamp() - mStartTime < mTimerMs)
	{
		// The write timer has not expired yet. Continue
		// waiting.
		return;
	}

	if (mCallSucceeded) return;

	// The write timer expireed.
	mLock->lock();

	// The call has not responded yet.
	mCallSucceeded = true;
	mLock->unlock();
	mCaller->callFinished(rdata, *this);
	logTimeoutCall(rdata);
	mCluster->eraseJimoCall(rdata, mJimoCallID);
	return;
}


bool
AosJimoCallAsync::waitForFinish()
{
	AosJimoCallWaiter::wait(*this, 60);
	return true;
}


bool
AosJimoCallAsync::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
//	AosBuffPtr buff = mBSON.getBuff();
//	aos_assert_r(buff, false);
	AosJimoCallPtr thisptr(this, false);
	bool rslt = mCluster->putTimeoutEndpointToEnd(mRdata.getPtrNoLock(), mCubeID,
			mEndpointIDs[mCrtEndpointIdx].physical_id, mEndpointIDs);
	if (!rslt)
	{
		OmnAlarm << "mCluster->updateLeadership failed, timeout endpoint id:"
				<< mEndpointIDs[mCrtEndpointIdx].physical_id << enderr;
		return false;
	}
	while (mCrtEndpointIdx < mEndpointIDs.size())
	{
		int endpoint_id = mEndpointIDs[mCrtEndpointIdx].physical_id;
		OmnScreen << "jimocall id:" << mJimoCallID << " is sending msg for timeout No." << mTimeoutCount << " to No. " << mCrtEndpointIdx << " endpoint." << endl;
		if (mCluster->makeJimoCall(mRdata.getPtrNoLock(), endpoint_id, thisptr, mBuffInitialSend.getPtrNoLock()))
		{
			state = OmnThrdStatus::eStop;
			if (0 < mRetryTimesThreshold)
			{
				OmnTimerObjPtr thisptr(this, false);
				mTimerLock->lock();
				mTimerID = OmnTimer::getSelf()->startTimer("AsyncJimoCallTimerForTimeoutRetry",
						mCallTimeoutThreshold, 0, thisptr, 0);
				mTimerLock->unlock();
				OmnScreen << "starting timer with id:" << mTimerID << " name:"
						<< "AsyncJimoCallTimerForTimeoutRetry" << endl;
			}
			return true;
		}
		mCrtEndpointIdx++;
	}
	return false;
}


bool
AosJimoCallAsync::signal(const int threadLogicId)
{
	return true;
}

void
AosJimoCallAsync::timeout(
		const int		timerId,
		const OmnString	&timerName,
		void			*parm)
{
	OmnScreen << "timeout No." << mTimeoutCount << " with id:" << timerId << " name:" << timerName
			<< " delay:" << mCallTimeoutThreshold << " in AosJimoCallAsync, jimocall id:" << mJimoCallID << endl;
	if (mCallSucceeded)
	{
		return;
	}
	if (mTimeoutCount > mRetryTimesThreshold)
	{
		OmnAlarm << "Timeout after " << mTimeoutCount
				<< " times that exceeds mRetryTimesThreshold:" << mRetryTimesThreshold << enderr;
		mStatus = eCallTimeout;
		return;
	}
	OmnThreadedObjPtr thisptr(this, false);
	if (mThread.notNull())
	{
		mThread->stop();
	}
	mThread = OmnNew OmnThread(thisptr, "AosJimoCallAsync", (int)mTimeoutCount++, false, false, __FILE__, __LINE__);
	mThread->start();
	return;
}
