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
#include "BlobSE/HeaderCache.h"

#include "API/AosApiG.h"
#include "BlobSE/BlobSE.h"
#include "BlobSE/BlobSESnapShotMgr.h"
#include "BlobSE/Compaction.h"
#include "BlobSE/DirtyHeaderCache.h"
#include "BlobSE/HeaderBuffAndFileMgr.h"
#include "BlobSE/HeaderChangeLog.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/OldHeaderCache.h"
#include "BlobSE/RawfidRange.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "Porting/Sleep.h"
#include "RawSE/RawFile.h"
#include "SEInterfaces/BitmapObj.h"
#include "Thread/LockGuard.h"
#include "Util/OmnNew.h"

AosHeaderCache::AosHeaderCache(
		AosBlobSE*					blob_se,
		AosRundata*					rdata,
		AosRawSE*					rawse,
		AosBlobSESnapshotMgrPtr&	snapshot_mgr,
		AosBlobHeaderPtr&			header,
		const AosXmlTagPtr			&conf)
:
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mRdata(rdata),
mHeaderFileMgr(OmnNew AosHeaderFileMgr(rdata, rawse, conf,
		header->getHeaderInCacheAndHeaderFileLength(), header->getCustomDataSize())),
mBlobSE(blob_se),
mSnapshotMgr(snapshot_mgr),
mHeaderBuffAndFileMgr(OmnNew AosHeaderBuffAndFileMgr(rdata, mHeaderFileMgr, conf)),
mDirtyHeaderCache(OmnNew AosDirtyHeaderCache(rdata, header->getCustomDataSize(), mHeaderFileMgr,
		mHeaderBuffAndFileMgr, blob_se, this, conf)),
mOldHeaderCache(OmnNew AosOldHeaderCache(rdata, header, mHeaderFileMgr, conf))
{
	OmnScreen << "getHeaderInCacheAndHeaderFileLength:" << header->getHeaderInCacheAndHeaderFileLength() << endl
			<< " getHeaderInACLAndHCLLen:" << header->getHeaderInACLAndHCLLen() << endl;
	if (conf.isNull())
	{
		rdata->setErrmsg("conf.isNull()");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	if (!mBlobSE)
	{
		rdata->setErrmsg("mBlobSE is NULL");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mSiteID = mBlobSE->getConfig().siteid;
	mCubeID = mBlobSE->getConfig().cubeid;
	mAseID = mBlobSE->getConfig().aseid;
	AosXmlTagPtr config = conf->getFirstChild("HeaderCache", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderCache");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeaderCacheSize = config->getAttrU32("header_cache_size", eHeaderCacheSize);
	mAgeOutBatchSize = config->getAttrU32("age_out_batch_size", eAgeOutBatchSize);
	OmnScreen << "mHeaderCacheSize:" << mHeaderCacheSize << " mAgeOutBatchSize:" << mAgeOutBatchSize << endl;
	mCompactor = OmnNew AosCompaction(rdata, blob_se, rawse, conf, mHeaderFileMgr, mHeaderBuffAndFileMgr,
			this, mOldHeaderCache.getPtrNoLock(), header->getCustomDataSize());
	mDirtyHeaderBitmap = AosGetBitmap();
	if (mDirtyHeaderBitmap.isNull())
	{
		rdata->setErrmsg("can not create mDirtyHeaderBitmap");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mpHead = NULL;
	mpTail = NULL;
}


AosHeaderCache::~AosHeaderCache()
{
    // TODO Auto-generated destructor stub
}


int
AosHeaderCache::deleteHeader(
		AosRundata	*rdata,
		const u64	docid,
		const u64	timestamp)
{
	/*
	 * this function deletes the header from the header cache,
	 * implemented as update it to deleted
	 * 1.write change log if it is on
	 * 2.update header cache
	 * 3.
	 */
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mBlobSE->getConfig().header_custom_data_size);
	pHeader->setBodyRawfid(0);
	pHeader->setBodyOffset(0);
	pHeader->setTimestamp(timestamp);
	pHeader->setDocid(docid);
	//TODO:how can I serialize the modification of a doc? what if when im deleting a doc, comes here and swapped out
	//but another thread is updating the same doc?
	if (0 != (iRet = updateHeader(rdata, pHeader)))
	{
		AosLogError(rdata, false, AosErrmsgId::eActiveLogFileAppendFailed)
			<< AosFieldName::eDocid << docid
			<< AosFieldName::eHeaderStatus << eDeleted
			<< AosFieldName::eReturnValue << iRet << enderr;
		return -1;
	}
	return 0;
}


int
AosHeaderCache::updateHeader(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&header)
{
	int iRet = 0;
//	if (mBlobSE->getConfig().strict_sanity_check)
//	{
//		dumpHeadersByDocid(header->getDocid(), __FUNCTION__, __LINE__);
//	}
	mLockRaw->lock();
//	if (0 != (iRet = writeChangelog(rdata, header)))
//	{
//		AosLogError(rdata, false, AosErrmsgId::eActiveLogFileAppendFailed)
//			<< AosFieldName::eDocid << header->getDocid()
//			<< AosFieldName::eReturnValue << iRet << enderr;
//		return -1;
//	}
	itr_t it = mHeaderMap.find(header->getDocid());
	if (mHeaderMap.end() == it)
	{
		header->setNextHeader(AosBlobHeader::unresolvedNextHeader);
		header->setPrevHeader(AosBlobHeader::noPrevHeader);
		mHeaderMap[header->getDocid()] = header;
		iRet = moveToAgingHead(header);
		if (0 != iRet)	//not vital
		{
			if (mBlobSE->getConfig().strict_sanity_check)
			{
				OmnAlarm << "moveToAgingHead failed, iRet=" << iRet << enderr;
				header->dump();
			}
			else
			{
				OmnScreen << "moveToAgingHead failed, iRet=" << iRet  << endl;
				header->dump();
			}
		}
		mDirtyHeaderBitmap->appendDocid(header->getDocid());
		iRet = ageOutCleanHeaders();
		if (0 != iRet)	//not vital
		{
			if (mBlobSE->getConfig().strict_sanity_check)
			{
				OmnAlarm << "ageOutCleanHeaders failed, iRet=" << iRet << enderr;
			}
			else
			{
				OmnScreen << "ageOutCleanHeaders failed, iRet=" << iRet  << endl;
			}
		}
		mLockRaw->unlock();
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			dumpHeadersByDocid(header->getDocid(), __FUNCTION__, __LINE__);
		}
		return 0;
	}
	//push the header within this(normal) header cache to dirty header cache
	AosBlobHeader::HeaderPointer sHeaderPointer = AosBlobHeader::unresolvedNextHeader;
	bool bNeedsToUpdateOldHeader = true;
	iRet = mDirtyHeaderCache->updateHeader(rdata, sHeaderPointer, it->second, bNeedsToUpdateOldHeader);
	if (0 != iRet)
	{
		OmnAlarm << "mDirtyHeaderCache->updateHeader failed for docid:" << header->getDocid()
				<< ", iRet=" << iRet << enderr;
		mLockRaw->unlock();
		return -2;
	}
	if (AosBlobHeader::unresolvedNextHeader == sHeaderPointer)		//just in case
	{
		OmnAlarm << "mDirtyHeaderCache->updateHeader failed for docid:" << header->getDocid() << enderr;
		mLockRaw->unlock();
		return -3;
	}
//	if (bNeedsToUpdateOldHeader)
//	{
//		iRet = mOldHeaderCache->updatePrevHeaderPointer(
//				rdata, header->getDocid(), it->second->getNextHeaderPtr(), sHeaderPointer);
//		if (0 != iRet)
//		{
//			OmnAlarm << "mOldHeaderCache->updatePrevHeaderPointer failed, iRet=" << iRet << " docid:" << header->getDocid()
//					<< enderr;
//			mLockRaw->unlock();
//			return -4;
//		}
//	}
	header->setNextHeader(sHeaderPointer);
	header->setPrevHeader(AosBlobHeader::noPrevHeader);
	unlinkHeader(it->second);
	mHeaderMap[header->getDocid()] = header;
	iRet = moveToAgingHead(header);
	if (0 != iRet)	//not vital
	{
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			OmnAlarm << "moveToAgingHead failed, iRet=" << iRet << enderr;
			header->dump();
		}
		else
		{
			OmnScreen << "moveToAgingHead failed, iRet=" << iRet  << endl;
			header->dump();
		}
	}
	mDirtyHeaderBitmap->appendDocid(header->getDocid());
	iRet = ageOutCleanHeaders();
	if (0 != iRet)	//not vital
	{
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			OmnAlarm << "ageOutCleanHeaders failed, iRet=" << iRet << enderr;
		}
		else
		{
			OmnScreen << "ageOutCleanHeaders failed, iRet=" << iRet  << endl;
		}
	}
	mLockRaw->unlock();
	if (mBlobSE->getConfig().strict_sanity_check)
	{
		dumpHeadersByDocid(header->getDocid(), __FUNCTION__, __LINE__);
	}
	return 0;
}


