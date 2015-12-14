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
#include "BlobSE/Compaction.h"

#include "BlobSE/BlobHeader.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/Entry.h"
#include "BlobSE/HeaderBuffAndFileMgr.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/Marker.h"
#include "BlobSE/OldHeaderCache.h"
#include "BlobSE/RawfidRange.h"
#include "RawSE/RawFile.h"
#include "RawSE/RawSE.h"
#include "Thread/ThreadPool.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util1/Timer.h"
AosCompaction::AosCompaction(
		AosRundata							*rdata,
		AosBlobSE							*blobse,
		AosRawSE							*rawse,
		const AosXmlTagPtr					&conf,
		const AosHeaderFileMgrPtr			&pHeaderFileMgr,
		const AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
		AosHeaderCache*						pHeaderCache,
		AosOldHeaderCache*					pOldHeaderCache,
		const u32							header_custom_data_size)
:
OmnThrdShellProc("BlobSEDirtyHeaderCache"),
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mIsCompacting(false),
mIsLeader(false),
mRdata(rdata)
{
	mBlobSE = blobse;
	if (!rawse)
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mRawSE = rawse;
	AosXmlTagPtr config = conf->getFirstChild("BlobSE", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named BlobSE");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mSiteID = config->getAttrU32("siteid", AosBlobSE::eSiteID);
	if (AosBlobSE::eSiteID == mSiteID)
	{
		OmnScreen << "site id set to default value:" << mSiteID
				<< ", please make sure this is what you want." << endl;
	}
	mCubeID = config->getAttrU32("cubeid", AosBlobSE::eCubeID);
	if (AosBlobSE::eCubeID == mCubeID)
	{
		OmnScreen << "cube id set to default value:" << mCubeID
				<< ", please make sure this is what you want." << endl;
	}
	mAseID = config->getAttrU32("aseid", AosBlobSE::eAseID);
	if (AosBlobSE::eAseID == mAseID)
	{
		OmnScreen << "ase id set to default value:" << mAseID
				<< ", please make sure this is what you want." << endl;
	}
	config = conf->getFirstChild("Compaction", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mDeletedPercentageThreshold = config->getAttrU32("deleted_percentage_threshold", eDeletedPercentageThreshold);
	if (eDeletedPercentageThreshold == mDeletedPercentageThreshold)
	{
		OmnScreen << "mDeletedPercentageThreshold set to default value:" << mDeletedPercentageThreshold
				<< ", please make sure this is what you want." << endl;
	}
	mHeaderCountForCompactionThreshold = config->getAttrU32("header_count_threshold", eHeaderCountForCompactionThreshold);
	if (eHeaderCountForCompactionThreshold == mHeaderCountForCompactionThreshold)
	{
		OmnScreen << "mHeaderCountForCompactionThreshold set to default value:" << mHeaderCountForCompactionThreshold
				<< ", please make sure this is what you want." << endl;
	}
	mCompactionTimeWindowInUnits =
			config->getAttrU32("compaction_time_windows_in_units", eCompactionTimeWindowInUnits);
	if (eCompactionTimeWindowInUnits == mCompactionTimeWindowInUnits)
	{
		OmnScreen << "mCompactionTimeWindowInUnits set to default value:"
				<< mCompactionTimeWindowInUnits << ", please make sure this is what you want." << endl;
	}
	mNewLogFileSizeThreshold = config->getAttrU32("new_log_file_size_threshold", eNewLogFileSizeThreshold);
	if (eNewLogFileSizeThreshold == mNewLogFileSizeThreshold)
	{
		OmnScreen << "mNewLogFileSizeThreshold set to default value:" << mNewLogFileSizeThreshold
				<< ", please make sure this is what you want." << endl;
	}
	mCompactionBackoffTime = config->getAttrU32("compaction_backoff_time", eCompactionBackoffTime);
	if (eCompactionBackoffTime == mCompactionBackoffTime)
	{
		OmnScreen << "mCompactionBackoffTime set to default value:" << mCompactionBackoffTime
				<< ", please make sure this is what you want." << endl;
	}
	mFileSeqVec.clear();
	mOprIDVec.clear();
	mHeader = OmnNew AosBlobHeader(0, header_custom_data_size);
	mPendingDeletionFile =
			mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID, ePendingDeletionFileID, AosRawFile::eModifyOnly);
	mDeletedOffsetsSet.clear();
	int iRet = openActiveCompactedFile();
	if (0 != iRet)
	{
		OmnAlarm << "openActiveCompactedFile failed, iRet=" << iRet << enderr;
		rdata->setErrmsg("openActiveCompactedFile failed");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeaderFileMgr = pHeaderFileMgr;
	mHeaderBuffAndFileMgr = pHeaderBuffAndFileMgr;
	mHeaderCache = pHeaderCache;
	mOldHeaderCache = pOldHeaderCache;
	mDeletedFileRawfidVec.clear();
	mCompareFunc = OmnNew AosFunCustom(
			mHeader->getHeaderInCacheAndHeaderFileLength(), false, conf->getFirstChild("Compaction", true));
//	mPendingDeletionFileBufferArray = OmnNew AosBuffArray(mCompareFunc, false, true,
//			mHeaderCountForCompactionThreshold * mHeader->getHeaderInCacheAndHeaderFileLength());
	mCompactionStartTimestamp = 0;
//	iRet = createNewTempOldHeaderFile();
//	if (0 != iRet)
//	{
//		OmnAlarm << "createNewTempOldHeaderFile failed, iRet=" << iRet << enderr;
//		rdata->setErrmsg("createNewTempOldHeaderFile failed");
//		OmnThrowException(rdata->getErrmsg());
//		return;
//	}
	mPendingDeletionFileBuff = OmnNew AosBuff(ePendingDeletionBuffSize AosMemoryCheckerArgs);
	mNewOldHeaderFileBuff = OmnNew AosBuff(eTempOldHeaderFileBuffSize AosMemoryCheckerArgs);
}


AosCompaction::~AosCompaction()
{
}


int
AosCompaction::doLeaderCompation()
{
	mIsLeader = true;
	if (!mHeaderCache->isFlushingDirtyHeaders())
	{
		startNewThreadToCompact();
	}
	else
    {
        OmnScreen << "header cache is flushing dirty headers, compaction backs off for "
        		<< mCompactionBackoffTime << " second(s)" << endl;
    }
    OmnTimerObjPtr thisptr(this, false);
    OmnTimer::getSelf()->startTimer("CompactionTimer", mCompactionBackoffTime, 0, thisptr, 0);
	return 0;
}


int
AosCompaction::doFollowerCompaction()
{
	mIsLeader = false;
	startNewThreadToCompact();
	return 0;
}


bool
AosCompaction::needsCompaction()
{
	if (mIsCompacting)
	{
		return false;
	}
	else
	{
		return true;
	}
}


int
AosCompaction::informRaftLeader()
{
	return 0;
}


int
AosCompaction::startNewThreadToCompact()
{
	OmnThrdShellProcPtr thisptr(this, false);
	OmnThreadPool::runProcAsync(thisptr);
	return 0;
}


int
AosCompaction::compactAsRaftLeader()
{
	OmnScreen << "compactAsRaftLeader" << endl;
	mPendingDeletionFileBufferArray = OmnNew AosBuffArray(mCompareFunc, false, true,
			mHeaderCountForCompactionThreshold * mHeader->getHeaderInCacheAndHeaderFileLength());
	mCompactionStartTimestamp = OmnTime::getTimestamp();
	int iRet = 0;
	iRet = findExpiredOldHeaderFiles();
	if (0 != iRet)
	{
		OmnAlarm << "findTheOldestOldHeaderFilesBeyondTimeBoundary failed, iRet=" << iRet << enderr;
		mIsCompacting = false;
		return -1;
	}
	if (0 == mFileSeqVec.size())
	{
		OmnScreen << "0 == mFileSeqVec.size(), no needs to do compaction" << endl;
		mIsCompacting = false;
		return 0;
	}
	iRet = saveMarkToActiveLogFile(AosMarker::eCompactionStart);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eCompactionStart << enderr;
		return -2;
	}
	iRet = processExpiredOldHeaderFiles();
	if (0 != iRet)
	{
		OmnAlarm << "readAndSortExpiredOldHeaderFiles failed, iRet=" << iRet << enderr;
		mIsCompacting = false;
		return -3;
	}
	iRet = processPendingDeletionFile();
	if (0 != iRet)
	{
		OmnAlarm << "processDeletedBlockGroup failed, iRet=" << iRet << enderr;
		mIsCompacting = false;
		return -4;
	}
	iRet = saveMarkToActiveLogFile(AosMarker::eCompactionFinished);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eCompactionFinished << enderr;
		return -5;
	}
	dumpDeletedFileIDs();
	mIsCompacting = false;
	iRet = informRaftThatCompactionHasCompleted();
	if (0 != iRet)
	{
		OmnAlarm << "informRaftThatCompactionHasCompleted failed, iRet=" << iRet << enderr;
		return -6;
	}
	return 0;
}


