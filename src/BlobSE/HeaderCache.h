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
// this class manages the blob header cache, it does the aging like every
// cache does. The disaster recovery is achieved by:
// 1.writing every change, modification and deletion included, into a header
//   change log, in memory, before doing any cache modification.
// 2.save the header change log to disk periodically, being header change log
//   files in every N seconds or the header change log in memory reaches size M.
// 3.append a header change log saved entry to the active change log
// 4.flush all the dirty headers into header files periodically, being header
//   files in every R seconds or the header change log in memory reaches size K.
// 5.append a header saved entry to the active change log
// 6.when system recovers from a failure, it does the following:
// 7.read the header change log files, constructs the header cache accordingly
// 8.read the active change log backwards, until it reaches the a header change
//   log saved entry or a header saved entry
// 9.read the active change log forwards, overwrite the header cache accordingly
// Modification History:
// 2014-12-19 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_HeaderCache_H_
#define Aos_BlobSE_HeaderCache_H_

#include "BlobSE/BlobHeader.h"
#include "BlobSE/Compaction.h"
#include "BlobSE/DirtyHeaderCache.h"
#include "BlobSE/Ptrs.h"
#include "DfmUtil/Ptrs.h"
#include "RawSE/RawFile.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"

using std::u64_hash;
using std::u64_cmp;

class AosHeaderCache : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eHeaderCacheSize	= 100000000,
		eAgeOutBatchSize	= 10000,
	};
	enum HeaderStatus
	{
		eActive		= 1,
		eDeleted	= 2,
	};
	typedef std::hash_map<const u64, AosBlobHeaderPtr, u64_hash, u64_cmp> map_t;
	typedef map_t::iterator itr_t;

private:
	u32							mSiteID;
	u32							mCubeID;
	u64							mAseID;
	u32							mHeaderCacheSize;
	u32							mAgeOutBatchSize;
	OmnMutex*					mLockRaw;
	OmnMutexPtr					mLock;
	AosRundata*					mRdata;
	map_t						mHeaderMap;		//stores the newest header objs that points to its older versions dwelling in mHeaderCacheBuff
	AosHeaderFileMgrPtr			mHeaderFileMgr;
//	AosHeaderChangeLogPtr		mChangLog;
	AosBlobSE*					mBlobSE;
	AosBlobSESnapshotMgrPtr		mSnapshotMgr;
	AosHeaderBuffAndFileMgrPtr	mHeaderBuffAndFileMgr;
	AosDirtyHeaderCachePtr		mDirtyHeaderCache;
	AosOldHeaderCachePtr		mOldHeaderCache;
	AosCompactionPtr			mCompactor;
	AosBitmapObjPtr				mDirtyHeaderBitmap;
	AosBitmapObjPtr				mDirtyHeaderBitmapForFlushing;
	AosBlobHeaderPtr			mpHead;				//for aging
	AosBlobHeaderPtr			mpTail;				//for aging

public:
    AosHeaderCache(
    		AosBlobSE*					blob_se,
    		AosRundata*					rdata,
    		AosRawSE*					rawse,
			AosBlobSESnapshotMgrPtr&	snapshot_mgr,
			AosBlobHeaderPtr&			header,
    		const AosXmlTagPtr			&conf);		//active change log
    ~AosHeaderCache();

    AosBlobHeaderPtr readHeader(
    		AosRundata	*rdata,
			const u64	docid,
			const u64	timestamp,
			bool		bShowAlarm = true);

    int updateHeader(
    		AosRundata				*rdata,
			const AosBlobHeaderPtr	&header);

    int deleteHeader(
    		AosRundata	*rdata,
			const u64	docid,
			const u64	timestamp);

    int init(AosRundata	*rdata);

    int shutdown(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

    int flushDirtyHeaders();

    int modifyBodyPointerToNewPos(
    		AosRundata	*rdata,
    		const u64	docid,
			const u64	timestamp,
    		const u64	ullOldRawfid,
    		const u64	ullOldOffset,
    		const u64	ullNewRawfid,
    		const u64	ullNewOffset);

	int doCompationAsRaftLeader();

	int doCompactionAsRaftFollower();

	bool isFlushingDirtyHeaders() const {return mDirtyHeaderCache->isFlushingDirtyHeaders();}

	bool isDoingCompaction() const {return mCompactor->isDoingCompaction();}

    void dumpHeadersByDocid(
    		const 		u64 docid,
    		const char*	pCaller = "",
			const int	line = 0);

	int updateHeaders(
			AosRundata							*rdata,
			const std::vector<AosBlobHeaderPtr>	&vHeaders,
			const u64							timestamp);

	int saveOldestOldHeaderFileSeqnoForCompaction(const u32 ulSeqno);

	int clearHeaderCacheForDocid(const u64 docid);

	int reloadNormalHeaderFileBuff(
			AosRundata	*rdata,
			const u64	docid,
			u32			&ulNormalHeaderFileSeqno,
			AosBuffPtr	&pNormalHeaderFileBuff,
			bool		&bNormalHeaderFileExists);

	int updateNormalHeaderInCache(const AosBlobHeaderPtr &header);

private:
    int serializeHeaderToNormalHeaderFileBuff(
			const u64	docid,
			AosBuffPtr	&pNormalHeaderFileBuff);

    bool needFlush(AosRundata *rdata);

    int writeChangelog(
    		AosRundata				*rdata,
			const AosBlobHeaderPtr	&header);

    u32 getCurrentHeaderInCache(const u64 docid);

    int moveToAgingHead(AosBlobHeaderPtr pHeader);

    int unlinkHeader(AosBlobHeaderPtr pHeader);

    AosBlobHeaderPtr readHeaderFromNormalHeaderFile(
    		AosRundata	*rdata,
			const u64	docid,
			bool		bShowAlarm = true);

    AosBlobHeaderPtr getNormalHeaderFromHeaderCache(const u64 docid);

    int rebuildHeaderCacheFromLogFile(AosRundata *rdata);	//Change Log Files

    int resolveUnresolvedNextHeaderPointer(
    		AosRundata *rdata,
    		const u64	docid,
    		AosBuffPtr	&pNormalHeaderFileBuff,
    		const bool	bNormalHeaderFileExists,
			bool 		&bNeedsToFlushNormalHeader);

    int resolveUnresolvedNextHeaderPointerOfNormalHeaderInCache(
    		AosRundata 			*rdata,
			AosBlobHeaderPtr	&pNormalHeaderInCache,
    		AosBuffPtr			&pNormalHeaderFileBuff,
    		const bool			bNormalHeaderFileExists);

    int resolveUnresolvedNextHeaderPointerOfDirtyHeaders(
    		AosBlobHeaderPtr	&pNormalHeader,
    		AosBuffPtr			&pNormalHeaderFileBuff,
    		const bool			bNormalHeaderFileExists,
			bool				&bNeedsToFlushNormalHeader);

    int pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush(
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sNextHeader,
    		AosBuffPtr						&pNormalHeaderFileBuff);

    void dumpNormalHeaderCache();

    int ageOutCleanHeaders();
};


#endif /* Aos_BlobSE_HeaderCache_H_ */