AosBlobHeaderPtr
AosHeaderCache::readHeader(
		AosRundata	*rdata,
		const u64	docid,
		const u64	timestamp,
		bool		bShowAlarm)
{
	/*****************************************************************************
	 *******************************  WARNNING  **********************************
	 ********************  DO NOT TRY TO DO REFACTORING!!!  **********************
	 *****************************************************************************
	 * this function maintains the header cache, which means if the header is not in the cache,
	 * it loads it to the cache, it reads from the cache otherwise.
	 * the possible structure of headers are as below:
	 * 1. no header at all, meaning the docid does not exist, return NULL
	 * 2. normalHeaderInFile only, read it, cache it and return it.
	 * 3. normalHeaderInFile---->(oldHeaderInCache or oldHeaderInFile), read the normal header, read and return the
	 *    old header if necessary judging by the timestamp.
	 * 4. normalHeaderInCache only, return it.
	 * 5. normalHeaderInCache---->?, go to 3 if needed judging by the timestamp.
	 * 6. normalHeaderInCache---->(oldHeaderInCache or oldHeaderInFile), return normalHeaderInCache or read and return
	 *    the old header if necessary judging by the timestamp.
	 * 7. normalHeaderInCache---->dirtyHeaderInCache---->(oldHeaderInCache or oldHeaderInFile), return
	 *    normalHeaderInCache or read dirtyHeaderInCache and return it or read the old header in if necessary judging
	 *    by the timestamp.
	 * 8. normalHeaderInCache---->dirtyHeaderInCache---->?, return normalHeaderInCache or dirtyHeaderInCache, or read
	 *    normalHeaderInFile if existed and got to 3
	 */
	int iRet = 0;
	LockGuard(g, mLockRaw);
	itr_t it = mHeaderMap.find(docid);
	if (it == mHeaderMap.end())		//empty cache
	{
		AosBlobHeaderPtr pNormalHeaderInFile = readHeaderFromNormalHeaderFile(rdata, docid, bShowAlarm);
		if (pNormalHeaderInFile.isNull())
		{
			if (bShowAlarm)
			{
				OmnAlarm << "readHeaderFromFile failed for docid:" << docid << " timestamp:"
						<< OmnTime::getTimestamp() << enderr;
			}
			else
			{
				OmnScreen<< "readHeaderFromFile failed for docid:" << docid
						<< " timestamp:" << OmnTime::getTimestamp() << endl;
			}
			return NULL;
		}
		iRet = moveToAgingHead(pNormalHeaderInFile);
		if (0 != iRet)	//not vital
		{
			if (mBlobSE->getConfig().strict_sanity_check)
			{
				OmnAlarm << "moveToAgingHead failed, iRet=" << iRet << enderr;
				pNormalHeaderInFile->dump();
			}
			else
			{
				OmnScreen << "moveToAgingHead failed, iRet=" << iRet  << endl;
				pNormalHeaderInFile->dump();
			}
		}
		mHeaderMap[docid] = pNormalHeaderInFile;
		if (0 == timestamp || timestamp >= pNormalHeaderInFile->getTimestamp() ||
				AosBlobHeader::noNextHeader == pNormalHeaderInFile->getNextHeaderPtr())
					//gives the newest one if timestamp is 0
		{
			return pNormalHeaderInFile;
		}
		//timestamp < pNormalHeaderInCache->getTimestamp() && it's next header pointer points to old header
		AosBlobHeader::HeaderPointer sNextHeader = pNormalHeaderInFile->getNextHeaderPtr();
		AosBlobHeaderPtr pOldHeader = mOldHeaderCache->readHeader(rdata, docid, sNextHeader, timestamp);
		if (pOldHeader.isNull())
		{
			OmnAlarm << "mOldHeaderCache->readHeader failed for docid:" << docid << " NextHeader:"
					<< sNextHeader.toStr() << " timestamp:" << timestamp << enderr;
			return NULL;
		}
		else
		{
			return pOldHeader;
		}
	}
	AosBlobHeaderPtr pNormalHeaderInCache = it->second;
	iRet = moveToAgingHead(pNormalHeaderInCache);
	if (0 != iRet)	//not vital
	{
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			OmnAlarm << "moveToAgingHead failed, iRet=" << iRet << enderr;
			pNormalHeaderInCache->dump();
		}
		else
		{
			OmnScreen << "moveToAgingHead failed, iRet=" << iRet  << endl;
			pNormalHeaderInCache->dump();
		}
	}

	if (0 == timestamp || timestamp >= pNormalHeaderInCache->getTimestamp() ||
			AosBlobHeader::noNextHeader == pNormalHeaderInCache->getNextHeaderPtr())
				//gives the newest one if timestamp is 0
	{
		return pNormalHeaderInCache;
	}
	//normalHeaderInCache====>(normalHeaderInFile or NULL)
	//timestamp < pNormalHeaderInCache->getTimestamp()
	if (AosBlobHeader::unresolvedNextHeader == pNormalHeaderInCache->getNextHeaderPtr())
	{
		AosBlobHeaderPtr pNormalHeaderInFile = readHeaderFromNormalHeaderFile(rdata, docid, false);
		if (pNormalHeaderInFile.isNull()) //the doc has just been created and the normal header file not created yet
		{
			OmnScreen<< "normal header file not created yet for docid:" << docid
			<< " timestamp:" << OmnTime::getTimestamp() << endl;
			pNormalHeaderInCache->setNextHeader(AosBlobHeader::noNextHeader);
			return pNormalHeaderInCache;
		}
		else    //got normal header in file
		{
			if (timestamp >= pNormalHeaderInFile->getTimestamp() ||
					AosBlobHeader::noNextHeader == pNormalHeaderInFile->getNextHeaderPtr())
			{
				return pNormalHeaderInFile;
			}
			//timestamp < pNormalHeaderInFile->getTimestamp()
			if (AosBlobHeader::unresolvedNextHeader == pNormalHeaderInFile->getNextHeaderPtr())
			{
				OmnAlarm << "pNormalHeaderInFile->getNextHeader() must not be unresolvedNextHeader"
						<< " docid:" << pNormalHeaderInFile->getDocid() << enderr;
				return NULL;
			}
			AosBlobHeader::HeaderPointer sNextHeader = pNormalHeaderInFile->getNextHeaderPtr();
			AosBlobHeaderPtr pOldHeader = mOldHeaderCache->readHeader(rdata, docid, sNextHeader, timestamp);
			if (pOldHeader.isNull())
			{
				OmnAlarm << "mOldHeaderCache->readHeader failed for docid:" << docid
						<< " NextHeader:" << sNextHeader.toStr() << " timestamp:" << timestamp << enderr;
				return NULL;
			}
			else
			{
				return pOldHeader;
			}
		}
	}
	AosBlobHeader::HeaderPointer sNextHeader = pNormalHeaderInCache->getNextHeaderPtr();
	AosBlobHeaderPtr pDirtyHeaderInCache = mDirtyHeaderCache->readHeader(rdata, docid, sNextHeader, timestamp);
	if (pDirtyHeaderInCache.notNull())
	{
		if (timestamp >= pDirtyHeaderInCache->getTimestamp() ||
				AosBlobHeader::noNextHeader == pDirtyHeaderInCache->getNextHeaderPtr())
		{
			return pDirtyHeaderInCache;
		}
		if (AosBlobHeader::unresolvedNextHeader == pDirtyHeaderInCache->getNextHeaderPtr())
		{
			AosBlobHeaderPtr pNormalHeaderInFile = readHeaderFromNormalHeaderFile(rdata, docid, false);
			if (pNormalHeaderInFile.isNull()) //the doc has just been created and the normal header file not created yet
			{
				OmnScreen << "normal header file not created yet for docid:" << docid
						<< " timestamp:" << OmnTime::getTimestamp() << endl;
				return pDirtyHeaderInCache;
			}
			if (timestamp >= pNormalHeaderInFile->getTimestamp() ||
					AosBlobHeader::noNextHeader == pNormalHeaderInFile->getNextHeaderPtr())
			{
				return pNormalHeaderInFile;
			}
			sNextHeader = pNormalHeaderInFile->getNextHeaderPtr();
		}
	}
	AosBlobHeaderPtr pOldHeader = mOldHeaderCache->readHeader(rdata, docid, sNextHeader, timestamp);
	if (pOldHeader.isNull())
	{
		OmnAlarm << "mOldHeaderCache->readHeader failed for docid:" << docid
				<< " NextHeader:" << sNextHeader.toStr() << " timestamp:" << timestamp << enderr;
		mDirtyHeaderCache->dumpEpocDayMap(__FUNCTION__);
		return NULL;
	}
	return pOldHeader;
}


