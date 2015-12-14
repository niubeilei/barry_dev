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
// 2015/03/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "BlobSE/BlobSESnapShotMgr.h"

#include "API/AosApiR.h"
#include "BlobSE/BlobHeader.h"
#include "BlobSE/RawfidRange.h"
#include "RawSE/RawSE.h"
#include "Thread/RwLock.h"
#include "Util/Buff.h"


AosBlobSESnapshotMgr::AosBlobSESnapshotMgr(
		const AosBlobSEPtr &blobSE,
		const AosRawSEPtr &rawSE)
:
mShowLog(true),
mBlobSE(blobSE),
mRawSE(rawSE),
mSnapshotsLock(OmnNew OmnRwLock()),
mCommittedDocidsLock(OmnNew OmnRwLock()),
mCrtSnapshotRawFileId(eSnapshotLogFileID_start),
mIsSaving(false),
mProcReqsLock(OmnNew OmnMutex()),
mLock(OmnNew OmnMutex())
{
	//mBlobSEConfig = blobSE->getConfig(); 
}


AosBlobSESnapshotMgr::~AosBlobSESnapshotMgr()
{
}


//////////////////////////////////////////
// File APIs
//////////////////////////////////////////
bool
AosBlobSESnapshotMgr::init(const AosRundataPtr &rdata)
{
	bool rslt;

	// initMgrFiles.
	mMgrFile1 = getRawFile(rdata, eSnapshotMgrFileID1, true);
	aos_assert_r(mMgrFile1, false);
	mMgrFile2 = getRawFile(rdata, eSnapshotMgrFileID2, true);
	aos_assert_r(mMgrFile2, false);

	if(mMgrFile1->getLength() == 0 && mMgrFile2->getLength() == 0)
	{
		// This BlobSE is New.
		return true;
	}
	
	rslt = initCrtActiveFile(rdata);
	aos_assert_r(rslt, false);
	
	rslt = initFromFile(rdata, mCrtActiveFile);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosBlobSESnapshotMgr::initCrtActiveFile(
		const AosRundataPtr &rdata)
{
	bool file1_good = isFileGood(rdata, mMgrFile1);
	bool file2_good = isFileGood(rdata, mMgrFile2);
	
	if(file1_good && file2_good)
	{
		u64 file1_save_seqno = readSaveSeqno(rdata, mMgrFile1);
		u64 file2_save_seqno = readSaveSeqno(rdata, mMgrFile2);
		aos_assert_r(file1_save_seqno && file2_save_seqno, false);
		aos_assert_r(file1_save_seqno != file2_save_seqno, false);
		
		if(file1_save_seqno > file2_save_seqno)
		{
			mCrtSaveSeqno = file1_save_seqno;
			mCrtActiveFile = mMgrFile1;
			return true;	
		}
	
		mCrtSaveSeqno = file2_save_seqno;
		mCrtActiveFile = mMgrFile2;
		return true;
	}
		
	aos_assert_r(file1_good || file2_good, false);
	if(file1_good)
	{
		mCrtSaveSeqno = readSaveSeqno(rdata, mMgrFile1);	
		aos_assert_r(mCrtSaveSeqno, false);
		
		mCrtActiveFile = mMgrFile1;
		return true;
	}
	
	mCrtSaveSeqno = readSaveSeqno(rdata, mMgrFile2);
	aos_assert_r(mCrtSaveSeqno, false);
	
	mCrtActiveFile = mMgrFile2;
	return true;
}


bool
AosBlobSESnapshotMgr::initFromFile(
		const AosRundataPtr &rdata,
		const AosRawFilePtr &file)
{
	aos_assert_r(file, false);
	bool rslt;
	u64 snap_id = 0;

	OmnScreen << "BlogSESnapshotMgr init from file"
		<< "; site_id:" << mBlobSE->getConfig().siteid
		<< "; cube_id:" << mBlobSE->getConfig().cubeid
		<< "; aseid:" << mBlobSE->getConfig().aseid
		<< endl;

	u64 file_len = file->getLength();
	aos_assert_r(file_len, false);

	AosBuffPtr buff = OmnNew AosBuff(file_len AosMemoryCheckerArgs);
	// WWWW
	//bool rslt = file->readToBuff(0, file_len, buff->data(), rdata.getPtr());
	//aos_assert_r(rslt, false);
	//buff->setDataLen(file_len);
	int file_rslt = file->read(rdata.getPtr(), 0,
			file_len, buff);
	aos_assert_r(file_rslt == 0, false);
	
	buff->setCrtIdx(eCrtSnapshotRawFileIdPos);
	mCrtSnapshotRawFileId = buff->getU64(0);
	aos_assert_r(mCrtSnapshotRawFileId > eSnapshotLogFileID_start, false);

	// init mSnapShots
	u32 snap_num = buff->getU32(0);
	AosBlobSESnapshotPtr snapshot;
	for(u32 i=0; i<snap_num; i++)
	{
		snapshot = AosBlobSESnapshot::createSnapshotStatic(rdata,
				mBlobSE->getConfig(), mRawSE, buff);
		aos_assert_r(snapshot, false);
		
		rslt = addSnapshotLocked(snapshot);
		aos_assert_r(rslt, false);
	}

	// try continue remove this invalid snap files.
	u32 invalid_snap_num = buff->getU32(0);
	set<u32> invalid_snap_ids;
	for(u32 i=0; i<invalid_snap_num; i++)
	{
		rslt = AosBlobSESnapshot::removeFilesStatic(rdata,
				mBlobSE->getConfig(), mRawSE, buff, snap_id);
		aos_assert_r(rslt, false);
		invalid_snap_ids.insert(snap_id);
	}

	// init mProcReqs.
	if(buff->getU8(0) == 1)
	{
		// has ProcingSnap.
		snap_id = buff->getU64(0);
		snapshot = getSnapshotById(snap_id);
		if(snapshot)
		{
			rslt = initProcReq(rdata, snapshot); 
			aos_assert_r(rslt, false);
		}
		else
		{
			// this Snap has proced succ.
			set<u32>::iterator itr = invalid_snap_ids.find(snap_id);
			aos_assert_r(itr != invalid_snap_ids.end(), false);
		}
	}
	u32 req_num = buff->getU32(0);
	for(u32 i=0; i<req_num; i++)
	{
		snap_id = buff->getU64(0);
		snapshot = getSnapshotById(snap_id);
		aos_assert_r(snapshot, false);
	
		rslt = initProcReq(rdata, snapshot); 
		aos_assert_r(rslt, false);
	}

	OmnScreen << "BlogSESnapshotMgr init from file finish"
		<< endl;
	return true;
}


bool
AosBlobSESnapshotMgr::isFileGood(
		const AosRundataPtr &rdata,
		const AosRawFilePtr &file)
{
	u64 file_len = file->getLength();
	if(file_len <= 8)	return false;

	AosBuffPtr data = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	u64 offset = file_len - 8;
	int rslt = file->read(rdata.getPtr(), offset, 8, data); 
	aos_assert_r(rslt == 0, false);

	if(data->getU32(0) != eFinishFlag)	return false;
	if(data->getU32(0) != eFileGoodFlag)	return false;

	return true;
}


u64 
AosBlobSESnapshotMgr::readSaveSeqno(
		const AosRundataPtr &rdata,
		const AosRawFilePtr &file)
{
	AosBuffPtr data = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	int rslt = file->read(rdata.getPtr(), eCrtSaveSeqnoPos, 8, data); 
	aos_assert_r(rslt == 0, 0);

	u64 save_seq = data->getU64(0);
	return save_seq;
}


 //////////////////////////////////////////      
 //// Snapshot request processing APIs             
 ////////////////////////////////////////////      

bool
AosBlobSESnapshotMgr::initProcReq(
		const AosRundataPtr &rdata,
		const AosBlobSESnapshotPtr &snapshot)
{
	bool rslt;
	AosSnapshotStatus::E sts = snapshot->getStatus();
	if(sts != AosSnapshotStatus::eSubmitted)
	{
		AosProcReq req(snapshot, sts);
		rslt = addProcReq(rdata, req);
		aos_assert_r(rslt, false);
		return true;
	}
	
	u32 p_snapid = snapshot->getParentSnapId();
	AosBlobSESnapshotPtr p_snap = getSnapshotById(p_snapid);
	aos_assert_r(p_snap, false);
	
	AosProcReq req(snapshot, sts, p_snap);
	rslt = addProcReq(rdata, req);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBlobSESnapshotMgr::addProcReq(
		const AosRundataPtr &rdata,
		const AosProcReq &req)
{
	mProcReqsLock->lock();
	
	if(mIsProcingSnap)
	{
		mProcReqs.push_back(req);
		mProcReqsLock->unlock();
		return true;
	}

	bool rslt = startProcReqLocked(rdata, req);
	aos_assert_rl(rslt, mProcReqsLock, false);

	mProcReqsLock->unlock();

	return true;
}


bool
AosBlobSESnapshotMgr::startProcReqLocked(
		const AosRundataPtr &rdata,
		const AosProcReq &req)
{
	mIsProcingSnap = req.snap;

	AosBlobSESnapshotMgrPtr thisPtr(this, false);
	OmnThrdShellProcPtr runner = OmnNew ThrdProcer(
			rdata, thisPtr, req);

	vector<OmnThrdShellProcPtr> runners;
	runners.push_back(runner);

	AosRunProcsSync(rdata.getPtr(), runners);
	return true;
}

bool
AosBlobSESnapshotMgr::procCommitReq(
		const AosRundataPtr &rdata,
		const AosBlobSESnapshotPtr &snapshot)
{
	// procThreadFunc. 
	aos_assert_r(snapshot, false);
	bool rslt;
	
	u64 snap_id = snapshot->getSnapId();

	vector<AosBlobHeaderPtr> vt_headers;
	rslt = snapshot->readAllEntrys(rdata, vt_headers);
	aos_assert_r(rslt, false);
	
	//rslt = commitSnapshotPriv(rdata, snap_id, 
	//		vt_headers, snapshot->getCommittedTimeStamp());
	//aos_assert_r(rslt, false);

	if(mShowLog)
	{
		OmnScreen << "BlobSE; commit snapshot extra docs;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< endl;
	}

	rslt = removeSnapshot(snapshot);
	aos_assert_r(rslt, false);

	// For update header
	vector<AosBlobHeaderPtr> headerList;
	headerList.clear();
	map<u64, AosBlobHeaderPtr>::iterator itr;
	
	set<u64> docid_set;
	u64 docid;
	for(u64 i=0; i<mCommittedSnapshots.size(); i++)
	{
		rslt = mCommittedSnapshots[i]->readAllDocids(rdata, docid_set);
		set<u64>::iterator set_itr = docid_set.begin();
		aos_assert_r(rslt, false);

		for(; set_itr != docid_set.end(); set_itr++)
		{
			docid = *set_itr;
			map<u64, AosBlobHeaderPtr> headerMap = mCommittedSnapshots[i]->getHeaderMap();
			itr = headerMap.find(docid); 
			if(itr != headerMap.end())
			{
				headerList.push_back(itr->second);
			}
			docid_set.clear();
		}

	}
	const u64 timestamp = OmnGetTimestamp();
	mBlobSE->updateHeaders(rdata.getPtr(), headerList, timestamp);

	/*
	snapshot->startCommitExtraEntry();
	
	vt_headers.clear();
	rslt = snapshot->readAllExtraEntrys(rdata, vt_headers);
	aos_assert_r(rslt, false);
	rslt = commitSnapshotPriv(rdata, snap_id, vt_headers);
	aos_assert_r(rslt, false);
	
	snapshot->commitExtraEntryFinish(); 
	*/
	
	//rslt = cleanCommittedDocids(rdata, snapshot);
	//aos_assert_r(rslt, false);


	return true;
}

bool
AosBlobSESnapshotMgr::procCancelReq(
		const AosRundataPtr &rdata,
		const AosBlobSESnapshotPtr &snapshot)
{
	// procThreadFunc. 
	aos_assert_r(snapshot, false);
	bool rslt;

	vector<AosBlobHeaderPtr> vt_headers;
	rslt = snapshot->readAllEntrys(rdata, vt_headers);
	aos_assert_r(rslt, false);
	
	u64 snap_id = snapshot->getSnapId();
	u64 docid = 0;
	u32 body_seqno = 0;
	u64 body_offset = 0;
	for(u32 i=0; i<vt_headers.size(); i++)
	{
		docid = vt_headers[i]->getDocid();
		body_seqno = vt_headers[i]->getBodyRawfid();
		body_offset = vt_headers[i]->getBodyOffset();
		
		if(mShowLog)
		{
			OmnScreen << "BlobSE; Canceling;" 
				<< "; site_id:" << mBlobSE->getConfig().siteid
				<< "; cube_id:" << mBlobSE->getConfig().cubeid
				<< "; aseid:" << mBlobSE->getConfig().aseid
				<< "; snap_id:" << snap_id
				<< "; docid:" << docid
				<< "; bodySeq:" << body_seqno
				<< "; bodyOff:" << body_offset
				<< endl;
		}
		
		if(body_seqno ==0 && body_offset ==0)	continue;

		int rslt = mBlobSE->addToDeleteLog(rdata.getPtr(), vt_headers[i]);
		aos_assert_r(rslt == 0, false);
	}

	rslt = removeSnapshot(snapshot);
	aos_assert_r(rslt, false);

	return true;
}

bool
AosBlobSESnapshotMgr::procSubmitReq(
		const AosRundataPtr &rdata,
		const AosBlobSESnapshotPtr &target_snapshot,
		const AosBlobSESnapshotPtr &submit_snapshot)
{
	// procThreadFunc. 
	aos_assert_r(target_snapshot && submit_snapshot, false);
	bool rslt;

	vector<AosBlobHeaderPtr> vt_headers;
	rslt = submit_snapshot->readAllEntrys(rdata, vt_headers);
	aos_assert_r(rslt, false);
	
	u32 target_snap_id = target_snapshot->getSnapId();
	u32 submit_snap_id = submit_snapshot->getSnapId();

	u64 docid = 0;
	u32 body_seqno = 0;
	u64 body_offset = 0;
	AosBlobHeaderPtr old_header;
	for(u32 i=0; i<vt_headers.size(); i++)
	{
		docid = vt_headers[i]->getDocid();
		body_seqno = vt_headers[i]->getBodyRawfid();
		body_offset = vt_headers[i]->getBodyOffset();
		
		if(mShowLog)
		{
			OmnScreen << "BlobSE; submitting;" 
				<< "; site_id:" << mBlobSE->getConfig().siteid
				<< "; cube_id:" << mBlobSE->getConfig().cubeid
				<< "; aseid:" << mBlobSE->getConfig().aseid
				<< "; target_snap_id:" << target_snap_id
				<< "; submit_snap_id:" << submit_snap_id
				<< "; docid:" << docid
				<< "; bodySeq:" << body_seqno
				<< "; bodyOff:" << body_offset
				<< endl;
		}
		
		rslt = target_snapshot->appendEntry(rdata, docid,
				vt_headers[i], old_header);
		aos_assert_r(rslt, false);
	
		if(old_header && old_header->getBodyRawfid() != 0 &&
			old_header->getBodyOffset() != 0)
		{
			//release the body and header used by the old header
			int rslt = mBlobSE->addToDeleteLog(rdata.getPtr(), old_header);
			aos_assert_r(rslt == 0, false);
		}
	}

	target_snapshot->removeChildSnapshot(rdata, submit_snap_id);
	
	rslt = removeSnapshot(submit_snapshot);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosBlobSESnapshotMgr::commitSnapshotPriv(
		const AosRundataPtr &rdata,
		const u64 snap_id,
		vector<AosBlobHeaderPtr> &vt_headers,
		u32 commit_time_stamp)
{
	if(vt_headers.size() == 0)	return true;
	
	bool rslt;
	u64 docid = 0;
	u32 body_seqno = 0;
	u64 body_offset = 0;
	for(u32 i=0; i<vt_headers.size(); i++)
	{
		docid = vt_headers[i]->getDocid();
		body_seqno = vt_headers[i]->getBodyRawfid();
		body_offset = vt_headers[i]->getBodyOffset();
		if(commit_time_stamp)
		{
			vt_headers[i]->setTimestamp(commit_time_stamp);
		}

		if(mShowLog)
		{
			OmnScreen << "BlobSE; commit snapshot;" 
				<< "; site_id:" << mBlobSE->getConfig().siteid
				<< "; cube_id:" << mBlobSE->getConfig().cubeid
				<< "; aseid:" << mBlobSE->getConfig().aseid
				<< "; snap_id:" << snap_id
				<< "; docid:" << docid
				<< "; bodySeq:" << body_seqno
				<< "; bodyOff:" << body_offset
				<< endl;
		}

		OmnNotImplementedYet;
		rslt = false; //mBlobSE->commitDoc(rdata, docid vt_headers[i]);
		aos_assert_r(rslt, false);
		/*
		if(body_seqno !=0 || body_offset !=0)
		{
			rslt = mBlobSE->createDoc(rdata, vt_headers[i]);
			aos_assert_r(rslt, false);
		}
		else
		{
			rslt = mBlobSE->deleteDoc(rdata, docid);
			aos_assert_r(rslt, false);
		}
		*/
	}
	
	return true;
}


//////////////////////////////////////////
// internal Snapshot APIs
//////////////////////////////////////////
bool
AosBlobSESnapshotMgr::addSnapshotLocked(const AosBlobSESnapshotPtr &snapshot)
{
	u64 snap_id = snapshot->getSnapId();
	aos_assert_r(snap_id, false);

	map<u64, AosBlobSESnapshotPtr>::iterator itr = mSnapshots.find(snap_id);
	//aos_assert_r(itr == mSnapshots.end(), false);
	if(itr != mSnapshots.end())
	{
		OmnAlarm << "This snap_id already exist!"
			<< "; snap_id:" << snap_id << enderr;
		return false;
	}

	mSnapshots.insert(make_pair(snap_id, snapshot));
	return true;
}


AosBlobSESnapshotPtr
AosBlobSESnapshotMgr::getSnapshotById(const u64 snap_id)
{
	aos_assert_r(snap_id != 0, 0);

	AosBlobSESnapshotPtr snapshot;
	
	mSnapshotsLock->readlock();
	map<u64, AosBlobSESnapshotPtr>::iterator itr = mSnapshots.find(snap_id);
	if(itr != mSnapshots.end())
	{
		snapshot = itr->second;
	}
	
	mSnapshotsLock->unlock();
	return snapshot;
}


bool
AosBlobSESnapshotMgr::removeSnapshot(const AosBlobSESnapshotPtr &snapshot)
{
	u64 snap_id = snapshot->getSnapId();
	aos_assert_r(snap_id, false);
	
	mSnapshotsLock->readlock();
	map<u64, AosBlobSESnapshotPtr>::iterator itr = mSnapshots.find(snap_id);
	if(itr == mSnapshots.end())
	{
		mSnapshotsLock->unlock();
		OmnAlarm << "remove Snapshot Error!"
			<< "snap_id:" << snap_id
			<< enderr;
		return false;
	}

	mSnapshots.erase(itr);
	if(snapshot->getStatus()== AosSnapshotStatus::eCommitted)
	{
		//add snapshot ---> mCommitSnapshot
		mCommittedSnapshots.push_back(snapshot);
		mCommittedSnapshotsMap[snap_id] = snapshot;
	}
	else if(snapshot->getStatus()== AosSnapshotStatus::eCanceled)
	{
		// move this snapshot to invalid_list.
		mCanceledSnapshots.push_back(snapshot);
	}
	
	mSnapshotsLock->unlock();
	return true;
}



//////////////////////////////////////////
// internal Doc APIs
//////////////////////////////////////////
bool
AosBlobSESnapshotMgr::appendCommittedDocids(
		const AosRundataPtr &rdata, 
		const AosBlobSESnapshotPtr &snapshot)
{
	vector<u64> *snapIdList;
	u64 snap_id = snapshot->getSnapId();
	
	set<u64> docid_set;
	bool rslt = snapshot->readAllDocids(rdata, docid_set);
	aos_assert_r(rslt, false);

	//add snapshot ---> mCommitSnapshot
	//mCommittedSnapshots.push_back(snapshot);

	mCommittedDocidsLock->writelock();
	//pair<multimap<u64, vector<u64> >::iterator,bool> ret;
	//map<u64, vector<u64> >::iterator map_itr = mDocidSnapshotMap.begin();
	hash_map<u64, vector<u64> >::iterator docid_itr;
	u64 docid;
	set<u64>::iterator set_itr = docid_set.begin();
	vector<u64> emptySnapshots;

	emptySnapshots.clear();
	//snapIdList.clear();
	for(; set_itr != docid_set.end(); set_itr++)
	{
		docid = *set_itr;
		docid_itr = mDocidSnapshotMap.find(docid);

		//when docid == mDocidSnapshotMap's first add snap_id to snapIdList
		//else add snap_id to new snapIdList
		if(docid_itr == mDocidSnapshotMap.end())
		{
			mDocidSnapshotMap[docid] = emptySnapshots;
		}

		snapIdList = &(mDocidSnapshotMap[docid]);
		snapIdList->push_back(snap_id);

		/*map_itr = ret.first;
		if(!ret.second)
		{
			// means insert failed. this docid has exist.
			// change the snap id list.
			map_itr->second = snapIdList;
		}
		*/
	}
	mCommittedDocidsLock->unlock();

	return true;
}


bool
AosBlobSESnapshotMgr::cleanCommittedDocids(
		const AosRundataPtr &rdata, 
		const AosBlobSESnapshotPtr &snapshot)
{
	u64 snap_id = snapshot->getSnapId();
	
	set<u64> docid_set;
	bool rslt = snapshot->readAllDocids(rdata, docid_set);
	aos_assert_r(rslt, false);

	mCommittedDocidsLock->writelock();
	pair<map<u64, vector<u64> >::iterator,bool> map_ret;
	hash_map<u64, vector<u64> >::iterator map_itr = mDocidSnapshotMap.begin();

	u64 docid;
	u64	snapid;
	set<u64>::iterator set_itr = docid_set.begin();
	for(; set_itr != docid_set.end(); set_itr++)
	{
		docid = *set_itr;
		map_itr = mDocidSnapshotMap.find(docid);
		if(map_itr == mDocidSnapshotMap.end())
		{
			OmnAlarm << "this docid not exist!!"
				<< "docid:" << docid
				<< enderr;
			continue;
		}
	
		for(u64 i=0; i<(map_itr->second).size(); i++)
		{
			snapid = (map_itr->second)[i];
			if(snapid == snap_id)
			{
				// this docid has commited.
				mDocidSnapshotMap.erase(map_itr);
			}
		}
	}
	mCommittedDocidsLock->unlock();

	return true;
}

bool
AosBlobSESnapshotMgr::readEntryFromSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id, 
		const u64 docid,
		AosBlobHeaderPtr &header)
{
	aos_assert_r(snap_id !=0, false);
	bool rslt;

	if(mShowLog)
	{
		OmnScreen << "BlobSE; readEntry from snap;"
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< "; docid:" << docid
			<< endl;
	}
	
	AosBlobSESnapshotPtr snapshot;
	if(isCommitted(snap_id))
	{
		snapshot = mCommittedSnapshotsMap[snap_id];
	}
	else
		snapshot = getSnapshotById(snap_id);

	if(!snapshot)
	{
		OmnAlarm << "BlobSE; no snapshot!"
				<< "snap_id:" << snap_id
				<< enderr;
		return false;
	}

	// try read from snapshot.
	rslt = snapshot->readEntry(rdata, docid, header);
	aos_assert_r(rslt, false);

	return true;
}

	
bool
AosBlobSESnapshotMgr::readEntryFromCommittedSnapshot(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosBlobHeaderPtr &header)
{
	vector<u64> snapIdList;
	bool rslt;
	header = 0;

	mCommittedDocidsLock->readlock();
	hash_map<u64, vector<u64> >::iterator  map_itr;
	map_itr = mDocidSnapshotMap.find(docid);
	if(map_itr == mDocidSnapshotMap.end())
	{
		header = 0;
		return true;
	}
	
	snapIdList = map_itr->second;
	for(u64 i=0; i<snapIdList.size(); i++)
	{

		//AosBlobSESnapshotPtr snapshot = getSnapshotById(snapIdList[i]);
		AosBlobSESnapshotPtr snapshot = mCommittedSnapshots[i];
		if(!snapshot)
		{
			mCommittedDocidsLock->unlock();
			OmnAlarm << "BlobSE; no snapshot!"
					<< "snap_id:" << snapIdList[i]
					<< enderr;
			return false;
		}
		mCommittedDocidsLock->unlock();

		rslt = snapshot->readCommittedEntry(rdata, docid, header);
		aos_assert_r(rslt, false);
		// maybe read header failed,
		// when this snapshot has committed finish
		//aos_assert_r(header, false);
		if(!header)	return true;
		
		// maybe bodyseq==0 && bodyoffset==0 means this doc
		// has deleted.
		if(mShowLog)
		{
			OmnScreen << "BlobSE; read from committed snapshot;" 
				<< "; site_id:" << mBlobSE->getConfig().siteid
				<< "; cube_id:" << mBlobSE->getConfig().cubeid
				<< "; aseid:" << mBlobSE->getConfig().aseid
				<< "; snap_id:" << snapIdList[0]
				<< "; docid:" << docid
				<< "; bodySeq:" << header->getBodyRawfid()
				<< "; bodyOff:" << header->getBodyOffset()
				<< endl;
		}
	}
		
	return true;
}

bool
AosBlobSESnapshotMgr::appendEntryToSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id, 
		const u64 docid,
		const AosBlobHeaderPtr &new_header)
{
	aos_assert_r(snap_id !=0, false);
	bool rslt;

	if(mShowLog)
	{
		OmnScreen << "BlobSE; appendDoc to snap;"
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< "; docid:" << docid
			<< "; body_rawfid:" << new_header->getBodyRawfid() 
			<< "; body_offset:" << new_header->getBodyOffset() 
			<< endl;
	}
	
	AosBlobSESnapshotPtr snapshot;
	if(isCommitted(snap_id))
	{
		snapshot = mCommittedSnapshotsMap[snap_id];
	}
	else
		snapshot = getSnapshotById(snap_id);

	if(!snapshot)
	{
		OmnAlarm << "BlobSE; no snapshot!"
				<< "snap_id:" << snap_id
				<< enderr;
		return false;
	}

	AosBlobHeaderPtr old_header;
	rslt = snapshot->appendEntryCheckStatus(
			rdata, docid, new_header, old_header);
	aos_assert_r(rslt, false);

	if(old_header && old_header->getBodyRawfid() != 0 &&
			old_header->getBodyOffset() != 0)
	{
		int rslt = mBlobSE->addToDeleteLog(rdata.getPtr(), old_header);
		aos_assert_r(rslt == 0, false);
	}
	return true;
}


bool
AosBlobSESnapshotMgr::appendEntryToCommittedSnapshot(
		const AosRundataPtr &rdata,
		const u64 docid,
		const AosBlobHeaderPtr &new_header,
		bool &succ)
{
	vector<u64> snapIdList;
	bool rslt;
	succ = false;

	mCommittedDocidsLock->readlock();
	hash_map<u64, vector<u64> >::iterator map_itr;
	map_itr = mDocidSnapshotMap.find(docid);
	if(map_itr == mDocidSnapshotMap.end())
	{
		mCommittedDocidsLock->unlock();
		return true;
	}
	
	snapIdList = map_itr->second;
	for(u64 i=0; i<snapIdList.size(); i++)
	{
		AosBlobSESnapshotPtr snapshot = getSnapshotById(snapIdList[i]);
		if(!snapshot)
		{
			mCommittedDocidsLock->unlock();
			OmnAlarm << "BlobSE; no snapshot!"
					<< "snap_id:" << snapIdList[i]
					<< enderr;
			return false;
		}
		mCommittedDocidsLock->unlock();

		rslt = snapshot->appendExtraEntry(rdata, docid,
				new_header, succ);
		aos_assert_r(rslt, false);
		if(!succ)	return true;

		if(mShowLog)
		{
			OmnScreen << "BlobSE; append to committed snapshot;" 
				<< "; site_id:" << mBlobSE->getConfig().siteid
				<< "; cube_id:" << mBlobSE->getConfig().cubeid
				<< "; aseid:" << mBlobSE->getConfig().aseid
				<< "; snap_id:" << snapIdList[0]
				<< "; docid:" << docid
				<< "; bodySeq:" << new_header->getBodyRawfid()
				<< "; bodyOff:" << new_header->getBodyOffset()
				<< endl;
		}
	}
	return true;
}


////////////////////////////////////////// 
//// snapshot APIs                           
//////////////////////////////////////////// 
AosBlobSESnapshotMgrPtr
AosBlobSESnapshotMgr::createSnapshotMgrStatic(
		const AosRundataPtr &rdata,
		const AosBlobSEPtr &blobSE,
		const AosRawSEPtr &rawSE)
{
	aos_assert_r(blobSE && rawSE, 0);
	AosBlobSESnapshotMgrPtr snapshot_mgr = OmnNew AosBlobSESnapshotMgr(blobSE, rawSE);

	bool rslt = snapshot_mgr->init(rdata);
	aos_assert_r(rslt, 0);
	return snapshot_mgr;
}



bool
AosBlobSESnapshotMgr::createSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	// Ketty 2014/03/05
	aos_assert_r(snap_id , false);
	bool rslt;
	
	if(mShowLog)
	{
		OmnScreen << "BlobSE; create snapshot;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< endl;
	}

	//AosRawFilePtr file1 = getRawFile(rdata, eSnapshotMgrFileID1, true);
	
	mSnapshotsLock->writelock();

	u64 raw_fileid1 = mCrtSnapshotRawFileId;
	u64 raw_fileid2 = mCrtSnapshotRawFileId + 1;
	mCrtSnapshotRawFileId += 2;
	
	AosBlobSESnapshotPtr snapshot;
	snapshot = AosBlobSESnapshot::createSnapshotStatic(
				rdata, mBlobSE->getConfig(), mRawSE,
				snap_id, raw_fileid1, raw_fileid2);
	aos_assert_rl(snapshot, mSnapshotsLock, false);
	
	rslt = addSnapshotLocked(snapshot);
	aos_assert_rl(rslt, mSnapshotsLock, false);
	
	mSnapshotsLock->unlock();

	return true;
}


