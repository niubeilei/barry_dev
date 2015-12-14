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
// There is a time stored in the head. Periodically, the system will
// go over all the cached. 
// Header format:
// 		timestamp	u64
// 		iilid		u64
//
// Modification History:
// 2013/03/02 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DfmUtil/DfmDocIILCache.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


// Ketty 
#if 0
//bool AosDfmDocIILCache::smShowLog = true;

AosDfmDocIILCache::AosDfmDocIILCache(
		const u64 &docid, 
		const Type type) 
:
AosDfmDoc(docid, eIILCache),
mType(type)
{
}


AosDfmDocIILCache::~AosDfmDocIILCache()
{
}

	
AosDfmDocPtr
AosDfmDocIILCache::clone(const u64 &docid, const Type type)
{
	return OmnNew AosDfmDocIILCache(docid, type);	
}


bool
AosDfmDocIILCache::serializeFrom()
{
	// Header format is:
	// 	body seqno		u32
	// 	body offset		u64
	// 	iilid			u64
	// 	type			u32
	// 	timestamp		u64
	
	aos_assert_r(mHeaderBuff, false);
	mHeaderBuff->setCrtIdx(0);

	mBodySeqno = mHeaderBuff->getU32(0);
	mBodyOffset = mHeaderBuff->getU64(0);
	
	mIILID = mHeaderBuff->getU64(0);
	mType = (Type)mHeaderBuff->getU32(0);
	mTimestamp = mHeaderBuff->getU64(0);
	return true;
}


bool
AosDfmDocIILCache::serializeTo()
{
	if(!mHeaderBuff)
	{
		mHeaderBuff = OmnNew AosBuff(eHeaderSize, 0 AosMemoryCheckerArgs);
	}
	memset(mHeaderBuff->data(), 0, eHeaderSize);
	mHeaderBuff->setCrtIdx(0);

	mHeaderBuff->setU32(mBodySeqno);
	mHeaderBuff->setU64(mBodyOffset);
	
	mHeaderBuff->setU64(mIILID);
	mHeaderBuff->setU32(mType);
	mHeaderBuff->setU64(mTimestamp);
	
	return true;
}

bool
AosDfmDocIILCache::setBodySeqOff(
		const u32 b_seqno,
		const u64 b_offset)
{
	mLock->lock();
	if(!mHeaderBuff) createHeaderBuffLocked();
	
	mHeaderBuff->setCrtIdx(0);
	mHeaderBuff->setU32(b_seqno);
	mHeaderBuff->setU64(b_offset);
	
	//mHeaderBuff->setU64(mIILID);
	//mHeaderBuff->setInt(mTimestamp);
	
	mLock->unlock();
	return true;
}

#endif
