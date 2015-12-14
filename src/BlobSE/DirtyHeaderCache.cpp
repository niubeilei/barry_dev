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
#include "BlobSE/DirtyHeaderCache.h"

#include "BlobSE/BlobHeader.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/Entry.h"
#include "BlobSE/HeaderBuffAndFileMgr.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "Porting/Sleep.h"
#include "RawSE/RawFile.h"
#include "Thread/ThreadPool.h"
#include "Util1/Timer.h"
AosDirtyHeaderCache::AosDirtyHeaderCache(
		AosRundata*							rdata,
		const u32							ulHeaderCustomDataSize,
		const AosHeaderFileMgrPtr			&pHeaderFileMgr,
		const AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
		AosBlobSE*							pBlobSE,
		AosHeaderCache*						pHeaderCache,
		const AosXmlTagPtr					&conf)
:
OmnThrdShellProc("BlobSEDirtyHeaderCache"),
mHeaderCustomDataSize(ulHeaderCustomDataSize),
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mBlobSE(pBlobSE),
mRdata(rdata),
mIsFlushingHeaders(false),
mHeaderFileMgr(pHeaderFileMgr),
mHeaderCache(pHeaderCache)
{
	AosXmlTagPtr config = conf->getFirstChild("DirtyHeaderCache", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named DirtyHeaderCache");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mFlushingWaitForComapctionToFinishTime =
			config->getAttrU32("flush_wait_for_compaction_to_finish", eFlushingWaitForComapctionToFinishTime);
	mFlushDirtyHeaderInterval =
			config->getAttrU32("flush_dirty_header_interval", eFlushDirtyHeaderInterval);
	mHeaderBuffAndFileMgr = pHeaderBuffAndFileMgr;
	OmnScreen << "mFlushDirtyHeaderInterval:" << mFlushDirtyHeaderInterval << endl;
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("FlushingTimer", mFlushDirtyHeaderInterval, 0, thisptr, 0);
}


AosDirtyHeaderCache::~AosDirtyHeaderCache()
{
    // TODO Auto-generated destructor stub
}


AosBlobHeaderPtr
AosDirtyHeaderCache::readHeader(
		AosRundata						*rdata,
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sNextHeader,
		const u64						ullTimestamp)
{
	/*
	 * return the header if it is in the dirty header cache and
	 * 1. it is older than timestamp
	 * 2. its next header pointer is noNextHeader
	 * 3. it is the oldest header in dirty header cache
	 */
	mLockRaw->lock();
	if (!isHeaderCachedInWorkingBuff(sNextHeader) && !isHeaderInBuffToFlush(sNextHeader))
	{
		mLockRaw->unlock();
		return NULL;
	}
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for docid:" << docid << enderr;
		mLockRaw->unlock();
		return NULL;
	}
	int iRet = 0;
	if (isHeaderCachedInWorkingBuff(sNextHeader))
	{
		do
		{
			iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
					getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sNextHeader));
			if (0 != iRet)
			{
				OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
						<< " header pointer:" << sNextHeader.toStr() << " iRet=" << iRet << enderr;
				mLockRaw->unlock();
				return NULL;
			}
			sNextHeader = pHeader->getNextHeaderPtr();
		}while(isHeaderCachedInWorkingBuff(sNextHeader) && ullTimestamp < pHeader->getTimestamp());
		if (ullTimestamp >= pHeader->getTimestamp() || AosBlobHeader::noNextHeader == pHeader->getNextHeaderPtr())
		{
			mLockRaw->unlock();
			return pHeader;
		}
	}
	if (isHeaderInBuffToFlush(sNextHeader))
	{
		do
		{
			iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
					getHeaderCacheForFlushingPointerByOffsetNoLock(sNextHeader));
			if (0 != iRet)
			{
				OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
						<< " header pointer:" << sNextHeader.toStr() << " iRet=" << iRet << enderr;
				mLockRaw->unlock();
				return NULL;
			}
			sNextHeader = pHeader->getNextHeaderPtr();
		}while(isHeaderInBuffToFlush(sNextHeader) && ullTimestamp < pHeader->getTimestamp());
	}
	mLockRaw->unlock();
	return pHeader;
}


