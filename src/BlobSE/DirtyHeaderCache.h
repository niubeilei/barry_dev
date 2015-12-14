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
#ifndef Aos_BlobSE_DirtyHeaderCache_H_
#define Aos_BlobSE_DirtyHeaderCache_H_
#include "aosUtil/Types.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Entry.h"
#include "BlobSE/Marker.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/ThrdShellProc.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"

class AosDirtyHeaderCache : virtual public OmnRCObject,
							public OmnThrdShellProc,
							public virtual OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eDirtyHeaderCacheSize					= 100000000,	//100M
		eHeaderCacheFlushThreshold				= eDirtyHeaderCacheSize / 2,		//50M
		eFlushingWaitForComapctionToFinishTime	= 2,
		eFlushDirtyHeaderInterval				= 10,			//in second
	};

private:
	u32							mHeaderCustomDataSize;
	u32							mFlushingWaitForComapctionToFinishTime;		//wait for certain seconds if it is doing compaction
	u32							mFlushDirtyHeaderInterval;
	OmnMutex*					mLockRaw;
	OmnMutexPtr					mLock;
	AosBlobSE*					mBlobSE;
	AosRundata*					mRdata;
	bool						mIsFlushingHeaders;
	AosHeaderFileMgrPtr			mHeaderFileMgr;
	AosHeaderCache*				mHeaderCache;
	AosHeaderBuffAndFileMgrPtr	mHeaderBuffAndFileMgr;

public:
    AosDirtyHeaderCache(
    		AosRundata*							rdata,
    		const u32							ulHeaderCustomDataSize,
    		const AosHeaderFileMgrPtr			&pHeaderFileMgr,
    		const AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
    		AosBlobSE*							pBlobSE,
    		AosHeaderCache*						pHeaderCache,
    		const AosXmlTagPtr					&conf);

    virtual ~AosDirtyHeaderCache();

    AosBlobHeaderPtr readHeader(
    		AosRundata						*rdata,
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sNextHeader,
    		const u64						ullTimestamp);

    AosBlobHeaderPtr getTheOldestHeaderByNextHeaderPointerNoLock(
    		const u64						docid,
    		AosBlobHeader::HeaderPointer	&sNextHeader);

    int updateHeader(
    		AosRundata						*rdata,
    		AosBlobHeader::HeaderPointer	&sHeaderPointer,
			const AosBlobHeaderPtr			&pHeader,
			bool							&bNeedsToUpdateOldHeader);

    int shutdown(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

    int appendHeaderToDirtyCacheBuffForFlushingNoLock(
			const AosBlobHeaderPtr			&pHeader,
    		AosBlobHeader::HeaderPointer	&sNextHeader);

    int serializeToDirtyHeaderCache(
    		AosBlobHeaderPtr					&pHeader,
    		const AosBlobHeader::HeaderPointer	&sHeaderPos,
			bool								&bNeedsToFlushNormalHeader);

    int updatePrevHeaderPointerOfNextHeader(
    		AosRundata							*rdata,
    		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer,
    		const AosBlobHeaderPtr				&pHeader,
    		bool								&bNeedsToUpdateOldHeader);

    int updatePrevHeaderPointerOfNextHeaderInBuffForFlushing(
    		AosRundata							*rdata,
    		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer,
    		const AosBlobHeaderPtr				&pHeader,
    		bool								&bNeedsToUpdateOldHeader);

    int updateNextHeaderPointerOfPrevHeader(
    		AosRundata							*rdata,
    		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer,
    		const AosBlobHeaderPtr				&pHeader,
    		bool								&bNeedsToUpdateOldHeader);

    int updateBodyPointerOfHeaderInWorkingBuff(
    		AosRundata							*rdata,
    		const u64							docid,
			const u64							timestamp,
    		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
			const u64							ullOldRawfid,
			const u64							ullOldOffset,
			const u64							ullNewRawfid,
			const u64							ullNewOffset);

    bool isHeaderCachedInWorkingBuff(const AosBlobHeader::HeaderPointer &sNextHeader);

    bool isHeaderInBuffToFlush(const AosBlobHeader::HeaderPointer &sNextHeader);

    bool isFlushingDirtyHeaders() const {return mIsFlushingHeaders;}

    int informRaftThatFlushingHasCompleted();

	//OmnThrdShellProc interface
	virtual bool run();
	virtual bool procFinished();
	//OmnThrdShellProc interface end

	//OmnTimerObj interface
	virtual void timeout(
			const int		timerId,
			const OmnString	&timerName,
			void			*parm);
	//OmnTimerObj interface end

	//for debugging
    void dumpEpocDayMap(const char* pCaller);

private:
    char* getHeaderCacheForWorkingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader);

    char* getHeaderCacheForFlushingPointerByOffsetNoLock(const AosBlobHeader::HeaderPointer &sNextHeader);

    int saveHeaderToCacheBuff(
    		AosRundata						*rdata,
    		const AosBlobHeaderPtr			&pHeader,
    		AosBlobHeader::HeaderPointer	&sHeaderPointer);

    bool needToFlush();

    int startNewThreadToFlushDirtyHeaders();

    int flushDirtyHeaders();

    int switchHeaderBuffAndFile();

    int switchHeaderFileForFlushingNoLock();

    int saveMarkToActiveLogFile(AosMarker::MarkerType eType);

    int saveDirtyHeaderCacheIntoOldHeaderFile();
};

#endif /* Aos_BlobSE_DirtyHeaderCache_H_ */
