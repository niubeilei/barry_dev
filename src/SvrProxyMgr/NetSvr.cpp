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
// Created: 04/25/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/NetSvr.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/ServerInfo.h"
#include "SvrProxyMgr/NetSvrInfo.h"
#include "SvrProxyMgr/SockConn.h"
#include "TransBasic/AppMsg.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/SvrMonitor.h"
#include "TransBasic/Trans.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"

#include "SysMsg/AddServerMsg.h"
#include "SysMsg/SvrUpMsg.h"
#include "SysMsg/SvrDeathMsg.h"
#include "SysMsg/SvrStoppedMsg.h"

#include <netinet/tcp.h>

AosNetSvr::AosNetSvr(
		const AosSvrProxyPtr &svr_proxy,
		const bool showlog)
:
mLock(OmnNew OmnMutex()),
mSvrProxy(svr_proxy),
mListenSock(-1),
mCrtJudgerSvrId(0),
mShowLog(showlog),
mStopping(false)
{
}


AosNetSvr::~AosNetSvr()
{
}


bool
AosNetSvr::config(const AosXmlTagPtr &conf)
{
	AosNetSvrPtr net_svr(this, false);
	mSvrMonitor = OmnNew AosSvrMonitor(net_svr, mShowLog);
	mLocalAddr = conf->getAttrStr(AOSCONFIG_LOCAL_ADDR, "0.0.0.0");
	mLocalPort = conf->getAttrInt(AOSCONFIG_LOCAL_PORT, -1);
	aos_assert_r(mLocalPort != -1, false);

	return true;
}

