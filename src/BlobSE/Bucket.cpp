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
#include "BlobSE/Bucket.h"

#include "Alarm/Alarm.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/HeaderBuffAndFileMgr.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/HeaderFileMgr.h"
#include "BlobSE/RawfidRange.h"
#include "Util/BuffArray.h"
#if 0
AosBucket::AosBucket(
		AosRundata					*rdata,
		const u32					ulBucketID,
		AosBlobSE					*blobSE,
		AosRawSE					*rawSE,
		AosHeaderFileMgrPtr			&pHeaderFileMgr,
		AosHeaderBuffAndFileMgrPtr	&pHeaderBuffAndFileMgr,
		AosHeaderCachePtr			&pHeaderCache)
:
mBucketID(ulBucketID),
mHeaderFileMgr(pHeaderFileMgr),
mHeaderBuffAndFileMgr(pHeaderBuffAndFileMgr),
mNormalHeaderCache(pHeaderCache)
{
	mHeaderBucketFile = rawSE->getRawFile(rdata, blobSE->getConfig().siteid, blobSE->getConfig().cubeid,
			blobSE->getConfig().aseid, getHeaderBucketFileRawfidByBucketID(), AosRawFile::eModifyOnly);
	mOprIDBucketFile = rawSE->getRawFile(rdata, blobSE->getConfig().siteid, blobSE->getConfig().cubeid,
			blobSE->getConfig().aseid, getOprIDBucketFileRawfidByBucketID(), AosRawFile::eModifyOnly);
}

AosBucket::~AosBucket()
{
    // TODO Auto-generated destructor stub
}


int
AosBucket::putOprIDEntryIntoBucket(
		AosRundata						*rdata,
		const AosOprIDCache::OprIDEntry	&sOprIDEntry)
{
	AosBuff buff(sizeof(AosOprIDCache::OprIDEntry));
	AosOprIDCache::OprIDEntry* pOprIDEntry = (AosOprIDCache::OprIDEntry*)buff.data();
	*pOprIDEntry = sOprIDEntry;
	int iRet = mOprIDBucketFile->append(rdata, buff.data(), (u64)sizeof(AosOprIDCache::OprIDEntry));
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDBucketFile->append failed, iRet=" << iRet << sOprIDEntry.toStr() << enderr;
		return -1;
	}
	return 0;
}


int
AosBucket::putHeaderIntoBucket(
		AosRundata				*rdata,
		const AosBlobHeaderPtr	&pHeader)
{
	AosBuff buff(pHeader->getHeaderInCacheAndHeaderFileLength());
	int iRet = pHeader->serializeToHeaderBucketFileBuff(buff.data());
	if (0 != iRet)
	{
		OmnAlarm << "pHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet << enderr;
		pHeader->dump();
		return -1;
	}
	iRet  = mHeaderBucketFile->append(rdata, buff.data(), pHeader->getHeaderInCacheAndHeaderFileLength());
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderBucketFile->append failed, iRet=" << iRet << enderr;
		pHeader->dump();
		return -2;
	}
	return 0;
}


u64
AosBucket::getHeaderBucketFileRawfidByBucketID()
{
	return eHeaderBucketFileID_start + mBucketID;
}


u64
AosBucket::getOprIDBucketFileRawfidByBucketID()
{
	return eOprIDBucketFileID_start + mBucketID;
}


int
AosBucket::restoreHeaders(AosRundata *rdata)
{
	int iRet = constructHeaderBuffArray(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderBuffArray failed, iRet=" << iRet << " bucket id:" << mBucketID << enderr;
		return -1;
	}
	iRet = constructOprIDBuffArray(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "constructOprIDBuffArray failed, iRet=" << iRet << " bucket id:" << mBucketID << enderr;
		return -2;
	}
	iRet = constructHeaderLinksAndRecoverHeaders(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderLinksAndRecoverHeaders failed, iRet=" << iRet << " bucket id:" << mBucketID << enderr;
		return -3;
	}
	iRet = mHeaderBuffAndFileMgr->flushRecreatedHeaders(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "flushRecreatedHeaders failed, iRet=" << iRet << enderr;
		return -4;
	}
	return 0;
}


