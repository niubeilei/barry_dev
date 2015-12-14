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
#include "FmtMgr/BkpFmtSvr.h"

#include "FmtMgr/Fmt.h"
#include "FmtMgr/FmtProcer.h"
#include "FmtMgr/FmtFile.h"
#include "FmtMgr/FmtIdMgr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/SystemId.h"
#include "TransBasic/AppMsg.h"
#include "Util/TransId.h"
#include "Util/Buff.h"

#include "SysMsg/NextSendFmtIdTrans.h"	
#include "SysMsg/ReadFmtTrans.h"	

AosBkpFmtSvr::AosBkpFmtSvr(const int crt_master)
:
AosFmtSvr(false),
mLock(OmnNew OmnMutex()),
mCrtMaster(crt_master),
mMaxFmtIdInFile(0),
mSyncFinish(false),
mRealSyncEnd(0),
mCrtSyncEnd(0),
mCrtRecvFmtId(0)
{
}

	
AosBkpFmtSvr::~AosBkpFmtSvr()
{
}

bool
AosBkpFmtSvr::config(const AosXmlTagPtr &conf)
{
	bool rslt = AosFmtSvr::config(conf);
	aos_assert_r(rslt, false);
	
	mMaxFmtIdInFile = getMaxFmtIdInFile();
	
	mFmtProc = OmnNew AosFmtProcer(mFmtFile, mShowLog);
	
	//OmnThreadedObjPtr thisptr(this, false);
	//mSyncThrd = OmnNew OmnThread(thisptr, "SyncThrd", 
	//		eSyncThrdId, true, true, __FILE__, __LINE__);
	
	initSvrIds();
	
	return true;
}


bool
AosBkpFmtSvr::initSvrIds()
{
	int svr_id = -1;
	SyncedSvrInfo svr_info;
	while(1)
	{
		svr_id = getNextSyncSvr(svr_id);
		if(svr_id == -1)	break;
		
		svr_info.mSvrId = svr_id;
		mSyncSvrs.push_back(svr_info);
	}
	return true;
}

int
AosBkpFmtSvr::getNextSyncSvr(const int crt_sid)
{
	// first sync from bkp. if not succ. sync from mCrtMaster.
	if(crt_sid == mCrtMaster)
	{
		// the mCrtMaster is the last sync.
		return -1;
	}

	int next_sid = crt_sid;
	while(1)
	{
		next_sid = AosGetNextSvrId(AosGetSelfCubeGrpId(), next_sid);
	
		if(next_sid != AosGetSelfServerId() && next_sid!= mCrtMaster) break;
	}

	if(next_sid == -1)
	{
		// try to mCrtMaster
		next_sid = mCrtMaster;
	}
	return next_sid;
}


bool
AosBkpFmtSvr::stop()
{
	mLock->lock();
	bool rslt = addProcSeqLocked();
	mLock->unlock();

	aos_assert_r(rslt, false);
	return mFmtProc->stop();
}


bool
AosBkpFmtSvr::syncFmt()
{
	if(mRealSyncEnd == 0)
	{
		u64 master_next_send = askMasterNextSendFmtId();
		aos_assert_r(master_next_send, false);
		
		// when come here. mRealSyncEnd maybe has inited by (recvFmt).
		aos_assert_r(mRealSyncEnd <= master_next_send, false);
		if(mRealSyncEnd == 0)	mRealSyncEnd = master_next_send; 
	}

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; SWH; start syncFmt"
			<< "; sync_end:" << mRealSyncEnd 
			<< "; crt_recv:" << mMaxFmtIdInFile 
			<< endl;
	}
	
	bool rslt = syncFmtPriv();
	aos_assert_r(rslt, false);

	mFmtProc->start();

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; SWH; syncFmt finish." << endl;
	}
	return true;
}


