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
#include "BlobSE/DeletionLog.h"

#include "alarm_c/alarm.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/RawfidRange.h"
#include "BlobSE/Ptrs.h"
#include "Debug/Except.h"
#include "DfmUtil/DfmDocHeader.h"
#include "DfmUtil/DfmDoc.h"
#include "Porting/Sleep.h"
#include "RawSE/RawSE.h"
#include "Thread/ThreadPool.h"
#include "Util/Buff.h"
#include "Util/BuffArray.h"
#include "Util1/Timer.h"
#include "Util/CompareFun.h"
#if 0
AosDeletionLog::AosDeletionLog(
		AosRundata			*rdata,
		AosRawSE 			*rawse,
		AosRawFile*			active_rawfile,
		const AosDfmDocPtr	&doc,
		const u32			header_size,
		AosHeaderCache*		header_cache,
		const AosXmlTagPtr	&conf)
:
OmnThrdShellProc("BlobSEHCL"),
mLastSavedDeltaPos(0),
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mSaveLockRaw(OmnNew OmnMutex()),
mSaveLock(mSaveLockRaw),
mRdata(rdata),
mCompareFunc(OmnNew AosFunCustom(sizeof(DeletionLogEntry), false, conf->getFirstChild("DeletionLog", true))),
mDelLogBuffArray(OmnNew AosBuffArray(mCompareFunc, false, true, eDelLogMaxSize)),	//TODO:args
mIsSavingDelta(false),
mIsCompacting(false),
mActiveRawfile(active_rawfile),
mDoc(doc),
mHeaderSize(header_size),
mHeaderCache(header_cache)
{
	mDeltaSavedCount = 0;
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
    mSiteID = config->getAttrU32("siteid", eSiteID);
    if (eSiteID == mSiteID)
    {
        OmnAlarm << "site id set to default value:" << mSiteID
        << ", please make sure this is what you want." << enderr;
    }
    mCubeID = config->getAttrU32("cubeid", eCubeID);
    if (eCubeID == mCubeID)
    {
        OmnAlarm << "cube id set to default value:" << mCubeID
        << ", please make sure this is what you want." << enderr;
    }
    mAseID = config->getAttrU32("aseid", eAseID);
    if (eAseID == mAseID)
    {
        OmnAlarm << "ase id set to default value:" << mAseID
        << ", please make sure this is what you want." << enderr;
    }
    config = conf->getFirstChild("DeletionLog", true);
    if (config.isNull())
    {
        rdata->setErrmsg("can not find config tag named DeletionLog");
        OmnThrowException(rdata->getErrmsg());
        return;
    }
    mDeltaDataSize = config->getAttrU32("delta_size", eDeltaSize);
    if (eDeltaSize == mDeltaDataSize)
    {
        OmnAlarm << "mDeltaDataSize set to default value:" << mDeltaDataSize
        << ", please make sure this is what you want." << enderr;
    }
    mDeltaSaveCountOfEveryCompation = config->getAttrU32(
                                          "delta_save_count_of_every_compaction", eDeltaSaveCountOfEveryCompaction);
    if (eDeltaSaveCountOfEveryCompaction == mDeltaSaveCountOfEveryCompation)
    {
        OmnAlarm << "mDeltaSaveCountOfCompation set to default value:" <<
        mDeltaSaveCountOfEveryCompation << ", please make sure this is what you want." << enderr;
    }
    mDeltaSavingInterval = config->getAttrU32(
                               "delta_saving_interval", eDeltaSavingInterval);
    if (eDeltaSavingInterval == mDeltaSavingInterval)
    {
        OmnAlarm << "mDeltaSavingInterval set to default value:" << mDeltaSavingInterval
        << ", please make sure this is what you want." << enderr;
    }
    mCompactionThreshold = config->getAttrU32(
                               "compaction_threshold", eCompactionThreshold);
    if (eCompactionThreshold == mCompactionThreshold)
    {
        OmnAlarm << "mCompactionThreshold set to default value:" << mCompactionThreshold
        << ", please make sure this is what you want." << enderr;
    }
    if (mCompactionThreshold == mDeltaDataSize)
    {
        rdata->setErrmsg("FATAL ERROR:mCompactionThreshold == mDelt	aDataSize");
        OmnThrowException(rdata->getErrmsg());
        return;
    }
    mCompactionPercentageThreshold = config->getAttrU32(
                               "compaction_percentage_threshold", eCompcationPercentageThreshold);
    if (eCompcationPercentageThreshold == mCompactionPercentageThreshold)
    {
        OmnAlarm << "mCompactionPercentageThreshold set to default value:" << mCompactionPercentageThreshold
        << ", please make sure this is what you want." << enderr;
    }
    mDeletionLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
                                          eDeletionLogFileID_start, AosRawFile::eModifyOnly);
    if (mDeletionLogFile.isNull())
    {
        rdata->setErrmsg("AosDeletionLog get rawfile failed, rawfid:" + (u32)eDeletionLogFileID_start);
        OmnThrowException(rdata->getErrmsg());
        return;
    }
    mCompactedBuffMaxSize = config->getAttrU32(
                               "compacted_buff_max_size", eCompactedBuffMaxSize);
    if (eCompactedBuffMaxSize == mCompactedBuffMaxSize)
    {
        OmnAlarm << "mCompactedBuffMaxSize set to default value:" << mCompactedBuffMaxSize
        << ", please make sure this is what you want." << enderr;
    }
    mCompactedBuff = OmnNew AosBuff(mCompactedBuffMaxSize);
    if (mCompactedBuff.isNull())
    {
        rdata->setErrmsg("mCompactedBuff can not be initialized.");
        OmnThrowException(rdata->getErrmsg());
        return;
    }
    mCompactedFileID = eComopactedFileID_start;
    mLastSavedTime = OmnTime::getCrtSec();
    OmnTimerObjPtr thisptr(this, false);
    OmnTimer::getSelf()->startTimer("SaveDeltaTimer", mDeltaSavingInterval, 0, thisptr, 0);
}