AosBlobHeaderPtr
AosHeaderCache::readHeaderFromNormalHeaderFile(
		AosRundata	*rdata,
		const u64	docid,
		bool		bShowAlarm)
{
	return  mHeaderFileMgr->readNormalHeader(rdata, docid, bShowAlarm);
}


bool
AosHeaderCache::needFlush(AosRundata *rdata)
{
	return true;
}


int
AosHeaderCache::init(AosRundata	*rdata)
{
	int iRet = rebuildHeaderCacheFromLogFile(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "rebuildHeaderCacheFromLogFile failed, iRet=" << iRet << enderr;
		return -1;
	}
	return 0;
}


int
AosHeaderCache::flushDirtyHeaders()
{
	/*
	 * this function flushes all the dirty headers into header files
	 * 1.sort the header change log if it is on, putting headers belong to one file together
	 * 2.or sort the header cache
	 * 2.read one header file entirely, make the change, write it back
	 * doing it in this way is for minimizing disk I/Os
	 */
//	return mChangLog->flushDirtyHeaders(rdata, active_rawfile);
	mLockRaw->lock();
	mDirtyHeaderBitmapForFlushing = mDirtyHeaderBitmap;
	mDirtyHeaderBitmap = AosGetBitmap();
	mDirtyHeaderBitmapForFlushing->reset();
	u64 docid = 0;
	u32 ulNormalHeaderFileSeqno = 0;
	AosBuffPtr pNormalHeaderFileBuff = NULL;
	bool bNormalHeaderFileExists = false;
	bool bNeedsToFlushNormalHeader = true;
	int iRet = 0;
//	dumpNormalHeaderCache();
	OmnScreen << "AosHeaderCache::flushDirtyHeaders" << endl;
	while(mDirtyHeaderBitmapForFlushing->nextDocid(docid))
	{
		OmnScreen << "docid:" << docid << endl;
		bNeedsToFlushNormalHeader = true;
		if (mHeaderFileMgr->getNormalHeaderFileSeqnoByDocid(docid) != ulNormalHeaderFileSeqno)
		{
			iRet = reloadNormalHeaderFileBuff(mRdata, docid, ulNormalHeaderFileSeqno,
					pNormalHeaderFileBuff, bNormalHeaderFileExists);
			if (0 != iRet)
			{
				OmnAlarm << "reloadNormalHeaderFileBuff failed, iRet=" << iRet << " docid:" << docid
						<< " ullNormalHeaderFileSeqno:" << ulNormalHeaderFileSeqno << enderr;
				mLockRaw->unlock();
				return -1;
			}
		}
		iRet = resolveUnresolvedNextHeaderPointer(
				mRdata, docid, pNormalHeaderFileBuff, bNormalHeaderFileExists, bNeedsToFlushNormalHeader);
		if (0 != iRet)
		{
			OmnAlarm << "resolveUnresolvedNextHeaderPointer failed on docid:" << docid << ", iRet=" << iRet << enderr;
			mLockRaw->unlock();
			dumpHeadersByDocid(docid, __FUNCTION__, __LINE__);
			return -2;
		}
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			mLockRaw->unlock();
			AosBlobHeaderPtr pTempHeader = readHeader(mRdata, docid, 1, true);
			if (pTempHeader.isNull())
			{
				OmnAlarm << "pTempHeader.isNull()" << enderr;
				dumpHeadersByDocid(docid, __FUNCTION__, __LINE__);
				return -99;
			}
			if (pTempHeader.notNull() && pTempHeader->getNextHeaderPtr() != AosBlobHeader::noNextHeader)
			{
				OmnAlarm << "something is wrong with resolveUnresolvedNextHeaderPointer! docid:" << docid << enderr;
				dumpHeadersByDocid(docid, __FUNCTION__, __LINE__);
				return -99;
			}
			mLockRaw->lock();
		}
		if (bNeedsToFlushNormalHeader)
		{
			iRet = serializeHeaderToNormalHeaderFileBuff(docid, pNormalHeaderFileBuff);
			if (0 != iRet)
			{
				OmnAlarm << "serializeHeaderToNormalHeaderFileBuff failed, iRet=" << iRet << " docid:" << docid << enderr;
				mLockRaw->unlock();
				return -3;
			}
		}
		else
		{
			mDirtyHeaderBitmap->appendDocid(docid);
		}
	}
	if (pNormalHeaderFileBuff.notNull())
	{
		OmnScreen << "overwriting normal header file with seqno:" << ulNormalHeaderFileSeqno << endl;
		iRet = mHeaderFileMgr->overwriteNormalHeaderFile(mRdata, ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->overwriteNormalHeaderFile failed, iRet=" << iRet << enderr;
			mLockRaw->unlock();
			return -4;
		}
	}
	mLockRaw->unlock();
	OmnScreen << "dirty header in header cache have been flushed" << endl;
	return 0;
}


