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
// 23 Jul 2015 created by White
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/DiskRecovery.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/BodyCache.h"
#include "BlobSE/BodyEntry.h"
#include "BlobSE/BucketMgr.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/OprIDCache.h"
#if 0
AosDiskRecovery::AosDiskRecovery(
		AosBlobSE					*blobSE,
		AosRawSE					*rawSE,
		AosHeaderFileMgrPtr			&pHeaderFileMgr,
		AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
		AosHeaderCachePtr			&pHeaderCache)
:
mBlobSE(blobSE),
mRawSE(rawSE),
mHeaderFileMgr(pHeaderFileMgr)
{
	mBucketMgr = OmnNew AosBucketMgr(blobSE, rawSE, pHeaderFileMgr, pHeaderBuffAndFileMgr, pHeaderCache);
}


AosDiskRecovery::~AosDiskRecovery()
{
}


int
AosDiskRecovery::doDiskRecovery(AosRundata *rdata)
{
	int iRet = 0;
	iRet = scanOprID(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "scanOprID failed, iRet=" << iRet << enderr;
		return -1;
	}
	iRet = scanHeaderFiles(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "scanHeaderFiles failed, iRet=" << iRet << enderr;
		return -2;
	}
	iRet = syncDamagedEntries(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "syncDamagedEntries failed, iRet=" << iRet << enderr;
		return -3;
	}
	iRet = restoreHeaders(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "restoreHeaders failed, iRet=" << iRet << enderr;
		return -4;
	}
	return 0;
}


int
AosDiskRecovery::scanOprID(AosRundata *rdata)
{
	int iRet = 0;
	AosOprIDCache::OprIDEntry sOprIDEntry;
	sOprIDEntry.ullOprID = AosOprIDCache::eStartOprID;
	AosBuffPtr pEntryBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBlobHeaderPtr pHeader = NULL;
	while(1 == (iRet = mOprIDCache->getNextOprIDEntry(rdata, sOprIDEntry)))
	{
		iRet = mBucketMgr->putOprIDEntryIntoBucket(rdata, sOprIDEntry);
		if (0 != iRet)
		{
			OmnAlarm << "mBucketMgr->insertIntoBucket failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
			return -1;
		}
		if (bodyDamaged(rdata, sOprIDEntry.ullRawfid))
		{
			pushDamagedOprId(sOprIDEntry);
		}
		else if (mOprIDCache->oprHasHeader(sOprIDEntry))
		{
			iRet = mBodyCache->readEntry(rdata, sOprIDEntry.ullRawfid, sOprIDEntry.ullOffset, sOprIDEntry.ullLen, pEntryBuff);
			if (0 != iRet)
			{
				OmnAlarm << "mBodyCache->readEntry failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
				return -2;
			}
			iRet = generateHeader(rdata, pHeader, pEntryBuff);
			if (0 != iRet)
			{
				OmnAlarm << "generateHeader failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
				return -3;
			}
			iRet = mBucketMgr->putHeaderIntoBucket(rdata, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "mBucketMgr->insertHeaderIntoBucket failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
				return -4;
			}
		}
	}
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->getNextOprIDEntry failed, iRet=" << iRet << " oprid:" << sOprIDEntry.ullOprID << enderr;
		return -5;
	}
	return 0;
}


int
AosDiskRecovery::syncDamagedEntries(AosRundata *rdata)
{
	//TODO:sync entries
	int iRet = 0;
	AosBuffPtr pEntryBuff;
	AosOprIDCache::OprIDEntry sOprIDEntry;
	iRet = mBodyCache->saveEntryToPos(rdata, sOprIDEntry.ullRawfid, sOprIDEntry.ullOffset, pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mBodyCache->saveEntryToPos failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
		return -1;
	}
	AosBlobHeaderPtr pHeader = NULL;
	if (mOprIDCache->oprHasHeader(sOprIDEntry))
	{
		iRet = generateHeader(rdata, pHeader, pEntryBuff);
		if (0 != iRet)
		{
			OmnAlarm << "generateHeader failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
			return -2;
		}
		if (pHeader.isNull())
		{
			OmnAlarm << "pHeader.isNull(), generateHeader failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
			return -2;
		}
		pHeader->setStatus(AosBlobHeader::eDamaged);
		iRet = mBucketMgr->putHeaderIntoBucket(rdata, pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mBucketMgr->insertHeaderIntoBucket failed, iRet=" << iRet << enderr;
			return -3;
		}
	}
	return 0;
}


bool
AosDiskRecovery::bodyDamaged(
		AosRundata	*rdata,
		const u32	ulBodySeqno)
{
	OmnNotImplementedYet;
}


void
AosDiskRecovery::pushDamagedOprId(const AosOprIDCache::OprIDEntry &sOprIDEntry)
{
	mOprIDOfDamagedEntriesVec.push_back(sOprIDEntry);
}


