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
// 	Created: 06/03/2011 by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SvrProxyMgr_IpcClt_h
#define AOS_SvrProxyMgr_IpcClt_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/SeConfig.h"
#include "SvrProxyUtil/Ptrs.h"
#include "SvrProxyMgr/Ptrs.h"
#include "SvrProxyMgr/ConnHandler.h"
#include "Thread/ThreadedObj.h"
#include "Thread/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "Util/TransId.h"
#include "Util/Ptrs.h"
#include "Util/String.h"

#include <deque>
#include <set>
using namespace std;

class AosIpcClt: public OmnThreadedObj,
				public AosIpcCltObj
{
	OmnDefineRCObject;
	
	enum
	{
		eMaxConnTrys = 5,
		eReConnTime = 2,
		
		eRecvConnThrdId = 1,
	};
	
private:
	OmnMutexPtr		mLock;
	OmnString		mTmpDir;
	OmnString		mUpath;

	OmnThreadPtr	mRecvThrd;
	
	AosIpcCltInfoPtr mIpcConn;
	bool			mShowLog;

public:
	AosIpcClt();
	~AosIpcClt();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId);

	virtual bool config(const AosXmlTagPtr &app_conf);
	virtual bool start();
	virtual bool stop();
	
	virtual bool connFailed();
	
	virtual void    setThreadStatus(){mThreadStatus = true;}
	
	virtual bool sendMsg(const AosAppMsgPtr &msg);
	virtual bool sendTrans(const AosTransPtr &trans);
	

private:
	bool 	connectIpcSvr(const int sock);
	bool 	recvMsg(const AosAppMsgPtr &msg);

};
#endif