int
AosHeaderCache::writeChangelog(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&header)
{
	return 0;	//TODO:
//	return mChangLog->appendHeader(rdata, header);
}


int
AosHeaderCache::shutdown(
		AosRundata	*rdata,
		AosRawFile*	active_rawfile)
{
	/*
	 * this function shuts down the header cache
	 * 1.shut down the header change log
	 * 2.flush dirty headers
	 */
	aos_assert_rr(active_rawfile, rdata, -1);
	while(mCompactor->isDoingCompaction())
	{
		OmnScreen << "mCompactor is sleeping, mCompactor->isDoingCompaction():"
				<< mCompactor->isDoingCompaction() << endl;
		OmnSleep(1);
	}
	int iRet = mDirtyHeaderCache->shutdown(rdata, active_rawfile);
	if (0 != iRet)
	{
		OmnAlarm << "mDirtyHeaderCache->shutdown failed, iRet=" << iRet << enderr;
		return -2;
	}
//	iRet = mChangLog->shutdown(rdata, active_rawfile);
//	if (0 != iRet)
//	{
//		OmnAlarm << "mChangLog->shutdown failed, iRet=" << iRet << enderr;
//		mLockRaw->unlock();
//		return -3;
//	}
	return 0;
}


int
AosHeaderCache::rebuildHeaderCacheFromLogFile(AosRundata *rdata)
{
	/*
	 * this function redoes the header changes according to the active change log
	 * 1.read the active change log backwards, until it reaches the a header change
	 * log saved entry or a header saved entry
	 * 2.read the active change log forwards, overwrite the header cache accordingly
	 */
	return 0;
	AosBuffPtr buff = OmnNew AosBuff(mBlobSE->getConfig().active_log_max_size * 2 AosMemoryCheckerArgs);
	if (buff.isNull())
	{
		OmnAlarm << "can not create AosBuff obj, buff size:" << mBlobSE->getConfig().active_log_max_size * 2 << enderr;
		return -1;
	}
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mBlobSE->getConfig().header_custom_data_size);
	u64 fileid = 0;
	u64	offset = 0;
	u64 snap_id = 0;
	iRet = mBlobSE->searchForSaveMark(rdata, fileid, offset, buff);
	if (0 != iRet)
	{
		OmnAlarm << "mBlobSE->searchForSaveMark failed, iRet=" << iRet << " fileid:" << fileid
				<< " offset:" << offset << enderr;
		return -2;
	}
	bool bFinish = false;
	while (1 == (iRet = mBlobSE->getNextHeader(rdata, pHeader, fileid, offset, buff, snap_id)))	//FIXME:use header interface
	{
		bFinish = false;
		bool rslt = mSnapshotMgr->saveDoc(rdata, pHeader, bFinish);
		if (!rslt)
		{
			OmnAlarm << "mSnapshotMgr->saveDoc failed on docid:" << pHeader->getDocid() << " snap_id:" << snap_id << enderr;
			return -3;
		}
		if (!bFinish)
		{
			iRet = updateHeader(rdata, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "mHeaderCache->updateHeader failed for docid:" << pHeader->getDocid() << enderr;
				return -4;
			}
		}
	}
	if (0 != iRet)
	{
		OmnAlarm << "mBlobSE->getNextHeader failed, iRet=" << iRet << enderr;
		return -5;
	}
	return 0;
}


u32
AosHeaderCache::getCurrentHeaderInCache(const u64 docid)
{
	itr_t itr = mHeaderMap.find(docid);
	if (itr != mHeaderMap.end())
	{
		return 1;
	}
	else
	{
		return 0;	//0 as a guard
	}
}


