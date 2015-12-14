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
//
// Modification History:
// 06/03/2011: Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TransServer/CubeBkpTransSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "FmtMgr/MasterFmtSvr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "TransServer/TransFileMgr.h"
#include "TransServer/SyncInfo.h"
#include "TransBasic/Trans.h"

#include "SysMsg/SyncUnfinishTrans.h"
#include "SysMsg/SwitchToBkpTrans.h"	


AosCubeBkpTransSvr::AosCubeBkpTransSvr(const int crt_master)
:
AosCubeTransSvr(false),
mCrtMaster(crt_master),
mSyncLock(OmnNew OmnMutex()),
mSyncFinish(false)
{
	mFmtSvr = OmnNew AosBkpFmtSvr(mCrtMaster);
	mFmtSvrRaw = (AosBkpFmtSvr *)mFmtSvr.getPtr();
}


AosCubeBkpTransSvr::~AosCubeBkpTransSvr()
{
	OmnScreen << "delete Cube Bkp TransSvr!" << endl;
}


bool
AosCubeBkpTransSvr::start()
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; bkp start." << endl;
	}

	bool rslt = storageDiskCheck();
	aos_assert_r(rslt, false);
	
	rslt = cleanDfmLog();
	aos_assert_r(rslt, false);

	rslt = syncData();
	aos_assert_r(rslt, false);
	
	rslt = mFmtSvrRaw->start();
	aos_assert_r(rslt, false);

	rslt = mergeNewerFiles();
	aos_assert_r(rslt, false);
	
	mTransFileMgr->start();
	return true;
}


bool
AosCubeBkpTransSvr::storageDiskCheck()
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	bool rslt = vfs_mgr->diskCheck();	// check whether need recover.
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeBkpTransSvr::cleanDfmLog()
{
	//clean DfmLog.
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	bool rslt = vfs_mgr->clean();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeBkpTransSvr::syncData()
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; start syncData" << endl;
	}
	u64 t1 = OmnGetTimestamp();
	
	// 1. sync trans.
	bool rslt = syncTrans();
	aos_assert_r(rslt, false);

	// 2. sync fmt.
	rslt = mFmtSvrRaw->syncFmt();
	aos_assert_r(rslt, false);

	// all sync finish.
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; syncData finish"
			<< "; time:" << OmnGetTimestamp() - t1 << endl;
	}
	return true;
}

bool
AosCubeBkpTransSvr::syncTrans()
{
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; start syncTrans." << endl;
	}
	u64 t1 = OmnGetTimestamp();
	
	bool rslt = initSyncBeg();
	aos_assert_r(rslt, false);

	bool succ = false;
	int sync_sid = -1;
	while(1)
	{
		sync_sid = getNextSyncSvr(sync_sid);
		aos_assert_r(rslt, false);
		if(sync_sid == -1)
		{
			aos_assert_r(!succ, false);
			OmnScreen << "TransServer; SWH; syncTrans failed. "
				<< "maybe all svr death; try again"
				<< "; " << endl;
			OmnSleep(1);
			continue;
		}

		rslt = syncTransFromSvr(mRdata, sync_sid, succ);
		aos_assert_r(rslt, false);
		if(succ)	break;
	}
	aos_assert_r(succ, false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; syncTrans finish." 
			<< "; time:" << OmnGetTimestamp() - t1 << endl;
	}
	return true;
}

bool
AosCubeBkpTransSvr::initSyncBeg()
{
	map<u64, u64>	max_recv_seq;
	bool rslt = mTransFileMgr->getMaxRecvTid(max_recv_seq);
	aos_assert_r(rslt, false);
	
	AosSyncInfoPtr sync_info;
	map<u64, u64>::iterator itr = max_recv_seq.begin();
	mSyncLock->lock();
	for(; itr != max_recv_seq.end(); itr++)
	{
		sync_info = getSyncInfo(itr->first);
		sync_info->setSyncBeg(itr->second + 1);
	}
	mSyncLock->unlock();
	return true;
}