int
AosDiskRecovery::generateHeader(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader,
		AosBuffPtr			&pEntryBuff)
{
	AosBodyEntry bodyEntry(mHeaderCustomDataSize);
	int iRet = bodyEntry.unserializeFromBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet << enderr;
		return -1;
	}
	pHeader = bodyEntry.getHeader();
	return 0;
}


int
AosDiskRecovery::restoreHeaders(AosRundata *rdata)
{
	return mBucketMgr->restoreHeaders(rdata);
}


int
AosDiskRecovery::scanHeaderFiles(AosRundata *rdata)
{
	int iRet = getAllExistingHeaderFileRawfid(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "getAllExistingHeaderFileRawfid failed, iRet=" << iRet << enderr;
		return -1;
	}
	AosRawFilePtr pHeaderFile = NULL;
	AosBuffPtr pBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	AosBlobHeader::HeaderPointer sHeaderPtr;
	for (existing_rawfid_vec::iterator it = mExistingHeaderFileRawfidVec.begin(); it != mExistingHeaderFileRawfidVec.end(); it++)
	{
		pHeaderFile = mRawSE->getRawFile(rdata, mBlobSE->getConfig().siteid, mBlobSE->getConfig().cubeid,
				mBlobSE->getConfig().aseid, *it, AosRawFile::eReadWrite);
		if (pHeaderFile.isNull())
		{
			OmnAlarm << "mRawSE->getRawFile failed, siteid:" << mBlobSE->getConfig().siteid
					<< " cubeid:" << mBlobSE->getConfig().cubeid << " aseid:" << mBlobSE->getConfig().aseid
					<< " rawfid:" << *it << enderr;
			return -2;
		}
		iRet = pHeaderFile->read(rdata, 0, pHeaderFile->getLength(), pBuff);
		if (0 != iRet)
		{
			OmnAlarm << "header file read failed, iRet=" << iRet << " rawfid:" << pHeaderFile->getRawFileID()
					<< " read len:" << pHeaderFile->getLength() << enderr;
			return -3;
		}
		i64 illOffset = 0;
		while(pBuff->dataLen() > illOffset)
		{
			iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(pBuff->data() + illOffset);
			if (0 != iRet)
			{
				OmnScreen << "pHeader->unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
						<< " header rawfid:" << pHeaderFile->getRawFileID() << " offset:" << illOffset
						<< " thers is something wrong if the header file is not a normal header file." << endl;
			}
			else
			{
				pHeader->setStatus(AosBlobHeader::eGood);
				sHeaderPtr.ulSeqno = mHeaderFileMgr->getOldHeaderFileSeqnoByRawfid(*it);
				sHeaderPtr.ulOffset = (u32)illOffset;
				pHeader->setSelfPos(sHeaderPtr);
				iRet = mBucketMgr->putHeaderIntoBucket(rdata, pHeader);
				if (0 != iRet)
				{
					OmnAlarm << "mBucketMgr->insertHeaderIntoBucket failed, iRet=" << iRet << enderr;
					pHeader->dump();
					return -4;
				}
			}
			illOffset += pHeader->getHeaderInCacheAndHeaderFileLength();
		}
	}
	return 0;
}


int
AosDiskRecovery::getAllExistingHeaderFileRawfid(AosRundata *rdata)
{
	int iRet = getAllExistingNormalHeaderFileRawfid(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "getAllExistingNormalHeaderFileRawfid failed, iRet=" << iRet << enderr;
		return -1;
	}
	iRet = getAllExistingOldHeaderFileRawfid(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "getAllExistingOldHeaderFileRawfid failed, iRet=" << iRet << enderr;
		return -2;
	}
	return 0;
}


int
AosDiskRecovery::getAllExistingNormalHeaderFileRawfid(AosRundata *rdata)
{
	for (u64 ullRawfid = eHeaderFileID_start + 1; ullRawfid <= eHeaderFileID_end; ullRawfid++)
	{
		if (mRawSE->fileExists(rdata, mBlobSE->getConfig().siteid, mBlobSE->getConfig().cubeid,
				mBlobSE->getConfig().aseid, ullRawfid))
		{
			mExistingHeaderFileRawfidVec.push_back(ullRawfid);
		}
	}
	return 0;
}


int
AosDiskRecovery::getAllExistingOldHeaderFileRawfid(AosRundata *rdata)
{
	//TODO:use epoch day to get all the old header files' rawfids
	for (u64 ullRawfid = eOldHeaderFileID_start + 1; ullRawfid <= eOldHeaderFileID_end; ullRawfid++)
	{
		if (mHeaderFileMgr->isOldHeaderFileMetaFileSeqno(mHeaderFileMgr->getOldHeaderFileSeqnoByRawfid(ullRawfid)))
		{
			continue;
		}
		if (mRawSE->fileExists(rdata, mBlobSE->getConfig().siteid, mBlobSE->getConfig().cubeid,
				mBlobSE->getConfig().aseid, ullRawfid))
		{
			mExistingHeaderFileRawfidVec.push_back(ullRawfid);
		}
	}
	return 0;
}
#endif
