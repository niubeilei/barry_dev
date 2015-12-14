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
// 04/29/2012: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEClient_TcpClientTester_TcpClientThrd_h
#define Aos_SEClient_TcpClientTester_TcpClientThrd_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "SEUtil/MetaExtension.h"
#include "SEInterfaces/Ptrs.h"
#include "TaskMgr/Ptrs.h"
#include "TransServer/Ptrs.h"
#include "TransUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <deque>
using namespace std;

class AosTcpClientThrd : virtual public OmnThreadedObj
{
	OmnDefineRCObject;
	
private:
	OmnMutexPtr        	mLock;
	OmnCondVarPtr      	mCondVar;
	u32					mThreadId;
	OmnThreadPtr		mThread;
	OmnTcpClientPtr		mClient;
public:
	AosTcpClientThrd(
			const OmnString &name,
			const OmnString &addr,
			const int port);

	~AosTcpClientThrd();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual void	heartbeat(const int thrdLogicId);
	virtual bool    signal(const int threadLogicId);
private:
	bool sendData();
};
#endif