int
AosCubeBkpTransSvr::getNextSyncSvr(const int crt_sid)
{
	if(crt_sid == -1)
	{
		// first sync from crt master. if not succ. sync from bkp.
		return mCrtMaster;
	}
	
	if(crt_sid == mCrtMaster)
	{
		OmnScreen << "TransServer; sync trans from master failed" << endl;
		// try from other bkp.
		return -1;
	}

	int next_sid = crt_sid;
	while(1)
	{
		next_sid = AosGetNextSvrId(AosGetSelfCubeGrpId(), crt_sid);
		
		if(next_sid!= AosGetSelfServerId() && next_sid != mCrtMaster) break;
	}
	return next_sid;
}


bool
AosCubeBkpTransSvr::syncTransFromSvr(
		const AosRundataPtr &rdata,
		const int sync_sid,
		bool &succ)
{
	aos_assert_r(sync_sid >=0 && (u32)sync_sid < eAosMaxServerId , false);
	
	bool rslt; 
	AosBuffPtr resp;
	bool finish = false;
	int read_id = -1;

	bool svr_death = false;
	succ = true;
	while(!finish)
	{
		AosTransPtr trans = OmnNew AosSyncUnfinishTrans(
				sync_sid, AosGetSelfCubeGrpId(), read_id);

		rslt = AosSendTrans(rdata, trans, resp, svr_death);
		aos_assert_r(rslt, false);
		if(svr_death)
		{
			succ = false;
			OmnScreen << "TransServer;  sync Trans, but svr death:" << sync_sid << endl;
			break;
		}
		
		rslt = resp->getU8(0);
		aos_assert_r(rslt, false);
		finish = resp->getU8(0);
		read_id = resp->getInt(-1);
		u32 cont_len = resp->getU32(0);
		if(!cont_len)
		{
			aos_assert_r(finish, false);
			break;	
		}

		aos_assert_r(read_id>= 0, false);
		AosBuffPtr trans_buff = resp->getBuff(cont_len, false AosMemoryCheckerArgs);
		aos_assert_r(trans_buff, false);
	
		rslt = recvTransBySync(trans_buff);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosCubeBkpTransSvr::recvTransBySync(const AosBuffPtr &cont)
{
	bool rslt;
	bool can_sync;
	
	AosBuffPtr trans_buffs = OmnNew AosBuff(cont->dataLen() AosMemoryCheckerArgs);
	u32 trans_size;
	mSyncLock->lock();
	while(1)
	{
		trans_size = AosTrans::getNextTransSize(cont);
		if(!trans_size)	break;
	
		// getTransIdFromBuff don't change cont's mCrtIdx.
		AosTransId trans_id = AosTrans::getTransIdFromBuff(cont); 
		aos_assert_r(trans_id != AosTransId::Invalid, false);
		
		rslt = checkTransCanSync(trans_id, can_sync);
		aos_assert_rl(rslt, mSyncLock, false);
		if(can_sync)
		{
			trans_buffs->setU32(trans_size);
			trans_buffs->setBuff(cont->data() + cont->getCrtIdx(), trans_size);
		}
		
		cont->setCrtIdx(cont->getCrtIdx() + trans_size);
	}
	mSyncLock->unlock();
	
	if(trans_buffs)
	{
		rslt = mTransFileMgr->saveToNormFile(trans_buffs);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosCubeBkpTransSvr::checkTransCanSync(const AosTransId &trans_id, bool &can_sync)
{
	can_sync = false;

	trans_id.getCltKey();
	// 1. first check whether this has recved before.
	AosSyncInfoPtr	sync_info = getSyncInfo(trans_id.getCltKey());
	aos_assert_r(sync_info, false);
	if(sync_info->isSyncFinish())	return true;

	u64 sync_beg = sync_info->getSyncBeg();
	u64 sync_end = sync_info->getSyncEnd();	
	u64 crt_seq = trans_id.getSeqno();
	
	if(crt_seq < sync_beg)
	{
		if(mShowLog)
		{
			OmnScreen << "TransServer; recvTransBySync"
				<< "; but this trans has recved in file."
				<< "; sync_beg:" << sync_beg 
				<< "; trans_id:" << trans_id.toString()
				<< endl;
		}
		return true;
	}
	
	if(sync_end !=0 && crt_seq >= sync_end)
	{
		// means sync finish.
		if(mShowLog)
		{
			OmnScreen << "TransServer; recvTransBySync"
				<< "; this clt has sync finish."
				<< "; sync_end:" << sync_end 
				<< "; trans_id:" << trans_id.toString()
				<< endl;
		}
		sync_info->setSyncFinish();
		return true;
	}
	
	sync_info->setCrtSyncSeq(crt_seq);
	can_sync = true;
	if(mShowLog)
	{
		OmnScreen << "TransServer; recvTransBySync"
			<< "; trans_id:" << trans_id.toString()
			<< endl;
	}
	return true;
}


AosSyncInfoPtr
AosCubeBkpTransSvr::getSyncInfo(const u64 clt_key)
{
	map<u64, AosSyncInfoPtr>::iterator itr = mSyncInfo.find(clt_key);
	if(itr != mSyncInfo.end())	return itr->second;

	AosSyncInfoPtr sync_info = OmnNew AosSyncInfo();
	mSyncInfo.insert(make_pair(clt_key, sync_info));
	return sync_info;
}


bool
AosCubeBkpTransSvr::mergeNewerFiles()
{
	// syncTrans finish.
	mSyncLock->lock();
	bool rslt = mTransFileMgr->mergeNewerFiles();
	aos_assert_rl(rslt, mSyncLock, false);

	u64 clt_key;
	AosSyncInfoPtr sync_info;
	u64 crt_sync_seq;
	map<u64, AosSyncInfoPtr>::iterator itr = mSyncInfo.begin();
	for(; itr != mSyncInfo.end(); itr++)
	{
		clt_key = itr->first;
		sync_info = itr->second;	
		crt_sync_seq = sync_info->getCrtSyncSeq();	
		if(crt_sync_seq == 0)	continue;
		tryToSetMaxRecv(clt_key, crt_sync_seq);
	}

	mSyncFinish = true;
	mSyncLock->unlock();
	return true;
}


bool
AosCubeBkpTransSvr::setNewMaster(const int new_master)
{
	if(mShowLog)
	{
		OmnScreen << "======== TransServer; recv new master:"
			<< "; old_master:" << mCrtMaster
			<< "; new_master:" << new_master
			<< endl;
	}
	OmnScreen << "AboutMaster set crt master:" << mCrtMaster << endl;
	
	mCrtMaster = new_master;
	bool rslt = mFmtSvrRaw->setNewMaster(new_master);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeBkpTransSvr::recvTrans(const AosTransPtr &trans)
{
	bool rslt;
	if(isRecvBefore(trans))
	{
		trans->sendAck();
		return true;
	}

	if(isRecvedBySync(trans))
	{
		trans->sendAck();
		return true;
	}
	
	setMaxRecvPriv(trans);
	if(trans->isSystemTrans())
	{
		// 3. if trans is sys trans. never ignore. so can't gerantee the order.
		rslt = pushMsgToQueue(trans.getPtr());
		aos_assert_r(rslt, false);
		trans->sendAck();
		return true;
	}
	if(!mSyncFinish)
	{
		mSyncLock->lock();
		if(!mSyncFinish)
		{
			rslt = recvNewerTrans(trans);
			mSyncLock->unlock();
			return true;
		}
		mSyncLock->unlock();
	}

	if(trans->isNeedSave())
	{
		rslt = mTransFileMgr->saveToNormFile(trans);
		aos_assert_r(rslt, false);
	}

	trans->sendAck();
	if(mShowLog && !trans->isNeedSave() && mCrtMaster != AosGetSelfServerId())
	{
		OmnScreen << "TransServer; trans don't need save. and is not in master svr."
			<< "; type:" << trans->getStrType()
			<< "; trans_id:" << trans->getTransId().toString()
			<< "; svr_id:" << AosGetSelfServerId()
			<< endl;
	}
	return true;
}


bool
AosCubeBkpTransSvr::isRecvedBySync(const AosTransPtr &trans)
{
	if(mSyncFinish)	return false;
	
	mSyncLock->lock();
	if(mSyncFinish)
	{
		// means mSyncFinish has finish.
		mSyncLock->unlock();
		return false;
	}

	u64 crt_seq = (trans->getTransId()).getSeqno();
	AosSyncInfoPtr	sync_info = getSyncInfo(trans->getCltKey());
	aos_assert_rl(sync_info, mSyncLock, false);

	if(sync_info->isNoSyncEnd())
	{
		sync_info->setSyncEnd(crt_seq);
		if(mShowLog)
		{
			OmnScreen << "CubeTransSvr; set sync end."
				<< "; clt_key:" << trans->getCltKey()
				<< "; seq:" << crt_seq << endl;
		}
	}

	u64 crt_sync_seq = sync_info->getCrtSyncSeq();
	mSyncLock->unlock();
	
	if(crt_seq > crt_sync_seq)	return false;
	
	if(mShowLog)
	{
		OmnScreen << "CubeTransSvr; this trans has recved by sync."
			<< "; crt_sync_seq:" << crt_sync_seq
			<< "; trans_id:" << (trans->getTransId()).toString()
			<< endl;
	}
	return true;
}


bool
AosCubeBkpTransSvr::recvNewerTrans(const AosTransPtr &trans)
{
	// when svr is syncing. 
	if(trans->isNeedSave())
	{
		bool rslt = mTransFileMgr->saveToNewerFile(trans);
		aos_assert_r(rslt, false);
		trans->sendAck();
		return true;
	}

	// maybe all svr has down. and this svr is the last master. and
	// this svr is just up.
	OmnShouldNeverComeHere;

	addIgnoreTrans(trans);
	OmnScreen << "TransServer; ignore trans; svr is syncing. recv unsave trans."
		<< "; trans_id:" << trans->getTransId().toString()
		<< "; type:" << trans->getStrType()
		<< "; svr_id:" << AosGetSelfServerId()
		<< endl;
	return true;
}


/*	
bool
AosCubeBkpTransSvr::start()
{
	// this happend when all svrs is just start.
	// if is cube svr && is master. need init max recv tid.
	if(mShowLog)
	{
		OmnScreen << "TransServer; bkp start." << endl;
	}
	
	bool rslt = startThrd();
	aos_assert_r(rslt, false);
	
	rslt = startIpcClt();
	aos_assert_r(rslt, false);

	rslt = storageDiskCheck();
	aos_assert_r(rslt, false);
	
	rslt = cleanDfmLog();
	aos_assert_r(rslt, false);

	rslt = syncData();
	aos_assert_r(rslt, false);
	
	rslt = mFmtSvrRaw->start();
	aos_assert_r(rslt, false);

	rslt = mergeNewerFiles();
	aos_assert_r(rslt, false);
	
	mTransFileMgr->start();
	return true;
}
*/

bool
AosCubeBkpTransSvr::switchFrom(const AosCubeTransSvrPtr &from)
{
	OmnShouldNeverComeHere;
	return false;
	/*
	aos_assert_r(from->isMaster(), false);

	bool rslt = AosCubeTransSvr::switchFrom(from);
	aos_assert_r(rslt, false);
	
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	rslt = vfs_mgr->switchToBkp();
	aos_assert_r(rslt, false);

	setIdle();
	return true;
	*/
}