int
AosCompaction::compactAsRaftFollower()
{
	return 0;
}


bool
AosCompaction::run()
{
	if (mIsCompacting)
	{
		OmnScreen << "doing compaction, return" << endl;
		return true;
	}
	else
	{
		mIsCompacting = true;
	}
	int iRet = 0;
	if (mIsLeader)
	{
		iRet = compactAsRaftLeader();
		if (0 != iRet)
		{
			OmnAlarm << "compactAsRaftLeader failed, iRet=" << iRet << enderr;
			return false;
		}
	}
	else
	{
		iRet = compactAsRaftFollower();
		if (0 != iRet)
		{
			OmnAlarm << "compactAsRaftFollower failed, iRet=" << iRet << enderr;
			return false;
		}
	}
	return true;
}


bool
AosCompaction::procFinished()
{
	return true;
}


int
AosCompaction::findExpiredOldHeaderFiles()
{
	//put expired old header files into mFileSeqVec;
	OmnScreen << "findExpiredOldHeaderFiles" << endl;
	u64 ullTimestampOfTheOldestExistingOldHeaderFile = getTimestampOfTheOldestExistingOldHeaderFileFromMetafile();
	if (0 == ullTimestampOfTheOldestExistingOldHeaderFile)
	{
		OmnAlarm << "getTimestampOfTheOldestExistingOldHeaderFileFromMetafile failed" << enderr;
		return -1;
	}
	u32 ulOldestEpochUnit = mHeaderFileMgr->getEpochUnitByTimestamp(ullTimestampOfTheOldestExistingOldHeaderFile);
	OmnScreen << "ullTimestampOfTheOldestExistingOldHeaderFile:" << ullTimestampOfTheOldestExistingOldHeaderFile
			<< " ulOldestEpochUnit:" << ulOldestEpochUnit << endl;
	if (ulOldestEpochUnit >= mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp))
	{
		OmnScreen << "ulOldestEpochUnit:" << ulOldestEpochUnit
				<< " >= mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp):"
				<< mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp)
				<< " mCompactionStartTimestamp:" << mCompactionStartTimestamp<< endl;
		return 0;
	}
	u64 ullOldestMetaFileRawfid =
			mHeaderFileMgr->getMetafileRawfidByEpochUnit(ulOldestEpochUnit);
	OmnScreen << "checking meta file with rawfid:" << ullOldestMetaFileRawfid
			<< " ulOldestEpochUnit:" << ulOldestEpochUnit << endl;
	while (!mRawSE->fileExists(mRdata, mSiteID, mCubeID, mAseID, ullOldestMetaFileRawfid) &&
			ulOldestEpochUnit < mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp))
	{
		OmnScreen << "checking meta file with rawfid:" << ullOldestMetaFileRawfid
				<< " epoch day:" << ulOldestEpochUnit << endl;
		ulOldestEpochUnit++;	//increase by one day
		ullOldestMetaFileRawfid = mHeaderFileMgr->getMetafileRawfidByEpochUnit(ulOldestEpochUnit);
	}
	int iRet = 0;
	u32 ulEpochUnitBeingChecked = ulOldestEpochUnit;
	OmnScreen << " ulEpochUnitBeingChecked:" << ulEpochUnitBeingChecked
			<< " mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp):"
			<< mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp)
			<< " >= mCompactionTimeWindowInDays:" << mCompactionTimeWindowInUnits << endl;
	while(mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp) -
			ulEpochUnitBeingChecked >= mCompactionTimeWindowInUnits)
	{
		OmnScreen << " ulEpochUnitBeingChecked:" << ulEpochUnitBeingChecked
				<< " mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp):"
				<< mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp)
				<< " >= mCompactionTimeWindowInDays:" << mCompactionTimeWindowInUnits << endl;
		u32 ulSeqnoForFlushing = 0;
		u32 ulSeqnoForWorking = 0;
		std::vector<u32> vExistingSeqno;
		vExistingSeqno.clear();
		iRet = mHeaderFileMgr->getOldHeaderFileSeqnosByEpochUnitFromMetafile(
				mRdata, ulEpochUnitBeingChecked, ulSeqnoForFlushing, ulSeqnoForWorking, vExistingSeqno);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->getOldHeaderFileSeqnosByEpochUnitFromMetafile failed, iRet=" << iRet << enderr;
			return -2;
		}
		for (std::vector<u32>::iterator it = vExistingSeqno.begin(); it != vExistingSeqno.end(); it++)
		{
			mFileSeqVec.push_back(*it);
		}
		ulEpochUnitBeingChecked++;	//increase by one day
	}
	dumpExpiredOldHeaderFileSeqnoVec();
	return 0;
}


int
AosCompaction::processExpiredOldHeaderFiles()
{
	OmnScreen << "processing expired old header files." << endl;
	int iRet = 0;
	for (v_file_t::iterator it = mFileSeqVec.begin();it != mFileSeqVec.end(); it++)
	{
		iRet = processExpiredOldHeaderFile(*it);
		if (0 != iRet)
		{
			OmnAlarm << "processExpiredOldHeaderFileBuff failed, iRet=" << iRet
					<< " old header file seqno:" << *it << enderr;
			return -1;
		}
	}
	mFileSeqVec.clear();
	return 0;
}


int
AosCompaction::processPendingDeletionFile()
{
	OmnScreen << "processing pending deletion file." << endl;
	i64 illStartPos = 0;
	i64 illEndPos = 0;
	int iRet = loadPendingDeletionFileIntoBuff();
	if (0 != iRet)
	{
		OmnAlarm << "loadPendingDeletionFileIntoBuff failed, iRet=" << iRet << enderr;
		return -1;
	}
	OmnScreen << "processing deleted block group with "
			<< mPendingDeletionFileBufferArray->getNumEntries() << " headers" << endl;
	if (0 == mPendingDeletionFileBufferArray->getNumEntries())
	{
		OmnScreen << "0 == mPendingDeletionFileBufferArray->getNumEntries(), no need to processPendingDeletionFile" << endl;
		return 0;
	}
	if (!mPendingDeletionFileBufferArray->sort())
	{
		OmnAlarm << "mPendingDeletionFileBufferArray->sort() failed." << enderr;
		return -2;
	}
	while(illStartPos < mPendingDeletionFileBufferArray->getNumEntries())
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(mPendingDeletionFileBufferArray->getEntry(illStartPos));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " start pos:" << illStartPos << enderr;
			return -3;
		}
		OmnScreen << "body rawfid:" << mHeader->getBodyRawfid() << "@illStartPos:" << illStartPos << endl;
		u64 ullTmpRawfid = mHeader->getBodyRawfid() + 1;
		mHeader->setBodyRawfid(ullTmpRawfid);
		AosBuff tempBuff(mHeader->getHeaderInCacheAndHeaderFileLength() AosMemoryCheckerArgs);
		iRet = mHeader->serializeToCacheAndHeaderFileBuff(tempBuff.data());
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet << enderr;
			return -4;
		}
		illEndPos = mPendingDeletionFileBufferArray->findBoundary(illStartPos, tempBuff.data(), false, false);
		if (0 == illEndPos)
		{
			OmnAlarm << "findBoundary returns 0, ullTmpRawfid:" << ullTmpRawfid
					<< " illStartPos:" << illStartPos << enderr;
			return -4;
		}
		illEndPos--;
		ullTmpRawfid--;	//restore it to the one being compacted
		OmnScreen << "processing rawfid:" << ullTmpRawfid << " with " << illEndPos - illStartPos + 1
				<< " headers" << endl;
		if (0 == ullTmpRawfid)
		{
			illStartPos = illEndPos + 1;
			continue;
		}
		if (!(eActiveLogFileID_end >= ullTmpRawfid && eActiveLogFileID_start <= ullTmpRawfid))
		{
			OmnAlarm << " mHeader->getBodyRawfid():" << ullTmpRawfid
					<< " is out of range:[" << eActiveLogFileID_start << "," << eActiveLogFileID_end  << "]" << enderr;
			return -5;
		}
		iRet = compactRawfile(ullTmpRawfid, illStartPos, illEndPos);
		if (0 != iRet)
		{
			OmnAlarm << "compactRawfile failed, iRet=" << iRet << " rawfid:" << ullTmpRawfid << enderr;
			return -6;
		}
		illStartPos = illEndPos + 1;
	}
	iRet = mPendingDeletionFile->del(mRdata);
	if (0 != iRet)
	{
		OmnAlarm << "mPendingDeletionFile->del failed, iRet=" << iRet
				<< " rawfid:" << mPendingDeletionFile->getRawFileID()<< enderr;
		return -7;
	}
	iRet = mPendingDeletionFile->create(mRdata);
	if (0 != iRet)
	{
		OmnAlarm << "mPendingDeletionFile->create failed, iRet=" << iRet
				<< " rawfid:" << mPendingDeletionFile->getRawFileID() << enderr;
		return -8;
	}
	return 0;
}


