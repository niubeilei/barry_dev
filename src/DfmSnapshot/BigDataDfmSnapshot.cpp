////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/02/26 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "DfmSnapshot/BigDataDfmSnapshot.h"

#include "DfmSnapshot/DfmSnapshotMgr.h"
#include "DfmUtil/DfmDoc.h"
#include "DfmUtil/DfmDocHeader.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEUtil/SeConfig.h"


AosBigDataDfmSnapshot::AosBigDataDfmSnapshot(
		const AosDfmSnapshotMgrPtr & snapshot_mgr,
		const u64 snap_id,
		const AosDfmDocType::E doc_type)
:
mLock(OmnNew OmnMutex()),
mSnapshotMgr(snapshot_mgr),
mSnapId(snap_id),
mDocType(doc_type),
mStatus(AosSnapshotSts::eActive),
mHeaderDataLen(0),
mEntrySize(0),
mFileIsProntToEnd(false),
mIdMapInited(false),
mShowLog(false)
{
}



AosBigDataDfmSnapshot::~AosBigDataDfmSnapshot()
{
}


AosDfmSnapshotObjPtr
AosBigDataDfmSnapshot::createSnapshotStatic(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotMgrPtr &snapshot_mgr, 
		const u64 snap_id,
		const AosDfmDocType::E doc_type)
{
	aos_assert_r(snapshot_mgr && snap_id, 0);
	
	AosBigDataDfmSnapshotPtr snapshot = OmnNew AosBigDataDfmSnapshot(
			snapshot_mgr, snap_id, doc_type);
	
	bool rslt = snapshot->createLogFile(rdata);
	aos_assert_r(rslt, 0);
	
	rslt = snapshot->init(rdata);
	aos_assert_r(rslt, 0);

	return snapshot;
}


bool
AosBigDataDfmSnapshot::init(const AosRundataPtr &rdata)
{
	//mTmpDfmDoc = AosDfmDoc::cloneDoc(AosDfmDocType::eIIL);
	mTmpDfmDoc = AosDfmDoc::cloneDoc(mDocType);
	aos_assert_r(mTmpDfmDoc, false);

	mHeaderDataLen = mTmpDfmDoc->getHeaderSize();
	mEntrySize = sizeof(u32) + sizeof(u64) + mHeaderDataLen;
	return true;
}


bool
AosBigDataDfmSnapshot::createLogFile(const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	OmnString prefix = mSnapshotMgr->getFileKey();
	aos_assert_r(prefix != "", false);
	prefix << "_" << mSnapId;
	mLogFile = vfsMgr->createRlbFile(rdata.getPtr(), mSnapshotMgr->getVirtualId(), prefix,
			eAosMaxFileSize, AosRlbFileType::eNormal, true);
	aos_assert_r(mLogFile, false);
	
	// save this snap_log file.
	//bool rslt = saveToFile(rdata, snap_id, mLogFile->getFileId());
	//bool rslt = mSnapshotMgr->triggerSaveToFile(rdata);
	//aos_assert_r(rslt, 0);
	return true;
}


AosDfmSnapshotObjPtr
AosBigDataDfmSnapshot::createSnapshotStatic(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotMgrPtr &snapshot_mgr, 
		const AosBuffPtr &buff,
		const AosDfmDocType::E doc_type)
{
	aos_assert_r(snapshot_mgr && buff, 0);
	
	AosBigDataDfmSnapshotPtr snapshot = OmnNew AosBigDataDfmSnapshot(
			snapshot_mgr, 0, doc_type);

	bool rslt = snapshot->serializeFrom(rdata, buff);
	aos_assert_r(rslt, 0);

	rslt = snapshot->init(rdata);
	aos_assert_r(rslt, 0);
	return snapshot;
}


bool
AosBigDataDfmSnapshot::serializeFrom(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	// this type has readed.
	mSnapId = buff->getU32(0);
	aos_assert_r(mSnapId, false);
	
	u64 log_file_id = buff->getU64(0);

	AosDfmDocType::E doc_type = (AosDfmDocType::E)buff->getU32(0);
	aos_assert_r(doc_type == mDocType, false);
	
	bool rslt = openLogFile(rdata, log_file_id);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBigDataDfmSnapshot::serializeTo(const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);

	buff->setU32(AosDfmSnapshotType::eBigData);

	buff->setU32(mSnapId);
	buff->setU64(mLogFile->getFileId());
	buff->setU32(mDocType);	
	return true;
}


bool
AosBigDataDfmSnapshot::openLogFile(
		const AosRundataPtr &rdata,
		const u64 log_file_id)
{
	aos_assert_r(log_file_id, false);
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	mLogFile = vfsMgr->openRlbFile(log_file_id, 
			AosRlbFileType::eNormal, rdata.getPtr());
	aos_assert_r(mLogFile, false);
	
	return true;
}
	
bool
AosBigDataDfmSnapshot::setStatus(const AosSnapshotSts::E sts)
{
	mLock->lock();
	mStatus = sts;
	mLock->unlock();
	return true;
}

