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
#include "SvrProxyMgr/IpcCltInfo.h"

#include "SEInterfaces/IpcCltObj.h"
#include "API/AosApi.h"

AosIpcCltInfo::AosIpcCltInfo(
		const AosIpcCltObjPtr &ipc_clt,
		const u32 m_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		bool show_log)
:
AosSvrInfo(m_cache_size, recv_hd, show_log),
mIpcClt(ipc_clt)
{
}


AosIpcCltInfo::~AosIpcCltInfo()
{
}


/*
bool
AosIpcCltInfo::connFailed(const int failed_sock)
{
	bool rslt = AosSvrInfo::connFailed(failed_sock);
	aos_assert_r(rslt, false);

	return mIpcClt->connFailed();
}
*/

u64
AosIpcCltInfo::getCheckKey(const AosTransPtr &trans)
{
	return 0;
}

OmnString
AosIpcCltInfo::getStrType()
{
	OmnString ss = "IpcCltInfo";
	ss << "; sock:" << getSockId();
	return ss;
}