AosDeletionLog::~AosDeletionLog()
{
}


int
AosDeletionLog::writeDelLog(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&header)
{
    // Any time when a doc is deleted, this function is called.
    // It appends the del log entry to its mBuff and returns.
    mLockRaw->lock();
    AosBuffPtr buff = OmnNew AosBuff(sizeof(DeletionLogEntry));
    // deletion log entry
    // u64		rawfid
    // u64		offset
    // u64		length
    DeletionLogEntry* pDeletionLogEntry = (DeletionLogEntry*)buff->data();
    pDeletionLogEntry->ullRawfid = header->getBodyRawfid();
    pDeletionLogEntry->ullOffset = header->getBodyOffset();
    pDeletionLogEntry->ullLength = header->getBodyLen();
    buff->setDataLen(sizeof(DeletionLogEntry));
    buff->setCrtIdx(0);
    bool rslt = mDelLogBuffArray->appendEntry(buff->data(), buff->dataLen(), rdata);
    aos_assert_rl(rslt, mLockRaw, -1);
    mLockRaw->unlock();
    if (needToCompact())
    {
        rslt = compact(rdata, mActiveRawfile);
        aos_assert_rl(rslt, mLockRaw, -2);
    }
    else if (needToSaveDelta())
    {
        int iRet = saveDelta();
        aos_assert_rl(0 == iRet, mLockRaw, -3);
    }
    return 0;
}


