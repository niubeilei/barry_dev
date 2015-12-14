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
// Created: 06/03/2011 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TransClient_TransClient_h
#define AOS_TransClient_TransClient_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "TransClient/Ptrs.h"
#include "Porting/GetTime.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/TransCallerObj.h"
#include "SEUtil/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/CondVar.h"
#include "Util/Ptrs.h"

#include <map>
#include <queue>
using namespace std;

class AosTransClient: public AosTransCltObj,
					  public AosTransCallerObj,
					  public OmnThreadedObj
{
	OmnDefineRCObject;
	
	enum
	{
		eReSendThrdId = 1,
		
		eCheckThrdSleep = 5,		//5 sec.
		eWaitTime = 2,				// 2 * eCheckThrdSleep sec.
		
		//eMaxCacheSize = 1 * 1000 * 1000,		// 1M.
		eMaxCacheSize = 500 * 1000 * 1000,		// 1M.
	};
	
	struct CacheInfo
	{
		u64 			mSize;
		OmnCondVarPtr   mCondVar;
		
		CacheInfo(const u64 init_size)
		:
		mSize(init_size),
		mCondVar(OmnNew OmnCondVar())
		{
		}
	};

	struct CallerEntry
	{
		u64						timestamp;
		AosTransCallerObjPtr	caller;

		CallerEntry()
		:
		timestamp(0)
		{
		}

		CallerEntry(const AosTransCallerObjPtr &caller)
		:
		timestamp(OmnGetTimestamp()),
		caller(caller)
		{
		}
	};

private:
	OmnMutexPtr		 mLock;
	OmnCondVarPtr    mResendCondVar;
	AosTransIdMgrPtr mIdMgr;
	
	map<u64, CacheInfo>	mCacheSize;
	map<AosTransId, AosWaitInfoPtr> mWaitMap;
	map<u64, CallerEntry>           mTransCallers;
	queue<AosTriggerResendMsgPtr>	mResendReq;
	
	OmnThreadPtr	 mReSendThrd;
	bool			 mShowLog;

public:
	AosTransClient();
	~AosTransClient();
	
	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId){ return true; };
	virtual bool    checkThread111(OmnString &err, const int thrdLogicId){ return true; };
	
	virtual bool config(const AosXmlTagPtr &app_conf);
	//virtual bool start(){ return true; };
	virtual bool start();
	virtual bool stop(){ return true; };

	virtual bool sendTransAsyncResp(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans);
	
	virtual bool sendTransSyncResp(
					const AosRundataPtr &rdata,
					vector<AosTransPtr> vt_trans);

	virtual bool sendTrans(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans,
					AosBuffPtr &resp,
					bool &svr_death);

	virtual bool sendTrans(
					const AosRundataPtr &rdata,
					const AosTransPtr &trans,
					bool &svr_death);

	virtual bool recvResp(
					const AosTransId &trans_id,
					const AosBuffPtr &resp,
					const int from_sid);
	virtual bool recvAck(
					const AosTransId &trans_id,
					const int from_sid);
	virtual bool recvSvrDeath(
					const AosTransId &trans_id, 
					const int death_sid);
	virtual bool triggerResend(const AosTriggerResendMsgPtr &msg);

	// Chen Ding, 2013/09/28
/*
	virtual u64 addTransCaller(
					const AosRundataPtr &rdata, 
					const AosTransCallerObjPtr &caller);
	virtual u64 removeTransCaller(
					const AosRundataPtr &rdata, 
					const u64 caller_id);
*/
	virtual bool transFinished(
					const AosRundataPtr &rdata, 
					const u64 trans_id, 
					const u64 caller_id, 
					const bool status, 
					const OmnString &errmsg, 
					const AosBuffPtr &data);
private:
	bool 	sendTransPriv(
				const AosRundataPtr &rdata,
				const AosTransPtr &trans);

	bool 	resendThrdFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool 	resendAllTrans(const AosTriggerResendMsgPtr msg);
	bool 	resendTransPriv(const AosTransPtr &trans);

	bool 	readyToSend(const AosTransPtr &trans);
	bool 	addToWaitQueue(const AosTransPtr &trans);
	bool 	removeFromWaitQueue(const AosTransId &tid);

};
#endif
