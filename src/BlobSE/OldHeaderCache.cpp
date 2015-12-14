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
#include "BlobSE/OldHeaderCache.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/Compaction.h"
#include "BlobSE/HeaderFileMgr.h"
#include "Util/Buff.h"

AosOldHeaderCache::AosOldHeaderCache(
		AosRundata*					rdata,
		AosBlobHeaderPtr&			header,
		const AosHeaderFileMgrPtr	&pHeaderFileMgr,
		const AosXmlTagPtr			&conf)
:
mLockRaw(OmnNew OmnMutex()),
mLock(mLockRaw),
mHeaderCustomDataSize(header->getCustomDataSize()),
mHeaderFileMgr(pHeaderFileMgr)
{
	if (conf.isNull())
	{
		rdata->setErrmsg("conf.isNull()");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	AosXmlTagPtr config = conf->getFirstChild("OldHeaderCache", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named OldHeaderCache");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mCacheSize = config->getAttrU32("old_header_cache_size", eOldHeaderCacheSize);
	config = conf->getFirstChild("Compaction", true);
	if (config.isNull())
	{
		rdata->setErrmsg("can not find config tag named HeaderFile");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mCompactionTimeWindowInDays =
			config->getAttrU32("compaction_time_windows_in_days", AosCompaction::eCompactionTimeWindowInUnits);
	if (AosCompaction::eCompactionTimeWindowInUnits == mCompactionTimeWindowInDays)
	{
		OmnScreen << "mHeaderFileExpirationThresholdInSecs set to default value:"
				<< mCompactionTimeWindowInDays << ", please make sure this is what you want." << endl;
	}
	mHeaderCacheBuff = OmnNew AosBuff(mCacheSize AosMemoryCheckerArgs);
	int iRet = initHeaderCacheBuff(header->getHeaderInCacheAndHeaderFileLength());
	if (0 != iRet)
	{
		rdata->setErrmsg("initHeaderCacheBuff failed");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosOldHeaderCache::~AosOldHeaderCache()
{
}


AosBlobHeaderPtr
AosOldHeaderCache::readHeader(
		AosRundata						*rdata,
		const u64						docid,
		AosBlobHeader::HeaderPointer	&sHeaderToRead,
		const u64						ullTimestamp)
{
	/*
	 * return the valid header judging by timestamp or the oldest header,
	 * NULL if any error occurs
	 */
	if (AosBlobHeader::noNextHeader == sHeaderToRead || AosBlobHeader::unresolvedNextHeader == sHeaderToRead)
	{
		return NULL;
	}
	if (AosBlobHeader::noPrevHeader == sHeaderToRead)	//just in case
	{
		OmnAlarm << "invalid sNextHeader:" << sHeaderToRead.toStr() << enderr;
		return NULL;
	}
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for docid:" << docid << enderr;
		return NULL;
	}
	AosBlobHeader::HeaderPointer sPrevHeader = sHeaderToRead;
//	bool bFirstTime = true;
	while(isHeaderCached(sHeaderToRead))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToRead)));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sHeaderToRead.toStr() << " iRet=" << iRet << enderr;
			return NULL;
		}
		if (docid != pHeader->getDocid())
		{
			OmnAlarm << "docid:" << docid << " != pHeader->getDocid():" << pHeader->getDocid()
					<< " header pos:" << sHeaderToRead.toStr() << enderr;
			pHeader->dump();
			return NULL;
		}
		if(ullTimestamp >= pHeader->getTimestamp())
		{
			return pHeader;
		}
//		if (pHeader->getPrevHeaderPtr() != sPrevHeader && !bFirstTime)
//		{
//			OmnAlarm << "pHeader->getPrevHeaderPtr():" << pHeader->getPrevHeaderPtr().toStr()
//					<< " != prevHeader:" << sPrevHeader.toStr() << " docid:" << docid
//					<< "!= sHeaderToRead:" << sHeaderToRead.toStr() << enderr;
//			pHeader->dump();
//			return NULL;
//		}
//		bFirstTime = false;
		sPrevHeader = sHeaderToRead;
		sHeaderToRead = pHeader->getNextHeaderPtr();
	}
	if (AosBlobHeader::noNextHeader == sHeaderToRead)
	{
		return pHeader;
	}
