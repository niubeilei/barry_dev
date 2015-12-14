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
// 2015/03/25 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
//
#include "BlobSE/BlobSESnapShot.h"

#include "BlobSE/BlobHeader.h"
#include "Thread/Mutex.h"
#include "Thread/RwLock.h"
#include "RawSE/RawFile.h"
#include "RawSE/RawSE.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"

////////////////////////////////////////// 
//// constructors/destructors                
//////////////////////////////////////////// 
AosBlobSESnapshot::AosBlobSESnapshot(const u64 snap_id)
		//const struct AosBlobSE::Config &blobSE_conf)
:
mSnapId(snap_id),
//mBlobSEConf(blobSE_conf),
mStatusLock(OmnNew OmnRwLock()),
mStatus(AosSnapshotStatus::eActive),
mCommittedTimeStamp(0),
mHeadersLock(OmnNew OmnRwLock()),
mExtraHeadersLock(OmnNew OmnRwLock()),
mExtraEntryCommitted(false),
mChildSnapsLock(OmnNew OmnMutex()),
mParentSnapId(0)
{
}


AosBlobSESnapshot::~AosBlobSESnapshot()
{
}

AosBlobSESnapshotPtr
AosBlobSESnapshot::createSnapshotStatic(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE,
		const u64 snap_id,
		const u64 raw_fileid_1,
		const u64 raw_fileid_2)
{
	aos_assert_r(snap_id, 0);
	aos_assert_r(raw_fileid_1 && raw_fileid_2, 0);
	
	AosBlobSESnapshotPtr snapshot = OmnNew AosBlobSESnapshot(snap_id);
	//		snap_id, blobSE_conf);
	bool rslt = snapshot->init(rdata, blobSE_conf,
			rawSE, raw_fileid_1, raw_fileid_2);
	aos_assert_r(rslt, 0);
	
	return snapshot;
}


AosBlobSESnapshotPtr
AosBlobSESnapshot::createSnapshotStatic(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE,
		const AosBuffPtr &buff)
{
	aos_assert_r(buff, 0);
	
	u64 snap_id = buff->getU32(0);
	u64 raw_fileid_1 = buff->getU64(0);
	u64 raw_fileid_2 = buff->getU64(0);
	aos_assert_r(snap_id, 0);
	aos_assert_r(raw_fileid_1 && raw_fileid_2, 0);
	
	u64 crt_active_fileid = buff->getU64(0); 
	
	AosBlobSESnapshotPtr snapshot = OmnNew AosBlobSESnapshot(snap_id);
	//		snap_id, blobSE_conf);
	bool rslt = snapshot->init(rdata, blobSE_conf, rawSE, 
			raw_fileid_1, raw_fileid_2, crt_active_fileid);
	aos_assert_r(rslt, 0);	
	
	return snapshot;
}

//////////////////////////////////////////       
// getters/setters                               
//////////////////////////////////////////       
bool
AosBlobSESnapshot::setCommitted(
		const AosRundataPtr &rdata,
		const u32 commit_time_stamp)
{
	mStatusLock->writelock();
	
	//if(mStatus != AosSnapshotStatus::eActive || 
	//		mStatus != AosSnapshotStatus::ePreCommitted)
	if(mStatus != AosSnapshotStatus::eActive)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId 
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	mStatus = AosSnapshotStatus::eCommitted;
	mCommittedTimeStamp = commit_time_stamp;
	
	mStatusLock->unlock();
	return true;
}

