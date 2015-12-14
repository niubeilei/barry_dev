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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/IpcClt.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/ProcessType.h"
#include "TransBasic/AppMsg.h"
#include "SvrProxyMgr/IpcCltInfo.h"
#include "SvrProxyMgr/Ptrs.h"
#include "Thread/ThreadMgr.h"
#include "Util/OmnNew.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>

AosIpcClt::AosIpcClt()
:
mLock(OmnNew OmnMutex()),
mShowLog(false)
{
}


AosIpcClt::~AosIpcClt()
{
}


bool
AosIpcClt::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr config = app_conf->getFirstChild("IpcClt");
	aos_assert_r(config, false);

	mTmpDir = config->getAttrStr("tmp_data_dir", "proxy_tmp/");
	if(mTmpDir != "" && mTmpDir.find('/', true) != mTmpDir.length() - 1)
	{
		mTmpDir << "/";
	}

	mShowLog = config->getAttrBool(AOSCONFIG_SHOWLOG, false);
	//mShowLog = true;

	u64 max_cache_size = config->getAttrU64(AOSCONFIG_TRANS_MAXCACHESIZE, 500) * 1000000;
	AosRecvEventHandlerPtr recv_hd;		// IpcClt don't has event_hd.
	AosIpcCltObjPtr thisptr(this, false);
	mIpcConn = OmnNew AosIpcCltInfo(thisptr, max_cache_size, recv_hd, mShowLog);

	return true;
}


bool
AosIpcClt::start()
{
	mUpath = mTmpDir; 
	mUpath << getpid() << "_" << AosGetSelfProcId() << "_" << AosProcessType::toStr(AosGetSelfProcType()) <<".sock";
	
	OmnThreadedObjPtr thisPtr(this, false);
	mRecvThrd = OmnNew OmnThread(thisPtr,
			"IpcCltConnFunc", eRecvConnThrdId, true, true, __FILE__, __LINE__);

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	aos_assert_r(sock, false);

	struct sockaddr_un clt_addr;
	bzero(&clt_addr, sizeof(clt_addr));
	clt_addr.sun_family = AF_UNIX;
	strcpy(clt_addr.sun_path, mUpath.data()); 

	int succ = ::bind(sock, (struct sockaddr*)&clt_addr, sizeof(clt_addr));
	if(succ == -1)
	{
		OmnAlarm << "error! Unable to bind :"<< mUpath << enderr;
		::close(sock);
		return false;
	}

	bool rslt = connectIpcSvr(sock);
	if(!rslt)
	{
		OmnScreen << "failed to connect IpcSvr" << endl;
		::close(sock);
		return true;
	}

	if (mShowLog)
	{
		OmnScreen << "create new connection: " << mUpath << endl;
	}

	mIpcConn->setName(mUpath);
	mIpcConn->setConnSock(sock);

	mRecvThrd->start();
	return true;
}


bool
AosIpcClt::stop()
{
	mIpcConn->stop();
	return true;
}


bool
AosIpcClt::connectIpcSvr(const int sock)
{
	OmnString svr_upath = mTmpDir; 
	svr_upath << "SvrProxy.sock";
	//OmnString svr_upath = "/tmp/zykie/SvrProxy.sock"; 
	struct sockaddr_un svr_addr;
	
	bzero(&svr_addr, sizeof(svr_addr));
	svr_addr.sun_family = AF_UNIX;
	strcpy(svr_addr.sun_path, svr_upath.data()); 

	int trys = eMaxConnTrys;
	while(trys--)
	{
		int rslt = connect(sock, (struct sockaddr*)&svr_addr, sizeof(svr_addr));
		if(rslt == 0) return true;
		
		OmnSleep(eReConnTime);
	}
	
	return false;
}


bool
AosIpcClt::signal(const int threadLogicId)
{
	return true;
}


bool
AosIpcClt::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		aos_assert_r(mIpcConn, false);
		AosAppMsgPtr msg = mIpcConn->smartReadSync();
	
		if(msg)		recvMsg(msg);
		
		//felicia, 2013/06/19
		mThreadStatus = true;
	}
	return true;
}


bool
AosIpcClt::recvMsg(const AosAppMsgPtr &msg)
{
	aos_assert_r(msg, false);
	
	AosTransSvrObjPtr trans_svr = AosTransSvrObj::getTransSvr();
	aos_assert_r(trans_svr, false);
	return trans_svr->recvMsg(msg);
}


bool
AosIpcClt::sendMsg(const AosAppMsgPtr &msg)
{
	bool succ;
	bool rslt = mIpcConn->sendAppMsg(msg, succ);
	aos_assert_r(rslt && succ, false);
	return true;
}

bool
AosIpcClt::sendTrans(const AosTransPtr &trans)
{
	return mIpcConn->sendTrans(trans);
}


bool
AosIpcClt::connFailed()
{
	//OmnAlarm << "Error!! connect with SvrProxy error!" << enderr;
	OmnScreen << "Error!! connect with SvrProxy error!" << endl;
	return true;
}


