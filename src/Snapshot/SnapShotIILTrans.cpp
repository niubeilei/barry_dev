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
#include "Snapshot/SnapShotIILTrans.h"

#include "DfmUtil/DfmDoc.h"
#include "DocFileMgr/DfmLogUtil.h"
#include "DocFileMgr/DfmLogFile.h"
#include "DocFileMgr/DfmLog.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "Snapshot/IILSaveDfmLog.h"
#include "Thread/ThreadPool.h"
#include "Rundata/Rundata.h"

OmnThreadPool sgSnapThreadPool("AosSnapShotIILTrans", __FILE__, __LINE__);

AosSnapShotIILTrans::AosSnapShotIILTrans(
		const u64 snap_id,
		const u32 virtual_id,
		const u32 dfm_id,
		const Status sts,
		const bool show_log)
:
mSnapId(snap_id),
mVirtualId(virtual_id),
mDfmId(dfm_id),
mSts(sts),
mShowLog(show_log)
{
	AosDfmLogUtil::createVidDir(mVirtualId);
	AosDfmLogUtil::createSnapShotDir(mVirtualId, mSnapId);

	u32 file_size = AosDfmLog::getMaxFileSize();
	mDfmLogFile = OmnNew AosDfmLogFile(AosDfmDoc::eIIL, file_size, mVirtualId, mDfmId, 0, mSnapId, mShowLog);
}


AosSnapShotIILTrans::~AosSnapShotIILTrans()
{
}


bool
AosSnapShotIILTrans::addEntry(
		const AosRundataPtr &rdata,
		const AosDfmDocPtr &doc,
		vector<AosTransId> &trans_ids)
{
	return mDfmLogFile->addEntry(rdata, doc, trans_ids);
}


AosDfmDocPtr
AosSnapShotIILTrans::readDoc(
		const AosRundataPtr &rdata,
		const u64 &docid)
{
	return mDfmLogFile->readDoc(rdata, docid);
}


bool
AosSnapShotIILTrans::rollBack(const AosRundataPtr &rdata)
{
	mSts = eRollback;
	OmnScreen << "AAAAAAAAAAA rollBack " << mSnapId << endl;

	AosSnapShotPtr thisptr(this, true);
	OmnThrdShellProcPtr req = OmnNew RollBackTrans(rdata, thisptr);
	sgSnapThreadPool.proc(req);
	return true;
}


bool
AosSnapShotIILTrans::rollBackFinishTransId(const AosRundataPtr &rdata)
{
	bool rslt = procRollBackTransId(rdata->getSiteid());
	aos_assert_r(rslt, false);
	AosIILSaveDfmLog::getSelf()->rollBackCb(rdata, mSnapId, mVirtualId, mDfmId);
	return true;
}


bool
AosSnapShotIILTrans::procRollBackTransId(const u32 siteid)
{
	OmnScreen << "AAAAAAAAAAA procRollBackTransId: " << mSnapId << endl;
	vector<u64> file_ids;
	bool rslt = getFileIdsBySnapShotDir(siteid, file_ids);
	aos_assert_r(rslt, false);

	for (u32 i = 0; i < file_ids.size(); i++)
	{
		rslt = mDfmLogFile->rollBackFinishTrans(file_ids[i]);
		if (!rslt)
		{
			OmnAlarm << "missing " << file_ids[i] << enderr;
		}
	}
	//remove empty directories
	OmnString path = AosDfmLogUtil::getSnapShotFullPath(mVirtualId, mSnapId);
	AosDfmLogUtil::removeEmptyDir(path);
	return true;
}


bool
AosSnapShotIILTrans::removeSnapShot(const AosRundataPtr &rdata)
{
	mSts = eRemoveSnap;
	return rollBack(rdata);
}


bool
AosSnapShotIILTrans::commit(
		const AosRundataPtr &rdata,
		const AosDocFileMgrObjPtr &docFileMgr)
{
	mSts = eCommit;
	return docFileMgr->commitDfmLog(rdata, mDfmLogFile);	
}


bool
AosSnapShotIILTrans::getFileIdsBySnapShotDir(
		const u32 siteid,
		vector<u64> &file_ids)
{
	vector<OmnString> file_path;
	AosDfmLogUtil::getSnapShotDirFileNames(file_path, mSnapId, mVirtualId, siteid, mDfmId);

	sort(file_path.begin(), file_path.end());

	for (u32 i = 0; i < file_path.size(); i++)
	{
		OmnString fname = file_path[i];
		aos_assert_r(fname != "" , false);

		OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
		aos_assert_r(file && file->isGood(), false);

		file->lock();
		u32 dfm_id = file->readBinaryU32(AosDfmLogFile::eDfmIdOff, 0);                           
		u64 file_id = file->readBinaryU64(AosDfmLogFile::eFileIdOff, 0);
		file->unlock();
		aos_assert_r(file_id, false);
		aos_assert_r(dfm_id == mDfmId, false);

		file_ids.push_back(file_id);
	}
	return true;
}


bool
AosSnapShotIILTrans::clearData()
{
	AosDfmLogUtil::removeSnapShotDir(mVirtualId, mSnapId);
	return true;
}


bool
AosSnapShotIILTrans::recoverCommit(const u32 siteid)
{
	vector<u64> file_ids;
	bool rslt = getFileIdsBySnapShotDir(siteid, file_ids);
	aos_assert_r(rslt, false);

	for (u32 i = 0; i < file_ids.size(); i++)
	{
		OmnScreen << "recoverCommit recoverCommit recoverCommit: " << mVirtualId << ";" << file_ids[i] << endl;
		AosDfmLogUtil::moveFile(file_ids[i], mVirtualId);
	}

	//remove empty directories
	OmnString path = AosDfmLogUtil::getSnapShotFullPath(mVirtualId, mSnapId);
	AosDfmLogUtil::removeEmptyDir(path);
	return true;
}


bool
AosSnapShotIILTrans::recoverRollBack(const u32 siteid)
{
	return procRollBackTransId(siteid);
}