bool
AosBlobSESnapshot::setPreCommitted(const AosRundataPtr &rdata)
{
	mStatusLock->writelock();
	
	if(mStatus != AosSnapshotStatus::eActive)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId 
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	mStatus = AosSnapshotStatus::ePreCommitted;
	
	mStatusLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::setCanceled(const AosRundataPtr &rdata)
{
	mStatusLock->writelock();
	
	if(mStatus != AosSnapshotStatus::eActive)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId 
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	//mStatus = AosSnapshotStatus::eRollbacked;
	mStatus = AosSnapshotStatus::eCanceled;
	
	mStatusLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::setSubmitted(
		const AosRundataPtr &rdata,
		const u32 p_snap_id)
{
	mStatusLock->writelock();
	
	if(mStatus != AosSnapshotStatus::eActive)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	mStatus = AosSnapshotStatus::eSubmitted;

	mParentSnapId = p_snap_id;

	mStatusLock->unlock();
	return true;
}



//////////////////////////////////////////
// Doc/header APIs
//////////////////////////////////////////
bool
AosBlobSESnapshot::readEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosBlobHeaderPtr &header)
{
	header = 0;
	
	mStatusLock->readlock();
	
	if(mStatus == AosSnapshotStatus::eCanceled)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId 
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	mHeadersLock->readlock();
	map<u64, AosBlobHeaderPtr>::iterator itr;
	itr = mDocidHeaderMap.find(docid);
	
	if(itr != mDocidHeaderMap.end())
	{
		header = itr->second;
	}
	mHeadersLock->unlock();
	
	mStatusLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::appendEntryCheckStatus(
		const AosRundataPtr &rdata,
		const u64 docid,
		const AosBlobHeaderPtr &new_header,
		AosBlobHeaderPtr &old_header)
{
	// deleteDoc is also en entry.
	
	old_header = 0;
	
	mStatusLock->readlock();
	if(mStatus != AosSnapshotStatus::eActive) 
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; snapshot is not active!"
			<< "snap_id:" << mSnapId
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	bool rslt = appendEntry(rdata, docid,
			new_header, old_header);
	aos_assert_rl(rslt, mStatusLock, false);

	mStatusLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::appendEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		const AosBlobHeaderPtr &new_header,
		AosBlobHeaderPtr &old_header)
{
	// when submit snapshot, target_snapshot will call this func.

	old_header = 0;
	
	mHeadersLock->writelock();
	map<u64, AosBlobHeaderPtr>::iterator itr;
	itr = mDocidHeaderMap.find(docid);

	if(itr != mDocidHeaderMap.end())
	{
		old_header = itr->second;
		itr->second = new_header;	
		
		//add old_header to deletedHeaders
		mDeletedHeaders.push_back(old_header);

		mHeadersLock->unlock();
		return true;
	}
	mDocidHeaderMap.insert(make_pair(new_header->getDocid(), new_header));  

	//add old_header to deletedHeaders
	//mDeletedHeaders.push_back(old_header);
	
	mHeadersLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::readCommittedEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		AosBlobHeaderPtr &header)
{
	header = 0;

	// when come here mStatus must be eCommitted.
	// other therad will not change this.
	// so not need mStatusLock.
	if(mStatus != AosSnapshotStatus::eCommitted)
	{
		OmnAlarm << "BlobSE; snapshot is not committed!"
			<< "snap_id:" << mSnapId
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}

	// 1. find from Extra Headers.
	mExtraHeadersLock->readlock();

	map<u64, u32>::iterator itr = mExtraDocids.find(docid);
	if(itr != mExtraDocids.end())
	{
		u32 idx = itr->second;
		aos_assert_rl(idx < mExtraHeaders.size(), mExtraHeadersLock, false);
		
		header = mExtraHeaders[idx];
		mExtraHeadersLock->unlock();
		return true;
	}
	mExtraHeadersLock->unlock();
	
	// 2. find from mDocidHeaderMap.
	// maybe the houseKeepThrd is submitting snapshot.
	// target->appendEntry(). so mHeadersLock->readLock()
	mHeadersLock->readlock();
	map<u64, AosBlobHeaderPtr>::iterator itr2;
	itr2 = mDocidHeaderMap.find(docid);
	if(itr2 != mDocidHeaderMap.end())
	{
		header = itr2->second;
	}
	mHeadersLock->unlock();

	return true;
}


bool
AosBlobSESnapshot::readAllDocids(
		const AosRundataPtr &rdata,
		set<u64> &docid_set)
{
	// only HouseKeeping Thrd will call this.	
	if(!mAllDocids.size())
	{
		bool rslt;
		
		mHeadersLock->readlock();
		set<u64>::iterator set_itr = mAllDocids.begin();
		map<u64, AosBlobHeaderPtr>::iterator map_itr;
		for(map_itr = mDocidHeaderMap.begin(); map_itr != mDocidHeaderMap.end(); map_itr++)
		{
			set_itr = mAllDocids.insert(set_itr, map_itr->first);
		}
		mHeadersLock->unlock();
		
		mChildSnapsLock->lock();
		map<u32, AosBlobSESnapshotPtr>::iterator c_itr;
		for(c_itr = mChildSnaps.begin(); c_itr != mChildSnaps.end(); c_itr++)
		{
			rslt = c_itr->second->readAllDocids(rdata, mAllDocids);
			aos_assert_rl(rslt, mChildSnapsLock, false);
		}
		mChildSnapsLock->unlock();
	}

	// docid_set may not empty.
	docid_set.insert(mAllDocids.begin(), mAllDocids.end());
	return true;
}


bool
AosBlobSESnapshot::readAllEntrys(
		const AosRundataPtr &rdata,
		vector<AosBlobHeaderPtr> &vt_headers)
{
	// only HouseKeeping Thrd will call this.	
	aos_assert_r(mChildSnaps.size() == 0, false);

	mHeadersLock->readlock();
	map<u64, AosBlobHeaderPtr>::iterator itr;
	for(itr = mDocidHeaderMap.begin(); itr != mDocidHeaderMap.end(); itr++)
	{
		vt_headers.push_back(itr->second);	
	}
	
	mHeadersLock->unlock();
	return true;
}





bool
AosBlobSESnapshot::serializeMetaInfo(
		const AosRundataPtr &rdata,
		const AosBuffPtr &buff)
{
	aos_assert_r(buff, false);
	
	buff->setU32(mSnapId);
	buff->setU64(mLogFile1->getRawFileID());
	buff->setU64(mLogFile2->getRawFileID());

	aos_assert_r(mCrtActiveFile, false);
	buff->setU64(mCrtActiveFile->getRawFileID());
	
	return true;
}


//////////////////////////////////////////
// Extra Doc/header APIs
//////////////////////////////////////////
void
AosBlobSESnapshot::startCommitExtraEntry()
{
	mExtraHeadersLock->readlock();
}


bool
AosBlobSESnapshot::readAllExtraEntrys(
		const AosRundataPtr &rdata,
		vector<AosBlobHeaderPtr> &vt_headers)
{
	// when call this func. the caller must first call
	// startCommitExtraEntry. then call commitExtraEntryFinish.
	aos_assert_r(mChildSnaps.size() == 0, false);
	
	// only HouseKeeping Thrd will call this.	
	for(u32 i=0; i<mExtraHeaders.size(); i++)
	{
		vt_headers.push_back(mExtraHeaders[i]);
	}
	return true;
}


void
AosBlobSESnapshot::commitExtraEntryFinish()
{
	mExtraEntryCommitted = true;
	mExtraHeadersLock->unlock();
}

	
bool
AosBlobSESnapshot::appendExtraEntry(
		const AosRundataPtr &rdata,
		const u64 docid,
		const AosBlobHeaderPtr &new_header,
		bool &succ)
{
	// this new_header has set time_stamp.
	// deleteDoc is also en entry.

	// when come here mStatus must be eCommitted.
	// other therad will not change this.
	// so not need mStatusLock.
	if(mStatus != AosSnapshotStatus::eCommitted)
	{
		OmnAlarm << "BlobSE; snapshot is not committed!"
			<< "snap_id:" << mSnapId 
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< enderr;
		return false;
	}
	
	mExtraHeadersLock->writelock();
	if(mExtraEntryCommitted)
	{
		// means This snapshot has committed finish.
		mExtraHeadersLock->unlock();
		succ = false;
		return true;
	}
	
	mExtraDocids[docid] = mExtraHeaders.size();
	mExtraHeaders.push_back(new_header);
	mExtraHeadersLock->unlock();
	
	succ = true;
	return true;
}


//////////////////////////////////////////
// parent/child snapshots
//////////////////////////////////////////
bool
AosBlobSESnapshot::addChildSnapshot(
		const AosRundataPtr &rdata,
		const AosBlobSESnapshotPtr &submit_snap)
{
	u32 submit_snap_id = submit_snap->getSnapId();
	mStatusLock->readlock();
	
	if(mStatus != AosSnapshotStatus::eActive)
	{
		mStatusLock->unlock();
		OmnAlarm << "BlobSE; target snapshot is not active!"
			<< "; snap_id:" << mSnapId
			<< "; sts:" << AosSnapshotStatus::toStr(mStatus)
			<< "; submit_snap_id:" << submit_snap_id
			<< enderr;
		return false;
	}

	mChildSnapsLock->lock();
	map<u32, AosBlobSESnapshotPtr>::iterator itr;
	itr = mChildSnaps.find(submit_snap_id);
	if(itr != mChildSnaps.end())
	{
		mChildSnapsLock->unlock();
		mStatusLock->unlock();
		
		OmnAlarm << "BlobSE; snapshot already has this child snap! " 
			<< "mSnapId:" << mSnapId
			<< "; child_snapid:" << submit_snap_id
			<< enderr;
		return false;
	}

	mChildSnaps.insert(make_pair(submit_snap_id, submit_snap));
	mChildSnapsLock->unlock();
		
	mStatusLock->unlock();
	return true;
}


bool
AosBlobSESnapshot::removeChildSnapshot(
		const AosRundataPtr &rdata,
		const u64 snap_id)
{
	// HouseKeepingThrd will call this.
	// not need check mStatus.
	
	mChildSnapsLock->lock();
	map<u32, AosBlobSESnapshotPtr>::iterator itr = mChildSnaps.find(snap_id);
	if(itr == mChildSnaps.end())
	{
		mChildSnapsLock->unlock();
		
		OmnAlarm << "BlobSE; snapshot don't has this child snap! " 
			<< "mSnapId:" << mSnapId
			<< "; child_snapid:" << snap_id
			<< enderr;
		return false;
	}

	mChildSnaps.erase(itr);
	mChildSnapsLock->unlock();

	return true;
}


//////////////////////////////////////////
//// File APIs                              
////////////////////////////////////////////

//save headers to file. Now for now
bool
AosBlobSESnapshot::saveToFile(const AosRundataPtr &rdata)
{
	// This func will save headers to log file.
	bool rslt;
	AosBuffPtr buff = OmnNew AosBuff(200 AosMemoryCheckerArgs); 
	
	mStatusLock->readlock();
	
	buff->setU32(mStatus);
	buff->setU32(mCommittedTimeStamp);
	buff->setU32(mParentSnapId);

	// not need, Mgr init mProcReqs will restruct 
	// this.
	//mChildSnapsLock->lock();
	//buff->setU32(mChildSnaps.size());
	//map<u32, AosBlobSESnapshotPtr>::iterator itr = mChildSnaps.begin();
	//for(; itr != mChildSnaps.end(); itr++)
	//{
	//	buff->setU32(itr->first);	
	//}
	//mChildSnapsLock->unlock();

	// save Headers.
	mHeadersLock->readlock();
	buff->setU32(mDocidHeaderMap.size());

	AosBlobHeaderPtr header;
	u32 expect_len = 0;
	map<u64, AosBlobHeaderPtr>::iterator itr2 = mDocidHeaderMap.begin();
	for(; itr2 != mDocidHeaderMap.end(); itr2++)
	{
		header = itr2->second;

		expect_len = buff->dataLen() + header->getHeaderInCacheAndHeaderFileLength();
		if(expect_len >= buff->buffLen())
		{
			buff->expandMemory1(expect_len + 200);
		}

		rslt = itr2->second->serializeToCacheAndHeaderFileBuff(
				buff->data() + buff->dataLen());
		if(!rslt)
		{
			OmnAlarm << "Internal Error!" << enderr;
			mHeadersLock->unlock();
			mStatusLock->unlock();
			return false;
		}

		buff->setDataLen(expect_len); 
	}
	mHeadersLock->unlock();
	
	if(mStatus == AosSnapshotStatus::eCommitted)
	{
		// save extra headers.
		mExtraHeadersLock->readlock();
		buff->setU32(mExtraHeaders.size());
		for(u32 i=0; i< mExtraHeaders.size(); i++)
		{
			expect_len = buff->dataLen() + 
				mExtraHeaders[i]->getHeaderInCacheAndHeaderFileLength();
			if(expect_len >= buff->buffLen())
			{
				buff->expandMemory1(expect_len + 200);
			}
		
			rslt = mExtraHeaders[i]->serializeToCacheAndHeaderFileBuff(
					buff->data() + buff->dataLen());
			if(!rslt)
			{
				OmnAlarm << "Internal Error!" << enderr;
				mExtraHeadersLock->unlock();
				mStatusLock->unlock();
				return false;
			}
			
			buff->setDataLen(expect_len); 
		}
		mExtraHeadersLock->unlock();
	}

	mStatusLock->unlock();
	
	if(!mCrtActiveFile)
	{
		mCrtActiveFile = mLogFile1;
	}
	else
	{
		mCrtActiveFile = (mCrtActiveFile == mLogFile1 ? mLogFile2 : mLogFile1);
	}

	// WWWW 
	//mCrtActiveFile->resetFile();
	//rslt = mCrtActiveFile->put(0, buff->data(), buff->dataLen(), true);
	//aos_assert_r(rslt, false);
	
	int file_rslt = mCrtActiveFile->overwrite(
			rdata.getPtr(), buff->data(),buff->dataLen());
	aos_assert_r(file_rslt == 0, false);

	return true;
}


bool
AosBlobSESnapshot::removeFilesStatic(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE,
		const AosBuffPtr &buff,
		u64 &snap_id)
{
	aos_assert_r(buff, false);
	
	snap_id = buff->getU32(0);
	aos_assert_r(snap_id, false);
	u64 raw_fileid_1 = buff->getU64(0);
	u64 raw_fileid_2 = buff->getU64(0);
	aos_assert_r(snap_id, 0);
	aos_assert_r(raw_fileid_1 && raw_fileid_2, false);
	
	u64 crt_active_fileid = buff->getU64(0); 
	aos_assert_r(crt_active_fileid, false);
	
	int int_rslt = rawSE->deleteFile(rdata.getPtr(), blobSE_conf.siteid, 
				blobSE_conf.cubeid, blobSE_conf.aseid,
				raw_fileid_1, AosRawFile::eReadWrite);
	if(int_rslt != 0)
	{
		OmnAlarm << "Delete File Error!"
			<< "; raw_fileid:" << raw_fileid_1
			<< enderr;
		return false;
	}
	
	int_rslt = rawSE->deleteFile(rdata.getPtr(), blobSE_conf.siteid, 
				blobSE_conf.cubeid, blobSE_conf.aseid,
				raw_fileid_2, AosRawFile::eReadWrite);
	if(int_rslt != 0)
	{
		OmnAlarm << "Delete File Error!"
			<< "; raw_fileid:" << raw_fileid_1
			<< enderr;
		return false;
	}

	return true;
}



bool
AosBlobSESnapshot::removeFiles(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE)
{
	int int_rslt = rawSE->deleteFile(rdata.getPtr(), blobSE_conf.siteid, 
				blobSE_conf.cubeid, blobSE_conf.aseid,
				mLogFile1->getRawFileID(), AosRawFile::eReadWrite);
	if(int_rslt != 0)
	{
		OmnAlarm << "Delete File Error!"
			<< "; raw_fileid:" << mLogFile1->getRawFileID() 
			<< enderr;
		return false;
	}
	
	int_rslt = rawSE->deleteFile(rdata.getPtr(), blobSE_conf.siteid, 
				blobSE_conf.cubeid, blobSE_conf.aseid,
				mLogFile2->getRawFileID(), AosRawFile::eReadWrite);
	if(int_rslt != 0)
	{
		OmnAlarm << "Delete File Error!"
			<< "; raw_fileid:" << mLogFile2->getRawFileID() 
			<< enderr;
		return false;
	}

	return true;
}


//////////////////////////////////////////
// internal File APIs
//////////////////////////////////////////
bool
AosBlobSESnapshot::init(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE,
		const u64 raw_fileid_1,
		const u64 raw_fileid_2,
		const u64 crt_active_fileid)
{
	if(crt_active_fileid != raw_fileid_1 &&
			crt_active_fileid != raw_fileid_2)
	{
		OmnAlarm << "Invalid file_id:" << crt_active_fileid
				<< "; mLogFile1:" << mLogFile1->getRawFileID() 
				<< "; mLogFile2:" << mLogFile2->getRawFileID() 
				<< enderr;
		return false;
	}
	
	mLogFile1 = getRawFile(rdata, rawSE,
			blobSE_conf, raw_fileid_1, true);
	aos_assert_r(mLogFile1, false);
	
	mLogFile2 = getRawFile(rdata, rawSE,
			blobSE_conf, raw_fileid_2, true);
	aos_assert_r(mLogFile2, false);
	
	mCrtActiveFile = (crt_active_fileid == raw_fileid_1 ? mLogFile1 : mLogFile2);

	bool rslt = initFromFile(rdata, mCrtActiveFile);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosBlobSESnapshot::init(
		const AosRundataPtr &rdata,
		const struct AosBlobSE::Config &blobSE_conf,
		const AosRawSEPtr &rawSE,
		const u64 raw_fileid_1,
		const u64 raw_fileid_2)
{
	mLogFile1 = getRawFile(rdata, rawSE,
			blobSE_conf, raw_fileid_1, true);
	aos_assert_r(mLogFile1, false);
	
	mLogFile2 = getRawFile(rdata, rawSE,
			blobSE_conf, raw_fileid_2, true);
	aos_assert_r(mLogFile2, false);
	
	return true;
}


AosRawFilePtr
AosBlobSESnapshot::getRawFile(
		const AosRundataPtr &rdata, 
		const AosRawSEPtr &rawSE,
		const struct AosBlobSE::Config &blobSE_conf,
		const u64 raw_fileid, 
		const bool create_flag)
{
	aos_assert_r(rawSE, 0);
	AosRawFilePtr file;
	if (rawSE->fileExists(rdata.getPtr(), blobSE_conf.siteid, 
				blobSE_conf.cubeid, blobSE_conf.aseid, raw_fileid))
	{
		file = rawSE->getRawFile(rdata.getPtr(), blobSE_conf.siteid, blobSE_conf.cubeid,
				blobSE_conf.aseid, raw_fileid, AosRawFile::eReadWrite);
		aos_assert_rr(file.notNull(), rdata, 0);
		return file;
	}
	
	if(!create_flag)	return 0;

	file = rawSE->createRawFile(rdata.getPtr(), blobSE_conf.siteid, blobSE_conf.cubeid,
		blobSE_conf.aseid, raw_fileid, AosMediaType::eDisk, AosRawFile::eReadWrite);
	aos_assert_rr(file.notNull(), rdata, 0);
	
	return file;
}



bool
AosBlobSESnapshot::initFromFile(
		const AosRundataPtr &rdata,
		const AosRawFilePtr &file)
{
	aos_assert_r(file, false);

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

	mStatus = (AosSnapshotStatus::E)buff->getU32(0); 
	mCommittedTimeStamp = buff->getU32(0);
	mParentSnapId = buff->getU32(0);

	// not need, Mgr init mProcReqs will restruct 
	//u32 child_snap_num = buff->getU32(0);
	//for(u32 i=0; i<child_snap_num; i++)
	//{
	//	u32 snap_id = buff->getU32(0);
	//	//AosBlobSESnapshotPtr snap = mgr->retrieveSnapshot(rdata, snap_id);
	// //aos_assert_r(snap, false);
	//	AosBlobSESnapshotPtr empty_snap;
	//	mChildSnaps.insert(make_pair(snap_id, empty_snap));
		// NNN
	//}
	
	u32 header_num = buff->getU32(0);
	AosBlobHeaderPtr header;
	for(u32 i=0; i<header_num; i++)
	{
		OmnNotImplementedYet;
		//header = AosBlobHeader::serializeFrom(buff);
		aos_assert_r(header, false);
		mDocidHeaderMap.insert(make_pair(header->getDocid(), header));	
	}

	if(mStatus == AosSnapshotStatus::eCommitted)
	{
		u32 extra_header_num = buff->getU32(0);	
		for(u32 i=0; i<extra_header_num; i++)
		{
			OmnNotImplementedYet;
			//header = AosBlobHeader::serializeFrom(rdata, buff);
			aos_assert_r(header, false);
			mExtraDocids.insert(make_pair(header->getDocid(), mExtraHeaders.size())); 
			mExtraHeaders.push_back(header);
		}
	}

	return true;
}

bool
AosBlobSESnapshot::releaseDeletedHeaders()
{
	//release the body and header used by the old header
	//int rslt = mBlobSE->addToDeleteLog(rdata.getPtr(), old_header);
	//aos_assert_r(rslt == 0, false);
	return true;
}

