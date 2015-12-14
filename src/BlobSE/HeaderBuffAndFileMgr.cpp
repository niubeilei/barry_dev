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
// 18 Jun 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/HeaderBuffAndFileMgr.h"

#include "Alarm/Alarm.h"
#include "BlobSE/Compaction.h"
#include "BlobSE/HeaderFileMgr.h"
#include "RawSE/RawFile.h"
#include "Util/OmnNew.h"

AosHeaderBuffAndFileMgr::AosHeaderBuffAndFileMgr(
		AosRundata*					rdata,
		const AosHeaderFileMgrPtr	&pHeaderFileMgr,
		const AosXmlTagPtr			&conf)
:
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mHeaderSaved(0)
{
	AosXmlTagPtr config = conf->getFirstChild("HeaderBuffAndFileMgr", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderBuffAndFileMgr");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeaderCacheSize = config->getAttrU32("dirty_header_cache_size", eDirtyHeaderCacheSize);
	mHeaderFlushThreshold = config->getAttrU32("cache_flush_threshold", eHeaderFlushThreshold);
	config = conf->getFirstChild("HeaderFile", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeaderFileMgr = pHeaderFileMgr;
	mHeaderFileSizeThreshold = config->getAttrU32("old_header_file_size", AosHeaderFileMgr::eOldHeaderFileSizeThreshold);
	config = conf->getFirstChild("Compaction", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mCompactionTimeWindowInUnits =
			config->getAttrU32("compaction_time_windows_in_units", AosCompaction::eCompactionTimeWindowInUnits);
	if (AosCompaction::eCompactionTimeWindowInUnits == mCompactionTimeWindowInUnits)
	{
		OmnScreen << "mCompactionTimeWindowInUnits set to default value:"
				<< mCompactionTimeWindowInUnits << ", please make sure this is what you want." << endl;
	}
}


AosHeaderBuffAndFileMgr::~AosHeaderBuffAndFileMgr()
{
}


int
AosHeaderBuffAndFileMgr::getEpochUnitInfoByTimestampNoLock(
		AosRundata		*rdata,
		const u64		timestamp,
		EpochUnitInfo	&sEpochUnitInfo)
{
	u32 ulEpochUnit = getExpirationUnitByTimestamp(timestamp);
	map_t::iterator it = mEpocUnitMap.find(ulEpochUnit);
	int iRet = 0;
	if (mEpocUnitMap.end() == it)
	{
		iRet = mHeaderFileMgr->getOldHeaderFileSeqnosByEpochUnitFromMetafile(rdata,
				getExpirationUnitByTimestamp(timestamp), sEpochUnitInfo.ulHeaderFileForFlushingSeqno,
				sEpochUnitInfo.ulHeaderFileForWorkingSeqno, sEpochUnitInfo.vExistingSeqno);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->getLastOldHeaderFileSeqnosFromMetafile failed, iRet=" << iRet << enderr;
			return -1;
		}
		sEpochUnitInfo.pHeaderFileForFlushing =
				mHeaderFileMgr->getOldHeaderFileBySeqno(rdata, sEpochUnitInfo.ulHeaderFileForFlushingSeqno);
		if (sEpochUnitInfo.pHeaderFileForFlushing.isNull())
		{
			OmnAlarm << "mHeaderFileMgr->getOldHeaderFileBySeqno failed, seqno:"
					<< sEpochUnitInfo.ulHeaderFileForFlushingSeqno << enderr;
			return -2;
		}
		sEpochUnitInfo.ullHeaderFileForFlushingLen = sEpochUnitInfo.pHeaderFileForFlushing->getLength();
		//TODO:needs to check whether ullHeaderFileForFlushingLen is smaller than mHeaderFileSizeThreshold
		sEpochUnitInfo.pHeaderCacheBuffForFlushing =
				OmnNew AosBuff(mHeaderFileSizeThreshold - sEpochUnitInfo.ullHeaderFileForFlushingLen AosMemoryCheckerArgs);
		if (sEpochUnitInfo.pHeaderCacheBuffForFlushing.isNull())
		{
			OmnAlarm << "failed to create sEpochUnitInfo.pHeaderCacheBuffForFlushing obj,"
					<< " mHeaderFileSizeThreshold:" << mHeaderFileSizeThreshold
					<< " ullHeaderFileForFlushingLen:" << sEpochUnitInfo.ullHeaderFileForFlushingLen << enderr;
			return -3;
		}
		sEpochUnitInfo.pHeaderFileForWorking =
				mHeaderFileMgr->getOldHeaderFileBySeqno(rdata, sEpochUnitInfo.ulHeaderFileForWorkingSeqno);
		if (sEpochUnitInfo.pHeaderFileForWorking.isNull())
		{
			OmnAlarm << "mHeaderFileMgr->getOldHeaderFileBySeqno failed, seqno:"
					<< sEpochUnitInfo.ulHeaderFileForWorkingSeqno << enderr;
			return -4;
		}
		sEpochUnitInfo.ullHeaderFileForWorkingLen = sEpochUnitInfo.pHeaderFileForWorking->getLength();
		sEpochUnitInfo.pHeaderCacheBuffForWorking = OmnNew AosBuff(mHeaderCacheSize AosMemoryCheckerArgs);
		if (sEpochUnitInfo.pHeaderCacheBuffForWorking.isNull())
		{
			OmnAlarm << "failed to create sEpochUnitInfo.pHeaderCacheBuffForWorking obj,"
					<< " mHeaderFileSizeThreshold:" << mHeaderFileSizeThreshold
					<< " ullHeaderFileForWorkingLen:" << sEpochUnitInfo.ullHeaderFileForWorkingLen << enderr;
			return -5;
		}
		mEpocUnitMap[ulEpochUnit] = sEpochUnitInfo;
	}
	else
	{
		sEpochUnitInfo = it->second;
	}
	return 0;
}


int
AosHeaderBuffAndFileMgr::saveHeaderToCacheBuff(
		AosRundata						*rdata,
		const AosBlobHeaderPtr			&pHeader,
		AosBlobHeader::HeaderPointer	&sHeaderPointer)
{
	EpochUnitInfo sEpochUnitInfo;
	int iRet = getEpochUnitInfoByTimestampNoLock(rdata, pHeader->getTimestamp(), sEpochUnitInfo);
	if (0 != iRet)
	{
		OmnAlarm << "getEpochUnitInfoByTimestampNoLock failed, iRet=" << iRet
				<< " timestamp:" << pHeader->getTimestamp() << enderr;
		return -1;
	}
	sHeaderPointer.ulSeqno = sEpochUnitInfo.ulHeaderFileForWorkingSeqno;
	sHeaderPointer.ulOffset =
			(u32)sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen() + (u32)sEpochUnitInfo.ullHeaderFileForWorkingLen;
	if (sEpochUnitInfo.pHeaderCacheBuffForWorking->buffLen() <
			sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength())
	{
		OmnAlarm << "sEpochUnitInfo.pHeaderCacheBuffForWorking->buffLen():" << sEpochUnitInfo.pHeaderCacheBuffForWorking->buffLen()
				<< " < sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen():" << sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen()
				<< " + pHeader->getHeaderInCacheAndHeaderFileLength():"
				<< pHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
		return -2;
	}
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			sEpochUnitInfo.pHeaderCacheBuffForWorking->data() + sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "serializeToCacheAndHeaderFileBuff failed for docid:" << pHeader->getDocid()
				<< ", iRet=" << iRet << enderr;
		return -3;
	}
	sEpochUnitInfo.pHeaderCacheBuffForWorking->setDataLen(
			sEpochUnitInfo.pHeaderCacheBuffForWorking->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength());
	mHeaderSaved++;
	return 0;
}


int
AosHeaderBuffAndFileMgr::switchHeaderBuffAndFile(AosRundata *rdata)
{
	dumpEpocDayMap(__FUNCTION__);
	for (map_t::iterator it = mEpocUnitMap.begin(); it != mEpocUnitMap.end(); it++)
	{
		it->second.pHeaderCacheBuffForFlushing = it->second.pHeaderCacheBuffForWorking;
		it->second.pHeaderCacheBuffForWorking = OmnNew AosBuff(mHeaderCacheSize AosMemoryCheckerArgs);
		if (it->second.pHeaderCacheBuffForWorking.isNull())
		{
			OmnAlarm << "create new mHeaderCacheBuffForWorking failed, mHeaderCacheSize:" << mHeaderCacheSize << enderr;
			return -1;
		}
		it->second.pHeaderCacheBuffForWorking->setDataLen(0);
		//swap
		u32 ulTempSeqno = it->second.ulHeaderFileForFlushingSeqno;
		it->second.ulHeaderFileForFlushingSeqno = it->second.ulHeaderFileForWorkingSeqno;
		it->second.ulHeaderFileForWorkingSeqno = ulTempSeqno;
		AosRawFilePtr pTempFile = it->second.pHeaderFileForFlushing;
		it->second.pHeaderFileForFlushing = it->second.pHeaderFileForWorking;
		it->second.pHeaderFileForWorking = pTempFile;
		u64 ullTempLen = it->second.ullHeaderFileForWorkingLen;
		it->second.ullHeaderFileForWorkingLen = it->second.ullHeaderFileForFlushingLen;
		it->second.ullHeaderFileForFlushingLen = ullTempLen;
		int iRet = saveOldHeaderFileSeqnosToMetafile(rdata, it->second.ulHeaderFileForFlushingSeqno,
				it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
		if (0 != iRet)
		{
			OmnAlarm << "saveOldHeaderFileSeqnosToMetafile failed, iRet=" << iRet << enderr;
			return -2;
		}
	}
	dumpEpocDayMap(__FUNCTION__);
	mHeaderSaved = 0;
	return 0;
}


int
AosHeaderBuffAndFileMgr::saveOldHeaderFileSeqnosToMetafile(
		AosRundata				*rdata,
		const u32				ulSeqnoForFlushing,
		const u32				ulSeqnoForWorking,
		const std::vector<u32>	&vExistingSeqno)
{
	return mHeaderFileMgr->saveOldHeaderFileSeqnosToMetafile(
			rdata, ulSeqnoForFlushing, ulSeqnoForWorking, vExistingSeqno);
}


int
AosHeaderBuffAndFileMgr::appendHeaderToBuffForFlushingNoLock(
		AosRundata						*rdata,
		const AosBlobHeaderPtr			&pHeader,
		AosBlobHeader::HeaderPointer	&sNextHeader)
{
	EpochUnitInfo sEpochUnitInfo;
	int iRet = getEpochUnitInfoByTimestampNoLock(rdata, pHeader->getTimestamp(), sEpochUnitInfo);
	if (0 != iRet)
	{
		OmnAlarm << "getEpochUnitInfoByTimestampNoLock failed, iRet=" << iRet
				<< " timestamp:" << pHeader->getTimestamp() << enderr;
		return -1;
	}
	if (sEpochUnitInfo.pHeaderCacheBuffForFlushing.isNull())
	{
		OmnAlarm << "sEpochUnitInfo.pHeaderCacheBuffForFlushing.isNull()" << enderr;
		return -2;
	}
	if (sEpochUnitInfo.pHeaderCacheBuffForFlushing->buffLen() <=
			sEpochUnitInfo.pHeaderCacheBuffForFlushing->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength())
	{
		OmnAlarm << "mHeaderCacheBuffForFlushing->buffLen():" << sEpochUnitInfo.pHeaderCacheBuffForFlushing->buffLen()
				<< " <= mHeaderCacheBuffForFlushing->dataLen():" << sEpochUnitInfo.pHeaderCacheBuffForFlushing->dataLen()
				<< " + pHeader->getHeaderInCacheAndHeaderFileLength():"
				<< pHeader->getHeaderInCacheAndHeaderFileLength()<< enderr;
		if (!sEpochUnitInfo.pHeaderCacheBuffForFlushing->expandMemory1(sEpochUnitInfo.pHeaderCacheBuffForFlushing->buffLen() * 2))
		{
			OmnAlarm << "mHeaderCacheBuffForFlushing->expandMemory1 failed, trying to expand memory size to:"
					<< sEpochUnitInfo.pHeaderCacheBuffForFlushing->buffLen() * 2 << enderr;
			return -3;
		}
	}
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			sEpochUnitInfo.pHeaderCacheBuffForFlushing->data() + sEpochUnitInfo.pHeaderCacheBuffForFlushing->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet << enderr;
		return -4;
	}
	sNextHeader.ulSeqno = sEpochUnitInfo.ulHeaderFileForFlushingSeqno;
	sNextHeader.ulOffset = (u32)sEpochUnitInfo.pHeaderCacheBuffForFlushing->dataLen() + sEpochUnitInfo.ullHeaderFileForFlushingLen;
	sEpochUnitInfo.pHeaderCacheBuffForFlushing->setDataLen(
			sEpochUnitInfo.pHeaderCacheBuffForFlushing->dataLen() + pHeader->getHeaderInCacheAndHeaderFileLength());
	return 0;
}


int
AosHeaderBuffAndFileMgr::getTheOldestHeaderByNextHeaderPointerNoLock(
		AosRundata						*rdata,
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sNextHeader,
		AosBlobHeaderPtr				&pHeader)
{
	int iRet = 0;
	if (!isHeaderInBuffToFlush(sNextHeader))
		//this should only happen on the doc whose modification triggers the flushing thread
	{
		OmnScreen << "header not in buff to flush, docid:" << docid << " nextheader:" << sNextHeader.toStr()
				<< " header in cache size:" << pHeader->getHeaderInCacheAndHeaderFileLength() << endl;
		iRet = getTheOldestHeaderInCacheBuffForWorkingNoLock(docid, sNextHeader, pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "header not in buff for working, docid:" << docid << " nextheader:" << sNextHeader.toStr()
					<< " header in cache size:" << pHeader->getHeaderInCacheAndHeaderFileLength() << enderr;
			dumpEpocDayMap(__FUNCTION__);
			return -1;
		}
		if (AosBlobHeader::unresolvedNextHeader == pHeader->getNextHeaderPtr())
		{
			return 0;
		}
		if (!isHeaderInBuffToFlush(pHeader->getNextHeaderPtr()) && AosBlobHeader::noNextHeader != pHeader->getNextHeaderPtr())
			//the oldest header in buff to work's next header must be in buff to flush
		{
			OmnScreen << "header not in buff to flush, docid:" << docid << " nextheader:" << pHeader->getNextHeaderPtr().toStr()
					<< " header in cache size:" << pHeader->getHeaderInCacheAndHeaderFileLength() << endl;
			return 0;		//TODO:this may be a problem, needs to test thoroughly
		}
		sNextHeader = pHeader->getNextHeaderPtr();
	}
	while(isHeaderInBuffToFlush(sNextHeader))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForFlushingPointerByHeaderPointerNoLock(sNextHeader));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sNextHeader.toStr() << " iRet=" << iRet << enderr;
			return -2;
		}
		if (!isHeaderInBuffToFlush(pHeader->getNextHeaderPtr()))
		{
			return 0;
		}
		sNextHeader = pHeader->getNextHeaderPtr();
	}
	return 0;
}


