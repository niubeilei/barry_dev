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
#include "TransServer/CubeTransSvr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEInterfaces/FmtMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "StorageMgr/SystemId.h"
#include "TransServer/TransFileMgr.h"
#include "TransBasic/Trans.h"
#include "TransUtil/TransAckMsg.h"

#include "TransServer/CubeMasterTransSvr.h"
#include "TransServer/CubeBkpTransSvr.h"


AosCubeTransSvr::AosCubeTransSvr(const bool is_master)
:
mIsMaster(is_master),
mStartFinish(false)
{
}

AosCubeTransSvr::~AosCubeTransSvr()
{
}

AosCubeTransSvrPtr
AosCubeTransSvr::getTransSvrStatic(const AosXmlTagPtr &conf)
{
	int crt_master = AosGetSelfCrtMaster();
	aos_assert_r(crt_master != -1, 0);
	OmnScreen << "AboutMaster config set crt master:" << crt_master << endl;

	AosCubeTransSvrPtr new_svr;
	if(crt_master == AosGetSelfServerId())
	{
		new_svr = OmnNew AosCubeMasterTransSvr();
	}
	else
	{
		new_svr = OmnNew AosCubeBkpTransSvr(crt_master);	
	}

	bool rslt = new_svr->config(conf);
	aos_assert_r(rslt, 0);
	
	return new_svr;
}

bool
AosCubeTransSvr::config(const AosXmlTagPtr &app_conf)
{
	bool rslt = AosTransServer::config(app_conf);
	aos_assert_r(rslt, false);
	
	rslt = mFmtSvr->config(app_conf);
	aos_assert_r(rslt, false);
	
	mRdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRdata->setSiteid(AOS_SYS_SITEID);
	
	return true;
}

bool
AosCubeTransSvr::init()
{
	bool rslt = startThrd();
	aos_assert_r(rslt, false);
	
	rslt = startIpcClt();
	aos_assert_r(rslt, false);

	return true;	
}

bool
AosCubeTransSvr::startVfsMgr()
{
	AosVfsMgrObjPtr vfs_mgr = AosVfsMgrObj::getVfsMgr();
	aos_assert_r(vfs_mgr, false);
	bool rslt = vfs_mgr->start();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeTransSvr::startIpcClt()
{
	AosIpcCltObjPtr ipc_clt = AosIpcCltObj::getIpcClt();
	aos_assert_r(ipc_clt, false);
	bool rslt = ipc_clt->start();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosCubeTransSvr::recvMsg(const AosAppMsgPtr &msg)
{
	if(!msg->isTrans())
	{
		if(mShowLog)
		{
			OmnScreen << "TransServer; RecvMsg:"
				<< "; type:" << msg->getStrType()
				<< endl;
		}
		
		if(msg->getType() == MsgType::eTransAckMsg)
		{
			AosTransAckMsg* ack_msg = (AosTransAckMsg*)msg.getPtr();
			
			if (mShowLog)
			{
				OmnScreen << "TransSvr RRecv trans ack :"
					<< (ack_msg->getReqId()).toString() << endl;
			}
		}
		
		return pushMsgToQueue(msg);
	}

	AosTransPtr trans = (AosTrans *)msg.getPtr();
	
	if(mShowLog)
	{
		OmnScreen << "TransServer; RecvTrans:"
			<< "; type:" << trans->getStrType()
			<< "; trans_id:" << trans->getTransId().toString() 
			<< "; cont:" << trans->toString()
			<< "; need_save:" << trans->isNeedSave() 
			<< "; is_resend:" << trans->isResend() 
			<< "; is_master:" << isMaster() 
			<< endl;
	}
	return recvTrans(trans);
}

AosCubeTransSvrPtr
AosCubeTransSvr::switchFromStatic(const AosCubeTransSvrPtr &from)
{
	AosCubeTransSvrPtr new_svr;
	if(from->isMaster())
	{
		new_svr = OmnNew AosCubeBkpTransSvr(-1);
	}
	else
	{
		new_svr = OmnNew AosCubeMasterTransSvr();
	}
	
	bool rslt = new_svr->switchFrom(from);
	aos_assert_r(rslt, 0);
	return new_svr;
}


bool
AosCubeTransSvr::switchFrom(const AosCubeTransSvrPtr &from)
{
	bool rslt = AosTransServer::switchFrom(from.getPtr());
	aos_assert_r(rslt, false);

	mRdata = from->mRdata;
	return true;
}

//bool
//AosCubeTransSvr::reSwitch()
//{
//	OmnShouldNeverComeHere;
//	return false;
//}
	
bool
AosCubeTransSvr::stopSendFmt()
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosCubeTransSvr::setNewMaster(const int new_master)
{
	OmnShouldNeverComeHere;
	return false;
}

bool
AosCubeTransSvr::readTransBySync(
		AosBuffPtr &trans_buffs,
		int &read_id,
		bool &finish)
{
	u32 max_buff_size = AosBuff::eDftMaxBuffLen / 10;
	trans_buffs = OmnNew AosBuff(max_buff_size + 500, 0 AosMemoryCheckerArgs);
	AosBuffPtr unfinish_buffs;
	while(1)
	{
		bool rslt = mTransFileMgr->readUnfinishTrans(unfinish_buffs, 
				read_id, finish);
		aos_assert_r(rslt, false);
		if(unfinish_buffs) trans_buffs->setBuff(unfinish_buffs);

		if(finish || trans_buffs->dataLen() >= max_buff_size)	break;
	}
	if(finish)	mTransFileMgr->normReqReadFinish(read_id);
	return true;
}

AosFmtSvrPtr
AosCubeTransSvr::getFmtSvr()
{
	return mFmtSvr;
}