AosBlobHeaderPtr
AosDirtyHeaderCache::getTheOldestHeaderByNextHeaderPointerNoLock(
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sNextHeader)	// in: the newest header pointer
													//out: the oldest header pointer
{
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for docid:" << docid << enderr;
		return NULL;
	}
	AosBlobHeader::HeaderPointer sTempHeaderPointer = sNextHeader;
	int iRet = mHeaderBuffAndFileMgr->getTheOldestHeaderByNextHeaderPointerNoLock(mRdata, docid, sNextHeader, pHeader);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderBuffAndFileMgr->getTheOldestHeaderByNextHeaderPointerNoLock failed, iRet=" << iRet
				<< " the newest header pointer:" << sTempHeaderPointer.toStr()
				<< " returned  header pointer:" << sNextHeader.toStr() << enderr;
		return NULL;
	}

	return pHeader;
}


char*
AosDirtyHeaderCache::getHeaderCacheForWorkingPointerByHeaderPointerNoLock(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	return mHeaderBuffAndFileMgr->getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sNextHeader);
}


char*
AosDirtyHeaderCache::getHeaderCacheForFlushingPointerByOffsetNoLock(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	return mHeaderBuffAndFileMgr->getHeaderCacheForFlushingPointerByHeaderPointerNoLock(sNextHeader);
}


int
AosDirtyHeaderCache::updateHeader(
		AosRundata						*rdata,
		AosBlobHeader::HeaderPointer	&sHeaderPointer,
		const AosBlobHeaderPtr			&pHeader,
		bool							&bNeedsToUpdateOldHeader)
{
	mLockRaw->lock();
	int iRet = saveHeaderToCacheBuff(rdata, pHeader, sHeaderPointer);
	if (0 != iRet)
	{
		OmnAlarm << "saveHeaderToCacheBuff failed for docid:" << pHeader->getDocid() << ", iRet=" << iRet << enderr;
		mLockRaw->unlock();
		return -1;
	}
	if (AosBlobHeader::noNextHeader != pHeader->getNextHeaderPtr()
			&& AosBlobHeader::unresolvedNextHeader != pHeader->getNextHeaderPtr())
	{
		if (AosBlobHeader::noPrevHeader == sHeaderPointer)
		{
			OmnAlarm << "AosBlobHeader::noPrevHeader == sHeaderPointer:" << sHeaderPointer.toStr() << enderr;
			mLockRaw->unlock();
			return -2;
		}
//		iRet = updatePrevHeaderPointerOfNextHeader(rdata, sHeaderPointer, pHeader, bNeedsToUpdateOldHeader);
//		if (0 != iRet)
//		{
//			OmnAlarm << "updatePrevHeaderPointerOfNextHeader failed, iRet=" << iRet
//					<< " docid:" << pHeader->getDocid() << "" << enderr;
//			mLockRaw->unlock();
//			return -3;
//		}
	}
	else
	{
		//pHeader->getNextHeaderPtr() == noNextHeader or unresolvedNextHeader, the later means that pHeader is the
		//header just has been pushed out of normal header cache and has not been flushed to disk, so leave it
		//to resolve unresolved next header pointer phase
		bNeedsToUpdateOldHeader = false;
	}
	mLockRaw->unlock();
	if (needToFlush())
	{
		iRet = startNewThreadToFlushDirtyHeaders();
		if (0 != iRet)
		{
			OmnAlarm << "startNewThreadToFlushDirtyHeaders failed, iRet=" << iRet << enderr;
			return -4;
		}
	}
	return 0;
}


int
AosDirtyHeaderCache::saveHeaderToCacheBuff(
		AosRundata						*rdata,
		const AosBlobHeaderPtr			&pHeader,
		AosBlobHeader::HeaderPointer	&sHeaderPointer)
{
	return mHeaderBuffAndFileMgr->saveHeaderToCacheBuff(rdata, pHeader, sHeaderPointer);
}


bool
AosDirtyHeaderCache::needToFlush()
{
	if (mIsFlushingHeaders)
	{
		return false;
	}
	return mHeaderBuffAndFileMgr->needToFlush();
}