bool
AosHeaderBuffAndFileMgr::isHeaderCachedInWorkingBuff(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	if (AosBlobHeader::noNextHeader == sNextHeader || AosBlobHeader::unresolvedNextHeader == sNextHeader
			|| AosBlobHeader::noPrevHeader == sNextHeader)
	{
		return false;
	}
	map_t::iterator it = mEpocUnitMap.find(mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno));
	if (mEpocUnitMap.end() == it)
	{
		OmnScreen << "sNextHeader:" << sNextHeader.toStr()
				<< " mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno):"
				<< mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno) << endl;
		return false;
	}
	return sNextHeader.ulSeqno == it->second.ulHeaderFileForWorkingSeqno && sNextHeader != AosBlobHeader::noNextHeader
			&& sNextHeader.ulOffset >= it->second.ullHeaderFileForWorkingLen &&
			(i64)(sNextHeader.ulOffset - it->second.ullHeaderFileForWorkingLen) < it->second.pHeaderCacheBuffForWorking->dataLen();
}


bool
AosHeaderBuffAndFileMgr::isHeaderInBuffToFlush(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	if (AosBlobHeader::noNextHeader == sNextHeader || AosBlobHeader::unresolvedNextHeader == sNextHeader
			|| AosBlobHeader::noPrevHeader == sNextHeader)
	{
		return false;
	}
	map_t::iterator it = mEpocUnitMap.find(mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno));
	if (mEpocUnitMap.end() == it)
	{
		OmnScreen << "sNextHeader:" << sNextHeader.toStr()
				<< " mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno):"
				<< mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno) << endl;
		return false;
	}
	return sNextHeader.ulSeqno == it->second.ulHeaderFileForFlushingSeqno && sNextHeader != AosBlobHeader::noNextHeader
			&& sNextHeader.ulOffset >= it->second.ullHeaderFileForFlushingLen &&
			(i64)(sNextHeader.ulOffset - it->second.ullHeaderFileForFlushingLen) < it->second.pHeaderCacheBuffForFlushing->dataLen();
}


