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
// Modification History:
// 3/27/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "AmClient/AmClient.h"

#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "AmClient/AmServer.h"
#include "AmClient/ApiUserBased.h"
#include "AmClient/AmThread.h"
#include "AmUtil/AmMsg.h"
#include "AmUtil/Ptrs.h"
#include "AmUtil/AmRequestTypeId.h"
#include "AmUtil/ReturnCode.h"
#include "Porting/Select.h"
#include "Thread/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
//#include "Thread/ThrdTrans.h"
//#include "Thread/ThrdMsgQueue.h"
#include "Thread/Thread.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"

static u16 sgTransId = 10;
static OmnMutex sgLock;

static u32 inline getTransId()
{
	return (sgTransId++ == 0)?++sgTransId:sgTransId;
}


AosAmTransMutexCondVar::AosAmTransMutexCondVar(u16 transId)
:
mTransId(transId),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMsg(0)
{}


AosAmTransMutexCondVar::~AosAmTransMutexCondVar()
{
}


AosAmClient::AosAmClient()
:
mServerIdx(0), 
mSlbMethod(eRoundRobin),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMsgQueueLock(OmnNew OmnMutex()),
//mMsgQueueCondVar(OmnNew OmnCondVar()),
//mMsgQueue(OmnNew AosThrdMsgQueue()),
mTimer(eDefaultTimer)
{
    FD_ZERO(&mReadFds);
    mFdscnt = 0;
}


AosAmClient::~AosAmClient()
{
}


bool
AosAmClient::authUser(const OmnString &user, 
					  const OmnString &password, 
					  AosAmRespCode::E &respCode,
					  OmnString &errmsg)
{
	AosAmServerPtr server = selectServer(user);
	if (!server)
	{
		OmnAlarm << "No available server" << enderr;
		return false;
	}

	mLock->lock();
	AosAmClientConnInfoPtr conn;
	u32 sessionId = 0;
	if (mConnTable.get(user.data(), conn, false))
	{
		sessionId = conn->getSessionId(server);
		if(sessionId)
		{
			respCode = AosAmRespCode::eAllowed;
			mLock->unlock();
			return true;
		}
	}
	mLock->unlock();

	// 
	// It sends an Access Request to one of the servers. 
	// 
	AosAmMsg req(500);
	u16 transId = (u16)getTransId();
	AosAmTransMutexCondVarPtr transSigPtr = OmnNew AosAmTransMutexCondVar(transId);

	mMsgQueueLock->lock();
	mMsgQueue.append(transSigPtr);
	mMsgQueueLock->unlock();

	req.setTransId(transId);
	req.setMsgId(AosAmMsgId::eUserAuth);
	req.setSessionId(sessionId);
//	aos_assert_r(req.addTag(AosAmTagId::eTransFuncId, (u16)AosAmRequestTypeId::eAmAttLocalApiId), false);
	aos_assert_r(req.addTag(AosAmTagId::eUser, user), false);
	aos_assert_r(req.addTag(AosAmTagId::ePasswd, password), false);

OmnAlarm << "SessionId[" << (int)req.getSessionId() << "]" << enderr;

	if (!sendMsg(req, server))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not send the request.";
		OmnAlarm << "Failed to send the request" << enderr;
		return false;
	}

	// 
	// Load response message of the same transaction Id
	// 
	OmnConnBuffPtr buffPtr;
	AosAmMsgPtr resp;
	if(!receiveResp(resp, transSigPtr))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not received response.";
		return false;
	}

OmnAlarm << "SessionId[" << (int)resp->getSessionId() << "]" << enderr;
	resp->parse();
	if (resp->isRespPositive())
	{
		sessionId = resp->getSessionId();
		mLock->lock();
		if(!mConnTable.get(user.data(), conn, false))
		{
			conn = OmnNew AosAmClientConnInfo();
			conn->setSessionId(server, sessionId);
			mConnTable.add(user.data(), conn);
		}
		else
		{
			conn->setSessionId(server, sessionId);
		}
		mLock->unlock();
		respCode = AosAmRespCode::eAllowed;
		return true;
	}

	respCode = (AosAmRespCode::E)resp->getRespCode();
	errmsg = resp->getErrmsg();

	return false;
}