int
AosCompaction::appendHeadersToPendingDeletionFile(
		const i64	illStartPos,
		const i64	illEndPos)
{
	int iRet = mPendingDeletionFile->append(mRdata, mPendingDeletionFileBufferArray->getEntry(illStartPos),
			(illEndPos - illStartPos + 1) * mPendingDeletionFileBufferArray->getRecordLen());
	if (0 != iRet)
	{
		OmnAlarm << "mPendingDeletionFile->append failed, iRet=" << iRet
				<< " start pos:" << illStartPos << " end pos:" << illEndPos
				<< " length:" << (illEndPos - illStartPos + 1) * mPendingDeletionFileBufferArray->getRecordLen() << enderr;
		return -1;
	}
	return 0;
}


int
AosCompaction::countDeletedSpaceOfOldHeaderFile(
		const i64	illStartPos,
		const i64	illEndPos,
		u64			&ullDeletedLen)
{
	int iRet = 0;
	ullDeletedLen = 0;
	OmnScreen << "counting deleted len, start pos:" << illStartPos << " end pos:" << illEndPos << endl;
	for (i64 illPos = illStartPos; illPos <= illEndPos; illPos++)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(mPendingDeletionFileBufferArray->getEntry(illPos));
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " pos:" << illPos << enderr;
			return -1;
		}
//		OmnScreen << "mHeader->getBodyRawfid():" << mHeader->getBodyRawfid()
//				<< " mHeader->getBodyLen():" << mHeader->getBodyLen() << endl;
		ullDeletedLen += mHeader->getBodyLen();
	}
	return 0;
}


bool
AosCompaction::fileNeedsCompaction(
		const u64	ullRawfid,
		const u64	ullDeleteLen)
{
	AosRawFilePtr pRawfile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID, ullRawfid, AosRawFile::eReadWrite);
	if (pRawfile.isNull())
	{
		OmnAlarm << "mRawSE->getRawFile failed, siteid:" << mSiteID << " cubeid:" << mCubeID
				<< " aseid:" << mAseID << " rawfid:" << ullRawfid << " type:" << AosRawFile::eModifyOnly << enderr;
		return false;
	}
	int iRet = 0;
	if (!pRawfile->isGood())
	{
		iRet = pRawfile->open(mRdata, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "open rawfile failed, iRet=" << iRet << " rawfid:" << pRawfile->getRawFileID()
					<< " open mode:" << AosRawFile::eReadWrite << enderr;
			return false;
		}
	}
	OmnScreen << "checking whether rawfile needs compaction, rawfid:" << pRawfile->getRawFileID()
			<< " mDeletedPercentageThreshold:" << mDeletedPercentageThreshold << " deleted len:" << ullDeleteLen
			<< " rawfile len:" << pRawfile->getLength() << endl;
	return mDeletedPercentageThreshold <= ullDeleteLen  * 100 / pRawfile->getLength();
}


int
AosCompaction::compactRawfile(
		const u64	ullRawfid,
		const i64	illStartPos,
		const i64	illEndPos)
{
	OmnScreen << "compacting rawfile with rawfid:" << ullRawfid << " start pos:" << illStartPos
			<< " end pos:" << illEndPos << endl;
	int iRet = generateSetOfDeletedOffsets(ullRawfid, illStartPos, illEndPos);
	if (0 != iRet)
	{
		OmnAlarm << "generateSetOfDeletedOffsets failed, iRet=" << iRet << " rawfid:" << ullRawfid
				<< " start pos:" << illStartPos << " end pos:" << illEndPos << enderr;
		return -1;
	}
	AosRawFilePtr pRawfile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID, ullRawfid, AosRawFile::eReadWrite);
	if (pRawfile.isNull())
	{
		OmnAlarm << "mRawSE->getRawFile failed, siteid:" << mSiteID << " cubeid:" << mCubeID
				<< " aseid:" << mAseID << " rawfid:" << ullRawfid << " type:" << AosRawFile::eModifyOnly << enderr;
		return -2;
	}
	if (!pRawfile->isGood())
	{
		iRet = pRawfile->open(mRdata, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "file open failed, iRet=" << iRet << " rawfid:" << pRawfile->getRawFileID()
					<< " open mode:" << AosRawFile::eReadWrite << enderr;
			return -3;
		}
	}
	AosBuffPtr pBuff = OmnNew AosBuff(pRawfile->getLength() AosMemoryCheckerArgs);
	if (pBuff.isNull())
	{
		OmnAlarm << "can not create AosBuff obj for rawfid:" << ullRawfid
				<< " length:" << pRawfile->getLength() << enderr;
		return -4;
	}
	iRet = mRawSE->readRawFile(mRdata, mSiteID, mCubeID, mAseID,
			ullRawfid, 0, pRawfile->getLength(), pBuff, AosRawFile::eReadWrite);
	if (0 != iRet)
	{
		OmnAlarm << "mRawSE->readRawFile failed, iRet=" << iRet << " siteid:" << mSiteID << " cubeid:" << mCubeID
				<< " aseid:" << mAseID << " rawfid:" << ullRawfid << " type:" << AosRawFile::eReadWrite
				<< " offset:" << 0 << " read len:" << pRawfile->getLength() << enderr;
		return -5;
	}
	u64 ullDeletedFileBuffOffset = 0;
	u64 ullNewLogFileRawfid = 0;
	u64 ullNewLogFileOffset = 0;
	BodyEntryGeneralHeader* pBodyEntryGeneralHeader = NULL;
	BodyEntryHead* pBodyEntryHead = NULL;
	iRet = saveMarkToActiveLogFile(AosMarker::eStartToProcessBody, ullRawfid, mActiveCompactedFile->getRawFileID());
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eStartToProcessBody << " rawfid:" << ullRawfid
				<< " new rawfid:" << mActiveCompactedFile->getRawFileID() << enderr;
		return -6;
	}
	while(pRawfile->getLength() > ullDeletedFileBuffOffset)
	{
		if(entryDeleted(ullDeletedFileBuffOffset))
		{
			pBodyEntryHead = (BodyEntryHead*)(pBuff->data() + ullDeletedFileBuffOffset);
			pushOprIDToVec(pBodyEntryHead->ullOprID);
			ullDeletedFileBuffOffset += pBodyEntryHead->ulLength;
			continue;
		}
		pBodyEntryGeneralHeader = (BodyEntryGeneralHeader*)(pBuff->data() + ullDeletedFileBuffOffset);
		iRet = appendNotDeletedEntryToActiveCompactedFile(pBuff->data() + ullDeletedFileBuffOffset,
				pBodyEntryGeneralHeader->ulLength, ullNewLogFileRawfid, ullNewLogFileOffset);
		if (0 != iRet)
		{
			OmnAlarm << "appendNotDeletedEntryToNewFile failed, iRet=" << iRet
					<< " offset:" << ullDeletedFileBuffOffset << " length:" << pBodyEntryGeneralHeader->ulLength << enderr;
			return -7;
		}
		if (eAddModify == pBodyEntryGeneralHeader->ucType)
		{
			pBodyEntryHead = (BodyEntryHead*)(pBuff->data() + ullDeletedFileBuffOffset);
			iRet = mHeader->unserializeFromACLandHCLBuff(
					pBuff->data() + ullDeletedFileBuffOffset + sizeof(BodyEntryHead));
			if (0 != iRet)
			{
				OmnAlarm << "unserializeFromACLandHCLBuff failed, iRet=" << iRet
						<< " DeletedFileBuffOffset:" << ullDeletedFileBuffOffset << enderr;
				return -8;
			}
			iRet = modifyBodyPointerToNewPos(pBodyEntryHead->ullDocid, mHeader->getTimestamp(),
					pRawfile->getRawFileID(), ullDeletedFileBuffOffset, ullNewLogFileRawfid, ullNewLogFileOffset);
			if (0 != iRet)
			{
				OmnAlarm << "modifyBodyPointerToNewPos failed, iRet=" << iRet << " docid:" << pBodyEntryHead->ullDocid
						<< " old body rawfid:" << pRawfile->getRawFileID() << " old body offset:" << ullDeletedFileBuffOffset
						<< " new body rawfid:" << ullNewLogFileRawfid << " new body offset:" << ullNewLogFileOffset
						<< " ullDeletedFileBuffOffset:" << ullDeletedFileBuffOffset << enderr;
				return -9;
			}
		}
		ullDeletedFileBuffOffset += pBodyEntryGeneralHeader->ulLength;
	}
	markFileAsBeingDeleted(ullRawfid);
	iRet = saveMarkToActiveLogFile(AosMarker::eProcessBodyFinished, ullRawfid, mActiveCompactedFile->getRawFileID());
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eProcessBodyFinished << " rawfid:" << ullRawfid
				<< " new rawfid:" << mActiveCompactedFile->getRawFileID() << enderr;
		return -10;
	}
	OmnScreen << "rawfile compacted, rawfid:" << ullRawfid << " start pos:" << illStartPos
			<< " end pos:" << illEndPos << endl;
	return 0;
}


