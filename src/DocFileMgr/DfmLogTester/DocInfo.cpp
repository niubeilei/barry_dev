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
// 04/11/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLogTester/DocInfo.h"

#include "Random/RandomUtil.h"
	
int	AosDocInfo::smMinSizes[eMaxLevel];
int AosDocInfo::smMaxSizes[eMaxLevel];
int AosDocInfo::smWeights[eMaxLevel];

void
AosDocInfo::staticInit()
{
	smMinSizes[0] = eSizeLevel0; smMaxSizes[0] = eSizeLevel1; smWeights[0] = eLevel1Weight; 
	smMinSizes[1] = eSizeLevel1; smMaxSizes[1] = eSizeLevel2; smWeights[1] = eLevel2Weight; 
	smMinSizes[2] = eSizeLevel2; smMaxSizes[2] = eSizeLevel3; smWeights[2] = eLevel3Weight; 
	smMinSizes[3] = eSizeLevel3; smMaxSizes[3] = eSizeLevel4; smWeights[3] = eLevel4Weight; 
	smMinSizes[4] = eSizeLevel4; smMaxSizes[4] = eSizeLevel5; smWeights[4] = eLevel5Weight; 
	smMinSizes[5] = eSizeLevel5; smMaxSizes[5] = eSizeLevel6; smWeights[5] = eLevel6Weight; 
	smMinSizes[6] = eSizeLevel6; smMaxSizes[6] = eSizeLevel7; smWeights[6] = eLevel7Weight;     
	smMinSizes[7] = eSizeLevel7; smMaxSizes[7] = eSizeLevel8; smWeights[7] = eLevel8Weight; 
	smMinSizes[8] = eSizeLevel8; smMaxSizes[8] = eSizeLevel9; smWeights[8] = eLevel9Weight; 
	smMinSizes[9] = eSizeLevel9; smMaxSizes[9] = eSizeLevel10; smWeights[9] = eLevel10Weight; 
	smMinSizes[10] = eSizeLevel0; smMaxSizes[10] = eSizeLevel11; smWeights[10] = eLevel11Weight;
}


AosDocInfo::AosDocInfo(const u64 docid)
:
mLock(OmnNew OmnMutex()),
mDocid(docid),
mPattern(0),
mRepeat(0)
{
	randData(mPattern, mRepeat);

	OmnString ss;
	ss << mPattern;
	u32 pattern_len = ss.length();
	mDocSize = pattern_len * mRepeat;	
}
	
AosDocInfo::AosDocInfo(
		const u64 docid,
		const u32 pattern,
		const u32 repeat)
:
mLock(OmnNew OmnMutex()),
mDocid(docid),
mPattern(pattern),
mRepeat(repeat)
{
	OmnString ss;
	ss << mPattern;
	u32 pattern_len = ss.length();
	mDocSize = pattern_len * mRepeat;	
}

AosDocInfo::~AosDocInfo()
{
}

AosDocInfoPtr
AosDocInfo::clone()
{
	AosDocInfoPtr doc = OmnNew AosDocInfo(mDocid, mPattern, mRepeat);	
	return doc;
}


void
AosDocInfo::reset()
{
	randData(mPattern, mRepeat);
	
	OmnString ss;
	ss << mPattern;
	u32 pattern_len = ss.length();
	mDocSize = pattern_len * mRepeat;	
}


AosBuffPtr
AosDocInfo::getData()
{
	/*
	static AosBuffPtr buff = OmnNew AosBuff(10000000, 0 AosMemoryCheckerArgs);
	static bool init = false;
	if (!init)
	{
		init = true;

		for (int i=0; i<10000000; i++)
		{
			buff->data()[i] = rand()%64 + 32;
		}

	}
	buff->setDataLen(10000000);
	return buff;
	*/

	AosBuffPtr buff = OmnNew AosBuff(mDocSize, 0 AosMemoryCheckerArgs);
	OmnString ss;
	ss << mPattern;
	
	for(u32 i=0; i<mRepeat; i++)
	{
		buff->setBuff(ss.data(), ss.length());
	}
	aos_assert_r(buff->dataLen() == mDocSize, 0);

	return buff;
}


bool
AosDocInfo::isSameData(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	OmnString ss;
	ss << mPattern;
	u32 pattern_len = ss.length();

	char *data = buff->data();
	for(u32 i=0; i<mRepeat; i++)
	{
		int ret = memcmp(&data[i*pattern_len], ss.data(), pattern_len);
		if(ret != 0)
		{
			mLock->unlock();
			return false;
		}
	}
	return true;
}


void
AosDocInfo::randData(u32 &pattern, u32 &repeat)
{
	// 1. Determine size. 
	u32 need_size = OmnRandom::intByRange(smMinSizes, smMaxSizes, smWeights, eLevelNum);
	
	pattern = rand();
	
	OmnString ss;
	ss << pattern;
	u32 pattern_len = ss.length();
	repeat = need_size / pattern_len;
}

	
AosDocInfoPtr
AosDocInfo::serializeFromStatic(const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	
	u64 docid = buff->getU64(0);
	u32 pattern = buff->getU32(0);
	u32 repeat = buff->getU32(0);
	
	return OmnNew AosDocInfo(docid, pattern, repeat);
}


bool
AosDocInfo::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	buff->setU64(mDocid);
	buff->setU32(mPattern);
	buff->setU32(mRepeat);
	return true;
}