bool
AosBlobSESnapshotMgr::commitSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	aos_assert_r(snap_id , false);
	
	if(mShowLog)
	{
		OmnScreen << "BlobSE; commit snapshot;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< endl;
	}
	
	AosBlobSESnapshotPtr snapshot = getSnapshotById(snap_id);
	if(!snapshot)
	{
		OmnAlarm << "BlobSE; no snapshot!"
				<< "snap_id:" << snap_id
				<< enderr;
		return false;
	}

	// NNN 0 is time_stamp.
	bool rslt = snapshot->setCommitted(rdata, 0);
	aos_assert_r(rslt, false);
	
	rslt = appendCommittedDocids(rdata, snapshot);
	aos_assert_r(rslt, false);

	//rslt = procCommitReq(rdata, snapshot);
	//aos_assert_r(rslt, false);
	
	//mBlobSE->updateHeader();

	AosProcReq req(snapshot, AosSnapshotStatus::eCommitted);
	rslt = addProcReq(rdata, req);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosBlobSESnapshotMgr::preCommitSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	aos_assert_r(snap_id , false);
	
	if(mShowLog)
	{
		OmnScreen << "BlobSE; precommit snapshot;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< endl;
	}
	
	AosBlobSESnapshotPtr snapshot = getSnapshotById(snap_id);
	if(!snapshot)
	{
		OmnAlarm << "BlobSE; no snapshot!"
				<< "snap_id:" << snap_id
				<< enderr;
		return false;
	}
	
	bool rslt = snapshot->setPreCommitted(rdata);
	aos_assert_r(rslt, false);
