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
#include "TransServer/CubeTransSvrMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "FmtMgr/FmtSvr.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/SystemId.h"
#include "TransBasic/Trans.h"
#include "TransServer/CubeTransSvr.h"
#include "Thread/RwLock.h"	

//#include "SysMsg/SwitchToBkpTrans.h"	
#include "SysMsg/StopSendFmtTrans.h"	
#include "SysMsg/SwitchToMasterFinishMsg.h"	

AosCubeTransSvrMgr::AosCubeTransSvrMgr()
:
mLock(OmnNew OmnRwLock()),
mShowLog(false)
{
}


AosCubeTransSvrMgr::~AosCubeTransSvrMgr()
{
}


bool
AosCubeTransSvrMgr::config(const AosXmlTagPtr &app_conf)
{
	mShowLog = app_conf->getAttrBool(AOSCONFIG_SHOWLOG, false);
	
	mCrtTransSvr = AosCubeTransSvr::getTransSvrStatic(app_conf);
	aos_assert_r(mCrtTransSvr, false);
	
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(AOS_SYS_SITEID);
	return true;
}


bool
AosCubeTransSvrMgr::start()
{
	mCrtTransSvr->init();	// will start IpcClt.

	mCrtTransSvr->start();
	if(mCrtTransSvr->isMaster() || !AosIsConfigCubeMaster())	return true;
	
	//bool rslt = triggerOldMasterSwitchToBkp(); 
	bool rslt = triggerOldMasterStopSendFmt(); 
	aos_assert_r(rslt, false);

	rslt = switchToMaster();
	aos_assert_r(rslt, false);

	//mTransFileMgr->start();
	return true;
}


bool
AosCubeTransSvrMgr::stop()
{
	aos_assert_r(mCrtTransSvr, false);
	
	mLock->readlock();
	bool rslt = mCrtTransSvr->stop();
	mLock->unlock();

	return rslt;
}


bool
AosCubeTransSvrMgr::recvMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(mCrtTransSvr, false);

	mLock->readlock();
	bool rslt = mCrtTransSvr->recvMsg(msg);
	mLock->unlock();
	
	return rslt;
}

bool
AosCubeTransSvrMgr::finishTrans(vector<AosTransId> &trans_ids)
{
	aos_assert_r(mCrtTransSvr, false);
	
	mLock->readlock();
	bool rslt = mCrtTransSvr->finishTrans(trans_ids);
	mLock->unlock();
	
	return rslt;
}


bool
AosCubeTransSvrMgr::finishTrans(const AosTransPtr &trans)
{
	aos_assert_r(mCrtTransSvr, false);

	mLock->readlock();
	bool rslt = mCrtTransSvr->finishTrans(trans);
	mLock->unlock();
	
	return rslt;
}

void
AosCubeTransSvrMgr::resetCrtCacheSize(const u64 proced_msg_size)
{
	aos_assert(mCrtTransSvr);
	
	mLock->readlock();
	mCrtTransSvr->resetCrtCacheSize(proced_msg_size);
	mLock->unlock();
}

AosFmtMgrObjPtr
AosCubeTransSvrMgr::getFmtSvr()
{
	aos_assert_r(mCrtTransSvr, 0);
	
	mLock->readlock();
	AosFmtSvrPtr fmt_svr = mCrtTransSvr->getFmtSvr();
	mLock->unlock();
	
	return fmt_svr;
}


bool
AosCubeTransSvrMgr::readTransBySync(
		AosBuffPtr &trans_buffs,
		int &read_id,
		bool &finish)
{
	aos_assert_r(mCrtTransSvr, false);
	
	mLock->readlock();
	bool rslt = mCrtTransSvr->readTransBySync(trans_buffs, read_id, finish);
	mLock->unlock();

	return rslt;
}