bool	
AosAmClient::userBasedAccess(const OmnString &app, 
						const OmnString &user, 
						const OmnString &opr, 
						const OmnString &rsc, 
						AosAmRespCode::E &respCode,
						OmnString &errmsg)
{
	AosAmServerPtr server = selectServer(user);
	if (!server)
	{
		OmnAlarm << "No available server" << enderr;
		return false;
	}

	AosAmClientConnInfoPtr conn;
	u32 sessionId = 0;
	mLock->lock();
	if (!mConnTable.get(user.data(), conn, false))
	{
		// 
		conn = OmnNew AosAmClientConnInfo();
		mConnTable.add(user.data(), conn);
		conn->setSessionId(server, 0);
	}
	else
	{
		sessionId = conn->getSessionId(server);
	}
	mLock->unlock();

	// 
	// It sends an Access Request to one of the servers. 
	// 
	AosAmMsg req(500);
	u16 transId = (u16)getTransId();
	AosAmTransMutexCondVarPtr transSigPtr = OmnNew AosAmTransMutexCondVar(transId);

	mMsgQueueLock->lock();
	mMsgQueue.append(transSigPtr);
	mMsgQueueLock->unlock();

	req.setTransId(transId);
	req.setMsgId(AosAmMsgId::eUserBasedAccess);
	req.setSessionId(sessionId);
	aos_assert_r(req.addTag(AosAmTagId::eApp, app), false);
	aos_assert_r(req.addTag(AosAmTagId::eUser, user), false);
	aos_assert_r(req.addTag(AosAmTagId::eOpr, opr), false);
	aos_assert_r(req.addTag(AosAmTagId::eRsc, rsc), false);

	if (!sendMsg(req, server))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not send the request.";
		OmnAlarm << "Failed to send the request" << enderr;
		return false;
	}

	// 
	// Load response message of the same transaction Id
	// 
	OmnConnBuffPtr buffPtr;
	AosAmMsgPtr resp;
	if(!receiveResp(resp, transSigPtr))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not received response.";
		return false;
	}

	resp->parse();
	if (resp->isRespPositive())
	{
		respCode = AosAmRespCode::eAllowed;
		return true;
	}

	respCode = (AosAmRespCode::E)resp->getRespCode();
	errmsg = resp->getErrmsg();

	return false;
}


bool 	
AosAmClient::userBasedAccess(const OmnString &user,
				  const OmnIpAddr &addr, 
				  const u16 port, 
				  AosAmRespCode::E &code, 
				  OmnString &errmsg)
{
	AosAmServerPtr server = selectServer(user);
	if (!server)
	{
		OmnAlarm << "No available server" << enderr;
		return false;
	}

	AosAmClientConnInfoPtr conn;
	u32 sessionId = 0;
	AosAmRespCode::E respCode;
	mLock->lock();
	if (!mConnTable.get(user.data(), conn, false))
	{
		// 
		conn = OmnNew AosAmClientConnInfo();
		mConnTable.add(user.data(), conn);
		conn->setSessionId(server, 0);
	}
	else
	{
		sessionId = conn->getSessionId(server);
	}
	mLock->unlock();

	// 
	// It sends an Access Request to one of the servers. 
	// 
	AosAmMsg req(500);
	u16 transId = (u16)getTransId();
	AosAmTransMutexCondVarPtr transSigPtr = OmnNew AosAmTransMutexCondVar(transId);

	mMsgQueueLock->lock();
	mMsgQueue.append(transSigPtr);
	mMsgQueueLock->unlock();

	req.setTransId(transId);
	req.setMsgId(AosAmMsgId::eUserBasedAccessL4);
	req.setSessionId(sessionId);
	aos_assert_r(req.addTag(AosAmTagId::eUser, user), false);
	aos_assert_r(req.addTag(AosAmTagId::eIp, (u32)addr.getIPv4()), false);
	aos_assert_r(req.addTag(AosAmTagId::ePort, port), false);

	if (!sendMsg(req, server))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not send the request.";
		OmnAlarm << "Failed to send the request" << enderr;
		return false;
	}

	// 
	// Load response message of the same transaction Id
	// 
	OmnConnBuffPtr buffPtr;
	AosAmMsgPtr resp;
	if(!receiveResp(resp, transSigPtr))
	{
		respCode = (AosAmRespCode::eUnknown);
		errmsg = "Not received response.";
		return false;
	}

	resp->parse();
	if (resp->isRespPositive())
	{
		respCode = AosAmRespCode::eAllowed;
		return true;
	}

	respCode = (AosAmRespCode::E)resp->getRespCode();
	errmsg = resp->getErrmsg();

	return false;
}


