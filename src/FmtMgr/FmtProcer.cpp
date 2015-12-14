////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// 06/17/2013	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "FmtMgr/FmtProcer.h"

#include "API/AosApi.h"
#include "FmtMgr/Fmt.h"
#include "FmtMgr/FmtFile.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/SystemId.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "Util/Buff.h"
#include "Thread/CondVar.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"

AosFmtProcer::AosFmtProcer(
		const AosFmtFilePtr &fmt_file,
		const bool show_log)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mStopCondVar(OmnNew OmnCondVar()),
mFmtFile(fmt_file),
mStoped(false),
//mFinishedFmtId(0),
mCrtAddedReq(-1),
mShowLog(show_log)
{
}


AosFmtProcer::~AosFmtProcer()
{
}


bool
AosFmtProcer::start()
{
	int proced_seq = -1;
	u64 max_fmt_id;
	
	mLock->lock();
	while(1)
	{
		proced_seq = mFmtFile->getNextProcedSeq(proced_seq);
		if(proced_seq == -1)	break;
	
		max_fmt_id = mFmtFile->getMaxFmtIdByFileSeq(proced_seq);
		aos_assert_rl(max_fmt_id, mLock, false);
	
		OmnScreen << "FmtMgr; add proced file:"
			<< "; max_fmt_id:" << max_fmt_id
			<< "; file_seq:" << proced_seq << endl;
		mProcedFileSeqs.insert(make_pair(max_fmt_id, proced_seq));
	}
	mLock->unlock();
	

	OmnThreadedObjPtr thisptr(this, false);
	mProcThrd = OmnNew OmnThread(thisptr, "ProcThrd", 
			eProcThrdId, true, true, __FILE__, __LINE__);
	
	mProcThrd->start();	
	return true;
}


bool
AosFmtProcer::stop()
{
	// when this svr switch to master. need stop the mProcThrd.
	// first. need proc all log finish.
	u64 t1 = OmnGetTimestamp();

	// signal the proc thrd to proc the fmt logs.
	mLock->lock();
	mStoped = true;
	mCondVar->signal();
	mLock->unlock();

	// wait the proc thrd proc finish.
	bool timeout;
	mLock->lock();
	while(mProcThrd->getStatus() == OmnThrdStatus::eActive)
	{
		mStopCondVar->timedWait(mLock, timeout, 10);
	}
	mLock->unlock();

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; stop proc finish."
			<< "; time:" << OmnGetTimestamp() - t1
			<< endl;
	}
	return true;
}


bool
AosFmtProcer::addReq(const u32 seq)
{
	mLock->lock();

	if(mCrtAddedReq >=0 && seq <= (u32)mCrtAddedReq)
	{
		mLock->unlock();
		return true;
		//aos_assert_r(seq > (u32)mCrtAddedReq, false);
	}
	OmnScreen << "sorFmt add Proc Fmt:" << seq << endl;
	mCrtAddedReq = seq;

	mProcQueue.push_back(seq);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosFmtProcer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 thread_id = thread->getLogicId();
	
	if(thread_id == eProcThrdId)
	{
		return procThrdFunc(state, thread);
	}
	
	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosFmtProcer::procThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	OmnScreen << "FmtMgr; proc fmt Thrd, cube_grp_id:" << AosGetSelfCubeGrpId() << endl;
	
	bool rslt;
	int next_seq = -1;
	while(state == OmnThrdStatus::eActive)
	{
		next_seq = getNextProcSeq();
		if(next_seq == -1)
		{
			//	means proc finish.	
			aos_assert_r(mStoped, false);
			break;
		}
		
		rslt = procEachFmtFile(next_seq);	
		aos_assert_r(rslt, false);	
	}

	OmnScreen << "FmtMgr; stop the proc thrd" << endl;

	mLock->lock();
	thread->stop();
	mStopCondVar->signal();
	mLock->unlock();
	
	return true;
}


int
AosFmtProcer::getNextProcSeq()
{
	bool timeout;
	mLock->lock();
	while(!mStoped && mProcQueue.empty())
	{
		mCondVar->timedWait(mLock, timeout, 10);
	}
	
	if(mProcQueue.empty())
	{
		mLock->unlock();
		return -1;
	}

	u32 crt_seq = mProcQueue.front();
	mProcQueue.pop_front();
	mLock->unlock();
	return crt_seq;
}

	
bool
AosFmtProcer::procEachFmtFile(const u32 file_seq)
{
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; ProcFmtFile."
			<< "; seq:" << file_seq 
			<< endl;
	}

	AosBuffPtr cont;
	bool rslt = mFmtFile->readNormFile(file_seq, cont);	
	aos_assert_r(rslt && cont, false);
	
	u64 max_fmt_id = 0;
	while(1)
	{
		AosFmtPtr fmt = AosFmt::getNextFmt(cont);
		if(!fmt) break;
		
		u64 fmt_id = fmt->getFmtId();
		aos_assert_r(max_fmt_id < fmt_id, false);
		max_fmt_id = fmt_id;

		rslt = procFmt(fmt); 
		aos_assert_r(rslt, false);
	}
	
	rslt = mFmtFile->renameToProced(file_seq);
	OmnScreen << "FmtMgr; add proced file, renamed:"
		<< "; max_fmt_id:" << max_fmt_id
		<< "; file_seq:" << file_seq << endl;
	aos_assert_r(rslt, false);

	mLock->lock();
	mProcedFileSeqs.insert(make_pair(max_fmt_id, file_seq));
	mLock->unlock();
	
	return true; 
}


