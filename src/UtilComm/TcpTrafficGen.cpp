////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpTrafficGen.cpp
// Description:
//	This utility serves as a trffic generator that generates
//	TCP traffic. The class may start large number of 
//	concurrent connections. Connections are grouped. Group
//	size is determined by eGroupSize (currently 20). Each
//	group runs its own thread. There is also a maximum 
//	limitation on the number of groups (eMaxThreads, currently
//	50). 
//
//	Example:
//	int test()
//	{
//		AosTcpTrafficGen gen(remoteAddr, remotePort, numPorts,
//        	repeat, concurrentConns, contentLen,
//        	contents, contentLen);
//
//	    gen.start();
//		return 0;
//	}
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpTrafficGen.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Porting/GetTime.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util1/Wait.h"
#include "UtilComm/TrafficGenThread.h"
#include "UtilComm/TcpServer.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpTrafficGenClient.h"
#include "UtilComm/TcpTrafficGenListener.h"
#include "UtilComm/ReturnCode.h"
#include "PacketCheck/PCTestsuite.h"

AosTcpTrafficGen::AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
			const int concurrentConns,
			char c,
			const int contentLen, 
			AosSendBlockType sendBlockType,
			const u32 sendBlockSize, 
			const u32 bytesExpected)
:
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mNumPorts(numPorts),
mRepeat(repeat),
mConcurrentConns(concurrentConns),
mBytesExpected(bytesExpected),
mContents(contentLen, c, true),
mContentLen(contentLen),
mLock(OmnNew OmnMutex()),
mSendBlockType(sendBlockType),
mSendBlockSize(sendBlockSize),
mGeneratorId(genId),
mCheckContent(false),
mInputDataType(eBuffer)
{
	mLock->lock();
	mLocalAddrList.append(localAddr);
	mLock->unlock();
}

// add by xiaoqing 12/08/2005

AosTcpTrafficGen::AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
			const int concurrentConns,
			const char* cStr,
			const int contentLen, 
			AosSendBlockType sendBlockType,
			const u32 sendBlockSize, 
			const u32 bytesExpected)
:
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mNumPorts(numPorts),
mRepeat(repeat),
mConcurrentConns(concurrentConns),
mBytesExpected(bytesExpected),
mContents(cStr,contentLen),
mContentLen(contentLen),
mLock(OmnNew OmnMutex()),
mSendBlockType(sendBlockType),
mSendBlockSize(sendBlockSize),
mGeneratorId(genId),
mCheckContent(false),
mInputDataType(eBuffer)
{
	mLock->lock();
	mLocalAddrList.append(localAddr);
	mLock->unlock();
}

//add by dingxr 11/7/2007
AosTcpTrafficGen::AosTcpTrafficGen(
			const u32 genId, 
			const OmnIpAddr &localAddr, 
			const OmnIpAddr &remoteAddr, 
			const int remotePort, 
			const int numPorts, 
			const int repeat,
            const int concurrentConns,
			AosPCTestsuite* testSuite)
:
mRemoteAddr(remoteAddr),
mRemotePort(remotePort),
mNumPorts(numPorts),
mRepeat(repeat),
mConcurrentConns(concurrentConns),
mBytesExpected(0),
mLock(OmnNew OmnMutex()),
mSendBlockType(eAosSendBlockType_Fixed),
mGeneratorId(genId),
mCheckContent(false),
mTestSuite(testSuite),
mInputDataType(eTestCaseConfigFile)
{
	mLock->lock();
	mLocalAddrList.append(localAddr);
	mLock->unlock();
}

int			
AosTcpTrafficGen::addLocalAddr(const OmnIpAddr &localAddr)
{
	mLock->lock();
	mLocalAddrList.append(localAddr);
	mLocalAddrList.reset();	
	mLock->unlock();
	return true;
}


