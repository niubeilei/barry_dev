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
#include "FmtMgr/MasterFmtSvr.h"

#include "FmtMgr/Fmt.h"
#include "FmtMgr/FmtIdMgr.h"
#include "FmtMgr/FmtFile.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "TransBasic/AppMsg.h"
#include "Util/TransId.h"
#include "Util/Buff.h"

#include "SysMsg/SendFmtTrans.h"	
#include "SysMsg/GetMaxRecvedFmtIdTrans.h"
#include "SysMsg/SendFinishedFmtIdTrans.h"

__thread AosFmt *    AosMasterFmtSvr::stmFmt = NULL;


AosMasterFmtSvr::AosMasterFmtSvr()
:
AosFmtSvr(true),
mLock(OmnNew OmnMutex()),
mStoped(false),
mNextSendFmtId(0),
mGlobalFmtLock(OmnNew OmnMutex())
{
}


AosMasterFmtSvr::~AosMasterFmtSvr()
{
}


bool
AosMasterFmtSvr::config(const AosXmlTagPtr &conf)
{
	bool rslt = AosFmtSvr::config(conf);
	aos_assert_r(rslt, false);
	
	rslt = init();
	aos_assert_r(rslt, false);
	
	if(!mNextSendFmtId)
	{
		mNextSendFmtId = getMaxFmtIdInFile() + 1;	
	}
	return true;
}


bool
AosMasterFmtSvr::init()
{
	mGlobalFmt = OmnNew AosFmt();
	OmnThreadedObjPtr thisptr(this, false);
	mCleanThrd = OmnNew OmnThread(thisptr, "FmtCleanThrd", 
			eCleanThrdId, true, true, __FILE__, __LINE__);
	
	initSvrIds();

	if(AosIsConfigCubeMaster())
	{
		mIdMgr = OmnNew AosFmtIdMgr(mDirname);
		mIdMgr->init();
		mNextSendFmtId = mIdMgr->nextSendFmtId();
	}

	return true;
}

bool
AosMasterFmtSvr::initSvrIds()
{
	int svr_id = -1;
	BkpSvrInfo svr_info;
	while(1)
	{
		svr_id = AosGetNextSvrId(AosGetSelfCubeGrpId(), svr_id);
		if(svr_id == -1)	break;
		if(svr_id == AosGetSelfServerId())	continue;
		
		svr_info.mSvrId = svr_id;
		mBkpSvrs.push_back(svr_info);
		//mSvrIds.push_back(svr_id);
	}

	return true;	
}



bool
AosMasterFmtSvr::start()
{
	if(mSwitchFrom)
	{
		aos_assert_r(!mSwitchFrom->isMaster(), false);
		bool rslt = switchFrom();
		aos_assert_r(rslt, false);
	
		mSwitchFrom = 0;
	}

	aos_assert_r(mCleanThrd, false);
	mCleanThrd->start();
	return true;
}


bool
AosMasterFmtSvr::switchFrom()
{
	// when come here. will can't recvFmt from other svr.
	// the master must first call stopSendFmt(old_master).
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; SWH; switch to master. start proc fmt." << endl;
	}
	
	u64 t1 = OmnGetTimestamp();
	
	bool rslt = mSwitchFrom->stop();
	aos_assert_r(rslt, false);

	rslt = AosFmtSvr::switchFrom(mSwitchFrom);
	aos_assert_r(rslt, false);
	
	rslt = init();
	aos_assert_r(rslt, false);
	
	if(!mNextSendFmtId)
	{
		mNextSendFmtId = mSwitchFrom->getMaxRecvedFmtId() + 1;
	}

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; SWH; switch to master. proc fmt finish"
			<< "; time:" << OmnGetTimestamp() - t1 << endl;
	}
	return true;
}


bool
AosMasterFmtSvr::stop()
{
	mStoped = true;
	return true;
}


bool
AosMasterFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata,
		const u64 file_id,
		const u64 offset,
		const char *data,
		const int len,
		const bool flushflag)
{
	mGlobalFmtLock->lock();
	sendGlobalFmtLocked(rdata);
	bool rslt = mGlobalFmt->add(file_id, offset, data, len, flushflag);
	mGlobalFmtLock->unlock();
	return rslt;
}


bool
AosMasterFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata,
		const AosStorageFileInfo &file_info)
{
	mGlobalFmtLock->lock();
	sendGlobalFmtLocked(rdata);
	bool rslt = mGlobalFmt->add(file_info);
	mGlobalFmtLock->unlock();
	return rslt;
}   


bool
AosMasterFmtSvr::addDeleteFileToGlobalFmt(
		const AosRundataPtr &rdata, 
		const u64 &file_id)
{
	mGlobalFmtLock->lock();
	sendGlobalFmtLocked(rdata);
	bool rslt = mGlobalFmt->addDeleteFile(file_id);
	mGlobalFmtLock->unlock();
	return rslt;
}