int
AosBucket::constructHeaderBuffArray(AosRundata *rdata)
{
	u64 ullHeaderBucketFileLen = mHeaderBucketFile->getLength();
	AosBuffPtr pHeaderBucketFileBuff = OmnNew AosBuff(ullHeaderBucketFileLen AosMemoryCheckerArgs);
	if (0 == ullHeaderBucketFileLen)
		//maybe all headers belong to this bucket is damaged and all opr ids belong to this bucket do not have a header
	{
		OmnAlarm << "0 == ullHeaderBucketFileLen, please verify it. rawfid:" << mHeaderBucketFile->getRawFileID() << enderr;
		return 0;	//TODO:this is a corner case, we may suppress the alarm after debugging
	}
	int iRet = mHeaderBucketFile->read(rdata, 0, ullHeaderBucketFileLen, pHeaderBucketFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderBucketFile->read failed, iRet=" << iRet << " rawfid:" << mHeaderBucketFile->getRawFileID()
				<< " read len:" << ullHeaderBucketFileLen << enderr;
		return -1;
	}
	mHeaderBuffArray->setBuff(pHeaderBucketFileBuff);
	if (!mHeaderBuffArray->sort())
	{
		OmnAlarm << "mHeaderBuffArray->sort() failed, number of entries:" << mHeaderBuffArray->getNumEntries() << enderr;
		return -2;
	}
	return 0;
}


int
AosBucket::constructOprIDBuffArray(AosRundata *rdata)
{
	u64 ullOprIDBucketFileLen = mOprIDBucketFile->getLength();
	if (0 == ullOprIDBucketFileLen)
	{
		OmnAlarm << "0 == ullOprIDBucketFileLen, rawfid:" << mOprIDBucketFile->getRawFileID() << enderr;
		return -1;
	}
	AosBuffPtr pOprIDBucketFileBuff = OmnNew AosBuff(ullOprIDBucketFileLen AosMemoryCheckerArgs);
	int iRet = mOprIDBucketFile->read(rdata, 0, ullOprIDBucketFileLen, pOprIDBucketFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDBucketFile->read failed, iRet=" << iRet << " rawfid:" << mOprIDBucketFile->getRawFileID()
							<< " read len:" << ullOprIDBucketFileLen << enderr;
		return -2;
	}
	mOprIDBuffArray->setBuff(pOprIDBucketFileBuff);
	if (!mOprIDBuffArray->sort())
	{
		OmnAlarm << "mOprIDBuffArray->sort(), number of entries:" << mOprIDBuffArray->getNumEntries() << enderr;
		return -3;
	}
	return 0;
}


int
AosBucket::constructHeaderLinksAndRecoverHeaders(AosRundata *rdata)
{
	int iRet = 0;
	i64 illHeaderStartPos = 0;
	i64 illHeaderEndPos = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	while(mHeaderBuffArray->getNumEntries() > illHeaderStartPos)
	{
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(mHeaderBuffArray->getEntry(illHeaderStartPos));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet
					<< " start pos:" << illHeaderStartPos << enderr;
			return -1;
		}
		OmnScreen << "docid:" << pHeader->getDocid() << "@illStartPos:" << illHeaderStartPos << endl;
		u64 ullTmpDocid = pHeader->getDocid() + 1;
		pHeader->setDocid(ullTmpDocid);
		AosBuff tempBuff(pHeader->getHeaderInCacheAndHeaderFileLength());
		iRet = pHeader->serializeToCacheAndHeaderFileBuff(tempBuff.data());
		if (0 != iRet)
		{
			OmnAlarm << "mHeader->serializeToCacheAndHeaderFileBuff failed, iRet=" << iRet << enderr;
			return -2;
		}
		illHeaderEndPos = mHeaderBuffArray->findBoundary(illHeaderStartPos, tempBuff.data(), false, false);
		if (0 == illHeaderEndPos)
		{
			OmnAlarm << "findBoundary returns 0, ullTmpRawfid:" << ullTmpDocid
					<< " illStartPos:" << illHeaderStartPos << enderr;
			return -3;
		}
		illHeaderEndPos--;
		ullTmpDocid--;	//restore it to the one being compacted
		OmnScreen << "processing docid:" << ullTmpDocid << " with " << illHeaderEndPos - illHeaderStartPos + 1
				<< " headers" << endl;
		if (0 == ullTmpDocid)
		{
			OmnShouldNeverComeHere;
			illHeaderStartPos = illHeaderEndPos + 1;
			continue;
		}
		iRet = processOneDocid(rdata, ullTmpDocid, illHeaderStartPos, illHeaderEndPos);
		if (0 != iRet)
		{
			OmnAlarm << "constructHeaderLinks failed, iRet=" << iRet << " docid:" << ullTmpDocid
					<< " start pos:" << illHeaderStartPos << " end pos:" << illHeaderEndPos << enderr;
			return -4;
		}
	}
	return 0;
}