int
AosDirtyHeaderCache::flushDirtyHeaders()
{
	if (mIsFlushingHeaders)
	{
		OmnScreen << "is flushing dirty headers, return" << endl;
		return 0;
	}
	mLockRaw->lock();
	mIsFlushingHeaders = true;
	int iRet = saveMarkToActiveLogFile(AosMarker::eHeaderStartToSave);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		mLockRaw->unlock();
		return -2;
	}
	iRet = switchHeaderBuffAndFile();
	if (0 != iRet)
	{
		OmnAlarm << "switchDirtyHeaderBuffAndOldHeaderFile failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		mLockRaw->unlock();
		return -3;
	}
	mLockRaw->unlock();
	iRet = mHeaderCache->flushDirtyHeaders();
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderCache->flushDirtyHeaders failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		return -4;
	}
	iRet = saveDirtyHeaderCacheIntoOldHeaderFile();
	if (0 != iRet)
	{
		OmnAlarm << "saveDirtyHeaderCacheIntoOldHeaderFile failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		return -5;
	}
	iRet = saveMarkToActiveLogFile(AosMarker::eHeaderSaved);
	if (0 != iRet)
	{
		OmnAlarm << "saveMarkToActiveLogFile failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		return -6;
	}
	iRet = switchHeaderFileForFlushingNoLock();
	if (0 != iRet)
	{
		OmnAlarm << "switchHeaderFile failed, iRet=" << iRet << enderr;
		mIsFlushingHeaders = false;
		return -7;
	}
	mIsFlushingHeaders = false;
	OmnTimerObjPtr thisptr(this, false);
	OmnTimer::getSelf()->startTimer("FlushingTimer", mFlushDirtyHeaderInterval, 0, thisptr, 0);
	return 0;
}


bool
AosDirtyHeaderCache::isHeaderCachedInWorkingBuff(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	return mHeaderBuffAndFileMgr->isHeaderCachedInWorkingBuff(sNextHeader);
}


bool
AosDirtyHeaderCache::isHeaderInBuffToFlush(const AosBlobHeader::HeaderPointer &sNextHeader)
{
	return mHeaderBuffAndFileMgr->isHeaderInBuffToFlush(sNextHeader);
}


int
AosDirtyHeaderCache::shutdown(
		AosRundata	*rdata,
		AosRawFile*	active_rawfile)
{
	/*
	 * this function shuts down the header cache
	 * 1.shut down the header change log
	 * 2.flush dirty headers
	 */
	while(mIsFlushingHeaders)
	{
		OmnScreen << "dirty header cache is sleeping, mIsFlushingHeaders:" << mIsFlushingHeaders << endl;
		OmnSleep(1);
	}
	int iRet = flushDirtyHeaders();
	if (0 != iRet)
	{
		OmnAlarm << "flushDirtyHeaders failed, iRet=" << iRet << enderr;
		return -2;
	}
	return 0;
}


int
AosDirtyHeaderCache::appendHeaderToDirtyCacheBuffForFlushingNoLock(
		const AosBlobHeaderPtr			&pHeader,
		AosBlobHeader::HeaderPointer	&sNextHeader)	//the pointer to the header just appended
{
	return mHeaderBuffAndFileMgr->appendHeaderToBuffForFlushingNoLock(mRdata, pHeader, sNextHeader);
}


int
AosDirtyHeaderCache::switchHeaderBuffAndFile()
{
	return mHeaderBuffAndFileMgr->switchHeaderBuffAndFile(mRdata);
}


int
AosDirtyHeaderCache::switchHeaderFileForFlushingNoLock()
{
	return mHeaderBuffAndFileMgr->switchHeaderFileForFlushingNoLock(mRdata);
}


int
AosDirtyHeaderCache::saveMarkToActiveLogFile(AosMarker::MarkerType eType)
{
	AosMarker marker(eType);
	int iRet = mBlobSE->saveMarkerToMarkerFile(mRdata, marker);
	if (0 != iRet)
	{
		OmnAlarm << "save eHeaderStartToSave entry to ACL failed, iRet=" << iRet << enderr;
		return -1;
	}
	OmnScreen << "SaveHeaderEntry with type:" << eType << " saved to marker file." << endl;
	return 0;
}


int
AosDirtyHeaderCache::saveDirtyHeaderCacheIntoOldHeaderFile()
{
	return mHeaderBuffAndFileMgr->saveDirtyHeaderCacheIntoOldHeaderFile(mRdata);
}