//	u64 ullRightnow = OmnTime::getTimestamp();
//	if (mHeaderFileMgr->getEpochUnitByTimestamp(pHeader->getTimestamp()) + mCompactionTimeWindowInDays
//			<= mHeaderFileMgr->getEpochUnitByTimestamp(ullRightnow) &&
//			mHeaderFileMgr->getEpochUnitBySeqno(pHeader->getNextHeaderPtr().ulSeqno) + mCompactionTimeWindowInDays
//			<= mHeaderFileMgr->getEpochUnitByTimestamp(ullRightnow))		//header is the first header out of time window
//	{
//		return pHeader;
//	}
	do
	{
		pHeader = mHeaderFileMgr->readOldHeader(rdata, sHeaderToRead);
		if (pHeader.isNull())
		{
			OmnAlarm << "mHeaderFile->readOldHeader failed for docid:" << docid
					<< " header pointer:" << sHeaderToRead.toStr() << enderr;
			return NULL;
		}
		if (docid != pHeader->getDocid())
		{
			OmnAlarm << "docid:" << docid << " != pHeader->getDocid():" << pHeader->getDocid()
					<< " header pos:" << sHeaderToRead.toStr() << enderr;
			pHeader->dump();
			return NULL;
		}
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(allocateHeaderCacheNoLock(docid, sHeaderToRead));
		if (0 != iRet)
		{
			OmnAlarm << "serializeToCacheAndHeaderFileBuff failed for docid:" << docid << " , iRet=" << iRet << enderr;
			pHeader->dump();
			return NULL;
		}
//		if (pHeader->getPrevHeaderPtr() != sPrevHeader && !bFirstTime)
//		{
//			OmnAlarm << "pHeader->getPrevHeaderPtr():" << pHeader->getPrevHeaderPtr().toStr()
//					<< " != prevHeader:" << sPrevHeader.toStr() << " docid:" << docid
//					<< " thisHeader:" << sHeaderToRead.toStr() << enderr;
//			pHeader->dump();
//			return NULL;
//		}
//		bFirstTime = false;
		sPrevHeader = sHeaderToRead;
		sHeaderToRead = pHeader->getNextHeaderPtr();
	}while(AosBlobHeader::noNextHeader != pHeader->getNextHeaderPtr() && ullTimestamp < pHeader->getTimestamp());
		//has older headers to read
	return pHeader;
}


char*
AosOldHeaderCache::allocateHeaderCacheNoLock(
		const u64							docid,
		const AosBlobHeader::HeaderPointer&	sNextHeader)
{
	int iRet = 0;
	if (0 == getAvailableHeaderCacheBlockCount())
	{
		if (0 != (iRet = ageOutHeaderBlocks()))
		{
			OmnAlarm << "ageOutHeaderBlocks failed for docid:" << docid << enderr;
			return NULL;
		}
	}
	u32 ulAvailableBlock = getAvailableHeaderCacheBlockPointer();
	if (0 == ulAvailableBlock)
	{
		OmnAlarm << "getAvailableHeaderCacheBlockPointer failed for docid:" << docid << enderr;
		return NULL;
	}
	char* p = getHeaderCachePointerByOffset(ulAvailableBlock);
	if (!p)
	{
		OmnAlarm << "getHeaderCachePointerByOffset failed on ulAvailableBlobck:" << ulAvailableBlock << enderr;
		return NULL;
	}
	else
	{
		mLockRaw->lock();
		mCacheMap[getHashKey(sNextHeader)] = ulAvailableBlock;
		mLockRaw->unlock();
		return p;
	}
}


u32
AosOldHeaderCache::getAvailableHeaderCacheBlockCount()
{
	return mAvailableBlocks.size();
}