int
AosHeaderCache::moveToAgingHead(AosBlobHeaderPtr pHeader)
{
	if (mBlobSE->getConfig().strict_sanity_check)
	{
		if (!((mpHead.isNull() && mpTail.isNull()) ||
				(mpHead.notNull() && mpTail.notNull() && mpHead->mPrev == mpTail && mpTail->mNext == mpHead)))
		{
			OmnAlarm << "There is something wrong with header linked list." << enderr;
			return -1;
		}
	}
	if (mpHead.isNull() && mpTail.isNull())		//the list is empty
	{
		mpHead = pHeader;
		mpTail = pHeader;
		mpHead->mPrev = mpTail;
		mpTail->mNext = mpHead;
	}
	if (mpHead == pHeader)
	{
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			aos_assert_r(pHeader->mPrev.notNull() && pHeader->mNext.notNull(), -2);
		}
		return 0;
	}
	if (mpTail == pHeader)
	{
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			aos_assert_r(pHeader->mPrev.notNull() && pHeader->mNext.notNull(), -3);
		}
		mpTail = pHeader->mPrev;
	}
	if (pHeader->mNext.notNull() && pHeader->mNext.notNull())	//not first inserted
	{
		pHeader->mNext->mPrev = pHeader->mPrev;
		pHeader->mPrev->mNext = pHeader->mNext;
	}
	pHeader->mNext = mpHead;
	mpHead->mPrev = pHeader;
	mpHead = pHeader;
	mpHead->mPrev = mpTail;
	mpTail->mNext = mpHead;
//	OmnScreen << "mHeaderMap.size():" << mHeaderMap.size() << endl;
	return 0;
}


int
AosHeaderCache::unlinkHeader(AosBlobHeaderPtr pHeader)
{
	if (mBlobSE->getConfig().strict_sanity_check)
	{
		if (!((mpHead.isNull() && mpTail.isNull()) ||
				(mpHead.notNull() && mpTail.notNull() && mpHead->mPrev == mpTail && mpTail->mNext == mpHead)))
		{
			OmnAlarm << "There is something wrong with header linked list." << enderr;
			return -1;
		}
	}
	if (mpHead.isNull() || mpTail.isNull())		//the list is empty
	{
		OmnAlarm << "mpHead.isNull():" << mpHead.isNull() << " mpTail.isNull():" << mpTail.isNull() << enderr;
		return -2;
	}
	if (mpHead == mpTail)	//there is only one header in the list
	{
		if (1 != mHeaderMap.size())
		{
			OmnAlarm << "1 != mHeaderMap.size():" << mHeaderMap.size() << enderr;
			return -3;
		}
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			aos_assert_r(mpHead == mpTail, -4);
		}
		pHeader->mNext = NULL;
		pHeader->mPrev = NULL;
		mpHead = NULL;
		mpTail = NULL;
		return 0;
	}
	if (pHeader->mNext.notNull() && pHeader->mPrev.notNull())	//not first inserted
	{
		pHeader->mNext->mPrev = pHeader->mPrev;
		pHeader->mPrev->mNext = pHeader->mNext;
	}
	if (mpHead == pHeader)
	{
		mpHead = pHeader->mNext;
	}
	if (mpTail == pHeader)
	{
		mpTail = pHeader->mPrev;
	}
	mpHead->mPrev = mpTail;
	mpTail->mNext = mpHead;
	return 0;
}


int
AosHeaderCache::resolveUnresolvedNextHeaderPointer(
		AosRundata *rdata,
		const u64	docid,
		AosBuffPtr	&pNormalHeaderFileBuff,
		const bool	bNormalHeaderFileExists,
		bool		&bNeedsToFlushNormalHeader)
{
	AosBlobHeaderPtr pNormalHeaderInCache = getNormalHeaderFromHeaderCache(docid);
	if (pNormalHeaderInCache.isNull())
	{
		OmnAlarm << "can not find header in cache, docid:" << docid << enderr;
		return -1;
	}
	if (pNormalHeaderFileBuff.isNull())
	{
		OmnAlarm << "pNormalHeaderFileBuff.isNull(), docid:" << docid << enderr;
		return -2;
	}
	int iRet = 0;
	if (AosBlobHeader::noNextHeader == pNormalHeaderInCache->getNextHeaderPtr())
	{
		return 0;
	}
	if (AosBlobHeader::unresolvedNextHeader == pNormalHeaderInCache->getNextHeaderPtr())
		//this doc only has been modified once
	{
		iRet = resolveUnresolvedNextHeaderPointerOfNormalHeaderInCache(
				rdata, pNormalHeaderInCache, pNormalHeaderFileBuff, bNormalHeaderFileExists);
		if (0 != iRet)
		{
			OmnAlarm << "resolveUnresolvedNextHeaderPointerOfNormalHeaderInCache failed, iRet=" << iRet
					<< " docid:" << docid << enderr;
			return -3;
		}
		if (mBlobSE->getConfig().strict_sanity_check)
		{
			AosBlobHeaderPtr pNormalHeaderForChecking = getNormalHeaderFromHeaderCache(docid);
			if (!bNormalHeaderFileExists && AosBlobHeader::noNextHeader != pNormalHeaderForChecking->getNextHeaderPtr())
			{
				OmnAlarm << "!bNormalHeaderFileExists && "
						<< "AosBlobHeader::noNextHeader != pNormalHeaderForChecking->getNextHeaderPtr()" << enderr;
				pNormalHeaderForChecking->dump();
				pNormalHeaderInCache->dump();
				return -4;
			}
			pNormalHeaderForChecking->dump();
			pNormalHeaderInCache->dump();
		}
		bNeedsToFlushNormalHeader = true;
		return 0;
	}
	//pHeader->getNextHeader() != AosBlobHeader::unresolvedNextHeader next header must be in dirty header cache
	iRet = resolveUnresolvedNextHeaderPointerOfDirtyHeaders(
			pNormalHeaderInCache, pNormalHeaderFileBuff, bNormalHeaderFileExists, bNeedsToFlushNormalHeader);
	if (0 != iRet)
	{
		OmnAlarm << "resolveUnresolvedNextHeaderPointerOfDirtyHeaders failed, iRet=" << iRet
				<< " docid:" << pNormalHeaderInCache->getDocid() << enderr;
		return -5;
	}
	return 0;
}