bool	
AosAmClient::userBasedAccess(const OmnString &user, 
				  const OmnIpAddr &addr, 
				  const u16 port, 
				  AosAmApiCallback callback)
{
	return false;
}


bool
AosAmClient::sendMsg(const AosAmMsg &msg, AosAmServerPtr &server) 
{

	return server->sendMsg(msg.getData(), msg.getDataLength());
}


AosAmServerPtr
AosAmClient::selectServer(const OmnString &user)
{
	mLock->lock();
	u32 numServers = mServers.entries();
	if (numServers <= 0)
	{
		OmnAlarm << "No server available" << enderr;
		mLock->unlock();
		return 0;
	}

	if (numServers == 1)
	{
		mLock->unlock();
		return mServers[0];
	}

	AosAmServerPtr server;
	AosAmClientConnInfoPtr conn;
	switch (mSlbMethod)
	{
	case eUserPersistent:
		 if (!mConnTable.get(user.data(), conn, false))
		 {
			conn = OmnNew AosAmClientConnInfo();
			mConnTable.add(user.data(), conn);
		 }

		 server = conn->getServer();
		 if (server)
		 {
		 	mLock->unlock();
			return server;
		 }

		 // 
		 // Otherwise, select one from the list using
		 // the Round Robin method
		 //
	     mServerIdx++;
		 if (mServerIdx >= numServers) mServerIdx = 0;
		 server = mServers[mServerIdx];
		 conn->setSessionId(server, 0);
		 mLock->unlock();
		 return server;

	case eRoundRobin:
	     mServerIdx++;
		 if (mServerIdx >= numServers) mServerIdx = 0;
		 server = mServers[mServerIdx];
		 mLock->unlock();
		 return server;

	case eFirstAvailable:
		 for (u32 i=0; i<numServers; i++)
		 {
		 	if (mServers[i]->isGood())
			{
				server = mServers[i];
				mLock->unlock();
				return server;
			}
		 }
		 mLock->unlock();
		 return 0;

	case eLeastBusy:
		{
		 	u32 useCount = 0;
			u32 idx = 0;
			for (u32 i=0; i<numServers; i++)
			{
				if (mServers[i]->isGood() && mServers[i]->getUseCount() > useCount)
				{
					idx = i;
					useCount = mServers[i]->getUseCount();
				}
			}

			server = mServers[idx];
			mLock->unlock();
			return server;
		}

	default:
		OmnAlarm << "Unrecognized Load Balance Method: " << mSlbMethod
				 << enderr;
		mLock->unlock();
		return mServers[0];
	}

	// 
	// Should never come to this point
	//
	aos_should_never_come_here;
	server = mServers[0];
	mLock->unlock();
	return server;
}