int
AosCompaction::appendNotDeletedEntryToActiveCompactedFile(
		const char*	pBuff,
		const u32	ulLen,
		u64			&ullRawfid,
		u64			&ullOffset)
{
	ullRawfid = mActiveCompactedFile->getRawFileID();
	ullOffset = mActiveCompactedFile->getLength();
	int iRet = mActiveCompactedFile->append(mRdata, pBuff, ulLen);
	if (0 != iRet)
	{
		OmnAlarm << "mNewLogFile->append failed, iRet=" << iRet << " len:" << ulLen << enderr;
		return -1;
	}
	if(isActiveFileBigEnough())
	{
		iRet = switchActiveCompactedFile();
		if (0 != iRet)
		{
			OmnAlarm << "switchNewLogFile failed, iRet=" << iRet << enderr;
			return -3;
		}
	}
	return 0;
}


int
AosCompaction::generateSetOfDeletedOffsets(
		const u64	ullRawfid,
		const i64	illStartPos,
		const i64	illEndPos)
{
	i64 illPos = illStartPos;
	int iRet = 0;
	mDeletedOffsetsSet.clear();
	while(illEndPos >= illPos)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(mPendingDeletionFileBufferArray->getEntry(illPos));
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " pos:" << illPos << enderr;
			return -1;
		}
		if (ullRawfid != mHeader->getBodyRawfid())
		{
			OmnAlarm << "ullRawfid:" << ullRawfid << " != mHeader->getBodyRawfid():" << mHeader->getBodyRawfid() << enderr;
			return -2;
		}
		mDeletedOffsetsSet.insert(mHeader->getBodyOffset());
		illPos++;
	}
	OmnString s;
	s << "dumping mDeletedOffsetsSet for rawfid:" << ullRawfid << "\n";
	for (set_offset_t::iterator it = mDeletedOffsetsSet.begin();it != mDeletedOffsetsSet.end();it++)
	{
		s << *it << "\t";
	}
	OmnScreen << s << endl;
	return 0;
}


bool
AosCompaction::entryDeleted(const u64 ullOffset)
{
	set_offset_t::iterator it = mDeletedOffsetsSet.find(ullOffset);
	return mDeletedOffsetsSet.end() != it;
}


bool
AosCompaction::isActiveFileBigEnough()
{
	return mNewLogFileSizeThreshold <= mActiveCompactedFile->getLength();
}


int
AosCompaction::switchActiveCompactedFile()
{
	u64 ullRawfid = mActiveCompactedFile->getRawFileID() + 1;
	mActiveCompactedFile = mRawSE->createRawFile(mRdata, mSiteID, mCubeID, mAseID,
			ullRawfid, AosMediaType::eDisk, AosRawFile::eModifyOnly);
	if (mActiveCompactedFile.isNull())
	{
		OmnAlarm << "create new log file failed,  siteid:" << mSiteID << " cubeid:" << mCubeID
				<< " aseid:" << mAseID << " rawfid:" << ullRawfid << " type:" << AosRawFile::eModifyOnly << enderr;
		return -1;
	}
	int iRet = saveCompactedFileRawfidToMetafile(ullRawfid);
	if (0 != iRet)
	{
		OmnAlarm << "saveCompactedFileRawfidToMetafile failed, iRet=" << iRet
				<< " rawfid trying to save:" << ullRawfid << enderr;
		return -2;
	}
	return 0;
}


int
AosCompaction::modifyBodyPointerToNewPos(
		const u64	docid,
		const u64	timestamp,
		const u64	ullOldRawfid,
		const u64	ullOldOffset,
		const u64	ullNewRawfid,
		const u64	ullNewOffset)
{
	return mHeaderCache->modifyBodyPointerToNewPos(
			mRdata, docid, timestamp, ullOldRawfid, ullOldOffset, ullNewRawfid, ullNewOffset);
}


void
AosCompaction::markFileAsBeingDeleted(const u64 ullRawfid)
{
	mDeletedFileRawfidVec.push_back(ullRawfid);
}


void
AosCompaction::dumpDeletedFileIDs()
{
	OmnString s;
	s << "dumping deleted file's rawfids.\n";
	for (v_deleted_file_t::iterator it = mDeletedFileRawfidVec.begin(); it != mDeletedFileRawfidVec.end(); it++)
	{
		s << "deleted rawfid:" << *it << "\t";
	}
	OmnScreen << s << endl;
}


void
AosCompaction::pushOprIDToVec(const u64 ullOprID)
{
	mOprIDVec.push_back(ullOprID);
}


u64
AosCompaction::getTimestampOfTheOldestExistingOldHeaderFileFromMetafile()	//not precisely
{
	return mBlobSE->getTimestampOfTheOldestExistingOldHeaderFileFromMetafile(mRdata);
}


int
AosCompaction::saveOldestOldHeaderFileTimestampToMetafile(const u64 ullTimestamp)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	if (buff.isNull())
	{
		OmnAlarm << "failed to create AosBuff obj" << enderr;
		return -1;
	}
	buff->reset();
	buff->appendU64(ullTimestamp);
	int iRet = mRawSE->overwriteRawFile(mRdata, mSiteID, mCubeID,
			mAseID, eMetaFile2ID, buff->data(), buff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mRawSE->overwriteRawFile failed, iRet=" << iRet << " siteid:" << mSiteID
				<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << eMetaFile2ID
				<< " overwrite len:" << buff->dataLen() << enderr;
		return -2;
	}
	return 0;
}


bool
AosCompaction::headerFileExpired(const AosRawFilePtr& pFile)
{
	int iRet = 0;
	if (!pFile->isGood())
	{
		iRet = pFile->open(mRdata, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "file open failed, iRet=" << iRet << " rawfid:" << pFile->getRawFileID()
					<< " open mode:" << AosRawFile::eReadWrite << enderr;
			return false;
		}
	}
	if (0 >= pFile->getLength())
	{
		OmnAlarm << "0 >= pFile->getLength():" << pFile->getLength() << enderr;
		return false;
	}
	AosBuffPtr pBuff = OmnNew AosBuff(pFile->getLength() AosMemoryCheckerArgs);
	if (pBuff.isNull())
	{
		OmnAlarm << "failed to create AosBuff obj, len:" << pFile->getLength() << enderr;
		return false;
	}
	iRet = pFile->read(mRdata, 0, pFile->getLength(), pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "old header file read failed, iRet=" << iRet << " rawfid:" << pFile->getRawFileID()
				<< " read len:" << pFile->getLength() << enderr;
		return false;
	}
	//the old header file consists of old headers most of which are in ascending order judging by timestamp,
	//but the ones at the tail are disordered, so we only look at the first one and the last 100 headers
	//and take the newest one as the indicator
	iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(pBuff->data());
	if (0 != iRet)
	{
		OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff, iRet=" << iRet
				<< " rawfid:" << pFile->getRawFileID() << " pBuff->dataLen():" << pBuff->dataLen() << enderr;
		return false;
	}
	u64 ullTimestamp = mHeader->getTimestamp();
	u32 ulCurrentSec = OmnTime::getCrtSec();
	//TODO:this judging algorithm is a draft that needs more improvement
	if (ulCurrentSec - (u32)(ullTimestamp / 1000000) < mCompactionTimeWindowInUnits)
	{
		OmnScreen << "ulCurrentSec:" << ulCurrentSec << " - (u32)(ullTimestamp >> 32):" << (u32)(ullTimestamp >> 32)
				<< " < mHeaderFileExpirationThresholdInSecs:" << mCompactionTimeWindowInUnits << endl;
		return false;
	}
	for (int i = 0; i < 100; i++)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(
				pBuff->data() + pBuff->dataLen() - i * mHeader->getHeaderInCacheAndHeaderFileLength());
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff, iRet=" << iRet
					<< " rawfid:" << pFile->getRawFileID() << " pBuff->dataLen():" << pBuff->dataLen() << enderr;
			return false;
		}
		ullTimestamp = mHeader->getTimestamp();
		if (ulCurrentSec - (u32)(ullTimestamp >> 32) < mCompactionTimeWindowInUnits)
		{
			OmnScreen << "ulCurrentSec:" << ulCurrentSec << " - (u32)(ullTimestamp >> 32):" << (u32)(ullTimestamp >> 32)
					<< " < mHeaderFileExpirationThresholdInSecs:" << mCompactionTimeWindowInUnits << endl;
			return false;
		}
	}
	return true;
}