// NNNNNNNN

	
	return true;
}


bool
AosBlobSESnapshotMgr::cancelSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	aos_assert_r(snap_id, false);
	
	if(mShowLog)
	{
		OmnScreen << "BlobSE; cancel snapshot;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; snap_id:" << snap_id
			<< endl;
	}
	
	AosBlobSESnapshotPtr snapshot = getSnapshotById(snap_id);
	if(!snapshot)
	{
		OmnAlarm << "BlobSE; no snapshot!"
				<< "snap_id:" << snap_id
				<< enderr;
		return false;
	}
	
	bool rslt = snapshot->setCanceled(rdata);
	aos_assert_r(rslt, false);
	//for one thread
	procCancelReq(rdata, snapshot);

	//for more threads
	/*
	AosProcReq req(snapshot, AosSnapshotStatus::eCanceled);
	rslt = addProcReq(rdata, req);
	aos_assert_r(rslt, false);
	*/
	
	return true;
}


bool
AosBlobSESnapshotMgr::submitSnapshot(
		const AosRundataPtr &rdata,
		const u64 target_snap_id,
		const u64 submit_snap_id)
{
	// merge submit_snap to target_snap.
	aos_assert_r(target_snap_id != 0, false);
	aos_assert_r(submit_snap_id != 0, false);
	if(target_snap_id == submit_snap_id)	return true;

	if(mShowLog)
	{
		OmnScreen << "BlobSE; submit snapshot;" 
			<< "; site_id:" << mBlobSE->getConfig().siteid
			<< "; cube_id:" << mBlobSE->getConfig().cubeid
			<< "; aseid:" << mBlobSE->getConfig().aseid
			<< "; submit_snap_id:" << submit_snap_id
			<< "; target_snap_id:" << target_snap_id
			<< endl;
	}

	AosBlobSESnapshotPtr target_snapshot = getSnapshotById(target_snap_id);
	if(!target_snapshot)
	{
		OmnAlarm << "BlobSE; no target_snap!"
			<< "; target_snap_id:" << target_snap_id
			<< "; submit_snap_id:" << submit_snap_id
			<< enderr;
		return false;
	}
	AosBlobSESnapshotPtr submit_snapshot = getSnapshotById(submit_snap_id);
	if(!submit_snapshot)
	{
		OmnScreen << "BlobSE; snapshot maybe has submit yet."
			<< "; target_snap_id:" << target_snap_id
			<< "; submit_snap_id:" << submit_snap_id
			<< endl;
		return true;
	}

	bool rslt = target_snapshot->addChildSnapshot(rdata, submit_snapshot);
	aos_assert_r(rslt, false);

	rslt = submit_snapshot->setSubmitted(rdata, target_snap_id);
	if(!rslt)
	{
		rslt = target_snapshot->removeChildSnapshot(rdata, submit_snap_id);
		aos_assert_r(rslt, false);
		
		OmnAlarm << "snapshot set status error!"
			<< enderr;
		return false;
	}
	
	AosProcReq req(submit_snapshot, 
			AosSnapshotStatus::eSubmitted, target_snapshot);
	rslt = addProcReq(rdata, req);
	aos_assert_r(rslt, false);

	return true;
}