bool
AosBigDataDfmSnapshot::commitFinish(const AosRundataPtr &rdata)
{
	mLock->lock();
	mStatus = AosSnapshotSts::eCommitted;
	removeLogFile(rdata);
	mLock->unlock();
	
	return true;
}

bool
AosBigDataDfmSnapshot::rollbackFinish(const AosRundataPtr &rdata)
{
	mLock->lock();
	mStatus = AosSnapshotSts::eRollbacked;
	removeLogFile(rdata);
	mLock->unlock();
	
	return true;
}

bool
AosBigDataDfmSnapshot::mergeFinish(const AosRundataPtr &rdata)
{
	mLock->lock();
	mStatus = AosSnapshotSts::eMerged;
	removeLogFile(rdata);
	mLock->unlock();
	
	return true;
}



bool
AosBigDataDfmSnapshot::removeLogFile(const AosRundataPtr &rdata)
{
	bool rslt = mLogFile->removeFile(rdata.getPtr());		// this remove file will generate fmt.
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosBigDataDfmSnapshot::initIdMapPriv(const AosRundataPtr &rdata)
{
	mIdMapInited = true;
	
	bool rslt, finished;
	AosDfmDocHeaderPtr header;
	AosBuffPtr file_data = OmnNew AosBuff(eAosSizePerRead AosMemoryCheckerArgs);
	u64 offset = eEntryStart;
	while(1)
	{
		rslt = readBuffFromFile(rdata, offset,
			file_data, finished);
		aos_assert_r(rslt, false);
		if(finished)	break;	
	
		while(1)
		{
			offset += mEntrySize; 
			
			rslt = readEntryFromBuff(rdata, file_data, header, finished); 
			aos_assert_r(rslt, false);
			if(finished)	break;
			
			if(!header)	continue;		// maybe this entry has been deleted.
			rslt = addInfoToMap(header->getDocid(), offset);
			aos_assert_r(rslt, false);
		}
	}
		
	return true;
}


u64
AosBigDataDfmSnapshot::getFileOffsetFromMap(const AosRundataPtr &rdata, const u64 docid)
{
	mLock->lock();
	if(!mIdMapInited)	initIdMapPriv(rdata);

	u64 offset = 0;
	map<u64, u64>::iterator itr = mIdToOffsetMap.find(docid);
	if(itr != mIdToOffsetMap.end())
	{
		offset = itr->second;
	}
	mLock->unlock();
	return offset;
}


bool
AosBigDataDfmSnapshot::addInfoToMap(const u64 docid, const u64 offset)
{
	map<u64, u64>::iterator itr = mIdToOffsetMap.find(docid);
	aos_assert_r(itr == mIdToOffsetMap.end(), false);

	mIdToOffsetMap.insert(make_pair(docid, offset));
	return true;
}


bool
AosBigDataDfmSnapshot::readEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		bool &exist,
		AosDfmDocHeaderPtr &header)
{
	u64 file_off = getFileOffsetFromMap(rdata, docid);
	if(file_off == 0)
	{
		header = mTmpDfmDoc->cloneDfmHeader(docid);
		exist = false;
		return true;
	}

	AosBuffPtr entry_buff = OmnNew AosBuff(mEntrySize AosMemoryCheckerArgs);
	
	int remain = mLogFile->getLength() - file_off;
	aos_assert_r(remain > 0 && (u32)remain >= mEntrySize, false);

	bool rslt = mLogFile->readToBuff(file_off, mEntrySize, entry_buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	entry_buff->setDataLen(mEntrySize);
	mFileIsProntToEnd = false;

	bool finished = false;
	rslt = readEntryFromBuff(rdata, entry_buff, header, finished);
	aos_assert_r(rslt, false);
	if(!header)
	{
		// this means this entry some times exist. but removed yet.
		exist = false;
		return true;
	}
	
	exist = true;
	aos_assert_r(header->getDocid() == docid, false);
	return true;
}

bool
AosBigDataDfmSnapshot::addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &header,
		const bool is_merge)
{
	// this add entry. maybe header is empty. user is delete this header.
	if(!is_merge) aos_assert_r(isActive(), false);

	aos_assert_r(header, false);
	
	u64 file_off = getFileOffsetFromMap(rdata, header->getDocid());
	if(file_off != 0) return modifyEntry(rdata, header, file_off);
	
	return appendEntry(rdata, header);
}


bool
AosBigDataDfmSnapshot::appendEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &header)
{
	aos_assert_r(header, false);
	bool rslt;

	mLock->lock();
	
	u64 file_off = mLogFile->getLength();
	if(file_off < eEntryStart)	file_off = eEntryStart;

	//if(!mFileIsProntToEnd)
	//{
	//	rslt = mLogFile->seek(file_off);
	//	aos_assert_r(rslt, false);
	//}
	
	rslt = appendToLogFile(rdata, file_off, header);
	aos_assert_r(rslt, false);
	
	//mFileIsProntToEnd = true;

	rslt = addInfoToMap(header->getDocid(), file_off);
	mLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBigDataDfmSnapshot::modifyEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocHeaderPtr &header,
		const u64 file_off)
{
	aos_assert_r(header && file_off, true);
	aos_assert_r(file_off + mEntrySize <= mLogFile->getLength(), false);
	
	//bool rslt = mLogFile->seek(file_off);
	//aos_assert_r(rslt, false);
	
	bool rslt = appendToLogFile(rdata, file_off, header);
	aos_assert_r(rslt, false);
	
	mFileIsProntToEnd = false;
	
	return true;
}