bool
AosMasterFmtSvr::addToGlobalFmt(
		const AosRundataPtr &rdata, 
		const AosFmtPtr &fmt)
{
	aos_assert_r(fmt, false);

	mGlobalFmtLock->lock();
	sendGlobalFmtLocked(rdata);
	bool rslt = mGlobalFmt->appendFmt(fmt);
	mGlobalFmtLock->unlock();
	return rslt;
}


void
AosMasterFmtSvr::sendGlobalFmtLocked(const AosRundataPtr &rdata)
{
	aos_assert(mGlobalFmt);
	AosBuffPtr fmt_buf = mGlobalFmt->getFmtBuff();
	aos_assert(fmt_buf);
	if (fmt_buf->dataLen() > eSendGlobalSize)
	{
		sendFmtPriv(rdata, mGlobalFmt);
		mGlobalFmt->reset(eSendGlobalSize);
	}
}


bool
AosMasterFmtSvr::getNextSendFmtId(
		const AosRundataPtr &rdata,
		const u32 cube_grp_id,
		const int from_svr_id,
		u64 &next_fmt_id)
{
	// this is called by Trans AosNextSendFmtIdTrans. 
	aos_assert_r(cube_grp_id == (u32)AosGetSelfCubeGrpId(), false);

	mLock->lock();
	next_fmt_id = mNextSendFmtId;
	
	bool find = false;
	for(u32 i=0; i<mBkpSvrs.size(); i++)
	{
		if(mBkpSvrs[i].mSvrId != from_svr_id)	continue;
		
		mBkpSvrs[i].mSvrDeathed = false;
		find = true;
	}
	aos_assert_r(find, false);

	mLock->unlock();

	return true;	
}


bool
AosMasterFmtSvr::sendFmt(
		const AosRundataPtr &rdata,
		const AosFmtPtr	&fmt)
{
	if(AosGetReplicPolicyType() == AosReplicPolicyType::eNoBkp)	return true;
	
	mGlobalFmtLock->lock();
	sendFmtPriv(rdata, mGlobalFmt);
	mGlobalFmt->reset(eSendGlobalSize);
	mGlobalFmtLock->unlock();

	return sendFmtPriv(rdata, fmt);
}


bool
AosMasterFmtSvr::sendFmtPriv(
		const AosRundataPtr &rdata,
		const AosFmtPtr	&fmt)
{
	if(mBkpSvrs.size() == 0)
	{
		// finish trans.
		//vector<AosTransId>  trans_ids = fmt->getTransIds();
		//AosFinishTrans(trans_ids);
		return true;
	}

	if(mStoped)
	{
		OmnScreen << "FmtMgr; sorFmt sendFmt failed. is stopped."
			<< endl;
		return false;
	}
	aos_assert_r(fmt, false);

	AosBuffPtr fmt_buff = fmt->getFmtBuff();
	if (!fmt_buff || fmt_buff->dataLen() <= 0)
	{
		OmnScreen << "fmt_buff dataLen == 0 or no fmt_buff" << endl;
		return true;
	}

	mLock->lock();
	u64 fmt_id = mNextSendFmtId++;
	if(mIdMgr)
	{
		u64 id2 = mIdMgr->nextFmtId();	// will save the FmtId to file.
		aos_assert_rl(id2 == fmt_id, mLock, false);
	}
	
	fmt->setFmtId(fmt_id);

	// check whether the svr_ids has deathed svr.
	bool rslt, saved = false;
	for(u32 i=0; i<mBkpSvrs.size(); i++)
	{
		rslt = sendFmtToSvr(rdata, mBkpSvrs[i], 
				fmt_id, fmt_buff);
		aos_assert_rl(rslt, mLock, false);
		if(!mBkpSvrs[i].mSvrDeathed)	continue;
		if(saved)	continue;
		
		rslt = saveToNormFile(fmt_id, fmt_buff);
		aos_assert_rl(rslt, mLock, false);
		saved = true;
	}

	mLock->unlock();

	// finish trans.
	//vector<AosTransId>  trans_ids = fmt->getTransIds();
	//AosFinishTrans(trans_ids);
	return true;
}


bool
AosMasterFmtSvr::saveToNormFile(
		const u64 fmt_id,
		const AosBuffPtr &fmt_buff)
{
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; sorFmt saveFmt to normFile, some bkp svr deathed."
			<< "; fmt_id:" << fmt_id << endl;
	}
	
	//rslt = mFmtFile->addData(fmt_buff->data(), fmt_buff->dataLen(), false);
	bool rslt = mFmtFile->addFmt(fmt_buff, false);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosMasterFmtSvr::sendFmtToSvr(
		const AosRundataPtr &rdata,
		BkpSvrInfo &svr,
		const u64 fmt_id,
		const AosBuffPtr &fmt_buff)
{
	if(svr.mSvrDeathed)	return true;

	if(mShowLog)
	{
		OmnScreen << "FmtMgr; sorFmt sendFmt"
			<< "; fmt_id:" << fmt_id
			<< "; to_svr_id:" << svr.mSvrId
			<< endl;
	}

	AosTransPtr trans = OmnNew AosSendFmtTrans(
			svr.mSvrId, AosGetSelfCubeGrpId(), fmt_buff);

	AosBuffPtr resp;
	bool rslt = AosSendTrans(rdata, trans, resp, svr.mSvrDeathed);
	aos_assert_r(rslt, false);
	if(svr.mSvrDeathed)
	{
		OmnScreen << "FmtMgr; send Fmt svr death."
			<< "; fmt_id:" << fmt_id
			<< "; svr_id:" << svr.mSvrId << endl;
	}
	return true;
}


