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
// Created: 06/03/2011 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/IpcSvr.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "SvrProxyMgr/SockConn.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "Util/OmnNew.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

AosIpcSvr::AosIpcSvr(
		const OmnString &dir_name,
		const AosProcessMgrPtr &proc_mgr,
		const bool showlog)
:
mListenSock(-1),
mProcessMgr(proc_mgr),
mShowLog(showlog),
mIsStopping(false)
{
	mUpath = dir_name;
	mUpath << "/SvrProxy.sock";
}


AosIpcSvr::~AosIpcSvr()
{
	close(mListenSock);
}


bool
AosIpcSvr::startListen()
{
	struct sockaddr_un svr_addr;
	bzero(&svr_addr, sizeof(svr_addr));
	svr_addr.sun_family = AF_UNIX;

	strcpy(svr_addr.sun_path, mUpath.data()); 
	mListenSock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (mListenSock == -1)
	{
		OmnAlarm << "error!" << enderr;
		return false;
	}
	
	int rslt = ::bind(mListenSock, (sockaddr*)&svr_addr, sizeof(svr_addr));
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
	// accept conn Thread.
	mConnThrd = OmnNew OmnThread(thisPtr,
			"IpcSvrConnFunc", eAcceptConnThrdId, true, true, __FILE__, __LINE__);
	mConnThrd->start();
	return true;
}


bool
AosIpcSvr::stop()
{
	mIsStopping = true;
	close(mListenSock);
	mListenSock = -1;
	
	mConnThrd->stop();
	return true;
}


bool
AosIpcSvr::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u64 thrd_id = thread->getLogicId();

	if(thrd_id == eAcceptConnThrdId)
	{
		return acceptConnThrdFunc(state, thread);
	}

	OmnAlarm << "Invalid thread logic id: " << thrd_id << enderr;
	return false;
}


bool
AosIpcSvr::acceptConnThrdFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	struct sockaddr_un clt_addr;
	socklen_t addr_len = sizeof(sockaddr_un);
	OmnString remote_upath;
	int proc_id;
	u32 logic_pid;
	bool rslt;
	
	while(state == OmnThrdStatus::eActive)
	{
		int clt_sock = accept(mListenSock,(sockaddr *)&clt_addr, &addr_len);
		if(mIsStopping)	return true;
		
		if(clt_sock<0)
		{
			int err = errno;
			OmnAlarm << "error clt_sock:" << clt_sock
				<< "; errno:" << err
				<< enderr;
			continue;
		}

		remote_upath = clt_addr.sun_path;
		AosProcessType::E ptype;
		rslt = getPidFromUpath(remote_upath, proc_id, logic_pid, ptype); 
		aos_assert_r(rslt, false);

		if (mShowLog)
		{
			OmnScreen << "accept new connection: " << remote_upath << endl;
		}
		mProcessMgr->recvNewConnSock(remote_upath, logic_pid, proc_id, clt_sock, ptype);
	}
	return true;
}


bool
AosIpcSvr::getPidFromUpath(
		const OmnString path,
		int &proc_id,
		u32 &logic_pid,
		AosProcessType::E &ptype)
{
	// path format: /tmp/zykie/svr_0/pid_logicpid_ptype.sock
	if(path == "")	return false;

	int s_pos = path.find('/', true);
	aos_assert_r(s_pos > 0, false);
	s_pos += 1;

	int e_pos = path.findSubString("_", s_pos);
	aos_assert_r(e_pos > 0, false);
	OmnString pid_str = path.subString(s_pos, e_pos - s_pos);
	aos_assert_r(pid_str != "", -1);
	proc_id = atoi(pid_str.data());

	
	s_pos = e_pos + 1;
	e_pos = path.findSubString("_", s_pos);
	aos_assert_r(e_pos > 0, false);
	OmnString logicpid_str = path.subString(s_pos, e_pos - s_pos);
	aos_assert_r(logicpid_str != "", -1);
	logic_pid = atoi(logicpid_str.data());
	aos_assert_r(logic_pid >0, false);

	s_pos = e_pos + 1;
	e_pos = path.findSubString(".", s_pos);
	aos_assert_r(e_pos > 0, false);
	OmnString ptype_str = path.subString(s_pos, e_pos - s_pos);
	ptype = AosProcessType::toEnum(ptype_str);
	aos_assert_r(AosProcessType::isValid(ptype), false);

	return true;
}


