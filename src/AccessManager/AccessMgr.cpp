////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AccessMgr.cpp
// Description:
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "AccessManager/AccessMgr.h"
#include "AccessManager/AmThread.h"
#include "AmProcFunc/AmProcFunc.h"
#include "AmProcFunc/UserBasedAC.h"

#include "AmUtil/AmExternalSvrMgr.h"
#include "AmUtil/AmRcvBuff.h"
#include "AmUtil/AmTrans.h"
#include "AmUtil/AmMsg.h"
#include "AmUtil/AmTagId.h"
#include "aosUtil/Alarm.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/ReturnCode.h"
#include "UtilComm/TcpBouncerListener.h"

#define AM_THREAD_POOL_SZ 30

AosAccessMgr::AosAccessMgr(
			const OmnString &dbConnStr, 
            const OmnIpAddr &localAddr,
            const int localPort,
            const int numLocalPorts,
            const int maxConns)
:
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(numLocalPorts),
mLock(OmnNew OmnMutex()),
mRecvQueueLock(OmnNew OmnMutex()),
mSendQueueLock(OmnNew OmnMutex()),
mThreadLock(OmnNew OmnMutex()), 
mRecvQueueCondVar(OmnNew OmnCondVar()),	// receive  message queue cond
mSendQueueCondVar(OmnNew OmnCondVar()),	// send out message queue cond
mThreadCondVar(OmnNew OmnCondVar()),
mRcvBuffHtableLock(OmnNew OmnMutex()),
//mSessionTableCondVar(OmnNew OmnCondVar()),
mSessionTableLock(OmnNew OmnMutex()),
mExternalSvrMgrLock(OmnNew OmnMutex())
{
	initialTransFunc();
	mServer = OmnNew OmnTcpServerGrp(localAddr, localPort,
                 numLocalPorts, "AccessMgr", maxConns, 
				 OmnTcp::eNoLengthIndicator);
	OmnTcpCltGrpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);

	// we have at most AM_THREAD_POOL_SZ[30] threads
	AosAmThreadPtr theThread;
	for(int i = 0; i < AM_THREAD_POOL_SZ; i++)
	{
		mThreadLock->lock();
		mThreads.append(theThread);
		mThreadLock->unlock();
	}

	mExternalSvrMgr = OmnNew AosAmExternalSvrMgr(dbConnStr.data());
	if(!mExternalSvrMgr->initialize())
	{
		OmnAlarm << "External Server Manager creation fail!" << enderr;
	}
}


AosAccessMgr::~AosAccessMgr()
{
	stopReading();
}


bool
AosAccessMgr::connect(OmnString &errmsg)
{
	if (!mServer)
	{
		errmsg = "Server not created before connecting";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mServer->connect(errmsg);
}

void
AosAccessMgr::msgRecved(const OmnTcpCltGrpPtr &group, 
						const OmnConnBuffPtr &buff,
						const OmnTcpClientPtr &conn)
{
	// OmnString request;
	// 
	// We received something from a client. This function processes the
	// message. 
	//
	// 
	// request = buff->getString();
	//
	// OmnTrace << "Received AM request: " << request << endl;

	// 
	// 1. Look up the AosAmRcvBuff based on 'conn'
	// 2. If not found, create the instance
	// 3. Call AosAmRcvBuff::append(buff);
	// 4. Slice the buffer and put the messages into the queue.
	// 

	// 
	// 1. Look up the instance
	//
	AosAmRcvBuff bb(conn);
	AosAmRcvBuffPtr bbPtr(&bb, false);
	mRcvBuffHtableLock->lock();
	AosAmRcvBuffPtr rcvBuff = mRcvBuffHtable.get(bbPtr);
	mRcvBuffHtableLock->unlock();

	if (!rcvBuff)
	{
		// 2. If not found, create the instance
		// 3. Call AosAmRcvBuff::append(buff);
		// 
		// Did not find it. Create one!
		//
		OmnAlarm << "Server new conn not create recieve buffer." << enderr;
		rcvBuff = OmnNew AosAmRcvBuff(conn, buff);
		aos_assert(rcvBuff);
		mRcvBuffHtableLock->lock();
		mRcvBuffHtable.add(rcvBuff);
		mRcvBuffHtableLock->unlock();
	}
	else
	{
		// 3. Call AosAmRcvBuff::append(buff);
		rcvBuff->append(buff);
	}

	// 4. Slice the buffer and put the transaction messages into the queue.
	AosAmTransPtr trans;
	while ((trans = rcvBuff->getTrans()))
	{
		// add the trans to the queue;
		// Create an AmTrans and put it into the queue
		//
		mRecvQueueLock->lock();
		mRecvQueue.append(trans);
		mRecvQueueCondVar->signal();
		mRecvQueueLock->unlock();
	}

	// 
	// Parse the request
	// AosAmParser parser(request);
	// OmnString errmsg;
	// if (!parser.parse(req, errmsg))
	// {
	// 	sendResponse(conn, errmsg);
	// 	return;
	// }
	// if (!req.isGood())
	// {
	// 	sendResponse(conn, "Syntex error");
	// 	return;
	// }
}


bool
AosAccessMgr::sendResponse(const AosAmTransPtr &trans, const OmnString &errmsg)
{
	// 
	// Create a negative response, put it into the trans, enqueue the trans.
	// 
	if(!trans)
	{
		return false;
	}
	trans->setErrMsg(errmsg);

	mSendQueueLock->lock();
	mSendQueue.append(trans);
	mSendQueueCondVar->signal();
	mSendQueueLock->unlock();
	
	// can't create another thread in this class 
	mSendQueue.popFront();
	trans->sendOut();
	
	return true;
}


bool
AosAccessMgr::sendResponse(const AosAmTransPtr &trans)
{
	mSendQueueLock->lock();
	mSendQueue.append(trans);
	mSendQueueCondVar->signal();
	mSendQueueLock->unlock();
	
	// can't create another thread in this class 
	mSendQueue.popFront();
	trans->sendOut();
	
	return true;
}


bool
AosAccessMgr::startReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before startReading()" << enderr;
		return false;
	}

	mServer->startReading();

	// Start transaction process
	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "TcpCltGrp", 0, false, true, __FILE__, __LINE__);
	}

	mThread->start();

	return true;
}


