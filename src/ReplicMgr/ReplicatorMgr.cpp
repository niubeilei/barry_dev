////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "ReplicMgr/ReplicatorMgr.h"

#include "API/AosApiG.h"



AosReplicatorMgr::AosReplicatorMgr()
{
}


AosReplicatorMgr::~AosReplicatorMgr()
{
}


bool
AosReplicatorMgr::replicateData(
		AosRundata *rdata, 
		const u64 docid,
		const u64 snapshot,
		const int replication_id,
		const int compressoin_id,
		const int encryption_id,
		AosBuff *data, 
		AosReplicCaller *caller)
{
	// This function creates a replicator and adds the replicator in 
	// its replicator queue. This is an asynced call.
	AosReplicatorPtr replicator = OmnNew AosReplicator(rdata, docid, snapshot, 
			replication_id, compression_id, encryption_id, data, caller);
	mLockRaw->lock();
	mMap[Entry(docid, snapshot)] = replicator;
	mLockRaw->unlock();
	replicator->start(rdata);
	return true;
}


bool
AosReplicatorMgr::createCubeComms(AosRundata *rdata) 
{
	// There is a AosFastComm for each cube. This function creates 
	// it. In the current implementations, it is implemented through
	// UDPs. It finds the cube's IP address and port. 
	mCubeComm = OmnNew AosCubeComm(rdata);
	mCubeComm->startReading(
	return true;
}


bool
AosReplicatorMgr::sendRequest(
		AosRundata *rdata, 
		const u32 from_endpoint_id,
		const u32 to_endpoint_id,
		AosBuff *request_raw)
{
	// This function sends the request 'request_raw' to the cube. 
	aos_assert_rr(cube_id < mCubeComms.size(), rdata, false);
	if (!mComm) createCubeComm(rdata);

	bool rslt = mComm->sendRequest(rdata, request_raw);
	if (!rslt)
	{
		// The cube is not good.
		handleFailedCube(rdata, cube_id);
		return true;
	}

	return true;
}


bool		
AosReplicatorMgr::msgRead(const OmnConnBuffPtr &buff)
{
	AosBuff bf(buff->getData());
	u32 msg_id = bf.getU32(0);
	switch (msg_id)
	{
	case OmnMsgId::eReplicRequest:
		 mLockRaw->lock();
		 while (1)
		 {
			 if (mReqQueue.size() > mMaxReqQueueSize)
			 {
				 mReqQueueCondVarRaw->wait(mLockRaw);
			 }
			 break;
		 }
		 mReqQueue.push_back(buff);
		 mReqQueueCondVarRaw->signal();
		 mLockRaw->unlock();
		 return true;

	case OmnMsgId::eReplicResponse:
		 return procReplicResponse(bf);

	default:
		 break;
	}
	
	OmnShouldNeverComeHere;
	return false;
}


OmnString	
AosReplicatorMgr::getCommListenerName() const
{
	return mName;
}


void 		
AosReplicatorMgr::readingFailed()
{
	return;
}


bool
AosReplicatorMgr::procReplicRequest(AosBuff &buff)
{
	// The message is in the following format:
	// 	message id			(u32)
	// 	from endpoint id	(u32)
	// 	replicator id		(u64)
	// 	proc id				(u32)
	u32 from_id = buff.getU32(0);
	u64 replicator_id = buff.getU64(0);
	u32 proc_id = buff.getU32(0);

	switch (proc_id
}


bool
AosReplicatorMgr::procReplicResponse(AosBuff &buff)
{
	// The message is in the following format:
	// 	message id			(u32)
	// 	from endpoint id	(u32)
	// 	replicator id		(u64)
	// 	status				(u8)
	u32 from_id = buff.getU32(0);
	u64 replicator_id = buff.getU64(0);
	u8 status = buff.getU8(0);
	if (from_id == 0)
	{
		OmnAlarm << "Invalid Sender" << enderr;
		return false;
	}

	mLock->lock();
	itr_t itr = mReplicators.find(replicator_id);
	if (itr == mReplicators.end())
	{
		mLock->unlock();
		OmnString errmsg;
		errmsg << "Replicator ID: " << replicator_id;
		AosBSONPtr log = logError(__FILE__, __LINE__, "missing_replicator", errmsg);
		addLog(log);
		return false;
	}

	AosReplicator *replicator = itr->second.getPtr();
	mLockRaw->unlock();
	return replicator->procResponse(status, from_id);
}


AosBSONPtr 
AosReplicatorMgr::logError(
		const OmnString &filename,
		const int line_num,
		const OmnString &err_type,
		const OmnString &err_msg)
{
	AosBSONPtr bson = OmnNew AosBSON();
	bson->appendStrFieldStr(AosFieldName::eErrorType, err_type);
	bson->appendStrFieldStr(AosFieldName::eErrorMsg, err_msg);
	bson->appendStrFieldStr(AosFieldName::eFilename, filename);
	bson->appendStrFieldStr(AosFieldName::eLineNum, line_num);
	return bson;
}


bool	
AosReplicatorMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnConnBuffPtr buff;
	while (state == OmnThrdStatus::eActive)
	{
		mLockRaw->lock();
		if (mReqQueue.size() == 0)
		{
			mReqQueueCondVarRaw->wait(mLockRaw);
			mLockRaw->unlock();
			continue;
		}
	
		buff = mReqQueue.front();
		mReqQueue.popFront();
		

	}

	return true;
}


bool	
AosReplicatorMgr::signal(const int threadLogicId)
{
	return true;
}
#endif
