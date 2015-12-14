////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/06/18	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLogFile.h"

#include "API/AosApi.h"
#include "DocFileMgr/DfmLogUtil.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocFileMgr/DfmLog.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"


AosDfmLogFile::AosDfmLogFile(
		const AosDfmDocType::E doc_type,
		const u32 file_size, 
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmLogPtr &log,
		const bool show_log)
:
mMaxFileSize(file_size),
mVirtualId(virtual_id),
mDfmId(dfm_id),
//mSnapId(0),
mLock(OmnNew OmnMutex()),
mFile(0),
mFileId(0),
mCrtOffset(0),
mDfmLog(log),
mType("norm"),
mTempDoc(0),
mShowLog(show_log)
{
	mTempDoc = AosDfmDoc::cloneDoc(doc_type);
}

/*
 * Linda, 2014/03/04
AosDfmLogFile::AosDfmLogFile(
		const AosDfmDocType::E doc_type,
		const u32 file_size, 
		const u32 virtual_id,
		const u32 dfm_id,
		const AosDfmLogPtr &log,
		const u64 snap_id,
		const bool show_log)
:
mMaxFileSize(file_size),
mVirtualId(virtual_id),
mDfmId(dfm_id),
mSnapId(snap_id),
mLock(OmnNew OmnMutex()),
mFile(0),
mFileId(0),
mCrtOffset(0),
mDfmLog(0),
mType("snap"),
mTempDoc(0),
mShowLog(show_log)
{
	mTempDoc = AosDfmDoc::cloneDoc(doc_type);
}
*/


AosDfmLogFile::~AosDfmLogFile()
{
}


bool
AosDfmLogFile::findDocLocked(const u64 &docid, DocFileInfo &info)
{
	mapitr_t itr = mDocMap.find(docid);
	if (itr != mDocMap.end())
	{
		info = itr->second;
		return true;
	}

	for (int idx = mMaps.size() -1 ; idx >= 0; idx--)
	{
		map_t vv = mMaps.at(idx);
		aos_assert_r(vv.size() > 0, false);

		itr = vv.find(docid);
		if (itr != vv.end())
		{
			info = itr->second;
			return true;
		}
	}
	return false;
}


AosDfmDocPtr
AosDfmLogFile::readDoc(
		const AosRundataPtr &rdata,
		const u64 docid)
{
	DocFileInfo info;
	mLock->lock();
	bool found = findDocLocked(docid, info);
	if (!found) 
	{
		mLock->unlock();
		return 0;
	}

	OmnFilePtr file;
	int64_t offset = info.offset;
	u64 file_id = info.fileid;
	if (file_id == mFileId)
	{
		file = mFile;
	}
	else
	{
		file = AosDfmLogUtil::openFile(file_id);
	}
	aos_assert_rl(file && file->isGood(), mLock, 0);
	mLock->unlock();
	
	AosDfmDocPtr doc = readFromFile(rdata, offset, file);
	aos_assert_r(doc && docid == doc->getDocid(), 0);

	if (mShowLog)
	{
		if (doc->getOpr() != AosDfmDoc::eDelete)
		{
			OmnScreen << "DfmLog; readDoc; finish."
				<< "; virtual_id:" << mVirtualId
				<< "; docid:" << docid 
				<< "; sts:" << (doc->getOpr() == AosDfmDoc::eSave?"save":"delete")
				<< "; body_len:" << (doc->getBodyBuff())->dataLen() 
				<< "; rcd_total:" << (doc->getBodyBuff())->dataLen() + 20 
				<< "; file_id:" << file_id 
				<< "; offset:" << offset 
				<< "; file_name:" << file->getFileName()
				<< endl;
		}
	}
	// Linda, 2013/09/23 move to DocFileMgr.cpp
	//if (doc->getOpr() == AosDfmDoc::eDelete) return 0;
	return doc;
}


AosDfmDocPtr
AosDfmLogFile::readFromFile(
		const AosRundataPtr &rdata,
		const int64_t &offset,
		const OmnFilePtr &file)
{
	file->lock();
	u32 rcd_total = file->readBinaryU32(offset, 0); 
	if (rcd_total <= 0)
	{
		file->unlock();
		OmnAlarm << "rcd_total < 0" << enderr;
		return 0;
	}

	AosBuffPtr buff = OmnNew AosBuff(rcd_total AosMemoryCheckerArgs);
	int len = file->readToBuff(offset + sizeof(u32), rcd_total, buff->data());	
	file->unlock();
	aos_assert_r(len > 0 && (u32)len == rcd_total, 0);
	buff->setDataLen(rcd_total);

	AosDfmDocPtr doc = AosDfmLogUtil::getDocInfoFromBuff(rdata, mTempDoc, buff);
	return doc;
}


