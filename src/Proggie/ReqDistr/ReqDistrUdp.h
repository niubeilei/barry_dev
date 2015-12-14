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
#ifndef Aos_Proggie_ReqDistr_ReqDistrUdp_h
#define Aos_Proggie_ReqDistr_ReqDistrUdp_h

// #include "Packages/Frontend/Ptrs.h"
#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ReqDistr/ConnListener.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/CommListener.h"
#include "UtilComm/AddrDef.h"
#include "UtilComm/PortDef.h"
#include "UtilComm/CommTypes.h"
#include "XmlUtil/Ptrs.h"
#include <list>
#include <queue>
using namespace std;


class AosReqDistrUdp : public OmnCommListener
{
	OmnDefineRCObject;

private:
	enum
	{
		eDftMaxConns = 500,
		eMaxNumThreads = 300,
		eMaxSock = 1024,
		eDftMaxQueueSize = 10000
	};

	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	OmnString			mServiceName;
	AosTcpLengthType	mReqDelinerator;
	int					mMaxProcTime;
	int 				mNumThreads;	
	OmnMutex*			mLock;
	OmnCondVar *		mCondVar;
	OmnThreadPtr		mThread;
	AosNetReqProcPtr	mProcessor;
	AosConnListenerPtr	mConnListener;
	AosFrontendPtr		mFrontends[eMaxNumThreads];
	OmnConnBuffPtr		mReqHead;
	OmnConnBuffPtr		mReqTail;
	OmnUdpCommPtr		mServer;
	bool				mIsBlocking;
	u32					mMaxQueueSize;
	queue<OmnConnBuffPtr>	mQueue;

public:
	AosReqDistrUdp(const int num_threads, const AosNetReqProcPtr &proc);
	~AosReqDistrUdp();

	bool 	config(const AosXmlTagPtr &def);
	bool 	config(
				const OmnString &local_addr, 
				const OmnString &local_port,
				const OmnString &server_name,
				const int max_conns)
	{
		return config(local_addr, local_port, server_name, "first_four_high", 
				1000, max_conns);
	}
	bool 	config(
				const OmnString &local_addr, 
				const OmnString &local_port,
				const OmnString &server_name,
				const OmnString &deli,
				const int max_proc_time,
				const int max_conns);
	bool	init();
	bool	start();
	bool	stop();

	// OmnCommListener Interface
	virtual bool		msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString	getCommListenerName() const;
	virtual void 		readingFailed();

	virtual void	connCreated(const OmnTcpClientPtr &conn)
					{
						if (mConnListener)
							mConnListener->connCreated(conn);

					}
					
	virtual void    connClosed(const OmnTcpClientPtr &conn)
					{
						if (mConnListener)
							mConnListener->connClosed(conn);
					}

	// ThreadedObj Interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
//	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	void			setConnListener(const AosConnListenerPtr &listener)
					{
						mConnListener = listener;
					}
	OmnConnBuffPtr	getRequest(const int index);
};
#endif