/*
bool
AosMasterFmtSvr::reSwitch()
{
	mNextSendFmtId = mFmtProc->getMaxFmtIdInFile() + 1;
	
	if(mShowLog)
	{
		OmnScreen << "FmtMgr; reSwitchToMaster:"
			<< "; mNextSendFmtId:" << mNextSendFmtId 
			<< endl;
	}
	return true;	
}
*/


bool
AosMasterFmtSvr::recvFmt(
		const AosRundataPtr &rdata,
		const u32 virtual_id,
		const int from_svrid,
		const AosBuffPtr &fmt_buff)
{
	if(!mStoped)
	{
		OmnShouldNeverComeHere;
		return false;
	}
	
	aos_assert_r(fmt_buff, false);
	u64 fmt_id = AosFmt::getFmtIdFromBuff(fmt_buff);
	// old master has stoped. maybe recv fmt. but don't proc.
	OmnScreen << "old master recv fmt from new master."
		<< " but ignore this fmt. old master will reboot"
		<< " new_master:" << from_svrid
		<< "; fmt_id:" << fmt_id << endl;
	return true;
}


u64
AosMasterFmtSvr::getMaxRecvedFmtId()
{
	//if(!mStoped)
	//{
	//	OmnShouldNeverComeHere;
	//	return false;
	//}

	// mayby mStopped. maby has mSyncFrom.
	// old master has stoped. maybe recv fmt. but don't proc.
	OmnScreen << "old master recv getMaxRecvedFmtId from new master."
		<< "; ignore." << endl;
	return 0;
}


bool
AosMasterFmtSvr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 thread_id = thread->getLogicId();
	
	if(thread_id == eCleanThrdId)
	{
		return cleanThrdhreadFunc(state, thread);
	}
	
	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosMasterFmtSvr::cleanThrdhreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u64 pre_max = 0, max_fmt_id = 0;
	bool rslt;

	while(state == OmnThrdStatus::eActive)
	{
		OmnSleep(eCleanSleepTime);

		rslt = collectCleanFmtId(max_fmt_id);
		aos_assert_r(rslt, false);
		
		if(max_fmt_id == 0)	continue;
		aos_assert_r(max_fmt_id >= pre_max, false);	
		if(max_fmt_id == pre_max) continue;

		rslt = sendFinishedFmtId(max_fmt_id);
		aos_assert_r(rslt, false);
		pre_max = max_fmt_id;
	}
	return true;
}


bool
AosMasterFmtSvr::collectCleanFmtId(u64 &max_fmt_id)
{
	u32 self_cube_grp_id = AosGetSelfCubeGrpId();

	AosTransPtr trans;
	AosBuffPtr resp;
	bool rslt, svr_death = false;
	u64 crt_fmt_id = 0;
	
	max_fmt_id = 0;
	for(u32 i=0; i<mBkpSvrs.size(); i++)
	{
		trans = OmnNew AosGetMaxRecvedFmtIdTrans(
				mBkpSvrs[i].mSvrId, self_cube_grp_id); 
		rslt = AosSendTrans(mRdata, trans, resp, svr_death);
		aos_assert_r(rslt, false);
		if(svr_death)
		{
			max_fmt_id = 0;
			return true;
		}
		
		aos_assert_r(resp, false);
		crt_fmt_id = resp->getU64(0);
		if(crt_fmt_id > max_fmt_id)	max_fmt_id = crt_fmt_id;
	}
	return true;
}


bool
AosMasterFmtSvr::sendFinishedFmtId(const u64 max_fmt_id)
{
	u32 self_cube_grp_id = AosGetSelfCubeGrpId();

	AosTransPtr trans;
	bool rslt, svr_death = false;
	for(u32 i=0; i<mBkpSvrs.size(); i++)
	{
		trans = OmnNew AosSendFinishedFmtIdTrans(
				mBkpSvrs[i].mSvrId, self_cube_grp_id, max_fmt_id); 
		rslt = AosSendTrans(mRdata, trans, svr_death);
		aos_assert_r(rslt, false);
	}

	// all bkp fmt svr has sync finished.
	// check BkpFmtSvr::getMaxRecvedFmtId
	return mFmtFile->deleteAllFiles();
}


