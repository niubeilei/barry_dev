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
// 03/03/2008: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_XmlInterface_WebProcMgr_h
#define Aos_XmlInterface_WebProcMgr_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"
#include "XmlInterface/Ptrs.h"
#include "XmlInterface/XmlRc.h"
#include <list>

class AosWebProcMgr : public OmnTcpCltGrpListener, 
					  public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eNumThreads = 10,
		eMaxConns = 600,
		eMaxNumThreads = 300
	};

	OmnIpAddr					mLocalAddr;
	int							mLocalPort;
	int							mNumPorts;
	int 						mTotalThreads;	
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	OmnThreadPtr				mThread;
	bool						mThreadStatus;
	OmnTcpServerGrpPtr			mServer;
	std::list<AosWebProcThreadPtr>	mIdleThreads;
	std::list<AosWebProcReqPtr>		mRequests;
	AosWebProcThreadPtr			mProcThreads[eNumThreads];

public:
	AosWebProcMgr(const OmnString &addr, const int port, const int numPorts);
	~AosWebProcMgr();

	bool		start();
	bool		stop();

	// OmnTcpCltGrpListener Interface
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
							const OmnConnBuffPtr &buff, 
							const OmnTcpClientPtr &conn);
	virtual	void		newConn(const OmnTcpClientPtr &conn);
	virtual bool		connClosed(const OmnTcpCltGrpPtr &, 
							const OmnTcpClientPtr &client);

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	// void 			returnThread(const AosWebProcThreadPtr &thread);
	AosWebProcReqPtr getRequest();
};
#endif