// 
// TcpTrafficGen maintains a list of TcpTrafficGenThread. Each thread
// is able to handle a number of clients. This function adds a client
// to one of the thread. Note that each thread has a maximum number
// of clients. The function will loop on all the threads to see 
// whether it can add to an existing thread. If not, it checks
// whether it has reached the maximum threads. If yes, it cannot
// add the client. Otherwise, it creates a new thread and adds
// the client to the new thread.
// Chen Ding, 01/10/2007
//
int
AosTcpTrafficGen::addClient(const AosTcpTrafficGenClientPtr &client)
{
	mLock->lock();
	for (int i=0; i<mNumThreads; i++)
	{
		if (mThreads[i]->addClient(client))
		{
			mLock->unlock();
			return 0;
		}
	}

	// 
	// Failed to add the client to the existing thread. Check
	// whether we can add more thread. 
	//
	// Chen Ding, 01/10/2007
	// while (mNumThreads < eMaxThreads)
	if (mNumThreads < eMaxThreads)
	{
		mLock->unlock();
		return aos_alarm("Too many clients: %d, %d", 
			mNumClients, mNumThreads);
	}

	// 
	// Create a new thread
	//
	AosTcpTrafficGenPtr selfPtr(this, false);
	mThreads[mNumThreads] = OmnNew AosTcpTrafficGenThread(
			selfPtr,
			mNumThreads * AosTcpTrafficGen::eGroupSize,
			mRemoteAddr, mRemotePort, mNumPorts, eGroupSize,
			mListener);
	if (!mThreads[mNumThreads])
	{
		mLock->unlock();
		return aos_alarm("Memory failure");
	}

	int ret = mThreads[mNumThreads]->addClient(client);
	if (ret < 0)
	{
		// 
		// Failed to add client
		//
		mLock->unlock();
		return aos_alarm("Failed to add client: %d", ret);
	}
		
	mNumThreads++;
	mLock->unlock();
	return ret;
}
	

bool
AosTcpTrafficGen::start()
{
	// 
	// We will start a number of threads. Each thread will start a number of 
	// TCP clients. Each client will establish the connection, send the 
	// contents, and then close. 
	// Each client will run a number of tries. 
	//
	if(!createClients())
	{
		return false;
	}
	
	mStartSec = OmnTime::getSecTick();

	for (int i=0; i<mNumThreads; i++)
	{
		mThreads[i]->start();
	}

	return true;
}


void
AosTcpTrafficGen::printStatus() const
{
	u64 bytesSent = 0;
	u64 bytesRcvd = 0;
	u32 conns = 0;
	u32 failedConns = 0;

	u32 diff = OmnTime::getSecTick() - mStartSec;
	if (diff == 0) diff = 1;

	for (int i=0; i<mNumThreads; i++)
	{
		bytesSent += mThreads[i]->getBytesSent();
		bytesRcvd += mThreads[i]->getBytesRcvd();
		conns 	  += mThreads[i]->getTotalConns();
		failedConns += mThreads[i]->getFailedConns();
	}

	cout << "==================================================" << endl;

	cout << "Generator ID: " << mGeneratorId 
		<< ": " << OmnGetTime(OmnApp::getLocale()).data() << endl;
	cout << "Start: " << mStartSec 
		<< ", Finish: " << OmnTime::getSecTick()
		<< ". Duration: " << OmnTime::getSecTick() - mStartSec << endl;
	cout << "Bytes sent(M): " << bytesSent/1000000 << ". Rate(KB/s): " << bytesSent/diff/1000 << endl;
	cout << "Bytes rcvd(M): " << bytesRcvd/1000000 << ". Rate(KB/s): " << bytesRcvd/diff/1000 << endl;
	cout << "Total Connections: " << conns 
		<< ". Connections/second: " << conns/diff << endl;
	cout << "Total Failed Connections: " << failedConns << endl;

	for (int i=0; i<mNumThreads; i++)	
	{
		mThreads[i]->printStatus();
	}

	cout << "==================================================" << endl;
}
	

bool        
AosTcpTrafficGen::closeConn(const OmnTcpClientPtr &client)
{
	return false;
}


bool        
AosTcpTrafficGen::restartClient(const OmnTcpClientPtr &client)
{
	return false;
}


bool
AosTcpTrafficGen::checkFinish() const
{
	static u64 lastPrint = 0;

	if (OmnTime::getCrtSec() - lastPrint >= 10)
	{
//		printStatus();
	}

	mLock->lock();
	for (int i=0; i<mNumThreads; i++)
	{
		if (!mThreads[i]->isAllFinished())
		{
			mLock->unlock();

			return false;
		}
	}

	mLock->unlock();

	printStatus();

	// 
	// All finished
	//
	if (mListener)
	{
		// 
		// Construct the list of all clients.
		//
		OmnVList<AosTcpTrafficGenClientPtr> clients;
		for (int i=0; i<mNumThreads; i++)
		{
			mThreads[i]->getClients(clients);
		}

		mListener->trafficGenFinished(clients);
	}

	return true;
}


void		
AosTcpTrafficGen::registerCallback(const AosTcpTrafficGenListenerPtr &callback)
{
	mListener = callback;
}