int
AosHeaderCache::resolveUnresolvedNextHeaderPointerOfNormalHeaderInCache(
		AosRundata 			*rdata,
		AosBlobHeaderPtr	&pNormalHeaderInCache,
		AosBuffPtr			&pNormalHeaderFileBuff,
		const bool			bNormalHeaderFileExists)
{
	if (pNormalHeaderInCache.isNull())
	{
		return -1;
	}
	if (pNormalHeaderInCache->getNextHeaderPtr() != AosBlobHeader::unresolvedNextHeader)
	{
		return -2;
	}
	if (!bNormalHeaderFileExists)		//this doc only has been modified once
	{
		pNormalHeaderInCache->setNextHeader(AosBlobHeader::noNextHeader);
		return 0;
	}
	//bNormalHeaderFileExists
	AosBlobHeader::HeaderPointer sNextHeader = AosBlobHeader::noNextHeader;
	int iRet = pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush(
			pNormalHeaderInCache->getDocid(), sNextHeader, pNormalHeaderFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush, iRet=" << iRet
				<< " docid:" << pNormalHeaderInCache->getDocid() << enderr;
		return -3;
	}
	pNormalHeaderInCache->setNextHeader(sNextHeader);
	return 0;
}


int
AosHeaderCache::resolveUnresolvedNextHeaderPointerOfDirtyHeaders(
		AosBlobHeaderPtr	&pNormalHeader,
		AosBuffPtr			&pNormalHeaderFileBuff,
		const bool			bNormalHeaderFileExists,
		bool				&bNeedsToFlushNormalHeader)
{
	if (pNormalHeader.isNull())
	{
		return -1;
	}
	AosBlobHeader::HeaderPointer sNextHeader = pNormalHeader->getNextHeaderPtr();
	AosBlobHeaderPtr pOldestDirtyHeader =
			mDirtyHeaderCache->getTheOldestHeaderByNextHeaderPointerNoLock(pNormalHeader->getDocid(), sNextHeader);
										//sNextHeader should be the pointer to the oldest header in dirty header cache
	if (pOldestDirtyHeader.isNull())
	{
		OmnAlarm << "mDirtyHeaderCache->getTheOldesHeaderInCacheBuffToFlush failed, docid:" << pNormalHeader->getDocid()
				<< " next header:" << sNextHeader.toStr() << enderr;
		return -2;
	}
	if(AosBlobHeader::noNextHeader == pOldestDirtyHeader->getNextHeaderPtr())
	{
		return 0;
	}
	int iRet = 0;
	if (AosBlobHeader::unresolvedNextHeader == pOldestDirtyHeader->getNextHeaderPtr())
	{
		if (!bNormalHeaderFileExists)
			//the oldest dirty header's next header pointer is unresolved and the normal header does not exist,
			//which means all the headers of this doc is with normal and dirty header cache,
			//so we just mark the next header pointer of the oldest header of this doc to null
		{
			pOldestDirtyHeader->setNextHeader(AosBlobHeader::noNextHeader);
		}
		else	//bNormalHeaderFileExists
		{
			AosBlobHeader::HeaderPointer sHeaderPos = AosBlobHeader::noNextHeader;
			iRet = pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush(
					pNormalHeader->getDocid(), sHeaderPos, pNormalHeaderFileBuff);
			if (0 != iRet)
			{
				OmnAlarm << "pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush, iRet=" << iRet
						<< " docid:" << pNormalHeader->getDocid()
						<< " bNormalHeaderFileExists:" << bNormalHeaderFileExists << enderr;
				return -3;
			}
			pOldestDirtyHeader->setNextHeader(sHeaderPos);
		}
		if (AosBlobHeader::unresolvedNextHeader == pOldestDirtyHeader->getNextHeaderPtr())
		{
			OmnAlarm << "AosBlobHeader::unresolvedNextHeader == pOldestDirtyHeader->getNextHeaderPtr()" << enderr;
			return -4;
		}
		iRet = mDirtyHeaderCache->serializeToDirtyHeaderCache(pOldestDirtyHeader, sNextHeader, bNeedsToFlushNormalHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mDirtyHeaderCache->markNextHeaderPointerOfDocsOldestDirtyHeaderToNULL, iRet=" << iRet
					<< " next header:" << pNormalHeader->getNextHeaderPtr().toStr() << enderr;
			return -5;
		}
	}
	return 0;
}


int
AosHeaderCache::pushHeaderInNormalHeaderFileBuffToDirtyHeaderCacheToFlush(
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sNextHeader,
		AosBuffPtr						&pNormalHeaderFileBuff)
{
	AosBlobHeaderPtr pTempHeader =
			mHeaderFileMgr->unserializeHeaderFromNormalHeaderFileBuff(docid, pNormalHeaderFileBuff);
	if (pTempHeader.isNull())
		//TODO:normal header may not have been created yet even normal header file existed, how to determine it?
	{
		OmnScreen << "mHeaderFileMgr->unserializeHeaderFromNormalHeaderFileBuff failed for docid:" << docid
				<< " normal header may not have been created yet"
				<< " happened at timestamp:" << OmnTime::getTimestamp() << endl;
		sNextHeader = AosBlobHeader::noNextHeader;
		return 0;
	}
	pTempHeader->setDocid(docid);
	int iRet = mDirtyHeaderCache->appendHeaderToDirtyCacheBuffForFlushingNoLock(pTempHeader, sNextHeader);
	if (0 != iRet)
	{
		OmnAlarm << "mDirtyHeaderCache->appendHeaderToDirtyCacheBuffForFlushing failed for docid:" << docid
				<< ", iRet=" << iRet << enderr;
		return -1;
	}
	if (AosBlobHeader::noNextHeader == sNextHeader || AosBlobHeader::noPrevHeader == sNextHeader ||
			AosBlobHeader::unresolvedNextHeader == sNextHeader)		//just double check
	{
		OmnAlarm << "returned next header pointer of mDirtyHeaderCache->appendHeaderToDirtyCacheBuffForFlushing"
				<< " is illegal:" << sNextHeader.toStr() << enderr;
		return -2;
	}
	if (AosBlobHeader::noNextHeader == pTempHeader->getNextHeaderPtr())
	{
		OmnScreen << "Header in normal header file of docid:" << docid << " has no next header" << endl;
		pTempHeader->dump();
		return 0;
	}
//	bool bNeedsToUpdateOldHeader = true;
//	iRet = mDirtyHeaderCache->updatePrevHeaderPointerOfNextHeaderInBuffForFlushing(
//			mRdata, sNextHeader, pTempHeader, bNeedsToUpdateOldHeader);
//	if (0 != iRet)
//	{
//		OmnAlarm << "mDirtyHeaderCache->updatePrevHeaderPointerOfNextHeaderInBuffForFlushing failed, iRet="
//				<< iRet << " docid:" << docid << enderr;
//		return -3;
//	}
//	if (bNeedsToUpdateOldHeader)
//	{
//		iRet = mOldHeaderCache->updatePrevHeaderPointer(mRdata, docid, pTempHeader->getNextHeaderPtr(), sNextHeader);
//		if (0 != iRet)
//		{
//			OmnAlarm << "mOldHeaderCache->updatePrevHeaderPointer failed, iRet=" << iRet
//					<< " docid:" << docid << " header to update:" << pTempHeader->getNextHeaderPtr().toStr() << enderr;
//			return -4;
//		}
//	}
	return 0;
}