u64
AosCompaction::getCompactedFileRawfidFromMetafile()
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	if (buff.isNull())
	{
		OmnAlarm << "failed to create AosBuff obj" << enderr;
		return 0;
	}
	int iRet = mRawSE->readRawFile(mRdata, mSiteID, mCubeID,
			mAseID, eMetaFile1ID, 0, sizeof(u32), buff, AosRawFile::eModifyOnly);
											//TODO:about eModifyOnly, please refer to the implementation
	if (0 != iRet)	//TODO:consider this as being brand new blobse
	{
		AosRawFilePtr ff = mRawSE->createRawFile(mRdata, mSiteID, mCubeID,
			mAseID, eMetaFile1ID, AosMediaType::eDisk, AosRawFile::eModifyOnly);
		if (ff.isNull())
		{
			OmnAlarm << "mRawSE->createRawFile failed,  siteid:" << mSiteID
						<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << eMetaFile1ID << enderr;
			return 0;
		}
		buff->reset();
		buff->appendU64(eComopactedFileID_start);
		iRet = ff->append(mRdata, buff->data(), buff->dataLen());
		if (0 != iRet)
		{
			OmnAlarm << "ff->append failed, iRet=" << iRet << " append len:" << buff->dataLen() << enderr;
			return 0;
		}
		iRet = ff->close();
		if (0 != iRet)
		{
			OmnAlarm << "file close failed, iRet=" << iRet << " rawfid:" << eMetaFile1ID << enderr;
			return 0;
		}
		return eComopactedFileID_start;	//does not need to close, rawfile is closed automatically
	}
	else	//TODO:when read a rawfile with mode eModifyOnly, the return value will be 0 even the file does not exist
	{
		u32 last_fid = buff->getU32(0);
		if (0 == last_fid)	//TODO:the meta file has just been created
		{
			buff->reset();
			buff->appendU64(eComopactedFileID_start);
			iRet = mRawSE->overwriteRawFile(mRdata, mSiteID, mCubeID,
					mAseID, eMetaFile1ID, buff->data(), buff->dataLen());
			if (0 != iRet)
			{
				OmnAlarm << "mRawSE->overwriteRawFile failed, iRet=" << iRet << " siteid:" << mSiteID
						<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << eMetaFile1ID
						<< " overwrite len:" << buff->dataLen() << enderr;
				return -5;
			}
			return eComopactedFileID_start;
		}
		if (0 >= last_fid)
		{
			OmnAlarm << "0 >= last_fid:" << last_fid << enderr;
			return 0;
		}
		return last_fid;
	}
}


int
AosCompaction::saveCompactedFileRawfidToMetafile(const u64 ullRawfid)
{
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	if (buff.isNull())
	{
		OmnAlarm << "failed to create AosBuff obj" << enderr;
		return -1;
	}
	buff->reset();
	buff->appendU64(ullRawfid);
	int iRet = mRawSE->overwriteRawFile(mRdata, mSiteID, mCubeID,
			mAseID, eMetaFile1ID, buff->data(), buff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mRawSE->overwriteRawFile failed, iRet=" << iRet << " siteid:" << mSiteID
				<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << eMetaFile1ID
				<< " overwrite len:" << buff->dataLen() << enderr;
		return -2;
	}
	return 0;
}


int
AosCompaction::openActiveCompactedFile()
{
	u64 ullActiveCompactedFileID = getCompactedFileRawfidFromMetafile();
	mActiveCompactedFile = mRawSE->getRawFile(mRdata,
			mSiteID, mCubeID, mAseID, ullActiveCompactedFileID, AosRawFile::eModifyOnly);
	if (mActiveCompactedFile.isNull())
	{
		OmnAlarm << "mRawSE->getRawFile failed, siteid:" << mSiteID
				<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << ullActiveCompactedFileID << enderr;
		return -1;
	}
	return 0;
}


void
AosCompaction::timeout(
    const int		timerId,
    const OmnString	&timerName,
    void			*parm)
{
    OmnScreen << "timeout" << endl;
    if (!mIsCompacting && !mHeaderCache->isFlushingDirtyHeaders())
    {
    	startNewThreadToCompact();
    	return;
    }
    else
    {
        OmnScreen << "timeout but is doing compaction or flush dirty headers, wait for another "
        		<< mCompactionBackoffTime << " seconds." << endl;
        OmnTimerObjPtr thisptr(this, false);
        OmnTimer::getSelf()->startTimer("CompactionTimer", mCompactionBackoffTime, 0, thisptr, 0);
    }
}


int
AosCompaction::informRaftThatCompactionHasCompleted()
{
	OmnScreen << "informRaftThatCompactionHasCompleted" << endl;
	return 0;
}


int
AosCompaction::loadPendingDeletionFileIntoBuff()
{
	int iRet = 0;
	if (!mPendingDeletionFile->isGood())
	{
		iRet = mPendingDeletionFile->open(mRdata, AosRawFile::eModifyOnly);
		if (0 != iRet)
		{
			OmnAlarm << "mPendingDeletionFile->open failed, iRet=" << iRet << " open mode:" << AosRawFile::eModifyOnly
					<< " rawfid:" << mPendingDeletionFile->getRawFileID() << enderr;
			return -1;
		}
	}
	if (0 == mPendingDeletionFile->getLength())	//the first time of compaction
	{
		OmnScreen << "pending deletion file not created yet." << endl;
		return 0;
	}
	OmnScreen << "loading pending deletion file with length:" << mPendingDeletionFile->getLength() << endl;
	AosBuffPtr pBuff = OmnNew AosBuff(mPendingDeletionFile->getLength() AosMemoryCheckerArgs);
	iRet = mPendingDeletionFile->read(mRdata, 0, mPendingDeletionFile->getLength(), pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mPendingDeletionFile->read failed, iRet=" << iRet
				<< " read len:" << mPendingDeletionFile->getLength()<< enderr;
		return -2;
	}
	i64 illOffset = 0;
	bool rslt = false;
	while(pBuff->dataLen() > illOffset)
	{
		rslt = mPendingDeletionFileBufferArray->appendEntry(
				pBuff->data() + illOffset, mHeader->getHeaderInCacheAndHeaderFileLength(), mRdata);
		if (!rslt)
		{
			OmnAlarm << "mExpiredOldHeaderFileBuffer->appendEntry failed, offset:" << illOffset
					<< " append len:" << mHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
			return -3;
		}
		illOffset += mHeader->getHeaderInCacheAndHeaderFileLength();
	}
	return 0;
}


//int
//AosCompaction::saveMarkToActiveLogFile(AosBodyEntry::EntryType eType)
//{
//	AosBuffPtr buff = OmnNew AosBuff(sizeof(AosBodyEntry::OperationEntry));
//	if (buff.isNull())
//	{
//		OmnAlarm << "new buff failed for saving header mark." << enderr;
//		return -1;
//	}
//	AosBodyEntry::OperationEntry* pCompactionEntry = (AosBodyEntry::OperationEntry*)buff->data();
//	pCompactionEntry->sGeneralHead.ulLength = sizeof(AosBodyEntry::OperationEntry);
//	pCompactionEntry->sGeneralHead.ucType = eType;
//	pCompactionEntry->ulLengthTail = pCompactionEntry->sGeneralHead.ulLength;
//	u64 ullOffset = 0;
//	u64 ullRawfid = 0;
//	int iRet = mBlobSE->appendDataToActiveLogFile(
//			mRdata, buff->data(), sizeof(AosBodyEntry::OperationEntry), ullRawfid, ullOffset);
//	if (0 != iRet)
//	{
//		OmnAlarm << "save OperationEntry entry to Active Log File failed, iRet=" << iRet
//				<< " rawfid:" << ullRawfid << enderr;
//		return -2;
//	}
//	return 0;
//}


int
AosCompaction::saveMarkToActiveLogFile(
		AosMarker::MarkerType	eType,
		const u64				ullBeingCompacted,
		const u64				ullCompacted)
{
	AosMarker marker(eType);
	switch (eType)
	{
	case AosMarker::eCompactionStart:
	case AosMarker::eCompactionFinished:
		break;
	case AosMarker::eStartToProcessHeader:
	case AosMarker::eProcessHeaderFinished:
		marker.setSeqnoBeingCompacted((u32)ullBeingCompacted);
		marker.setSeqnoCompacted((u32)ullCompacted);
		break;
	case AosMarker::eStartToProcessBody:
	case AosMarker::eProcessBodyFinished:
		marker.setBodyRawfidBeingCompacted(ullBeingCompacted);
		marker.setBodyRawfidCompacted(ullCompacted);
		break;
	default:
		OmnAlarm << "Unknown entry type:" << eType << enderr;
		return -1;
		break;
	}
	int iRet = mBlobSE->saveMarkerToMarkerFile(mRdata, marker);
	if (0 != iRet)
	{
		OmnAlarm << "save OperationEntry entry to Active Log File failed, iRet=" << iRet << enderr;
		return -2;
	}
	return 0;
}


