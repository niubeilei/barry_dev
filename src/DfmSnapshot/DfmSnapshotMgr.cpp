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
// 2014/03/06 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "DfmSnapshot/DfmSnapshotMgr.h"

#include "DfmUtil/DfmDoc.h"
#include "DfmSnapshot/Ptrs.h"
#include "DfmSnapshot/DfmSnapshotCreator.h"
#include "DocFileMgr/DocFileMgr.h"
#include "ReliableFile/ReliableFile.h"
#include "SEInterfaces/DfmSnapshotObj.h"
//#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEUtil/SeConfig.h"


AosDfmSnapshotMgr::AosDfmSnapshotMgr(const AosDocFileMgrPtr &dfm)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mDfm(dfm)
{
}


AosDfmSnapshotMgr::~AosDfmSnapshotMgr()
{
}


AosDfmSnapshotMgrPtr
AosDfmSnapshotMgr::createSnapshotMgrStatic(
		const AosRundataPtr &rdata,
		const AosDocFileMgrPtr &dfm)
{
	AosDfmSnapshotMgrPtr snapshot_mgr = OmnNew AosDfmSnapshotMgr(dfm);

	bool rslt = snapshot_mgr->init(rdata);
	aos_assert_r(rslt, 0);
	return snapshot_mgr;
}


bool
AosDfmSnapshotMgr::init(const AosRundataPtr &rdata)
{
	bool rslt = initFile(rdata);
	aos_assert_r(rslt && mMgrFile, false);

	rslt = initFromFile(rdata);
	aos_assert_r(rslt, false);
	
	// if mStatus == eCommitting or == eRollbacking
	// then this trans maybe not finished. will be recover and continue
	// commit or rollback.
	return true;
}


bool
AosDfmSnapshotMgr::initFile(const AosRundataPtr &rdata)
{
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);

	mFileKey = "SnapshotMgr_";
	mFileKey << mDfm->getId();
	mMgrFile = vfsMgr->openRlbFileByStrKey(
			rdata.getPtr(), mDfm->getVirtualId(), mFileKey, mFileKey,
			eAosMaxFileSize, true, AosRlbFileType::eNormal);
	aos_assert_r(mMgrFile, false);
	mMgrFile->setUseGlobalFmt(true);
	return true;
}


