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
#include "SvrProxyMgr/NetSvrInfo.h"

#include "API/AosApi.h"
#include "SvrProxyMgr/SvrMonitor.h"

AosNetSvrInfo::AosNetSvrInfo(
		const AosSvrMonitorPtr &svr_monitor,
		const int svr_id, 
		const u32 m_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		bool show_log)
:
AosSvrInfo(m_cache_size, recv_hd, show_log),
mSvrMonitor(svr_monitor),
mSvrId(svr_id),
mInit(false),
mSvrReady(false),
mSvrStopping(false),
mRecvConnSockTime(0),
mConnCheck(false)
{
}


AosNetSvrInfo::~AosNetSvrInfo()
{
}


bool
AosNetSvrInfo::stop()
{
	if(mSvrId == AosGetSelfServerId())	return true;
	return AosSvrInfo::stop();
}


bool
AosNetSvrInfo::setConnSock(const int sock_id)
{
	mRecvConnSockTime = OmnGetTimestamp();

	return AosSvrInfo::setConnSock(sock_id);
}


bool
AosNetSvrInfo::setConnFailed()
{
	OmnScreen << "<ProxyMsg> ConnFailed!!!; " << mSvrId << endl;
	
	mSvrReady = false;	
	
	return AosSvrInfo::setConnFailed();
}


bool
AosNetSvrInfo::setSvrDeath()
{
	OmnScreen << "<ProxyMsg> SvrDeath!!!!!; " << mSvrId << endl;
	
	mSvrReady = false;
	
	return AosSvrInfo::setSvrDeath();
}


bool
AosNetSvrInfo::setSvrReady()
{
	bool rslt = svrIsUp();
	aos_assert_r(rslt, false);

	if (!isConnGood())
	{
		OmnAlarm << "set setSvrReady error, connetion is not good"	 << enderr;
	}
	
	mInit = true;
	mSvrReady = true;
	mConnCheck = false;
	return true;
}


bool
AosNetSvrInfo::isNeedConnect()
{
	OmnScreen << "================deubg============mSvrReady: " << mSvrReady << " , isConnGood(): " << isConnGood() << endl;
	if(mSvrReady)	return false;
	
	if(!isConnGood())	return true;
	
	aos_assert_r(mRecvConnSockTime, false);
	u64 wait_svrup_timeout = mConnCheck ? eWaitSvrUpTimeOut2 : eWaitSvrUpTimeOut;
	u64 time_sec = (OmnGetTimestamp() - mRecvConnSockTime) / 1000 / 1000;
	if(time_sec < wait_svrup_timeout) return false;

	mConnCheck = true;
	OmnScreen << "<ProxyMsg::Error> NetSvr thouth conn is good, but maybe svr not good. "
		<< "svr: " << mSvrId << "; "
		<< "sock: " << getSockId() << "; "
		<< endl; 
	return true;
}


bool
AosNetSvrInfo::sendSysMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(isConnGood(), false);

	// ignore mSvrDeath flag. if svr is death. but conn is good. need send this msg.
	bool succ;
	bool rslt = sendAppMsgPriv(msg, succ);
	aos_assert_r(rslt && succ, false);
	return true;
}


bool
AosNetSvrInfo::sendAppMsg(const AosAppMsgPtr &msg, bool &succ)
{
	succ = false;
	if(!mSvrReady)		return true;
	if(mSvrStopping) 	return true;
	return AosSvrInfo::sendAppMsg(msg, succ);
}


bool
AosNetSvrInfo::sendTrans(const AosTransPtr &trans)
{
	//if (mShowLog)
	//{
	//	OmnScreen << "SvrProxy Send trans:" 
	//		<< trans->getTransId().toString()
	//		<< "; svr_ready:" << mSvrReady
	//		<< "; svr_stopping:" << mSvrStopping
	//		<< endl;
	//}
	
	if(!mSvrReady && !isDeath())		return true;
	if(mSvrStopping)	return true;
	return AosSvrInfo::sendTrans(trans);
}


void
AosNetSvrInfo::setStopping()
{
	// just that svr is stoping, not self svr
	// the conn can't stop. this stopping svr will still send fmt.
	mSvrStopping = true;
}


bool
AosNetSvrInfo::getId(bool &is_icp_conn, u32 &id)
{
	is_icp_conn = false;
	id = mSvrId;
	return true;
}
	
void
AosNetSvrInfo::updateHeartbeat()
{
	mSvrMonitor->recvHeartBeat(mSvrId);
}

void
AosNetSvrInfo::updateSockLastRecvTime()
{
	mSvrMonitor->updateSockLastRecvTime(mSvrId);
}

void
AosNetSvrInfo::repaireConnect()
{
	mSvrMonitor->repaireConnect(mSvrId);
}
