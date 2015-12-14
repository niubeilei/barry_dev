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
#ifndef Aos_BlobSE_Bucket_H_
#define Aos_BlobSE_Bucket_H_
#include "BlobSE/BlobHeader.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/OprIDCache.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Util/HashUtil.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#if 0
class AosBucket : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	typedef std::vector<AosBlobHeaderPtr>								header_vec_t;
	typedef std::hash_map<const u64, header_vec_t, u64_hash, u64_cmp>	map_t;

private:
	u32							mBucketID;
	u32							mHeaderCustomDataSize;
	AosBuffArrayPtr				mHeaderBuffArray;
	AosBuffArrayPtr				mOprIDBuffArray;
	AosHeaderFileMgrPtr			mHeaderFileMgr;
	AosHeaderBuffAndFileMgrPtr	mHeaderBuffAndFileMgr;
	AosBuffPtr					mNormalHeaderFileBuff;
	u32							mNormalHeaderFileSeqno;
	AosHeaderCachePtr			mNormalHeaderCache;
	AosRawFilePtr				mHeaderBucketFile;
	AosRawFilePtr				mOprIDBucketFile;

public:
    AosBucket(
    		AosRundata					*rdata,
    		const u32					ulBucketID,
			AosBlobSE					*blobSE,
			AosRawSE					*rawSE,
			AosHeaderFileMgrPtr			&pHeaderFileMgr,
			AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
    		AosHeaderCachePtr			&pHeaderCache);

    virtual ~AosBucket();

    u32 getBucketID() const {return mBucketID;}

    int putOprIDEntryIntoBucket(
    		AosRundata						*rdata,
    		const AosOprIDCache::OprIDEntry	&sOprIDEntry);

    int putHeaderIntoBucket(
    		AosRundata				*rdata,
    		const AosBlobHeaderPtr	&pHeader);

    int restoreHeaders(AosRundata *rdata);

private:
    u64 getHeaderBucketFileRawfidByBucketID();

    u64 getOprIDBucketFileRawfidByBucketID();

    int constructHeaderBuffArray(AosRundata *rdata);

    int constructOprIDBuffArray(AosRundata *rdata);

    int constructHeaderLinksAndRecoverHeaders(AosRundata *rdata);

    int processOneDocid(
    		AosRundata	*rdata,
			const u64	ullDocid,
			const i64	illStartPos,
			const i64	illEndPos);

    int constructHeaderVector(
    		AosRundata		*rdata,
			const u64		ullDocid,
			const i64		illStartPos,
			const i64		illEndPos,
			header_vec_t	&vHeader);

    int saveNormalHeader(
    		AosRundata			*rdata,
    		AosBlobHeaderPtr	&pHeader);

    int saveOldHeaderToPos(
    		AosRundata			*rdata,
    		AosBlobHeaderPtr	&pHeader);

    int resolveHeaderPos(
    		AosRundata		*rdata,
			header_vec_t	&vHeader);

    int allocateSpaceForDamagedHeaderWithUnresolvedPos(
    		AosRundata			*rdata,
    		AosBlobHeaderPtr	&pHeader);

    int flushRecoveredHeaders(
    		AosRundata		*rdata,
			header_vec_t	&vHeader);

    int getOprIDPointersByDocid(
    		const u64	ullDocid,
    		i64			&illStartPos,
    		i64			&illEndPos);

    void dumpHeaderVector(
    		const char		*func,
			const int		line,
    		header_vec_t	&vHeader);
};

#endif /* Aos_BlobSE_Bucket_H_ */
#endif