//////////////////////////////////////////
// Doc APIs
//////////////////////////////////////////
bool
AosBlobSESnapshotMgr::readDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id, 
		const u64 docid,
		AosBlobHeaderPtr &header, //return 
		const u64 timestamp)
{
	bool rslt;
	header = 0;

	if(snap_id != 0)
	{
		rslt = readEntryFromSnapshot(rdata, snap_id, docid, header);
		aos_assert_r(rslt, false);
		
		if(header)	return true;
		// try read header from committed snapshot
	}

	rslt = readEntryFromCommittedSnapshot(rdata, docid, header);
	aos_assert_r(rslt, 0);

	// If header == 0, blobSE will continue read doc.
	return true;
}


bool
AosBlobSESnapshotMgr::saveDoc(
		const AosRundataPtr &rdata,
		const AosBlobHeaderPtr &new_header,
		bool &finish)
{
	const u64 docid = new_header->getDocid();
	const u64 snap_id = new_header->getSnapshotID();

	bool rslt;
	if(snap_id == 0)
	{
		//these doc modifications is not in a transaction
		rslt = appendEntryToCommittedSnapshot(rdata, docid, new_header, finish);	
		aos_assert_r(rslt, false);
		return true;
	}

	finish = true;
	rslt = appendEntryToSnapshot(rdata, snap_id,
			docid, new_header);
	aos_assert_r(rslt, false);
	
	return true;
}

