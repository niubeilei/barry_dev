////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 10/12/2010	Created by Chen Ding
// 2014/01/28 Turned off by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_DataSync_Conn_h
#define AOS_DataSync_Conn_h

#include "DataSync/DsTags.h"
#include "DataSync/Ptrs.h"
#include "HealthCheck/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/CondVar.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/IpAddr.h"
#include "Util/HashUtil.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "TransUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"

#include <map>
#include <queue>
using namespace std;

class AosConn: virtual public OmnThreadedObj, public OmnCommListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxPending = 10,
		eWaitTime = 3,
		eMonitorFreqSec = 30,
		eQueueLength = 1000
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr       mCondVar;
	OmnString			mClientId;
	OmnThreadPtr		mMonitorThread;
	OmnString			mAddr;
	int					mPort;
	OmnTcpCommCltPtr	mConn;
	u32					mLogid;
	AosRundataPtr		mRundata;

public:
	AosConn();
	AosConn(const AosXmlTagPtr &def);
	~AosConn();

	// OmnCommListener Interface
	virtual bool        msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString   getCommListenerName() const;
	virtual void        readingFailed(){};

	// Thread Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
	bool	start(const AosXmlTagPtr &def);

private:
	bool    monitorThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	bool	createConnection();
	bool	procResp(const AosXmlTagPtr &root, 
				const AosXmlTagPtr &child, 
				const AosRundataPtr &);
	bool	sendResp(const OmnString req);
};

#endif
#endif
