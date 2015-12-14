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
#if 0
#include "IILCache/DfmDocIILCache.h"

#include "Thread/Mutex.h"
#include "Util/Buff.h"
#include "Util/MemoryChecker.h"


bool AosDfmDocIILCache::smShowLog = true;

AosDfmDocIILCache::AosDfmDocIILCache(
		const u64 &docid, 
		const Type type) 
:
//AosDfmDoc(docid, eIILCache, eHeaderSize),
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
AosDfmDocIILCache::getBodySeqOff(
		const AosBuffPtr &header_buff,
		u32 &b_seqno,
		u64 &b_offset)
{
	// Header format is:
	// 	body seqno		u32
	// 	body offset		u64
	// 	iilid			u64
	// 	type			u32
	// 	timestamp		u64

	aos_assert_r(header_buff, false);
	if(header_buff->dataLen() == 0)
	{
		b_seqno = 0;
		b_offset = 0;
		return true;
	}
	
	header_buff->setCrtIdx(0);
	b_seqno = header_buff->getU32(0);
	b_offset = header_buff->getU64(0);
	
	//mIILID = header_buff->getU64(0);
	//mType = (Type)header_buff->getU32(0);
	//mTimestamp = header_buff->getU64(0);
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


void                                                                  
AosDfmDocDatalet::createHeaderBuffLocked()
{
	mHeaderBuff = OmnNew AosBuff(eHeaderSize, 0 AosMemoryCheckerArgs);
	memset(mHeaderBuff->data(), 0, eHeaderSize);
}

#endif
