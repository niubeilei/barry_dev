////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/TransHandler.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/ProcessType.h"
#include "TransBasic/AppMsg.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "TransUtil/CubicTrans.h"
#include "Util/File.h"

AosTransHandler::AosTransHandler(
		const AosSvrProxyPtr &svr_proxy,
		const bool &show_log)
:
mLock(OmnNew OmnMutex()),
mSvrProxy(svr_proxy),
mShowLog(show_log)
{
}


AosTransHandler::~AosTransHandler()
{
}
	

bool
AosTransHandler::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild(AOSCONFIG_TRANS_SERVER);
	aos_assert_r(conf, false);

	mShowLog = conf->getAttrBool(AOSCONFIG_SHOWLOG, false);
	return true;	
}

int
AosTransHandler::getCrtMaster(const u32 cube_grp_id)
{
	map<u32, int>::iterator itr = mCrtMaster.find(cube_grp_id);
	aos_assert_r(itr != mCrtMaster.end(), -1);
	int svr_id = itr->second;
	return svr_id;
}

bool
AosTransHandler::procTrans(const AosTransPtr &trans)
{
	bool rslt;
	int from_svr_id = trans->getFromSvrId();
	aos_assert_r(from_svr_id >= 0 && (u32)from_svr_id < eAosMaxServerId, false);
	if(from_svr_id != AosGetSelfServerId() )
	{
		// This trans(maybe cube) is from remote and to local process.
		rslt = sendTransPriv(trans);
		aos_assert_r(rslt, false);
		return true;
	}
	
	if(!trans->isCubeTrans())
	{
		rslt = sendTransPriv(trans);
		aos_assert_r(rslt, false);
		return true;
	}

	AosCubicTransPtr c_trans = (AosCubicTrans *)trans.getPtr();
	rslt = procCubeTrans(c_trans);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosTransHandler::procCubeTrans(const AosCubicTransPtr &trans)
{
	u32 cube_grp_id = AosGetCubeGrpIdByCubeId(trans->getCubeId());
	bool rslt;
	
	int crt_master = getCrtMaster(cube_grp_id);
	aos_assert_r(crt_master >= 0, false);
	
	if(!trans->isNeedSave())
	{
		trans->setToSvrId(crt_master);
		
		rslt = sendTransPriv(trans.getPtr());
		aos_assert_r(rslt, false);
		return true;
	}

	int to_sid = AosGetNextSvrId(cube_grp_id, -1);
	aos_assert_r(to_sid != -1, false);

	while(to_sid != -1)
	{
		AosConnMsgPtr c_msg = trans->copy();
		AosTransPtr new_trans = (AosTrans *)c_msg.getPtr();
		new_trans->setToSvrId(to_sid);
		
		rslt = sendTransPriv(new_trans.getPtr());
		aos_assert_r(rslt, false);

		to_sid = AosGetNextSvrId(cube_grp_id, to_sid);
	}
	return true;
}

bool
AosTransHandler::setNewMaster(const u32 cube_grp_id, const int svr_id)
{
	aos_assert_r(svr_id >=0 && (u32)svr_id <eAosMaxServerId, false);

	mLock->lock();
	mCrtMaster[cube_grp_id] = svr_id;

	AosBuffPtr master_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	master_buff->setU64(OmnGetTimestamp());
	master_buff->setU32(mCrtMaster.size());

	map<u32, int>::iterator itr = mCrtMaster.begin();
	for(; itr != mCrtMaster.end(); itr++)
	{
		master_buff->setU32(itr->first);	
		master_buff->setInt(itr->second);	
	}
	mLock->unlock();

	bool rslt = saveMasterBuffToFile(master_buff); 
	aos_assert_r(rslt, false);
	return true;
}

bool
AosTransHandler::saveMasterBuffToFile(const AosBuffPtr master_buff)
{
	aos_assert_r(master_buff, false);

	OmnFilePtr file = getMasterFile();
	aos_assert_r(file, false);

	bool rslt = mMasterFile->put(0, master_buff->data(),
			master_buff->dataLen(), true);
	aos_assert_r(rslt, false);
	return true;
}

OmnFilePtr
AosTransHandler::getMasterFile()
{
	if(mMasterFile)	return mMasterFile;

	mLock->lock();
	if(mMasterFile)	return mMasterFile;

	OmnString fname = OmnApp::getAppBaseDir();
	fname << "crtMaster";
	mMasterFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);		
	if(!mMasterFile || !mMasterFile->isGood())
	{
		mMasterFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);		
	}
	aos_assert_r(mMasterFile && mMasterFile->isGood(), 0);
	mLock->unlock();
	return mMasterFile;
}