int
AosBucket::processOneDocid(
		AosRundata	*rdata,
		const u64	ullDocid,
		const i64	illStartPos,
		const i64	illEndPos)
{
	header_vec_t vHeader;
	int iRet = constructHeaderVector(rdata, ullDocid, illStartPos, illEndPos, vHeader);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderVector failed, iRet=" << iRet << " docid:" << ullDocid
				<< " start pos:" << illStartPos << " end pos:" << illEndPos << enderr;
		return -1;
	}
	iRet = resolveHeaderPos(rdata, vHeader);
	if (0 != iRet)
	{
		OmnAlarm << "constructHeaderLinkRecursive failed, iRet=" << iRet << " docid:" << ullDocid << enderr;
		return -2;
	}
	iRet = flushRecoveredHeaders(rdata, vHeader);
	if (0 != iRet)
	{
		OmnAlarm << "flushRecoveredHeaders failed, iRet=" << iRet << " docid:" << ullDocid << enderr;
		return -3;
	}
	return 0;
}


int
AosBucket::constructHeaderVector(
		AosRundata		*rdata,
		const u64		ullDocid,
		const i64		illStartPos,
		const i64		illEndPos,
		header_vec_t	&vHeader)
{
	/*
	 * This function creates a header vector which should include every header of the given docid
	 * from the header buff array and the oprid list.
	 * The header buff array that is sorted by docid ascending and oprid descending may have two
	 *  headers for one oprid, one is good, the other is damaged.
	 * The oprid list is sorted by docid ascending and oprid descending
	 */
	vHeader.clear();
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
	AosBlobHeaderPtr pNextHeader;
	int iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(mOprIDBuffArray->getEntry(illStartPos));
	if (0 != iRet)
	{
		OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet << " docid:" << ullDocid
				<< " pos:" << illStartPos << enderr;
		return -1;
	}
	i64 illOprStartPos = 0;
	i64 illOprEndPos = 0;
	iRet = getOprIDPointersByDocid(ullDocid, illOprStartPos, illOprEndPos);
	if (0 != iRet)
	{
		OmnAlarm << "getOprIDPointersByDocid failed, iRet=" << iRet << " docid:" << ullDocid << enderr;
		return -2;
	}
	i64 illOprPos = illOprStartPos;
	AosOprIDCache::OprIDEntry* pOprIDEntry;
	i64 illHeaderPos = illStartPos;
	while(illHeaderPos <= illEndPos)
	{
		pOprIDEntry = (AosOprIDCache::OprIDEntry*)mOprIDBuffArray->getEntry(illOprPos);
		pHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
		iRet = pHeader->unserializeFromCacheAndHeaderFileBuff(mOprIDBuffArray->getEntry(illHeaderPos));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet << " docid:" << ullDocid
					<< " pos:" << illHeaderPos << enderr;
			return -3;
		}
		if (pHeader->getOprID() != pOprIDEntry->ullOprID)
		{
			OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << "!= pOprIDEntry->ullOprID:"
					<< pOprIDEntry->ullOprID << "something is wrong with the sort." << enderr;
			return -4;
		}
		pNextHeader = OmnNew AosBlobHeader(0, mHeaderCustomDataSize);
		iRet = pNextHeader->unserializeFromCacheAndHeaderFileBuff(mOprIDBuffArray->getEntry(illHeaderPos + 1));
		if (0 != iRet)
		{
			OmnAlarm << "unserializeFromCacheAndHeaderFileBuff failed, iRet=" << iRet << " docid:" << ullDocid
					<< " pos:" << illHeaderPos + 1 << enderr;
			return -5;
		}
		if (pHeader->getOprID() == pNextHeader->getOprID())
		{
			if (pHeader->getStatus() == AosBlobHeader::eGood)
			{
				vHeader.push_back(pHeader);
			}
			else if (pNextHeader->getStatus() != AosBlobHeader::eGood)
			{
				OmnAlarm << "there must be one good header within two headers with the same opr id!" << enderr;
				pHeader->dump();
				pNextHeader->dump();
				return -6;
			}
			else
			{
				vHeader.push_back(pNextHeader);
			}
			illOprPos++;
			illHeaderPos += 2;
			continue;
		}
		//pHeader->getOprID() != pNextHeader->getOprID()
		illOprPos++;
		illHeaderPos++;
	}
	return 0;
}


