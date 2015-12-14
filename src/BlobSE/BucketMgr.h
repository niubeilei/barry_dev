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
// 23 Jul 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_BucketMgr_H_
#define Aos_BlobSE_BucketMgr_H_
#include "BlobSE/OprIDCache.h"
#include "BlobSE/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#if 0
class AosBucketMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef std::hash_map<const u32, AosBucketPtr, std::u32_hash, std::u32_cmp>	bucket_map_t;

	bucket_map_t				mBucketMap;
	AosBlobSE*					mBlobSE;
	AosRawSE*					mRawSE;
	u32							mOprIDsPerBucket;
	AosHeaderFileMgrPtr			mHeaderFileMgr;
	AosHeaderBuffAndFileMgrPtr	mHeaderBuffAndFileMgr;
	AosHeaderCachePtr			mNormalHeaderCache;

public:
    AosBucketMgr(
    		AosBlobSE					*blobSE,
			AosRawSE					*rawSE,
			AosHeaderFileMgrPtr			&pHeaderFileMgr,
			AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
    		AosHeaderCachePtr			&pHeaderCache);

    virtual ~AosBucketMgr();

    int putOprIDEntryIntoBucket(
    		AosRundata						*rdata,
    		const AosOprIDCache::OprIDEntry	&sOprIDEntry);

    int putHeaderIntoBucket(
    		AosRundata				*rdata,
    		const AosBlobHeaderPtr	&pHeader);

    int restoreHeaders(AosRundata *rdata);

private:
    AosBucketPtr getBucketByOprID(
    		AosRundata	*rdata,
    		const u64	ullOprID);

    u32 getBucketIDByOprID(const u64 ullOprID);
};

#endif /* Aos_BlobSE_BucketMgr_H_ */
#endif