u32
AosBkpFmtSvr::askMasterNextSendFmtId()
{
	AosTransPtr trans = OmnNew AosNextSendFmtIdTrans(
			mCrtMaster, AosGetSelfCubeGrpId());

	bool svr_death;
	AosBuffPtr resp;
	bool rslt = AosSendTrans(mRdata, trans, resp, svr_death);
	aos_assert_r(rslt, 0);
	if(svr_death)
	{
		OmnAlarm << "master svr just down. master_svr:" << mCrtMaster
			<< "; cube_grp_id:" << AosGetSelfCubeGrpId() 
			<< enderr;
		return 0;
	}

	aos_assert_r(resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	u64 next_fmt_id = resp->getU64(0);
	return next_fmt_id;
}


bool
AosBkpFmtSvr::syncFmtPriv()
{
	// maybe expect_sync_end == 0. because mFirstRecvFmtId maybe == 0.
	u64 sync_beg = getMaxFmtIdInFile() + 1;
	
	u64 expect_sync_end;
	getNextExpectSyncEnd(expect_sync_end);

	bool rslt, sync_succ;
	u32 idx = 0;
	while(idx < mSyncSvrs.size())
	{
		rslt = syncFmtFromSvr(mSyncSvrs[idx], sync_beg,
				expect_sync_end, sync_succ);
		aos_assert_r(rslt, false);
		
		if(!sync_succ)
		{
			// maybe svr death or this svr has no more fmts.
			idx++;
			sync_beg = mCrtSyncEnd + 1; 	// maybe has synced some fmt.
			continue;
		}
		
		if(expect_sync_end == mRealSyncEnd)	break;

		// merge the log files.
		rslt = mFmtFile->mergeNextAreaFiles();
		aos_assert_r(rslt, false);

		sync_beg = mCrtSyncEnd + 1;
		getNextExpectSyncEnd(expect_sync_end);
		continue;
	}
	
	mLock->lock();
	mSyncFinish = true;
	rslt = mFmtFile->mergeNextAreaFiles();
	aos_assert_r(rslt, false);
	
	addProcSeqLocked();
	mLock->unlock();

	if(expect_sync_end != mRealSyncEnd)
	{
		//OmnAlarm << "Sync Data failed; "
		//  << "; cube_grp_id: " << mCubeGrpId
		//  << "; sync_beg:" << sync_beg
		//  << "; mSyncEnd:" << mRealSyncEnd << enderr;
		//return false;
	}
	return true;
}


bool
AosBkpFmtSvr::addProcSeqLocked()
{
	int act_seq = mFmtFile->activeSeq();
	int proc_seq = mFmtFile->endProcedSeq();
	while(1)
	{
		proc_seq = mFmtFile->getNextSeqno(proc_seq);
		aos_assert_r(proc_seq >=-1, false);
		if(proc_seq == -1)	break;
	
		if(proc_seq == act_seq) mFmtFile->startNextActive();

		mFmtProc->addReq(proc_seq);
	}

	return true;	
}


bool
AosBkpFmtSvr::getNextExpectSyncEnd(u64 &expect_sync_end)
{
	u64 merge_fmt_beg = getMergeBegFmtId();
	if(merge_fmt_beg != 0)
	{
		expect_sync_end = merge_fmt_beg;
		return true;
	}

	expect_sync_end = mRealSyncEnd;
	return true;
}


u64
AosBkpFmtSvr::getMergeBegFmtId()
{
	int beg_file_seq = mFmtFile->mergeBegSeq();
	if(beg_file_seq == -1)	return 0;

	AosBuffPtr cont;
	bool rslt = mFmtFile->readMergeFile(beg_file_seq, cont);	
	aos_assert_r(rslt && cont, 0);
	
	// get first fmt_id.
	u32 fmt_len = cont->getU32(0);
	aos_assert_r(fmt_len, 0);

	u64 beg_fmt_id = AosFmt::getFmtIdFromBuff(cont);
	aos_assert_r(beg_fmt_id > 0, 0);
	
	return beg_fmt_id;
}


bool
AosBkpFmtSvr::syncFmtFromSvr(
		SyncedSvrInfo &svr,
		const u64 sync_beg,
		const u64 expect_sync_end,
		bool &sync_succ)
{
	// this func will change sync_beg.
	// read the fmt log from the remote server.
	
	OmnScreen << "FmtMgr; SWH; start sync fmt"
		<< "; from_svr:" << svr.mSvrId
		<< "; sync_beg:" << sync_beg
		<< "; expect_sync_end:" << expect_sync_end
		<< endl;
	
	sync_succ = false;
	
	bool rslt;
	AosBuffPtr resp;
	int next_read_file_seq = svr.mCrtReadFileSeq;
	while(1)
	{
		rslt = sendReadFmtTrans(svr.mSvrId, next_read_file_seq, sync_beg,
				expect_sync_end, svr.mSvrDeathed, resp);
		aos_assert_r(rslt, false);
		if(svr.mSvrDeathed)
		{
			OmnScreen << "FmtMgr; svr death. can't sync fmt from svr:" << svr.mSvrId << endl;
			return true;
		}
		
		svr.mCrtReadFileSeq = next_read_file_seq;
		svr.mReadFinish = resp->getU8(0);
		next_read_file_seq = resp->getInt(-1);
		rslt = recvFmtBySync(resp, expect_sync_end, sync_succ);
		aos_assert_r(rslt, false);

		if(sync_succ)	break;

		if(!svr.mReadFinish)
		{
			aos_assert_r(next_read_file_seq >= 0, false);
			continue;
		}

		OmnScreen << "FmtMgr; SWH; svr has sync finish, svr_id::" << svr.mSvrId
			<< "; but need continue. "
			<< "; crt_sync_end:" << mCrtSyncEnd 
			<< endl;
		return true;
	}
	
	OmnScreen << "FmtMgr; SWH; sync fmt succ"
		<< "; from_svr:" << svr.mSvrId
		<< "; sync_beg:" << sync_beg
		<< "; expect_sync_end:" << expect_sync_end
		<< endl;
	return true;
}


bool
AosBkpFmtSvr::sendReadFmtTrans(
		const int svr_id,
		const int file_seq,
		const u64 sync_beg,
		const u64 expect_sync_end,
		bool &svr_death,
		AosBuffPtr &resp)
{
	AosTransPtr trans = OmnNew AosReadFmtTrans(svr_id, 
			AosGetSelfCubeGrpId(), file_seq, sync_beg, expect_sync_end);
	bool rslt = AosSendTrans(mRdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death) return true;

	aos_assert_r(resp, false);
	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBkpFmtSvr::recvFmtBySync(
		const AosBuffPtr &cont,
		const u64 expect_sync_end,
		bool &sync_succ)
{
	sync_succ = false;
	u32 fmt_beg_idx = cont->getCrtIdx();

	u32 fmt_len;
	u64 pre_fmt_id = 0, crt_fmt_id = 0;
	while(1)
	{
		fmt_len = cont->getU32(0);
		if(fmt_len == 0)	break;
		
		crt_fmt_id = AosFmt::getFmtIdFromBuff(cont);
		aos_assert_r(crt_fmt_id, false);
		
		if(crt_fmt_id >= expect_sync_end)
		{
			// the last sync_end is for error checking. not need save to norm file.
			sync_succ = true;
			break;
		}
			
		if(mShowLog)
		{
			OmnScreen << "FmtMgr; sorFmt recvFmt by sync" 
				<< ";fmt_id:" << crt_fmt_id 
				<< endl;
		}
		pre_fmt_id = crt_fmt_id;
		cont->setCrtIdx(cont->getCrtIdx() + fmt_len);
	}

	if(sync_succ)
	{
		if(pre_fmt_id != 0)	mCrtSyncEnd = pre_fmt_id;
	}
	else
	{
		mCrtSyncEnd = crt_fmt_id;
	}
	
	u32 total_fmt_len = cont->getCrtIdx() - fmt_beg_idx;
	if(total_fmt_len == 0)	return true;
	
	// saveToNormFile.
	bool rslt = mFmtFile->addData(cont->data() + fmt_beg_idx, 
			total_fmt_len, true);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBkpFmtSvr::recvFmt(
		const AosRundataPtr &rdata,
		const u32 cube_grp_id,
		const int from_svrid,
		const AosBuffPtr &fmt_buff)
{
	aos_assert_r(fmt_buff, false);
	aos_assert_r(cube_grp_id == (u32)AosGetSelfCubeGrpId(), false);

	bool rslt;
	u64 fmt_id = AosFmt::getFmtIdFromBuff(fmt_buff);

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; sorFmt recvFmt from master:" << mCrtMaster
			<< "; fmt_id:" << fmt_id
			<< "; mCrtRecvFmtId:" << mCrtRecvFmtId
			<< endl;
	}

	mLock->lock();
	if(!mSyncFinish)
	{
		if(mShowLog)
		{
			OmnScreen << "FmtMgr; sorFmt recvFmt from master:" << mCrtMaster
				<< "; fmt_id:" << fmt_id
				<< "; !SyncFinish"
				<< endl;
		}
		
		if(mRealSyncEnd == 0)	mRealSyncEnd = fmt_id;
		
		rslt = recvNewerFmt(rdata, fmt_id, fmt_buff);
		aos_assert_rl(rslt, mLock, false);
		mLock->unlock();
		return true;
	}
	
	rslt = recvNormFmt(rdata, fmt_id, fmt_buff);
	aos_assert_rl(rslt, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosBkpFmtSvr::recvNewerFmt(
		const AosRundataPtr &rdata,
		const u64 fmt_id,
		const AosBuffPtr &fmt_buff)
{
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; saveFmt to mergeFile, recvFmt" 
			<< "; fmt_id:" << fmt_id << endl;
	}
	
	if(mCrtRecvFmtId == 0)  aos_assert_r(fmt_id > mMaxFmtIdInFile, false);
	aos_assert_r(fmt_id > mCrtRecvFmtId, false);
	
	//bool rslt = mFmtFile->addMergeData(fmt_buff->data(), fmt_buff->dataLen());
	bool rslt = mFmtFile->addMergeFmt(fmt_buff);
	aos_assert_r(rslt, false);
	mCrtRecvFmtId = fmt_id;

	return true;
}


bool
AosBkpFmtSvr::recvNormFmt(
		const AosRundataPtr &rdata,
		const u64 fmt_id,
		const AosBuffPtr &fmt_buff)
{
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; saveFmt to normFile, recvFmt"
			<< "; fmt_id:" << fmt_id << endl;
	}
	
	if(mCrtRecvFmtId == 0)  aos_assert_r(fmt_id > mMaxFmtIdInFile, false);
	aos_assert_r(fmt_id > mCrtRecvFmtId, false);
	
	//bool rslt = saveToNormFile(rdata,
	//		fmt_buff->data(), fmt_buff->dataLen());
	bool rslt = saveToNormFile(rdata, fmt_buff);
	aos_assert_r(rslt, false);
	mCrtRecvFmtId = fmt_id;

	return true;
}


bool
AosBkpFmtSvr::saveToNormFile(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &fmt_buff)
{
	aos_assert_r(mFmtFile, false);
	
	int old_act_seq = mFmtFile->activeSeq();
	bool rslt = mFmtFile->addFmt(fmt_buff, true);
	aos_assert_r(rslt, false);

	int crt_act_seq = mFmtFile->activeSeq();
	aos_assert_r(crt_act_seq >=0, false);
	
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; save Fmt." 
			<< "; cube_grp_id:" << AosGetSelfCubeGrpId() 
			<< "; fmt_buff_len:" << fmt_buff->dataLen()
			<< "; old_act_seq:" << old_act_seq
			<< "; crt_act_seq:" << crt_act_seq
			<< endl; 
	}
	if(old_act_seq >=0 && old_act_seq != crt_act_seq)
	{
		// signal the proc thrd to proc the fmt logs.
		mFmtProc->addReq(old_act_seq);
	}
	return true;
}


/*
bool
AosBkpFmtSvr::switchFrom()
{
	OmnNotImplementedYet;
	return false;
	aos_assert_r(from->isMaster(), false);
	
	bool rslt = AosFmtSvr::switchFrom(from);	
	aos_assert_r(rslt, false);

	from->stop();
	mFmtProc->startProc();

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; switchToBkp:" << endl;
	}
	return true;
}
*/


bool
AosBkpFmtSvr::setNewMaster(const int new_master)
{
	// the old_master should never is self.
	// if old_master is self. then this proc is need reboot.
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; resetNewMaster:"
			<< "; old_master:" << mCrtMaster 
			<< "; new_master:" << new_master
			<< endl;
	}
	
	mCrtMaster = new_master;

	// maybe using another thrd.
	bool rslt = syncFmt();
	aos_assert_r(rslt, false);
	return true;
}

u64
AosBkpFmtSvr::getMaxRecvedFmtId()
{
	if(!mSyncFinish)	return 0;

	return mCrtRecvFmtId;
}

bool
AosBkpFmtSvr::recvFinishedFmtId(const u64 max_fmt_id)
{
	aos_assert_r(max_fmt_id > 0, false);
	mFmtProc->recvFinishedFmtId(max_fmt_id);	
	return true;
}