bool
AosCompaction::headerValid(const AosBlobHeaderPtr	&pHeader)
{
	if (AosBlobHeader::noNextHeader == pHeader->getPrevHeaderPtr() ||
			AosBlobHeader::unresolvedNextHeader == pHeader->getPrevHeaderPtr())
	{
		OmnAlarm << "illegal pHeader->getPrevHeaderPtr():" << pHeader->getPrevHeaderPtr().toStr() << enderr;
		return true;	//due to the true/false return type issue, we let it pass so that it will fail
						//in update previous header's next header pointer phase
	}
	if (AosBlobHeader::noPrevHeader == pHeader->getPrevHeaderPtr())
		//means that the header is the first old header on disk, so we need to judge from normal header on disk's
		//timestamp
	{
		AosBlobHeaderPtr pNormalHeaderInFile = mHeaderFileMgr->readNormalHeader(mRdata, pHeader->getDocid());
		if (pNormalHeaderInFile.isNull())
		{
			OmnAlarm << "read normal header from file failed, docid:" << pHeader->getDocid() << enderr;
			return true;	//due to the true/false return type issue, we let it pass so that it will fail
							//in update previous header's next header pointer phase
		}
		if (mHeaderFileMgr->getEpochUnitByTimestamp(pNormalHeaderInFile->getTimestamp()) + mCompactionTimeWindowInUnits
				> mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp))
		{
//			OmnScreen << "mHeaderFileMgr->getEpochUnitByTimestamp(pNormalHeaderInFile->getTimestamp():"
//					<< pNormalHeaderInFile->getTimestamp() << "):" << mHeaderFileMgr->getEpochUnitByTimestamp(pNormalHeaderInFile->getTimestamp())
//					<< " + mCompactionTimeWindowInDays:" << mCompactionTimeWindowInDays
//					<< " > mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp):"
//					<< mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp)
//					<< " mCompactionStartTimestamp:" << mCompactionStartTimestamp << endl;
//			pHeader->dump();
//			pNormalHeaderInFile->dump();
			return true;
		}
		else	//update normal header on disk's next header pointer
		{
			return false;
		}
		return true;
	}
	return mHeaderFileMgr->getEpochUnitBySeqno(pHeader->getPrevHeaderPtr().ulSeqno) + mCompactionTimeWindowInUnits
			> mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp);
}


int
AosCompaction::processExpiredOldHeaderFile(const u32 ulSeqno)
{
	OmnScreen << "processing expired old header file with seqno:" << ulSeqno
			<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << endl;
	u32 ulNewSeqno = mHeaderBuffAndFileMgr->getMinAvailableSeqnoOfEpochUnit(
					mRdata, mHeaderFileMgr->getEpochUnitBySeqno(ulSeqno));
	if (0 == ulNewSeqno)
	{
		OmnAlarm << "getMinAvailableOverflowIDOfEpochUnit failed, seqno:" << ulSeqno
				<< " epoch day:" << mHeaderFileMgr->getEpochUnitBySeqno(ulSeqno) << enderr;
		return -1;
	}
	int iRet = saveMarkToActiveLogFile(AosMarker::eStartToProcessHeader, ulSeqno, ulNewSeqno);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eStartToProcessHeader << " seqno:" << ulSeqno << " new seqno:" << ulNewSeqno << enderr;
		return -2;
	}
	u64 ullBuffLen = 2 * AosHeaderFileMgr::eOldHeaderFileSizeThreshold;	//TODO:needs a better way to determine this size
	AosBuffPtr pTempBuff = OmnNew AosBuff(ullBuffLen AosMemoryCheckerArgs);
	if (pTempBuff.isNull())
	{
		OmnAlarm << "can not create AosBuff obj in processExpiredOldHeaderFile, bufflen:" << ullBuffLen << enderr;
		return -3;
	}
	OmnScreen << "loading expired old header file with seqno:" << ulSeqno
			<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << endl;
	AosRawFilePtr pOldHeaderFile = mHeaderFileMgr->getOldHeaderFileBySeqno(mRdata, ulSeqno);
	if (pOldHeaderFile.isNull())
	{
		OmnAlarm << "mHeaderFileMgr->getOldHeaderFileBySeqno failed, seqno:" << ulSeqno
				<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << enderr;
		return -4;
	}
	iRet = pOldHeaderFile->open(mRdata, AosRawFile::eReadWrite);
	if (0 != iRet)
	{
		OmnAlarm << "old header file open failed, iRet=" << iRet << " seqno:" << ulSeqno << " rawfid:"
				<< mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << " mode:" << AosRawFile::eReadWrite << enderr;
		return -5;
	}
	if (0 != pOldHeaderFile->getLength() % mHeader->getHeaderInCacheAndHeaderFileLength())
	{
		OmnAlarm << "0 != pOldHeaderFile->getLength():" << pOldHeaderFile->getLength()
				<< " % mHeader->getHeaderInCacheAndHeaderFileLength():"
				<< mHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
		return -6;
	}
	if (pOldHeaderFile->getLength() > ullBuffLen)
	{
		bool rslt = pTempBuff->expandMemory1(pOldHeaderFile->getLength());
		if (!rslt)
		{
			OmnAlarm << "pTempBuff->expandMemory1 failed, length:" << pOldHeaderFile->getLength() << enderr;
			return -7;
		}
	}
	iRet = pOldHeaderFile->read(mRdata, 0, pOldHeaderFile->getLength(), pTempBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pOldHeaderFile->read failed, iRet=" << iRet << " seqno:" << ulSeqno
				<< " rawfid:" << pOldHeaderFile->getRawFileID()
				<< " read len:" << pOldHeaderFile->getLength() << enderr;
		return -8;
	}
	if ((u64)pTempBuff->dataLen() != pOldHeaderFile->getLength())
	{
		OmnAlarm << "pOldHeaderFile->read failed, iRet=" << iRet << " rawfid:" << pOldHeaderFile->getRawFileID()
				<< " read len:" << pOldHeaderFile->getLength() << enderr;
		return -9;
	}
	i64 illOffset = 0;
	while(pTempBuff->dataLen() > illOffset)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(pTempBuff->data() + illOffset);
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " header file seqno:" << ulSeqno << " offset:" << illOffset << enderr;
			return -10;
		}
		if (headerValid(mHeader))
		{
			iRet = copyHeaderToNewOldHeaderFile(mHeader);
			if (0 != iRet)
			{
				OmnAlarm << "copyHeaderToCompactionTempHeaderFile failed, iRet=" << iRet << enderr;
				return -11;
			}
		}
		else
		{
			OmnScreen << "invalid header, body rawfid:" << mHeader->getBodyRawfid()
					<< " offset:" << mHeader->getBodyOffset() << endl;
			iRet = copyHeaderToPendingDeletionFileBuff(mHeader);
			if (0 != iRet)
			{
				OmnAlarm << "copyHeaderToCompactionTempHeaderFile failed, iRet=" << iRet << enderr;
				return -12;
			}
		}
		illOffset += mHeader->getHeaderInCacheAndHeaderFileLength();
	}
	if (mNewOldHeaderFileBuff->dataLen() > 0)
	{
		iRet = saveNewOldHeaderFileBuff(ulSeqno, ulNewSeqno);
		if (0 != iRet)
		{
			OmnAlarm << "saveTempOldHeaderFileBuff failed, iRet=" << iRet << enderr;
			return -13;
		}
		iRet = updateHeaderPointers(ulNewSeqno);
		if (0 != iRet)
		{
			OmnAlarm << "updateHeaderPointers failed, iRet=" << iRet << " seqno:" << ulSeqno << enderr;
			return -14;
		}
		mNewOldHeaderFileBuff->setDataLen(0);
	}
	iRet = pOldHeaderFile->del(mRdata);
	if (0 != iRet)
	{
		OmnAlarm << "pOldHeaderFile->del failed, iRet=" << iRet
				<< " rawfid:" << pOldHeaderFile->getRawFileID() << enderr;
		return -15;
	}
	OmnScreen << "old header file deleted, seqno:" << ulSeqno
			<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << endl;
	iRet = flushPendingDeletionFileBuffAndSetNextHeaderPointerOfPrevHeaderToNull();
	if (0 != iRet)
	{
		OmnAlarm << "flushPendingDeletionFileBuff failed, iRet=" << iRet << enderr;
		return -16;
	}
	iRet = mHeaderBuffAndFileMgr->delSeqnoInEpochUnitInfo(mRdata, ulSeqno);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderBuffAndFileMgr->delSeqnoInEpochUnitInfo failed, iRet=" << iRet << " seqno:" << ulSeqno << enderr;
		return -17;
	}
	iRet = saveMarkToActiveLogFile(AosMarker::eProcessHeaderFinished, ulSeqno, ulNewSeqno);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << " mark type:"
				<< AosMarker::eProcessHeaderFinished << " seqno:" << ulSeqno << " new seqno:" << ulNewSeqno << enderr;
		return -18;
	}
	return 0;
}