int
AosHeaderBuffAndFileMgr::getTheOldestHeaderInCacheBuffForWorkingNoLock(
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sNextHeader,
		AosBlobHeaderPtr				&pHeader)
{
	if (!isHeaderCachedInWorkingBuff(sNextHeader))
	{
		OmnAlarm << "header not in buff for working, docid:" << docid << " nextheader:" << sNextHeader.toStr()
//				<< " mHeaderFileForFlushingSeqno:" << mHeaderFileForFlushingSeqno
//				<< " mHeaderFileForFlushing->getLength():" << mHeaderFileForFlushing->getLength()
//				<< " mHeaderCacheBuffForFlushing->dataLen():" << mHeaderCacheBuffForFlushing->dataLen()
//				<< " mHeaderFileForWorkingSeqno:" << mHeaderFileForWorkingSeqno
//				<< " mHeaderFileForWorking->getLength():" << mHeaderFileForWorking->getLength()
//				<< " mHeaderCacheBuffForWorking->dataLen():" << mHeaderCacheBuffForWorking->dataLen()
				<< enderr;
		return -1;
	}
	int iRet = 0;
	while(isHeaderCachedInWorkingBuff(sNextHeader))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sNextHeader));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sNextHeader.toStr() << " iRet=" << iRet << enderr;
			return -2;
		}
		if (!isHeaderCachedInWorkingBuff(pHeader->getNextHeaderPtr()))
		{
			return 0;
		}
		sNextHeader = pHeader->getNextHeaderPtr();
	};
	return 0;
}