bool
AosAmClient::addServer(const OmnIpAddr &addr, 
					   const u16 port, 
					   const u32 numPorts, 
					   const OmnString &name)
{
	int i = 0;
	mLock->lock();
OmnAlarm << addr.toString() << ":" << port << ":" << numPorts << ":" << name << enderr;
	if (mServers.entries() > eMaxServers)
	{
		OmnAlarm << "Too many servers" << enderr;
		mLock->unlock();
		return false;
	}
	AosAmServerPtr server = OmnNew AosAmServer(addr, port, numPorts, name);

OmnAlarm << addr.toString() << ":" << port << ":" << numPorts << ":" << name << enderr;
	// look up the same server in servers list, 
	// If found return true
	for(i = 0; i < mServers.entries(); i++)
	{
		if((*(mServers[i])) == (*server))
		{
			mLock->unlock();
			return true;
		}
	}
	mServers.append(server);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool
AosAmClient::start()
{
	int theSock;
	bool allGood = true;
	FD_ZERO(&mReadFds);
	mLock->lock();
	for (int i=0; i<mServers.entries(); i++)
	{
		if (!mServers[i]->connect())
		{
			OmnAlarm << "Failed to connect to server: " 
				<< mServers[i]->toString() << enderr;
			allGood = false;
		}
		theSock = mServers[i]->getSock();
		if(theSock > 0)
		{
		    FD_SET(theSock, &mReadFds);
		    if(mFdscnt <= theSock)
		    {
		    	mFdscnt = theSock + 1;
		    }
		}
	}
	mLock->unlock();

	// Start the thread threadFunc
	if(!mThread)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "AmThread", 0, false, true, __FILE__, __LINE__);
	}
	mThread->start();

	return allGood;
}


bool
AosAmClient::stop()
{
	mLock->lock();
	for (int i=0; i<mServers.entries(); i++)
	{
		mServers[i]->stop();
	}
	mLock->unlock();

	// Stop the thread threadFunc
	mThread->stop();
	while(1)
	{
		if(mThread->isStopped())
		{
			break;
		}
		else
		{
			sleep(1);
		}
	}

	return true;
}


bool
AosAmClient::isGood() const
{
	for (int i=0; i<mServers.entries(); i++)
	{
		if (mServers[i]->isGood())
		{
			return true;
		}
	}

	return false;
}


bool 
AosAmClient::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	AosAmServerPtr serverConn;
	OmnVList<OmnConnBuffPtr> buffQueue;
	int i, retVal;
	fd_set workingFds;
	OmnConnBuffPtr buff;
    while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (0 >= mServers.entries())
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		workingFds = mReadFds;
		mLock->unlock();

		//
		// Process detecting 
		//
		// 1. Check anybody have anything to tell me
		timeval tval;
		tval.tv_sec = mTimer;
		tval.tv_usec = 0;
		retVal = OmnSocketSelect(mFdscnt, &workingFds, 0, 0, &tval);
		if(retVal == 0)
		{
			continue;
		}
		if(retVal < 0)
		{
            switch (OmnGetErrno())
            {
                case EINTR:
                case EINVAL:
					continue;

                case EBADF:
					OmnAlarm << "Bad connections found!" << enderr;
					checkConns();
					continue;

                default:
					//
					// It failed the selection. This should never happen.
					//
					OmnAlarm << "Something is wrong in select: " 
						<< OmnGetErrno() << enderr;
					return false; 
            }
			mLock->lock();
			if(mServers.entries() <= 0)
			{
				mLock->unlock();
				return false;
			}
			mLock->unlock();
		}

		//
		// Process receiving 
		//
		// 2.1 Pick up the server connection
		// 2.2 Recieve the message
		// 2.3 Push the buffer obj to the Queue
		// 
		mLock->lock();
		for(i = 0; i < mServers.entries(); i++)
		{
			// 2.1 Pick up the server connection
			serverConn = mServers[i];
			if(serverConn && FD_ISSET(serverConn->getSock(), &workingFds) && serverConn->isGood())
			{
				// 2.2 Recieve the message
				if(true == serverConn->receiveResp(buff))
				{
					aos_assert_r(buff, false);
					buffQueue.append(buff);
				}
				else
				{
					// according to the TcpCltGrp.cpp 
					// close it directly... 
					// Is it right?
					FD_CLR(serverConn->getSock(), &mReadFds);
					mServers.remove(serverConn);
					serverConn->stop();
					if(mServers.entries() <= 0)
					{
						mLock->unlock();
						return false;
					}
				}
				buff = 0;
				FD_CLR(serverConn->getSock(), &workingFds);
			}
			serverConn = 0;
		}
		mLock->unlock();

		//
		// Process distribution 
		//
		// 3 Push the message obj to the VList
		// 4 Send signal
		// 5 goto step 1
		// 
		mMsgQueueLock->lock();
		while(buffQueue.entries() > 0)
		{
			buff = buffQueue.popFront();
			aos_assert_r(buff, false);
			// 3 Push the message obj to the VList
			AosAmMsgPtr msg = OmnNew AosAmMsg(buff);
			for(i = 0; i < mMsgQueue.entries(); i++)
			{
				// if this message not found transId, just throw away
				if(msg->getTransId() == mMsgQueue[i]->mTransId)
				{
					// 4 Send signal
					mMsgQueue[i]->mLock->lock();
					mMsgQueue[i]->mMsg = msg;
					mMsgQueue[i]->mCondVar->signal();
					mMsgQueue[i]->mLock->unlock();
					break;
				}
			}
			buff = 0;
		}
		mMsgQueueLock->unlock();
		// 5 goto step 1
	}

	return true;
}

