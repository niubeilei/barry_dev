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
#include "SvrProxyMgr/SvrProxy.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "SvrProxyMgr/TransHandler.h"
#include "SvrProxyMgr/CubeProcess.h"
#include "SvrProxyMgr/BkpCubeProcess.h"
#include "SvrProxyMgr/NetSvr.h"
#include "SvrProxyMgr/NetSvrInfo.h"
#include "SvrProxyMgr/SockConn.h"
#include "SvrProxyMgr/ResendCheckThrd.h"
#include "TransBasic/AppMsg.h"
#include "TransBasic/Trans.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

#include "SysMsg/SwitchToMasterMsg.h"
#include "SysMsg/SetNewMasterMsg.h"	
#include "SysMsg/AddServerMsg.h"
#include "SysMsg/AddClusterMsg.h"

#include "SysMsg/TriggerResendMsg.h"
#include "SysMsg/ResendEndMsg.h"	

#include "SysMsg/GetCrtMastersMsg.h"
#include "SysMsg/SendCrtMastersMsg.h"
#include "SysMsg/GetClusterConfigMsg.h"
#include "TransUtil/TransAckMsg.h"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

ofstream AosSvrProxy::smLogFile;

map<OmnString, AosBuffPtr> AosSvrProxy::smTestBuff;
map<OmnString, AosBuffPtr> AosSvrProxy::smTestIOVBuff;

AosSvrProxy::AosSvrProxy()
:
mLock(OmnNew OmnMutex()),
mInitLock(OmnNew OmnMutex()),
mSwitchLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMastersCondVar(OmnNew OmnCondVar()),
mClusterCondVar(OmnNew OmnCondVar()),
mMaxConnCacheSize(0),
mSvrLoaded(false),
mClusterLoaded(false),
mMastersLoaded(false),
mShowLog(false)
{
}


AosSvrProxy::~AosSvrProxy()
{
}

bool
AosSvrProxy::config(const AosXmlTagPtr &app_conf)
{
	initLogFile();
	
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild("SvrProxy");
	aos_assert_r(conf, false);

	mRecvThrdNum = conf->getAttrU32("recv_thrd_num", 5); 
	mMaxConnCacheSize = conf->getAttrU64(AOSCONFIG_TRANS_MAXCACHESIZE, 500) * 1000000;
	mShowLog = conf->getAttrBool(AOSCONFIG_SHOWLOG, 0);

	AosSvrProxyPtr thisptr(this, false);
	mProcessMgr = OmnNew AosProcessMgr(thisptr, mShowLog);
	mNetSvr = OmnNew AosNetSvr(thisptr, mShowLog);
	mTransHandler = OmnNew AosTransHandler(thisptr, app_conf);		// will init crt master.

	mProcessMgr->config(conf);
	mNetSvr->config(app_conf);

	OmnThreadedObjPtr thisPtr(this, false);
	//Jozhi JIMODB-1101
	//mInitThrd = OmnNew OmnThread(thisPtr, 
	//		"NetSvrCheckFunc", eInitThrdId, true, true, __FILE__, __LINE__);
	for(u32 i=0; i<mRecvThrdNum; i++)
	{
		// recv msg Thread.
		mRecvThrd[i] = OmnNew OmnThread(thisPtr, "SvrProxyRecvFunc",
				eRecvThrdId+ i, true, true, __FILE__, __LINE__);
	}

	// Ketty 2014/06/17
	mResendCheckThrd = OmnNew AosResendCheckThrd();; 
	return true;
}


bool
AosSvrProxy::initLogFile()
{
	if (smLogFile.is_open())	return true;
		
	OmnString fname = OmnApp::getAppBaseDir();
	fname << "svrproxy_log.txt";
	smLogFile.open(fname.data(), ios::app|ios::out);
	return true;
}


void
AosSvrProxy::log(const ostringstream &data)
{	
	smLogFile << data.str(); 
	smLogFile.flush();

	cout << data.str();
	//cout.flush();
}


bool
AosSvrProxy::start(int argc, char **argv)
{
	for(u32 i=0; i<mRecvThrdNum; i++)
	{
		mRecvThrd[i]->start(); 
	}
	
	mNetSvr->startListen();
	mProcessMgr->start(argc, argv);
	return true;
}


