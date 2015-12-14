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
#include "BlobSE/HeaderChangeLog.h"

#include "alarm_c/alarm.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/DeletionLog.h"
#include "BlobSE/Entry.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/RawfidRange.h"
#include "BlobSE/Ptrs.h"
#include "Debug/Except.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "Porting/Sleep.h"
#include "RawSE/RawSE.h"
#include "Thread/ThreadPool.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util1/Timer.h"
#include "Util/CompareFun.h"
#if 0
AosHeaderChangeLog::AosHeaderChangeLog(
		AosRundata			*rdata,
		AosRawSE 			*rawse,
		AosBlobSE			*blob_se,
		AosHeaderFileMgr*		header_file,
		AosRawFile*			active_rawfile,
		const u32			header_size,
		const AosXmlTagPtr	&conf)
:
OmnThrdShellProc("BlobSEHCL"),
mLastSavedDeltaPos(0),
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mSaveLockRaw(OmnNew OmnMutex()),
mSaveLock(mSaveLockRaw),
mRdata(rdata),
mCompareFunc(OmnNew AosFunCustom(sizeof(HeaderChangeLogEntry) + header_size, false, conf->getFirstChild("HeaderChangeLog", true))),
mChangeLogBuffArray(OmnNew AosBuffArray(mCompareFunc, false, true, eHeaderChageLogMaxSize)),	//TODO:args
mIsSavingDelta(false),
mIsFlushingHeaders(false),
mHeaderFile(header_file),
mActiveRawfile(active_rawfile)
{
	if (!rawse)
	{
		rdata->setErrmsg("rawse is NULL");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mRawSE = rawse;
	if (!blob_se)
	{
		rdata->setErrmsg("blob_se is NULL");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mBlobSE = blob_se;
	mSiteID = mBlobSE->getConfig().siteid;
	mCubeID = mBlobSE->getConfig().cubeid;
	mAseID = mBlobSE->getConfig().aseid;
	AosXmlTagPtr config = conf->getFirstChild("HeaderChangeLog", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderChangeLog");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mDeltaDataSize = config->getAttrU32("delta_size", eDeltaSize);
	if (eDeltaSize == mDeltaDataSize)
	{
		OmnAlarm << "mDeltaDataSize set to default value:" << mDeltaDataSize
				<< ", please make sure this is what you want." << enderr;
	}
	mDeltaSaveCountOfEveryHeaderFlush = config->getAttrU32(
			"delta_save_count_of_every_header_flush", eDeltaSaveCountOfEveryHeaderFlush);
	if (eDeltaSaveCountOfEveryHeaderFlush == mDeltaSaveCountOfEveryHeaderFlush)
	{
		OmnAlarm << "mDeltaSaveCountOfEveryHeaderFlush set to default value:" <<
				mDeltaSaveCountOfEveryHeaderFlush << ", please make sure this is what you want." << enderr;
	}
	mDeltaSavingInterval = config->getAttrU32(
			"delta_saving_interval", eDeltaSavingInterval);
	if (eDeltaSavingInterval == mDeltaSavingInterval)
	{
		OmnAlarm << "mDeltaSavingInterval set to default value:" << mDeltaSavingInterval
				<< ", please make sure this is what you want." << enderr;
	}
	mHeaderChangeLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
			eHeaderChangLogFileID_start, AosRawFile::eModifyOnly);
	if (mHeaderChangeLogFile.isNull())
	{
		rdata->setErrmsg("AosHeaderChangeLog get rawfile failed, rawfid:" + (u32)eHeaderChangLogFileID_start);
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mDeltaSavedCount = 0;
	mLastSavedTime = OmnTime::getCrtSec();
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("SaveDeltaTimer", mDeltaSavingInterval, 0, thisptr, 0);
}


AosHeaderChangeLog::~AosHeaderChangeLog()
{
}


int
AosHeaderChangeLog::appendHeader(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&header)
{
	// Any time when a header is modified, this function is called. 
	// It appends the header to its mBuff and returns.
	int iRet = 0;
	mLockRaw->lock();
	AosBuffPtr buff = OmnNew AosBuff(sizeof(HeaderChangeLogEntry) + header->getHeaderInACLAndHCLLen());
	if (buff.isNull())
	{
		OmnAlarm << "create buff obj failed for appending header to HCL, docid:" << header->getDocid()
				<< " timpstamp:" << header->getTimestamp() << enderr;
		return -1;
	}
	// header change log entry
	// u64		docid
	// u64		snap_id
	// char[]	header_buff(docid not included)
	if (0 == header->getDocid())
	{
		OmnAlarm << "0 == doc->getDocid()" << enderr;
	}
	HeaderChangeLogEntry* pHeaderChangeLogEntry = (HeaderChangeLogEntry*)buff->data();
	pHeaderChangeLogEntry->ullDocID = header->getDocid();
	iRet = header->serializeToACLandHCLBuff(pHeaderChangeLogEntry->pHeaderBuff);
	if (0 != iRet)
	{
		OmnAlarm << "header->serializeToChangeLogBuff failed for docid:" << header->getDocid() << enderr;
		mLockRaw->unlock();
		return -2;
	}
	buff->setDataLen(sizeof(HeaderChangeLogEntry) + header->getHeaderInACLAndHCLLen());
	buff->setCrtIdx(0);
//	if (mBlobSE->getConfig().del_log_enabled)
//	{
//		iRet = mDelLog->writeDelLog(rdata.getPtrNoLock(), header);
//		if (0 != iRet)
//		{
//			OmnAlarm << "del log writing failed, iRet=" << iRet << enderr;	//not fatal error
//		}
//	}
//	OmnScreen << "docid in buff:" << buff->getU64(0) << " docid appended:" << header->getDocid() << endl;
	bool rslt = mChangeLogBuffArray->appendEntry(buff->data(), buff->dataLen(), header->getDocid(), rdata);
	aos_assert_rl(rslt, mLockRaw, -3);
	mLockRaw->unlock();
	if (needToFlushDirtyHeaders())
	{
		rslt = flushDirtyHeaders(rdata, mActiveRawfile);
		aos_assert_rl(rslt, mLockRaw, -4);
	}
	else if (needToSaveDelta())
	{
		int iRet = saveDelta();
		aos_assert_rl(0 == iRet, mLockRaw, -5);
	}
	return 0;
}


bool
AosHeaderChangeLog::run()
{
	/*
	 * This function saves the delta data
	 * starts from mLastSavedDeltaPos + 1
	 * to mChangeLogBuffArray->buffLen()
	 * to header change log file
	 */
	int iRet = 0;
	OmnScreen << "saving delta, mChangeLogBuffArray->getBuff()->data():'"
			<< mChangeLogBuffArray->getBuff()->data() << "', mChangeLogBuffArray->getBuff()->dataLen()"
			<< mChangeLogBuffArray->getBuff()->dataLen() << endl;
	char *data = mChangeLogBuffArray->getBuff()->data() +
			mLastSavedDeltaPos * mChangeLogBuffArray->getRecordLen();
	mLockRaw->lock();
	AosBuffPtr buff = OmnNew AosBuff(mDeltaDataSize * mChangeLogBuffArray->getRecordLen());
	memcpy(buff->data(), data, mDeltaDataSize * mChangeLogBuffArray->getRecordLen());
	mLockRaw->unlock();
	buff->setDataLen(mDeltaDataSize * mChangeLogBuffArray->getRecordLen());

	u64 crt_rawfid = mHeaderChangeLogFile->getRawFileID();
	u64 data_len = buff->dataLen();
	aos_assert_rr(data_len > 0, mRdata, false);

	if (0 != ( iRet = mHeaderChangeLogFile->append(mRdata, data, data_len)))	//TODO:make this thread safe
	{
		AosLogError(mRdata, false, AosErrmsgId::eSaveDelta)
				<< AosFieldName::eReturnValue << iRet << enderr;
		mIsSavingDelta = false;
		return false;
	}
	else
	{
		OmnScreen << "delta saved to rawfile with rawfid:" << crt_rawfid << endl;
		mHeaderChangeLogFile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID,
				crt_rawfid + 1, AosRawFile::eModifyOnly);
		aos_assert_rr(mHeaderChangeLogFile.notNull(), mRdata, false);
		OmnScreen << "mHeaderChangeLogFile switched to rawfid:" << crt_rawfid + 1 << endl;
	}
	mLastSavedTime = OmnTime::getCrtSec();
	mIsSavingDelta = false;
	mLastSavedDeltaPos += mDeltaDataSize;
	mDeltaSavedCount++;
	return true;
}


bool
AosHeaderChangeLog::procFinished()
{
	return true;
}


int
AosHeaderChangeLog::shutdown(
		AosRundata	*rdata,
		AosRawFile*	active_rawfile)
{
	/*
	 * 1.save delta
	 */
	OmnScreen << "HeaderChangeLog is shutting down" << endl;
	while(mIsSavingDelta || mIsFlushingHeaders)
	{
		OmnScreen << "HeaderChangeLog is sleeping, mIsSavingDelta:" << mIsSavingDelta
				<< " mIsFlushingHeaders:" << mIsFlushingHeaders << endl;
		OmnSleep(1);
	}
	int iRet = 0;
	if (needToSaveDelta())
	{
		OmnScreen << "HeaderChangeLog is saving delta for shutting down" << endl;
		iRet = saveDelta();
		aos_assert_rr(0 == iRet, rdata, iRet);
	}
	OmnScreen << "HeaderChangeLog is flushing dirty headers for shutting down" << endl;
	bool rslt = flushDirtyHeaders(rdata, active_rawfile);
	OmnScreen << "HeaderChangeLog has flushed dirty headers for shutting down" << endl;
	aos_assert_rr(rslt, rdata, -1);
	return 0;
}


bool
AosHeaderChangeLog::flushDirtyHeaders(
		AosRundata	*rdata,
		AosRawFile*	active_rawfile)
{
	// this function overwrite the header file according to the header change log
	aos_assert_rr(active_rawfile, rdata, false);
	int iRet = 0;
	mLockRaw->lock();
	if (0 == mChangeLogBuffArray->getNumEntries())
	{
		OmnScreen << "0 == mChangeLogBuffArray->getNumEntries()" << endl;
		mLockRaw->unlock();
		return true;
	}
	OmnScreen << "flushing headers: mChangeLogBuffArray->getNumEntries():"
			<< mChangeLogBuffArray->getNumEntries() << endl;
	mOldBuff = mChangeLogBuffArray;
	mChangeLogBuffArray = OmnNew AosBuffArray(mCompareFunc, false, true, eHeaderChageLogMaxSize);
	OmnScreen << "mOldBuff->getNumEntries():" << mOldBuff->getNumEntries() << endl;
	AosBuffPtr buff = OmnNew AosBuff(sizeof(SaveHeaderEntry));
	if (buff.isNull())
	{
		OmnAlarm << "new buff failed for saving header mark." << enderr;
		mLockRaw->unlock();
		return false;
	}
	SaveHeaderEntry* pSaveHeaderEntry = (SaveHeaderEntry*)buff->data();
	pSaveHeaderEntry->ulLength = sizeof(SaveHeaderEntry);
	pSaveHeaderEntry->ucType = eHeaderStartToSave;
	pSaveHeaderEntry->ulLengthTail = pSaveHeaderEntry->ulLength;
	iRet = mActiveRawfile->append(rdata, buff->data(), sizeof(SaveHeaderEntry));
	if (0 != iRet)
	{
		OmnAlarm << "save eHeaderStartToSave entry to ACL failed, iRet=" << iRet
				<< " rawfid:" << mActiveRawfile->getRawFileID() << enderr;
		mLockRaw->unlock();
		return false;
	}
	mIsFlushingHeaders = true;
	mLastSavedDeltaPos = 0;
	mLockRaw->unlock();
	bool rslt = false;
	rslt = mOldBuff->sort();
	if (!rslt)
	{
		OmnAlarm << "mOldBuff->sort() failed." << enderr;
		mIsFlushingHeaders = false;
		return false;
	}
	i64 start_pos = 0;
	i64 end_pos = 0;
	HeaderChangeLogEntry* pHeaderChangeLogEntry = (HeaderChangeLogEntry*)mOldBuff->getEntry(start_pos);
	u64 start_docid = pHeaderChangeLogEntry->ullDocID;
	while(start_pos < mOldBuff->getNumEntries())
	{
		pHeaderChangeLogEntry = (HeaderChangeLogEntry*)mOldBuff->getEntry(start_pos);
		if (!pHeaderChangeLogEntry)
		{
			OmnAlarm << "mOldBuff->getEntry(end_pos) failed, mOldBuff->getNumEntries():" << mOldBuff->getNumEntries()
					<< " end_pos:" << end_pos << enderr;
			mIsFlushingHeaders = false;
			return false;
		}
		start_docid = pHeaderChangeLogEntry->ullDocID;
		u64 last_docid_in_file = mHeaderFile->getEndDocidOfNormalHeaderFileByDocid(start_docid);
		end_pos = mOldBuff->findBoundary(start_pos, (char*)&last_docid_in_file, false, false);
		if (mOldBuff->getNumEntries() == end_pos)
		{
			end_pos--;
		}
		rslt = mHeaderFile->flushChangeLogToFile(rdata, mOldBuff.getPtrNoLock(), start_pos, end_pos);
		if (!rslt)
		{
			OmnAlarm << "flushChangeLogToFile failed, mOldBuff->getNumEntries():" << mOldBuff->getNumEntries()
					<< " start_docid:" << start_docid << " last_docid_in_file:" << last_docid_in_file
					<< " start_pos:" << start_pos << " end_pos:" << end_pos
					<< " docid@start_pos:" << ((HeaderChangeLogEntry*)mOldBuff->getEntry(start_pos))->ullDocID
					<< " docid@end_pos:" << ((HeaderChangeLogEntry*)mOldBuff->getEntry(end_pos))->ullDocID<< enderr;
			mIsFlushingHeaders = false;
			return false;
		}
		start_pos = end_pos + 1;
	}

	//save the header saved mark
	AosBuffPtr marker_buff = OmnNew AosBuff();
	marker_buff->setU32(sizeof(u32) + sizeof(u8) + sizeof(u32));
	marker_buff->setU8(eHeaderSaved);
	marker_buff->setU32(sizeof(u32) + sizeof(u8) + sizeof(u32));
	iRet = active_rawfile->append(rdata, marker_buff->data(), marker_buff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "append header saved mark to acitve file failed, rawfid:" << active_rawfile->getRawFileID()
				<< " iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		return false;
	}
	rslt = removeHeaderChangeLogFiles(rdata);
	if (!rslt)
	{
		OmnAlarm << "removeHeaderChangeLogFiles failed." << enderr;
		mIsFlushingHeaders = false;
		return false;
	}
	mIsFlushingHeaders = false;
	return true;
}


bool
AosHeaderChangeLog::sort(AosRundata *rdata)
{
	// this function sorts all the header change log according to docid
	// 1.sort the buff array(consists of docid + header)
	// 2.group the elements so that the headers belong to one header file falls into the
	//   same group
	// 3.read the header file entirely, overwrite the changed headers, write is back
	return mChangeLogBuffArray->sort();
}


int
AosHeaderChangeLog::saveDelta()
{
	// This function saves the delta (or the recent changes) to 
	// Change Log File. This should be done through a separate
	// thread. The change log buff can be conceptually considered
	// as a sequence of blocks. Each block is a delta. This function
	// saves the last block (or detal) to file, which is identified
	// by [start_pos, end_pos]. 
	mIsSavingDelta = true;
	OmnThrdShellProcPtr thisptr(this, false);
	OmnThreadPool::runProcAsync(thisptr);
	return 0;
}


AosBuffPtr
AosHeaderChangeLog::readHeaderChangeLogFromFiles(AosRundata *rdata)
{
	/*
	 * this function search the header change log files starting from rawfid eHeaderChangLogFileID_start
	 * to eHeaderChangLogFileID_end,
	 */
	AosBuffPtr buff_one = OmnNew AosBuff(eHeaderChangeLogFileSize_default);
	AosBuffPtr buff = OmnNew AosBuff(eHeaderChageLogMaxSize);
	int iRet = 0;
	bool rslt = false;
	for(u64 i = eHeaderChangLogFileID_start; i < eHeaderChangLogFileID_end; i++)
	{
		if (mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, i))
		{
			if (0 != (iRet = mRawSE->readRawFile(rdata, mSiteID, mCubeID, mAseID,
					i, 0, eHeaderChangeLogFileSize_default, buff_one, AosRawFile::eModifyOnly)))
					//the eModifyOnly may seem wired, because RawSE holds the file handler and has open type
					//checking, this is the first time to open the file, so we have to open it as eModifyOnly
					//for later use(deletion)
			{	// we assume that the open operation always succeeds if the file being opened exists
				OmnAlarm << "header change log file reading failed, rawfid=" << i
						<< " iRet=" << iRet << enderr;
				return NULL;
			}
		}
		else
		{
			if (eHeaderChangLogFileID_start == i)
			{
				i++;
			}
			mHeaderChangeLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
					i - 1, AosRawFile::eModifyOnly);
			if (mHeaderChangeLogFile.isNull())	//this should never happen, but just in case, you know
			{
				OmnShouldNeverComeHere;
			}
			return buff;
		}
		rslt = buff->appendBuff(buff_one);
		aos_assert_rr(rslt, rdata, NULL);
	}
	return buff;
}