bool
AosTransHandler::sendTransPriv(const AosTransPtr &trans)
{
	if(trans->isResend())
	{
		OmnScreen << "resend trans." 
			<< ". trans_type:" << trans->getType()
			<< ". trans_id:" << trans->getTransId().toString()
			<< ". from_svr_id:" << trans->getFromSvrId()
			<< ". from_proc_id:" << trans->getFromProcId()
			<< ". to_svr_id:" << trans->getToSvrId()
			<< ". to_proc_id:" << trans->getToProcId()
			<< endl;
	}

	//OmnScreen << "send trans." 
	//	<< ". trans_type:" << trans->getType()
	//	<< ". trans_id:" << trans->getTransId().toString()
	//	<< ". to_svr_id:" << trans->getToSvrId()
	//	<< ". to_proc_id:" << trans->getToProcId()
	//	<< endl;
	bool rslt = mSvrProxy->sendTrans(trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosTransHandler::getMasters(const AosBuffPtr &master_buff)
{
	aos_assert_r(master_buff, false);
	
	master_buff->setU32(mCrtMaster.size());
	map<u32, int>::iterator itr = mCrtMaster.begin();
	for(; itr != mCrtMaster.end(); itr++)
	{
		master_buff->setU32(itr->first);
		master_buff->setInt(itr->second);
	}

	return true;
}


bool
AosTransHandler::getMastersFromFile(const AosBuffPtr &master_buff)
{
	aos_assert_r(master_buff, false);
	
	OmnFilePtr file = getMasterFile();
	aos_assert_r(file, false);
	u32 file_len = file->getLength();
	if(file_len == 0)
	{
		master_buff->setU64(0);
		return true;
	}
	
	AosBuffPtr file_data;
	bool rslt = file->readToBuff(file_data, file_len + 1);
	aos_assert_r(rslt, false);
	
	u64 saved_time = file_data->getU64(0);
	aos_assert_r(saved_time, false);
	u32 num = file_data->getU32(0);
	master_buff->setU64(saved_time);
	master_buff->setU32(num);
	
	for(u32 i=0; i<num; i++)
	{
		master_buff->setU32(file_data->getU32(0));
		master_buff->setInt(file_data->getInt(-1));
	}
	return true;
}


bool
AosTransHandler::recvMasters(const AosBuffPtr &master_buff)
{
	mLock->lock();
	
	bool rslt = serializeMasterBuff(master_buff);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}


bool
AosTransHandler::serializeMasterBuff(const AosBuffPtr &master_buff)
{
	u32 nn = master_buff->getU32(0);
	for(u32 i=0; i<nn; i++)
	{
		u32 cube_grp_id = master_buff->getU32(0);
		int master = master_buff->getInt(-1);
		
		aos_assert_r(master >=0 && (u32)master < eAosMaxServerId, false);
		mCrtMaster.insert(make_pair(cube_grp_id, master)); 
		
		OmnScreen << "init Crt Master"
			<< "; cube_grp_id:" << cube_grp_id
			<< "; crt_master:" << master << endl; 
	}
	
	return true;
}


bool
AosTransHandler::recvMastersFromOtherSvrFile(
		const int from_sid,
		const AosBuffPtr &master_buff,
		bool &collect_finish)
{
	aos_assert_r(master_buff, false);
	
	mLock->lock();
	bool rslt = pushToWaitMasterMapPriv(from_sid, master_buff);
	aos_assert_rl(rslt, mLock, false);
	
	if(mWaitMasterMap.size() < (u32)AosGetNumPhysicals())
	{
		collect_finish = false;
		mLock->unlock();
		return true;
	}

	collect_finish = true;
	
	AosBuffPtr correct_master_buff;
	rslt = getMastersFromWaitMap(correct_master_buff);
	aos_assert_rl(rslt, mLock, false);
	
	// maybe correct_master_buff == 0. all files don't has buff.
	if(correct_master_buff)
	{
		rslt = serializeMasterBuff(correct_master_buff);
		aos_assert_rl(rslt, mLock, false);
	}
	
	mWaitMasterMap.clear();
	mLock->unlock();
	return true;
}

bool
AosTransHandler::contInitCrtMasters()
{
	// just after add cluster. can call this func. know total cubes.

	mLock->lock();
	vector<u32> cube_grps;
	AosGetTotalCubeGrps(cube_grps);
	map<u32, int>::iterator itr;
	for(u32 i=0; i<cube_grps.size(); i++)
	{
		u32 cube_grp_id = cube_grps[i];
		itr = mCrtMaster.find(cube_grp_id);
		if(itr != mCrtMaster.end())	continue;

		// use config's master.
		int crt_master = AosGetConfigMaster(cube_grp_id);
		//aos_assert_r(crt_master >=0, false);
		if(crt_master <0) continue;

		mCrtMaster.insert(make_pair(cube_grp_id, crt_master)); 
		OmnScreen << "init Crt Master; useing config master"
			<< "; cube_grp_id:" << cube_grp_id
			<< "; crt_master:" << crt_master << endl; 
	}
	mLock->unlock();
	return true;
}


bool
AosTransHandler::pushToWaitMasterMapPriv(
		const int from_sid,
		const AosBuffPtr &master_buff)
{
	aos_assert_r(master_buff, false);

	u32 data_len = master_buff->dataLen() - master_buff->getCrtIdx();
	AosBuffPtr m_buff = master_buff->getBuff(data_len, true AosMemoryCheckerArgs);
	mWaitMasterMap.insert(make_pair(from_sid, m_buff));
	OmnScreen << "recv some svr's crt masters:" << from_sid << endl;
	return true;
}


bool
AosTransHandler::getMastersFromWaitMap(AosBuffPtr &master_buff)
{
	aos_assert_r(mWaitMasterMap.size() == (u32)AosGetNumPhysicals(), false);
	
	// means all svr's master in file has come.
	u64 max_time = 0;
	map<u32, AosBuffPtr>::iterator itr = mWaitMasterMap.begin();
	for(; itr != mWaitMasterMap.end(); itr++)
	{
		AosBuffPtr crt_data = itr->second;
		u64 crt_time = crt_data->getU64(0); 
		if(crt_time == 0 || crt_time <= max_time)	continue;
		
		max_time = crt_time;
		u32 len = crt_data->dataLen() - crt_data->getCrtIdx();
		master_buff = crt_data->getBuff(len, false AosMemoryCheckerArgs);
	}
	
	return true;
}

