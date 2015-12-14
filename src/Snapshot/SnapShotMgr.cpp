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
// 09/02/2013 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/SnapShotMgr.h"

#include "DocFileMgr/DfmLogUtil.h"
#include "Rundata/Rundata.h"
#include "Snapshot/SnapShotType.h"
#include "Snapshot/Ptrs.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/Mutex.h"

AosSnapShotMgr::AosSnapShotMgr(
		const AosSnapShotType::E &type,
		const u32 entry_size,
		const bool show_log)
:
mEntrySize(entry_size),
mCtrlFile(0),
mCrtSnapSeqno(0),
mLock(OmnNew OmnMutex()),
mType(type),
mShowLog(show_log)
{
	if (mEntrySize < eEntryMinSize) mEntrySize = eEntryMinSize;
	if (mEntrySize > eEntryMaxSize) mEntrySize = eEntryMaxSize; 

	AosDfmLogUtil::createDfmLogBaseDir();
	OmnString alloc_fname = AosDfmLogUtil::getBaseDirname();
	alloc_fname << AosDfmLogUtil::getDfmLogBaseDirName();

	alloc_fname << "snap_ctrl_" << AosSnapShotType::toString(mType);
	mCtrlFile = OmnNew OmnFile(alloc_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mCtrlFile || !mCtrlFile->isGood())
	{
		mCtrlFile = OmnNew OmnFile(alloc_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!mCtrlFile || !mCtrlFile->isGood())
		{
			OmnAlarm << "Failed to open file: " << alloc_fname << enderr;
			OmnThrowException("Failed to open file:");	
		}
	}
	init();
}


AosSnapShotMgr::~AosSnapShotMgr()
{
}


bool
AosSnapShotMgr::clearData()
{
	if (!mCtrlFile) return true;
	for (u32 seqno = 1; seqno <= mCrtSnapSeqno; seqno++)
	{
		AosSnapShotMgrPtr thisPtr(this, true);
		AosSnapShotInfoPtr info = OmnNew AosSnapShotInfo(thisPtr, seqno, mType, mShowLog);
		info->clearData();
	}
	mCtrlFile->resetFile();
	init();
	return true;
}


void
AosSnapShotMgr::init()
{
	u64 crtFileSize = mCtrlFile->getFileCrtSize();
	mCrtSnapSeqno = crtFileSize/mEntrySize;
}


bool
AosSnapShotMgr::saveToFile(
		const u32 snap_seqno,
		const AosBuffPtr &buff)
{
	aos_assert_r(mCtrlFile, false);
	aos_assert_r(buff->dataLen() <= mEntrySize, false);
	u64 offset = eEntryStart + snap_seqno * mEntrySize;
	mCtrlFile->lock();
	mCtrlFile->put(offset, buff->data(), mEntrySize, true);
	mCtrlFile->unlock();
	return true;
}


AosBuffPtr
AosSnapShotMgr::readFromFile(const u32 snap_seqno)
{
	aos_assert_r(mCtrlFile, 0);
	u64 offset = eEntryStart + snap_seqno * mEntrySize;
	mCtrlFile->lock();
	u64 crtFileSize = mCtrlFile->getFileCrtSize();
	if (offset >= crtFileSize)	
	{
		mCtrlFile->unlock();
		return 0;
	}

	AosBuffPtr buff = OmnNew AosBuff(mEntrySize AosMemoryCheckerArgs);
	mCtrlFile->readToBuff(offset, mEntrySize, buff->data());
	mCtrlFile->unlock();
	buff->setDataLen(mEntrySize);
	return buff;
}


bool
AosSnapShotMgr::removeSnapShotInfo(const u32 snap_seqno)
{
	mapitr_t itr = mSnapShot.find(snap_seqno);
	aos_assert_r(itr != mSnapShot.end(), false);
	mSnapShot.erase(itr);

	removeToFile(snap_seqno);
	return true;
}


bool
AosSnapShotMgr::removeToFile(const u32 snap_seqno)
{
	AosBuffPtr buff = OmnNew AosBuff(mEntrySize AosMemoryCheckerArgs);
	memset(buff->data(), 0, mEntrySize);
	saveToFile(snap_seqno, buff);
	return true;
}


AosSnapShotPtr
AosSnapShotMgr::retrieveSnapShotLocked(
		const AosRundataPtr &rdata,
		const u32 snap_seqno,
		const u32 virtual_id,
		const u32 dfm_id,
		AosSnapShotInfoPtr &info,
		const bool create_flag)
{
	info = 0;
	AosSnapShotPtr snap_shot;
	mapitr_t itr = mSnapShot.find(snap_seqno);
	if (itr != mSnapShot.end())
	{
		info = itr->second;
		snap_shot = info->retrieveSnapShot(rdata, virtual_id, dfm_id, create_flag);
		return snap_shot;
	}

	AosSnapShotMgrPtr thisPtr(this, true);
	info = OmnNew AosSnapShotInfo(thisPtr, snap_seqno, mType, mShowLog);
	info->init(rdata, false);

	snap_shot = info->retrieveSnapShot(rdata, virtual_id, dfm_id, create_flag);
	if (!snap_shot && !create_flag) return 0;

	return snap_shot;
}


AosSnapShotPtr
AosSnapShotMgr::retrieveSnapShot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const u32 virtual_id,
		const u32 dfm_id,
		AosSnapShotInfoPtr &info,
		const bool create_flag)
{
	mLock->lock();
	u32 snap_seqno = (u32)snap_id; 
	AosSnapShotPtr snap_shot = retrieveSnapShotLocked(rdata, 
			snap_seqno, virtual_id, dfm_id, info, create_flag);
	mLock->unlock();
	return snap_shot;
}


bool
AosSnapShotMgr::addSnapShot(
		const AosRundataPtr &rdata,
		u64 &snap_id)
{
	mLock->lock();
	u32 snap_seqno = ++ mCrtSnapSeqno;
	snap_id = ((u64)mType << 32) + snap_seqno;

	AosSnapShotMgrPtr thisPtr(this, true);
	AosSnapShotInfoPtr info = OmnNew AosSnapShotInfo(thisPtr, snap_seqno, mType, mShowLog);
	aos_assert_rl(info, mLock, false);

	info->init(rdata, true);

	mSnapShot.insert(make_pair(snap_seqno, info));
	mLock->unlock();
	return true;
}


OmnString
AosSnapShotMgr::getSnapShotDirName(const u64 snap_id)
{
	u32 t = (u32)(snap_id >> 32);
	u32 seqno = (u32)snap_id;
	AosSnapShotType::E type = (AosSnapShotType::E) t;
	OmnString dir = AOS_SNAPSHOT_DIRNAME_PRE;
	dir << AosSnapShotType::toString(type) << "_"<< seqno << "/";
	return dir;
}


bool
AosSnapShotMgr::updateSnapShotInfo(
		const AosRundataPtr &rdata,
		const AosSnapShotInfoPtr &info,
		const AosSnapShot::Status sts,
		const u32 virtual_id,
		const u32 dfm_id)
{
	mLock->lock();
	bool rslt = info->updateInfo(rdata, sts, virtual_id, dfm_id);
	mLock->unlock();
	return rslt;
}


void
AosSnapShotMgr::updateSnapShotStatus(
		const AosSnapShot::Status sts,
		const AosSnapShotInfoPtr &info)
{
	mLock->lock();
	info->updateStatus(sts);
	mLock->unlock();
}