bool
AosDeletionLog::run()
{
    /*
     * This function saves the delta data
     * starts from mLastSavedDeltaPos + 1
     * to mDelLogBuffArray->buffLen()
     * to header change log file
     */
    int iRet = 0;
    OmnScreen << "saving delta, mDelLogBuffArray->getBuff()->data():'"
		<< mDelLogBuffArray->getBuff()->data() << "', mDelLogBuffArray->getBuff()->dataLen():"
		<< mDelLogBuffArray->getBuff()->dataLen() << endl;
    char *data = mDelLogBuffArray->getBuff()->data() +
                 mLastSavedDeltaPos * mDelLogBuffArray->getRecordLen();
    mLockRaw->lock();
    u32 this_delta_size = mDeltaDataSize < (mDelLogBuffArray->getNumEntries() - mLastSavedDeltaPos) ?
    		mDeltaDataSize : mDelLogBuffArray->getNumEntries() - mLastSavedDeltaPos;
    mLockRaw->unlock();
    AosBuffPtr buff = OmnNew AosBuff(this_delta_size * mDelLogBuffArray->getRecordLen());
    memcpy(buff->data(), data, this_delta_size * mDelLogBuffArray->getRecordLen());
    buff->setDataLen(this_delta_size * mDelLogBuffArray->getRecordLen());
    u64 crt_rawfid = mDeletionLogFile->getRawFileID();
    u64 data_len = buff->dataLen();
    if (0 == data_len)	//TODO:
    {
    	return true;
    }
    if (0 != ( iRet = mDeletionLogFile->overwrite(mRdata, data, data_len)))	//TODO:make this thread safe
    {
        AosLogError(mRdata, false, AosErrmsgId::eSaveDelta)
        		<< AosFieldName::eReturnValue << iRet << enderr;
        mIsSavingDelta = false;
        return false;
    }
    else
    {
        OmnScreen << "delta saved to rawfile with rawfid:" << crt_rawfid << endl;
        mDeletionLogFile = mRawSE->getRawFile(mRdata, mSiteID, mCubeID, mAseID,
                                              crt_rawfid + 1, AosRawFile::eModifyOnly);
        aos_assert_rr(mDeletionLogFile.notNull(), mRdata, false);
        OmnScreen << "mDeletionLogFile switched to rawfid:" << crt_rawfid + 1 << endl;
    }
    mLastSavedTime = OmnTime::getCrtSec();
    mIsSavingDelta = false;
    mLastSavedDeltaPos += mDeltaDataSize;
    return true;
}


bool
AosDeletionLog::procFinished()
{
    return true;
}