char*
AosHeaderBuffAndFileMgr::getHeaderCacheForWorkingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	map_t::iterator it = mEpocUnitMap.find(mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno));
	if (mEpocUnitMap.end() == it)
	{
		return NULL;
	}
	if (it->second.pHeaderCacheBuffForWorking->dataLen() < (i64)(sNextHeader.ulOffset - it->second.ullHeaderFileForWorkingLen))
		//ullHeaderFileForWorkingLen should not be bigger than a u32 can hold
	{
		OmnAlarm << "it->second.pHeaderCacheBuffForWorking->dataLen():" << it->second.pHeaderCacheBuffForWorking->dataLen()
				<< " < offset:" << sNextHeader.ulOffset - it->second.ullHeaderFileForWorkingLen
				<< " header pointer:" << sNextHeader.toStr()
				<< " it->second.ullHeaderFileForWorkingLen:" << it->second.ullHeaderFileForWorkingLen << enderr;
		return NULL;
	}
	return it->second.pHeaderCacheBuffForWorking->data() + sNextHeader.ulOffset - it->second.ullHeaderFileForWorkingLen;
}


char*
AosHeaderBuffAndFileMgr::getHeaderCacheForFlushingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	map_t::iterator it = mEpocUnitMap.find(mHeaderFileMgr->getEpochUnitBySeqno(sNextHeader.ulSeqno));
	if (mEpocUnitMap.end() == it)
	{
		return NULL;
	}
	if (it->second.pHeaderCacheBuffForFlushing->dataLen() < (i64)(sNextHeader.ulOffset - it->second.ullHeaderFileForFlushingLen))
		//ullHeaderFileForFlushingLen should not be bigger than a u32 can hold
	{
		OmnAlarm << "it->second.pHeaderCacheBuffForFlushing->dataLen():" << it->second.pHeaderCacheBuffForFlushing->dataLen()
				<< " < offset:" << sNextHeader.ulOffset - it->second.ullHeaderFileForFlushingLen
				<< " header pointer:" << sNextHeader.toStr()
				<< " it->second.ullHeaderFileForFlushingLen:" << it->second.ullHeaderFileForFlushingLen << enderr;
		return NULL;
	}
	return it->second.pHeaderCacheBuffForFlushing->data() + sNextHeader.ulOffset - it->second.ullHeaderFileForFlushingLen;
}


