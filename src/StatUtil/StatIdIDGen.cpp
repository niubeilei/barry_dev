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
// 2014/08/11 Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "StatUtil/StatIdIDGen.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILUtil/IILId.h"
#include "Debug/Debug.h"
#include "SEInterfaces/IILClientObj.h"
AosStatIdIDGen::AosStatIdIDGen(const OmnString &stat_definition_key, const u32 nums)
:
mLock(OmnNew OmnMutex()),
mKey(stat_definition_key),
mCrtDocids(0),
mNumDocids(0),
mDocidNums(nums)
{
	mKey << "_ctrl";
	mNumVirtuals = AosGetNumCubes();
	
	for (int i = 0; i < mNumVirtuals; i++)
	{
		mCrtDocids.push_back(0);
		mNumDocids.push_back(0);
	}

}


AosStatIdIDGen::~AosStatIdIDGen()
{
}


u64
AosStatIdIDGen::nextDocid(const AosRundataPtr &rdata, const u32 cube_id)
{
	aos_assert_r(mNumVirtuals >0 && cube_id < (u32)mNumVirtuals, 0);
	mLock->lock();
	if (mNumDocids[cube_id] <= 0)
	{
		bool rslt = retrieveDocids(rdata, cube_id);
		if (!rslt)
		{
			mLock->unlock();
			return 0;
		}
	}
	u64 docid = mCrtDocids[cube_id];
	mCrtDocids[cube_id] ++;
	mNumDocids[cube_id] --;
	mLock->unlock();
	
	u64 stat_docid = cube_id;
	stat_docid = (stat_docid << 32) + docid;
	return stat_docid;
}


bool
AosStatIdIDGen::retrieveDocids(const AosRundataPtr &rdata, const u32 cube_id)
{
	u64 docid = 0;
	u64 initDocid = 1;
	u64 incValue = mDocidNums;
	u64 iilid = AOSIILID_STATID; 
	aos_assert_r(mKey != "", false);
	
	OmnString key = mKey;	
	key << "_" << cube_id;

	// Chen Ding, 2014/07/24
	AosIILClientObjPtr iilclient = AosIILClientObj::getIILClient();
	aos_assert_rr(iilclient, rdata, false);
	//bool rslt = AosIILClient::getSelf()->incrementDocidToTable(
	bool rslt = iilclient->incrementDocidToTable(
			iilid, key, docid, incValue, initDocid, true, rdata);
	aos_assert_rr(rslt, rdata, false);
	aos_assert_rr(docid, rdata, false);
	mCrtDocids[cube_id] = docid;
	mNumDocids[cube_id] = incValue;
	OmnTagFuncInfo << "key is: " << key << endl;
	return true;
}

