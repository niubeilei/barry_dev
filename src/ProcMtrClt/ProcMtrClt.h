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
// 09/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_ProcMtr_ProcMtrClt_h
#define Aos_ProcMtr_ProcMtrClt_h

#include "SEUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommTypes.h"


class AosProcMtrClt : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnTcpClientPtr		mConn;
	int					mProcid;
	OmnIpAddr			mAddr;
	int					mPort;
	int					mNumPorts;
	AosTcpLengthType	mLenInd;
	OmnString			mAppName;
	int					mRetryFreq;

public:
	AosProcMtrClt();
	~AosProcMtrClt();

	// OmnThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	start();
	bool	config(const AosXmlTagPtr &config);

private:
	bool	connect();
	bool 	procReq(const AosXmlTagPtr &req);
	bool 	procHeartbeat(const AosXmlTagPtr &req);
	bool 	sendInitHeartbeat();
};
#endif