int
AosOldHeaderCache::ageOutHeaderBlocks()	//TODO:
{
//	char* pCache = getHeaderCachePointerByOffset(*it);
//	if (!pCache)
//	{
//		OmnAlarm << "getHeaderCachePointerByOffset failed for offset:" << *it << enderr;
//		return -2;
//	}
//	int iRet = 0;
//	//collect the available blocks
//	while(AosBlobHeader::noNextHeader != pHeader->getNextHeader())
//	{
//		mAvailableBlocks.push(pHeader->getNextHeader());
//		pCache = getHeaderCachePointerByOffset(pHeader->getNextHeader());
//		if (!pCache)
//		{
//			OmnAlarm << "getHeaderCachePointerByOffset failed for offset:" << pHeader->getNextHeaderInCache() << enderr;
//			return -5;
//		}
//		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(pCache);
//		if (0 != iRet)
//		{
//			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for aging" << enderr;
//			return -6;
//		}
//	}
//	mpTail = mpTail->mPrev;
//	mHeaderMap.erase(pHeader->getDocid());
	return 0;
}


u32
AosOldHeaderCache::getAvailableHeaderCacheBlockPointer()
{
	if (0 == getAvailableHeaderCacheBlockCount())
	{
		OmnAlarm << "there is no available header block!" << enderr;
		return 0;	//0 as a guard
	}
	i32 iAvailableBlock = mAvailableBlocks.front();
	mAvailableBlocks.pop();
	return iAvailableBlock;
}


char*
AosOldHeaderCache::getHeaderCachePointerByOffset(const u32 offset)
{
	if (mHeaderCacheBuff.isNull())
	{
		OmnAlarm << "mHeaderCacheBuff.isNull()" << enderr;
		return NULL;
	}
	if (mHeaderCacheBuff->buffLen() < offset)
	{
		OmnAlarm << "mHeaderCacheBuff->buffLen():" << mHeaderCacheBuff->buffLen() << " < offset:" << offset << enderr;
		return NULL;
	}
	if (0 == offset)	//use 0 as a guard
	{
		return NULL;
	}
	return mHeaderCacheBuff->data() + offset;
}


int
AosOldHeaderCache::initHeaderCacheBuff(const u32 ulHeaderSize)
{
	if (mHeaderCacheBuff.isNull())
	{
		OmnAlarm << "mHeaderCacheBuff.isNull()" << enderr;
		return -1;
	}
	for (u32 i = ulHeaderSize;i < mHeaderCacheBuff->buffLen();i += ulHeaderSize)
	{
		mAvailableBlocks.push(i);
	}
	OmnScreen << "mAvailableBlocks.size():" << mAvailableBlocks.size() << endl;
	return 0;
}


bool
AosOldHeaderCache::isHeaderCached(const AosBlobHeader::HeaderPointer& sNextHeader)
{
	if (AosBlobHeader::noNextHeader == sNextHeader || AosBlobHeader::unresolvedNextHeader == sNextHeader ||
			AosBlobHeader::noPrevHeader == sNextHeader)
	{
		return false;
	}
	mLockRaw->lock();
	itr_t it = mCacheMap.find(getHashKey(sNextHeader));
	if (mCacheMap.end() == it)
	{
		mLockRaw->unlock();
		return false;
	}
	mLockRaw->unlock();
	return true;
}


u64
AosOldHeaderCache::getHashKey(const AosBlobHeader::HeaderPointer& sNextHeader)
{
	u64 ullSeqno = sNextHeader.ulSeqno;
	u64 ullKey = (ullSeqno << 32) & 0xFFFFFFFF00000000;
	ullKey |= (u64)sNextHeader.ulOffset;
	return ullKey;
}


u32
AosOldHeaderCache::getOffsetByNextHeaderPointer(const AosBlobHeader::HeaderPointer& sNextHeader)
{
	mLockRaw->lock();
	itr_t it = mCacheMap.find(getHashKey(sNextHeader));
	if (mCacheMap.end() == it)
	{
		mLockRaw->unlock();
		return 0;
	}
	else
	{
		u32 ulTempOffset = it->second;
		mLockRaw->unlock();
		return ulTempOffset;
	}
}


