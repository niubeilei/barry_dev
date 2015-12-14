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
#ifndef AOS_SvrProxyMgr_SvrInfo_h
#define AOS_SvrProxyMgr_SvrInfo_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/TargetUtil.h"
#include "SysMsg/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "TransBasic/Trans.h"
#include "Util/TransId.h"

#include <deque>
#include <map>
using namespace std;

class AosSvrInfo : public OmnRCObject 
{
	OmnDefineRCObject;
	
	struct ResendWaiter
	{
		u64 	mCltKey;
		bool	mWaitAllResend;
		map<AosTransId, AosTransPtr> mAllTrans;
		
		ResendWaiter(const u64 key)
		:
		mCltKey(key),
		mWaitAllResend(false)
		{
		};
	};
	
	typedef ResendWaiter * ResendWaiterPtr;

private:
	OmnMutexPtr			mLock;
	AosSockConnPtr		mConn;
	bool				mSvrDeath;

	set<TransIdExpand>	mIgnoreTransIds;
	deque<AosTransPtr>	mIgnoreTrans;
	map<u64, ResendWaiterPtr> mResendWaiter;

protected:
	OmnString 			mName;
protected:
	bool				mShowLog;
	
public:
	AosSvrInfo(
		const u32 max_cache_size,
		const AosRecvEventHandlerPtr &recv_hd,
		bool show_log);
	~AosSvrInfo();

	virtual bool stop();
	virtual bool setConnSock(const int sock_id);
	virtual bool setConnFailed();
	virtual bool setSvrDeath();
	virtual bool isSvrReady();
	//virtual AosTriggerResendMsgPtr createTriggerResendMsg();
	virtual bool getId(bool &is_icp_conn, u32 &id){ return false;};
	virtual bool isNetSvr(){ return false; };

	bool 	isConnGood();

	bool 	svrIsUp();
	bool	isDeath(){ return mSvrDeath; };

	virtual bool sendAppMsg(const AosAppMsgPtr &msg, bool &succ);
	virtual bool sendTrans(const AosTransPtr &trans);
	virtual void updateHeartbeat(){};
	virtual void updateSockLastRecvTime() {};
	virtual void repaireConnect() {};
	
	AosAppMsgPtr smartReadSync();

	bool 	sendIgnoreTransPublic();
	bool 	recvResendEnd(const u32 from_sid, const u32 from_pid);
	void 	setName(const OmnString &name) 
	{
		OmnString s = name.substr(name.find('/', true) + 1);
		mName = s;
	}
	
protected:
	int		getSockId();
	bool 	setConnSockPriv(const int sock_id);
	bool 	resetConnSock();
	bool 	sendAppMsgPriv(const AosAppMsgPtr &msg, bool &succ);
private:
	bool 	sendSvrDeathMsg(const AosTransPtr &orig_trans, const int death_svr_id);
	
	bool 	sendOneTrans(const AosTransPtr &trans);
	bool	sendAllTrans(map<AosTransId, AosTransPtr> & rs_trans);

	bool 	addIgnoreTrans(const AosTransPtr &trans);
	bool 	sendIgnoreTrans();
	
	bool 	sendMsgPriv(const AosAppMsgPtr &msg, bool &is_ignore, bool &bad_conn);
	
	ResendWaiterPtr getResendWaiter(const u64 clt_key, const bool create = false);
	bool 	hasIgnore();

	bool 	setWaitResendTrans();
	bool 	waitResendTimeout(const u64 start_time);
	bool	notifyResendTrans(const int to_sid, const u32 to_pid);

};

#endif