bool	
AosAmClient::userBasedAccess(const OmnString &app, 
						const OmnString &user, 
						const OmnString &opr, 
						const OmnString &rsc, 
						AosAmApiCallback callback)
{
	// selectServer don't care who is user, if mSlbMethod is eRoundRobin 
	AosAmServerPtr server = selectServer(user); 
	if (!server)
	{
		OmnAlarm << "No server is available" << enderr;
		return false;
	}

	mLock->lock();
	AosAmClientConnInfoPtr conn;
	if (!mConnTable.get(user.data(), conn, false))
	{
		// 
		// The user has not logged in yet.
		//
		OmnAlarm << "User has not logged in: " << user << enderr;
		mLock->unlock();
		return false;
	}
	mLock->unlock();
	u32 sessionId = conn->getSessionId(server);

	AosAmClientPtr client(this, false);
	AosApiUserBasedPtr acc = OmnNew AosApiUserBased(
		app, user, opr, rsc, getTransId(), sessionId, 
		mTimer, server, client, callback);
	OmnThrdShellProcPtr proc = OmnNew AosAmThread(acc);
	OmnThreadShellMgr::getSelf()->proc(proc);
	return true;
}


//bool
//AosAmClient::msgRcved(const OmnConnBuffPtr &buff, const u16 transId)
//{
//	mMsgQueue->msgRcved(buff, transId);
//	return true;
//}


// processing blocking call response
bool 
AosAmClient::receiveResp(AosAmMsgPtr &msg, const AosAmTransMutexCondVarPtr &transSigPtr)
{
	bool isTimeOut;

	transSigPtr->mLock->lock();
	transSigPtr->mCondVar->timedWait(transSigPtr->mLock, isTimeOut, mTimer);
	if(isTimeOut)
	{
		// not recieved the message, it's time out. 
		msg = 0;
	}
	else
	{
		// recieved the message, it's not time out. 
		aos_assert_r(transSigPtr->mMsg, false);
		msg = transSigPtr->mMsg;
	}
	transSigPtr->mLock->unlock();

	mMsgQueueLock->lock();
	// remove the transId from mMsgQueue
	mMsgQueue.remove(transSigPtr);
	mMsgQueueLock->unlock();

	return (!isTimeOut);
}




void
AosAmClient::checkConns()
{
    //
    // This function is called when a bad file descriptor is found.
    // All connections are saved in mClients. This function simply
    // go over all the connections in mClients. If a connection is
    // bad, it removes it.
    //
	int theSock;
	bool bChanged = false;

	mLock->lock();
	int nSize = mServers.entries();
	for(int i = 0; i < nSize; i ++)
	{
		if(!mServers[i]->isGood())
		{
            OmnTrace << "Bad connection found: "
				<< mServers[i]->getSock() << endl;
            FD_CLR(mServers[i]->getSock(), &mReadFds);
            bChanged = true;
            mServers.remove(i);
            nSize --;
		}
	}
	if(bChanged)
	{
		mFdscnt = 0;
		nSize = mServers.entries();
		for (int i=0; i < nSize; i++)
		{
			theSock = mServers[i]->getSock();
			if(theSock > 0)
			{
			    FD_SET(theSock, &mReadFds);
			    if(mFdscnt <= theSock)
			    {
			    	mFdscnt = theSock + 1;
			    }
			}
		}
	}
	mLock->unlock();
}