bool
AosDeletionLog::init(AosRundata	*rdata)
{
	bool rslt = readDeletionLogFromFiles(rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


int
AosDeletionLog::shutdown(
    AosRundata	*rdata,
    AosRawFile*	active_rawfile)
{
    /*
     * 1.save delta
     */
    OmnScreen << "DeletionLog is shutting down" << endl;
    while(mIsSavingDelta || mIsCompacting)
    {
        OmnScreen << "DeletionLog is sleeping" << endl;
        OmnSleep(1);
    }
    OmnScreen << "DeletionLog is saving delta for shutting down" << endl;
    bool rslt = run();	//save delta
    aos_assert_rr(rslt, rdata, -1);
    rslt = flushAndSwitchCompactedBuff(rdata);
    if (!rslt)
    {
        OmnAlarm << "flushAndSwitchCompactedBuff failed." << enderr;
        return -2;	//TODO:should I return false here?
    }
    return 0;
}


bool
AosDeletionLog::compact(
    AosRundata	*rdata,
    AosRawFile*	active_rawfile)
{
	aos_assert_rr(active_rawfile, rdata, false);
	mLockRaw->lock();
	if (0 == mDelLogBuffArray->getNumEntries())
	{
		OmnScreen << "0 == mDelLogBuffArray->getNumEntries()" << endl;
		return true;
	}
	OmnScreen << "compacting: mDelLogBuffArray->getNumEntries():"
			<< mDelLogBuffArray->getNumEntries() << endl;
	mOldBuff = mDelLogBuffArray;
	mDelLogBuffArray = OmnNew AosBuffArray(mCompareFunc, false, true, eDelLogMaxSize);
	OmnScreen << "mOldBuff->getNumEntries():" << mOldBuff->getNumEntries() << endl;
	mIsCompacting = true;
	mLastSavedDeltaPos = 0;
	mLockRaw->unlock();
	i64 start_pos = 0;
	bool rslt = false;
	rslt = mOldBuff->sort();
	if (!rslt)
	{
		mIsCompacting = false;
		return false;
	}
	i64 end_pos = 0;
	DeletionLogEntry* pDeletionLogEntry = (DeletionLogEntry*)mOldBuff->getEntry(start_pos);
	while(start_pos < mOldBuff->getNumEntries())
	{
		pDeletionLogEntry = (DeletionLogEntry*)mOldBuff->getEntry(start_pos);
		u64 tmp_rawfid = pDeletionLogEntry->ullRawfid + 1;
		end_pos = mOldBuff->findBoundary(start_pos, (char*)&tmp_rawfid, false, false);
		end_pos--;	//findBoundary will find the first entry that equals tmp_rawfid
		OmnScreen << "findBoundary result: start_pos=" << start_pos << " end_pos:" << end_pos
				<< " rawfid@start_pos:" << pDeletionLogEntry->ullRawfid
				<< " rawfid@(end_pos - 1):" << ((DeletionLogEntry*)mOldBuff->getEntry(end_pos - 1))->ullRawfid
				<< " rawfid@end_pos:" << ((DeletionLogEntry*)mOldBuff->getEntry(end_pos))->ullRawfid
				<< " mOldBuff->getNumEntries():" << mOldBuff->getNumEntries() << endl;
		if (end_pos < mOldBuff->getNumEntries() - 1)
		{
			OmnScreen << "rawfid@(end_pos + 1):" << ((DeletionLogEntry*)mOldBuff->getEntry(end_pos + 1))->ullRawfid
					<< endl;
		}
		rslt = compactRawfile(rdata, mOldBuff.getPtrNoLock(), start_pos, end_pos);
		if (!rslt)
		{
			mIsCompacting = false;
			return false;
		}
		start_pos = end_pos + 1;
	}

	//save the compacted mark
	AosBuffPtr marker_buff = OmnNew AosBuff();
	marker_buff->setU32(sizeof(u32) + sizeof(u8) + sizeof(u32));
	marker_buff->setU8(eCompacted);
	marker_buff->setU32(sizeof(u32) + sizeof(u8) + sizeof(u32));
	int iRet = active_rawfile->append(rdata, marker_buff->data(), marker_buff->dataLen());
	mIsCompacting = false;
	aos_assert_rr(0 == iRet, rdata, false);
	return true;
}


bool
AosDeletionLog::sort(AosRundata *rdata)
{
    // this function sorts all the header change log according to docid
    // 1.sort the buff array(consists of docid + header)
    // 2.group the elements so that the headers belong to one header file falls into the
    //   same group
    // 3.read the header file entirely, overwrite the changed headers, write is back
    return mDelLogBuffArray->sort();
}


int
AosDeletionLog::saveDelta()
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


bool
AosDeletionLog::readDeletionLogFromFiles(AosRundata *rdata)
{
    /*
     * this function search the header change log files starting from rawfid eDeletionLogFileID_start
     * to eDeletionLogFileID_end,
     */
    AosBuffPtr buff_one = OmnNew AosBuff(eDeletionLogFileSize_default);
    AosBuffPtr buff = OmnNew AosBuff(eDelLogMaxSize);
    int iRet = 0;
    bool rslt = false;
    i64 number_of_entries = 0;
    for(u64 i = eDeletionLogFileID_start; i < eDeletionLogFileID_end; i++)
    {
        if (mRawSE->fileExists(rdata, mSiteID, mCubeID, mAseID, i))
        {
            if (0 != (iRet = mRawSE->readRawFile(rdata, mSiteID, mCubeID, mAseID,
                                                 i, 0, eDeletionLogFileSize_default, buff_one, AosRawFile::eModifyOnly)))
                //the eModifyOnly may seem wired, because RawSE holds the file handler and has open type
                //checking, this is the first time to open the file, so we have to open it as eModifyOnly
                //for later use(deletion)
            {	// we assume that the open operation always succeeds if the file being opened exists
                OmnAlarm << "del log file reading failed, rawfid=" << i
                		<< " iRet=" << iRet << enderr;
                return false;
            }
            number_of_entries += mDeltaDataSize;
        }
        else
        {
            if (eDeletionLogFileID_start == i)
            {
                i++;
            }
            mDeletionLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
                                                  i - 1, AosRawFile::eModifyOnly);
            if (mDeletionLogFile.isNull())	//this should never happen, but just in case, you know
            {
                OmnShouldNeverComeHere;
            }
            return true;
        }
        rslt = buff->appendBuff(buff_one);
        aos_assert_rr(rslt, rdata, false);
    }
    rslt = mDelLogBuffArray->setBuff(buff);
    aos_assert_rr(rslt, rdata, false);
    mDelLogBuffArray->setNumEntries(number_of_entries);
    mLastSavedDeltaPos = number_of_entries;
    return true;
}


bool
AosDeletionLog::needToSaveDelta()
{
    //	OmnScreen << "mDelLogBuffArray->dataLen():" << mDelLogBuffArray->dataLen()
    //			<< " mLastSavedDeltaPos:" << mLastSavedDeltaPos << " mDeltaDataSize:" << mDeltaDataSize << endl;
    if (mIsSavingDelta)
    {
        return false;
    }

    return mDelLogBuffArray->getNumEntries() - mLastSavedDeltaPos >= mDeltaDataSize;
}


void
AosDeletionLog::timeout(
    const int		timerId,
    const OmnString	&timerName,
    void			*parm)
{
    OmnScreen << "timeout" << endl;
    if (!mIsSavingDelta)
    {
        if (!mIsCompacting && 0 == mDeltaSavedCount % mDeltaSaveCountOfEveryCompation
        		&& 0 != mDeltaSavedCount)
        {
            if (!compact(mRdata, mActiveRawfile))
            {
                OmnAlarm << "compaction failed" << enderr;
            }
        }
        else
        {
            OmnScreen << "timeout but no need to compact" << endl;
            if (needToSaveDelta())
            {
                saveDelta();
                mDeltaSavedCount++;
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
AosDeletionLog::needToCompact()
{
    return mDelLogBuffArray->getNumEntries() >= mCompactionThreshold;
}


bool
AosDeletionLog::removeDeletionLogFiles(AosRundata *rdata)
{
    int iRet = 0;
    for(u64 i = eDeletionLogFileID_start; i <= mDeletionLogFile->getRawFileID(); i++)
    {
        if (0 != (iRet = mRawSE->deleteFile(rdata, mSiteID, mCubeID, mAseID, i, AosRawFile::eModifyOnly)))
        {	// we assume that the deletion always succeeds if the file being opened exists
            OmnAlarm << "header change log file deletion failed, siteid=" << mSiteID
            << " cubeid=" << mCubeID << " aseid=" << mAseID << " rawfid=" << i << enderr;
            //TODO:since the header change log files will be overwritten later,
            //so we do not worry too much about the elimination
        }
    }
    mDeletionLogFile = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
                                          eDeletionLogFileID_start, AosRawFile::eModifyOnly);
    if (mDeletionLogFile.isNull())
    {
        OmnAlarm << "can not switch mDeletionLogFile back to start, rawfid="
        << eDeletionLogFileID_start << enderr;
        return false;
    }
    return true;
}


void
AosDeletionLog::switchActiveFile(AosRawFile*	active_rawfile)
{
    mActiveRawfile = active_rawfile;
}


bool
AosDeletionLog::compactRawfile(
    AosRundata		*rdata,
    AosBuffArray	*buff,
    i64				start,
    i64				end)
{
	aos_assert_rr(buff, rdata, false);
	u64	length_of_deleted = 0;
	DeletionLogEntry* pDeletionLogEntry = (DeletionLogEntry*)buff->getEntry(start);
	if (mActiveRawfile->getRawFileID() == pDeletionLogEntry->ullRawfid)
	{
		OmnScreen << "no need to compact active rawfile, rawfild:" << pDeletionLogEntry->ullRawfid << endl;
		copyDelLogBuffToNewDelLogBuff(rdata, buff, start, end);	//TODO:not fatal?
		return true;
	}
	u64	rawfid = pDeletionLogEntry->ullRawfid;
	bool rslt = false;
	//calculating the deleted space of this rawfile
	OmnScreen << "compacting rawfile with rawfid:" << rawfid << " start:" << start << " end:" << end << endl;
	for (i64 i = start; i <= end; i++)
	{
		pDeletionLogEntry = (DeletionLogEntry*)buff->getEntry(i);
		OmnScreen << "rawfid:" << pDeletionLogEntry->ullRawfid << " length:" << pDeletionLogEntry->ullLength << endl;
		aos_assert_rr(rawfid == pDeletionLogEntry->ullRawfid, rdata, false);	//double check
		length_of_deleted += pDeletionLogEntry->ullLength;
	}
	AosRawFilePtr raw_file = mRawSE->getRawFile(rdata, mSiteID, mCubeID, mAseID,
							 rawfid, AosRawFile::eReadWrite);
	if (raw_file.isNull())
	{
		OmnScreen << "rawfile deleted, rawfid:" << rawfid << endl;
		return true;
	}
	if (0 == raw_file->getLength())
	{
		OmnAlarm << "0 == raw_file->getLength(), rawfid:" << rawfid << enderr;
		return false;
	}
	OmnScreen << "compacting rawfile with rawfid:" << raw_file->getRawFileID() << " mActiveRawfile->getRawFileID():"
			<< mActiveRawfile->getRawFileID() << " length_of_deleted:" << length_of_deleted
			<< length_of_deleted << " raw_file->getLength():" << raw_file->getLength() << endl;
	if (mCompactionPercentageThreshold > (length_of_deleted * 100 / raw_file->getLength()))
		//no needs of compaction
	{
		OmnScreen << "deleted proportion does not reach the threshold, rawfid:" << rawfid
				<< " length_of_deleted:" << length_of_deleted << " raw_file->getLength():" << raw_file->getLength()
				<< " mCompactionPercentageThreshold:" << mCompactionPercentageThreshold << endl;
		copyDelLogBuffToNewDelLogBuff(rdata, buff, start, end);	//TODO:not fatal?
		return true;
	}
	u64 file_len = raw_file->getLength();
	AosBuffPtr file_buff = OmnNew AosBuff(file_len);
	aos_assert_rr(file_buff.notNull(), rdata, false);
	int iRet = raw_file->read(rdata, 0, raw_file->getLength(), file_buff);
	if (0 != iRet)
	{
		OmnAlarm << "body file read failed on rawfid:" << rawfid
		<< " with read length:" << file_len << enderr;
		return false;
	}

	//traverse the file buff
	aos_assert_rr(file_buff.notNull(), rdata, false);
	i64 pos = 0;
	while(pos < file_buff->dataLen())
	{
		if (!entryDeleted(rdata, file_buff->data() + pos, rawfid, pos))
		{
			rslt = copyEntryToCompactedBuff(rdata, file_buff->data() + pos);
			if (!rslt)
			{
				OmnAlarm << "copyEntryToNewFile failed." << enderr;
				return false;	//TODO:should I return false here?
			}
		}
		pos += *(u32*)(file_buff->data() + pos);
	}
	mvCompactedRawfids.push_back(rawfid);
	if (compactedBuffBigEnough())
	{
		rslt = flushAndSwitchCompactedBuff(rdata);
		if (!rslt)
		{
			OmnAlarm << "flushAndSwitchCompactedBuff failed." << enderr;
			return false;	//TODO:should I return false here?
		}
	}
	return true;
}


bool
AosDeletionLog::copyDelLogBuffToNewDelLogBuff(
			AosRundata		*rdata,
		    AosBuffArray	*buff,
		    i64				start,
		    i64				end)
{
	bool rslt = false;
	for (i64 i = start; i <= end; i++)	//append the entries to new buff
	{
		rslt = mDelLogBuffArray->appendEntry(buff->getEntry(i), buff->getRecordLen(), rdata);
		if (!rslt)
		{
			OmnAlarm << "copy del log to new del log buff failed" << enderr;
			return false;
		}
	}
	return true;
}


bool
AosDeletionLog::entryDeleted(
		AosRundata	*rdata,
		const char	*entry,
		const u64	rawfid,
		const u64	offset)
{
	u64 docid = *(u64*)(entry + sizeof(u32) + sizeof(u8) + sizeof(u64));
	u64 snap_id = *(u64*)(entry + sizeof(u32) + sizeof(u8) + sizeof(u64) + sizeof(u64));	//TODO:this is a problem
	AosBlobHeaderPtr pHeader = mHeaderCache->readHeader(rdata, docid, snap_id);
	if (pHeader.isNull())
	{
		OmnAlarm << "read header failed for judging whether the entry is deleted."
				<< " rawfid:" << rawfid << " offset:" << offset << " docid:" << docid << enderr;
		return false;
	}
	if (pHeader->getDocid() == docid && (pHeader->getBodyRawfid() != rawfid || pHeader->getBodyOffset() != offset))
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool
AosDeletionLog::copyEntryToCompactedBuff(
    AosRundata		*rdata,
    char*			buff)
{
    i64	len = *(u32*)buff;
    return mCompactedBuff->addBuff(buff, len);
}


bool
AosDeletionLog::compactedBuffBigEnough()
{
    return mCompactedBuff->dataLen() >= (i64)mCompactedBuffMaxSize;	//TODO:may be risky
}


bool
AosDeletionLog::flushAndSwitchCompactedBuff(AosRundata		*rdata)
{
    OmnScreen << "flushAndSwitchCompactedBuff, mCompactedBuff->dataLen():"
    		<< mCompactedBuff->dataLen() << endl;
	AosRawFilePtr file = mRawSE->createRawFile(rdata, mSiteID, mCubeID, mAseID,
                         mCompactedFileID, AosMediaType::eDisk, AosRawFile::eReadWrite);
    if (file.isNull())
    {
        OmnAlarm << "failed to create compacted file with rawfid:" << mCompactedFileID << enderr;
        return false;
    }
    int iRet = file->overwrite(rdata, mCompactedBuff->data(), mCompactedBuff->dataLen());
    if (0 != iRet)
	{
		OmnAlarm << "failed to append compacted buff to rawfile with rawfid:" << file->getRawFileID()
				<< " buffLen:" << mCompactedBuff->dataLen() << enderr;
		return false;
	}
	//switch the compacted buff
	AosBuffPtr oldCompactedBuff = mCompactedBuff;
	mCompactedBuff = OmnNew AosBuff(mCompactedBuffMaxSize);
	aos_assert_rr(mCompactedBuff.notNull(), rdata, false);
	//update headers of the docs in the old compacted buff
	BodyEntryHead* pEntry = (BodyEntryHead*)oldCompactedBuff->data();
	DeletionLogEntry* pEntryInNewDelLog = NULL;
	aos_assert_rr(pEntry, rdata, false);
	i64 pos = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderSize);	//TODO:
	aos_assert_rr(pHeader.notNull(), rdata, false);
	u64 snap_id = pEntry->ullSnapID;	//TODO:how to deal with it?
	while(pos < oldCompactedBuff->dataLen())
	{
		pEntry = (BodyEntryHead*)(oldCompactedBuff->data() + pos);
		snap_id = pEntry->ullSnapID;
		if (NULL == (pEntryInNewDelLog = findEntryWithinNewDelLog(rdata, pEntry->ullDocid)))
		{
			//update the header in header cache
			pHeader->setDocid(pEntry->ullDocid);
			pHeader->setBodyRawfid(mCompactedFileID);
			pHeader->setBodyOffset(pos);
			if (0 != (iRet = mHeaderCache->updateHeader(rdata, pHeader)))
			{
				OmnAlarm << "update header failed for compaction, docid:" << pEntry->ullDocid
						<< " snap_id:" << snap_id << enderr;
				return false;
			}
		}
		else
		{
			//update the entry in new del log
			pEntryInNewDelLog->ullRawfid = mCompactedFileID;		//TODO:this is fucking ugly
			pEntryInNewDelLog->ullOffset = pos;
		}
		pos += pEntry->ulLength;
	}
	mCompactedFileID++;
	bool rslt = removeDeletionLogFiles(rdata);		//TODO:
	aos_assert_rr(rslt, rdata, false);
	rslt = removeCompactedFiles(rdata);
	aos_assert_rr(rslt, rdata, false);
	return true;
}

/*
AosDfmDocHeaderPtr
AosDeletionLog::getHeaderFromEntry(
		AosRundata	*rdata,
		const char	*entry)
{
	AosDfmDocHeaderPtr header = mHeader->copy();	//TODO:is this right?
	if (header.isNull())
	{
		OmnAlarm << "mHeader->copy() failed" << enderr;
		return NULL;
	}
	AosBuffPtr buff = OmnNew AosBuff();
	aos_assert_rr(buff.notNull(), rdata, NULL);
	buff->setBuff(entry, mHeaderSize);
	header->resetBuff(buff);
	return header;
}
*/

DeletionLogEntry*
AosDeletionLog::findEntryWithinNewDelLog(
		AosRundata	*rdata,
		const u64	docid)
{
	/*
	 * this function is supposed to check if the given rawfid and offset is within
	 * the new del log.
	 * 1.search in the new del log buffer
	 */

	return NULL;
}


bool
AosDeletionLog::removeCompactedFiles(AosRundata *rdata)
{
	int iRet = 0;
	for (std::vector<u64>::iterator it = mvCompactedRawfids.begin();it != mvCompactedRawfids.end();it++)
	{
		iRet = mRawSE->deleteFile(rdata, mSiteID, mCubeID, mAseID, *it, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "delete file with rawfid:" << *it << " failed, iRet=" << iRet << enderr;
		}
	}
	mvCompactedRawfids.clear();
	return true;
}
#endif