bool
AosBigDataDfmSnapshot::appendToLogFile(
		const AosRundataPtr &rdata,
		const u64 file_off,
		const AosDfmDocHeaderPtr &header)
{
	aos_assert_r(header, false);
	AosBuffPtr header_data = header->getHeaderBuff();
	aos_assert_r(header_data && header_data->dataLen() <= mHeaderDataLen, false);

	u32 entry_size = sizeof(u32) + sizeof(u64) + mHeaderDataLen;
	AosBuffPtr entry_buff = OmnNew AosBuff(entry_size AosMemoryCheckerArgs);
	memset(entry_buff->data(), 0, entry_size);
	entry_buff->setU32(eValidEntry);
	entry_buff->setU64(header->getDocid());
	entry_buff->setBuff(header_data);

	//bool rslt = mLogFile->append(rdata, file_off,
	//		entry_buff->data(), entry_size, true);
	bool rslt = mLogFile->put(file_off,
			entry_buff->data(), entry_size, true, rdata.getPtr());
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBigDataDfmSnapshot::removeEntry(
		const AosRundataPtr &rdata,
		const u64 docid)
{
	aos_assert_r(isActive(), false);

	u64 file_off = getFileOffsetFromMap(rdata, docid);
	if(file_off == 0) return true;

	bool rslt = mLogFile->setU32(file_off, eInvalidEntry, true, rdata.getPtr());
	aos_assert_r(rslt, false);
	mFileIsProntToEnd = false;
	
	return true;
}


bool
AosBigDataDfmSnapshot::getEntrys(
		const AosRundataPtr &rdata, 
		vector<AosDfmDocHeaderPtr> &entrys,
		AosSnapshotReadInfo &read_info)
{
	// this func is only called by DfmSnapshot Proc Thrd.
	if(!read_info.file_data)
	{
		read_info.file_data = OmnNew AosBuff(eAosSizePerRead AosMemoryCheckerArgs);
		read_info.file_off = eEntryStart;
	}
	AosBuffPtr buff = read_info.file_data;
	aos_assert_r(buff->dataLen() == buff->getCrtIdx(), false);
	
	bool rslt = readBuffFromFile(rdata, read_info.file_off,
			buff, read_info.finished);
	if(read_info.finished)	return true;	
	read_info.file_off += buff->dataLen();
	
	bool finished = false;
	AosDfmDocHeaderPtr header;
	while(1)
	{
		rslt = readEntryFromBuff(rdata, buff, header, finished); 
		aos_assert_r(rslt, false);
		if(finished)	break;
		
		if(header) entrys.push_back(header);
	}
	
	return true;
}

	
bool
AosBigDataDfmSnapshot::readBuffFromFile(
		const AosRundataPtr &rdata,
		const u64 file_off,
		const AosBuffPtr &buff,
		bool &finished)
{
	int remain = mLogFile->getLength() - file_off;
	if(remain < 0 || (u32)remain < mEntrySize)
	{
		finished = true;
		return true;
	}

	finished = false;
	u64 read_len = (u32)remain > eAosSizePerRead ? eAosSizePerRead : remain;
	read_len = (read_len / mEntrySize) * mEntrySize;
	
	aos_assert_r(read_len <= (u32)buff->buffLen(), false); 
	bool rslt = mLogFile->readToBuff(file_off, read_len, 
			buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	mFileIsProntToEnd = false;
	
	buff->setCrtIdx(0);
	buff->setDataLen(read_len);
	return true;
}

bool
AosBigDataDfmSnapshot::readEntryFromBuff(
		const AosRundataPtr &rdata,
		const AosBuffPtr &entry_buff,
		AosDfmDocHeaderPtr &header,
		bool &finished)
{
	u64 remain = entry_buff->dataLen() - entry_buff->getCrtIdx();
	if(remain < mEntrySize)
	{
		finished = true;
		return true;
	}

	finished = false;
	if(entry_buff->getU32(0) == eInvalidEntry)
	{
		header = 0;
		u32 crt_idx = entry_buff->getCrtIdx() + sizeof(u64) + mHeaderDataLen;
		entry_buff->setCrtIdx(crt_idx);
		return true;
	}

	u64 docid = entry_buff->getU64(0);
	AosBuffPtr header_data = entry_buff->getBuff(mHeaderDataLen, true AosMemoryCheckerArgs); 
	aos_assert_r(header_data, false);
	
	header = mTmpDfmDoc->cloneDfmHeader(docid, header_data);
	aos_assert_r(header, false);
	return true;
}


