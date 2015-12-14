////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServerGrp.cpp
// Description:
//	This class is copied from OmnTcpServerGrp. The differences are:
//		1. Added a thread to accept new connections only
//		2. All connections are now handled by OmnTcpCltGrp.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpServerGrp.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/IPv6.h"
#include "Porting/Select.h"
#include "Porting/SockLen.h"
#include "Porting/BindErr.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/Sleep.h"
#include "Tracer/Tracer.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/TcpCltGrp.h"
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/TcpCltGrpListener.h"



OmnTcpServerGrp::OmnTcpServerGrp(AosAddrDef &addrDef, 
			AosPortDef &ports,
			const OmnString &name,
			const int maxConns, 
			const AosTcpLengthType length_type)
:
OmnTcp(addrDef, ports, name, length_type),
mLock(OmnNew OmnMutex()),
mNumConns(0),
mMaxAllowedConns(maxConns),
mIsReading(false)
{
}


OmnTcpServerGrp::OmnTcpServerGrp(const OmnIpAddr &localIpAddr,
						   const int localPort,
						   const int localNumPorts,
						   const OmnString &name,
						   const int maxConns,
						   const AosTcpLengthType lt)
:
OmnTcp(localIpAddr, localPort, localNumPorts, name, lt),
mLock(OmnNew OmnMutex()),
mNumConns(0),
mMaxAllowedConns(maxConns),
mIsReading(false)
{
    OmnTraceComm << "OmnTcpServerGrp on port: " << mLocalCrtPort << endl;
}


OmnTcpServerGrp::~OmnTcpServerGrp()
{
    OmnTrace << "OmnTcpServerGrp deleted: " << this << endl;

	closeConn();
	mNumConns = 0;
    mGroups.clear();

}


bool
OmnTcpServerGrp::addConn(const OmnTcpClientPtr &client)
{
	//
	// When new connection is detected and accepted successfully,
	// an instance of OmnTcpClient is created. This function
	// is called to add the connection to mGroups. It will not
	// check whether the connection is already there to make 
	// the implementation more efficient. The caller must make
	// sure no connection is added multiple times.
	//

	client->setLengthType(mLengthType);
	// int theSock = client->getSock();
	// OmnTrace << "To add connection: " << theSock 
	// 	<< " mNumConns = " << mNumConns
	// 	<< ". " << client.getPtr() << endl;

	mLock->lock();
	mGroups.reset();
	while (mGroups.hasMore())
	{
		if ((mGroups.crtValue())->addConn(client))
		{
			// 
			// Connection added. 
			//
			mNumConns++;
			mLock->unlock();
			return true;
		}

		mGroups.next();
	}

	// 
	// All groups are full. Check whether more connections can be 
	// added.
	//
	if (mNumConns >= mMaxAllowedConns)
	{
		OmnAlarm << "Too many connections added to the server: "
			<< mNumConns << ". Maximum allowed: " 
			<< mMaxAllowedConns << enderr;
		return false;
	}

	// 
	// Create a new group
	//
	OmnTcpCltGrpPtr group = OmnNew OmnTcpCltGrp();
	if (!group)
	{
		mLock->unlock();
		OmnAlarm << "Failed to create group" << enderr;
		return false;
	}

	OmnTcpServerGrpPtr thisPtr(this, false);
	group->setServer(thisPtr);
	if (group->addConn(client))
	{
		// 
		// Connection added. 
		//
		mNumConns++;
		mGroups.append(group);
		OmnTrace << "Number of groups: " << mGroups.entries() << endl;
		group->setListener(mListener);
		if (mIsReading)
		{
			group->startReading();
		}
		mLock->unlock();
		return true;
	}
	else
	{
		OmnAlarm << "Failed to add client" << enderr;
		return false;
	}
}


bool
OmnTcpServerGrp::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter TcpServerGrp::threadFunc." << endl;

	OmnTcpClientPtr tcp;
	fd_set readFds; 		  
    FD_ZERO(&readFds);

	OmnConnBuffPtr buff;
	int ret = 0;
    while ( state == OmnThrdStatus::eActive )
    {
		if (mSock > 0)
		{
			FD_SET(mSock, &readFds); 
		}
		else
		{
			OmnSleep(1);
			OmnString errmsg;
			if (!connect(errmsg))
			{
				OmnAlarm << "Failed to connect: " << errmsg << enderr;
				mSock = -1;
			}
			continue;
		}

		if (!OmnSocketSelect(mSock+1, &readFds, 0, 0, 0))
        {
			OmnAlarm << "Failed to select" << enderr;

            switch (OmnGetErrno())
            {
                case EINTR:
                case EINVAL:
                     continue;

                case EBADF:
					 OmnAlarm << "Bad connections found!" << enderr;
                     continue;

                default:
					 //
					 // It failed the selection. This should never happen.
					 //
					 OmnAlarm << "Something is wrong in select: " 
						<< OmnGetStrError(OmnErrType::eSelect) << enderr;
                     return false; 
            }
        }

		//
        // This means the select is successful. Check if new connection
		// is present.
		//
        ret = checkEvent(&readFds);
		if(ret == 0)
        {
			//
			// It means that some events occured on mSock, which means 
			// that new connections arrived. 
			//
			OmnTcpClientPtr newConn = acceptNewConn();

			if (!newConn)
			{
				OmnAlarm << "NewConn is null: " << mSock << enderr;
				continue;
			}

			
			// 
			// Notify the listener
			//
			if (mListener)
			{
				mListener->newConn(newConn);
			}

			//
			// New connection has been accepted. 
			// Add the connection and go back to select again.
			//
			if (!newConn)
			{
				OmnAlarm << "NewConn is null: " << mSock << enderr;
				continue;
			}

            // OmnTrace << "New connection accepted: "
            //        << newConn->getSock() << endl;
            addConn(newConn);
			
			//
			// Go back to the loop until all connection requests are processed.
			//
            continue;
        }
		else if(ret == -1)
		{
			// connection is broken
			continue;
		}
	}

	return true;
}