bool
AosDfmSnapshotMgr::initFromFile(const AosRundataPtr &rdata)
{
	u64 file_len = mMgrFile->getLength();
	if(file_len == 0)	return true;

	AosBuffPtr buff = OmnNew AosBuff(file_len AosMemoryCheckerArgs);
	bool rslt = mMgrFile->readToBuff(0, file_len, buff->data(), rdata.getPtr());
	aos_assert_r(rslt, false);
	buff->setDataLen(file_len);

	u32 snap_num = buff->getU32(0);

	OmnScreen << "DocFileMgr; DfmSnapshotMgr init from file"
		<< "; virtual_id:" << getVirtualId() 
		<< "; dfm_id:" << mDfm->getId() 
		<< endl;

	AosDfmSnapshotObjPtr snapshot;
	AosDfmSnapshotMgrPtr thisptr(this, false);
	for(u32 i=0; i<snap_num; i++)
	{
		snapshot = AosDfmSnapshotCreator::createSnapshot(
			rdata, thisptr, buff, mDfm->getDocType());
		aos_assert_r(snapshot, false);
		
		rslt = addSnapshotPriv(snapshot);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosDfmSnapshotMgr::saveToFile(const AosRundataPtr &rdata)
{
	AosBuffPtr buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	buff->setU32(mSnapshots.size());
	
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.begin();
	AosDfmSnapshotObjPtr snapshot;
	for(; itr != mSnapshots.end(); itr++)
	{
		snapshot = itr->second;
		snapshot->serializeTo(buff);
	}
	
	bool rslt = mMgrFile->put(0, buff->data(), buff->dataLen(), true, rdata.getPtr());
	aos_assert_r(rslt, false);
	return true;
}


u64
AosDfmSnapshotMgr::createSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		const AosDfmSnapshotType::E snap_tp)
{
	// Ketty 2014/03/05
	bool rslt;
	
	mLock->lock();
	AosDfmSnapshotMgrPtr thisptr(this, false);
	AosDfmSnapshotObjPtr snapshot = AosDfmSnapshotCreator::createSnapshot(
			rdata, snap_tp, thisptr, snap_id, mDfm->getDocType());
	aos_assert_rl(snapshot, mLock, 0);
	
	rslt = addSnapshotPriv(snapshot);
	aos_assert_rl(rslt, mLock, 0);

	rslt = saveToFile(rdata);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return snap_id;
}

bool
AosDfmSnapshotMgr::addSnapshotPriv(const AosDfmSnapshotObjPtr &snapshot)
{
	u64 snap_id = snapshot->getSnapId();
	aos_assert_r(snap_id, false);
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.find(snap_id);
	aos_assert_r(itr == mSnapshots.end(), false);

	mSnapshots.insert(make_pair(snap_id, snapshot));
	return true;
}


bool
AosDfmSnapshotMgr::commitSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	aos_assert_r(snap_id , false);
	
	mLock->lock();
	AosDfmSnapshotObjPtr snapshot = getSnapshotByIdPriv(snap_id);
	if(!snapshot)
	{
		mLock->unlock();
		OmnScreen << "This snapshot maybe commited or rollbacked yet."
				<< "snap_id:" << snap_id
				<< endl;
		return true;
	}

	snapshot->setStatus(AosSnapshotSts::eCommitting);
	AosSnapReq req(rdata, snapshot, AosSnapshotSts::eCommitting);
	mSnapReqs.push_back(req);
	startProcThrd();
	
	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::rollbackSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	aos_assert_r(snap_id, false);
	
	mLock->lock();
	AosDfmSnapshotObjPtr snapshot = getSnapshotByIdPriv(snap_id);
	if(!snapshot)
	{
		mLock->unlock();
		OmnScreen << "This snapshot maybe commited or rollbacked yet."
				<< "snap_id:" << snap_id
				<< endl;
		return true;
	}

	snapshot->setStatus(AosSnapshotSts::eRollbacking);
	AosSnapReq req(rdata, snapshot, AosSnapshotSts::eRollbacking);
	mSnapReqs.push_back(req);
	startProcThrd();

	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::mergeSnapshot(
		const AosRundataPtr &rdata,
		const u64 target_snap_id,
		const u64 merger_snap_id)
{
	aos_assert_r(target_snap_id, false);
	aos_assert_r(merger_snap_id, false);
	if(target_snap_id == merger_snap_id)	return true;

	mLock->lock();
	AosDfmSnapshotObjPtr target_snapshot = getSnapshotByIdPriv(target_snap_id);
	aos_assert_rl(target_snapshot, mLock, false);

	AosDfmSnapshotObjPtr merger_snapshot = getSnapshotByIdPriv(merger_snap_id);
	if(!merger_snapshot)
	{
		OmnScreen << "This snapshot maybe has merged yet."
			<< "; target_snap_id:" << target_snap_id
			<< "; merger_snap_id:" << merger_snap_id
			<< endl;
		mLock->unlock();
		return true;
	}

	merger_snapshot->setStatus(AosSnapshotSts::eMergeing);
	AosSnapReq req(rdata, target_snapshot, AosSnapshotSts::eMergeing, merger_snapshot);
	mSnapReqs.push_back(req);
	startProcThrd();

	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::startProcThrd()
{
	if(!mProcThrd)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mProcThrd = OmnNew OmnThread(thisPtr, 
				"DfmSnapshotMgrFunc", 0, true, true, __FILE__, __LINE__);
		mProcThrd->start();
		return true;
	}
	
	mCondVar->signal();
	return true;
}


bool
AosDfmSnapshotMgr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mSnapReqs.empty())
		{
			//mCommitting.clear();
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		AosSnapReq req = mSnapReqs.front();
		mLock->unlock();
		
		switch(req.snapOpr)
		{
		case AosSnapshotSts::eCommitting:
			 commitSnapshotPriv(req.rdata, req.snap);
			 break;

		case AosSnapshotSts::eRollbacking:
			 rollbackSnapshotPriv(req.rdata, req.snap);
			 break;

		case AosSnapshotSts::eMergeing:
			 mergeSnapshotPriv(req.rdata, req.snap, req.snap2);
			 break;
		
		default:
			 OmnAlarm << "error!" << enderr;
			 break;
		}
	}
	return true;
}


bool
AosDfmSnapshotMgr::commitSnapshotPriv(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, false);

	bool rslt = mDfm->commit(rdata, snapshot);
	aos_assert_r(rslt, false);

	mLock->lock();
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.find(snapshot->getSnapId());
	aos_assert_rl(itr != mSnapshots.end(), mLock, false);
	mSnapshots.erase(itr);

	rslt = saveToFile(rdata);
	aos_assert_rl(rslt, mLock, false);
	
	rslt = snapshot->commitFinish(rdata);
	aos_assert_rl(rslt, mLock, false);
		
	mSnapReqs.pop_front();
	mLock->unlock();	
	return true;
}


