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
// 2010/12/25	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "HeartbeatNew/HeartbeatMgr.h"

#include "Alarm/Alarm.h"
#include "Porting/GetTime.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "UtilComm/ConnBuff.h"



AosHeartbeatMgr::AosHeartbeatMgr()
:
mLock(OmnNew OmnMutex()),
mHeartbeatFreq(eDftHeartbeatFreq),
mCondVar(OmnNew OmnCondVar()),
mHbTimerSec(eDefaultHbTimerSec),
mHeartbeatSent(false)
{
	OmnIpAddr local_addr = OmnCommUtil::getLocalIpAddr();
	int local_port = 7007;
	mComm = OmnNew OmnUdpComm(local_addr, local_port, "heartbeat_msg");
}


bool
AosHeartbeatMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "HeartbeatMgr", 0, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosHeartbeatMgr::stop()
{
	mThread->stop();
	return true;
}


AosHeartbeatMgr::~AosHeartbeatMgr()
{
}


OmnRslt
AosHeartbeatMgr::config(const AosXmlTagPtr &conf)
{

	return true;
}

	
bool
AosHeartbeatMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	int num_checks = 0;
	mResponded = OmnNew char[mMonitored.size()];

    while (state == OmnThrdStatus::eActive)
    {
		startHeartbeat();
		OmnSleep(mHeartbeatFreq);
		checkHeartbeat();
    }
    return true;
}


bool
AosHeartbeatMgr::checkHeartbeat()
{
	// It checks whether all monitored have responded. 
	if (!mHeartbeatSent) return true;

	if (mNumResponded == mMonitored.size()) return true;

	// Some did not respond. 
	AosBuff errmsg;
	errmsg.setOmnStr("first_round_missed");
	int num_failed = 0;
	for (u32 i=0; i<mMonitored.size(); i++)
	{
	 	if (!mResponded[i])
	 	{
			errmsg.setInt(i);
			num_failed++;
		}
	}

	errmsg.setInt(-1);
	logError(errmsg);
	return true;
}


bool
AosHeartbeatMgr::startHeartbeat()
{
	// It sends a heartbeat message to every monitored. 
	aos_assert_r(mConn, false);
	memset(mResponded, 0, mMonitored.size());
	mNumResponded = 0;
	for (u32 i=0; i<mMonitored.size(); i++)
	{
		AosBuff buff;
		buff.setOmnStr("hb_request");
		buff.setInt(i);
		mComm->writeTo(buff.data(), buff.dataLen(), 
				mMonitored[i].ip_addr, mMonitored[i].port);
	}
	return true;
}


bool
AosHeartbeatMgr::signal(const int threadLogicId)
{
    return true;
}


bool
AosHeartbeatMgr::checkThread(OmnString &errmsg) const
{
    return true;
}


bool
AosHeartbeatMgr::addMonitor(
		const OmnString &ip_addr, 
		const int port, 
		const OmnString &name, 
		const OmnString &type) 
{
	// Add the monitored into the hash table.
	Monitored monitored(ip_addr, port, name, type);
	mMonitored.push_back(monitored);
	return true;
}


bool        
AosHeartbeatMgr::msgRead(const OmnConnBuffPtr &buff)
{
	AosBuff bb(buff->getData(), buff->getDataLength());
	OmnString type = bb.getOmnStr("");
	if (type == "hb_request") return procHeartbeatRequest(buff, bb);
	if (type == "hb_resp") return procHeartbeatResp(buff);

	OmnAlarm << "Unrecognized message: " << type << enderr;
	return false;
}


bool
AosHeartbeatMgr::processHeartbeatRequest(
		const OmnConnBuffPtr &buff, 
		AosBuff &input_buff)
{
	// A hearbeat request message is:
	// 	"hb_request" + device_id
	int device_id = input_buff.getInt(-1);

	AosBuff bb;
	bb.setOmnStr("hb_resp");
	bb.setInt(device_id);
	bb.setInt(eStatusSuccess);

	OmnIpAddr remote_addr = buff->getRemoteAddr();
	int remote_port = buff->getRemotePort();
	mConn->sendTo(bb.data(), bb.length(), remote_addr, remote_port);
	return true;
}
	

bool
AosHeartbeatMgr::procHeartbeatResp(
		const OmnConnBuffPtr &buff, 
		AosBuff &input_buff)
{
	int device_id = input_buff.getInt(-1);
	int status = input_buff.getInt(-1);
	aos_assert_r(device_id >= 0 && (u32)device_id < mMonitored.size(), false);

	int status = bb.getInt(-1);
	aos_assert_r(status > 0, false);

	if (mResponded[device_id] == 0)
	{
		mNumResponded++;
	}

	mResponded[device_id] = status;
	return true;
}


void        
AosHeartbeatMgr::readingFailed()
{
	OmnAlarm << "Reading failed" << enderr;
}


void
AosHeartbeatMsg::logError(const AosBuff &buff)
{
	// not implemented yet;
	return;
}

