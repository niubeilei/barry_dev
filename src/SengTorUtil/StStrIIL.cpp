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
// 2014/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/StStrIIL.h"

#include "Random/RandomUtil.h"
#include "Util/BuffArrayVar.h"
#include "XmlUtil/XmlTag.h"


AosStStrIIL::AosStStrIIL(
		const OmnString &name, 
		const u64 iilid,
		const bool key_unique,
		const bool docid_unique,
		const bool stable)
:
AosStIIL(name, iilid, key_unique, docid_unique),
mMinLen(eDftMinLen),
mMaxLen(eDftMaxLen)
{
	AosRundataPtr rdata = OmnNew AosRundata();
	AosCompareFunPtr compKey = OmnNew AosFunVarStr(0, true, true);
	AosCompareFunPtr compDocid = OmnNew AosFunVarStr(0, true, true);
	mCacheKey = AosBuffArrayVar::create(compKey, stable, rdata);
	mCacheDocid = AosBuffArrayVar::create(compDocid, stable, rdata);
}


AosStStrIIL::~AosStStrIIL()
{
}


bool
AosStStrIIL::pickEntry(
		std::string &key,
		u64 &docid,
		bool &key_unique,
		bool &docid_unique, 
		bool &added,
		AosRundata *rdata)
{
	// It randomly generate an entry (key, docid). 
	// 1. If key_unique is true, if 'key' is already used, 'added' set to false.
	// 2. Otherwise, if 'key' is already used:
	// 	  1. if docid_unique is true and docid is used, 'added' set to false.
	// In all other cases, 'added' set to true.
	
	OmnString kk = OmnRandom::digitStr(mMinLen, mMaxLen);
	key = kk.data();
	docid = rand();
	key_unique = mKeyUnique;
	docid_unique = mDocidUnique;

	bool unique, rslt;
	int key_idx, docid_idx;

	key_idx = mCacheKey->findPos(0, kk.data(), kk.length(), unique);
	if ((key_idx >= 0 && !unique) && key_unique)
	{
		added = false;
		return true;
	}
	docid_idx = mCacheDocid->findPos(0, (char*)&docid, 8, unique);
	if ((docid_idx >= 0 && !unique) && docid_unique)
	{
		added = false;
		return true;
	}
	added = true;

	rslt = mCacheKey->insertEntry(key_idx, kk.data(), kk.length(), rdata);
	aos_assert_r(rslt, false);

	rslt = mCacheDocid->insertEntry(key_idx, (char*)&docid, 8, rdata);
	aos_assert_r(rslt, false);
	return true;
}


i64
AosStStrIIL::getIILSize()
{
	return mCacheKey->getNumEntries() + mCacheDocid->getNumEntries();
}


bool
AosStStrIIL::nextValue(i64 &idx, OmnString &key,
						u64 &docid)
{
	aos_assert_r(mCacheKey->getNumEntries() == mCacheDocid->getNumEntries(), false);
	aos_assert_r(idx < mCacheKey->getNumEntries(), false);

	char *data;
	int len;
	bool rslt = mCacheKey->getEntry(idx, data, len);
	aos_assert_r(rslt, false);

	OmnString key_str(data, len);
	key = "";
	key << key_str;

	rslt = mCacheDocid->getEntry(idx, data, len);
	aos_assert_r(rslt, false);
	docid = *((u64*)data);
	idx++;
	return true;
}