int
AosBucket::saveNormalHeader(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
	if (pHeader->getStatus() == AosBlobHeader::eGood)
	{
		OmnScreen << "normal header is not damaged, skip. docid:" << pHeader->getDocid() << endl;
		return 0;
	}
	bool bNormalHeaderFileExists = false;
	int iRet = 0;
	if (mHeaderFileMgr->getNormalHeaderFileSeqnoByDocid(pHeader->getDocid()) != mNormalHeaderFileSeqno)
	{
		iRet = mNormalHeaderCache->reloadNormalHeaderFileBuff(
				rdata, pHeader->getDocid(), mNormalHeaderFileSeqno, mNormalHeaderFileBuff, bNormalHeaderFileExists);
		if (0 != iRet)
		{
			OmnAlarm << "reloadNormalHeaderFileBuff failed, iRet=" << iRet << enderr;
			pHeader->dump();
			return -1;
		}
	}
	iRet = mHeaderFileMgr->serializeHeaderToNormalHeaderFileBuff(pHeader, mNormalHeaderFileBuff);
	if (0 != iRet)
	{
		OmnAlarm << "serializeHeaderToNormalHeaderFileBuff failed, iRet=" << iRet << enderr;
		pHeader->dump();
		return -2;
	}
	return 0;
}


int
AosBucket::saveOldHeaderToPos(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
	return mHeaderFileMgr->recoverOldHeader(rdata, pHeader);
}


int
AosBucket::resolveHeaderPos(
		AosRundata		*rdata,
		header_vec_t	&vHeader)
{
	int iRet = 0;
	if (vHeader.size() == 1)
	{
		vHeader[0]->setNextHeader(AosBlobHeader::noNextHeader);
		return 0;
	}
	vHeader[0]->setSelfPos(AosBlobHeader::noPrevHeader);	//to simplify the following operations
	int j = vHeader.size() - 1;
	for(int i = 1;i < vHeader.size() - 1;i++)	//starts from the fisrt old header
	{
		if (vHeader[i]->getStatus() == AosBlobHeader::eGood)
		{
//			if (vHeader[i + 1]->getStatus() == AosBlobHeader::eDamaged)
//			{
//				vHeader[i + 1]->setSelfPos(vHeader[i]->getNextHeaderPtr());
//				if (i + 2 <= vHeader.size() - 1)	//i + 2 does not exceed the boundary
//				{
//					vHeader[i + 2]->setPrevHeader(vHeader[i]->getNextHeaderPtr());
//				}
//			}
			continue;
		}
		//vHeader[i]->getStatus() == AosBlobHeader::eDamaged
		if (vHeader[i - 1]->getStatus() == AosBlobHeader::eGood)
		{
			vHeader[i]->setSelfPos(vHeader[i - 1]->getNextHeaderPtr());
			vHeader[i]->setStatus(AosBlobHeader::eRecoverd);
			if (vHeader[i + 1]->getStatus() == AosBlobHeader::eDamaged)
			{
				vHeader[i + 1]->setPrevHeader(vHeader[i - 1]->getNextHeaderPtr());
			}
			continue;
		}
		//vHeader[i - 1]->getStatus() == AosBlobHeader::eDamaged
		if (vHeader[i + 1]->getStatus() == AosBlobHeader::eGood)
		{
			vHeader[i]->setSelfPos(vHeader[i + 1]->getPrevHeaderPtr());
			vHeader[i]->setStatus(AosBlobHeader::eRecoverd);
			vHeader[i - 1]->setNextHeader(vHeader[i + 1]->getPrevHeaderPtr());
			continue;
		}
	}
	if (vHeader[vHeader.size() - 1]->getStatus() == AosBlobHeader::eDamaged)
	{
		if (vHeader[vHeader.size() - 2]->getStatus() == AosBlobHeader::eGood)
		{
			vHeader[vHeader.size() - 1]->setSelfPos(vHeader[vHeader.size() - 2]->getNextHeaderPtr());
		}
		//vHeader[vHeader.size() - 2]->getStatus() == AosBlobHeader::eDamaged
		iRet = allocateSpaceForDamagedHeaderWithUnresolvedPos(rdata, vHeader[vHeader.size() - 1]);
		if (0 != iRet)
		{
			OmnAlarm << "allocateSpaceForDamagedHeaderWithUnresolvedPos failed, iRet=" << iRet << enderr;
			vHeader[vHeader.size() - 1]->dump();
			return -1;
		}
		vHeader[vHeader.size() - 2]->setNextHeader(vHeader[vHeader.size() - 1]->getSelfPos());
	}
	return 0;
}