bool
AosDfmLogFile::addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	// u32 		rcd_total
	// u64		docid
	// u32     	timestats 
	// u32		status
	// int64	compress_len
	// int64	orig_len
	// int64	header_data_len
	// char *	header
	// int64 	body_data_len
	// char *	body
	aos_assert_r(doc, false);
	aos_assert_r(doc->getOpr() > AosDfmDoc::eInvalidOpr && doc->getOpr() < AosDfmDoc::eMaxOpr, 0);
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	bool rslt = AosDfmLogUtil::setDocInfoToBuff(rdata, buff, doc, trans_ids); 
	aos_assert_r(rslt, false);

	u32 rcd_total = buff->dataLen();
	aos_assert_r(rslt && rcd_total > 0 , false);

	mLock->lock();
	int64_t requested_space = rcd_total + sizeof(u32);	
	OmnFilePtr file = getFileLocked(rdata, requested_space);
	aos_assert_rl(file && file->isGood(), mLock, false);

	u64 offset = mCrtOffset;	
	file->lock();
	file->setU32(offset, rcd_total, true);
	file->put(offset + sizeof(u32), buff->data(), rcd_total, true);
	file->unlock();

	u64 docid = doc->getDocid();
	DocFileInfo info;
	info.offset = offset;
	info.fileid = mFileId;

	mCrtOffset += requested_space;
	mDocMap[docid] = info;
	mLock->unlock();

	if (mShowLog)
	{
		OmnString trans_str;
		for(u32 i=0; i<trans_ids.size(); i++)
		{
			trans_str << trans_ids[i].toString() << "; ";
		}
		
		OmnScreen << "DfmLog; addEntry; finish."
			<< "; virtual_id:" << mVirtualId
			<< "; docid:" << doc->getDocid()
			<< "; sts:" << (doc->getOpr() == AosDfmDoc::eSave?"save":"delete")
			<< "; body_len:" << rcd_total - 20
			<< "; rcd_total:" << rcd_total
			<< "; file_id:" << mFileId
			<< "; offset:" << offset 
			<< "; file_name:" << file->getFileName()
			<< "; trans_ids:" << trans_str
			<< endl;
	}
	return true;
}


OmnFilePtr
AosDfmLogFile::getFileLocked(
		const AosRundataPtr &rdata,
		const int64_t requested_space)
{
	aos_assert_r(requested_space <= mMaxFileSize, 0);
	if (mFile)
	{
		if (mCrtOffset + requested_space < mMaxFileSize) 
		{
			return mFile;
		}

		addCrtRequestLocked(rdata);
	}

	u64 file_id = 0;
	//mFile = AosDfmLogUtil::createNewFile(mVirtualId, rdata->getSiteid(), mDfmId, mSnapId, file_id);	
	mFile = AosDfmLogUtil::createNewFile(mVirtualId, rdata->getSiteid(), mDfmId, file_id);	
	aos_assert_r(mFile && mFile->isGood(), 0);

	mFileId = file_id;
	AosBuffPtr buff = OmnNew AosBuff(eStartOff, 0 AosMemoryCheckerArgs);
	memset(buff->data(), 0, eStartOff);
	buff->setU32(0);
	buff->setU32(0);
	buff->setU32(mDfmId);
	buff->setU64(mFileId);
	mFile->put(0, buff->data(), eStartOff, true);
	//OmnScreen << "createNewFile file_id:" << mFileId << "; fname:" << mFile->getFileName() << endl;
	//mFile->setU32(eFlagOff, 0, false);
	//mFile->setU32(eDfmIdOff, mDfmId, false);
	//mFile->setU64(eFileIdOff, mFileId, true);
	mCrtOffset = eStartOff;
	return mFile;
}


void
AosDfmLogFile::addCrtRequestLocked(const AosRundataPtr &rdata)
{
	if (mDocMap.size() <= 0) return;

	map_t vv;
	vv.swap(mDocMap);

	mMaps.push_back(vv);
	if (mDfmLog) mDfmLog->addRequest(rdata, mFileId); 

	mFile = 0;
	mFileId = 0;
	//OmnScreen << " queue size: " << mMaps.size() << endl;
}


bool
AosDfmLogFile::readFileIdFromQueue(const u64 &file_id)
{
	mLock->lock();
	aos_assert_rl(mMaps.size() != 0, mLock, false);

	map_t vv = mMaps.front();
	mapitr_t iter = vv.begin();
	aos_assert_rl(iter != vv.end(), mLock, false);
	aos_assert_rl(iter->second.fileid == file_id, mLock, false);
	mLock->unlock();
	return true;
}

bool
AosDfmLogFile::removeFileIdFromQueue(const u64 &file_id)
{
	mLock->lock();
	map_t vv = mMaps.front();
	mapitr_t iter = vv.begin();
	aos_assert_rl(iter != vv.end(), mLock, false);
	aos_assert_rl(iter->second.fileid == file_id, mLock, false);
	
	mMaps.pop_front();
	mLock->unlock();
	return true;
}