bool
AosAccessMgr::stopReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before stopReading()" << enderr;
		return false;
	}

	mServer->stopReading();
	return true;
}


void
AosAccessMgr::newConn(const OmnTcpClientPtr& conn)
{
	// 
	// A new connection has been created. 
	//
	AosAmRcvBuff bb(conn);
	AosAmRcvBuffPtr bbPtr(&bb, false);
 	mRcvBuffHtableLock->lock();
	AosAmRcvBuffPtr rcvBuff = mRcvBuffHtable.get(bbPtr);
	if (!rcvBuff)
	{
		rcvBuff = OmnNew AosAmRcvBuff(conn);
		aos_assert(rcvBuff);

		mRcvBuffHtable.add(rcvBuff);
	}
	else
	{
		OmnAlarm << "Server old recieve buffer not deleted yet." << enderr;
	}

	mRcvBuffHtableLock->unlock();
	return;
}


bool        
AosAccessMgr::connClosed(const OmnTcpCltGrpPtr &, 
                         const OmnTcpClientPtr &conn)
{
	// 
	// A connection has been closed. 
	//
	AosAmRcvBuff bb(conn);
	AosAmRcvBuffPtr bbPtr(&bb, false);
 	mRcvBuffHtableLock->lock();
	// find it and delete it
	mRcvBuffHtable.get(bbPtr, true);
 	mRcvBuffHtableLock->unlock();

	return true;
}


// 
// Distributor
// The thread function for Queued Message distribution
// 
bool    
AosAccessMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosAmTransPtr trans;
	AosAmThreadPtr theThread;

    while (state == OmnThrdStatus::eActive)
	{
		if (!trans)
		{
			mRecvQueueLock->lock();
			if (mRecvQueue.entries() <= 0)
			{
				mRecvQueueCondVar->wait(mRecvQueueLock);
				mRecvQueueLock->unlock();
				continue;
			}

			trans = mRecvQueue.popFront();
			mRecvQueueLock->unlock();
		}

		mThreadLock->lock();
		if (mThreads.entries() <= 0)
		{
			mThreadCondVar->wait(mThreadLock);
			mThreadLock->unlock();
			continue;
		}

		theThread = mThreads.popFront();
		mThreadLock->unlock();
		if(!theThread)
		{
			// the thread has not created yet. Create one.
			theThread = OmnNew AosAmThread(AosAccessMgrPtr(this));
		}
		theThread->process(trans);
		trans = 0;
	}

	return true;
}