void
OmnTcpServerGrp::checkConns()
{
    //
    // This function is called when a bad file descriptor is found.
    // All connections are saved in mConnections. This function simply
    // go over all the connections in mConnections. If a connection is
    // bad, it removes it.
    //

	mLock->lock();
	mGroups.reset();
    while (mGroups.hasMore())
    {
		(mGroups.crtValue())->checkConns();
		mGroups.next();
    }
	mLock->unlock();
}


OmnRslt
OmnTcpServerGrp::connect(OmnString &err)
{
	//
	// This function is used to create the server sock and make connection.
	// If connection failed, it will try until either the connection is
	// established or timer "timerValue" expires.
	//

	if (OmnConn::isConnGood(mSock))
	{
		return true;
	}

	if (mSock > 0)
	{
		OmnCloseSocket(__FILE__, __LINE__, mSock);
		mSock = -1;
	}

	struct sockaddr_in  serv_addr;
	// mSock = ::socket( AF_INET, SOCK_STREAM, 0 );
	mSock = OmnCreateTcpSocket(__FILE__, __LINE__);
	if (mSock <=0)
	{
		err = "Can't open sock for TCP Server: ";
		err << mName
			<< ". Error no.:"
			<< OmnGetErrno();
		return OmnAlarm << err << enderr;
	}

	bool bindSuccess = false;
	for (int i=0; i<mLocalNumPorts; i++)
	{
		//
		// To bind the sock to mLocalCrtPort
		//
		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		//serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		serv_addr.sin_addr.s_addr = mLocalIpAddr.getIPv4();
		serv_addr.sin_port = htons(mLocalCrtPort);
		int on=1;
		setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(mSock, (struct sockaddr *) &serv_addr, 
					sizeof(serv_addr)) != 0)
		{
			// 
			// Failed the binding. If local port is 0, it must be
			// the case that the IP address is not local. Abort.
			//
			if (mLocalCrtPort == 0)
			{
				err = "Failed binding. Local address invalid: ";
				err << mLocalIpAddr.toString();
				return OmnWarn << OmnErrId::eWarnCommError << err << enderr;
			}

			//
			// Try the next port
			//
			mLocalCrtPort++;
			if (mLocalCrtPort >= mLocalStartPort + mLocalNumPorts)
			{
				mLocalCrtPort = mLocalStartPort;
				continue;
			}
		}
		else
		{
			//
			// Note that if mLocalCrtPort is 0, it means that we want 
			// the system to assign a port instead of us requesting 
			// the use of a specific port.
			// In this case, we need to retrieve the port number.
			//
			bindSuccess = true;
			if (mLocalCrtPort == 0)
			{
				OmnSockLen len = sizeof(serv_addr); 
				if(getsockname(mSock, 
					(struct sockaddr *) &serv_addr, &len) != 0)
				{
					OmnAlarm << "Getsockname failed. Errno = " 
						<< OmnGetErrno() << enderr;
				}
				else
				{
					mLocalCrtPort = serv_addr.sin_port;
				}
			} 
			break;
		}
	}

	if (!bindSuccess)
	{
		// 
		// Failed binding. 
		//
		err = "Can't bind local addresss port=";
		err	<< mLocalIpAddr.toString() << ":" << mLocalCrtPort 
			<< ". Errno = " 
			<< strerror(OmnGetErrno());

		return OmnAlarm << err << enderr;
	}

	//
	// Listen to the newly created port. 
	//
	struct linger ling;
	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt(mSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
	setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	if (listen(mSock, eAcceptQueueSize) < 0)
	{
		err = "Listen failed. Errno: ";
		err << OmnGetErrno();
		return OmnAlarm << err << enderr;
	}
	
	OmnTrace << "Listen success on: " << mSock 
		<< " Accept Queue Size: " << eAcceptQueueSize << endl;

	//
	// At this point, the connection has been established successfully.
	//
	return true;
}


