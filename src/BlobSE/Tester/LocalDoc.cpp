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
// 2015/04/05	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/Tester/LocalDoc.h"

#include "Util/Buff.h"
#include "Random/RandomUtil.h"


// BlobSETester
//mBigDocWeight--->gBigDocWeight
//mSmallDocWeight--->gSmallDocWeight
//mSeedMap--->gSeedMap.
//mSeedCount--->gSeedCount.

extern int	gBigDocWeight;
extern int 	gSmallDocWeight;
extern u32	gSeedCount;
extern std::map<u32, OmnString>	gSeedMap;


AosLocalDoc::AosLocalDoc(const u64 docid)
:
mDocid(docid),
mDeletedFlag(false),
mSeedId(0),
mRepeat(0)
{
}


AosLocalDoc::AosLocalDoc(
		const u64 docid,
		const bool delete_flag,
		const u32 seed_id,
		const u32 repeat)
:
mDocid(docid),
mDeletedFlag(delete_flag),
mSeedId(seed_id),
mRepeat(repeat)
{
}


AosLocalDoc::~AosLocalDoc()
{
}


void
AosLocalDoc::resetData()
{
	mDeletedFlag = false;
	mSeedId = rand() % gSeedCount;
	switch(OmnRandom::percent(gBigDocWeight, gSmallDocWeight))
	{
		case 0: //big
			mRepeat = OmnRandom::nextInt1(30, 300);
			break;

		case 1: //small
			mRepeat = OmnRandom::nextInt1(0, 30);
			break;

		default:
			OmnAlarm << "invalid percentage!" << enderr;
	}
}

void
AosLocalDoc::markDeleted()
{
	// mark this local has deleted.
	mDeletedFlag = true;
}


u32
AosLocalDoc::getDocSize()
{
	aos_assert_r(mSeedId < gSeedCount, 0);
	return gSeedMap[mSeedId].length() * mRepeat;
}

OmnString 
AosLocalDoc::getPattern()
{
	aos_assert_r(mSeedId < gSeedCount, "");
	return gSeedMap[mSeedId];
}


AosBuffPtr
AosLocalDoc::getData()
{
	aos_assert_r(mSeedId < gSeedCount, 0);
	OmnString &pattern = gSeedMap[mSeedId];

	u32 doc_size = pattern.length() * mRepeat;
	AosBuffPtr buff = OmnNew AosBuff(doc_size
			AosMemoryCheckerArgs);
	for(u32 i=0; i<mRepeat; i++)
	{
		buff->setBuff(pattern.data(), pattern.length());
	}

	return buff;
}


bool
AosLocalDoc::isSameData(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	aos_assert_r(mSeedId < gSeedCount, 0);
	OmnString &pattern = gSeedMap[mSeedId];
	u32 pattern_len = pattern.length();

	char *data = buff->data();
	for(u32 i=0; i<mRepeat; i++)
	{
		int ret = memcmp(&data[i*pattern_len], pattern.data(), pattern_len);
		if(ret != 0) return false;
	}
	return true;
}