bool
AosDfmSnapshotMgr::rollbackSnapshotPriv(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &snapshot)
{
	aos_assert_r(snapshot, false);
	
	bool rslt = mDfm->rollback(rdata, snapshot);
	aos_assert_r(rslt, false);

	mLock->lock();
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.find(snapshot->getSnapId());
	aos_assert_rl(itr != mSnapshots.end(), mLock, false);
	mSnapshots.erase(itr);

	rslt = saveToFile(rdata);
	aos_assert_rl(rslt, mLock, false);

	rslt = snapshot->rollbackFinish(rdata);
	aos_assert_rl(rslt, mLock, false);
		
	mSnapReqs.pop_front();
	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::mergeSnapshotPriv(
		const AosRundataPtr &rdata,
		const AosDfmSnapshotObjPtr &target_snapshot,
		const AosDfmSnapshotObjPtr &merger_snapshot)
{
	aos_assert_r(target_snapshot && merger_snapshot, false);
	
	bool rslt = mDfm->merge(rdata, target_snapshot, merger_snapshot);
	aos_assert_r(rslt, false);

	mLock->lock();
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.find(merger_snapshot->getSnapId());
	aos_assert_rl(itr != mSnapshots.end(), mLock, false);
	mSnapshots.erase(itr);
	
	rslt = saveToFile(rdata);
	aos_assert_rl(rslt, mLock, false);
	
	rslt = merger_snapshot->mergeFinish(rdata);
	aos_assert_rl(rslt, mLock, false);

	mSnapReqs.pop_front();
	mLock->unlock();	
	return true;
}


bool
AosDfmSnapshotMgr::readFromCommitingSnapshot(
		const AosRundataPtr &rdata,
		const u64 docid,
		bool &exist,
		AosDfmDocHeaderPtr &crt_header)
{
	bool rslt;
	exist = false;
	mLock->lock();

	deque<AosSnapReq>::reverse_iterator ritr;
	for(ritr = mSnapReqs.rbegin(); ritr != mSnapReqs.rend(); ritr++)
	{
		AosSnapReq req = *ritr;	
		if(req.snapOpr != AosSnapshotSts::eCommitting)	continue;
		
		AosDfmSnapshotObjPtr snapshot = req.snap;
		
		rslt = readEntryPriv(rdata, docid, snapshot, exist, crt_header); 
		aos_assert_rl(rslt, mLock, false);
		if(exist)	break;
	}
	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::readEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		const u64 snap_id,
		bool &exist,
		AosDfmDocHeaderPtr &crt_header)
{
	// maybe some other mergeing to this snap_id.
	mLock->lock();
	
	AosDfmSnapshotObjPtr crt_snapshot = getSnapshotByIdPriv(snap_id);
	aos_assert_rl(crt_snapshot, mLock, false);
	
	bool rslt = readEntryPriv(rdata, docid, crt_snapshot, exist, crt_header);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosDfmSnapshotMgr::readEntryPriv(
		const AosRundataPtr &rdata,
		const u64 docid,
		const AosDfmSnapshotObjPtr &crt_snapshot,
		bool &exist,
		AosDfmDocHeaderPtr &crt_header)
{
	// maybe some other mergeing to this crt_snapshot.
	aos_assert_r(crt_snapshot, false);

	bool rslt;
	deque<AosSnapReq>::reverse_iterator ritr;
	for(ritr = mSnapReqs.rbegin(); ritr != mSnapReqs.rend(); ritr++)
	{
		AosSnapReq req = *ritr;	
		if(req.snapOpr != AosSnapshotSts::eMergeing)	continue;
		
		AosDfmSnapshotObjPtr target_snapshot = req.snap;
		if(target_snapshot->getSnapId() != crt_snapshot->getSnapId())		continue;	
	
		AosDfmSnapshotObjPtr merger_snapshot = req.snap2;
		rslt = merger_snapshot->readEntry(rdata, docid, exist, crt_header);
		aos_assert_r(rslt, false);
		if(exist) return true;
	}
	
	rslt = crt_snapshot->readEntry(rdata, docid, exist, crt_header);
	aos_assert_r(rslt, false);
	return true;
}

AosDfmSnapshotObjPtr
AosDfmSnapshotMgr::getSnapshotById(const u64 snap_id)
{
	mLock->lock();
	AosDfmSnapshotObjPtr snapshot = getSnapshotByIdPriv(snap_id);
	mLock->unlock();
	
	return snapshot;
}

AosDfmSnapshotObjPtr
AosDfmSnapshotMgr::getSnapshotByIdPriv(const u64 snap_id)
{
	aos_assert_r(snap_id, 0);
	
	map<u64, AosDfmSnapshotObjPtr>::iterator itr = mSnapshots.find(snap_id);
	if(itr != mSnapshots.end())
	{
		AosDfmSnapshotObjPtr snapshot = itr->second;
		if(snapshot->isActive())	return snapshot;
		
		return 0;
	}
	
	return 0;
}


u64
AosDfmSnapshotMgr::getVirtualId()
{
	return mDfm->getVirtualId();	
}