bool
AosHeaderChangeLog::needToSaveDelta()
{
//	OmnScreen << "mChangeLogBuffArray->dataLen():" << mChangeLogBuffArray->dataLen()
//			<< " mLastSavedDeltaPos:" << mLastSavedDeltaPos << " mDeltaDataSize:" << mDeltaDataSize << endl;
	return false;	//TODO:we will postpone the feature
	if (mIsSavingDelta)
	{
		return false;
	}

	return mChangeLogBuffArray->getNumEntries() - mLastSavedDeltaPos >= mDeltaDataSize;
}


void        
AosHeaderChangeLog::timeout(
		const int		timerId,
		const OmnString	&timerName,
		void			*parm)
{
	OmnScreen << "timeout, OmnTime::getCrtSec():" << OmnTime::getCrtSec() << endl;
	if (!mIsSavingDelta)
	{
		if (!mIsFlushingHeaders && 0 == mDeltaSavedCount % mDeltaSaveCountOfEveryHeaderFlush
				&& 0 != mDeltaSavedCount)
		{
			if (!flushDirtyHeaders(mRdata, mActiveRawfile))
			{
				OmnAlarm << "flush dirty headers failed" << enderr;
			}
		}
		else
		{
			OmnScreen << "timeout but no need to flush, mDeltaSavedCount:" << mDeltaSavedCount
					<< " mDeltaSaveCountOfEveryHeaderFlush:" << mDeltaSaveCountOfEveryHeaderFlush << endl;
			if (needToSaveDelta())
			{
				saveDelta();
			}
		}
	}
	else
	{
		OmnScreen << "timeout but is saving delta" << endl;
	}
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("SaveDeltaTimer", mDeltaSavingInterval, 0, thisptr, 0);
}


