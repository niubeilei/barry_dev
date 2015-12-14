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
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataSync/DsRequest.h"

#include "Porting/GetTime.h"
#include "Thread/Mutex.h"



static u64 sgRequestId = 100;
static OmnMutex sgLock;

AosDsRequest::AosDsRequest(
		const int num_syncs,
		const AosDsDataType::E data_type, 
		const AosXmlTagPtr &data)
:
mNumSyncs(num_syncs),
mDataType(data_type),
mTime(OmnGetSecond()),
mData(data)
{
	aos_assert(mNumSyncs > 0);
	sgLock.lock();
	mRequestId = sgRequestId++;
	sgLock.unlock();

	memsg(mRespReceived, 0, sizeof(bool)); 
}


AosDsRequest::~AosDsRequest()
{
}


bool
AosDsRequest::respReceived(const int conn_idx)
{
	aos_assert_r(conn_idx >= 0 && conn_idx < mNumSyncs, false);

	sgLock.lock();
	mRespReceived[conn_idx] = true;
	for (int i=0; i<mNumSyncs; i++)
	{
		if (!mRespReceived[i]) 
		{
			sgLock.unlock();
			return true;
		}
	}
	sgLock.unlock();

	// All responses have been received. It is the time to remove the
	// request.
	mDataSync->requestFinished(mRequestId);
	return true;
}

#endif