bool
AosBlobSESnapshotMgr::isCommitted(const u64 snap_id)
{
	if(snap_id == 0 && mCommittedSnapshots.size() == 0)
		return false;
	else if(snap_id == 0 && mCommittedSnapshots.size() != 0)
		return true;
	else
	{
		AosBlobSESnapshotPtr snapshot = getSnapshotById(snap_id);
		if(!snapshot && mCommittedSnapshots.size() != 0)
			return true;
		else
			return false;
	}
}

/*
bool
AosBlobSESnapshotMgr::deleteDoc(
		const AosRundataPtr &rdata,
		const u64 snap_id, 
		const u64 docid,
		bool &finish)
{
	bool rslt;

	AosBlobHeaderPtr empty_header = OmnNew AosBlobHeader(docid, 0);
	empty_header->setBodyRawfid(0);
	empty_header->setBodyOffset(0);
	
	if(snap_id == 0)
	{
		rslt = appendEntryToCommittedSnapshot(rdata,
				docid, empty_header, finish);	
		aos_assert_r(rslt, false);
		return true;
	}

	finish = true;
	
	rslt = appendEntryToSnapshot(rdata, snap_id,
			docid, empty_header);
	aos_assert_r(rslt, false);
	
	return true;
}
*/


bool
AosBlobSESnapshotMgr::saveToFile(const AosRundataPtr &rdata)
{
	// Single Thread Safe.
	aos_assert_r(!mIsSaving, false);
	bool rslt;

	mIsSaving = true;
	mSnapshotsLock->readlock();
	
	AosBuffPtr buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	
	//buff->setU32(0);	// file is good flag.
	mCrtSaveSeqno += 1;
	buff->setU64(mCrtSaveSeqno);  // file save seqno.
	buff->setU64(mCrtSnapshotRawFileId);
	
	// save normal snapshots.
	buff->setU32(mSnapshots.size());
	map<u64, AosBlobSESnapshotPtr>::iterator itr;
	AosBlobSESnapshotPtr snapshot; 
	for(itr= mSnapshots.begin(); itr != mSnapshots.end(); itr++)
	{
		snapshot = itr->second;

		rslt = snapshot->saveToFile(rdata);
		aos_assert_rl(rslt, mSnapshotsLock, false);
		
		rslt = snapshot->serializeMetaInfo(rdata, buff);
		aos_assert_rl(rslt, mSnapshotsLock, false);
	}

	// save invalid snapshots.
	buff->setU32(mCanceledSnapshots.size());
	for(u32 i=0; i<mCanceledSnapshots.size(); i++)
	{
		rslt = mCanceledSnapshots[i]->serializeMetaInfo(
				rdata, buff);
		aos_assert_rl(rslt, mSnapshotsLock, false);
	}
	
	// save mProcReqs.
	mProcReqsLock->lock();
	
	if(mIsProcingSnap)
	{
		buff->setU8(1);
		buff->setU32(mIsProcingSnap->getSnapId());
	}
	else
	{
		buff->setU8(0);
	}

	buff->setU32(mProcReqs.size());
	deque<AosProcReq>::iterator d_itr;
	for(d_itr = mProcReqs.begin(); d_itr != mProcReqs.end(); d_itr++)
	{
		buff->setU32((*d_itr).snap->getSnapId());	
	}
	mProcReqsLock->unlock();
	buff->setU32(eFinishFlag);

	// save buff to mgr_file.
	if(!mCrtActiveFile)
	{
		mCrtActiveFile = mMgrFile1;
	}
	else
	{
		mCrtActiveFile = (mCrtActiveFile == mMgrFile1 ? mMgrFile2 : mMgrFile1);
	}

	int file_rslt = mCrtActiveFile->overwrite(
			rdata.getPtr(), buff->data(), buff->dataLen());
	aos_assert_rl(file_rslt == 0, mSnapshotsLock, false);

	u32 good_flag = eFileGoodFlag;
	file_rslt = mCrtActiveFile->append(rdata.getPtr(),
			(char *)&good_flag, 4); // set file is good flag. 
	aos_assert_rl(file_rslt == 0, mSnapshotsLock, false);
		
	// clean invalid Snapshots.
	for(u32 i=0; i<mCanceledSnapshots.size(); i++)
	{
		rslt = mCanceledSnapshots[i]->removeFiles(rdata, 
				mBlobSE->getConfig(), mRawSE);
		aos_assert_rl(rslt, mSnapshotsLock, false);
	}
	mCanceledSnapshots.clear();
	
	mSnapshotsLock->unlock();

	mIsSaving = false;
	return true;
}