int
AosDirtyHeaderCache::serializeToDirtyHeaderCache(
		AosBlobHeaderPtr					&pHeader,
		const AosBlobHeader::HeaderPointer	&sHeaderPos,
		bool								&bNeedsToFlushNormalHeader)
{
	if (!isHeaderInBuffToFlush(sHeaderPos) && !isHeaderCachedInWorkingBuff(sHeaderPos))
	{
		OmnAlarm << "header not in buff to flush or work, docid:" << pHeader->getDocid()
				<< " OldestHeaderPos:" << sHeaderPos.toStr() << enderr;
		return -1;
	}
	if (pHeader.isNull())
	{
		return -2;
	}
	int iRet = 0;
	if (isHeaderInBuffToFlush(sHeaderPos))
	{
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(getHeaderCacheForFlushingPointerByOffsetNoLock(sHeaderPos));
		if (0 != iRet)
		{
			OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " for docid:" << pHeader->getDocid() << " header pos:" << sHeaderPos.toStr() << enderr;
			return -3;
		}
		return 0;
	}
	if (isHeaderCachedInWorkingBuff(sHeaderPos))
	{
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sHeaderPos));
		if (0 != iRet)
		{
			OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " for docid:" << pHeader->getDocid() << " header pos:" << sHeaderPos.toStr() << enderr;
			return -4;
		}
		bNeedsToFlushNormalHeader = false;
		return 0;
	}
	return 0;
}


int
AosDirtyHeaderCache::startNewThreadToFlushDirtyHeaders()
{
	OmnThrdShellProcPtr thisptr(this, false);
	OmnThreadPool::runProcAsync(thisptr);
	return 0;
}


bool
AosDirtyHeaderCache::run()
{
	int iRet = flushDirtyHeaders();
	if (0 != iRet)
	{
		OmnAlarm << "flushDirtyHeaders failed, iRet=" << iRet << enderr;
		return false;
	}
	return true;
}


bool
AosDirtyHeaderCache::procFinished()
{
	return true;
}


void
AosDirtyHeaderCache::timeout(
    const int		timerId,
    const OmnString	&timerName,
    void			*parm)
{
    OmnScreen << "timeout" << endl;
    if (!mIsFlushingHeaders && !mHeaderCache->isDoingCompaction())
    {
    	startNewThreadToFlushDirtyHeaders();
    	return;
    }
    else
    {
        OmnScreen << "timeout but is flushing dirty headers or doing compaction, wait for another "
        		<< mFlushingWaitForComapctionToFinishTime << " seconds." << endl;
        OmnTimerObjPtr thisptr(this, false);
        OmnTimer::getSelf()->startTimer("FlushingTimer", mFlushingWaitForComapctionToFinishTime, 0, thisptr, 0);
    }
}


int
AosDirtyHeaderCache::informRaftThatFlushingHasCompleted()
{
	OmnScreen << "informRaftThatFlushingHasCompleted" << endl;
	return 0;
}


int
AosDirtyHeaderCache::updatePrevHeaderPointerOfNextHeader(
		AosRundata							*rdata,
		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer,
		const AosBlobHeaderPtr				&pHeader,
		bool								&bNeedsToUpdateOldHeader)
{
	int iRet = 0;
	if (isHeaderCachedInWorkingBuff(pHeader->getNextHeaderPtr()))
	{
		AosBlobHeaderPtr pTmpHeader = OmnNew AosBlobHeader(pHeader->getDocid(), mHeaderCustomDataSize);
		iRet = pTmpHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForWorkingPointerByHeaderPointerNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -1;
		}
		pTmpHeader->setPrevHeader(sPrevHeaderPointer);
		iRet = pTmpHeader->serializeToCacheAndHeaderFileBuff(
				getHeaderCacheForWorkingPointerByHeaderPointerNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -2;
		}
		bNeedsToUpdateOldHeader = false;
		return 0;
	}
	else if (isHeaderInBuffToFlush(pHeader->getNextHeaderPtr()))
	{
		AosBlobHeaderPtr pTmpHeader = OmnNew AosBlobHeader(pHeader->getDocid(), mHeaderCustomDataSize);
		iRet = pTmpHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForFlushingPointerByOffsetNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -1;
		}
		pTmpHeader->setPrevHeader(sPrevHeaderPointer);
		iRet = pTmpHeader->serializeToCacheAndHeaderFileBuff(
				getHeaderCacheForFlushingPointerByOffsetNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -2;
		}
		bNeedsToUpdateOldHeader = false;
		return 0;
	}
	else
	{
		bNeedsToUpdateOldHeader = true;
		return 0;
	}
}


