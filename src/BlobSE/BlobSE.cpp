////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2014/11/07 Created by Chen Ding.
////////////////////////////////////////////////////////////////////////////
#include "BlobSE/BlobSE.h"

#include "BlobSE/BlobHeader.h"
#include "UtilData/BlobSEReqEntry.h"
#include "BlobSE/BlobSESnapShotMgr.h"
#include "BlobSE/BodyCache.h"
#include "BlobSE/BodyEntry.h"
#include "BlobSE/DeletionLog.h"
#include "BlobSE/Entry.h"
#include "BlobSE/HeaderCache.h"
#include "BlobSE/MarkerMgr.h"
#include "BlobSE/RawfidRange.h"
#include "BlobSE/OprIDCache.h"
#include "RawSE/RawSE.h"
#include "DfmUtil/DfmDocHeader.h"
#include "DfmUtil/DfmDocDatalet.h"
#include "DfmUtil/Ptrs.h"
#include "UtilData/BlobSEReqEntry.h"
#include "Thread/LockGuard.h"

AosBlobSE::AosBlobSE(
		AosRundata 					*rdata,
		const AosDfmDocPtr			&doc,
		const AosXmlTagPtr			&conf)
:
mLockRaw(OmnNew OmnMutex()),
mActiveRawFileLock(OmnNew OmnMutex()),
mLock(mLockRaw),
mDoc(doc)
{
	mIsFirstStart = false;
	if (mDoc.isNull())
	{
		rdata->setErrmsg("mDoc.isNull()");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	int iRet = 0;
	if (0 != (iRet = config(rdata, conf)))
	{
		rdata->setErrmsg("BlobSE can not configure itself.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mHeader = OmnNew AosBlobHeader(0, mConfig.header_custom_data_size);
	if (mHeader.isNull())
	{
		rdata->setErrmsg("mHeader.isNull()");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	AosXmlTagPtr config = conf->getFirstChild("RawSE", true);
	if (config.isNull())
	{
		rdata->setErrmsg("missing config for RawSE");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mRawSE = OmnNew AosRawSE(config);
	if (!mRawSE->isReady())
	{
		rdata->setErrmsg("RawSE is not ready.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	bool rslt = openActiveLogFile(rdata);
	if (!rslt)
	{
		rdata->setErrmsg("openActiveLogFile failed.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	if (mActiveRawFile.isNull())
	{
		rdata->setErrmsg("mActiveRawFile is null.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mSnapshotMgr = OmnNew AosBlobSESnapshotMgr(this, mRawSE);
	mHeaderCache = OmnNew AosHeaderCache(this, rdata, mRawSE.getPtrNoLock(), mSnapshotMgr, mHeader, conf);
	if (mHeaderCache.isNull())
	{
		rdata->setErrmsg("mHeaderCache is null.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
	mBodyCache = OmnNew AosBodyCache(rdata, mRawSE.getPtrNoLock(),
			mConfig.siteid, mConfig.cubeid, mConfig.aseid);
	if (mBodyCache.isNull())
	{
		rdata->setErrmsg("mBodyCache is null.");
		OmnThrowException(rdata->getErrmsg());
		return;
	}
//	mDelLog = OmnNew AosDeletionLog(rdata, mRawSE.getPtrNoLock(), mActiveRawFile.getPtrNoLock(), doc,
//			doc->getHeaderSize(), mHeaderCache.getPtrNoLock(), conf);
	mOprIDCache = OmnNew AosOprIDCache(rdata, mRawSE.getPtrNoLock(), conf);
	mActiveFileSize = 0;
	mMarkerMgr = OmnNew AosMarkerMgr(rdata, this, mRawSE.getPtrNoLock());
}


AosBlobSE::~AosBlobSE()
{
	OmnDelete mActiveRawFileLock;
}


int
AosBlobSE::init(
		AosRundata	*rdata,
		const u64	ullLastAppliedOprID)
{
	int iRet = 0;
	if (!isFirstStart())
	{
		u64 ullSnapshotSavedOprID = 0;
		u64 ullFlushDirtyHeaderOprID = 0;
		bool rslt = false;
		bool bFinish = false;
		iRet = mMarkerMgr->getRecoverStartLocation(rdata, ullSnapshotSavedOprID, ullFlushDirtyHeaderOprID);
		if (0 != iRet)
		{
			OmnAlarm << " failed, iRet=" << iRet << enderr;
			return -1;
		}
		OmnScreen << "ullSnapshotSavedOprID:" << ullSnapshotSavedOprID
				<< " ullFlushDirtyHeaderOprID:" << ullFlushDirtyHeaderOprID
				<< " ullLastAppliedOprID:" << ullLastAppliedOprID << endl;
		AosBlobHeaderPtr pHeader;
		u64 ullSnapshotRecoverPointOprID =
				ullFlushDirtyHeaderOprID < ullLastAppliedOprID ? ullFlushDirtyHeaderOprID : ullLastAppliedOprID;
		for (u64 ullOprID = ullSnapshotSavedOprID + 1; ullOprID <= ullSnapshotRecoverPointOprID; ullOprID++)
		{
			iRet = generateHeaderForOprID(rdata, ullOprID, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "generateHeaderForOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
				return -2;
			}
			if (pHeader->getOprID() != ullOprID)
			{
				OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << " != ullOprID:" << ullOprID << enderr;
				return -3;
			}
			rslt = mSnapshotMgr->saveDoc(rdata, pHeader, bFinish);
			if (!rslt)
			{
				OmnAlarm << "mSnapshotMgr->saveDoc failed on docid:" << pHeader->getDocid()
						<< " oprid:" << ullOprID << " snap_id:" << pHeader->getSnapshotID() << enderr;
				return -4;
			}
		}
		for (u64 ullOprID = ullFlushDirtyHeaderOprID + 1; ullOprID <= ullLastAppliedOprID; ullOprID++)
		{
			iRet = generateHeaderForOprID(rdata, ullOprID, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "generateHeaderForOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
				return -5;
			}
			if (pHeader->getOprID() != ullOprID)
			{
				OmnAlarm << "pHeader->getOprID():" << pHeader->getOprID() << " != ullOprID:" << ullOprID << enderr;
				return -6;
			}
			iRet = updateHeader(rdata, pHeader);
			if (0 != iRet)
			{
				OmnAlarm << "updateHeader failed on docid:" << pHeader->getDocid()
						<< " oprid:" << ullOprID << " snap_id:" << pHeader->getSnapshotID() << enderr;
				return -7;
			}
		}
	}
	if (0 == getTimestampOfTheOldestExistingOldHeaderFileFromMetafile(rdata))
	{
		OmnAlarm << "0 == getTimestampOfTheOldestExistingOldHeaderFileFromMetafile(rdata)" << enderr;
		return -8;
	}
	return 0;
}


bool
AosBlobSE::saveDoc(
		const AosRundataPtr	&rdata,
		const u64			snap_id,
		const AosDfmDocPtr	&doc,
		const u64			opr_id,
		const u64			timestamp)
{
	if (OmnTime::getTimestamp() - mConfig.write_req_time_window > timestamp)
	{
		OmnAlarm << "timestamp:" << timestamp << " exceeds write_req_timeout_threshold:"
				<< mConfig.write_req_time_window << " right now:" << OmnTime::getTimestamp() << enderr;
		return false;
	}
	int iRet = 0;
	aos_assert_rr(mActiveRawFile.notNull(), rdata, false);
	u32 body_len = doc->getOrigLen();	//TODO:getOrigLen or getCompressedLen?
	if (blobTooBig(body_len))
	{
		AosLogError(rdata, true, AosErrmsgId::eDataTooBig)
		 	<< AosFieldName::eMaxSize << mConfig.max_doc_size
		 	<< AosFieldName::eActualLength << doc->getOrigLen() << enderr;
		return false;
	}
	if (isHugeBlob(body_len))
	{
		return saveHugeBlob(rdata, doc, snap_id);
	}

	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(doc->getDocid(), mConfig.header_custom_data_size);
	AosBuffPtr pCustomData = OmnNew AosBuff(mConfig.header_custom_data_size AosMemoryCheckerArgs);
	memset(pCustomData->data(), 0, mConfig.header_custom_data_size);
	pHeader->setDocid(doc->getDocid());
	pHeader->setBodyLen(body_len);
	pHeader->setTimestamp(timestamp);
	pHeader->setNextHeader(AosBlobHeader::unresolvedNextHeader);
	pHeader->setCustomDataSize(mConfig.header_custom_data_size);
	pHeader->setCustomData(pCustomData);
	u32 entry_size = sizeof(BodyEntryHead) + pHeader->getHeaderInACLAndHCLLen() + body_len + sizeof(u32);
	AosBuffPtr entry = OmnNew AosBuff(entry_size AosMemoryCheckerArgs);
	BodyEntryHead* pAddModifyEntryHead = (BodyEntryHead*)entry->data();
	pAddModifyEntryHead->ulLength = entry_size;
	pAddModifyEntryHead->ucType = eAddModify;
	pAddModifyEntryHead->ullOprID = opr_id;
	pAddModifyEntryHead->ullDocid = doc->getDocid();
	pAddModifyEntryHead->ullSnapID = snap_id;
	iRet = pHeader->serializeToACLandHCLBuff(entry->data() + sizeof(BodyEntryHead));
	if (0 != iRet)
	{
		OmnAlarm << "serializeToActiveLogBuff failed for docid:" << doc->getDocid()
				<< " snap_id:" << snap_id << " opr_id:" << opr_id << enderr;
		return false;
	}
	entry->setCrtIdx(sizeof(BodyEntryHead) + pHeader->getHeaderInACLAndHCLLen());
	entry->setBuff(doc->getBodyBuff());
	entry->setU32(entry_size);
	u64 ullOffset = 0;
	u64 ullRawfid = 0;
	iRet = appendDataToActiveLogFile(rdata.getPtrNoLock(), entry->data(), entry->dataLen(), ullRawfid, ullOffset);
	if (0 != iRet)
	{
		AosLogError(rdata, false, AosErrmsgId::eDataTooBig)
				<< AosFieldName::eDocid << doc->getDocid()
				<< AosFieldName::eReturnValue << iRet << enderr;
		return false;
	}
	iRet = mOprIDCache->saveOprID(rdata.getPtrNoLock(), opr_id, ullRawfid, ullOffset, entry->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->saveOprID failed, iRet=" << iRet << " opr id:" << opr_id
				<< " rawfid:" << ullRawfid << " offset:" << ullOffset << enderr;
		return false;
	}
	pHeader->setBodyRawfid(ullRawfid);
	pHeader->setBodyOffset(ullOffset);
//	bool bFinish = false;
//	bool rslt = mSnapshotMgr->saveDoc(rdata, snap_id, doc->getDocid(), pHeader, bFinish);	//TODO:needs timestamp?
//	if (!rslt)
//	{
//		OmnAlarm << "mSnapshotMgr->saveDoc failed on docid:" << doc->getDocid() << " snap_id:" << snap_id << enderr;
//		return false;
//	}
//	if (!bFinish)
	{
		iRet = mHeaderCache->updateHeader(rdata.getPtrNoLock(), pHeader);
		if (0 != iRet)
		{
			OmnAlarm << "mHeaderCache->updateHeader failed for docid:" << pHeader->getDocid() << enderr;
			return false;
		}
	}
	return true;
}


bool
AosBlobSE::modifyDoc(
		const AosRundataPtr	&rdata,
		const u64			snap_id,
		const AosDfmDocPtr	&doc,
		const u64			trans_id,
		const u64			timestamp)
{
	return true;
}


bool
AosBlobSE::deleteDoc(
		const AosRundataPtr	&rdata,
		const u64			snap_id,
		const AosDfmDocPtr	&doc,
		const u64			opr_id,
		const u64			timestamp)
{
	if (OmnTime::getTimestamp() - mConfig.write_req_time_window > timestamp)
	{
		OmnAlarm << "timestamp:" << timestamp << " exceeds write_req_timeout_threshold:"
				<< mConfig.write_req_time_window << " right now:" << OmnTime::getTimestamp() << enderr;
		return false;
	}
	char buff[sizeof(DeleteEntry)];
	DeleteEntry* pDeleteEntry = (DeleteEntry*)buff;
	pDeleteEntry->ulLength = sizeof(DeleteEntry);
	pDeleteEntry->ucType = eDelete;
	pDeleteEntry->ullSnapID = snap_id;
	pDeleteEntry->ullDocid = doc->getDocid();
	pDeleteEntry->ullOprID = opr_id;
	pDeleteEntry->ullTimestamp = timestamp;
	pDeleteEntry->ulLengthTail = pDeleteEntry->ulLength;
	u64 ullOffset = 0;
	u64 ullRawfid = 0;
	int iRet = appendDataToActiveLogFile(rdata.getPtrNoLock(), buff, pDeleteEntry->ulLength, ullRawfid, ullOffset);
	if (0 != iRet)
	{
		AosLogError(rdata, false, AosErrmsgId::eActiveLogFileAppendFailed)
			<< AosFieldName::eDocid << doc->getDocid()
			<< AosFieldName::eSnapshot << snap_id << enderr;
		return false;
	}
	iRet = mOprIDCache->saveOprID(rdata.getPtrNoLock(), opr_id, ullRawfid, ullOffset, sizeof(DeleteEntry));
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->saveOprID failed, iRet=" << iRet << " opr id:" << opr_id
				<< " rawfid:" << ullRawfid << " offset:" << ullOffset << enderr;
		return false;
	}
//	bool bFinish = false;
//	bool rslt = false;
//	rslt = mSnapshotMgr->deleteDoc(rdata, snap_id, doc->getDocid(), bFinish);
//	if (!rslt)
//	{
//		OmnAlarm << "mSnapshotMgr->deleteDoc failed on docid:" << doc->getDocid() << " snap_id:" << snap_id << enderr;
//		return false;
//	}
//	if (bFinish)
//	{
//		return true;
//	}
	iRet = mHeaderCache->deleteHeader(rdata.getPtrNoLock(), doc->getDocid(), timestamp);
	if (0 != iRet)
	{
		OmnAlarm << "delete header failed for docid:" << doc->getDocid() << enderr;
		return false;
	}
	return true;
}


AosDfmDocPtr
AosBlobSE::readDoc(
		const AosRundataPtr	&rdata,
		const u64			snap_id,
		const u64			docid,
		const u64			timestamp)
{
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(docid, mConfig.header_custom_data_size);
//	bool rslt = false;
//	rslt = mSnapshotMgr->readDoc(rdata, snap_id, docid, pHeader, timestamp);
//	if (!rslt || pHeader.isNull())
//	{
//		OmnAlarm << "read doc from snapshot mgr failed for docid:" << docid << " snpa_id:" << snap_id << enderr;
//		return NULL;
//	}
//	if (0 == pHeader->getBodyRawfid())
	{
		pHeader = mHeaderCache->readHeader(rdata.getPtrNoLock(), docid, timestamp);
		if (pHeader.isNull())
		{
			AosLogError(rdata, true, AosErrmsgId::eDataletDoesNotExist)
				<< AosFieldName::eDocid << docid
				<< AosFieldName::eSnapshot << snap_id << enderr;
			OmnAlarm << "read header failed for docid:" << docid << " timestamp:" << timestamp << enderr;
			return NULL;
		}
	}
	if (0 == pHeader->getBodyRawfid())	//doc is deleted
	{
		return NULL;
	}
	AosBuffPtr buff = OmnNew AosBuff((i64)mConfig.max_doc_size AosMemoryCheckerArgs);
	u64 ullEntryLen = sizeof(BodyEntryHead) + pHeader->getHeaderInACLAndHCLLen()
			+ pHeader->getBodyLen() + sizeof(u32);
	if (0 != (iRet = mBodyCache->readFile(rdata.getPtrNoLock(), pHeader->getBodyRawfid(),
			pHeader->getBodyOffset(), ullEntryLen, buff)))
	{
		// Failed on reading file.
		AosLogError(rdata, true, AosErrmsgId::eReadDocFailed)
			<< AosFieldName::eDocid << docid
			<< AosFieldName::eSnapshot << snap_id << enderr;
		OmnAlarm << "failed to read ActiveLog(body file) on rawfid:" << pHeader->getBodyRawfid()
				<< " offset:" << pHeader->getBodyOffset() << " for docid:" << docid << enderr;
		pHeader->dump();
		mHeaderCache->dumpHeadersByDocid(docid);
		return NULL;
	}
	buff->setCrtIdx(sizeof(BodyEntryHead) + pHeader->getHeaderInACLAndHCLLen());
	AosBuffPtr pBodyBuff = buff->getBuff(pHeader->getBodyLen(), true AosMemoryCheckerArgs);
	// Verify Checksum
	if (!verifyChecksum(rdata.getPtrNoLock(), buff))	//TODO:
	{
		return NULL;
	}
	BodyEntryHead* pAddModifyEntryHead = (BodyEntryHead*)buff->data();
	if (eAddModify != pAddModifyEntryHead->ucType)
	{
		AosLogError(rdata.getPtrNoLock(), false, AosErrmsgId::eDataInconsistent)
				<< AosFieldName::eDocid << docid
				<< AosFieldName::eSnapshot << snap_id << enderr;
		OmnAlarm << "pAddModifyEntryHead->ucType:" << pAddModifyEntryHead->ucType
				<< " != eAddModify:" << eAddModify << "\n body rawfid:" << pHeader->getBodyRawfid()
				<< " body offset:" << pHeader->getBodyOffset() << " docid:" << docid << enderr;
		pHeader->dump();
		mHeaderCache->dumpHeadersByDocid(docid);
		return NULL;
	}
	if (docid != pAddModifyEntryHead->ullDocid)
	{
		OmnAlarm << "trying to read doc with docid:" << docid << " but docid in the buff read is:"
				<< pAddModifyEntryHead->ullDocid << " body rawfid:" << pHeader->getBodyRawfid()
				<< " offset:" << pHeader->getBodyOffset() << enderr;
		pHeader->dump();
		mHeaderCache->dumpHeadersByDocid(docid);
		return NULL;
	}
	AosDfmDocPtr doc = mDoc->clone(docid);
	doc->setBodyBuff(pBodyBuff);
	doc->setOrigLen(pHeader->getBodyLen());
	return doc;
}


bool
AosBlobSE::blobTooBig(u64 blob_len)
{
	return false;
}



bool
AosBlobSE::isHugeBlob(u64 blob_len)
{
	return false;
}



int
AosBlobSE::config(
		AosRundata*			rdata,
		const AosXmlTagPtr	&conf)
{
	AosXmlTagPtr config = conf->getFirstChild("BlobSE", true);
	aos_assert_rr(config.notNull(), rdata, -1);
	mConfig.active_log_max_size = config->getAttrInt("active_log_max_size", eDftActiveLogSize);
	if (mConfig.active_log_max_size <= 0)
	{
		OmnAlarm << "active_log_max_size:" << mConfig.active_log_max_size
				<< " configuration error, set to default:" << eDftActiveLogSize << enderr;
	}
	mConfig.cubeid = config->getAttrU32("cubeid", 1);
	mConfig.siteid = config->getAttrU32("siteid", 1);
	mConfig.aseid = config->getAttrU64("aseid", 1);
	OmnScreen << "cube id:" << mConfig.cubeid << " site id:" << mConfig.siteid << " ase id:" << mConfig.aseid << endl;
	mConfig.max_doc_size = config->getAttrU32("max_doc_size", 100000);
	mConfig.header_custom_data_size = config->getAttrU32("header_custom_data_size", 12);
	mConfig.write_req_time_window = config->getAttrU64("write_req_timeout_threshold", 10000);
	mConfig.show_alarm_on_doc_not_exist = config->getAttrBool("show_alarm_on_doc_not_exist", false);
	mConfig.strict_sanity_check = config->getAttrBool("strict_sanity_check", false);
	return 0;
}


bool
AosBlobSE::verifyChecksum(
		AosRundata			*rdata,
		const AosBuffPtr	&buff)
{
	return true;
}


bool
AosBlobSE::isActiveFileTooBig() const
{
	return mActiveFileSize >= mConfig.active_log_max_size;
}


int
AosBlobSE::createNewActiveRawFileNoLock(const AosRundataPtr & rdata)
{
	u64 rawfid = getNextActiveRawfileRawfid(mActiveRawFile->getRawFileID());
	if (eActiveLogFileID_end < rawfid)
	{
		OmnAlarm << "rawfid:" << rawfid << " exceeds boundary:" << (u64)eActiveLogFileID_end << enderr;
		return -1;
	}
	int iRet = mActiveRawFile->close();
	if (0 != iRet)
	{
		OmnAlarm << "mActiveRawFile rawfid:" << rawfid - 1 << " close failed, iRet=" << iRet << enderr;
		return -2;
	}
	mActiveRawFile = mRawSE->createRawFile(rdata.getPtrNoLock(), mConfig.siteid, mConfig.cubeid,
			mConfig.aseid, rawfid, AosMediaType::eNVRAM, AosRawFile::eReadWriteCreate);
	aos_assert_rr(mActiveRawFile.notNull(), rdata, -3);
	mActiveFileSize = 0;
	if (0 != (iRet = saveActiveLogFileIDToMetafile(rdata.getPtrNoLock())))
	{
		OmnAlarm << "saveLastLogFileIDToMetafile failed on rawfid:" << rawfid << ", iRet=" << iRet << enderr;
		return -4;
	}
	return 0;
}


int
AosBlobSE::appendDataToActiveLogFile(
		AosRundata	*rdata,
		const char*	data,
		const u64	len,
		u64			&rawfid,
		u64			&offset)
{
	if (NULL == data)
	{
		return -1;
	}
	if (0 == len)
	{
		return -2;
	}
	mActiveRawFileLock->lock();
	mActiveRawFile->lock();
	int iRet = mActiveRawFile->append(rdata, data, len);
	if (0 != iRet)
	{
		OmnAlarm << "mActiveRawFile->append failed, iRet=" << iRet << " data len:" << len
				<< " rawfid:" << mActiveRawFile->getRawFileID() << enderr;
		mActiveRawFile->unlock();
		mActiveRawFileLock->unlock();
		return -3;
	}
	rawfid = mActiveRawFile->getRawFileID();
	offset = mActiveFileSize;
	mActiveFileSize += len;
	AosRawFilePtr pOldActiveFile = mActiveRawFile;
	if (isActiveFileTooBig())
	{
		iRet = createNewActiveRawFileNoLock(rdata);
		if (0 != iRet)
		{
			OmnAlarm << "createNewActiveRawFileNoLock failed" << enderr;
			pOldActiveFile->unlock();
			mActiveRawFileLock->unlock();
			return -4;
		}
	}
	pOldActiveFile->unlock();
	mActiveRawFileLock->unlock();
	return 0;
}


bool
AosBlobSE::saveHugeBlob(
		const AosRundataPtr	&rdata,
		const AosDfmDocPtr	&doc,
		const u64			snap_id)
{
	return true;
}


int
AosBlobSE::searchForSaveMark(
		AosRundata	*rdata,
		u64			&rawfid,
		u64			&offset,
		AosBuffPtr	&buff)
{
	/*
	 * this function searches for the first saved entry, header saved entry or header change log saved
	 * entry backwards, starting from the tail of last change log.
	 */
	return 0;
	aos_assert_rr(buff.notNull(), rdata, -1);
	AosRawFilePtr rawfile = mActiveRawFile;
	int iRet = 0;
	iRet = mRawSE->readRawFile(rdata, mConfig.siteid, mConfig.cubeid, mConfig.aseid,
			rawfile->getRawFileID(), 0, rawfile->getLength(), buff, AosRawFile::eModifyOnly);
	if (0 != iRet)
	{
		OmnAlarm << "read active log file failed, iRet=" << iRet << " rawfid:" << rawfile->getRawFileID()
				<< " read len:" << rawfile->getLength() << enderr;
		return -2;
	}
	char *pos = buff->data() + buff->dataLen();
	iRet = getPrevEntry(rdata, pos);
	if (0 > iRet)
	{
		OmnAlarm << "getPrevEntry failed, iRet=" << iRet << " pos:" << pos << enderr;
		return -3;
	}
	bool bSavedEntryFound = false;
	while(1)
	{
		if(isSavedEntry(pos))
		{
			bSavedEntryFound = true;
		}
		if(isStartToSaveEntry(pos) && bSavedEntryFound)
		{
			break;
		}
		iRet = getPrevEntry(rdata, pos);
		if (0 > iRet)
		{
			OmnAlarm << "getPrevEntry failed, iRet=" << iRet << " pos:" << pos << enderr;
			return -4;
		}
		else if (0 == iRet)	//reaches the beginning of a file
		{
			if (eActiveLogFileID_start >= rawfile->getRawFileID())	//no save mark exists
			{
				rawfid = 0;
				offset = 0;
				return 0;
			}
			rawfile = getPrevActiveLogFile(rdata, rawfile->getRawFileID());
			if (rawfile.isNull())
			{
				OmnAlarm << "getPrevActiveLogFile failed, rawfid:" << rawfile->getRawFileID() << enderr;
				return -5;
			}
			iRet = mRawSE->readRawFile(rdata, mConfig.siteid, mConfig.cubeid, mConfig.aseid,
					rawfile->getRawFileID(), 0, rawfile->getLength(), buff, AosRawFile::eReadWrite);
			if (0 != iRet)
			{
				OmnAlarm << "read active log file failed, iRet=" << iRet << " rawfid:" << rawfile->getRawFileID()
						<< " read len:" << rawfile->getLength() << enderr;
				return -6;
			}
		}
	}
	pos += *(u32*)pos;
	rawfid = rawfile->getRawFileID();
	offset = pos - buff->data();
	return 0;
}


int
AosBlobSE::getNextHeader(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&header,
		u64					&rawfid,
		u64					&offset,
		AosBuffPtr			&buff,
		u64					&snap_id)
{
	aos_assert_rr(buff.notNull(), rdata, -1);
	if (buff->dataLen() < (i64)offset)
	{
		OmnAlarm << "buff->dataLen():" << buff->dataLen() << " < (i64)offset:" << (i64)offset << enderr;
		return -2;
	}
	offset += *(u32*)(buff->data() + offset);	//length
	if (buff->dataLen() <= (i64)offset)
		//reached the end of this file, open the next file and read it
		//the i64 cast is due to some compromise of buff->dataLen()'s return type
	{
		AosRawFilePtr rawfile = getNextActiveLogFile(rdata, rawfid);
		offset = 0;
		bool rslt = buff->reset();
		if (!rslt)
		{
			OmnAlarm << "buff->reset() failed" << enderr;
			return -3;
		}
		if (rawfile.notNull())
		{
			rawfid++;
			int iRet = rawfile->read(rdata, 0, rawfile->getLength(), buff);
			if (0 != iRet)
			{
				OmnAlarm << "rawfile->read failed, iRet=" << iRet << " rawfid:" << rawfile->getRawFileID()
						<< " read len:" << rawfile->getLength() << enderr;
				return -4;
			}
		}
		else
		{
			rawfid = 0;
			return 0;
		}
	}
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	bodyEntry.setHeader(header);
	AosBuffPtr pTempBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	int iRet = 0;
	while(1)	//FIXME:should check the last applied oprid and just generates the headers of those not applied
	{
		if (!pTempBuff->setBuff(buff->data() + offset, buff->dataLen() - offset))
		{
			OmnAlarm << "pTempBuff->setBuff failed, offset:" << offset << " buff->dataLen():" << buff->dataLen() << enderr;
			return -5;
		}
		iRet = bodyEntry.unserializeFromBuff(pTempBuff);
		if (0 != iRet)
		{
			OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet << " rawfid:" << rawfid
					<< " offset:" << offset << enderr;
			return -6;
		}
		if (AosBodyEntry::eDelete == bodyEntry.getEntryType())
		{
			header->setBodyRawfid(0);
			header->setBodyOffset(0);
			return 1;
		}
		else if (AosBodyEntry::eSave == bodyEntry.getEntryType())
		{
			header->setBodyRawfid(rawfid);
			header->setBodyOffset(offset);
			header->setSnapshotID(bodyEntry.getSnapshotID());
			return 1;
		}
	}
	return 1;
}


int
AosBlobSE::getPrevEntry(
		AosRundata	*rdata,
		char*		&pos)
{
	if (0 == pos)
	{
		return 0;
	}
	aos_assert_rr(pos, rdata, -1);
	pos -= sizeof(u32);
	pos -= *(u32*)pos - sizeof(u32);
	aos_assert_rr(0 < pos, rdata, -2);
	return 1;
}


bool
AosBlobSE::openActiveLogFile(AosRundata	*rdata)
{
	// This function tries to open the active log file.
	// If this is a new BlobSE, there shall not be any log files.
	// In this case, no log file is returned. The function returns true.
	// If errors are encountered, it returns false. Otherwise,
	// it opens the previous log file.
	u64 last_fid = getLastLogFileIDFromMetafile(rdata);
	int iRet = 0;
	if (mRawSE->fileExists(rdata, mConfig.siteid, mConfig.cubeid,
			mConfig.aseid, last_fid))
	{
		mActiveRawFile = mRawSE->getRawFile(rdata, mConfig.siteid, mConfig.cubeid,
					mConfig.aseid, last_fid, AosRawFile::eReadWriteCreate);
		iRet = mActiveRawFile->open(rdata, AosRawFile::eReadWriteCreate);
		if (0 != iRet)
		{
			OmnAlarm << "mActiveRawFile->open failed, iRet=" << iRet << " open mode:" << AosRawFile::eReadWriteCreate << enderr;
			return false;
		}
		mActiveFileSize = mActiveRawFile->getLength();
		AosRawFilePtr pLogFileWithContent = mActiveRawFile;
		if (0 == mActiveFileSize && eActiveLogFileID_start > mActiveRawFile->getRawFileID())
		{
			pLogFileWithContent = getPrevActiveLogFile(rdata, mActiveRawFile->getRawFileID());
			if (pLogFileWithContent.isNull())
			{
				OmnAlarm << "getPrevActiveLogFile failed, rawfid:" << mActiveRawFile->getRawFileID() - 1 << enderr;
				return false;
			}
		}
		u64 ullFileLen = pLogFileWithContent->getLength();
		if (0 < ullFileLen)
		{
			AosBuffPtr pBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
			iRet = pLogFileWithContent->read(rdata, 0, ullFileLen, pBuff);
			if (0 != iRet)
			{
				OmnAlarm << "pLogFileWithContent->read failed, iRet=" << iRet << " rawfid:"
						<< pLogFileWithContent->getRawFileID() << " read len:" << ullFileLen << enderr;
				return false;
			}
			u32 ulEntryLen = *(u32*)(pBuff->data() + ullFileLen - sizeof(u32));
			AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
			AosBuffPtr pEntryBuff = OmnNew AosBuff(ulEntryLen AosMemoryCheckerArgs);
			pEntryBuff->setBuff(pBuff->data() + ullFileLen - ulEntryLen, ulEntryLen);
			iRet = bodyEntry.unserializeFromBuff(pEntryBuff);
			if (0 != iRet)
			{
				OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet
						<< " body rawfid:" << pLogFileWithContent->getRawFileID() << " offset:" << ullFileLen - ulEntryLen
						<< " entry len:" << ulEntryLen << enderr;
				return false;
			}
			mLastAppendedOprID = bodyEntry.getOprID();
		}
		else
		{
			mLastAppendedOprID = 0;
		}
	}
	else
	{
		mActiveRawFile = mRawSE->createRawFile(rdata, mConfig.siteid, mConfig.cubeid,
					mConfig.aseid, last_fid, AosMediaType::eDisk, AosRawFile::eReadWriteCreate);
		mLastAppendedOprID = 0;
		mActiveFileSize = 0;
	}
	if (mActiveRawFile.isNull())
	{
		OmnAlarm << "initialize mActiveRawFile failed, siteid:" << mConfig.siteid << " cubeid:" << mConfig.cubeid
						<< " aseid:" << mConfig.aseid << enderr;
		return false;
	}
	return true;
}


AosRawFilePtr
AosBlobSE::getPrevActiveLogFile(
		AosRundata	*rdata,
		const u64	rawfid)
{
	if (eActiveLogFileID_start >= rawfid)
	{
		return 0;
	}
	return mRawSE->getRawFile(rdata, mConfig.siteid, mConfig.cubeid,
				mConfig.aseid, rawfid - 1, AosRawFile::eReadWrite);
}


AosRawFilePtr
AosBlobSE::getNextActiveLogFile(
		AosRundata	*rdata,
		const u64	rawfid)
{
	if (eActiveLogFileID_start >= rawfid)
	{
		return 0;
	}
	return mRawSE->getRawFile(rdata, mConfig.siteid, mConfig.cubeid,
				mConfig.aseid, rawfid + 1, AosRawFile::eReadWrite);
}


bool
AosBlobSE::isSavedEntry(const char* buff)
{
	AosBodyEntry::BodyEntryGeneralHead* p = (AosBodyEntry::BodyEntryGeneralHead*)buff;
	return AosMarker::eHeaderSaved == p->ucType;
}


bool
AosBlobSE::isStartToSaveEntry(const char* buff)
{
	AosBodyEntry::BodyEntryGeneralHead* p = (AosBodyEntry::BodyEntryGeneralHead*)buff;
	return AosMarker::eHeaderStartToSave == p->ucType;
}


int
AosBlobSE::shutdown(AosRundata	*rdata)
{
	/*
	 * this function shuts down BlobSE
	 * 1.save rawfid of active log file to meta file
	 * 2.shut header cache
	 */
	OmnScreen << "BlobSE is shutting down." << endl;
	mLockRaw->lock();
	int iRet = saveActiveLogFileIDToMetafile(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "saveActiveLogFileIDToMetafile failed, iRet=" << iRet << enderr;
		mLockRaw->unlock();
		return iRet;
	}
	iRet = mOprIDCache->shutdown();
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->shutdown failed, iRet=" << iRet << enderr;
		return iRet;
	}
	iRet = mHeaderCache->shutdown(rdata, mActiveRawFile.getPtrNoLock());
	if (0 != iRet)
	{
		OmnAlarm << "mHeaderCache->shutdown failed, iRet=" << iRet << enderr;
		mLockRaw->unlock();
		return iRet;
	}
	iRet = mActiveRawFile->close();
	if (0 != iRet)
	{
		OmnAlarm << "mActiveRawFile->close() failed, iRet=" << iRet << enderr;
		mLockRaw->unlock();
		return iRet;
	}
	mLockRaw->unlock();
	OmnScreen << "BlobSE has shutdown." << endl;
	return 0;
}


u64
AosBlobSE::getLastLogFileIDFromMetafile(AosRundata	*rdata)
{
	/*
	 * this function opens the metafile that should contain the last log file id.
	 * If the metafile does not exist, we assume that this is a brand new blobse,
	 * so just create the metafile and return a default log file id, otherwise,
	 * return the file id read from the metafile.
	 */
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	aos_assert_rr(buff.notNull(), rdata, -1);
	int iRet = mRawSE->readRawFile(rdata, mConfig.siteid, mConfig.cubeid,
			mConfig.aseid, eMetaFile3ID, 0, sizeof(u64), buff, AosRawFile::eModifyOnly);
											//TODO:about eModifyOnly, please refer to the implementation
	if (0 != iRet)	//TODO:consider this as being brand new blobse
	{
		AosRawFilePtr ff = mRawSE->createRawFile(rdata, mConfig.siteid, mConfig.cubeid,
			mConfig.aseid, eMetaFile3ID, AosMediaType::eDisk, AosRawFile::eModifyOnly);
		aos_assert_rr(ff.notNull(), rdata, -2);
		buff->reset();
		buff->appendU64(eActiveLogFileID_start);
		iRet = ff->append(rdata, buff->data(), buff->dataLen());
		aos_assert_rr(0 == iRet, rdata, -3);
		iRet = ff->close();
		aos_assert_rr(0 == iRet, rdata, -4);
		mIsFirstStart = true;
		return eActiveLogFileID_start;	//does not need to close, rawfile is closed automatically
	}
	else	//TODO:when read a rawfile with mode eModifyOnly, the return value will be 0 even the file does not exist
	{
		u64 last_fid = buff->getU64(0);
		if (0 == last_fid)	//TODO:the meta file has just been created
		{
			buff->reset();
			buff->appendU64(eActiveLogFileID_start);
			iRet = mRawSE->overwriteRawFile(rdata, mConfig.siteid, mConfig.cubeid,
					mConfig.aseid, eMetaFile3ID, buff->data(), buff->dataLen());
			aos_assert_rr(0 == iRet, rdata, -5);
			mIsFirstStart = true;
			return eActiveLogFileID_start;
		}
		aos_assert_rr(last_fid > 0, rdata, -6);
		return last_fid;
	}
}


int
AosBlobSE::saveActiveLogFileIDToMetafile(AosRundata	*rdata)
{
	AosBuffPtr buff = OmnNew AosBuff(sizeof(u64) AosMemoryCheckerArgs);
	aos_assert_rr(buff.notNull(), rdata, -1);
	buff->reset();
	buff->appendU64(mActiveRawFile->getRawFileID());
	int iRet = 0;
	if (0 != (iRet = mRawSE->overwriteRawFile(rdata, mConfig.siteid, mConfig.cubeid,
			mConfig.aseid, eMetaFile3ID, buff->data(), buff->dataLen())))
	{
		OmnAlarm << "rewrite meta file failed, eMetaFileID:" << (int)eMetaFile3ID
				<< " iRet=" << iRet << enderr;
		return -2;
	}
	return 0;
}


int
AosBlobSE::addToDeleteLog(
			AosRundata				*rdata,
			const AosBlobHeaderPtr	&header)
{
	return 0;//mDelLog->writeDelLog(rdata, header);
}


int
AosBlobSE::createSnapshot(
		const AosRundataPtr	&rdata,
		const u64			ullSnapshotID)
{
	if (!mSnapshotMgr->createSnapshot(rdata, ullSnapshotID))
	{
		OmnAlarm << "mSnapshotMgr->createSnapshot failed, snap id:" << ullSnapshotID << enderr;
		return -1;
	}
	return 0;
}


int
AosBlobSE::commitSnapshot(
		const AosRundataPtr	&rdata,
		const u64			ullSnapshotID)
{
	if (!mSnapshotMgr->commitSnapshot(rdata, ullSnapshotID))
	{
		OmnAlarm << "mSnapshotMgr->commitSnapshot failed, snap id:" << ullSnapshotID << enderr;
		return -1;
	}
	return 0;
}


int
AosBlobSE::cancelSnapshot(
		const AosRundataPtr	&rdata,
		const u64			ullSnapshotID)
{
	if (!mSnapshotMgr->cancelSnapshot(rdata, ullSnapshotID))
	{
		OmnAlarm << "mSnapshotMgr->cancelSnapshot failed, snap id:" << ullSnapshotID << enderr;
		return -1;
	}
	return 0;
}


int
AosBlobSE::doCompationAsRaftLeader()
{
	return mHeaderCache->doCompationAsRaftLeader();
}


int
AosBlobSE::doCompactionAsRaftFollower()
{
	return mHeaderCache->doCompactionAsRaftFollower();
}


int
AosBlobSE::updateHeaders(
		AosRundata							*rdata,
		const std::vector<AosBlobHeaderPtr>	&vHeaders,
		const u64							timestamp)
{
	return mHeaderCache->updateHeaders(rdata, vHeaders, timestamp);
}


int
AosBlobSE::saveDoc(
		const AosRundataPtr	&rdata,
		const u32			ulTermID,
		const u64			ullOprID,
		AosBlobSEReqEntry*	pReq,
		AosBlobHeaderPtr&	pHeader)
{
	if (!pReq)
	{
		OmnAlarm << "pReq is NULL, oprid:" << ullOprID << enderr;
		return -1;
	}
	u64 ullTimestamp = OmnTime::getTimestamp();
	if ((ullTimestamp > pReq->getTimestamp() && (ullTimestamp - pReq->getTimestamp() > mConfig.write_req_time_window)) ||
			(pReq->getTimestamp() > ullTimestamp && (pReq->getTimestamp() - ullTimestamp > mConfig.write_req_time_window)))
	{
		OmnAlarm << "timestamp:" << pReq->getTimestamp() << " exceeds write_req_time_window:"
				<< mConfig.write_req_time_window << " right now:" << ullTimestamp << enderr;
		return -2;
	}
	int iRet = 0;
	u32 ulBodyLen = pReq->getBodyBuff()->dataLen();
	pHeader->setDocid(pReq->getDocid());
	pHeader->setBodyLen(ulBodyLen);
	pHeader->setTimestamp(pReq->getTimestamp());
	pHeader->setSnapshotID(pReq->getSnapshotID());
	pHeader->setNextHeader(AosBlobHeader::unresolvedNextHeader);
	pHeader->setCustomDataSize(mConfig.header_custom_data_size);
	pHeader->setCustomData(pReq->getHeaderCustomDataBuff());
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	bodyEntry.setEntryType(AosBodyEntry::eSave);
	bodyEntry.setHeader(pHeader);
	bodyEntry.setBodyBuff(pReq->getBodyBuff());
	u32 ulEntrySize = bodyEntry.calculateEntryLen();
	if (0 == ulEntrySize)
	{
		OmnAlarm << "bodyEntry.calculateEntryLen() failed for docid:" << pReq->getDocid()
				<< " snap_id:" << pReq->getSnapshotID() << " opr_id:" << ullOprID << enderr;
		return -3;
	}
	bodyEntry.setDocid(pReq->getDocid());
	bodyEntry.setTermID(ulTermID);
	bodyEntry.setOprID(ullOprID);
	bodyEntry.setSnapshotID(pReq->getSnapshotID());
	AosBuffPtr pEntryBuff = OmnNew AosBuff(ulEntrySize AosMemoryCheckerArgs);
	iRet = bodyEntry.serializeToBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.serializeToBuff failed, iRet=" << iRet << " docid:" << pReq->getDocid()
				<< " snap_id:" << pReq->getSnapshotID() << " opr_id:" << ullOprID << enderr;
		return -4;
	}
	u64 ullOffset = 0;
	u64 ullRawfid = 0;
	iRet = appendDataToActiveLogFile(rdata.getPtrNoLock(), pEntryBuff->data(), pEntryBuff->dataLen(), ullRawfid, ullOffset);
	if (0 != iRet)
	{
		OmnAlarm << "appendDataToActiveLogFile failed, iRet=" << iRet << " append len:" << pEntryBuff->dataLen() << enderr;
		return -4;
	}
	iRet = mOprIDCache->saveOprID(rdata.getPtrNoLock(), ullOprID, ullRawfid, ullOffset, pEntryBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->saveOprID failed, iRet=" << iRet << " opr id:" << ullOprID
				<< " rawfid:" << ullRawfid << " offset:" << ullOffset << enderr;
		return -5;
	}
	pHeader->setBodyRawfid(ullRawfid);
	pHeader->setBodyOffset(ullOffset);
	return 0;
}


int
AosBlobSE::deleteDoc(
		const AosRundataPtr	&rdata,
		const u32			ulTermID,
		const u64			ullOprID,
		AosBlobSEReqEntry*	pReq,
		AosBlobHeaderPtr&	pHeader)
{
	if (!pReq)
	{
		OmnAlarm << "pReq is NULL, oprid:" << ullOprID << enderr;
		return -1;
	}
	u64 ullTimestamp = OmnTime::getTimestamp();
	if ((ullTimestamp > pReq->getTimestamp() && (ullTimestamp - pReq->getTimestamp() > mConfig.write_req_time_window)) ||
			(pReq->getTimestamp() > ullTimestamp && (pReq->getTimestamp() - ullTimestamp > mConfig.write_req_time_window)))
	{
		OmnAlarm << "timestamp:" << pReq->getTimestamp() << " exceeds write_req_time_window:"
				<< mConfig.write_req_time_window << " right now:" << ullTimestamp << enderr;
		return -2;
	}
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	bodyEntry.setEntryType(AosBodyEntry::eDelete);
	AosBuffPtr pEntryBuff = OmnNew AosBuff(bodyEntry.calculateEntryLen() AosMemoryCheckerArgs);
	bodyEntry.setDocid(pReq->getDocid());
	bodyEntry.setTermID(ulTermID);
	bodyEntry.setOprID(ullOprID);
	bodyEntry.setSnapshotID(pReq->getSnapshotID());
	bodyEntry.setTimestamp(pReq->getTimestamp());
	int iRet = bodyEntry.serializeToBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.serializeToBuff failed, iRet=" << iRet << " docid:" << pReq->getDocid()
				<< " snap_id:" << pReq->getSnapshotID() << " opr_id:" << ullOprID
				<< " timestamp:" << pReq->getTimestamp() << enderr;
		return -3;
	}
	u64 ullOffset = 0;
	u64 ullRawfid = 0;
	iRet = appendDataToActiveLogFile(rdata.getPtrNoLock(), pEntryBuff->data(), pEntryBuff->dataLen(), ullRawfid, ullOffset);
	if (0 != iRet)
	{
		OmnAlarm << "appendDataToActiveLogFile failed, iRet=" << iRet << " append len:" << pEntryBuff->dataLen() << enderr;
		return -4;
	}
	iRet = mOprIDCache->saveOprID(rdata.getPtrNoLock(), ullOprID, ullRawfid, ullOffset, pEntryBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->saveOprID failed, iRet=" << iRet << " opr id:" << ullOprID
				<< " rawfid:" << ullRawfid << " offset:" << ullOffset << enderr;
		return -5;
	}
	pHeader->setBodyRawfid(0);
	pHeader->setBodyOffset(0);
	pHeader->setTimestamp(pReq->getTimestamp());
	pHeader->setSnapshotID(pReq->getSnapshotID());
	pHeader->setDocid(pReq->getDocid());
	pHeader->setNextHeader(AosBlobHeader::unresolvedNextHeader);
	pHeader->setCustomDataSize(mConfig.header_custom_data_size);
	pHeader->setCustomData(pReq->getHeaderCustomDataBuff());
	return 0;
}


int
AosBlobSE::readDoc(
		const AosRundataPtr	&rdata,
		AosBlobSEReqEntry*	pReq)
{
	if (!pReq)
	{
		OmnAlarm << "pReq is NULL." << enderr;
		return -1;
	}
	int iRet = 0;
	AosBlobHeaderPtr pHeader = OmnNew AosBlobHeader(pReq->getDocid(), mConfig.header_custom_data_size);
	bool rslt = false;
	rslt = mSnapshotMgr->readDoc(rdata, pReq->getSnapshotID(), pReq->getDocid(), pHeader, pReq->getTimestamp());
	if (!rslt)
	{
		OmnAlarm << "read doc from snapshot mgr failed for docid:" << pReq->getDocid()
				<< " snpa_id:" << pReq->getSnapshotID() << enderr;
		return -2;
	}
	if (pHeader.isNull())
	{
		pHeader = mHeaderCache->readHeader(rdata.getPtrNoLock(), pReq->getDocid(),
				pReq->getTimestamp(), mConfig.show_alarm_on_doc_not_exist);
		if (pHeader.isNull())
		{
			if (mConfig.show_alarm_on_doc_not_exist)
			{
				OmnAlarm << "read header failed for docid:" << pReq->getDocid()
						<< " timestamp:" << pReq->getTimestamp() << enderr;
			}
			else
			{
				OmnScreen << "aseid:" << mConfig.aseid << " read header failed for docid:" << pReq->getDocid()
						<< " timestamp:" << pReq->getTimestamp() << endl;
			}
			return -3;
		}
	}
	if (0 == pHeader->getBodyRawfid())	//doc is deleted
	{
		pReq->setReqTypeAndResult(AosBlobSEReqEntry::eDeleted);
		pReq->setBodyBuff(NULL);
		return 0;
	}
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	bodyEntry.setHeader(pHeader);
	bodyEntry.setEntryType(AosBodyEntry::eSave);
	bodyEntry.setBodyDataSize(pHeader->getBodyLen());
	u64 ullEntryLen = bodyEntry.calculateEntryLen();
	AosBuffPtr pEntryBuff = OmnNew AosBuff(ullEntryLen AosMemoryCheckerArgs);
	if (pHeader->getBodyRawfid() == mActiveRawFile->getRawFileID())
	{
		mActiveRawFile->lock();
		iRet = mActiveRawFile->read(rdata.getPtrNoLock(), pHeader->getBodyOffset(), ullEntryLen, pEntryBuff, true);
		mActiveRawFile->unlock();
	}
	else
	{
		iRet = mBodyCache->readFile(rdata.getPtrNoLock(), pHeader->getBodyRawfid(),
						pHeader->getBodyOffset(), ullEntryLen, pEntryBuff);
	}
	if (0 != iRet)
	{
		OmnAlarm << "failed to read ActiveLog(body file) on rawfid:" << pHeader->getBodyRawfid()
				<< " offset:" << pHeader->getBodyOffset() << " for docid:" << pReq->getDocid() << enderr;
		mHeaderCache->dumpHeadersByDocid(pReq->getDocid());
		return -4;
	}
	iRet = bodyEntry.unserializeFromBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet
				<< " body rawfid:" << pHeader->getBodyRawfid() << " offset:" << pHeader->getBodyOffset()
				<< " entry len:" << ullEntryLen << enderr;
		mHeaderCache->dumpHeadersByDocid(pReq->getDocid());
		return -5;
	}
	AosBuffPtr pBodyBuff = bodyEntry.getBodyBuff();
	if (AosBodyEntry::eSave != bodyEntry.getEntryType())
	{
		OmnAlarm << "bodyEntry.getEntryType():" << bodyEntry.getEntryType()
				<< " != AosBodyEntry::eSave:" << AosBodyEntry::eSave << "\n body rawfid:" << pHeader->getBodyRawfid()
				<< " body offset:" << pHeader->getBodyOffset() << " docid:" << pReq->getDocid()
				<< " req timestamp:" << pReq->getTimestamp() << enderr;
		mHeaderCache->dumpHeadersByDocid(pReq->getDocid());
		return -6;
	}
	if (pReq->getDocid() != bodyEntry.getDocid())
	{
		OmnAlarm << "trying to read doc with docid:" << pReq->getDocid() << " but docid in the buff read is:"
				<< bodyEntry.getDocid() << " body rawfid:" << pHeader->getBodyRawfid()
				<< " offset:" << pHeader->getBodyOffset() << " req timestamp:" << pReq->getTimestamp() << enderr;
		mHeaderCache->dumpHeadersByDocid(pReq->getDocid());
		return -7;
	}
	pReq->setReqTypeAndResult(AosBlobSEReqEntry::eSave);
	pReq->setBodyBuff(pBodyBuff);
	pReq->setHeaderCustomDataBuff(pHeader->getCustomData());
	return 0;
}


int
AosBlobSE::updateHeader(
		AosRundata			*rdata,
		AosBlobHeaderPtr&	pHeader)
{
	bool bFinish = false;
	bool rslt = mSnapshotMgr->saveDoc(rdata, pHeader, bFinish);
	if (!rslt)
	{
		OmnAlarm << "mSnapshotMgr->saveDoc failed on docid:" << pHeader->getDocid()
				<< " snap_id:" << pHeader->getSnapshotID() << enderr;
		return -1;
	}
	if (bFinish)
	{
		return 0;
	}
	return mHeaderCache->updateHeader(rdata, pHeader);;
}


int
AosBlobSE::getEntryByOprID(
		const AosRundataPtr	&rdata,
		const u64			ullOprID,
		u32					&ulTermID,
		AosBuffPtr			&pBuff)
{
	if (pBuff.isNull())
	{
		OmnAlarm << "pBuff.isNull()" << enderr;
		return -1;
	}
	u64 ullRawfid = 0;
	u64 ullOffset = 0;
	u64 ullLen = 0;
	int iRet = mOprIDCache->getRawfidAndOffsetAndLenByOprID(ullOprID, ullRawfid, ullOffset, ullLen);
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->getRawfidAndOffsetByOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
		return -2;
	}
	if (0 == ullLen)	//no-op entry
	{
		AosBlobSEReqEntry reqEntry(mConfig.header_custom_data_size);
		reqEntry.setOprID(ullOprID);
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eNotExist);
		iRet = reqEntry.serializeToBuff(pBuff);
		if (0 != iRet)
		{
			OmnAlarm << "reqEntry.serializeToBuff failed, iRet=" << iRet << enderr;
			return -3;
		}
		return 0;
	}
	AosBuffPtr pEntryBuff = OmnNew AosBuff(ullLen AosMemoryCheckerArgs);
	iRet = mBodyCache->readFile(rdata.getPtrNoLock(), ullRawfid, ullOffset, ullLen, pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mBodyCache->readFile failed, iRet=" << iRet << " rawfid:" << ullRawfid
				<< " offset:" << ullOffset << " read len:" << ullLen << enderr;
		return -4;
	}
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	iRet = bodyEntry.unserializeFromBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet
				<< " pEntryBuff->dataLen():" << pEntryBuff->dataLen() << enderr;
		return -5;
	}
	if (ullOprID != bodyEntry.getOprID())
	{
		OmnAlarm << "ullOprID != bodyEntry.getOprID()" << enderr;
		return -6;
	}
	ulTermID = bodyEntry.getTermID();
	AosBlobSEReqEntry reqEntry(mConfig.header_custom_data_size);
	reqEntry.setOprID(bodyEntry.getOprID());
	switch (bodyEntry.getEntryType())
	{
	case AosBodyEntry::eSave:
		reqEntry.setDocid(bodyEntry.getDocid());
		reqEntry.setHeaderCustomDataBuff(bodyEntry.getHeader()->getCustomData());
		reqEntry.setTimestamp(bodyEntry.getTimestamp());
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eSave);
		reqEntry.setBodyBuff(bodyEntry.getBodyBuff());
		break;
	case AosBodyEntry::eDelete:
		reqEntry.setDocid(bodyEntry.getDocid());
		reqEntry.setTimestamp(bodyEntry.getTimestamp());
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eDelete);
		break;
	case AosBodyEntry::eCreateSnapshot:
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eCreateSnapshot);
		break;
	case AosBodyEntry::eCommitSnapshot:
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eCommitSnapshot);
		break;
	case AosBodyEntry::eCancelSnapshot:
		reqEntry.setReqTypeAndResult(AosBlobSEReqEntry::eCancelSnapshot);
		break;
	default:
		OmnAlarm << "unsupported entry type:" << bodyEntry.getEntryType() << enderr;
		return -99;
		break;
	}
	iRet = reqEntry.serializeToBuff(pBuff);
	if (0 != iRet)
	{
		OmnAlarm << "reqEntry.serializeToBuff failed, iRet=" << iRet << enderr;
		return -7;
	}
	return 0;
}


int
AosBlobSE::getBodyFileBoundaryByTimestamp(
		AosRundata	*rdata,
		const u64	ullTimestamp,
		u64			&ullBoundaryRawfid,
		u64			&ullBoundaryOffset)
{
	return 0;
}


u64
AosBlobSE::getTimestampOfTheOldestExistingOldHeaderFileFromMetafile(AosRundata	*rdata)	//not precisely
{
	/*
	 * since we only need to know in which epoch day that the oldest old header file dwells,
	 * the timestamp is not precise, we just use the first start timestamp
	 */
	AosBuffPtr buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	if (buff.isNull())
	{
		OmnAlarm << "failed to create AosBuff obj" << enderr;
		return 0;
	}
	int iRet = 0;
	u64 ullTimestamp = OmnTime::getTimestamp();
	if (0 == ullTimestamp)
	{
		OmnAlarm << "OmnTime::getTimestamp() returns 0, did you start OmnTimeDriver?\n"
				<< "please add OmnTimeDriver::getSelf()->start() to your main function."<< enderr;
		return 0;
	}
	if (!mRawSE->fileExists(rdata, mConfig.siteid, mConfig.cubeid, mConfig.aseid, eMetaFile2ID))
			//TODO:consider this as being brand new blobse
	{
		AosRawFilePtr ff = mRawSE->createRawFile(rdata, mConfig.siteid, mConfig.cubeid,
				mConfig.aseid, eMetaFile2ID, AosMediaType::eDisk, AosRawFile::eModifyOnly);
		if (ff.isNull())
		{
			OmnAlarm << "mRawSE->createRawFile failed, siteid:" << mConfig.siteid << " cubeid:" << mConfig.cubeid
					<< " aseid:" << mConfig.aseid << " rawfid:" << eMetaFile2ID << enderr;
			return 0;
		}
		buff->reset();
		buff->appendU64(ullTimestamp);
		iRet = ff->append(rdata, buff->data(), buff->dataLen());
		if (0 != iRet)
		{
			OmnAlarm << "ff->append failed, iRet=" << iRet << " append len:" << buff->dataLen() << enderr;
			return 0;
		}
		iRet = ff->close();
		if (0 != iRet)
		{
			OmnAlarm << "file close failed, iRet=" << iRet << " rawfid:" << eMetaFile2ID<< enderr;
			return 0;
		}
		return ullTimestamp;	//does not need to close, rawfile is closed automatically
	}
	else
	{
		iRet = mRawSE->readRawFile(rdata, mConfig.siteid, mConfig.cubeid, mConfig.aseid,
				eMetaFile2ID, 0, sizeof(u64), buff, AosRawFile::eReadWrite);
		if (0 != iRet)
		{
			OmnAlarm << "mRawSE->readRawFile failed, iRet=" << iRet << " siteid:" << mConfig.siteid
						<< " cubeid:" << mConfig.cubeid << " aseid:" << mConfig.aseid << " rawfid:" << eMetaFile2ID
						<< " read len:" << sizeof(u64) << enderr;
			return 0;
		}
		u64 ullTempTimestamp = buff->getU64(0);
		if (0 == ullTempTimestamp)
		{
//			buff->reset();
//			buff->appendU64(ullTimestamp);
//			iRet = mRawSE->overwriteRawFile(mRdata, mSiteID, mCubeID,
//					mAseID, eMetaFile2ID, buff->data(), buff->dataLen());
//			if (0 != iRet)
//			{
//				OmnAlarm << "mRawSE->overwriteRawFile failed, iRet=" << iRet << " siteid:" << mSiteID
//						<< " cubeid:" << mCubeID << " aseid:" << mAseID << " rawfid:" << eMetaFile2ID
//						<< " overwrite len:" << buff->dataLen() << enderr;
//				return 0;
//			}
//			return ullTimestamp;
			OmnAlarm << "0 == ullTempTimestamp" << enderr;
			return 0;
		}
		return ullTempTimestamp;
	}
}


int
AosBlobSE::removeEntry(
		AosRundata			*rdata,
		AosBlobHeaderPtr	&pHeader)
{
	int iRet = 0;
	mActiveRawFileLock->lock();
	if (pHeader->getBodyRawfid() == mActiveRawFile->getRawFileID())
	{
		if (mActiveRawFile->getLength() < pHeader->getBodyOffset())
		{
			OmnAlarm << "mActiveRawFile->getLength():" << mActiveRawFile->getLength()
					<< " < pHeader->getBodyOffset():" << pHeader->getBodyOffset() << enderr;
			mActiveRawFileLock->unlock();
			return -1;
		}
		iRet = mActiveRawFile->truncate(rdata, pHeader->getBodyOffset());
		if (0 != iRet)
		{
			OmnAlarm << "mActiveRawFile->truncate failed, iRet=" << iRet << " original len:" << mActiveFileSize
					<< " trying to truncate to len:" << pHeader->getBodyOffset() << enderr;
			mActiveRawFileLock->unlock();
			return -2;
		}
		mActiveFileSize = pHeader->getBodyOffset();
		mActiveRawFileLock->unlock();
		return 0;
	}
	if (0 != mActiveFileSize)
	{
		OmnAlarm << "it should not remove entry that is not the last one! 0 != mActiveFileSize:" << mActiveFileSize
				<< " mActiveRawFile->getLength():" << mActiveRawFile->getLength() << enderr;
		mActiveRawFileLock->unlock();
		return -3;
	}
	iRet = mActiveRawFile->del(rdata);
	if (0 != iRet)
	{
		OmnAlarm << "mActiveRawFile->del failed, iRet=" << iRet << enderr;
		mActiveRawFileLock->unlock();
		return -4;
	}
	u64 ullPrevActiveRawfileRawfid = getPrevActiveRawfileRawfid(mActiveRawFile->getRawFileID());
	mActiveRawFile = getPrevActiveLogFile(rdata, ullPrevActiveRawfileRawfid);
	if (mActiveRawFile.isNull())
	{
		OmnAlarm << "getPrevActiveLogFile failed, rawfid:" << ullPrevActiveRawfileRawfid << enderr;
		mActiveRawFileLock->unlock();
		return -5;
	}
	iRet = mActiveRawFile->truncate(rdata, pHeader->getBodyOffset());
	if (0 != iRet)
	{
		OmnAlarm << "mActiveRawFile->truncate failed, iRet=" << iRet << " original len:" << mActiveFileSize
				<< " trying to truncate to len:" << pHeader->getBodyOffset() << enderr;
		mActiveRawFileLock->unlock();
		return -2;
	}
	mActiveFileSize = mActiveRawFile->getLength();
	mActiveRawFileLock->unlock();
	return 0;
}


int
AosBlobSE::saveMarkerToMarkerFile(
		AosRundata	*rdata,
		AosMarker	&marker)
{
	LockGuard(g, mActiveRawFileLock);
	marker.setRawfid(mActiveRawFile->getRawFileID());
	marker.setOffset(mActiveFileSize);
	marker.setOprID(mLastAppendedOprID);
	return mMarkerMgr->saveMarkerToMarkerFile(rdata, marker);
}


int
AosBlobSE::saveSnapshotOprEntry(
		AosRundata						*rdata,
		const u32						ulTermID,
		const u64						ullOprID,
		const u64						ullSnapshotID,
		AosBlobHeader::OperationType	eOprType)
{
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	bodyEntry.setEntryType(AosBodyEntry::eDelete);
	AosBuffPtr pEntryBuff = OmnNew AosBuff(bodyEntry.calculateEntryLen() AosMemoryCheckerArgs);
	bodyEntry.setOprID(ullOprID);
	bodyEntry.setSnapshotID(ullSnapshotID);
	int iRet = bodyEntry.serializeToBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.serializeToBuff failed, iRet=" << iRet << enderr;
		return -1;
	}
	u64 ullOffset = 0;
	u64 ullRawfid = 0;
	iRet = appendDataToActiveLogFile(rdata, pEntryBuff->data(), pEntryBuff->dataLen(), ullRawfid, ullOffset);
	if (0 != iRet)
	{
		OmnAlarm << "appendDataToActiveLogFile failed, iRet=" << iRet << " append len:" << pEntryBuff->dataLen() << enderr;
		return -2;
	}
	iRet = mOprIDCache->saveOprID(rdata, ullOprID, ullRawfid, ullOffset, pEntryBuff->dataLen());
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->saveOprID failed, iRet=" << iRet << " opr id:" << ullOprID
				<< " rawfid:" << ullRawfid << " offset:" << ullOffset << enderr;
		return -3;
	}
	return 0;
}


