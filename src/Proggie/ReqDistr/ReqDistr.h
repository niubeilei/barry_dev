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
#ifndef Aos_Proggie_ReqDistr_ReqDistr_h
#define Aos_Proggie_ReqDistr_ReqDistr_h

#include "Proggie/ReqDistr/Ptrs.h"
#include "Proggie/ReqDistr/ConnListener.h"
#include "Proggie/ProggieUtil/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/AddrDef.h"
#include "UtilComm/PortDef.h"
#include "UtilComm/CommTypes.h"
#include "XmlUtil/Ptrs.h"
#include <list>


class AosReqDistr : public OmnTcpListener, public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eInitNumThreads = 20,
		eDftMaxConns = 500,
		eMaxNumThreads = 300,
		eMaxSock = 1024
	};

	AosAddrDef			mLocalAddrDef;
	AosPortDef			mLocalPorts;
	OmnString			mServiceName;
	AosTcpLengthType	mReqDelinerator;
	int					mMaxProcTime;
	int					mMaxConns;
	int 				mNumThreads;	
	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	AosNetReqProcPtr	mProcessor;
	AosConnListenerPtr	mConnListener;
	AosReqProcThrdPtr	mProcThreads[eMaxNumThreads];
	OmnConnBuffPtr		mReqHead;
	OmnConnBuffPtr		mReqTail;
	OmnTcpServerEpollPtr	mServer;
	bool				mIsBlocking;

public:
	AosReqDistr(const AosNetReqProcPtr &proc);
	~AosReqDistr();

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

	// OmnTcppListener Interface
	virtual OmnString	getTcpListenerName() const;
	virtual void		msgRecved(const OmnConnBuffPtr &buff, 
							const OmnTcpClientPtr &conn);
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

