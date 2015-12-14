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
// 2015-02-09 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_DeletionLog_H_
#define Aos_BlobSE_DeletionLog_H_

#include "BlobSE/Entry.h"
#include "BlobSE/HeaderCache.h"
#include "DfmUtil/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#if 0
class AosDeletionLog :	public virtual OmnRCObject,
							public OmnThrdShellProc,
							public virtual OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eSiteID								= 1,
		eCubeID								= 1,
		eAseID								= 1,
		eDeltaSize							= 10000,
		eDelLogMaxSize						= 1014*1024*1024,	//1G
		eDeletionLogFileSize_default		= 1024*1024*10,		//10M
		eMaxNumOfHeaders					= 1000000,
		eDeltaSavingInterval				= 10,				//10 seconds
		eDeltaSaveCountOfEveryCompaction	= 60,
		eCompactedBuffMaxSize				= 1024*1024*100,	//100M
		eCompactionThreshold				= 100000,
		eCompcationPercentageThreshold		= 10,
	};

private:
	u32					mSiteID;
	u32					mCubeID;
	u64					mAseID;
	u32					mLastSavedDeltaPos;
	OmnMutex*			mLockRaw;
	OmnMutexPtr			mLock;
	OmnMutex*			mSaveLockRaw;
	OmnMutexPtr			mSaveLock;
	AosRundata*			mRdata;
	AosCompareFunPtr	mCompareFunc;
	AosBuffArrayPtr		mDelLogBuffArray;
	AosBuffArrayPtr		mOldBuff;
	bool				mIsSavingDelta;		//for serializing delta saving in order to avoid concurrency issue
	bool				mIsCompacting;
	AosRawSE*			mRawSE;
	AosRawFilePtr		mDeletionLogFile;
	u32					mDeltaDataSize;		//count of headers
	u32					mLastSavedTime;
	AosRawFile*			mActiveRawfile;
	int					mDeltaSavedCount;
	u32					mDeltaSaveCountOfEveryCompation;
	u32					mDeltaSavingInterval;
	u32					mCompactionPercentageThreshold;
	AosBuffPtr			mCompactedBuff;
	u64					mCompactedFileID;
	u64					mCompactedBuffMaxSize;
	AosDfmDocPtr		mDoc;
	u32					mHeaderSize;
	u32					mCompactionThreshold;	//count of delete log entries
	AosHeaderCache*		mHeaderCache;
	std::vector<u64>	mvCompactedRawfids;

public:
    AosDeletionLog(
    		AosRundata			*rdata,
    		AosRawSE 			*rawse,
    		AosRawFile*			active_rawfile,
    		const AosDfmDocPtr	&doc,
    		const u32			header_size,
			AosHeaderCache*		header_cache,
    		const AosXmlTagPtr	&conf);
    ~AosDeletionLog();

	int writeDelLog(
			AosRundata				*rdata,
			const AosBlobHeaderPtr	&header);

	bool init(AosRundata	*rdata);

	int shutdown(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

	bool readDeletionLogFromFiles(AosRundata	*rdata);

	//OmnThrdShellProc interface
	virtual bool run();
	virtual bool procFinished();
	//OmnThrdShellProc interface end

	bool sort(AosRundata *rdata);

	void switchActiveFile(AosRawFile*	active_rawfile);

	bool compact(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

	bool removeDeletionLogFiles(AosRundata *rdata);

	//OmnTimerObj interface
	virtual void timeout(
			const int		timerId,
			const OmnString	&timerName,
			void			*parm);
	//OmnTimerObj interface end

private:
	bool needToSaveDelta();

	bool needToCompact();

	int saveDelta();

	bool compactRawfile(
			AosRundata		*rdata,
			AosBuffArray	*buff,
			i64				start,
			i64				end);

	bool switchCompactedBuff();

	bool entryDeleted(
			AosRundata	*rdata,
			const char	*entry,
			const u64	rawfid,
			const u64	offset);

	bool copyEntryToCompactedBuff(
			AosRundata		*rdata,
			char*			buff);

	bool compactedBuffBigEnough();

	bool flushAndSwitchCompactedBuff(AosRundata		*rdata);

//	AosDfmDocHeaderPtr getHeaderFromEntry(
//			AosRundata	*rdata,
//			const char	*entry);

	DeletionLogEntry* findEntryWithinNewDelLog(
			AosRundata	*rdata,
			const u64	docid);

	bool copyDelLogBuffToNewDelLogBuff(
			AosRundata		*rdata,
		    AosBuffArray	*buff,
		    i64				start,
		    i64				end);

	bool removeCompactedFiles(AosRundata *rdata);
};
#endif

#endif /* Aos_BlobSE_DeletionLog_H_ */