int
AosBlobSE::generateHeaderForOprID(
		AosRundata			*rdata,
		const u64			ullOprID,
		AosBlobHeaderPtr	&pHeader)
{
	u64 ullRawfid = 0;
	u64 ullOffset = 0;
	u64 ullLen = 0;
	int iRet = mOprIDCache->getRawfidAndOffsetAndLenByOprID(ullOprID, ullRawfid, ullOffset, ullLen);
	if (0 != iRet)
	{
		OmnAlarm << "mOprIDCache->getRawfidAndOffsetByOprID failed, iRet=" << iRet << " oprid:" << ullOprID << enderr;
		return -1;
	}
	AosBuffPtr pEntryBuff = OmnNew AosBuff(ullLen AosMemoryCheckerArgs);
	iRet = mBodyCache->readFile(rdata, ullRawfid, ullOffset, ullLen, pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "mBodyCache->readFile failed, iRet=" << iRet << " rawfid:" << ullRawfid
				<< " offset:" << ullOffset << " read len:" << ullLen << enderr;
		return -2;
	}
	AosBodyEntry bodyEntry = AosBodyEntry(mConfig.header_custom_data_size);
	iRet = bodyEntry.unserializeFromBuff(pEntryBuff);
	if (0 != iRet)
	{
		OmnAlarm << "bodyEntry.unserializeFromBuff failed, iRet=" << iRet
				<< " pEntryBuff->dataLen():" << pEntryBuff->dataLen() << enderr;
		return -3;
	}
	if (ullOprID != bodyEntry.getOprID())
	{
		OmnAlarm << "ullOprID != bodyEntry.getOprID()" << enderr;
		return -4;
	}
	pHeader = bodyEntry.getHeader();
	if (pHeader.isNull())
	{
		OmnAlarm << "bodyEntry.getHeader() failed, oprid:" << ullOprID << " body rawfid:" << ullRawfid
				<< " body offset:" << ullOffset << enderr;
		return -5;
	}
	return 0;
}


u64 AosBlobSE::getNextActiveRawfileRawfid(const u64 rawfid)
{
	return rawfid + 1;
}


u64 AosBlobSE::getPrevActiveRawfileRawfid(const u64 rawfid)
{
	return rawfid - 1;
}


OmnString
AosBlobSE::getBaseDir()
{
	OmnString dir = mRawSE->getBaseDir();
	dir << "/" << mConfig.cubeid << "/" << mConfig.aseid << "/";
	std::string s = "mkdir -p ";
	s += dir.data();
	system(s.c_str());
	return dir;
}
