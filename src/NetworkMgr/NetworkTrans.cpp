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
//
// Modification History:
// 03/10/2012 Created by Chen Ding
// 2013/10/25 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "NetworkMgr/NetworkTrans.h"

#include "SEInterfaces/NetListener.h"

// static u64 sgBitmap[64] = 
// {
// 	0x01,
// 	0x02,
// 	0x04,
// 	0x08,
// 	0x10,
// 	0x20,
// 	0x40,
// 	0x80,
// 
// 	0x0100,
// 	0x0200,
// 	0x0400,
// 	0x0800,
// 	0x1000,
// 	0x2000,
// 	0x4000,
// 	0x8000,
// 
// 	0x010000,
// 	0x020000,
// 	0x040000,
// 	0x080000,
// 	0x100000,
// 	0x200000,
// 	0x400000,
// 	0x800000,
// 
// 	0x01000000,
// 	0x02000000,
// 	0x04000000,
// 	0x08000000,
// 	0x10000000,
// 	0x20000000,
// 	0x40000000,
// 	0x80000000,
// 
// 	0x0100000000LL,
// 	0x0200000000LL,
// 	0x0400000000LL,
// 	0x0800000000LL,
// 	0x1000000000LL,
// 	0x2000000000LL,
// 	0x4000000000LL,
// 	0x8000000000LL,
// 
// 	0x010000000000LL,
// 	0x020000000000LL,
// 	0x040000000000LL,
// 	0x080000000000LL,
// 	0x100000000000LL,
// 	0x200000000000LL,
// 	0x400000000000LL,
// 	0x800000000000LL,
// 
// 	0x01000000000000LL,
// 	0x02000000000000LL,
// 	0x04000000000000LL,
// 	0x08000000000000LL,
// 	0x10000000000000LL,
// 	0x20000000000000LL,
// 	0x40000000000000LL,
// 	0x80000000000000LL,
// 
// 	0x0100000000000000LL,
// 	0x0200000000000000LL,
// 	0x0400000000000000LL,
// 	0x0800000000000000LL,
// 	0x1000000000000000LL,
// 	0x2000000000000000LL,
// 	0x4000000000000000LL,
// 	0x8000000000000000LL
// };


AosNetworkTrans::AosNetworkTrans(
		const u64 &transid, 
		const AosXmlTagPtr &request,
		const u32 total_servers)
:
mLock(OmnNew OmnMutex()),
mTransId(transid),
mVirtualId(-1),
mModuleId(AosModuleId::eInvalid),
mCommand(AosNetCommand::eInvalid),
mRequest(request),
mFinishedFlag(false)
{
	if (total_servers > eMaxServers)
	{
		OmnThrowException("Too many servers");
	}
}


AosNetworkTrans::AosNetworkTrans(
		const u64 &transid, 
		const int virtual_id,
		const AosModuleId::E module_id,
		const AosNetCommand::E command)
:
mLock(OmnNew OmnMutex()),
mTransId(transid),
mVirtualId(-1),
mModuleId(AosModuleId::eInvalid),
mCommand(AosNetCommand::eInvalid),
mFinishedFlag(false)
{
}


AosNetworkTrans::~AosNetworkTrans()
{
}


/*
bool 
AosNetworkTrans::isAllFinished() const
{
	aos_assert_r(mTotalServers > 0, true);

	mLock->lock();
	bool rslt = false;
	if (mTotalServers == 1)
	{
		if (mStatus.size() == 1) rslt = mStatus[0];
		mLock->unlock();
		return rslt;
	}

	u32 remainder = mTotalServers & 0x3f;
	int num_elems = mTotalServers >> 6;

	if (num_elems > 0)
	{
		if (mStatus.size() < (u32)num_elems)
		{
			mLock->unlock();
			return false;
		}

		for (int i=0; i<num_elems; i++)
		{
			if (mStatus[i] != 0xffffffffffffffffLL)
			{
				mLock->unlock();
				return false;
			}
		}
	}

	if (remainder == 0)
	{
		mLock->unlock();
		return true;
	}

	num_elems++;
	if (mServers.size() < num_elems)
	{
		mLock->unlock();
		return false;
	}

	u64 dd = mServers[num_elems-1];
	rslt = dd & sgBitmaps[remainder];
	mLock->unlock();
	return rslt;
}


bool 
AosNetworkTrans::transFinished(
		const u32 sid, 
		bool &all_finished,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sid < mTotalServers, rdata, false);

	mLock->lock();
	mTotalFinished++;
	all_finished = (mTotalFinished >= mTotalServers);

	u32 remainder = sid & 0x3f;
	u32 num_elems = (sid >> 6) + 1;

	while (mStatus.size() < num_elems)
	{
		mStatus.push_back(0);
	}

	mStatus[num_elems-1] |= sgBitmap[remainder];
	mLock->unlock();
	return true;
}
*/

#endif
