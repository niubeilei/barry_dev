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
// 2014-12-19 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_HeaderChangeLog_H_
#define Aos_BlobSE_HeaderChangeLog_H_

#include "BlobSE/HeaderFileMgr.h"
#include "DfmUtil/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#if 0
class AosHeaderChangeLog :	public virtual OmnRCObject,
							public OmnThrdShellProc,
							public virtual OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum DefaultValues
	{
		eDeltaSize							= 10000,
		eHeaderChageLogMaxSize				= 1014*1024*1024,	//1G
		eHeaderChangeLogFileSize_default	= 1024*1024*10,		//10M
		eMaxNumOfHeaders					= 1000000,
		eDeltaSavingInterval				= 10,				//10 seconds
		eDeltaSaveCountOfEveryHeaderFlush	= 60,
		eHeadersHolding						= 1000000,			//count of headers
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
	AosBuffArrayPtr		mChangeLogBuffArray;
	AosBuffArrayPtr		mOldBuff;
	bool				mIsSavingDelta;		//for serializing delta saving in order to avoid concurrency issue
	bool				mIsFlushingHeaders;
	AosRawSE*			mRawSE;
	AosRawFilePtr		mHeaderChangeLogFile;
	u32					mDeltaDataSize;		//count of headers
	u32					mLastSavedTime;
	AosHeaderFileMgr*		mHeaderFile;
	AosRawFile*			mActiveRawfile;
	int					mDeltaSavedCount;
	u32					mDeltaSaveCountOfEveryHeaderFlush;
	u32					mDeltaSavingInterval;
	AosDeletionLogPtr	mDelLog;
	AosBlobSE*			mBlobSE;

public:
    AosHeaderChangeLog(
    		AosRundata			*rdata,
    		AosRawSE 			*rawse,
			AosBlobSE			*blob_se,
    		AosHeaderFileMgr*		header_file,
    		AosRawFile*			active_rawfile,
    		const u32			header_size,
    		const AosXmlTagPtr	&conf);
    ~AosHeaderChangeLog();

	int appendHeader(
			AosRundata				*rdata,
			const AosBlobHeaderPtr	&header);

	int shutdown(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

	AosBuffPtr readHeaderChangeLogFromFiles(AosRundata	*rdata);

	//OmnThrdShellProc interface
	virtual bool run();
	virtual bool procFinished();
	//OmnThrdShellProc interface end

	bool sort(AosRundata *rdata);

	void switchActiveFile(AosRawFile*	active_rawfile);

	bool flushDirtyHeaders(
    		AosRundata	*rdata,
    		AosRawFile*	active_rawfile);

	bool removeHeaderChangeLogFiles(AosRundata *rdata);

	//OmnTimerObj interface
	virtual void timeout(
			const int		timerId,
			const OmnString	&timerName,
			void			*parm);
	//OmnTimerObj interface end

private:
	bool needToSaveDelta();

	bool needToFlushDirtyHeaders();

	int saveDelta();
};
#endif

#endif /* Aos_BlobSE_HeaderChangeLog_H_ */