int
AosBucket::allocateSpaceForDamagedHeaderWithUnresolvedPos(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
	pHeader->setStatus(AosBlobHeader::eRecreated);
	return mHeaderBuffAndFileMgr->allocateSpaceForRecreatedHeaders(rdata, pHeader);
}


int
AosBucket::flushRecoveredHeaders(
		AosRundata		*rdata,
		header_vec_t	&vHeader)
{
	int iRet = saveNormalHeader(rdata, vHeader.front());
	if (0 != iRet)
	{
		OmnAlarm << "saveNormalHeader failed, iRet=" << iRet << " docid:" << vHeader.front()->getDocid() << enderr;
		vHeader.front()->dump();
		return -1;
	}
	if (vHeader.size() == 1)
	{
		return 0;
	}
	for (header_vec_t::iterator it = vHeader.begin(); it != vHeader.end(); it++)
	{
		if ((*it)->getStatus() == AosBlobHeader::eRecoverd)
		{
			iRet = saveOldHeaderToPos(rdata, *it);
			if (0 != iRet)
			{
				OmnAlarm << "saveOldHeaderToPos failed, iRet=" << iRet << " pos@" << (*it)->getSelfPos().toStr() << enderr;
				(*it)->dump();
				return -2;
			}
		}
		else
		{
			iRet = mHeaderBuffAndFileMgr->saveRecreatedHeaderToBuff(rdata, *it);
			if (0 != iRet)
			{
				OmnAlarm << "saveRecreatedHeaderToBuff failed, iRet=" << iRet
						<< " pos@" << (*it)->getSelfPos().toStr() << enderr;
				(*it)->dump();
				return -3;
			}
		}
	}
	return 0;
}


int
AosBucket::getOprIDPointersByDocid(
		const u64	ullDocid,
		i64			&illStartPos,
		i64			&illEndPos)
{
	AosBuff opridEntryBuff(sizeof(AosOprIDCache::OprIDEntry));
	AosOprIDCache::OprIDEntry* pOprIDEntry = (AosOprIDCache::OprIDEntry*)opridEntryBuff.data();
	pOprIDEntry->ullDocid = ullDocid;
	AosOprIDCache::OprIDEntry sOprIDEntry;
	illStartPos = mOprIDBuffArray->findBoundary(0, opridEntryBuff.data(), false, false);
	pOprIDEntry = (AosOprIDCache::OprIDEntry*)mOprIDBuffArray->getEntry(illStartPos);
	if (ullDocid != pOprIDEntry->ullDocid)
	{
		OmnAlarm << "ullDocid:" << ullDocid << " != pOprIDEntry->ullDocid:" << pOprIDEntry->ullDocid
				<< " pos:" << illStartPos << enderr;
		return -1;
	}
	pOprIDEntry = (AosOprIDCache::OprIDEntry*)opridEntryBuff.data();
	pOprIDEntry->ullDocid = ullDocid + 1;
	illEndPos = mOprIDBuffArray->findBoundary(0, opridEntryBuff.data(), false, false);
	illEndPos--;
	pOprIDEntry = (AosOprIDCache::OprIDEntry*)mOprIDBuffArray->getEntry(illEndPos);
	if (ullDocid != pOprIDEntry->ullDocid)
	{
		OmnAlarm << "ullDocid:" << ullDocid << " != pOprIDEntry->ullDocid:" << pOprIDEntry->ullDocid
				<< " pos:" << illEndPos << enderr;
		return -2;
	}
	return 0;
}


void
AosBucket::dumpHeaderVector(
		const char		*func,
		const int		line,
		header_vec_t	&vHeader)
{
	OmnScreen << "dumping header vector, called in function:" << func << " line:" << line << endl;
	for (header_vec_t::iterator it = vHeader.begin(); it != vHeader.end(); it++)
	{
		(*it)->dump();
	}
}
#endif