int
AosDirtyHeaderCache::updatePrevHeaderPointerOfNextHeaderInBuffForFlushing(
		AosRundata							*rdata,
		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer,
		const AosBlobHeaderPtr				&pHeader,
		bool								&bNeedsToUpdateOldHeader)
{
	int iRet = 0;
	if (isHeaderInBuffToFlush(pHeader->getNextHeaderPtr()))
	{
		AosBlobHeaderPtr pTmpHeader = OmnNew AosBlobHeader(pHeader->getDocid(), mHeaderCustomDataSize);
		iRet = pTmpHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForFlushingPointerByOffsetNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -1;
		}
		pTmpHeader->setPrevHeader(sPrevHeaderPointer);
		iRet = pTmpHeader->serializeToCacheAndHeaderFileBuff(
				getHeaderCacheForFlushingPointerByOffsetNoLock(pHeader->getNextHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pTmpHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getNextHeaderPtr().toStr() << enderr;
			return -2;
		}
		bNeedsToUpdateOldHeader = false;
		return 0;
	}
	else
	{
		bNeedsToUpdateOldHeader = true;
		return 0;
	}
}


int
AosDirtyHeaderCache::updateNextHeaderPointerOfPrevHeader(
		AosRundata							*rdata,
		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer,
		const AosBlobHeaderPtr				&pHeader,
		bool								&bNeedsToUpdateOldHeader)
{
	if (AosBlobHeader::noPrevHeader == pHeader->getPrevHeaderPtr())
	{
		OmnAlarm << "AosBlobHeader::noPrevHeader == pHeader->getPrevHeaderPtr():"
				<< pHeader->getPrevHeaderPtr().toStr() << enderr;
		return -1;
	}
	int iRet = 0;
	if (isHeaderCachedInWorkingBuff(pHeader->getPrevHeaderPtr()))
	{
		AosBlobHeaderPtr pPrevHeader = OmnNew AosBlobHeader(pHeader->getDocid(), mHeaderCustomDataSize);
		iRet = pPrevHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCacheForWorkingPointerByHeaderPointerNoLock(pHeader->getPrevHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pPrevHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getPrevHeaderPtr().toStr() << enderr;
			return -2;
		}
		if (pPrevHeader->getDocid() != pHeader->getDocid())
		{
			OmnAlarm << "pPrevHeader->getDocid():" << pPrevHeader->getDocid()
					<< " != pHeader->getDocid():" << pHeader->getDocid() << enderr;
			return -3;
		}
		pPrevHeader->setNextHeader(sNextHeaderPointer);
		iRet = pPrevHeader->serializeToCacheAndHeaderFileBuff(
				getHeaderCacheForWorkingPointerByHeaderPointerNoLock(pHeader->getPrevHeaderPtr()));
		if (0 != iRet)
		{
			OmnAlarm << "pPrevHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " docid:" << pHeader->getDocid() << " header pos:" << pHeader->getPrevHeaderPtr().toStr() << enderr;
			return -4;
		}
		bNeedsToUpdateOldHeader = false;
		return 0;
	}
	else
	{
		bNeedsToUpdateOldHeader = true;
		return 0;
	}
}


int
AosDirtyHeaderCache::updateBodyPointerOfHeaderInWorkingBuff(
		AosRundata							*rdata,
		const u64							docid,
		const u64							timestamp,
		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
		const u64							ullOldRawfid,
		const u64							ullOldOffset,
		const u64							ullNewRawfid,
		const u64							ullNewOffset)
{
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mHeaderCustomDataSize);
	iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
			getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sHeaderToUpdate));
	if (0 != iRet)
	{
		OmnAlarm << "pTmpHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " docid:" << docid << " header pos:" << sHeaderToUpdate.toStr() << enderr;
		return -1;
	}
	if (ullOldRawfid != pHeader->getBodyRawfid() || ullOldOffset != pHeader->getBodyOffset()
			|| timestamp != pHeader->getTimestamp())
	{
		OmnAlarm << "ullOldRawfid:" << ullOldRawfid << " != pHeader->getBodyRawfid():" << pHeader->getBodyRawfid()
				<< " || ullOldOffset:" << ullOldOffset << " != pHeader->getBodyOffset():" << pHeader->getBodyOffset()
				<< " || timestamp:" << timestamp << " != pHeader->getTimestamp():" << pHeader->getTimestamp() << enderr;
		return -2;
	}
	pHeader->setBodyRawfid(ullNewRawfid);
	pHeader->setBodyOffset(ullNewOffset);
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			getHeaderCacheForWorkingPointerByHeaderPointerNoLock(sHeaderToUpdate));
	if (0 != iRet)
	{
		OmnAlarm << "pTmpHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " docid:" << docid << " header pos:" << sHeaderToUpdate.toStr() << enderr;
		return -3;
	}
	return 0;
}


void
AosDirtyHeaderCache::dumpEpocDayMap(const char* pCaller)
{
	mHeaderBuffAndFileMgr->dumpEpocDayMap(pCaller);
}