int
AosCompaction::copyHeaderToNewOldHeaderFile(const AosBlobHeaderPtr	&pHeader)
{
	if (mNewOldHeaderFileBuff->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength()
			> mNewOldHeaderFileBuff->buffLen())
	{
		bool rslt = mNewOldHeaderFileBuff->expandMemory1(mNewOldHeaderFileBuff->buffLen() * 2);
		if (!rslt)
		{
			OmnAlarm << "mNewOldHeaderFileBuff->expandMemory1 failed, length:"
					<< mNewOldHeaderFileBuff->buffLen() * 2 << enderr;
			return -1;
		}
	}
	pHeader->setNextHeader(AosBlobHeader::noNextHeader);	//valid header should be the only one that exceeds time window
	int iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			mNewOldHeaderFileBuff->data() + mNewOldHeaderFileBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " mNewOldHeaderFileBuff->dataLen():" << mNewOldHeaderFileBuff->dataLen() << enderr;
		return -2;
	}
	mNewOldHeaderFileBuff->setDataLen(
			mNewOldHeaderFileBuff->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength());
	return 0;
}


int
AosCompaction::copyHeaderToPendingDeletionFileBuff(const AosBlobHeaderPtr	&pHeader)
{
	if (mPendingDeletionFileBuff->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength()
			> mPendingDeletionFileBuff->buffLen())
	{
		bool rslt = mPendingDeletionFileBuff->expandMemory1(mPendingDeletionFileBuff->buffLen() * 2);
		if (!rslt)
		{
			OmnAlarm << "mPendingDeletionFileBuff->expandMemory1 failed, length:"
					<< mPendingDeletionFileBuff->buffLen() * 2 << enderr;
			return -1;
		}
	}
	int iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			mPendingDeletionFileBuff->data() + mPendingDeletionFileBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " mPendingDeletionFileBuff->dataLen():" << mPendingDeletionFileBuff->dataLen() << enderr;
		return -2;
	}
	mPendingDeletionFileBuff->setDataLen(
			mPendingDeletionFileBuff->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength());
	return 0;
}


int
AosCompaction::saveNewOldHeaderFileBuff(
		const u32	ulSeqno,
		const u32	ulNewSeqno)
{
	OmnScreen << "saveTempOldHeaderFileBuff from seqno:" << ulSeqno << " rawfid:"
			<< mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulSeqno) << " to seqno:" << ulNewSeqno << " rawfid:"
			<< mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulNewSeqno) << endl;
	AosRawFilePtr pTempOldHeaderFile = mRawSE->createRawFile(mRdata, mSiteID, mCubeID, mAseID,
			mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulNewSeqno), AosMediaType::eDisk, AosRawFile::eModifyOnly);
	if (pTempOldHeaderFile.isNull())
	{
		OmnAlarm << "createRawFile failed, site id:" << mSiteID << " cube id:" << mCubeID << " ase id:" << mAseID
				<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulNewSeqno)
				<< " seqno:" << ulNewSeqno << enderr;
		return -2;
	}
	int iRet = pTempOldHeaderFile->append(mRdata, mNewOldHeaderFileBuff->data(), mNewOldHeaderFileBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mTempOldHeaderFile->append failed, iRet=" << iRet
				<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(ulNewSeqno)
				<< " mNewOldHeaderFileBuff->dataLen():" << mNewOldHeaderFileBuff->dataLen() << enderr;
		return -3;
	}
	return 0;
}


int
AosCompaction::flushPendingDeletionFileBuffAndSetNextHeaderPointerOfPrevHeaderToNull()
{
	if (0 >= mPendingDeletionFileBuff->dataLen())
	{
		OmnScreen << "0 >= mPendingDeletionFileBuff->dataLen():" << mPendingDeletionFileBuff->dataLen() << endl;
		return 0;
	}
	int iRet = mPendingDeletionFile->append(
			mRdata, mPendingDeletionFileBuff->data(), mPendingDeletionFileBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mPendingDeletionFile->append failed, iRet=" << iRet
				<< " mPendingDeletionFileBuff->dataLen():" << mPendingDeletionFileBuff->dataLen() << enderr;
		return -1;
	}
	iRet = updateHeaderPointersForDeletedHeaders();
	if (0 != iRet)
	{
		OmnAlarm << "updateHeaderPointersForDeletedHeaders failed, iRet=" << iRet << enderr;
		return -2;
	}
	mPendingDeletionFileBuff->setDataLen(0);
	return 0;
}


int
AosCompaction::replaceOldHeaderFileWithTempOldHeaderFile(const u32 ulSeqno)
{
//	AosRawFilePtr pOldHeaderFile = mHeaderFileMgr->getOldHeaderFileBySeqno(mRdata, ulSeqno);
//	if (pOldHeaderFile.isNull())
//	{
//		OmnAlarm << "mHeaderFileMgr->getOldHeaderFileBySeqno failed, seqno:" << ulSeqno << enderr;
//		return -1;
//	}
//	int iRet = rename(mTempOldHeaderFile->getFileNameWithFullPath().data(),
//			pOldHeaderFile->getFileNameWithFullPath().data());
//	if (0 != iRet)
//	{
//		OmnAlarm << "rename failed, iRet=" << iRet
//				<< " old name:" << mTempOldHeaderFile->getFileNameWithFullPath().data()
//				<< " new name:" << pOldHeaderFile->getFileNameWithFullPath().data() << enderr;
//		return -2;
//	}
//	iRet = createNewTempOldHeaderFile();
//	if (0 != iRet)
//	{
//		OmnAlarm << "createNewTempOldHeaderFile failed, iRet=" << iRet << enderr;
//		return -3;
//	}
	return 0;
}


int
AosCompaction::updateHeaderPointers(const u32	ulSeqno)
{
	OmnScreen << "updateHeaderPointers to new seqno:" << ulSeqno << endl;
	i64 illOffset = 0;
	int iRet = 0;
	u32 ulNormalHeaderFileSeqno = 0;
	AosBuffPtr pNormalHeaderFileBuff = NULL;
	while(mNewOldHeaderFileBuff->dataLen() > illOffset)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(mNewOldHeaderFileBuff->data() + illOffset);
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " header file seqno:" << ulSeqno << " offset:" << illOffset << enderr;
			return -1;
		}
		AosBlobHeader::HeaderPointer sHeaderPointer = {ulSeqno, (u32)illOffset};
		if (mHeader->getPrevHeaderPtr() == AosBlobHeader::noPrevHeader)
		{
			iRet = updateNextHeaderPointerOfNormalHeaderInFile(mHeader->getDocid(), sHeaderPointer,
					ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
			if (0 != iRet)
			{
				OmnAlarm << "updateNextHeaderPointerNormalHeaderInFile failed, iRet=" << iRet
						<< " docid:" << mHeader->getDocid() << " header pointer:" << sHeaderPointer.toStr() << enderr;
				return -2;
			}
		}
		else
		{
//		iRet = mDirtyHeaderCache->updateNextHeaderPointerOfPrevHeader(
//				mRdata, sHeaderPointer, mHeader, bNeedsToUpdateOldHeader);
//		if (0 != iRet)
//		{
//			OmnAlarm << "mDirtyHeaderCache->updateNextHeaderPointerOfPrevHeader failed, iRet=" << iRet << enderr;
//			return -2;
//		}
//		if (bNeedsToUpdateOldHeader)
//		{
			iRet = mOldHeaderCache->updateNextHeaderPointer(
					mRdata, mHeader->getDocid(), mHeader->getPrevHeaderPtr(), sHeaderPointer);
			if (0 != iRet)
			{
				OmnAlarm << "mOldHeaderCache->updateNextHeaderPointer failed, iRet=" << iRet
						<< " header@" << sHeaderPointer.toStr() << enderr;
				return -3;
			}
		}
		illOffset += mHeader->getHeaderInCacheAndHeaderFileLength();
	}
	if (pNormalHeaderFileBuff.notNull())
	{
		iRet = mHeaderFileMgr->overwriteNormalHeaderFile(mRdata, ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->overwriteNormalHeaderFile failed, iRet=" << iRet << enderr;
			return -4;
		}
	}
	OmnScreen << "updateHeaderPointers to new seqno:" << ulSeqno << " finished"<< endl;
	return 0;
}


int
AosCompaction::createNewTempOldHeaderFile()
{
//	int iRet = 0;
//	if (mRawSE->fileExists(mRdata, mSiteID, mCubeID, mAseID, eTempOldHeaderFileID))
//	{
//		iRet = mRawSE->deleteFile(mRdata, mSiteID, mCubeID, mAseID, eTempOldHeaderFileID);
//		if (0 != iRet)
//		{
//			OmnAlarm << "mRawSE->deleteFile failed, iRet=" << iRet << enderr;
//			return -1;
//		}
//	}
//	mTempOldHeaderFile = mRawSE->createRawFile(
//			mRdata, mSiteID, mCubeID, mAseID, eTempOldHeaderFileID, AosMediaType::eNVRAM, AosRawFile::eReadWrite);
//	if (mTempOldHeaderFile.isNull())
//	{
//		OmnAlarm << "create mTempOldHeaderFile failed, siteid:" << mSiteID << " cubeid:" << mCubeID
//				<< " aseid:" << mAseID << "rawfid:" << eTempOldHeaderFileID << enderr;
//		return -2;
//	}
	return 0;
}


