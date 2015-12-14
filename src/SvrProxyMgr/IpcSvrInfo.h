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
#ifndef AOS_SvrProxyMgr_IpcSvrInfo_h
#define AOS_SvrProxyMgr_IpcSvrInfo_h

#include "SvrProxyMgr/SvrInfo.h"

class AosIpcSvrInfo : public AosSvrInfo
{
	OmnDefineRCObject;
	
private:
	OmnMutexPtr	  mConnLock;
	AosProcessPtr mProcess;

public:
	AosIpcSvrInfo(
			const AosProcessPtr &proc, 
			const u32 m_cache_size,
			const AosRecvEventHandlerPtr &recv_hd,
			bool show_log);
	~AosIpcSvrInfo();

	virtual bool setConnSock(const int sock_id);
	virtual bool getId(bool &is_icp_conn, u32 &id);

};


#endif