bool
AosHeaderBuffAndFileMgr::needToFlush()
{
	return mHeaderSaved >= mHeaderFlushThreshold;
}


int
AosHeaderBuffAndFileMgr::saveDirtyHeaderCacheIntoOldHeaderFile(AosRundata *rdata)
{
	OmnScreen << "saveDirtyHeaderCacheIntoOldHeaderFile" << endl;
	int iRet = 0;
	for (map_t::iterator it = mEpocUnitMap.begin(); it != mEpocUnitMap.end(); it++)
	{
		if (it->second.pHeaderCacheBuffForFlushing->dataLen() > 0)
		{
			it->second.pHeaderFileForFlushing->lock();
			iRet = it->second.pHeaderFileForFlushing->append(rdata,
					it->second.pHeaderCacheBuffForFlushing->data(), it->second.pHeaderCacheBuffForFlushing->dataLen());
		}
		if (0 != iRet)
		{
			OmnAlarm << "it->second.pHeaderFileForFlushing->append failed, iRet=" << iRet
					<< " it->first, i.e.,epoch day:" << it->first
					<< " it->second.pHeaderCacheBuffForFlushing->dataLen():" << it->second.pHeaderCacheBuffForFlushing->dataLen() << enderr;
			it->second.pHeaderFileForFlushing->unlock();
			return -1;
		}
		it->second.ullHeaderFileForFlushingLen += it->second.pHeaderCacheBuffForFlushing->dataLen();
		it->second.pHeaderFileForFlushing->unlock();
		it->second.pHeaderCacheBuffForFlushing->setDataLen(0);
	}
	return 0;
}


