////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
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
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Aos_AccessManager_AccessMgr_h
#define Aos_AccessManager_AccessMgr_h
 
#include "AccessManager/Ptrs.h"
#include "AmProcFunc/AmProcFunc.h"
#include "AmUtil/Ptrs.h"
#include "AmUtil/AmRequestTypeId.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/AsciiHexHash.h"
#include "Util/String.h"
#include "Util/IpAddr.h"
#include "Util/ValList.h"
#include "Util/HashSPtr.h"
#include "Util1/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/TcpCltGrpListener.h"


//typedef int (*AosAmProcFunc)(const AosAmTransPtr &trans, const AosAccessMgrPtr & amMgr);

class AosAccessMgr : public OmnTcpCltGrpListener, 
					 public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxSock = 5000
	};
	OmnIpAddr			mLocalAddr;
	int					mLocalPort;
	int					mNumLocalPorts;
	OmnMutexPtr			mLock;
	OmnTcpServerGrpPtr	mServer;
	OmnThreadPtr		mThread;
//	AosTcpBouncerListenerPtr	mListener;
	OmnMutexPtr			mRecvQueueLock;		// receive  message queue lock
	OmnMutexPtr			mSendQueueLock;		// send out message queue lock
	OmnMutexPtr			mThreadLock;
	OmnCondVarPtr		mRecvQueueCondVar;	// receive  message queue cond
	OmnCondVarPtr		mSendQueueCondVar;	// send out message queue cond
	OmnCondVarPtr		mThreadCondVar;
	OmnVList<AosAmThreadPtr>	mThreads;
	OmnVList<AosAmTransPtr>		mRecvQueue;	// receive  message queue
	OmnVList<AosAmTransPtr>		mSendQueue;	// send out message queue 

	OmnMutexPtr 				mRcvBuffHtableLock;	// receive buffer queue lock
	OmnHashSPtr<AosAmRcvBuffPtr, eMaxSock>	mRcvBuffHtable;
	AosAmProcFunc 		mProcFuncTable[AosAmRequestSvrTypeId::eMaxSvrTypeId];

	// this sequentially algorithm may encounter error ....
	u16 				mMaxSessionId; 			// current max session Id
	OmnAsciiHexHash<u16, 2048>	mSessionTable;	// set the session Id
//	OmnCondVarPtr		mSessionTableCondVar;
	OmnMutexPtr			mSessionTableLock;

	AosAmExternalSvrMgrPtr 	mExternalSvrMgr;
	OmnMutexPtr 			mExternalSvrMgrLock;

public:
	AosAccessMgr(
			const OmnString &dbConnStr, 
			const OmnIpAddr &localAddr, 
			const int localPort, 
			const int numLocalPorts, 
			const int maxConns);

	~AosAccessMgr();

	// 
	// TcpListener Interface
	//
    virtual OmnString   getTcpListenerName() const {return "AccessManager";}
	virtual void		msgRecved(const OmnTcpCltGrpPtr &group, 
								  const OmnConnBuffPtr &buff,
								  const OmnTcpClientPtr &conn);
	virtual void		newConn(const OmnTcpClientPtr&);
    virtual bool        connClosed(const OmnTcpCltGrpPtr &, 
                                const OmnTcpClientPtr &client);

	// 
	// ThreadedObj interface
	//
	// The thread function for Queued Message distribution
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	// The thread function for send out the Queued Message result
	virtual bool 	senderThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	void		returnThread(const AosAmThreadPtr &thread);
	bool		connect(OmnString &errmsg);
	bool		startReading();
	bool		stopReading();

	bool		sendResponse(const AosAmTransPtr &trans, const OmnString &errmsg);
	bool		sendResponse(const AosAmTransPtr &trans);

	AosAmProcFunc lookupTransFunc(const AosAmRequestSvrTypeId::E );

	u16 		getSessionId(const OmnString &usr);
	bool 		addSessionId(const OmnString &usr, const u16 sessionId);
	void 		removeSessionId(const OmnString &usr);
	bool 		addUser(u16 &sessionId, const OmnString &usr);

	bool 		getMsgSvrArray(const u8 msgId, OmnVList<AosAmExternalSvrPtr> &serverArray);
private:
	int initialTransFunc();
};

#endif
#endif
