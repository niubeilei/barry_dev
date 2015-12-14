////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_TransServer_TransServer_h
#define Aos_TransServer_TransServer_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEInterfaces/ProcessType.h"
#include "SEUtil/MetaExtension.h"
#include "UtilComm/Ptrs.h"
#include "Util/Ptrs.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SvrProxyUtil/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Util/TransId.h"
#include "XmlInterface/Server/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <queue>
#include <map>
#include <set>
#include <list>
using namespace std;


class AosTransServer : public AosTransSvrObj
{
	OmnDefineRCObject;
	
	enum 
	{
		eMaxProcThrdNum = 20,
		
		eProcThrdId = 0,

		eMinCacheSize = 2000000000,
	};
	
private:
	OmnMutexPtr             	mLock;
	u32							mProcThrdNum;
	AosTransSvrThrdPtr			mProcThrds[eMaxProcThrdNum];
	AosProcThrdMonitorPtr		mProcThrdMonitor;

	u64							mMaxCacheSize;
	u64							mCrtCacheSize;

	queue<AosTransPtr>		    mIgnoreTrans;
	set<AosTransId>			    mIgnoreTransIds;
	map<u64, u64>				mMaxRecvSeq;

protected:
	//Sts							mStatus;
	AosTransFileMgrPtr			mTransFileMgr;
	bool						mShowLog;

public:
	AosTransServer();
	~AosTransServer();
	
	virtual bool	config(const AosXmlTagPtr &conf);
	virtual bool 	start();
	virtual bool 	stop();

	virtual bool	recvMsg(const AosAppMsgPtr &msg);
	virtual bool 	finishTrans(vector<AosTransId> &trans_ids);
	virtual bool    finishTrans(const AosTransPtr &trans);
	virtual void    resetCrtCacheSize(const u64 proced_msg_size);

	// CubeTransSvr call this func.
	bool switchFrom(const AosTransServerPtr &from);

	// for TransFileMgr.
	bool serializeMaxRecvSeq(const AosBuffPtr &buff);

	// for TransSvrThrd.
	//virtual bool procedMsgSize(const u64 proced_msg_size);

protected:
	bool 	startThrd();

	//void	setActive(){ mStatus = eActive; };
	//void	setIdle(){ mStatus = eIdle; };

	bool 	pushMsgToQueue(const AosAppMsgPtr &msg);
	bool	tryPushTransToQueue(const AosTransPtr &trans);
	
	bool 	isRecvBefore(const AosTransPtr &trans);
	void 	addIgnoreTrans(const AosTransPtr &trans);
	bool 	setMaxRecvPriv(const AosTransPtr &trans);
	bool 	tryToSetMaxRecv(const u64 clt_key, const u64 crt_seq);

	bool 	isCacheFull();
	bool 	hasIgnoreTrans();
	bool 	pushManyTransToQueue(vector<AosTransPtr> &v_trans);
	bool 	pushIgnoreTransToQueue();

private:
	virtual bool contPushTransToQueue();
	bool 	recvTrans(const AosTransPtr &trans);

	bool 	pushIgnoreTransToQueuePriv();
	bool 	pushToQueueLocked(const AosAppMsgPtr &msg);

	AosTransSvrThrdPtr getProcThrd(const AosAppMsgPtr &msg);
	bool	reSet(const u64 &check_key);
	void 	addIgnoreTransPriv(const AosTransPtr &trans);

};
#endif