int
AosHeaderBuffAndFileMgr::switchHeaderFileForFlushingNoLock(AosRundata *rdata)
{
	for (map_t::iterator it = mEpocUnitMap.begin(); it != mEpocUnitMap.end(); it++)
	{
		if (it->second.ullHeaderFileForFlushingLen >= mHeaderFileSizeThreshold)
		{
			it->second.vExistingSeqno.push_back(it->second.ulHeaderFileForFlushingSeqno);
			std::sort(it->second.vExistingSeqno.begin(), it->second.vExistingSeqno.end());
			u32 ulMinAvailableSeqno = mHeaderFileMgr->getMinAvailableSeqnoOfEpochUnit(it->first,
					it->second.ulHeaderFileForFlushingSeqno, it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
			if (0 == ulMinAvailableSeqno)
			{
				OmnAlarm << "getMinAvailableOverflowIDOfEpochUnit failed, epoch day:" << it->first << enderr;
				return -1;
			}
			if (ulMinAvailableSeqno == it->second.ulHeaderFileForFlushingSeqno ||
					ulMinAvailableSeqno == it->second.ulHeaderFileForWorkingSeqno)
			{
				OmnAlarm << "ulMinAvailableSeqno:" << ulMinAvailableSeqno << " == ulHeaderFileForFlushingSeqno:"
						<< it->second.ulHeaderFileForFlushingSeqno << " or ulHeaderFileForWorkingSeqno:"
						<< it->second.ulHeaderFileForWorkingSeqno << enderr;
				return -2;
			}
			it->second.ulHeaderFileForFlushingSeqno = ulMinAvailableSeqno;
			it->second.pHeaderFileForFlushing =
					mHeaderFileMgr->getOldHeaderFileBySeqno(rdata, it->second.ulHeaderFileForFlushingSeqno);
			if (it->second.pHeaderFileForFlushing.isNull())
			{
				OmnAlarm << "mHeaderFileMgr->getHeaderFileBySeqno failed, seqno:"
						<< it->second.ulHeaderFileForFlushingSeqno << enderr;
				return -3;
			}
			it->second.ullHeaderFileForFlushingLen = it->second.pHeaderFileForFlushing->getLength();
			int iRet = saveOldHeaderFileSeqnosToMetafile(rdata, it->second.ulHeaderFileForFlushingSeqno,
					it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
			if (0 != iRet)
			{
				OmnAlarm << "saveOldHeaderFileSeqnosToMetafile failed, iRet=" << iRet << " epoch day:" << it->first
						<< " ulHeaderFileForFlushingSeqno:" << it->second.ulHeaderFileForFlushingSeqno
						<< " ulHeaderFileForWorkingSeqno:" << it->second.ulHeaderFileForWorkingSeqno << enderr;
				return -4;
			}
		}
	}
	return 0;
}


void
AosHeaderBuffAndFileMgr::dumpEpocDayMap(const char* pCaller)
{
	OmnScreen << "dumping EpocDayMap in " << pCaller << " with size:" << mEpocUnitMap.size() << "..." << endl;
	for (map_t::iterator it = mEpocUnitMap.begin(); it != mEpocUnitMap.end(); it++)
	{
		OmnString s = "";
		for (std::vector<u32>::iterator i = it->second.vExistingSeqno.begin(); i != it->second.vExistingSeqno.end(); i++)
		{
			s << "seqno:"<< *i << " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(*i) << "\n";
		}
		OmnScreen << "epoch day:" << it->first << endl
				<< "            ullHeaderFileForWorkingLen:" << it->second.ullHeaderFileForWorkingLen << endl
				<< "           ullHeaderFileForFlushingLen:" << it->second.ullHeaderFileForFlushingLen << endl
				<< "          ulHeaderFileForFlushingSeqno:" << it->second.ulHeaderFileForFlushingSeqno << endl
				<< "  pHeaderFileForFlushing->getRawFileID:" << it->second.pHeaderFileForFlushing->getRawFileID() << endl
				<< "           ulHeaderFileForWorkingSeqno:" << it->second.ulHeaderFileForWorkingSeqno << endl
				<< "   pHeaderFileForWorking->getRawFileID:" << it->second.pHeaderFileForWorking->getRawFileID() << endl
				<< " pHeaderCacheBuffForWorking->dataLen():" << it->second.pHeaderCacheBuffForWorking->dataLen() << endl
				<< "pHeaderCacheBuffForFlushing->dataLen():" << it->second.pHeaderCacheBuffForFlushing->dataLen() << endl
				<< " existing seqno:\n" << s << endl;
	}
}


u32
AosHeaderBuffAndFileMgr::getMinAvailableSeqnoOfEpochUnit(
		AosRundata	*rdata,
		const u32	ulEpochUnit)
{
	map_t::iterator it = mEpocUnitMap.find(ulEpochUnit);
	if (mEpocUnitMap.end() == it)
	{
		OmnAlarm << "can not get epoch day info for epoch day:" << ulEpochUnit << enderr;
		dumpEpocDayMap(__FUNCTION__);
		return 0;
	}
	else
	{
		u32 ulSeqno = mHeaderFileMgr->getMinAvailableSeqnoOfEpochUnit(ulEpochUnit, it->second.ulHeaderFileForFlushingSeqno,
				it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
		if (0 == ulSeqno)
		{
			OmnAlarm << "getMinAvailableOverflowIDOfEpochUnit failed, ulEpochUnit:" << ulEpochUnit << enderr;
			return 0;
		}
		else
		{
			it->second.vExistingSeqno.push_back(ulSeqno);
			std::sort(it->second.vExistingSeqno.begin(), it->second.vExistingSeqno.end());
			int iRet = saveOldHeaderFileSeqnosToMetafile(rdata, it->second.ulHeaderFileForFlushingSeqno,
					it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
			if (0 != iRet)
			{
				OmnAlarm << "saveOldHeaderFileSeqnosToMetafile failed, iRet=" << iRet
						<< " epoch day:" << ulEpochUnit << " seqno:" << ulSeqno << enderr;
				return -2;
			}
			return ulSeqno;
		}
	}
}


int
AosHeaderBuffAndFileMgr::delSeqnoInEpochUnitInfo(
		AosRundata	*rdata,
		const u32	ulSeqno)
{
	u32 ulEpochUnit = mHeaderFileMgr->getEpochUnitBySeqno(ulSeqno);
	map_t::iterator it = mEpocUnitMap.find(ulEpochUnit);
	if (mEpocUnitMap.end() == it)
	{
		OmnAlarm << "can not get epoch day info for epoch day:" << ulEpochUnit << " seqno:" << ulSeqno << enderr;
		dumpEpocDayMap(__FUNCTION__);
		return -1;
	}
	else
	{
		for (std::vector<u32>::iterator i = it->second.vExistingSeqno.begin(); i != it->second.vExistingSeqno.end(); i++)
		{
			if (ulSeqno == *i)
			{
				it->second.vExistingSeqno.erase(i);
				int iRet = saveOldHeaderFileSeqnosToMetafile(rdata, it->second.ulHeaderFileForFlushingSeqno,
						it->second.ulHeaderFileForWorkingSeqno, it->second.vExistingSeqno);
				if (0 != iRet)
				{
					OmnAlarm << "saveOldHeaderFileSeqnosToMetafile failed, iRet=" << iRet
							<< " epoch day:" << ulEpochUnit << " seqno:" << ulSeqno << enderr;
					return -2;
				}
				return 0;
			}
		}
		OmnAlarm << "can not find seqno in epoch day info, seqno:" << ulSeqno << enderr;
		dumpEpocDayMap(__FUNCTION__);
		return -3;
	}
}


int
AosHeaderBuffAndFileMgr::allocateSpaceForRecreatedHeaders(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
//	u32 ulEpochUnit = mHeaderFileMgr->getEpochUnitByTimestamp(pHeader->getTimestamp());
//	map_recover_t::iterator it = mRecoverMap.find(ulEpochUnit);
//	if (mRecoverMap.end() == it)
//	{
//		EpochUnitInfoForRecover s;
//		s.ulHeaderFileSeqno = getMinAvailableSeqnoOfEpochUnit(rdata, ulEpochUnit);
//		if (0 == s.ulHeaderFileSeqno)
//		{
//			OmnAlarm << "getMinAvailableOverflowIDOfEpochUnit failed for epoch day:" << ulEpochUnit << enderr;
//			return -1;
//		}
//		s.ulHeaderFileLen = 0;
//		s.pHeaderFile = mHeaderFileMgr->getOldHeaderFileBySeqno(rdata, s.ulHeaderFileSeqno);
//		AosBlobHeader::HeaderPointer sHeaderPointer = {s.ulHeaderFileSeqno, s.ulHeaderFileLen};
//		pHeader->setSelfPos(sHeaderPointer);
//		s.ulHeaderFileLen += pHeader->getHeaderInCacheAndHeaderFileLength();
//		s.pBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
//		mRecoverMap[ulEpochUnit] = s;
//	}
//	else
//	{
//		AosBlobHeader::HeaderPointer sHeaderPointer = {it->second.ulHeaderFileSeqno, it->second.ulHeaderFileLen};
//		pHeader->setSelfPos(sHeaderPointer);
//		it->second.ulHeaderFileLen += pHeader->getHeaderInCacheAndHeaderFileLength();
//	}
	return 0;
}


int
AosHeaderBuffAndFileMgr::saveRecreatedHeaderToBuff(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
//	u32 ulEpochUnit = getExpirationUnitByTimestamp(pHeader->getTimestamp());
//	map_recover_t::iterator it = mRecoverMap.find(ulEpochUnit);
//	if (mRecoverMap.end() == it)
//	{
//		OmnAlarm << "can not find epoch day info for epoch day:" << ulEpochUnit << enderr;
//		return -1;
//	}
//	else
//	{
//		if (pHeader->getSelfPos().ulOffset >= it->second.ulHeaderFileLen)
//		{
//			OmnAlarm << "pHeader->getSelfPos().ulOffset:" << pHeader->getSelfPos().ulOffset
//					<< " >= it->second.ulHeaderFileLen:" << it->second.ulHeaderFileLen
//					<< " epoch day:" << ulEpochUnit << enderr;
//			return -2;
//		}
//		if (it->second.pBuff->buffLen() < (i64)it->second.ulHeaderFileLen)
//		{
//			if (!it->second.pBuff->expandMemory1(it->second.ulHeaderFileLen * 2))
//			{
//				OmnAlarm << "it->second.pBuff->expandMemory1 failed, try to expand to size:"
//						<< it->second.ulHeaderFileLen * 2 << enderr;
//				return -3;
//			}
//		}
//		int iRet = pHeader->serializeToCacheAndHeaderFileBuff(
//				it->second.pBuff->data() + pHeader->getSelfPos().ulOffset);
//		if (0 != iRet)
//		{
//			OmnAlarm << "serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
//					<< " offset:" << pHeader->getSelfPos().ulOffset << enderr;
//			return -4;
//		}
//	}
	return 0;
}


int
AosHeaderBuffAndFileMgr::flushRecreatedHeaders(AosRundata *rdata)
{
//	int iRet = 0;
//	for (map_recover_t::iterator it = mRecoverMap.begin(); it != mRecoverMap.end(); it++)
//	{
//		iRet = it->second.pHeaderFile->append(rdata, it->second.pBuff->data(), it->second.ulHeaderFileLen);
//		if (0 != iRet)
//		{
//			OmnAlarm << "append failed, iRet=" << iRet << " rawfid:" << it->second.pHeaderFile->getRawFileID()
//					<< " append len:" << it->second.ulHeaderFileLen << " epoch day:" << it->first << enderr;
//			return -1;
//		}
//	}
	return 0;
}


u32 AosHeaderBuffAndFileMgr::getExpirationUnitByTimestamp(const u64 timestamp)
{
	u32 ulEpochUnit = mHeaderFileMgr->getEpochUnitByTimestamp(timestamp);
	return ulEpochUnit + mCompactionTimeWindowInUnits;
}