AosBlobHeaderPtr
AosHeaderCache::getNormalHeaderFromHeaderCache(const u64 docid)
{
	itr_t it = mHeaderMap.find(docid);
	if (mHeaderMap.end() == it)
	{
		OmnAlarm << "can not find docid in mHeaderMap, docid:" << docid << enderr;
		return NULL;
	}
	it->second->setDocid(docid);
	return it->second;
}


int
AosHeaderCache::serializeHeaderToNormalHeaderFileBuff(
			const u64	docid,
			AosBuffPtr	&pNormalHeaderFileBuff)
{
	AosBlobHeaderPtr pHeader = getNormalHeaderFromHeaderCache(docid);
	if (pHeader.isNull())
	{
		OmnAlarm << "can not find header in cache, docid:" << docid << enderr;
		return -1;
	}
	return mHeaderFileMgr->serializeHeaderToNormalHeaderFileBuff(pHeader, pNormalHeaderFileBuff);
}


void
AosHeaderCache::dumpNormalHeaderCache()
{
	OmnScreen << "dumping normal header cache" << endl;
	for(itr_t it = mHeaderMap.begin();it != mHeaderMap.end();it++)
	{
		it->second->dump();
	}
	OmnScreen << "normal header cache dumped" << endl;
}


int
AosHeaderCache::reloadNormalHeaderFileBuff(
		AosRundata	*rdata,
		const u64	docid,
		u32			&ulNormalHeaderFileSeqno,
		AosBuffPtr	&pNormalHeaderFileBuff,
		bool		&bNormalHeaderFileExists)
{
	OmnScreen << "switching header file from seqno:" << ulNormalHeaderFileSeqno
			<< " to seqno:" << mHeaderFileMgr->getNormalHeaderFileSeqnoByDocid(docid) << endl;
	int iRet = 0;
	if (pNormalHeaderFileBuff.notNull())
	{
		OmnScreen << "overwriting normal header file with seqno:" << ulNormalHeaderFileSeqno << endl;
		iRet = mHeaderFileMgr->overwriteNormalHeaderFile(rdata, ulNormalHeaderFileSeqno, pNormalHeaderFileBuff);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderFileMgr->overwriteNormalHeaderFile failed, iRet=" << iRet << enderr;
			return -1;
		}
	}
	ulNormalHeaderFileSeqno = mHeaderFileMgr->getNormalHeaderFileSeqnoByDocid(docid);
	pNormalHeaderFileBuff = mHeaderFileMgr->readNormalHeaderFile(rdata, ulNormalHeaderFileSeqno);
	if (pNormalHeaderFileBuff.isNull())	//this normal file does not exist
	{
		OmnScreen << "Normal Header File does not exist, seqno:" << ulNormalHeaderFileSeqno
				<< " docid:" << docid << endl;
		pNormalHeaderFileBuff = OmnNew AosBuff(mHeaderFileMgr->getNormalHeaderFileSize() AosMemoryCheckerArgs);
		if (pNormalHeaderFileBuff.isNull())
		{
			OmnAlarm << "failed to create normal header file buff obj, ullNormalHeaderFileSeqno:"
					<< ulNormalHeaderFileSeqno << enderr;
			return -2;
		}
		memset(pNormalHeaderFileBuff->data(), 0, pNormalHeaderFileBuff->buffLen());
		pNormalHeaderFileBuff->setDataLen(mHeaderFileMgr->getNormalHeaderFileSize());
		bNormalHeaderFileExists = false;
	}
	else
	{
		bNormalHeaderFileExists = true;
	}
	return 0;
}


int
AosHeaderCache::modifyBodyPointerToNewPos(
		AosRundata	*rdata,
		const u64	docid,
		const u64	timestamp,
		const u64	ullOldRawfid,
		const u64	ullOldOffset,
		const u64	ullNewRawfid,
		const u64	ullNewOffset)
{
	AosBlobHeaderPtr pTempHeader = readHeader(rdata, docid, 0);
	if (pTempHeader.isNull())
	{
		OmnAlarm << "readHeader failed for docid:" << docid << " timestamp:" << 0 << enderr;
		return -1;
	}
	int iRet = 0;
	if (pTempHeader->getBodyRawfid() == ullOldRawfid && pTempHeader->getBodyOffset() == ullOldOffset)
	{
		pTempHeader->setBodyRawfid(ullNewRawfid);
		pTempHeader->setBodyOffset(ullNewOffset);
		mDirtyHeaderBitmap->appendDocid(docid);
		return 0;
	}
	AosBlobHeader::HeaderPointer sHeaderPointer = pTempHeader->getNextHeaderPtr();
	//the header that needs to be modified must dwell in the old header files,
	//otherwise the compaction time threshold is set incorrectly, i.e. too small
	AosBlobHeaderPtr pDirtyHeaderInCache =
			mDirtyHeaderCache->readHeader(rdata, docid, sHeaderPointer, timestamp);
	if (pDirtyHeaderInCache.isNull())
	{
		OmnScreen << "header not in dirty header cache, docid:" << docid
				<< " header pointer:" << pTempHeader->getNextHeaderPtr().toStr() << " timestamp:" << timestamp << endl;
		iRet = mOldHeaderCache->updateBodyPointer(
				rdata, docid, timestamp, pTempHeader->getNextHeaderPtr(),
				ullOldRawfid, ullOldOffset, ullNewRawfid, ullNewOffset);
		if (0 != iRet)
		{
			OmnAlarm << "mOldHeaderCache->updateBodyPointer failed, iRet=" << iRet << " docid:" << docid
					<< " timestamp:" << timestamp << " header pos:" << pTempHeader->getNextHeaderPtr().toStr()
					<< " body old pos@rawfid:" << ullOldRawfid << " offset:" << ullOldOffset
					<< " body new pos@rawfid:" << ullNewRawfid << " offset:" << ullNewOffset << enderr;
			return -2;
		}
		return 0;
	}
	else
	{
		if (mDirtyHeaderCache->isHeaderCachedInWorkingBuff(sHeaderPointer))
			//we only need to worry about dirty headers within working buff, coz the ones in flushing buff should
			//already be on the disk, we should update them through old header cache
		{
			iRet = mDirtyHeaderCache->updateBodyPointerOfHeaderInWorkingBuff(
					rdata, docid, timestamp, sHeaderPointer,
					ullOldRawfid, ullOldOffset, ullNewRawfid, ullNewOffset);
			if (0 != iRet)
			{
				OmnAlarm << "mDirtyHeaderCache->updateBodyPointer failed, iRet=" << iRet << " docid:" << docid
						<< " timestamp:" << timestamp << " header pos:" << sHeaderPointer.toStr()
						<< " body old pos@rawfid:" << ullOldRawfid << " offset:" << ullOldOffset
						<< " body new pos@rawfid:" << ullNewRawfid << " offset:" << ullNewOffset << enderr;
				return -3;
			}
		}
		return 0;
	}
	return 0;
}


