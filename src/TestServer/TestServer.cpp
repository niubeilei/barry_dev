////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestServer/TestServer.h"

#include "Alarm/Alarm.h"
#include "Message/MsgFactory.h"
#include "Message/Msg.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "TestServer/TestAgentInfo.h"
#include "TestUtil/SmRPC.h"
#include "TestUtil/SmRPCResp.h"
#include "TestUtil/SmTestAgentReg.h"
#include "TestUtil/SmTestAgentRegResp.h"
#include "TestUtil/RpcCaller.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpCommSvr.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpClient.h"

OmnSingletonImpl(OmnTestServerSingleton,
				 OmnTestServer,
                 OmnTestServerSelf,
                 "OmnTestServer");



OmnTestServer::OmnTestServer()
:
mLock(OmnNew OmnMutex()),
mLocalPort(-1),
mRemotePort(-1)
{
	
}


OmnTestServer::~OmnTestServer()
{
}


bool
OmnTestServer::start()
{
	//
	// Create the communication 
	//
	mComm = OmnNew OmnTcpCommSvr(mLocalAddr, mLocalPort, "TestServer");
	OmnTcpListenerPtr requester(this, false);
	mComm->startReading(requester);

	/*
	// 
	// Create the thread.
	//
    OmnThreadedObjPtr thisPtr(this, false);
    mReadingThread = OmnNew OmnThread(thisPtr, "TestServer", 0, true, true);

	// 
	// Start the thread.
	//
    OmnRslt rslt = mReadingThread->start();
    if (!rslt)
    {
        OmnAlarm << "Failed to start TestServer thread: "
            << rslt.toString() << enderr;
		return false;
	}
	*/

	return true;
}


bool
OmnTestServer::stop()
{
//	if (mReadingThread)
//	{
//		mReadingThread->stop();
//	}

	return true;
}


OmnRslt
OmnTestServer::config(const OmnXmlParserPtr &parser)
{
	return true;
}
 

//bool 
//OmnTestServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
//{
//	while (state == OmnThrdStatus::eActive)
//	{
//
//	}
//
//	return true;
//}


//bool 
//OmnTestServer::signal(const int)
//{
//	OmnNotImplementedYet;
//	return true;
//}


//bool 
//OmnTestServer::checkThread(OmnString &errmsg, const int tid) const
//{
//	return mThreadStatus;
//}


void
OmnTestServer::msgRecved(const OmnConnBuffPtr &buff,
						 const OmnTcpClientPtr &client)

{
	OmnMsgPtr msg = OmnMsgFactorySelf->createMsg(buff);
	if (!msg)
	{
		return;
	}

	switch (msg->getMsgId())
	{
	case OmnMsgId::eSmTestAgentReg:
		 procAgentRegistration((OmnSmTestAgentReg*)msg.getPtr(), client);

	default:
		 OmnAlarm << "Unclaimed message received: " 
			 << msg->toString() << enderr;
		 return;
	}

	OmnShouldNeverComeToThisPoint;
	return;
}


bool
OmnTestServer::procAgentRegistration(const OmnSmTestAgentRegPtr &msg, 
									 const OmnTcpClientPtr &client)
{
	// 
	// Each test agent must register with the server before it can be
	// used. Agent registration is the first thing an agent should do
	// after start. This funciton checks whether the agent has been 
	// registered. If not, it adds the agent info into the list. Otherwise,
	// it updates the agent info. 
	//
	OmnString agentName = msg->getAgentName();
	mLock->lock();
	OmnTestAgentInfoPtr agent = getAgent(agentName);
	if (!agent)
	{
		// 
		// The agent is not in the list yet. Create it. 
		// 
		agent = OmnNew OmnTestAgentInfo(agentName, client);
		mAgentList.append(agent);
		mLock->unlock();

		OmnTrace << "Agent: " << agentName << " created: " 
			<< client->toString() << endl;
	}
	else
	{
		// 
		// The agent is already in the list. We need to update the client.
		//
		agent->setConn(client);
		mLock->unlock();
		OmnTrace << "Agent: " << agentName << " reconnected: " 
			<< client->toString() << endl;
	}

	// 
	// Send the response back. 
	// 
	OmnSmTestAgentRegRespPtr resp = OmnNew OmnSmTestAgentRegResp();
	resp->setRespCode(OmnErrId::e200Ok);
	resp->toggleAddrFrom(msg.getPtr());
	client->sendMsgTo(resp.getPtr());
	return true;
}


OmnTestAgentInfoPtr
OmnTestServer::getAgent(const OmnString &name)
{
	OmnTestAgentInfoPtr agent;
	mLock->lock();
	mAgentList.reset();
	while (mAgentList.hasMore())
	{
		agent = mAgentList.next();
		if (agent->getName() == name)
		{
			mLock->unlock();
			return agent;
		}
	}

	mLock->unlock();
	return 0;
}

bool
OmnTestServer::callFunc(const OmnRpcCallerPtr &rpc)
{
	// 
	// It sends the message to the agent. It then stores the 'rpc' in its
	// list. If the agent receives the message and processes the message
	// it should return a response to this server. When the server receives
	// the response, it will put the response back into 'rpc' and then 
	// inform the caller. 
	//
	OmnCheckAReturn(rpc->getReq(), "", false);

	// 
	// Determine the agent. 
	// 
	OmnString agentName = rpc->getAgentName();
	OmnTestAgentInfoPtr agent = getAgent(agentName);
	if (!agent)
	{
		OmnTrace << "Agent: " << agentName << " not found!";
		return false;
	}

	// 
	// Determine whether the agent is connected.
	//
	if (!agent->isConnGood())
	{
		OmnTrace << "Agent is not connected!" << endl;
		return false;
	}

	// 
	// Time to send the message to the agent.
	//
	if (!agent->sendMsg((rpc->getReq()).getPtr()))
	{
		OmnTrace << "Failed to send the message to the agent!" << endl;
		return false;
	}

	// 
	// Sent successfully. Store the 'rpc' into the list.
	//
	mLock->lock();
	mRpcQueue.append(rpc);
	mLock->unlock();

	return true;
}