bool
AosBlobSESnapshotMgr::ThrdProcer::run()
{
	mMgr->procReq(mRdata, mReq);
	return true;
}


bool
AosBlobSESnapshotMgr::ThrdProcer::procFinished()
{
	mMgr->oneReqProcFinished(mRdata);
	return true;
}


bool
AosBlobSESnapshotMgr::oneReqProcFinished(const AosRundataPtr &rdata)
{
	// ThreadShell's Thread will call this func.
	// so this func can't be blocked.
	// startProcNextReq.

	// when come here. this Proced Snapshot 
	// has add to mCanceledSnapshots
	
	mLock->lock();
	//mProcReqsLock->lock();
	if(mProcReqs.size() == 0)
	{
		mIsProcingSnap = 0;
		//mProcReqsLock->unlock();
		mLock->unlock();
		return true;
	}
	
	AosProcReq req = mProcReqs.front();
	mProcReqs.pop_front();

	bool rslt = startProcReqLocked(rdata, req);
	aos_assert_rl(rslt, mProcReqsLock, false);

	//mProcReqsLock->unlock();
	mLock->unlock();

	return true;
}


bool
AosBlobSESnapshotMgr::procReq(
		const AosRundataPtr &rdata,
		const AosProcReq &req)
{
	switch(req.opr)
	{
	case AosSnapshotStatus::eCommitted:
		 procCommitReq(rdata, req.snap);
		 break;

	case AosSnapshotStatus::eCanceled:
		 procCancelReq(rdata, req.snap);
		 break;

	case AosSnapshotStatus::eSubmitted:
		 procSubmitReq(rdata, req.snap, req.snap2);
		 break;
	
	default:
		 OmnAlarm << "error!" << enderr;
		 break;
	}
	
	return true;
}


AosRawFilePtr
AosBlobSESnapshotMgr::getRawFile(
		const AosRundataPtr &rdata, 
		const u64 raw_fileid, 
		const bool create_flag)
{
	AosRawFilePtr file;
	if (mRawSE->fileExists(rdata.getPtr(), 
				mBlobSE->getConfig().siteid, 
				mBlobSE->getConfig().cubeid,
				mBlobSE->getConfig().aseid, raw_fileid))
	{
		file = mRawSE->getRawFile(rdata.getPtr(), 
				mBlobSE->getConfig().siteid, 
				mBlobSE->getConfig().cubeid,
			 	mBlobSE->getConfig().aseid, 
				raw_fileid, AosRawFile::eReadWrite);
		aos_assert_rr(file.notNull(), rdata, 0);
		return file;
	}
	
	if(!create_flag)	return 0;

	file = mRawSE->createRawFile(rdata.getPtr(), 
			mBlobSE->getConfig().siteid, 
			mBlobSE->getConfig().cubeid,
			mBlobSE->getConfig().aseid, raw_fileid,
			AosMediaType::eDisk, AosRawFile::eReadWrite);
	aos_assert_rr(file.notNull(), rdata, 0);
	
	return file;
}


