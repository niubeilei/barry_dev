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
// 5 May 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_Compaction_H_
#define Aos_BlobSE_Compaction_H_
#include <set>
#include "BlobSE/BlobHeader.h"
#include "BlobSE/BodyEntry.h"
#include "BlobSE/Marker.h"
#include "BlobSE/Ptrs.h"
#include "RawSE/Ptrs.h"
#include "RawSE/RawSE.h"
#include "Thread/ThrdShellProc.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util1/TimerObj.h"


class AosCompaction : virtual public OmnRCObject,
							public OmnThrdShellProc,
							public virtual OmnTimerObj
{
	OmnDefineRCObject;

public:
	enum
	{
		eDeletedPercentageThreshold				= 10,
		eHeaderCountForCompactionThreshold		= 100000,
		eNewLogFileSizeThreshold				= 10000000,
		eCompactionTimeWindowInUnits			= 30,
		eCompactionBackoffTime					= 20,
		ePendingDeletionBuffSize				= 10000000,
		eTempOldHeaderFileBuffSize				= 10000000,
	};

private:
	typedef std::vector<u32>	v_file_t;
	typedef std::vector<u64>	v_oprid_t;
	typedef std::set<u64>		set_offset_t;
	typedef std::vector<u64>	v_deleted_file_t;

private:
	u32							mSiteID;
	u32							mCubeID;
	u64							mAseID;
	OmnMutex*					mLockRaw;
	OmnMutexPtr					mLock;
	u32							mDeletedPercentageThreshold;
	u32							mHeaderCountForCompactionThreshold;
	u32							mCompactionBackoffTime;
	bool						mIsCompacting;
	bool						mIsLeader;
	u32							mCompactionTimeWindowInUnits;
	u64							mNewLogFileSizeThreshold;
	v_file_t					mFileSeqVec;
	v_oprid_t					mOprIDVec;
	AosCompareFunPtr			mCompareFunc;
	AosBuffArrayPtr				mPendingDeletionFileBufferArray;
	AosBlobHeaderPtr			mHeader;
	AosBlobSE*					mBlobSE;
	AosRawSE*					mRawSE;
	AosRundata*					mRdata;
	AosRawFilePtr				mPendingDeletionFile;
	AosRawFilePtr				mActiveCompactedFile;
	set_offset_t				mDeletedOffsetsSet;
	AosHeaderFileMgrPtr			mHeaderFileMgr;
	AosHeaderBuffAndFileMgrPtr	mHeaderBuffAndFileMgr;
	AosHeaderCachePtr			mHeaderCache;
	AosOldHeaderCache*			mOldHeaderCache;
	AosDirtyHeaderCache*		mDirtyHeaderCache;
	v_deleted_file_t			mDeletedFileRawfidVec;
	AosBuffPtr					mNewOldHeaderFileBuff;
	AosBuffPtr					mPendingDeletionFileBuff;
	u64							mCompactionStartTimestamp;

public:
    AosCompaction(
    		AosRundata							*rdata,
			AosBlobSE							*blobse,
    		AosRawSE							*rawse,
    		const AosXmlTagPtr					&conf,
    		const AosHeaderFileMgrPtr			&pHeaderFileMgr,
    		const AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
    		AosHeaderCache*						pHeaderCache,
			AosOldHeaderCache*					pOldHeaderCache,
    		const u32							header_custom_data_size);

    virtual ~AosCompaction();

    int doLeaderCompation();

    int doFollowerCompaction();

    int informRaftThatCompactionHasCompleted();

    bool needsCompaction();

    bool isDoingCompaction() const {return mIsCompacting;}

	int saveOldestOldHeaderFileTimestampToMetafile(const u64 ullTimestamp);

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

private:
    int startNewThreadToCompact();

    int informRaftLeader();

    int compactAsRaftLeader();

    int compactAsRaftFollower();

    int findExpiredOldHeaderFiles();

    int processExpiredOldHeaderFiles();

    int processPendingDeletionFile();

    int appendHeadersToPendingDeletionFile(
    		const i64	illStartPos,
    		const i64	illEndPos);

    int countDeletedSpaceOfOldHeaderFile(
    		const i64	illStartPos,
    		const i64	illEndPos,
    		u64			&ullDeletedLen);

    bool fileNeedsCompaction(
    		const u64	ullRawfid,
    		const u64	ullDeleteLen);

    int compactRawfile(
    		const u64	ullRawfid,
    		const i64	illStartPos,
    		const i64	illEndPos);

    int appendNotDeletedEntryToActiveCompactedFile(
    		const char*	pBuff,
    		const u32	ulLen,
    		u64			&ullRawfid,
    		u64			&ullOffset);

    int generateSetOfDeletedOffsets(
    		const u64	ullRawfid,
    		const i64	illStartPos,
    		const i64	illEndPos);

    bool entryDeleted(const u64 ullOffset);

    bool isActiveFileBigEnough();

    int switchActiveCompactedFile();

    int modifyBodyPointerToNewPos(
    		const u64	docid,
			const u64	timestamp,
    		const u64	ullOldRawfid,
    		const u64	ullOldOffset,
    		const u64	ullNewRawfid,
    		const u64	ullNewOffset);

    void markFileAsBeingDeleted(const u64 ullRawfid);

    void dumpDeletedFileIDs();

    void pushOprIDToVec(const u64 ullOprID);

	u64 getTimestampOfTheOldestExistingOldHeaderFileFromMetafile();

	u64 getCompactedFileRawfidFromMetafile();

	int saveCompactedFileRawfidToMetafile(const u64 ullRawfid);

	bool headerFileExpired(const AosRawFilePtr& pFile);

	int openActiveCompactedFile();

	int loadPendingDeletionFileIntoBuff();

//	int saveMarkToActiveLogFile(AosBodyEntry::EntryType eType);

	int saveMarkToActiveLogFile(
			AosMarker::MarkerType	eType,
			const u64				ullBeingCompacted = 0,
			const u64				ullCompacted = 0);

	bool headerValid(const AosBlobHeaderPtr	&pHeader);

	int processExpiredOldHeaderFile(const u32 ulSeqno);

	int copyHeaderToNewOldHeaderFile(const AosBlobHeaderPtr	&pHeader);

	int copyHeaderToPendingDeletionFileBuff(const AosBlobHeaderPtr	&pHeader);

	int saveNewOldHeaderFileBuff(
			const u32	ulSeqno,
			const u32	ulNewSeqno);

	int flushPendingDeletionFileBuffAndSetNextHeaderPointerOfPrevHeaderToNull();

	int replaceOldHeaderFileWithTempOldHeaderFile(const u32 ulSeqno);

	int updateHeaderPointers(const u32	ulSeqno);

	int createNewTempOldHeaderFile();

	void dumpExpiredOldHeaderFileSeqnoVec();

	int updateNextHeaderPointerOfNormalHeaderInFile(
			const u64							docid,
			const AosBlobHeader::HeaderPointer	&sNextHeaderPointer);

	int updateNextHeaderPointerOfNormalHeaderInFile(
			const u64							docid,
			const AosBlobHeader::HeaderPointer	&sNextHeaderPointer,
			u32									&ullNormalHeaderFileSeqno,
			AosBuffPtr							&pNormalHeaderFileBuff);

	int updateHeaderPointersForDeletedHeaders();
};

#endif /* Aos_BlobSE_Compaction_H_ */