// Sendor
// The thread function for send out the Queued Message result
bool    
AosAccessMgr::senderThreadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosAmMsgPtr msg;
	AosAmTransPtr trans;
	OmnTcpClientPtr conn;

    while (state == OmnThrdStatus::eActive)
	{
		mSendQueueLock->lock();
		if (mSendQueue.entries() <= 0)
		{
			mSendQueueCondVar->wait(mSendQueueLock);
			mSendQueueLock->unlock();
			continue;
		}

		trans = mSendQueue.popFront();
		mSendQueueLock->unlock();

		// 
		// Get the message from the transaction
		// Get the connection from the transaction
		// Use that connection to send the message
		// 
//		msg = trans->getMsg();
//		conn = trans->getConn();
//		conn->writeTo(msg->getData(), msg->getDataLength());
		trans->sendOut();
		trans = 0;
	}

	return true;
}


bool    
AosAccessMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosAccessMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


void 
AosAccessMgr::returnThread(const AosAmThreadPtr &thread)
{
	mThreadLock->lock();
	mThreads.append(thread);
	mThreadCondVar->signal();
	mThreadLock->unlock();
}


AosAmProcFunc 
AosAccessMgr::lookupTransFunc(const AosAmRequestSvrTypeId::E requestId)
{
	if(requestId >= AosAmRequestSvrTypeId::eMaxSvrTypeId)
	{
		return 0;
	}
	return mProcFuncTable[requestId];
}


int 
AosAccessMgr::initialTransFunc()
{
	for(int i = 0; i < AosAmRequestSvrTypeId::eMaxSvrTypeId; i++)
	{
		mProcFuncTable[i] = 0;
	}
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeLocal] 			= (&AmAttLocal);
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeLdap] 			= (&AmAttLdap);
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeRadius] 			= (&AmAttRadius);
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeAd] 				= (&AmAttActivedirectory);
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeExternal] 		= (&AmAttExternal);
	mProcFuncTable[AosAmRequestSvrTypeId::eSvrTypeReserved] 		= (&AmAttReserved);

	return 0;
}


u16 
AosAccessMgr::getSessionId(const OmnString &usr)
{
	u16 sessionId = 0;

	mSessionTableLock->lock();
	if(!mSessionTable.get(usr.data(), sessionId, false))
	{
		mMaxSessionId++;
		sessionId = mMaxSessionId;
	}
	mSessionTableLock->unlock();

	return sessionId;
}


bool 
AosAccessMgr::addSessionId(const OmnString &usr, const u16 sessionId)
{
	bool ret;

	mSessionTableLock->lock();
	ret = mSessionTable.add(usr.data(), sessionId);
	mSessionTableLock->unlock();

	return ret;
}


void 
AosAccessMgr::removeSessionId(const OmnString &usr)
{
	u16 sessionId;

	mSessionTableLock->lock();
	mSessionTable.get(usr.data(), sessionId, true);
	mSessionTableLock->unlock();
}


bool 
AosAccessMgr::addUser(u16 &sessionId, const OmnString &usr)
{
	bool ret = true;

	mSessionTableLock->lock();
	if(!mSessionTable.get(usr.data(), sessionId, false))
	{
		mMaxSessionId++;
		sessionId = mMaxSessionId;
		ret = mSessionTable.add(usr.data(), sessionId);
		if(!ret)
		{
			OmnAlarm << "Fail to add user["<< usr << "] to server!" << enderr;
		}
	}
	mSessionTableLock->unlock();

	return ret;
}


bool 
AosAccessMgr::getMsgSvrArray(const u8 msgId, OmnVList<AosAmExternalSvrPtr> &serverArray)
{
	bool retVal;

	mExternalSvrMgrLock->lock();
	if(!mExternalSvrMgr->isGood())
	{
		mExternalSvrMgrLock->unlock();
		OmnAlarm << "External Server Manager not available!" << enderr;
		return false;
	}
	retVal = mExternalSvrMgr->getMsgSvrArray(msgId, serverArray);
	mExternalSvrMgrLock->unlock();

serverArray.reset();
AosAmExternalSvrPtr tmpSvr = serverArray.next();
printf("Server response trace response OK!mMsgQueue[%d].entries=[%d] tmpSvr[%d] FILE(%s)LINE(%d)\n",msgId, serverArray.entries(), (int)tmpSvr, __FILE__, __LINE__ );
// printf("Server response trace response OK!tmpSvr[%d] tmpSvr->getServerTypeId=[%d] FILE(%s)LINE(%d)\n", (int)tmpSvr, tmpSvr->getServerTypeId(), __FILE__, __LINE__ );

	return retVal;
}
#endif
