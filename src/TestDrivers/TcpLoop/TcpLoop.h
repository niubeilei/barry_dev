////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tcp.h
// Description:
//   
//
// Modification History:
// 08/04/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TcpLoop_TcpLoop_h
#define Omn_TcpLoop_TcpLoop_h

#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/IpAddr.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/CommListener.h"


class OmnTcpLoop : public OmnTcpListener, 
				   public OmnThreadedObj,
				   public OmnCommListener
{
public:
	OmnDefineRCObject;

	enum
	{
		eFailedCreateServerSleep = 1,
		eFailedCreateClientSleep = 1,
		eFailedToSendSleep = 1,
		eMaxSendBlockSize = 65535
	};

	enum BounceMethod
	{
		eDrop, 
		eSimpleBounce
	};

private:
	OmnIpAddr			mServerAddr;
	OmnIpAddr			mClientAddr;
	OmnIpAddr			mRemoteAddr;
	u32					mServerPort;
	u32					mClientPort;
	u32					mRemotePort;

	OmnTcpCommCltPtr	mClient;
	OmnTcpServerPtr		mServer;
	BounceMethod		mBounceMethod;
	OmnThreadPtr		mSendingThread;
	OmnString			mSendData;
	u32					mSendBlockSize;
	int					mSendSize;
	u32					mSendWaitSec;
	u32					mSendWaitUsec;
	u64					mTotalReceived;
	u64					mLastTotal;

public:
	OmnTcpLoop();
	OmnTcpLoop(const OmnIpAddr &remoteAddr, 
			   const u32 remotePort,
			   const OmnIpAddr &clientAddr,
			   const u32 clientPort, 
			   const OmnIpAddr &serverAddr,
			   const u32 serverPort, 
			   const BounceMethod method, 
			   const u32 sendBlockSize, 
			   const int sendSize,
			   const u32 sendWaitSec, 
			   const u32 sendWaitUsec);
	virtual ~OmnTcpLoop();

	// 
	// TcpListener Interface
	//
	virtual OmnString   getTcpListenerName() const {return "TcpLoop";}
	virtual void        msgRecved(const OmnConnBuffPtr &buff,
			   				const OmnTcpClientPtr &conn);

	// 
	// CommListener Interface
	//
	virtual bool        msgRead(const OmnConnBuffPtr &buff);
	virtual OmnString   getCommListenerName() const {return "TcpLoop";}

	// 
	// The ThreadedObj interface (for sending thread)
	//
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual void    heartbeat(const int tid);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool		connect(OmnString &errmsg);
	bool		start();
	u64			getTotalReceived() const {return mTotalReceived;}
	u64			getLastTotal() const;
	void		resetLastTotal() {mLastTotal = 0;}
	u32			getServerPort() const {return mServerPort;}
	u32			getClientPort() const {return mClientPort;}


private:
	bool		createServer(OmnString &errmsg);
	bool		createClient(OmnString &errmsg);
};
#endif