int
AosHeaderCache::doCompationAsRaftLeader()
{
	if (mCompactor->needsCompaction())
	{
		return mCompactor->doLeaderCompation();
	}
	else
	{
		OmnScreen << "no needs to do compaction" << endl;
		return 0;
	}
}


int
AosHeaderCache::doCompactionAsRaftFollower()
{
	return mCompactor->doFollowerCompaction();
}


void
AosHeaderCache::dumpHeadersByDocid(
		const 		u64 docid,
		const char*	pCaller,
		const int	line)
{
	u64 ullTimestamp = 0;
	OmnScreen << "dumping headers of docid:" << docid << " in '" << pCaller << "' line:" << line << endl;
	AosBlobHeaderPtr pHeader = readHeader(mRdata, docid, ullTimestamp);
	if (pHeader.isNull())
	{
		OmnAlarm << "read header failed for docid:" << docid << " timestamp:" << ullTimestamp << enderr;
		return;
	}
	pHeader->dump();
	while(AosBlobHeader::noNextHeader != pHeader->getNextHeaderPtr() &&
			AosBlobHeader::unresolvedNextHeader != pHeader->getNextHeaderPtr())
	{
		ullTimestamp = pHeader->getTimestamp() - 1;
		pHeader = readHeader(mRdata, docid, ullTimestamp);
		if (pHeader.isNull())
		{
			OmnAlarm << "read header failed for docid:" << docid << " timestamp:" << ullTimestamp << enderr;
			return;
		}
		pHeader->dump();
	}
}


int
AosHeaderCache::updateHeaders(
		AosRundata							*rdata,
		const std::vector<AosBlobHeaderPtr>	&vHeaders,
		const u64							timestamp)
{
	if (0 >= vHeaders.size())
	{
		OmnAlarm << "0 >= vHeaders.size():" << vHeaders.size() << enderr;
		return -1;
	}
	OmnScreen << "updating headers for snapshot manager, got " << vHeaders.size() << " headers." << endl;
	int iRet = 0;
	for (std::vector<AosBlobHeaderPtr>::const_iterator it = vHeaders.begin(); it != vHeaders.end(); it++)
	{
		(*it)->setTimestamp(timestamp);
		iRet = updateHeader(rdata, *it);
		if (0 != iRet)
		{
			OmnAlarm << "updateHeader failed, iRet=" << iRet << enderr;
			(*it)->dump();
			return -2;
		}
	}
	return 0;
}


int
AosHeaderCache::saveOldestOldHeaderFileSeqnoForCompaction(const u32 ulSeqno)
{
//	return mCompactor->saveOldestOldHeaderFileSeqnoToMetafile(ulSeqno);
	return 0;
}


int
AosHeaderCache::clearHeaderCacheForDocid(const u64 docid)
{
	u64 ullCount = mHeaderMap.erase(docid);
	if (1 != ullCount)
	{
		OmnAlarm << "something is wrong with mHeaderMap.erase, return val:" << ullCount << " docid:" << docid << enderr;
		return -1;
	}
	return 0;
}


int
AosHeaderCache::updateNormalHeaderInCache(const AosBlobHeaderPtr &header)
{
	mLockRaw->lock();
	if (mDirtyHeaderBitmap->checkDoc(header->getDocid()))
	{
		OmnScreen << "docid:" << header->getDocid() << " is dirty, no need to update it's normal header in cahce" << endl;
		mLockRaw->unlock();
		return 0;
	}
	mHeaderMap[header->getDocid()] = header;
	moveToAgingHead(header);
	mLockRaw->unlock();
	return 0;
}


int
AosHeaderCache::ageOutCleanHeaders()
{
	if (mHeaderMap.size() <= mHeaderCacheSize)
	{
		OmnScreen << "mHeaderMap.size():" << mHeaderMap.size() << " <= mHeaderCacheSize:" << mHeaderCacheSize
				<< ", no needs to age out." << endl;
		return 0;
	}
	OmnScreen << "mHeaderMap.size() before aging out:" << mHeaderMap.size() << endl;
	AosBlobHeaderPtr pHeader = mpTail;
	int iRet = 0;
	for (u32 ulCount = 0; ulCount < mAgeOutBatchSize; ulCount++)
	{
		pHeader = mpTail;
		if (pHeader == mpHead)
		{
			OmnScreen << "mHeaderMap has " << mHeaderMap.size() << " headers now and they are all dirty. "
					<< ulCount << " headers have been aged out this time."
					<< " If this happens too often, you may consider to set a larger header_cache_size." << endl;
			break;
		}
		if (mDirtyHeaderBitmap->checkDoc(pHeader->getDocid()))
		{
			pHeader = pHeader->mPrev;
		}
		else
		{
			iRet = unlinkHeader(pHeader);
			if (0 != iRet)
			{
				if (mBlobSE->getConfig().strict_sanity_check)
				{
					OmnAlarm << "unlinkHeader failed, iRet=" << iRet << enderr;
					pHeader->dump();
					return -1;
				}
				else
				{
					OmnScreen << "unlinkHeader failed, iRet=" << iRet << endl;
					pHeader->dump();
					return -2;
				}
			}
			mHeaderMap.erase(pHeader->getDocid());
		}
	}
	OmnScreen << "mHeaderMap.size() after aging out:" << mHeaderMap.size() << endl;
	return 0;
}
