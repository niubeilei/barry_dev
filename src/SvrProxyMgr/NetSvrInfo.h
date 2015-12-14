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
#ifndef AOS_SvrProxyMgr_NetSvrInfo_h
#define AOS_SvrProxyMgr_NetSvrInfo_h

#include "SvrProxyMgr/SvrInfo.h"

class AosNetSvrInfo : public AosSvrInfo 
{
	OmnDefineRCObject;

	enum
	{
		eWaitSvrUpTimeOut = 20, // 20 sec.
		eWaitSvrUpTimeOut2 = 3, // 3 sec.
	};

private:
	//AosNetSvrPtr mNetSvr;
	AosSvrMonitorPtr  mSvrMonitor;
	int			 mSvrId;
	bool		 mInit;
	bool		 mSvrReady;
	bool		 mSvrStopping;
	u64			 mRecvConnSockTime;
	bool		 mConnCheck;

public:
	AosNetSvrInfo(
			//const AosNetSvrPtr &svr_mgr, 
			const AosSvrMonitorPtr &svr_monitor,
			const int svr_id, 
			const u32 m_cache_size,
			const AosRecvEventHandlerPtr &recv_hd,
			bool show_log);
	~AosNetSvrInfo();

	virtual bool stop();
	virtual bool setConnSock(const int sock_id);
	virtual bool setConnFailed();
	virtual bool setSvrDeath();
	virtual bool isSvrReady(){ return mSvrReady; };
	virtual bool getId(bool &is_icp_conn, u32 &id);

	virtual bool sendAppMsg(const AosAppMsgPtr &msg, bool &succ);
	virtual bool sendTrans(const AosTransPtr &trans);
	virtual bool isNetSvr(){ return true; };
	bool 	sendSysMsg(const AosAppMsgPtr &msg);
	
	int		getSvrId(){ return mSvrId; };
	bool 	setSvrReady();
	bool 	isNeedConnect();
	
	bool	isInited(){ return mInit; };
	void 	setStopping();
	
	virtual void updateHeartbeat();
	virtual void updateSockLastRecvTime();
	virtual void repaireConnect();

};

#endif