int
AosOldHeaderCache::updatePrevHeaderPointer(
		AosRundata							*rdata,
		const u64							docid,
		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
		const AosBlobHeader::HeaderPointer	&sPrevHeaderPointer)
{
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for header:" << sHeaderToUpdate.toStr() << enderr;
		return -1;
	}
	if (AosBlobHeader::noPrevHeader == sPrevHeaderPointer || AosBlobHeader::noNextHeader == sPrevHeaderPointer ||
			AosBlobHeader::unresolvedNextHeader == sPrevHeaderPointer ||
			AosBlobHeader::noPrevHeader == sHeaderToUpdate || AosBlobHeader::noNextHeader == sHeaderToUpdate ||
			AosBlobHeader::unresolvedNextHeader == sHeaderToUpdate)
	{
		OmnAlarm << "illegal sPrevHeaderPointer or sHeaderToUpdate, header to update:" << sHeaderToUpdate.toStr()
				<< " prev header pointer:" << sPrevHeaderPointer.toStr() << enderr;
		return -2;
	}
	if(isHeaderCached(sHeaderToUpdate))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sHeaderToUpdate.toStr() << " iRet=" << iRet << enderr;
			return -3;
		}
	}
	else
	{
		pHeader = mHeaderFileMgr->readOldHeader(rdata, sHeaderToUpdate);
		if (pHeader.isNull())
		{
			OmnAlarm << "mHeaderFile->readOldHeader failed for docid:" << docid
					<< " header pointer:" << sHeaderToUpdate.toStr()
					<< " rawfid:" << mHeaderFileMgr->getOldHeaderFileRawfidBySeqno(sHeaderToUpdate.ulSeqno) << enderr;
			return -4;
		}
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(allocateHeaderCacheNoLock(docid, sHeaderToUpdate));
		if (0 != iRet)
		{
			OmnAlarm << "serializeToCacheAndHeaderFileBuff failed for docid:" << docid << " , iRet=" << iRet << enderr;
			return -5;
		}
	}
	if (pHeader->getDocid() != docid)
	{
		OmnAlarm << "pHeader->getDocid():" << pHeader->getDocid() << " != docid:" << docid << enderr;
		return -6;
	}
	pHeader->setPrevHeader(sPrevHeaderPointer);
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " header to udpate:" << sHeaderToUpdate.toStr() << enderr;
		return -6;
	}
	iRet = mHeaderFileMgr->overwriteOldHeader(rdata, pHeader, sHeaderToUpdate);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderFileMgr->overwriteOldHeader failed, iRet=" << iRet << " docid:" << docid
				<< " header pos:" << sHeaderToUpdate.toStr() << enderr;
		return -7;
	}
	return 0;
}