bool
AosSvrProxy::stop()
{
	mNetSvr->notifyIsStopping();
	mProcessMgr->stopAll();
	
	OmnSleep(5);
	
	mNetSvr->stop();
	return true;
}

bool
AosSvrProxy::kill()
{
	mProcessMgr->killAll();
	return true;
}

bool
AosSvrProxy::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u32 thread_id = thread->getLogicId();
	if(thread_id >= eRecvThrdId && thread_id < (eRecvThrdId + mRecvThrdNum))
	{
		u32 recv_tid = thread_id - eRecvThrdId;
		aos_assert_r(recv_tid < mRecvThrdNum, false); 
		return recvThrdFunc(state, thread, recv_tid);
	}
	
	if(thread_id == eInitThrdId)
	{
		bool rlst = initThrdFunc(state, thread);
		OmnSleep(3);
		return rlst;
	}

	OmnAlarm << "Invalid thread logic id: " << thread->getLogicId() << enderr;
	return false;
}


bool
AosSvrProxy::msgRecvEvent(const AosSockConnPtr &conn)
{
	mLock->lock();
	mEventConns.push_back(conn);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosSvrProxy::recvThrdFunc(
		OmnThrdStatus::E &state, 
		const OmnThreadPtr &thread,
		const u32 crt_thrdid)
{
	while(state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if(mEventConns.empty())
		{
			bool timeout = false;
			mCondVar->timedWait(mLock, timeout, 5);
			mLock->unlock();
			continue;
		}
		
		AosSockConnPtr conn;
		list<AosSockConnPtr>::iterator itr = mEventConns.begin();
		for(; itr !=mEventConns.end(); itr++)
		{
			AosSockConnPtr check_conn = *itr;

			bool can_proc = true;
			for(u32 i=0; i<mRecvThrdNum; i++)
			{
				can_proc = (mProcingConn[i] != check_conn);
				if(!can_proc)	break;
			}
			if(can_proc)
			{
				conn = check_conn;
				break;
			}
		}
		
		if(!conn)
		{
			// not find.
			bool timeout;
			mCondVar->timedWait(mLock, timeout, 1);
			mLock->unlock();
			continue;
		}

		aos_assert_rl(itr != mEventConns.end(), mLock, false);
		mEventConns.erase(itr);
		mProcingConn[crt_thrdid] = conn;
		mLock->unlock();

		while(1)
		{
			AosAppMsgPtr msg = conn->smartReadAsync();
			if(!msg)	break;
			//if(mShowLog)
			//{
			//	OmnScreen << "SvrProxy:: recvMsg"
			//		<< "; conn:" << conn->getSock()
			//		<< "; msg_rlb_seq:"<< msg->getRlbSeq() << endl;
			//}
			recvMsg(msg);
		}
		
		mLock->lock();
		mProcingConn[crt_thrdid] = 0;
		mLock->unlock();
	}
	return true;
}


bool
AosSvrProxy::recvMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(msg, false);

	bool rslt = false, succ;
	
	if(!msg->isTrans())
	{
		if(msg->getToProcId() == AOSTAG_ADMIN_PID)
		{
			// temp this must be trans ack. trans is from admin. so ack must to admin svr.
			return sendAppMsgToChildProc(msg, succ);
		}
		
		if(msg->getType() == MsgType::eTransAckMsg)
		{
			//AosTransAckMsg* ack_msg = (AosTransAckMsg*)msg.getPtr();
			
			//OmnScreen << "SvrProxy RRecv trans ack :"
			//	<< (ack_msg->getReqId()).toString() << endl;
		}

		rslt = sendAppMsg(msg, succ);
		aos_assert_r(rslt, false);
		return true;
	}

	// this msg is Trans.
	AosTransPtr trans = (AosTrans *)msg.getPtr();

	//OmnScreen << "SvrProxy RRecv trans:" << trans->getTransId().toString() << endl;
	if (mShowLog)
	{
		OmnScreen << "SvrProxy RRecv trans:" << trans->getTransId().toString() << endl;
	}

	u32 from_proc_id = trans->getFromProcId();
	if (from_proc_id == AOSTAG_ADMIN_PID && !trans->isAdminTrans())
	{
		trans->setToProcId(from_proc_id);
		sendAppMsgToChildProc(msg, succ);
		return true;
	}

	aos_assert_r(from_proc_id != AOSTAG_ADMIN_PID, false);
	rslt = mTransHandler->procTrans(trans);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::sendTrans(const AosTransPtr &trans)
{
	int to_svr_id = trans->getToSvrId();
	if (to_svr_id < 0)
	{
		OmnScreen << "Alarm**********to_svr_id is -1, may an error: " << trans->getType() << endl;;
	}
	aos_assert_r(to_svr_id >= 0, false);
	
	if(to_svr_id != AosGetSelfServerId())	return mNetSvr->sendTrans(to_svr_id, trans); 
	return mProcessMgr->sendTrans(trans);
}


bool
AosSvrProxy::sendAppMsg(const AosAppMsgPtr &msg, bool &succ)
{
	// This func maybe called by TransProced. TransProced will handler ignore
	int to_svr_id = msg->getToSvrId();
	aos_assert_r(to_svr_id >= 0 && (u32)to_svr_id < eAosMaxServerId, false);
	
	if(isMsgToSvrProxy(msg))
	{
		succ = true;
		return msgIsToSvrProxy(msg);
	}

	if(to_svr_id != AosGetSelfServerId())	return sendAppMsgToNet(msg, succ); 
	return sendAppMsgToChildProc(msg, succ);
}

bool
AosSvrProxy::isMsgToSvrProxy(const AosAppMsgPtr &msg)
{
	MsgType::E msg_tp = msg->getType();
	if(msg_tp == MsgType::eAddServer)	return true;

	// broadcast self is up need send msg to net.
	if(msg_tp == MsgType::eSvrUpMsg && !svrIsLoaded())	return true;
	
	if(msg->getToSvrId() == AosGetSelfServerId() && msg->isToSvrProxy())	return true;
	
	return false;
}


bool
AosSvrProxy::sendAppMsgToNet(const AosAppMsgPtr &msg, bool &succ)
{
	int to_svr_id = msg->getToSvrId();
	aos_assert_r(to_svr_id >= 0 && (u32)to_svr_id < eAosMaxServerId, false);
	aos_assert_r(to_svr_id != AosGetSelfServerId(), false);
	
	bool rslt = mNetSvr->sendAppMsg(to_svr_id, msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::sendAppMsgToChildProc(const AosAppMsgPtr &msg, bool &succ)
{
	bool rslt = mProcessMgr->sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::msgIsToSvrProxy(const AosAppMsgPtr &msg)
{
	if(mShowLog)
	{
		OmnScreen << "SvrProxy log; procMsg"
			<< "; msg_type:" << msg->getType()
			<< endl;
	}
	msg->proc();
	return true;
}


int
AosSvrProxy::getCrtMaster(const u32 cube_grp_id)
{
	mSwitchLock->lock();
	int master = mTransHandler->getCrtMaster(cube_grp_id);
	mSwitchLock->unlock();
	return master;
}

bool
AosSvrProxy::broadcastMsg(const AosAppMsgPtr &msg)
{
	return mNetSvr->broadcastMsg(msg);
}

bool
AosSvrProxy::killProc(const u32 logic_pid)
{
	// for tester trans.
	return mProcessMgr->killProc(logic_pid);
}


bool
AosSvrProxy::startProc(const u32 logic_pid)
{
	// for tester trans.
	return mProcessMgr->startProc(logic_pid);
}
	
bool
AosSvrProxy::startProc(
		const u32 from_pid, 
		const AosProcessType::E tp,
		const int listen_svr_id)
{
	return mProcessMgr->startProc(tp, from_pid, mArgs, listen_svr_id);
}


bool
AosSvrProxy::recvSvrStopped(const int stopped_sid)
{
	aos_assert_r(stopped_sid>= 0 && (u32)stopped_sid < eAosMaxServerId, false);
	mNetSvr->recvSvrStopping(stopped_sid);
	return true;	
}

bool
AosSvrProxy::stopProcExcludeAdmin(const int num)
{
	return mProcessMgr->stopProcExcludeAdmin(num);	
}

bool
AosSvrProxy::addServers(
		const int crt_judger_sid,		
		const AosXmlTagPtr &svrs_conf, 
		const AosXmlTagPtr &admin_conf)
{
	//if(crt_judger_sid == -1)	means this msg is from admin svr.	
	mInitLock->lock();
	if(mSvrLoaded)
	{
		mInitLock->unlock();
		return true;
	}
	
	//AosProxyLog << "load servers!!! " << endl;
	OmnScreen << "<ProxyMsg> SvrProxy recv server config; start load servers. "
		<< "recved from " << ((crt_judger_sid == -1) ? "admin" : "judger svr")
		<< endl;
	bool rslt = configSvrProxy(svrs_conf, admin_conf);
	aos_assert_rl(rslt, mInitLock, false);
	
	mSvrLoaded = true;
	mInitLock->unlock();
	
	if (AosGetSelfServerId() != 0)
	{
		rslt = sendConfigToAdmin(svrs_conf, admin_conf);
		aos_assert_r(rslt, false);
	}

	mNetSvr->init(svrs_conf, admin_conf, crt_judger_sid);

	//Jozhi JIMODB-1101
	OmnThreadedObjPtr thisPtr(this, false);
	mInitThrd = OmnNew OmnThread(thisPtr, 
			"NetSvrCheckFunc", eInitThrdId, true, true, __FILE__, __LINE__);
	mInitThrd->start();
	return true;
}


bool
AosSvrProxy::initThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	bool rslt = mNetSvr->start();
	aos_assert_r(rslt, false);
	
	rslt = initCrtMaster();
	aos_assert_r(rslt, false);
	
	// svr 0 admin will to send addCluster msg.
	if(AosGetSelfServerId() != 0)
	{
		rslt = initCluster();
		aos_assert_r(rslt, false);
	}
	thread->stop();
	return true;
}


bool
AosSvrProxy::initCrtMaster()
{
	// this func is will move to admin svr later.
	bool is_judger = mNetSvr->selfIsJudgerSvr();
	int judger_sid = mNetSvr->getJudgerSvrId();
	aos_assert_r(judger_sid >=0, false);

	OmnScreen << "<ProxyMsg> SvrProxy start init crt masters; "
		<< "self_is_judger:" << is_judger << "; "
		<< "judger_svr: " << judger_sid << "; " << endl;
	
	bool rslt, succ;
	AosAppMsgPtr msg;
	if(is_judger)
	{
		// maybe all svr just up.
		AosBuffPtr masters_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
		rslt = mTransHandler->getMastersFromFile(masters_buff);
		aos_assert_r(rslt, false);
		
		rslt = recvMastersBuff(AosGetSelfServerId(), true, masters_buff);
		aos_assert_r(rslt, false);
		
		msg = OmnNew AosGetCrtMastersMsg(0);
		rslt = mNetSvr->broadcastMsg(msg);
		aos_assert_r(rslt, false);
	}
	else
	{
		msg = OmnNew AosGetCrtMastersMsg(judger_sid);
		rslt = sendAppMsgToNet(msg, succ);
		aos_assert_r(rslt, false);
	}

	mInitLock->lock();
	while(!mMastersLoaded)
	{
		OmnScreen << "<ProxyMsg> SvrProxy initCrtMaster; wait masters initing " << endl;
	
		mMastersCondVar->wait(mInitLock);
	}
	mInitLock->unlock();

	return true;
}


bool
AosSvrProxy::getCrtMastersBuff(const int to_svr_id)
{
	// this func is will move to admin svr later.
	aos_assert_r(svrIsLoaded(), false);

	bool rslt, succ;
	AosAppMsgPtr msg;
	AosBuffPtr master_buff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	if(mNetSvr->selfIsJudgerSvr())
	{
		if(!mMastersLoaded)
		{
			OmnScreen << "<ProxyMsg> SvrProxy getCrtMasters failed; " 
				<< "svr:" << to_svr_id << "; "
				<< " wait master init... "
				<< endl;
		
			msg = OmnNew AosSendCrtMastersMsg(to_svr_id);
			rslt = sendAppMsg(msg, succ);
			aos_assert_r(rslt, false);
			return true;	
		}
		
		OmnScreen << "<ProxyMsg> SvrProxy get CrtMasters; "
			<< "svr:" << to_svr_id << "; "
			<< "from JudgerSvr."
			<< endl;
		
		rslt = mTransHandler->getMasters(master_buff);
		aos_assert_r(rslt, false);

		msg = OmnNew AosSendCrtMastersMsg(to_svr_id,
				false, master_buff);
		rslt = sendAppMsg(msg, succ);
		aos_assert_r(rslt, false);
		return true;
	}

	// self is not JudgerSvr.
	if(mMastersLoaded)
	{
		if(mNetSvr->getJudgerSvrId() == to_svr_id)
		{
			// svr0 down. other svr even don't monitor that svr 0 has death.
			// don't change judger svr. and svr0 up. admin send addServer to svr0.
			//this is Temp. will delete later.
			aos_assert_r(to_svr_id == 0, false);
			if(mNetSvr->calcuNextJudgerSvrId(to_svr_id) == AosGetSelfServerId())
			{
				rslt = mTransHandler->getMasters(master_buff);
				aos_assert_r(rslt, false);

				msg = OmnNew AosSendCrtMastersMsg(to_svr_id,
						false, master_buff);
				rslt = sendAppMsg(msg, succ);
				aos_assert_r(rslt, false);
				return true;
			}
		}

		OmnScreen << "<ProxyMsg> SvrProxy get CrtMasters; "
			<< "svr:" << to_svr_id << "; "
			<< "master has inited. but crt svr is not judger svr, "
			<< "not need send crt masters. " << endl;
		return true;
	}
	
	OmnScreen << "<ProxyMsg> SvrProxy get CrtMasters; "
		<< "svr:" << to_svr_id << "; "
		<< "read from file."
		<< endl;
	rslt = mTransHandler->getMastersFromFile(master_buff);
	aos_assert_r(rslt, false);

	msg = OmnNew AosSendCrtMastersMsg(to_svr_id, true, master_buff);
	rslt = sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::getMastersFailed(const int from_sid)
{
	// this func is will move to admin svr later.
	aos_assert_r(AosGetSelfServerId() != 0, false);
	
	OmnScreen << "<ProxyMsg> SvrProxy init crt masters failed; continue get crt masters; "
		<< "judger_svr: " << from_sid << "; " << endl;

	OmnSleep(1);

	AosAppMsgPtr msg = OmnNew AosGetCrtMastersMsg(from_sid);
	bool succ;
	bool rslt = sendAppMsgToNet(msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::recvMastersBuff(
		const int from_sid,
		const bool from_file,
		const AosBuffPtr &masters_buff)
{
	// this func is will move to admin svr later.
	
	bool rslt;
	if(!from_file)
	{
		// means this is from crt judger.
		OmnScreen << "<ProxyMsg> SvrProxy recv crt masters from judger svr; "
			<< "; judger_sid:" << from_sid
			<< "; init crt masters finish. " << endl;
		
		rslt = mTransHandler->recvMasters(masters_buff);
		aos_assert_r(rslt, false);
	
		mInitLock->lock();
		mMastersLoaded = true;
		//mMastersCondVar->signal();
		mMastersCondVar->broadcastSignal();
		mInitLock->unlock();
		return true;
	}
	
	// means all svr has down. just svr 0 need collect masters from other svr files.
	aos_assert_r(AosGetSelfServerId() == 0, false);
	bool collect_finish;
	rslt = mTransHandler->recvMastersFromOtherSvrFile(from_sid, masters_buff, collect_finish);
	aos_assert_r(rslt, false);
	if(!collect_finish)		return true;
	
	OmnScreen << "<ProxyMsg> SvrProxy judger svr init crt masters finish; "
		<< endl;
		
	mInitLock->lock();
	mMastersLoaded = true;
	//mMastersCondVar->signal();
	mMastersCondVar->broadcastSignal();
	mInitLock->unlock();
	return true;
}


bool
AosSvrProxy::initCluster()
{
	// this func is will move to admin svr later.
	OmnScreen << "<ProxyMsg> SvrProxy start init crt cluster; " << endl;

	int judger_sid = mNetSvr->getJudgerSvrId();
	aos_assert_r(judger_sid >=0, false);

	AosAppMsgPtr msg = OmnNew AosGetClusterConfigMsg(judger_sid);
	bool succ;
	bool rslt = sendAppMsgToNet(msg, succ);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosSvrProxy::getCluster(const int to_svr_id)
{
	aos_assert_r(svrIsLoaded(), false);
	
	while(!mClusterLoaded)
	{
		OmnScreen << "<ProxyMsg> SvrProxy getCluster; " 
			<< "svr:" << to_svr_id << "; "
			<< " wait cluster init... "
			<< endl;
		mInitLock->lock();
		if(mClusterLoaded)
		{
			// Ketty 2014/06/12
			mInitLock->unlock();
			break;	
		}
		mClusterCondVar->wait(mInitLock);
		mInitLock->unlock();
	}
	aos_assert_r(mClusterConfig && mNormConfig, false);
	
	OmnScreen << "<ProxyMsg> SvrProxy send cluster config to svr: "
		<< to_svr_id << "; " 
		<< endl;
	
	AosXmlTagPtr cconfig = mClusterConfig->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr nconfig = mNormConfig->clone(AosMemoryCheckerArgsBegin);
	AosAppMsgPtr msg = OmnNew AosAddClusterMsg(
			mArgs, cconfig, nconfig, to_svr_id, 0);
	aos_assert_r(msg, false);

	bool succ;
	bool rslt = sendAppMsgToNet(msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosSvrProxy::addCluster(
		const u32 from_pid,
		const OmnString &args,
		const AosXmlTagPtr &cluster_config,
		const AosXmlTagPtr &norm_config)
{
	aos_assert_r(args != "" && cluster_config && norm_config, false);

	mInitLock->lock();
	while(!mMastersLoaded)
	{
		aos_assert_rl(AosGetSelfServerId() == 0, mInitLock, false);
		mMastersCondVar->wait(mInitLock);
	}
	
	if(mClusterLoaded)
	{
		mInitLock->unlock();
		return true;
	}
	
	mClusterLoaded = true;
	mArgs = args;
	mClusterConfig = cluster_config;
	mNormConfig = norm_config; 
	//mClusterCondVar->signal();
	mClusterCondVar->broadcastSignal();
	
	mInitLock->unlock();
	
	OmnScreen << "<ProxyMsg> SvrProxy recv cluster config; start load cluster. " << endl;

	AosNetworkMgrObjPtr network_mgr = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(network_mgr, false);
	bool rslt = network_mgr->configCluster(mClusterConfig);
	aos_assert_r(rslt, false);
	
	rslt = mTransHandler->contInitCrtMasters();
	aos_assert_r(rslt, false);

	rslt = mProcessMgr->startDefProc(from_pid, mArgs, mNormConfig, mClusterConfig);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosSvrProxy::configSvrProxy(
		const AosXmlTagPtr &svrs_conf, 
		const AosXmlTagPtr &admin_conf)
{
	aos_assert_r(svrs_conf && admin_conf, false);
	
	AosNetworkMgrObjPtr network_mgr = AosNetworkMgrObj::getNetworkMgr();
	aos_assert_r(network_mgr, false);
	
	bool rslt = network_mgr->configSelfSvr(svrs_conf);
	aos_assert_r(rslt, false);
	
	rslt = network_mgr->configServers(svrs_conf);
	aos_assert_r(rslt, false);
	
	OmnScreen << "<ProxyMsg> SvrProxy config servers finish; "
		<< "self_svr_id:" << AosGetSelfServerId()  << endl;
	return true;
}


bool
AosSvrProxy::sendConfigToAdmin(
		const AosXmlTagPtr &svrs_conf, 
		const AosXmlTagPtr &admin_conf)
{
	OmnScreen << "<ProxyMsg> SvrProxy send config to self admin svr. " << endl;
	
	AosXmlTagPtr sconfig = svrs_conf->clone(AosMemoryCheckerArgsBegin);
	AosXmlTagPtr aconfig = admin_conf->clone(AosMemoryCheckerArgsBegin);
	u32 svr_id = AosGetSelfServerId();
	sconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	sconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);
	AosAppMsgPtr msg = OmnNew AosAddServerMsg(
			sconfig, aconfig, svr_id, AOSTAG_ADMIN_PID);
	aos_assert_r(msg, false);
	bool succ;
	bool rslt = sendAppMsgToChildProc(msg, succ);
	aos_assert_r(rslt && succ, false);
	return true;	
}


bool
AosSvrProxy::diskDamaged()
{
	return mProcessMgr->handDiskDamaged();	
}


bool
AosSvrProxy::recvSvrUp(const u32 up_svr_id)
{
	aos_assert_r(up_svr_id >= 0 && (u32)up_svr_id < eAosMaxServerId, false);
	//if(svrIsLoaded() && up_svr_id == (u32)AosGetSelfServerId())	return true;

	OmnScreen << "<ProxyMsg> SvrProxy recv svr ready. " << up_svr_id << endl;
	bool rslt = mNetSvr->recvSvrReady(up_svr_id);
	aos_assert_r(rslt, false);
	
	if(!svrIsLoaded())	return true;

	OmnString reason = "svr ready. svr_id:";
	reason << up_svr_id;
	AosTriggerResendMsgPtr msg = OmnNew AosTriggerResendMsg(
			reason, AosGetSelfServerId(), up_svr_id, false);
	
	return triggerResend(msg);
}


bool
AosSvrProxy::recvSvrDeath(const int death_svr_id)
{
	// SysMonitor func call this.
	aos_assert_r(death_svr_id >= 0 && (u32)death_svr_id < eAosMaxServerId, false);
	if(svrIsLoaded()) aos_assert_r(death_svr_id != AosGetSelfServerId(), false);
	
	OmnScreen << "<ProxyMsg> SvrProxy recv svr death. " << death_svr_id << endl;
	
	mNetSvr->recvSvrDeath(death_svr_id);
	
	if(!svrIsLoaded())	return true;
	
	OmnString reason = "svr death. svr_id:";
	reason << death_svr_id;
	AosTriggerResendMsgPtr msg = OmnNew AosTriggerResendMsg(reason);
	
	return triggerResend(msg);
}

bool
AosSvrProxy::triggerResend(const AosTriggerResendMsgPtr &msg)
{
	// 1. just proc up or svr up. need_resendend_flag is true. to mark trans begin.
	// 2. proc down or svr down. just need resend trans. to send svrDeathMsg.
	// 3. switch to master. just need resend trans. to send to the right master.(need??)
	
	return mProcessMgr->notifyResendTrans(msg);
}


bool
AosSvrProxy::recvResendEnd(
		const int target_svr_id,
		const int target_proc_id,
		const bool is_ipc_conn,
		const int resend_from_sid,
		const u32 resend_from_pid)
{
	aos_assert_r(target_svr_id >= 0 && (u32)target_svr_id < eAosMaxServerId, false);
	aos_assert_r(svrIsLoaded(), false);

	if(target_svr_id != AosGetSelfServerId())
	{
		aos_assert_r(resend_from_sid == AosGetSelfServerId(), false);
		
		AosAppMsgPtr msg = OmnNew AosResendEndMsg(target_svr_id, target_proc_id,
				is_ipc_conn, resend_from_sid, resend_from_pid);
	
		bool succ;
		bool rslt = sendAppMsgToNet(msg, succ);
		aos_assert_r(rslt, false);
		return true;
	}
	
	if(is_ipc_conn)		return mProcessMgr->recvResendEnd(resend_from_sid, resend_from_pid, target_proc_id);

	// then target_proc_id is another svr_id.
	return mNetSvr->recvResendEnd(resend_from_sid, resend_from_pid, target_proc_id);
}


bool
AosSvrProxy::switchToMaster(const u32 cube_grp_id)
{
	// let self's cube svr to be master.
	OmnScreen << "SvrProxy======= "
		<< "switch to master start" << endl;
	
	int new_master = AosGetSelfServerId();
	
	// send this switchToMaster msg to proc_id
	AosAppMsgPtr msg = OmnNew AosSwitchToMasterMsg(new_master, cube_grp_id);
	
	bool succ;
	bool rslt = sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	if(succ)	return true;
	
	return mProcessMgr->startBkpCubeSvr(cube_grp_id, new_master);
}


bool
AosSvrProxy::switchToMasterFinish(const u32 cube_grp_id, const u32 proc_id)
{
	// this func is called when some self's proc switchToMaster finish
	OmnScreen << "SvrProxy======= "
		<< "switch to master finish" << endl;

	// broadcast all SvrProxy.
	mSwitchLock->lock();
	
	int self_svr_id = AosGetSelfServerId();
	bool rslt = setNewMasterPriv(cube_grp_id, self_svr_id);
	aos_assert_r(rslt, false);

	//mTransHandler->setNewMaster(cube_grp_id, self_svr_id);
	AosAppMsgPtr msg = OmnNew AosSetNewMasterMsg(0,
			AOSTAG_SVRPROXY_PID, cube_grp_id, self_svr_id);
	mNetSvr->broadcastMsg(msg);
	
	mSwitchLock->unlock();
	return true;
}


bool
AosSvrProxy::setNewMasterPriv(const u32 cube_grp_id, const int new_master)
{
	OmnScreen << "SvrProxy======= "
		<< "set new master"
		<< "; cube_grp_id:" << cube_grp_id
		<< "; new_master:" << new_master
		<< endl;

	bool rslt = mTransHandler->setNewMaster(cube_grp_id, new_master);
	aos_assert_r(rslt, false);
	
	OmnString reason = "set new master. cube_grp_id:";
	reason << cube_grp_id << "; new_master:" << new_master;
	AosTriggerResendMsgPtr resend_msg = OmnNew AosTriggerResendMsg(reason);
	triggerResend(resend_msg);

	return true;	
}


bool
AosSvrProxy::setNewMaster(const u32 cube_grp_id, const int new_master)
{
	// this func is called by AppMsg: AosSetNewMasterMsg.. 
	int old_master = mTransHandler->getCrtMaster(cube_grp_id);
	
	bool rslt = setNewMasterPriv(cube_grp_id, new_master);
	aos_assert_r(rslt, false);

	if(!isCubeGrpInSelfSvr(cube_grp_id))	return true;

	int self_svr_id = AosGetSelfServerId();
	if(old_master == self_svr_id)
	{
		rslt = mProcessMgr->rebootCubeProc(cube_grp_id);
		aos_assert_r(rslt, false);
		return true;
	}

	OmnScreen << "SvrProxy======= "
		<< "set new master, send to child proc(self's cubesvr)"
		<< endl;
	AosAppMsgPtr msg = OmnNew AosSetNewMasterMsg(self_svr_id,
			cube_grp_id, new_master);
	bool succ;
	rslt = sendAppMsgToChildProc(msg, succ);
	aos_assert_r(rslt, false);
	if(!succ)
	{
		OmnScreen << "setNewMaster. but cube proc death"
			<< "; cube_pid:" << cube_grp_id << endl;
	}

	return true;
}

bool
AosSvrProxy::isCubeGrpInSelfSvr(const u32 cube_grp_id)
{
	int self_svr_id = AosGetSelfServerId();
	vector<u32> & svr_ids = AosGetCubeSvrIds(cube_grp_id);
	for(u32 i=0; i<svr_ids.size(); i++)
	{
		if(svr_ids[i] == (u32)self_svr_id)	return true; 
	}
	return false;
}


bool
AosSvrProxy::monitorSvrDeath(const int death_svr_id)
{
	// SysMonitor func call this.
	aos_assert_r(death_svr_id >= 0 && (u32)death_svr_id < eAosMaxServerId, false);

	OmnScreen << "<ProxyMsg> SvrProxy svr death! "
		<< "; death_svr:" << death_svr_id 
		<< "; try to start bkp svr. "
		<< endl;
	
	bool rslt = mProcessMgr->handSvrDeath(death_svr_id); 
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSvrProxy::selfIsJudgerSvr()
{
	return mNetSvr->selfIsJudgerSvr();
}


bool
AosSvrProxy::startServer()
{
	bool rslt = mProcessMgr->startDefProc();
	aos_assert_r(rslt, false);
	return true;
}

bool
AosSvrProxy::addWaitResendEnd(
		const int svr_id,
		const u32 proc_id,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	return mResendCheckThrd->addWaitResendEnd(svr_id, proc_id, 
			wait_is_ipc_conn, wait_id);
}

bool
AosSvrProxy::removeWaitResendEnd(
		const int svr_id,
		const u32 proc_id,
		const bool wait_is_ipc_conn,
		const u32 wait_id)
{
	return mResendCheckThrd->removeWaitResendEnd(svr_id, proc_id, 
			wait_is_ipc_conn, wait_id);
}

bool
AosSvrProxy::startJobSvr()
{
	aos_assert_r(mProcessMgr, false);
	return mProcessMgr->startJobSvr();
}

bool
AosSvrProxy::notifyProcIsUp(const int child_pid)
{
	return mProcessMgr->notifyProcIsUp(child_pid);
}