AosBuffPtr
AosDfmLogFile::readDfmLogFile(
		const u64 &file_id,
		const bool mod_flag,
		bool &no_send_fmt)
{
	OmnFilePtr file = AosDfmLogUtil::openFile(file_id);
	aos_assert_r(file && file->isGood(), 0);

	if (mod_flag) file->setU32(eFlagOff, 1, true);

	AosBuffPtr buff = OmnNew AosBuff(mMaxFileSize AosMemoryCheckerArgs);
	file->lock();
	u32 fmt = file->readBinaryU32(AosDfmLogFile::eFmtFlagOff, 0);
	aos_assert_r(fmt < 2, 0);
	no_send_fmt = fmt;

	u64 fileid = file->readBinaryU64(eFileIdOff, 0);

	u64 file_len = file->getLength() - eStartOff;
	int len = file->readToBuff(eStartOff, mMaxFileSize, buff->data());
	file->unlock();

	aos_assert_r(fileid != 0 && fileid == file_id, 0);
	aos_assert_r(len > 0 && (u64)len == file_len, 0);
	buff->setDataLen(len);
	return buff;
}


bool
AosDfmLogFile::removeFile(const u64 &file_id)
{
	return AosDfmLogUtil::removeFile(file_id);	
}


bool
AosDfmLogFile::addCrtRequest(const AosRundataPtr &rdata)
{
	OmnScreen << "DfmLog add crt request virtualid: " << mVirtualId << endl;
	mLock->lock();
	if (mDocMap.size() == 0)
	{
		mLock->unlock();
		return true;
	}
	addCrtRequestLocked(rdata);
	mLock->unlock();
	return true;
}

/*
 * Linda, 2014/04/04
bool 
AosDfmLogFile::getMapQueue(deque<map_t> &maps)
{
	mMaps.push_back(mDocMap);
	maps = mMaps; 
	return true;
}


OmnString
AosDfmLogFile::getDfmLogDirPath()
{
	return AosDfmLogUtil::getSnapShotFullPath(mVirtualId, mSnapId);
}


bool
AosDfmLogFile::commit(
		const AosRundataPtr &rdata, 
		const AosDfmLogFilePtr &dfmlogfile)
{
	deque<map_t> rhs_maps;
	dfmlogfile->getMapQueue(rhs_maps);

	u64 t1 = OmnGetTimestamp();
	mLock->lock();
	// clear current Memory dfm log
	addCrtRequestLocked(rdata);
	while(!rhs_maps.empty())
	{
		map_t vv = rhs_maps.front();
		rhs_maps.pop_front();
		mMaps.push_back(vv);

		mapitr_t iter = vv.begin();
		aos_assert_rl(iter != vv.end(), mLock, false);

		u64 file_id = iter->second.fileid;
		AosDfmLogUtil::moveFile(file_id, mVirtualId);
		mDfmLog->addRequest(rdata, file_id); 
	}

	AosDfmLogUtil::removeEmptyDir(dfmlogfile->getDfmLogDirPath());
	mLock->unlock();
	aos_assert_r(mDfmLog, false);
u64 t2 = OmnGetTimestamp(); 
OmnScreen << "cost: " <<  t2 - t1  << " , " << AosTimestampToHumanRead(t2-t1)<< endl;

	return true;
}


bool
AosDfmLogFile::rollBackFinishTrans(const u64 file_id)
{
	OmnScreen << "AAAAAAAAAAA rollBackFinishTrans " << file_id << endl; 
	bool no_send_fmt = false;
	AosBuffPtr buff = readDfmLogFile(file_id, false, no_send_fmt);
	aos_assert_r(buff, false);
	buff->reset();
	int64_t buff_len = buff->dataLen();
	while(buff->getCrtIdx() < buff_len)
	{
		u32 rcd_total = buff->getU32(0);
		aos_assert_r(rcd_total != 0 && (u64)rcd_total <= buff_len, false);

		int64_t crt_idx = buff->getCrtIdx();
		vector<AosTransId> trans_ids;
		AosDfmDocPtr doc = AosDfmLogUtil::getDocInfoFromBuff(mTempDoc, buff, trans_ids);
		aos_assert_r(doc && doc.getPtr(), false);

		if (trans_ids.size() > 0)
		{
		    AosFinishTrans(trans_ids);
		}

		buff->setCrtIdx(crt_idx + rcd_total);
	}
	OmnScreen << "AAAAAAAAAAA roll Back remove File " << file_id << endl; 
	removeFile(file_id);
	return true;
}
*/


bool
AosDfmLogFile::addEntrys(
		const AosRundataPtr &rdata,
		map<u64, u64> &index_map,
		const AosBuffPtr &buff)
{
	// This function is used by save IIL Data.
	//Linda, 2014/02/27 
	mLock->lock();
	int64_t requested_space = buff->dataLen();	
	OmnFilePtr file = getFileLocked(rdata, requested_space);
	aos_assert_rl(file && file->isGood(), mLock, false);

	u64 offset = mCrtOffset;	
	file->lock();
	file->put(offset, buff->data(), requested_space, true);
	file->unlock();

	map<u64, u64> ::iterator itr;
	for (itr = index_map.begin(); itr != index_map.end(); ++itr)
	{
		u64 docid = itr->first;
		u64 idx = itr->second;
		DocFileInfo info;
		info.offset = offset + idx;
		info.fileid = mFileId;

		mDocMap[docid] = info;
	}

	mCrtOffset += requested_space;
	mLock->unlock();
	return true;
}