int
AosOldHeaderCache::updateNextHeaderPointer(
		AosRundata							*rdata,
		const u64							docid,
		const AosBlobHeader::HeaderPointer	&sHeaderToUpdate,
		const AosBlobHeader::HeaderPointer	&sNextHeaderPointer)
{
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for header:" << sHeaderToUpdate.toStr() << enderr;
		return -1;
	}
	if (AosBlobHeader::noPrevHeader == sNextHeaderPointer || AosBlobHeader::unresolvedNextHeader == sNextHeaderPointer
			|| AosBlobHeader::noPrevHeader == sHeaderToUpdate || AosBlobHeader::noNextHeader == sHeaderToUpdate ||
			AosBlobHeader::unresolvedNextHeader == sHeaderToUpdate)
	{
		OmnAlarm << "illegal sNextHeaderPointer or sHeaderToUpdate, header to update:" << sHeaderToUpdate.toStr()
				<< " next header pointer:" << sNextHeaderPointer.toStr() << enderr;
		return -2;
	}
	if(isHeaderCached(sHeaderToUpdate))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sHeaderToUpdate.toStr() << " iRet=" << iRet << enderr;
			return -3;
		}
	}
	else
	{
		pHeader = mHeaderFileMgr->readOldHeader(rdata, sHeaderToUpdate);
		if (pHeader.isNull())
		{
			OmnScreen << "header file has already been comapcted, header pointer:" << sHeaderToUpdate.toStr() << endl;
			return 0;
		}
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(allocateHeaderCacheNoLock(docid, sHeaderToUpdate));
		if (0 != iRet)
		{
			OmnAlarm << "serializeToCacheAndHeaderFileBuff failed for docid:" << docid << " , iRet=" << iRet << enderr;
			return -4;
		}
	}
	if (pHeader->getDocid() != docid)
	{
		OmnAlarm << "pHeader->getDocid():" << pHeader->getDocid() << " != docid:" << docid << enderr;
		return -5;
	}
	pHeader->setNextHeader(sNextHeaderPointer);
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " header to udpate:" << sHeaderToUpdate.toStr() << enderr;
		return -6;
	}
	iRet = mHeaderFileMgr->overwriteOldHeader(rdata, pHeader, sHeaderToUpdate);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderFileMgr->overwriteOldHeader failed, iRet=" << iRet << " docid:" << docid
				<< " header pos:" << sHeaderToUpdate.toStr() << enderr;
		return -7;
	}
	return 0;
}


int
AosOldHeaderCache::updateBodyPointer(
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
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	if (pHeader.isNull())
	{
		OmnAlarm << "create header obj failed for header:" << sHeaderToUpdate.toStr() << enderr;
		return -1;
	}
	if(isHeaderCached(sHeaderToUpdate))
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(
				getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed for docid:" << docid
					<< " header pointer:" << sHeaderToUpdate.toStr() << " iRet=" << iRet << enderr;
			return -2;
		}
	}
	else
	{
		pHeader = mHeaderFileMgr->readOldHeader(rdata, sHeaderToUpdate);
		if (pHeader.isNull())
		{
			OmnAlarm << "mHeaderFile->readOldHeader failed for docid:" << docid
					<< " header pointer:" << sHeaderToUpdate.toStr() << enderr;
			return -3;
		}
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(allocateHeaderCacheNoLock(docid, sHeaderToUpdate));
		if (0 != iRet)
		{
			OmnAlarm << "serializeToCacheAndHeaderFileBuff failed for docid:" << docid << " , iRet=" << iRet << enderr;
			return -4;
		}
	}
	if (ullOldRawfid != pHeader->getBodyRawfid() || ullOldOffset != pHeader->getBodyOffset()
			|| timestamp != pHeader->getTimestamp())
	{
		OmnAlarm << "docid:" << docid << " pHeader->getDocid():" << pHeader->getDocid()
				<< " ullOldRawfid:" << ullOldRawfid << " != pHeader->getBodyRawfid():" << pHeader->getBodyRawfid()
				<< " || ullOldOffset:" << ullOldOffset << " != pHeader->getBodyOffset():" << pHeader->getBodyOffset()
				<< " || timestamp:" << timestamp << " != pHeader->getTimestamp():" << pHeader->getTimestamp()
				<< " header@" << sHeaderToUpdate.toStr() << enderr;
		return -5;
	}
	pHeader->setBodyRawfid(ullNewRawfid);
	pHeader->setBodyOffset(ullNewOffset);
	iRet = pHeader->serializeToCacheAndHeaderFileBuff(
			getHeaderCachePointerByOffset(getOffsetByNextHeaderPointer(sHeaderToUpdate)));
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet
				<< " header to udpate:" << sHeaderToUpdate.toStr() << enderr;
		return -6;
	}
	iRet = mHeaderFileMgr->overwriteOldHeader(rdata, pHeader, sHeaderToUpdate);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderFileMgr->overwriteOldHeader failed, iRet=" << iRet << " docid:" << docid
				<< " header pos:" << sHeaderToUpdate.toStr() << enderr;
		return -7;
	}
	return 0;
}