bool
AosHeaderChangeLog::needToFlushDirtyHeaders()
{
	return mChangeLogBuffArray->getNumEntries() >= eHeadersHolding;
}


bool
AosHeaderChangeLog::removeHeaderChangeLogFiles(AosRundata *rdata)
{
	int iRet = 0;
	for(u64 i = eHeaderChangLogFileID_start; i <= mHeaderChangeLogFile->getRawFileID(); i++)
	{
		if (0 != (iRet = mRawSE->deleteFile(rdata, mSiteID, mCubeID, mAseID, i, AosRawFile::eModifyOnly)))
		{	// we assume that the deletion always succeeds if the file being opened exists
			OmnAlarm << "header change log file deletion failed, siteid=" << mSiteID
					<< " cubeid=" << mCubeID << " aseid=" << mAseID << " rawfid=" << i << enderr;
			//TODO:since the header change log files will be overwritten later,
			//so we do not worry too much about the elimination
		}
	}
	mHeaderChangeLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
			eHeaderChangLogFileID_start, AosRawFile::eModifyOnly);
	if (mHeaderChangeLogFile.isNull())
	{
		OmnAlarm << "can not switch mHeaderChangeLogFile back to start, rawfid="
				<< eHeaderChangLogFileID_start << enderr;
		return false;
	}
	return true;
}


void
AosHeaderChangeLog::switchActiveFile(AosRawFile*	active_rawfile)
{
	mActiveRawfile = active_rawfile;
}
#endif
