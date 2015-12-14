////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 9 Apr 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_OldHeaderCache_H_
#define Aos_BlobSE_OldHeaderCache_H_

#include <ext/hash_map>
#include <queue>

#include "BlobSE/BlobHeader.h"
#include "BlobSE/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"

class AosOldHeaderCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eOldHeaderCacheSize	= 100000000,	//100M
	};

private:
	typedef std::hash_map<const u64, u32, u64_hash, u64_cmp> map_t;
	typedef AosOldHeaderCache::map_t::iterator itr_t;

private:
	OmnMutex*			mLockRaw;
	OmnMutexPtr			mLock;
	u32					mHeaderCustomDataSize;
	u32					mCacheSize;
	u32					mCompactionTimeWindowInDays;
	AosBuffPtr			mHeaderCacheBuff;
	std::queue<u32>		mAvailableBlocks;
	AosHeaderFileMgrPtr	mHeaderFileMgr;
	map_t				mCacheMap;

public:
    AosOldHeaderCache(
    		AosRundata*					rdata,
    		AosBlobHeaderPtr&			header,
			const AosHeaderFileMgrPtr	&pHeaderFileMgr,
			const AosXmlTagPtr			&conf);
    virtual ~AosOldHeaderCache();

    AosBlobHeaderPtr readHeader(
    		AosRundata						*rdata,
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sHeaderToRead,
    		const u64						ullTimestamp);

    int updatePrevHeaderPointer(
    		AosRundata							*rdata,
    		const u64							docid,
    		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
    		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer);

    int updateNextHeaderPointer(
    		AosRundata							*rdata,
    		const u64							docid,
    		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
    		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer);

    int updateBodyPointer(
    		AosRundata							*rdata,
    		const u64							docid,
			const u64							timestamp,
    		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
			const u64							ullOldRawfid,
			const u64							ullOldOffset,
			const u64							ullNewRawfid,
			const u64							ullNewOffset);

private:
    u32 getOffsetByNextHeaderPointer(const AosBlobHeader::HeaderPointer& sNextHeader);

    char* getHeaderCachePointerByOffset(const u32 offset);

    bool isHeaderCached(const AosBlobHeader::HeaderPointer& sNextHeader);

    char* allocateHeaderCacheNoLock(
    		const u64							docid,
    		const AosBlobHeader::HeaderPointer&	sNextHeader);

    u32 getAvailableHeaderCacheBlockCount();

    u32 getAvailableHeaderCacheBlockPointer();

    int ageOutHeaderBlocks();

    int initHeaderCacheBuff(const u32 ulHeaderInCacheSize);

    u64 getHashKey(const AosBlobHeader::HeaderPointer& sNextHeader);
};

#endif /* Aos_BlobSE_OldHeaderCache_H_ */
