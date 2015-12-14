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
#ifndef Aos_BlobSE_DiskRecovery_H_
#define Aos_BlobSE_DiskRecovery_H_
#include "BlobSE/BlobSE.h"
#include "BlobSE/OprIDCache.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#if 0
class AosDiskRecovery : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	typedef std::vector<AosOprIDCache::OprIDEntry>	oprid_entry_vec;
	typedef std::vector<u64>						existing_rawfid_vec;

private:
	AosOprIDCachePtr			mOprIDCache;
	oprid_entry_vec				mOprIDOfDamagedEntriesVec;
	AosBucketMgrPtr				mBucketMgr;
	AosBodyCachePtr				mBodyCache;
	AosBlobSE*					mBlobSE;
	AosRawSE*					mRawSE;
	AosHeaderFileMgrPtr			mHeaderFileMgr;
	u32							mHeaderCustomDataSize;
	existing_rawfid_vec			mExistingHeaderFileRawfidVec;

public:
    AosDiskRecovery(
    		AosBlobSE					*blobSE,
			AosRawSE					*rawSE,
			AosHeaderFileMgrPtr			&pHeaderFileMgr,
			AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
    		AosHeaderCachePtr			&pHeaderCache);
    virtual ~AosDiskRecovery();

    int doDiskRecovery(AosRundata *rdata);

private:
    int scanOprID(AosRundata *rdata);

    int syncDamagedEntries(AosRundata *rdata);

    bool bodyDamaged(
    		AosRundata	*rdata,
			const u32	ulBodySeqno);

    void pushDamagedOprId(const AosOprIDCache::OprIDEntry &sOprIDEntry);

    int generateHeader(
    		AosRundata			*rdata,
			AosBlobHeaderPtr	&pHeader,
			AosBuffPtr			&pEntryBuff);

    int restoreHeaders(AosRundata *rdata);

    int scanHeaderFiles(AosRundata *rdata);

    int getAllExistingHeaderFileRawfid(AosRundata *rdata);

    int getAllExistingNormalHeaderFileRawfid(AosRundata *rdata);

    int getAllExistingOldHeaderFileRawfid(AosRundata *rdata);
};

#endif /* Aos_BlobSE_DiskRecovery_H_ */
#endif