OmnTcpClientPtr
OmnTcpServerGrp::acceptNewConn()
{
    // 
    // This function is called to accept a new connection.
	// An instance of OmnTcpClient will be created for
	// this connection.
	// 
	// If successful, it returns the OmnTcpClient. If failed, 
	// it returns 0.
    //

    struct sockaddr_in cli_addr;
    OmnSockLen clilen = (OmnSockLen)sizeof(cli_addr);
    memset((char *) &cli_addr, 0, sizeof(cli_addr));

    int newSock = ::accept(mSock, (struct sockaddr *) &cli_addr, &clilen);

    if (newSock < 0)
    {
		OmnAlarm << OmnErrId::eSocketAcceptError
			<< "mSock = " << mSock << ". "
			<< "Accept error err=" 
			<< OmnGetStrError(OmnErrType::eAccept) << enderr;
		return 0;
    }

    OmnIpAddr remoteIpAddr = OmnRetrieveIpAddr(cli_addr);
	int remotePort = ntohs(cli_addr.sin_port);

	OmnTraceComm << "Accepted new sock: " << newSock 
		<< " Remote: " << remoteIpAddr.toString() 
		<< ":" << remotePort << endl;
	OmnTcpClientPtr theTcp = OmnNew OmnTcpClient(
			OmnString("TcpServerGrp:") << mName << ":Client", 
		remoteIpAddr, remotePort, 1, mLengthType);
    theTcp->setSock(newSock);

	OmnTraceComm << "New sock set: " << theTcp->getSock() << endl;
    return theTcp;
}


OmnString
OmnTcpServerGrp::getConnInfo() const
{
	return OmnString("TCP Server Connection on port = ") <<
			OmnStrUtil::itoa(mLocalCrtPort);
}

	
OmnRslt
OmnTcpServerGrp::removeConn(const OmnTcpClientPtr &client)
{
	//
    // Need to close all member connections
    //
	int sock = client->getSock();
	OmnTrace << "To remove connection: " << sock << endl;
	mLock->lock();
    mGroups.reset();
	OmnTcpClientPtr c;
    while (mGroups.hasMore())
    {
		if ((mGroups.crtValue())->removeConn(client))
		{
			OmnTrace << "Connection: " << sock 
				<< " removed: " << mNumConns << endl;
			mNumConns--;
			mLock->unlock();
			return true;
		}

		mGroups.next();
	}

	mLock->unlock();
	return OmnWarn << "To remove connection: " << sock 
		<< " but not found!" << enderr;
}


void
OmnTcpServerGrp::setListener(const OmnTcpCltGrpListenerPtr &listener)
{
	if (mListener)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mListener = listener;
}


void
OmnTcpServerGrp::startReading()
{
	if (!mListener)
	{
		OmnAlarm << "To start reading a TCP Server but no message processor" 
			<< enderr;
		return;
	}

	mLock->lock();
	mIsReading = true;
	mGroups.reset();
	while (mGroups.hasMore())
	{
		(mGroups.crtValue())->startReading();
		mGroups.next();
	}

	mLock->unlock();

	if (!mNewConnThread)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mNewConnThread = OmnNew OmnThread(thisPtr, "TcpServerGrp", 0, false, true, __FILE__, __LINE__);
	}

	mNewConnThread->start();
}	


void
OmnTcpServerGrp::stopReading()
{
	// 
	// Once it is stopped, it cannot be started again.
	// This function is used mainly for deleting an object. 
	//
	mLock->lock();
	mIsReading = false;
	mGroups.reset();
	while (mGroups.hasMore())
	{
		(mGroups.crtValue())->stopReading();
		mGroups.next();
	}

	mLock->unlock();

	if (mNewConnThread)
	{
		mNewConnThread->stop();
	}
}	


bool 
OmnTcpServerGrp::signal(const int threadLogicId)
{
	mLock->lock();
	mGroups.reset();
	while (mGroups.hasMore())
	{
		(mGroups.crtValue())->signal(threadLogicId);
		mGroups.next();
	}

	return true;
}


bool
OmnTcpServerGrp::checkThread(OmnString &errmsg, const int tid) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
	mLock->lock();
	((OmnTcpServerGrp*)this)->mGroups.reset();
	while (mGroups.hasMore())
	{
		(mGroups.crtValue())->checkThread(errmsg, tid);
		((OmnTcpServerGrp *)this)->mGroups.next();
	}
	
    return true;
}


OmnRslt
OmnTcpServerGrp::closeConn()
{
    if (mSock !=-1)
    {
        OmnTraceComm << "Closing sock: " << mSock << endl;
        OmnCloseSocket(__FILE__, __LINE__, mSock);
        mSock = -1;
        return true;
    }

	//
	// Need to close all member connections
	//
	mLock->lock();
    mGroups.reset();
    while (mGroups.hasMore())
    {
        (mGroups.crtValue())->closeConn();

		mGroups.next();
    }

	mLock->unlock();
	return true;
}


bool
OmnTcpServerGrp::connClosed(
			const OmnTcpCltGrpPtr &group, 
			const OmnTcpClientPtr &client)
{
	mNumConns--;

	if (mListener)
	{
		mListener->connClosed(group, client);
	}

	return true;
}

