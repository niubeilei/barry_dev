////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgent_JimoAgentReport_h
#define Aos_JimoAgent_JimoAgentReport_h

#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "UtilComm/TcpClient.h"
#include "Util/RCObject.h"
#include "Util/Ptrs.h"

class AosJimoAgentReport : public OmnThreadedObj
{
	OmnDefineRCObject;
private:
	int						mHeartBeatTime;
	OmnTcpClientPtr			mClient;
	OmnThreadPtr			mThread;
public:
	AosJimoAgentReport(
			const OmnString &addr,
			const int port,
			const int heartbeat_time);
	~AosJimoAgentReport();
	bool			start();
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	virtual bool    signal(const int threadLogicId);
private:
	bool 			sendReport();
	OmnString 		getReport();
};
#endif