bool
AosFmtProcer::procFmt(const AosFmtPtr &fmt)
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(AOS_SYS_SITEID);

	u64 fmt_id = fmt->getFmtId();
	OmnScreen << "FmtMgr; sorFmt proc Fmt, fmt_id:" << fmt_id << endl;

	while(1)
	{
		AosFmt::FmtOpr opr = fmt->nextFmtOpr();
		if(opr == AosFmt::eInvalid)	return true; 
		
		switch(opr)
		{
		case AosFmt::eCreateFile:
			procCreateFile(rdata, fmt);
			break;

		case AosFmt::eModifyFile:
			procModifyFile(rdata, fmt);
			break;
		
		case AosFmt::eDeleteFile:
			procDeleteFile(rdata, fmt);
			break;

		case AosFmt::eTransIds:
			procTransId(rdata, fmt);
			break;

		default:
			OmnAlarm<< "error!" << enderr;
		}
	}

	return true;
}


bool
AosFmtProcer::procCreateFile(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	AosStorageFileInfo file_info;	
	bool rslt = fmt->getOprInfo(file_info);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; proc Fmt, create_file" 
			<< "; fileId:" << file_info.fileId
			<< "; fname:" << file_info.fname 
			<< "; cube_grp_id:" << AosGetSelfCubeGrpId() 
			<< endl; 
	}

	int filter_deviceid = -1;

	//if(isLocalBackupVFS())
	//{
	//	// This means the backup and the orig file is in the same machines.
	//	// So. the orig and the backup file's device must be different.
	//	filter_deviceid = device_id;
	//}

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	rdata->setSiteid(file_info.siteId);
	rslt = vfsMgr->addFileByFileId(rdata.getPtr(), file_info, 
			filter_deviceid);
	aos_assert_r(rslt, false);
	
	OmnString full_fname = file_info.getFullFname();
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		AosSetErrorU(rdata, "failed_create_file") << ": " << full_fname << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	return true;
}


bool
AosFmtProcer::procModifyFile(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	u64 file_id = 0;
	u64 file_offset = 0;
	AosBuffPtr data;
	bool flushflag = false;	
	
	bool rslt = fmt->getOprInfo(file_id, file_offset, data, flushflag);
	aos_assert_r(rslt && file_id && data, false);

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; proc Fmt, modify_file" 
			<< "; fileId:" << file_id
			<< "; offset:" << file_offset
			<< "; len:" << data->dataLen() 
			<< "; cube_grp_id:" << AosGetSelfCubeGrpId() 
			<< endl; 
	}

	bool find = false;
	AosStorageFileInfo file_info;
	
	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	rslt = vfsMgr->getFileInfo(file_id, file_info, find, rdata.getPtr());
	aos_assert_r(rslt, false);
	if(!find)
	{
		// This means this fileid is new. need create it.
		OmnAlarm << "this fileid is not find: file_id:" << file_id << enderr;
		return false;
	}
	
	OmnString full_fname = file_info.getFullFname();
	OmnFilePtr file = OmnNew OmnFile(full_fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!file->isGood())
	{
		AosSetErrorU(rdata, "failed_create_file") << ": " << full_fname << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	
	rslt = file->put(file_offset, data->data(), data->dataLen(), flushflag); 
	aos_assert_r(rslt, false);
	return true;
}


bool
AosFmtProcer::procDeleteFile(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	u64 file_id;
	bool rslt = fmt->getOprInfo(file_id);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; proc log, delete_file" 
			<< "; cube_grp_id:" << AosGetSelfCubeGrpId() 
			<< "; fileId:" << file_id
			<< endl; 
	}

	AosVfsMgrObjPtr vfsMgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfsMgr, false);
	rslt = vfsMgr->removeFile(file_id, rdata.getPtr());
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosFmtProcer::procTransId(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	vector<AosTransId>	trans_ids;
	bool rslt = fmt->getOprInfo(trans_ids);
	aos_assert_r(rslt, false);

	AosFinishTrans(trans_ids);
	return false;
}


bool
AosFmtProcer::recvFinishedFmtId(const u64 fmt_id)
{
	bool rslt;
	mLock->lock();
	map<u64, u32>::iterator itr = mProcedFileSeqs.begin();
	for(; itr != mProcedFileSeqs.end(); itr++)
	{
		if(itr->second > fmt_id)	break;
		
		// delete file.
		OmnScreen << "FmtMgr; sorFmt. remove Fmt file:"
			<< "; file_seq:" << itr->second<< endl;
		
		rslt = mFmtFile->deleteFile(itr->second); 
		aos_assert_rl(rslt, mLock, false);
	}
	mProcedFileSeqs.erase(mProcedFileSeqs.begin(), itr);
	mLock->unlock();

	return true;
}


