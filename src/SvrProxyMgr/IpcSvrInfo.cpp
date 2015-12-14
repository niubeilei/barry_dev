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
#include "SvrProxyMgr/IpcSvrInfo.h"

#include "API/AosApi.h"
#include "SvrProxyMgr/Process.h"

#include "SysMsg/TriggerResendMsg.h"

AosIpcSvrInfo::AosIpcSvrInfo(
		const AosProcessPtr &proc, 
		const u32 m_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		bool show_log)
:
AosSvrInfo(m_cache_size, recv_hd, show_log),
mConnLock(OmnNew OmnMutex()),
mProcess(proc)
{
}


AosIpcSvrInfo::~AosIpcSvrInfo()
{
}
	
bool
AosIpcSvrInfo::setConnSock(const int sock_id)
{
	mConnLock->lock();
	bool rslt = setConnSockPriv(sock_id);
	mConnLock->unlock();
	aos_assert_r(rslt, false);
	return true;
}


bool
AosIpcSvrInfo::getId(bool &is_icp_conn, u32 &id)
{
	is_icp_conn = true;
	id = mProcess->getLogicPid();
	return true;
}