bool
//AosCubeTransSvrMgr::triggerOldMasterSwitchToBkp()
AosCubeTransSvrMgr::triggerOldMasterStopSendFmt()
{
	// If old_master not death. first notify old_aster.
	// It's sync call. when notify finish. old_master will
	// not send Fmt To Other svr.
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; trigger old master switch to bkp." << endl;
	}
	u64 t1 = OmnGetTimestamp();

	int old_master = mCrtTransSvr->getCrtMaster();
	aos_assert_r(old_master >=0, false);
	//AosTransPtr trans = OmnNew AosSwitchToBkpTrans(old_master, AosGetSelfCubeGrpId());
	AosTransPtr trans = OmnNew AosStopSendFmtTrans(old_master, AosGetSelfCubeGrpId());

	bool svr_death;		// this svr_death is the old_master.
	AosBuffPtr resp;
	bool rslt = AosSendTrans(mRdata, trans, resp, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death)
	{
		if(mShowLog)
		{
			OmnScreen << "TransServer; SWH; switchToMaster. old_master deathed:"
				<< old_master << endl;
		}
		return true;
	}

	aos_assert_r(resp, false);

	rslt = resp->getU8(0);
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; trigger old master switch to bkp finish."
			<< "; time:" << OmnGetTimestamp() - t1 << endl;
	}
	return true;
}



bool
AosCubeTransSvrMgr::switchToMaster()
{
	// this func is called by AosSwitchToMasterMsg or syncData().
	aos_assert_r(!mCrtTransSvr->isMaster(), false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; switch to master:"
			<< "; old_master:" << mCrtTransSvr->getCrtMaster() 
			<< "; new_master:" << AosGetSelfServerId() 
			<< endl;
	}
	
	mLock->writelock();
	mCrtTransSvr = AosCubeTransSvr::switchFromStatic(mCrtTransSvr);
	mLock->unlock();

	aos_assert_r(mCrtTransSvr, false);
	
	bool rslt = mCrtTransSvr->start();
	aos_assert_r(rslt, false);
	
	rslt = sendSwitchToMasterFinish();
	aos_assert_r(rslt, false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; switch to master finish1"
			<< endl;
	}

	return true;
}


bool
AosCubeTransSvrMgr::sendSwitchToMasterFinish()
{
	int cube_grpid = AosGetSelfCubeGrpId();
	aos_assert_r(cube_grpid != -1, false);	
	AosAppMsgPtr msg = OmnNew AosSwitchToMasterFinishMsg(
			AosGetSelfServerId(), AOSTAG_SVRPROXY_PID,
			AosGetSelfProcId(), cube_grpid);

	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeTransSvrMgr::stopSendFmt()
{
	//some svr has switch to master.
	//this crt master need stop send fmt.
	//actual this proc will reboot.
	aos_assert_r(mCrtTransSvr->isMaster(), false);

	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; stop send fmt:"
			<< endl;
	}
	
	mCrtTransSvr->stopSendFmt();
	return true;
}


bool
AosCubeTransSvrMgr::setNewMaster(const u32 cube_grp_id, const int new_master)
{
	// this func is called by AosSetNewMasterTrans
	aos_assert_r(cube_grp_id == (u32)AosGetSelfCubeGrpId(), false);
	aos_assert_r(!mCrtTransSvr->isMaster(), false);
	
	mCrtTransSvr->setNewMaster(new_master);
	return true;
}


/*
bool
AosCubeTransSvrMgr::switchToBkp()
{
	// this func is called by AosSwitchToBkp.
	// actual this proc will reboot.
	aos_assert_r(mCrtTransSvr->isMaster(), false);

	if(mShowLog)
	{
		OmnScreen << "TransServer; SWH; swtich to bkp:"
			<< endl;
	}
	mCrtTransSvr = AosCubeTransSvr::switchFromStatic(mCrtTransSvr);
	aos_assert_r(mCrtTransSvr, false);
	return true;
}
*/

bool
AosCubeTransSvrMgr::reSwitchToMaster()
{
	OmnNotImplementedYet;
	/*
	aos_assert_r(mCrtTransSvr->isMaster(), false);
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; re swtich to master svr:"
			<< endl;
	}
	
	mCrtTransSvr->reSwitch();
	*/
	return true;
}


bool
AosCubeTransSvrMgr::isSwitchToMFinish(bool &finish)
{
	OmnNotImplementedYet;
	return false;
	/*
	if(!mCrtTransSvr->isMaster())
	{
		OmnScreen << "!!!!! This svr is not master." << endl;
		finish = false;
		return true;
	}
	
	finish = mCrtTransSvr->isStartFinished();
	return true;
	*/
}