void
AosCompaction::dumpExpiredOldHeaderFileSeqnoVec()
{
	OmnScreen << "dumping mFileSeqVec with size:" << mFileSeqVec.size() << endl;
	for (v_file_t::iterator it = mFileSeqVec.begin();it != mFileSeqVec.end();it++)
	{
		OmnScreen << "expired old header file seqno:" << *it
				<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(*it) << endl;
	}
}


int
AosCompaction::updateNextHeaderPointerOfNormalHeaderInFile(
		const u64							docid,
		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer)
{
	AosBlobHeaderPtr pNormalHeaderInFile = OmnNew AosBlobHeader(docid, mHeader->getCustomDataSize());
	if (pNormalHeaderInFile.isNull())
	{
		OmnAlarm << "create header object failed, docid:" << docid << enderr;
		return -1;
	}
	u64 ullNormalHeaderFileRawfid = mHeaderFileMgr->getNormalHeaderFileRawfidByDocid(docid);
	AosRawFilePtr pNormalHeaderFile = mRawSE->getRawFile(
			mRdata, mSiteID, mCubeID, mAseID, ullNormalHeaderFileRawfid, AosRawFile::eReadWrite);
	if (pNormalHeaderFile.isNull())
	{
		OmnAlarm << "get normal header file object failed, rawfid:" << ullNormalHeaderFileRawfid
				<< " site id:" << mSiteID << " cube id:" << mCubeID << " ase id:" << mAseID
				<< " docid:" << docid << enderr;
		return -2;
	}
	pNormalHeaderFile->lock();
	AosBuffPtr pNormalHeaderFileBuff = OmnNew AosBuff(mHeaderFileMgr->getNormalHeaderFileSize() AosMemoryCheckerArgs);
	if (pNormalHeaderFileBuff.isNull())
	{
		OmnAlarm << "pNormalHeaderFileBuff.isNull()" << enderr;
		pNormalHeaderFile->unlock();
		return -3;
	}
	int iRet = pNormalHeaderFile->read(mRdata, 0, mHeaderFileMgr->getNormalHeaderFileSize(), pNormalHeaderFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "read normal header file failed, iRet=" << iRet << " rawfid:" << ullNormalHeaderFileRawfid
				<< " read len:" << mHeaderFileMgr->getNormalHeaderFileSize() << enderr;
		pNormalHeaderFile->unlock();
		return -4;
	}
	iRet = pNormalHeaderInFile->unserializeFromCacheAndHeaderFileBuff(
			pNormalHeaderFileBuff->data() + mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid));
	if (0 != iRet)
	{
		OmnAlarm << "pNormalHeaderInFile->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " offset:" << mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid) << enderr;
		pNormalHeaderFile->unlock();
		return -5;
	}
	pNormalHeaderInFile->setNextHeader(sNextHeaderPointer);
	iRet = pNormalHeaderInFile->serializeToCacheAndHeaderFileBuff(
			pNormalHeaderFileBuff->data() + mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid));
	if (0 != iRet)
	{
		OmnAlarm << "pNormalHeaderInFile->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " offset:" << mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid) << enderr;
		pNormalHeaderFile->unlock();
		return -6;
	}
	iRet = pNormalHeaderFile->overwrite(mRdata, pNormalHeaderFileBuff->data(), mHeaderFileMgr->getNormalHeaderFileSize());
	if (0 != iRet)
	{
		OmnAlarm << "overwrite normal header file failed, iRet=" << iRet
				<< " overwrite len:" << mHeaderFileMgr->getNormalHeaderFileSize() << enderr;
		pNormalHeaderFile->unlock();
		return -7;
	}
	pNormalHeaderFile->unlock();
	return 0;
}


int
AosCompaction::updateNextHeaderPointerOfNormalHeaderInFile(
		const u64							docid,
		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer,
		u32									&ulNormalHeaderFileSeqno,
		AosBuffPtr							&pNormalHeaderFileBuff)
{
	int iRet = 0;
	bool bNormalHeaderFileExists = false;
	if (mHeaderFileMgr->getNormalHeaderFileSeqnoByDocid(docid) != ulNormalHeaderFileSeqno)
	{
		iRet = mHeaderCache->reloadNormalHeaderFileBuff(mRdata, docid, ulNormalHeaderFileSeqno,
				pNormalHeaderFileBuff, bNormalHeaderFileExists);
		if (0 != iRet)
		{
			OmnAlarm << "reloadNormalHeaderFileBuff failed, iRet=" << iRet << " docid:" << docid
					<< " ullNormalHeaderFileSeqno:" << ulNormalHeaderFileSeqno << enderr;
			return -1;
		}
	}
	AosBlobHeaderPtr pNormalHeaderInFile = OmnNew AosBlobHeader(docid, mHeader->getCustomDataSize());
	if (pNormalHeaderInFile.isNull())
	{
		OmnAlarm << "create header object failed, docid:" << docid << " ulNormalHeaderFileSeqno:"
				<< ulNormalHeaderFileSeqno << enderr;
		return -2;
	}
	iRet = pNormalHeaderInFile->unserializeFromCacheAndHeaderFileBuff(
			pNormalHeaderFileBuff->data() + mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid));
	if (0 != iRet)
	{
		OmnAlarm << "pNormalHeaderInFile->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " ulNormalHeaderFileSeqno:" << ulNormalHeaderFileSeqno
				<< " offset:" << mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid) << enderr;
		return -3;
	}
	pNormalHeaderInFile->setNextHeader(sNextHeaderPointer);
	iRet = mHeaderCache->updateNormalHeaderInCache(pNormalHeaderInFile);
	if (0 != iRet)
	{
		OmnAlarm << "updateNormalHeaderInCache failed, iRet=" << iRet
				<< " ulNormalHeaderFileSeqno:" << ulNormalHeaderFileSeqno << enderr;
		return -4;
	}
	iRet = pNormalHeaderInFile->serializeToCacheAndHeaderFileBuff(
			pNormalHeaderFileBuff->data() + mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid));
	if (0 != iRet)
	{
		OmnAlarm << "pNormalHeaderInFile->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " offset:" << mHeaderFileMgr->getNormalHeaderOffsetByDocid(docid) << enderr;
		return -5;
	}
	return 0;
}


int
AosCompaction::updateHeaderPointersForDeletedHeaders()
{
	i64 illOffset = 0;
	int iRet = 0;
	u32 ulNormalHeaderFileSeqno = 0;
	AosBuffPtr pNormalHeaderFileBuff = NULL;
	while(mPendingDeletionFileBuff->dataLen() > illOffset)
	{
		iRet = mHeader->unserializeFromCacheAndHeaderFileBuff(mPendingDeletionFileBuff->data() + illOffset);
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet << " offset:" << illOffset << enderr;
			return -1;
		}
		if (AosBlobHeader::noPrevHeader == mHeader->getPrevHeaderPtr())
		{
			iRet = updateNextHeaderPointerOfNormalHeaderInFile(mHeader->getDocid(), AosBlobHeader::noNextHeader,
					ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
			if (0 != iRet)
			{
				OmnAlarm << "updateNextHeaderPointerNormalHeaderInFile failed, iRet=" << iRet << " docid:"
						<< mHeader->getDocid() << " header pointer:" << AosBlobHeader::noNextHeader.toStr() << enderr;
				return -2;
			}
		}
		else if (mHeaderFileMgr->getEpochUnitBySeqno(mHeader->getPrevHeaderPtr().ulSeqno) + mCompactionTimeWindowInUnits
			> mHeaderFileMgr->getEpochUnitByTimestamp(mCompactionStartTimestamp))
		{
			iRet = mOldHeaderCache->updateNextHeaderPointer(
					mRdata, mHeader->getDocid(), mHeader->getPrevHeaderPtr(), AosBlobHeader::noNextHeader);
			if (0 != iRet)
			{
				OmnAlarm << "mOldHeaderCache->updateNextHeaderPointer failed, iRet=" << iRet
						<< " header@" << AosBlobHeader::noNextHeader.toStr() << enderr;
				return -3;
			}
		}
		illOffset += mHeader->getHeaderInCacheAndHeaderFileLength();
	}
	if (pNormalHeaderFileBuff.notNull())
	{
		iRet = mHeaderFileMgr->overwriteNormalHeaderFile(mRdata, ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->overwriteNormalHeaderFile failed, iRet=" << iRet << enderr;
			return -4;
		}
	}
	return 0;
}
