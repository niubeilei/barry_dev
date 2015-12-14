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
// Modification History:
// 10/15/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocClient/Docid.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "IILUtil/IILId.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/IILClientObj.h"

OmnDocid::OmnDocid(const u64 &init_did, const u32 nums, const OmnString &key)
:
mLock(OmnNew OmnMutex()),
mCrtVirNum(0),
mDocidNums(nums)
{
	mDocidKey = key;
	mNumVirtuals = AosGetNumCubes();

	mInitDocid = init_did;
	mInitDocid = (mInitDocid - (mInitDocid % mNumVirtuals)) + mNumVirtuals;

	for (int i = 0; i < mNumVirtuals; i++)
	{
		mCrtDocids.push_back(0);
		mNumDocids.push_back(0);
	}
}


OmnDocid::~OmnDocid()
{
}


u64
OmnDocid::nextDocid(const int &vid, const AosRundataPtr &rdata)
{
	aos_assert_r(mNumVirtuals, 0);
	aos_assert_r(mDocidKey != "", 0);
	aos_assert_r(mInitDocid, 0);

	int vvid = vid;
	if (vvid < 0)
	{
		vvid = mCrtVirNum;
		mCrtVirNum = (mCrtVirNum + 1) % mNumVirtuals;
	}
	aos_assert_r(vvid < mNumVirtuals, 0);

	mLock->lock();
	if (mNumDocids[vvid] <= 0)
	{
		bool rslt = retrieveDocids(vvid, rdata);
		if (!rslt)
		{
			mLock->unlock();
			return 0;
		}
	}
	//docid = mCrtDocids[vvid] + mNumVirtuals;
	u64 docid = mCrtDocids[vvid];
	//mCrtDocids[vvid] ++;
	mCrtDocids[vvid] += mNumVirtuals;
	mNumDocids[vvid] --;
	mLock->unlock();
aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(docid)) == vvid, 0);
	return docid;
}


bool
OmnDocid::retrieveDocids(const int &vid, const AosRundataPtr &rdata)
{
	aos_assert_r(mDocidKey != "", false);
	u64 iilid = AOSIILID_SYSTEM_DOCIDS; 
	OmnString key = mDocidKey;	
	key << "_"<< vid;

	u64 incValue = mDocidNums * mNumVirtuals;
	u64 initDocid = mInitDocid + vid;
aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(initDocid)) == vid, 0);

	u64 docid = 0;
	bool rslt = AosIILClientObj::getIILClient()->incrementDocidToTable(
			iilid, key, docid, incValue, initDocid, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(docid, rdata, false);

aos_assert_r(AosGetCubeId(AosXmlDoc::getOwnDocid(docid)) == vid, 0);
	mCrtDocids[vid] = docid;
	mNumDocids[vid] = mDocidNums;
	return true;
}


void
OmnDocid::returnDocid(const u64 &docid, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
}
