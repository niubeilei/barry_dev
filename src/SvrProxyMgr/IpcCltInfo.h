////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// Created: 08/09/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_IpcCltInfo_h
#define AOS_SvrProxyMgr_IpcCltInfo_h

#include "SvrProxyMgr/SvrInfo.h"
#include "SEInterfaces/Ptrs.h"

class AosIpcCltInfo : public AosSvrInfo
{
	OmnDefineRCObject;
	
private:
	AosIpcCltObjPtr mIpcClt;

public:
	AosIpcCltInfo(const AosIpcCltObjPtr &ipc_clt, bool show_log);
	AosIpcCltInfo(
			const AosIpcCltObjPtr &ipc_clt,
			const u32 max_cache_size,
			const AosRecvEventHandlerPtr &recv_hd,
			bool show_log);
	~AosIpcCltInfo();

	//virtual bool connFailed(const int failed_sock);
	virtual OmnString getStrType();

protected:
	virtual u64 getCheckKey(const AosTransPtr &trans);

};


#endif