bool
AosTcpTrafficGen::checkConns()
{
	for (int i=0; i<mNumThreads; i++)
	{
		mThreads[i]->checkConns();
	}

	return true;
}


//	add by xiaoqing 03/11/2006
//	for ocspcrl testing
int 
AosTcpTrafficGen::getRecv()
{
	int bytesRecv = 0;
	for (int i = 0; i < mNumThreads; i++)
	{
		bytesRecv += mThreads[i]->getBytesRcvd();
	}	
	return bytesRecv;
}


//	add by xiaoqing 03/15/2006
//	for ocspcrl testing
int
AosTcpTrafficGen::getFailedConns()
{
	int failedConns = 0;
	for (int i = 0; i < mNumThreads; i++)
    {
        failedConns += mThreads[i]->getFailedConns();
    }
	return failedConns;
}


//	add by xiaoqing 03/15/2006
//	for ocspcrl testing
int
AosTcpTrafficGen::getSuccessConns()
{
	int conns = 0;
	int failedConns = 0;
	for (int i = 0; i < mNumThreads; i++)
	{
        conns     += mThreads[i]->getTotalConns();
        failedConns += mThreads[i]->getFailedConns();
	}
	return(conns - failedConns); // return back successful conns
}


bool		
AosTcpTrafficGen::stop()
{
	for (int i = 0; i < mNumThreads; i++)
	{
        mThreads[i]->exit();
	}
	bool allStopped = false;	
	while(!allStopped)
	{
		OmnWait::getSelf()->wait(0,100000);
		allStopped = true;
		for (int j = 0; j < mNumThreads; j++)
		{
	        if(!mThreads[j]->isExited())
	        {
	        	allStopped = false;
	        	break;
	        }
		}
		
	}
	return true;
}


bool
AosTcpTrafficGen::createClients()
{
	int numConns = 0;
	mNumThreads = 0;
	int grpIndex = 0;
	OmnIpAddr localAddr;

	mLock->lock();

	if(mLocalAddrList.entries() <= 0)
	{
		return false;
	}
	
	mLocalAddrList.reset();
	AosTcpTrafficGenPtr thisPtr(this, false);
	while (numConns < mConcurrentConns && mNumThreads < eMaxThreads)
	{
		// 
		// Determine the group size. mConcurrentConns is the total number of 
		// connections we need to create and 'numConns' is the number of
		// connections we have already created. 
		//
		int gsize = mConcurrentConns - numConns;
		if (gsize > eGroupSize) gsize = eGroupSize;
			
		int idx = grpIndex * AosTcpTrafficGen::eGroupSize;
		mThreads[mNumThreads] = OmnNew AosTcpTrafficGenThread(
			thisPtr, idx,
			mRemoteAddr, mRemotePort, mNumPorts, gsize, 
			mListener);
	
		// 
		// Create clients for the group
		//
		OmnTcpCltGrpPtr group = mThreads[mNumThreads]->getGroup();
		AosTcpTrafficGenClientPtr client;
    	for (int i=0; i<gsize; i++)
    	{
    		// determine localAddr
    		if(!mLocalAddrList.hasMore())
    		{
				mLocalAddrList.reset();
			}
    		localAddr = mLocalAddrList.next();

			
			if(mInputDataType == eBuffer)
			{
			// create client   		
        		client = OmnNew AosTcpTrafficGenClient(
        		   	thisPtr, 
        		   	group, 
        		   	idx+i,
            		localAddr, 
            		mRemoteAddr, 
            		mRemotePort,
            		mRepeat, 
            		mBytesExpected,
            		(char *)mContents.data(), 
					mContentLen, 
					mSendBlockType, 
					mSendBlockSize, 
            		AosTcpTrafficGenClient::eReceiveDriven, 
					i, 
					mListener);
			}
			else
			{
				client = OmnNew AosTcpTrafficGenClient(
        		   	thisPtr, 
        		   	group, 
        		   	idx+i,
            		localAddr, 
            		mRemoteAddr, 
            		mRemotePort,
            		mRepeat, 
					mSendBlockType, 
            		AosTcpTrafficGenClient::eReceiveDriven, 
					i, 
					mListener,
					mTestSuite);	
				
			}		
			client->setCheckContent(mCheckContent);
			int ret = mThreads[mNumThreads]->addClient(client);
			if (ret < 0)
			{
				aos_alarm("Failed to add client: %d", ret);
			}
    	}

		grpIndex++;
		numConns += gsize;
		mNumThreads++;
	}
	
	mLock->unlock();

	return true;
} 

