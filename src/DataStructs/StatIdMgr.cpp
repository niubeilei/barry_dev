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
#include "DataStructs/StatIdMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"

AosStatIdMgr::AosStatIdMgr(const AosBuffPtr &buff, const u64 &docs_per_distblock)
:
mBuff(0),
mDistBlockEntries(0),
mNumDistBlockEntries(0),
mDocsPerDistBlock(docs_per_distblock)
{
	if(buff) reset(buff);	
}


AosStatIdMgr::~AosStatIdMgr()
{
}

bool
AosStatIdMgr::createOneEmptyDocid()
{
	int64_t crt_idx = eDocidOff + mNumDistBlockEntries * eDocidLen; 
	mBuff->setCrtIdx(crt_idx);
	mBuff->setU64(0);

	mBuff->setCrtIdx(0);
	int64_t num = mBuff->getI64(-1);
	aos_assert_r(num == mNumDistBlockEntries, false);

	mDistBlockEntries = (u64 *)(mBuff->data()+ eDocidOff);
	mNumDistBlockEntries ++;

	mBuff->setCrtIdx(0);
	mBuff->setInt64(mNumDistBlockEntries);

	aos_assert_r(mBuff->dataLen() == eDocidOff + (mNumDistBlockEntries * eDocidLen), false);
	return true;
}


u64	
AosStatIdMgr::getEntry(const u32 &idx)
{
	if (idx >= mNumDistBlockEntries) return 0;
	return mDistBlockEntries[idx];
}


bool	
AosStatIdMgr::addEntry(const u32 &idx, const u64 &binary_docid)
{
	aos_assert_r(mDistBlockEntries[idx] == 0, false);
	mDistBlockEntries[idx] = binary_docid;
	return true;
}

bool
AosStatIdMgr::isCheck(const u32 &idx)
{
	if (idx * eDocidLen + eDocidOff < mBuff->dataLen());
	{
		return true;
	}
	return false;
}

void
AosStatIdMgr::reset(const AosBuffPtr &buff)
{
	aos_assert(buff);
	mBuff = buff;
	mBuff->reset();
	mNumDistBlockEntries = mBuff->getI64(-1);
	if (mNumDistBlockEntries < 0)
	{
		OmnAlarm << "xxx" << enderr;
		return;
	}

	mDistBlockEntries = (u64 *)(mBuff->data()+ eDocidOff);
	aos_assert(mBuff->dataLen() == eDocidOff + (mNumDistBlockEntries * eDocidLen));
}

u64 
AosStatIdMgr::getEntryByStatId(const u64 &stat_id)
{
	u32 idx = stat_id / mDocsPerDistBlock;
	if (idx >= mNumDistBlockEntries)
	{
		return 0;
	}

	aos_assert_r(isCheck(idx), 0);
	u64 docid = getEntry(idx);
	return docid;
}


bool 
AosStatIdMgr::addEntryByStatId(const u64 &stat_id, const u64 &binary_docid)
{
	u32 idx = stat_id / mDocsPerDistBlock;
	if (idx < mNumDistBlockEntries && getEntry(idx) != 0)
	{
		OmnAlarm << "internal_error" << enderr;
		return false;
	}

	while(mNumDistBlockEntries <= idx)
	{
		bool rslt = createOneEmptyDocid();
		aos_assert_r(rslt, false);
	}

	aos_assert_r(isCheck(idx), 0);
	bool rslt = addEntry(idx, binary_docid);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStatIdMgr::addEntryByStatId(
		vector<u64> &vt_statid,
		vector<u64> &vt_binary_docid)
{
	bool rslt;
	aos_assert_r(vt_statid.size() == vt_binary_docid.size(), false);

	for(u32 i = 0; i<vt_statid.size(); i++)
	{
		rslt = addEntryByStatId(vt_statid[i], vt_binary_docid[i]);
		aos_assert_r(rslt, false);
	}
	return true;
}