bool
AosNetSvr::startListen()
{
	aos_assert_r(mLocalAddr!= "" && mLocalPort > 0, false);

	mListenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (mListenSock == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	
	const int on = 1;
	setsockopt(mListenSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	struct sockaddr_in serveraddr;
	bzero(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	inet_aton(mLocalAddr.data(),&(serveraddr.sin_addr));
	serveraddr.sin_port = htons(mLocalPort);
	
	int rslt = ::bind(mListenSock,(sockaddr *)&serveraddr, sizeof(serveraddr));
	if (rslt == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	
	rslt = listen(mListenSock, 100);
	if (rslt == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	
	OmnThreadedObjPtr thisPtr(this, false);
	mConnThrd = OmnNew OmnThread(thisPtr,
			"NetSvrConnFunc", eAcceptConnThrdId, true, true, __FILE__, __LINE__);
	mCheckConnThrd = OmnNew OmnThread(thisPtr, 
			"NetSvrCheckFunc", eCheckConnThrdId, true, true, __FILE__, __LINE__);

	//Jozhi 2015-11-26 SvrMonitor init move to config 
	//AosNetSvrPtr net_svr(this, false);
	//mSvrMonitor = OmnNew AosSvrMonitor(net_svr, mShowLog);
	startConnThrd();
	
	return true;
}


bool
AosNetSvr::init(
		const AosXmlTagPtr &svrs_config,
		const AosXmlTagPtr &admin_config,
		const int crt_judger_sid)
{
	if(crt_judger_sid != -1)	setNewJudgerSvrId(crt_judger_sid);

	mSvrsConfig = svrs_config;
	mAdminConfig = admin_config;
	return true;
}


bool
AosNetSvr::start()
{
	OmnScreen << "<ProxyMsg> NetSvr starting" << endl;

	AosNetSvrPtr thisptr(this, false);
	aos_assert_r(mSvrMonitor, false);
	bool rslt = mSvrMonitor->start();
	aos_assert_r(rslt, false);
	
	int self_svr_id = AosGetSelfServerId();
	aos_assert_r(self_svr_id >= 0 && (u32)self_svr_id < eAosMaxServerId, false);
	bool self_is_judger = selfIsJudgerSvr();
	
	vector<u32> & svr_ids = AosGetServerIds();
	for(u32 i=0; i<svr_ids.size(); i++)
	{
		int svr_id = svr_ids[i];
		AosNetSvrInfoPtr svr = getSvr(svr_id, true);
		aos_assert_r(svr, false);

		if(svr_id < self_svr_id)
		{
			if(!svr->isConnGood())	continue;
			
			rslt = sendSvrReadyMsg(svr);
			aos_assert_r(rslt, false);
			continue;
		}
		if(svr_id == self_svr_id) continue;

		if(!svr->isConnGood())
		{
			rslt = connectSvr(svr, eMaxConnTrys);
			aos_assert_r(rslt, false);
		}
		else
		{
			// if svr conn is good. recved by accept thrd. maybe this svr is the crt judger svr.
			// don't setConnSock.
			OmnScreen << "<ProxyMsg> NetSvr connect to svr. but has recved this conn. "
				<< "svr: " << svr_id << "; must be from real judger svr." 
				<< endl;
		}
		if(!svr->isConnGood()) continue;
		
		if(self_is_judger)
		{
			rslt = sendAddServerMsg(svr);
			aos_assert_r(rslt, false);
		}
		// if call this func. self must be loaded.
		rslt = sendSvrReadyMsg(svr); 
		aos_assert_r(rslt, false);
	}
	startCheckConnThrd();
	return true;
}


bool
AosNetSvr::selfIsJudgerSvr()
{
	aos_assert_r(mSvrProxy->svrIsLoaded(), false);

	return (u32)AosGetSelfServerId() == mCrtJudgerSvrId;
}


bool
AosNetSvr::sendSvrReadyMsg(const AosNetSvrInfoPtr &svr)
{
	OmnScreen << "<ProxyMsg> NetSvr send self is up to svr: " << svr->getSvrId() << endl;
	
	AosAppMsgPtr msg = OmnNew AosSvrUpMsg(svr->getSvrId(), AosGetSelfServerId());
	bool rslt = svr->sendSysMsg(msg);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetSvr::notifyIsStopping()
{
	mStopping = true;
	
	bool rslt;
	int self_svr_id = AosGetSelfServerId();
	int to_svr_id;
	AosNetSvrInfoPtr svr;
	AosAppMsgPtr msg;
	map<int, AosNetSvrInfoPtr>::iterator itr;
	for(itr = mSvrs.begin(); itr != mSvrs.end(); itr++)
	{
		to_svr_id = itr->first;
		svr = itr->second; 
		if(to_svr_id == self_svr_id)	continue;
	
		msg = OmnNew AosSvrStoppedMsg(to_svr_id, AosGetSelfServerId());

		rslt = svr->sendSysMsg(msg);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosNetSvr::stop()
{
	close(mListenSock);
	mListenSock = -1;
	
	map<int, AosNetSvrInfoPtr>::iterator itr = mSvrs.begin(); 
	for(; itr != mSvrs.end(); itr++)
	{
		AosNetSvrInfoPtr svr= itr->second;
		svr->stop();
	}
	
	if (mConnThrd) mConnThrd->stop();
	return true;
}

void
AosNetSvr::startConnThrd()
{
	OmnThrdStatus::E sts = mConnThrd->getStatus();
	if(sts != OmnThrdStatus::eActive)
	{
		mConnThrd->start();
	}
}


void
AosNetSvr::startCheckConnThrd()
{
	OmnThrdStatus::E sts = mCheckConnThrd->getStatus();
OmnScreen << "====================debug==================thread status: " << sts << endl;
	if(sts != OmnThrdStatus::eActive)
	{
		OmnScreen << "<ProxyMsg> NetSvr start checkConnThrd. " << endl;
	
		mCheckConnThrd->start();
	}
}


bool
AosNetSvr::connectSvr(const AosNetSvrInfoPtr &svr, int trys)
{
	aos_assert_r(svr, false);
	int svr_id = svr->getSvrId();
	aos_assert_r(svr_id >=0 && (u32)svr_id < eAosMaxServerId, false);

	OmnScreen << "<ProxyMsg> NetSvr try to connect svr: " << svr_id << endl;

	AosServerInfoPtr svr_info = AosGetSvrInfo(svr_id);
	aos_assert_r(svr_info, false);
	OmnString addr_str = svr_info->getAddr();
	int port = svr_info->getPort();
	aos_assert_r(addr_str != "" && port > 0, false);

	struct sockaddr_in svr_addr;
	bzero(&svr_addr, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	inet_aton(addr_str.data(),&(svr_addr.sin_addr));
	svr_addr.sin_port = htons(port);

	int new_sock = socket(AF_INET, SOCK_STREAM, 0);
	aos_assert_r(new_sock != -1, false);

	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	int ret = setsockopt(new_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
	aos_assert_r(ret != -1, false);
	
	bool rslt = bindSelfConnSock(new_sock);
	aos_assert_r(rslt, false);
	while(trys--)
	{
		int rslt = connect(new_sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr));
		if(rslt != 0)
		{
			OmnScreen << "==============debug================errno: " << errno << endl;
			if(trys)	OmnSleep(eReConnTime);
			continue;
		}
		
		char *ip =  inet_ntoa(svr_addr.sin_addr);
		int port = ntohs(svr_addr.sin_port);
		OmnString s;
		s << ip << ":" << port;
		svr->setName(s);
		
		rslt = svr->setConnSock(new_sock);
		aos_assert_r(rslt, false);
		OmnScreen << "<ProxyMsg> NetSvr recv conn by connect. "
			<< "svr: " << svr_id << "; " 
			<< "sock: " << new_sock << "; " 
			<< endl; 
		return true;
	}

	::close(new_sock);

	OmnScreen << "<ProxyMsg> NetSvr fail to connect svr: " << svr_id << endl;
	return true;
}


bool
AosNetSvr::bindSelfConnSock(const int sock_id)
{
	aos_assert_r(AosGetSelfServerId() >= 0 && (u32)AosGetSelfServerId() < eAosMaxServerId, false);
	AosServerInfoPtr svr_info= AosGetSvrInfo(AosGetSelfServerId());
	OmnString addr = svr_info->getAddr();
	
	struct sockaddr_in self_addr;
	bzero(&self_addr, sizeof(self_addr));
	self_addr.sin_family = AF_INET;
	inet_aton(addr.data(),&(self_addr.sin_addr));

	int rslt = -1;
	u32 trys = 0;
	u32 port = 0;
	while(rslt == -1)
	{
		port = randomGetPort(AosGetSelfServerId(), trys+1);	
		aos_assert_r(port, false);	
		self_addr.sin_port= htons(port);
	
		rslt = ::bind(sock_id,(sockaddr *)&self_addr, sizeof(self_addr));
		trys++;
		if(trys == 300)
		{
			OmnAlarm << "error! "
				<< " addr: " << addr
				<< " port: " << port
				<< " errno:" << errno
				<< enderr;
			return false;
		}
	}

	OmnScreen << "============debug==========connect to other server bind self port: " << port << endl;
	return true;
}


u32
AosNetSvr::randomGetPort(const int svr_id, const int trys)
{
	aos_assert_r(svr_id >= 0 && (u32)svr_id < eAosMaxServerId, 0);
	//u32 or_port = (rand() % 1000) + 5200;
	//u32 port = (or_port) / 1024 * 1024 + svr_id;
	//u32 port = (or_port) / 97 * 97 + svr_id;
	u32 port = 5000 + trys*100 + svr_id;
//OmnScreen << "*****************rand port: " << port << " , svr id: " << svr_id << endl;
	return port;
}


int 
AosNetSvr::getSvrIdByPort(const int clt_port)
{
	aos_assert_r(clt_port >= 5000, -1);
	//return clt_port % 1024;
	//return clt_port % 97;
//OmnScreen << "*****************clt port: " << clt_port << endl;
	return clt_port % 100;
}

bool
AosNetSvr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u64 thrd_id = thread->getLogicId();

	if(thrd_id == eAcceptConnThrdId)
	{
		return acceptConnThrdFunc(state, thread);
	}

	if(thrd_id == eCheckConnThrdId)
	{
		return checkConnThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thrd_id << enderr;
	return false;
}


bool
AosNetSvr::acceptConnThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct sockaddr_in clt_addr;
	socklen_t addr_len = sizeof(sockaddr_in);
	
	OmnString clt_addr_str;
	int clt_port;
	int clt_id;
	AosSockConnPtr conn;
	bool rslt;

	while(state == OmnThrdStatus::eActive)
	{
		int clt_sock = accept(mListenSock,(sockaddr *)&clt_addr, &addr_len);
		if(mStopping)	return true;

		if(clt_sock <0)
		{
			int err = errno;
			OmnAlarm << "error clt_sock:" << clt_sock
				<< "; errno:" << err
				<< enderr;
			continue;
		}
	
		// Disable the Nagle (TCP No Delay) algorithm
		int flag = 1;
		int ret = setsockopt(clt_sock, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );
		aos_assert_r(ret != -1, false);

		clt_port = ntohs(clt_addr.sin_port);
		clt_id = getSvrIdByPort(clt_port); 
		if(clt_id<0 || (u32)clt_id >= eAosMaxServerId)
		{
			OmnAlarm << "Invalid svr id: " << clt_id << enderr;   
			continue;
		}
		
		AosNetSvrInfoPtr svr = getSvr(clt_id, true);
		aos_assert_r(svr, false);
	
		if (mSvrProxy->svrIsLoaded() && clt_id >= AosGetNumPhysicals())
		{
			mLock->lock();
			map<int, AosNetSvrInfoPtr>::iterator itr = mSvrs.find(clt_id);
			if(itr != mSvrs.end())
			{
				OmnScreen << "may be and error, invalid clt_id: " << clt_id << endl;
				mSvrs.erase(itr);
			}
			mLock->unlock();
			continue;
		}

		char *ip =  inet_ntoa(clt_addr.sin_addr);
		int port = ntohs(clt_addr.sin_port);
		OmnString s;
		s << ip << ":" << port;
		svr->setName(s);

		rslt = svr->setConnSock(clt_sock);
		aos_assert_r(rslt, false);
		
		OmnScreen << "============debug=============NetSvr accept. "
			<< "svr: " << clt_id << "; " 
			<< "sock: " << clt_sock << endl; 
	
		if(!mSvrProxy->svrIsLoaded())	continue;
		rslt = sendSvrReadyMsg(svr); 
		aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosNetSvr::checkConnThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	OmnScreen << "<ProxyMsg> NetSvr checkConnThrd started. " << endl;

	map<int, AosNetSvrInfoPtr>::iterator itr; 
	bool rslt;
	bool all_succ = true;
	int self_svr_id = AosGetSelfServerId();
	aos_assert_r(self_svr_id >= 0 && (u32)self_svr_id < eAosMaxServerId, false);
	bool self_is_judger = selfIsJudgerSvr();
	while(state == OmnThrdStatus::eActive)
	{
		all_succ = true;
		for(itr = mSvrs.begin(); itr != mSvrs.end(); itr++)
		{
			int svr_id = itr->first;
			AosNetSvrInfoPtr svr = itr->second;
			if(!self_is_judger && svr_id < self_svr_id)	continue;
			if(svr_id == 0)		continue;	// this is Temp. will delete later.
			if(svr_id == self_svr_id)	continue;
			if(svr->isSvrReady())	continue;
			
			all_succ = false;
			if(!svr->isNeedConnect())	continue;

			rslt = connectSvr(svr, eMaxConnTrys);
			aos_assert_r(rslt, false);	
			if(!svr->isConnGood())	continue;
			
			if(self_is_judger)
			{
				rslt = sendAddServerMsg(svr);
				aos_assert_r(rslt, false);
			}
		OmnScreen << "============debug=============NetSvr connect to svr. " << "svr: " << svr_id << endl;

			rslt = sendSvrReadyMsg(svr); 
			aos_assert_r(rslt, false);
		}

		if(all_succ)	break;
		OmnSleep(eReConnTime);
		break;
	}

	thread->stop();
	
	OmnScreen << "<ProxyMsg> NetSvr checkConnThrd stopped. " << endl;
	return true;
}

bool
AosNetSvr::recvSvrReady(const int svr_id)
{
	OmnScreen << "<ProxyMsg> NetSvr RecvSvrReady; " << svr_id << endl;
	
	AosNetSvrInfoPtr svr = getSvr(svr_id, true);
	aos_assert_r(svr, false);	
	svr->setSvrReady();
	return true;
}


bool
AosNetSvr::sendAddServerMsg(const AosNetSvrInfoPtr &svr)
{
	aos_assert_r(selfIsJudgerSvr(), false);
	aos_assert_r(mSvrsConfig && mAdminConfig, false);
	
	OmnScreen << "<ProxyMsg> NetSvr send server config to svr: "
		<< svr->getSvrId() << "; " 
		<< endl;
	
	int svr_id = svr->getSvrId();
	AosXmlTagPtr sconfig = mSvrsConfig->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr aconfig = mAdminConfig->clone(AosMemoryCheckerArgsBegin);
	sconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	sconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);
	AosAppMsgPtr msg = OmnNew AosAddServerMsg(mCrtJudgerSvrId,
			        sconfig, aconfig, svr_id, 0);
	aos_assert_r(msg, false);
	
	bool rslt = svr->sendSysMsg(msg);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetSvr::sendSvrDeathMsg(const int to_sid, const int death_sid)
{
	OmnScreen << "<ProxyMsg> NetSvr send deathed_svr to svr; "
		<< "to_svr: " << to_sid << "; "
		<< "death_svr:" << death_sid << "; "
		<< endl;
			
	AosAppMsgPtr msg = OmnNew AosSvrDeathMsg(to_sid, death_sid);
	
	AosNetSvrInfoPtr to_svr = getSvr(to_sid);
	aos_assert_r(to_svr, false);	
	bool rslt = to_svr->sendSysMsg(msg);
	aos_assert_r(rslt, false);
	return true;	
}


AosNetSvrInfoPtr
AosNetSvr::getSvr(const int svr_id, const bool create)
{
	aos_assert_r(svr_id >= 0 && (u32)svr_id < eAosMaxServerId, 0);

	mLock->lock();
	AosNetSvrInfoPtr svr;
	map<int, AosNetSvrInfoPtr>::iterator itr = mSvrs.find(svr_id);
	if(itr != mSvrs.end())
	{
		svr = itr->second;
		mLock->unlock();
		return svr;
	}

	if(!create)	
	{
		mLock->unlock();
		return 0;
	}
	
	itr = mSvrs.find(svr_id);
	if(itr != mSvrs.end())
	{
		svr = itr->second;
		mLock->unlock();
		return svr;
	}

	//AosNetSvrPtr thisptr(this, false);
	AosRecvEventHandlerPtr recv_hd = mSvrProxy;
	u32 max_cache_size = mSvrProxy->getMaxCacheSize();
	//svr = OmnNew AosNetSvrInfo(thisptr, svr_id, 
	svr = OmnNew AosNetSvrInfo(mSvrMonitor, svr_id, 
			max_cache_size, recv_hd, mShowLog);
	mSvrs[svr_id] = svr;
	
	mLock->unlock();
	return svr;
}


bool
AosNetSvr::setConnFailed(const int svr_id)
{
	// SvrMonitor call this func.
	OmnScreen << "===============debug================connnect failed, svr_id:" << svr_id << endl;
	AosNetSvrInfoPtr svr = getSvr(svr_id);
	aos_assert_r(svr, false);
	svr->setConnFailed();
	startCheckConnThrd();
	return true;
}

bool
AosNetSvr::stop(const int svr_id)
{
	AosNetSvrInfoPtr svr = getSvr(svr_id);
	aos_assert_r(svr, false);
	svr->stop();
	return true;
}

bool
AosNetSvr::repaireConnect(const int svr_id)
{
	aos_assert_r(svr_id > AosGetSelfServerId(), false);
	AosNetSvrInfoPtr svr = getSvr(svr_id);
	aos_assert_r(svr, false);

	bool rslt = connectSvr(svr, eMaxConnTrys);
	aos_assert_r(rslt, false);	

	if(!svr->isConnGood()) return false;

	rslt = sendSvrReadyMsg(svr);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetSvr::sendTrans(const int to_svr_id, const AosTransPtr &trans)
{
	aos_assert_r(to_svr_id >= 0 && (u32)to_svr_id < eAosMaxServerId, false);
	aos_assert_r(to_svr_id != AosGetSelfServerId(), false);
	aos_assert_r(trans, false);	
	
	AosNetSvrInfoPtr svr = getSvr(to_svr_id);
	if (!svr)
	{
		OmnAlarm << "get to svr failed: " << to_svr_id << enderr;
	}
	aos_assert_r(svr, false);

	bool rslt = svr->sendTrans(trans.getPtr());
	aos_assert_r(rslt, false);
	return true;
}

bool
AosNetSvr::sendAppMsg(
		const int to_svr_id,
		const AosAppMsgPtr &msg,
		bool &succ)
{
	aos_assert_r(to_svr_id >= 0 && (u32)to_svr_id < eAosMaxServerId, false);
	aos_assert_r(to_svr_id != AosGetSelfServerId(), false);
	aos_assert_r(msg, false);	

	AosNetSvrInfoPtr svr = getSvr(to_svr_id);
	aos_assert_r(svr, false);
	
	bool rslt = sendAppMsgPriv(svr, msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetSvr::sendAppMsgPriv(
		const AosNetSvrInfoPtr &svr,
		const AosAppMsgPtr &msg,
		bool &succ)
{
	aos_assert_r(svr && msg, false);	

	bool rslt = svr->sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	if(succ)	return true;

	// continue send this msg.
	while(1)
	{
		rslt = svr->sendAppMsg(msg, succ);
		aos_assert_r(rslt, false);
		if(succ)	return true;

		if(svr->isDeath())	return true;

		OmnScreen << "continue send msg; "
			<< "msg:" << msg->getStrType() << "; "
			<< "to_sid:" << svr->getSvrId()
			<< endl;
		OmnSleep(1);
	}
	return false;
}


bool
AosNetSvr::recvSvrDeath(const int death_svr_id)
{
	aos_assert_r(mSvrProxy->svrIsLoaded(), false);
	
	OmnScreen << "!!!! recv svr death!!" << death_svr_id << endl;

	AosNetSvrInfoPtr svr = getSvr(death_svr_id, true);
	aos_assert_r(svr, false);
	svr->setSvrDeath();
	if(mCrtJudgerSvrId == (u32)death_svr_id)
	{
		// set new judger svr id.
		int next_sid = calcuNextJudgerSvrId(mCrtJudgerSvrId);
		aos_assert_r(next_sid != -1, false);
		
		setNewJudgerSvrId(next_sid);
	}
	startCheckConnThrd();
	return true; 
}


void
AosNetSvr::setNewJudgerSvrId(const int judger_sid)
{
	OmnScreen << "<ProxyMsg> NetSvr new judger svr: " << judger_sid << endl;
	mCrtJudgerSvrId = judger_sid;
}


bool
AosNetSvr::recvSvrStopping(const int svr_id)
{
	if(svr_id == AosGetSelfServerId())	return true;
	AosNetSvrInfoPtr svr = getSvr(svr_id);
	aos_assert_r(svr, false);

	svr->setStopping();
	return true;
}


bool
AosNetSvr::recvResendEnd(const u32 from_sid, const u32 from_pid, const int target_sid)
{
	AosNetSvrInfoPtr svr = getSvr(target_sid);
	aos_assert_r(svr, false);
	
	return svr->recvResendEnd(from_sid, from_pid);
}


bool
AosNetSvr::monitorSvrDeath(const int death_svr_id)
{
	// SysMonitor func call this.
	aos_assert_r(death_svr_id >= 0 && (u32)death_svr_id < eAosMaxServerId, false);
	OmnScreen << "<ProxyMsg> NetSvr monitor svr death! "
		<< "death_svr:" << death_svr_id
		<< endl;
	
	AosNetSvrInfoPtr svr = getSvr(death_svr_id);
	aos_assert_r(svr, false);
	
	if(!svr->isInited())
	{
		OmnScreen << "<ProxyMsg> NetSvr monitor svr death! "
			<< "death_svr:" << death_svr_id << "; "
			<< "but this svr never start before. wait some preson mark it death "
			<< "or wait it up."
			<< endl;
		return true;
	}

	bool rslt = recvSvrDeath(death_svr_id);
	aos_assert_r(rslt, false);

	AosAppMsgPtr msg = OmnNew AosSvrDeathMsg(0, death_svr_id);
	rslt = broadcastMsg(msg);
	aos_assert_r(rslt, false);

	return mSvrProxy->monitorSvrDeath(death_svr_id);
}


int
AosNetSvr::calcuNextJudgerSvrId(const int crt_judger_sid)
{
	map<int, AosNetSvrInfoPtr>::iterator itr = mSvrs.find(crt_judger_sid);
	aos_assert_r(itr != mSvrs.end(), false);
	
	int times = mSvrs.size();
	int crt_sid;
	AosNetSvrInfoPtr svr;
	while(times--)
	{
		itr++;
		if(itr == mSvrs.end())	itr = mSvrs.begin();
		
		crt_sid = itr->first;
		svr = itr->second;
		if(svr->isDeath())
		{
			OmnScreen << "<ProxyMsg> NetSvr calculate next Judger svr; "
				<< "svr death:" << crt_sid << endl;
			continue;
		}
		
		return crt_sid;
	}
	
	OmnShouldNeverComeHere;
	return -1;
}


bool
AosNetSvr::broadcastMsg(const AosAppMsgPtr &msg)
{
	// broadcast all other SvrProxy. ignore self svr.
	aos_assert_r(msg, false);

	int self_svr_id = AosGetSelfServerId();
	bool rslt, succ;
		
	int svr_id;
	AosNetSvrInfoPtr svr;
	map<int, AosNetSvrInfoPtr>::iterator itr;
	for(itr = mSvrs.begin(); itr != mSvrs.end(); itr++)
	{
		AosNetSvrInfoPtr svr = itr->second; 
		svr_id = itr->first;
		if(svr_id == self_svr_id)	continue;

		AosConnMsgPtr c_msg = msg->copy();
		AosAppMsgPtr new_msg = (AosAppMsg *)c_msg.getPtr();
		new_msg->setToSvrId(svr_id);
	
		rslt = sendAppMsgPriv(svr, new_msg, succ);
		aos_assert_r(rslt, false);
	}
	return true;
}
