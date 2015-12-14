////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServer.cpp
// Description:
//		This is the server side of a TCP connection. On construction
//	it takes a port. Then it creates an instance of TCP connection.
//  After that, it listens on the connection. When something happens,
//  it checks whether it is a connection request. If yes, it accepts
//  the connection, add the connection to its connection list. Then 
//  it checks whether there is anything on any existing connections
//  to read. 
//
//  An instance of OmnTcpServer is used to maintain the server side of
//  a TCP connection. When instantiating, we need to provide a port
//  number. It will make the connection at construction time. If failed
//  to connect, a flag "mIsGood" is set to FALSE.
//
//  In addition to make connection, it also provides a function to 
//  accept new connections.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/TcpServer.h"

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
#include "UtilComm/Ptrs.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpListener.h"
#include "UtilComm/TcpMsgReader.h"
#include "UtilComm/CommTypes.h"
#include "Util1/Wait.h"

#include <sys/un.h>

OmnTcpServer::OmnTcpServer(const OmnIpAddr &localIpAddr,
						   const int localPort,
						   const int localNumPorts,
						   const OmnString &name,
						   const AosTcpLengthType lt)
:
OmnTcp(localIpAddr, localPort, localNumPorts, name, lt),
mLock(OmnNew OmnMutex()),
mFdscnt(0),
mConnFlag(true)
{
    OmnTraceComm << "OmnTcpServer on port: " << mLocalCrtPort << endl;

	//
	// Set the bit string used by select().
	//
    FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);
}

OmnTcpServer::OmnTcpServer(const OmnString &uPath,
						   const OmnString &name,

						   const AosTcpLengthType lt)
:
OmnTcp(uPath, name, lt),
mLock(OmnNew OmnMutex()),
mFdscnt(0),
mConnFlag(true)
{
    OmnTraceComm << "OmnTcpServer on path: " << uPath << endl;

	//
	// Set the bit string used by select().
	//
    FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);
}

OmnTcpServer::~OmnTcpServer()
{
    OmnTrace << "OmnTcpServer deleted: " << this << endl;

    FD_ZERO(&mReadFds);
    FD_ZERO(&mWorkingFds);

	mLock->lock();
	mConnections.clear();
	mLock->unlock();
	

	closeConn();
}


OmnRslt
OmnTcpServer::closeConn(const OmnTcpClientPtr &client)
{
	//
	// To close a connection identified by 'connId'. When closing a connection,
	// two things need to be done. One is to modify
	// the mReadFds and the other is to remove the entry in mConnections.
	//

	//
	// Modify mReadFds
	//
	if(mReader)
	{
		mReader->connClosed(client);
	}

	int theSock = client->getSock();

	OmnTraceComm << "To close connection: " << theSock << endl;

	removeConn(theSock);
	if (theSock <= 0)
	{
		//
		// This should not happen
		//
		OmnAlarm << "To close a TCP Client, but the client has already been closed!"
			<< enderr;
	}
	else
	{
    	OmnTrace << "Close Connection: " << theSock << endl;
		client->closeConn();
		mLock->lock();
       	FD_CLR((size_t)theSock, &mReadFds);
		mLock->unlock();
	}

    return true;
}


OmnTcpClientPtr
OmnTcpServer::getConn(const int sock)
{
	//
	// The server keeps a list of all active connections in 
	// mConnections. This function will look up the list 
	// If it is found, its OmnTcpClient pointer is returned.
	// Otherwise, null is returned;
	//
	OmnTraceComm << "To get conn: " << sock << endl;
	mLock->lock();
	mConnections.reset();
    while (mConnections.hasMore())
    {
        if (mConnections.crtValue()->getSock() == sock)
        {
			OmnTcpClientPtr client = mConnections.crtValue();
			mLock->unlock();
			OmnTraceComm << "Found" << endl;
            return client;
        }

		mConnections.next();
    }

	//
	// Not found
	//
	mLock->unlock();
	OmnTraceComm << "Not found" << endl;
    return 0;
}


bool
OmnTcpServer::getConnEvent(OmnTcpClientPtr &client)
{
	//
	// It assumes that events have been detected and recorded on 
	// mWorkingFds. This function searches the entries in mConnections
	// to see which ones have events. The first one that has
	// the event is retrieved. The corresponding flags in mWorkingFds
	// are cleared so that the next time this function is called,
	// it will not be found again. Eventually, all responsible
	// connections will be processed. If false is returned, it means
	// that no one is responsible for the events recorded in _tmpReadfs.
	//
	// If it returns true, 'client' is guaranteed not null. If it returns
	// false, 'client' is set to 0.
	//

	OmnTraceComm << "To get event" << endl;
	mLock->lock();
	mConnections.reset();
	OmnTcpClientPtr c;
	int ret = 0;
    while (mConnections.hasMore())
    {
        c =  mConnections.crtValue();
		
		//
		// Check whether the connection is responsible for the events.
		//
		ret = c->checkEvent(&mWorkingFds);
		if(ret == 0)
        {
            client = c;
			mLock->unlock();

			OmnTraceComm << "Found connection: " << client->getSock() << endl;
            return true;
        }
		else if(ret == -1)
		{
			//
			// This is not a good connection
			//
			OmnWarn << OmnErrId::eInvalidSocketError
				<< "Found a bad connection" 
				<< enderr;
			OmnTrace << "Bad connection removed: " << c.getPtr() << endl;

			mConnections.eraseCrt();
			continue;
		}

		mConnections.next();
    }

	mLock->unlock();

	client = 0;
    return false;
}


bool
OmnTcpServer::addConn(const OmnTcpClientPtr &client)
{
	//
	// When new connection is detected and accepted successfully,
	// an instance of OmnTcpClient is created. This function
	// is called to add the connection to mConnections, to set
	// mReadFds, mFdscnt. It will also check whether the connection
	// is already there. If it is, it is an error. 
	//

	int theSock = client->getSock();
	OmnTraceComm << "To add connection: " << theSock 
		<< ". " << client.getPtr() << endl;

	//
	// Find the index of the element in mConnections that 
	// corresponds to theSock.
	//
    OmnTcpClientPtr c = getClient(theSock);

    if (!c)
    {
		//
		// This means that the connection doesn't exist.
		// Set the flag, add the connection to the connection list
		// and return.
		//
		mLock->lock();
        FD_SET(theSock, &mReadFds); 
		mConnections.append(client);

        if (mFdscnt <= theSock)
        {
            mFdscnt = theSock + 1 ;
        }

		mLock->unlock();
        OmnTraceComm << "AddConnection: " << theSock
               << " successful!" 
			   << ". Total connections: " 
			   << mConnections.entries() << endl;
        return true;
    }

	OmnWarn << OmnErrId::eAddSocketError
		<< "Connection already exist: " 
		<< client->toString() << enderr;
    return false;
}


bool
OmnTcpServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter TcpServer::threadFunc." << endl;

	OmnTcpClientPtr tcp;
	bool timeout;

	OmnConnBuffPtr buff;
	int msgLen = 0;
    while ( state == OmnThrdStatus::eActive )
    {
		buff = 0;
		tcp = 0;

		// 
		// Chen Ding, 07/02/2007
		//
		while (mSock < 0)
		{
			//OmnAlarm << "Server sock becomes negative. Try to reconnect ..." << enderr;

			OmnString errmsg;
			if (!connect(errmsg))
			{
				OmnAlarm << "Failed to reconnect. Will sleep one second and retry ..." << enderr;
				OmnSleep(1);
				continue;
			}

			// 
			// Connection re-connected
			//
			break;
		}

    	if(!waitOnEvent(tcp, timeout))		
		{			
			continue;		
		}

		if (!tcp)	
		{
			continue;
		}

		//
		// Found a valid tcp that has something to read
		//
		// Changed by Chen Ding, 07/02/2007
		// if (!tcp->readFrom1(buff, connBroken, mReader.notNull()))
		//
		// Chen Ding, 11/18/2010
		// if (!tcp->readFrom1(buff, connBroken, isEndOfFile))
		if (!tcp->smartRead(buff))
		{
			// 
			// Either the remote has closed the connection or something
			// wrong with the connection. We will close the connection.
			//
			// if (!connBroken)
			// {
			// 	OmnAlarm << "Reading TCP client error: " 
			// 		<< tcp->toString() 
			// 		<< ". Connection closed!" << enderr;
			// }
			// else
			// {
			// 	OmnTrace << "Remote side closed connection" << endl;
			// }
			closeConn(tcp);
		}
		else
		{
			// Chen Ding, 11/18/2010
			if (!buff || buff->getDataLength() == 0)
			{
				// It did not read anything. 
				closeConn(tcp);
				continue;
			}

			// Successfully read something. 
			if (mListener)
			{
				mListener->msgRecved(buff, tcp);
			}
			else if (mReader)
			{
				msgLen = mReader->nextMsg(tcp->getDataBuff(), tcp);
				if(msgLen)
				{
					tcp->truncateBuff(msgLen);
				}					
			}
			else
			{
				OmnAlarm << OmnErrId::eUnclaimedTcpPacket
					<< "Read a message but no message processor" << enderr;
			}
		}
	}

	return true;
}


bool
OmnTcpServer::waitOnEvent(OmnTcpClientPtr &theTcp,bool &timeout)
{
    //
    // This member function determines whether there is any event on
    // any connection this class manages, or whether there is any
    // new connection request.
    //
    // If there is at least one event, it finds which connId and TCP
    // the event occurs and returns true.
    //
    // Whether there is event or not is indicated by "mWorkingFds", which
    // was set by the most recent call of "select(...)".
	//
    //
    // If there is no event, it goes to a while(1) loop, which first
    // reset "mWorkingFds" to "mReadFds", and then call the "select(...)".
    // Two things can cause the call return. One is there is at least
    // one event on one of the connections and the other is that there
    // is a new connection request.
    //
    // Once returned from "select(...)", it checks new connection first.
    // If there is new connection, it creates a new instance of TCP, and
    // adds the pair (sock, TCP) to mConnections.
    //
    // It then checks whether there is any new events. If no, it goes back
    // to the beginning of the loop. Otherwise, it returns the connId and
    // TCP.
    //

	//
	// It checks whether there is any events on mWorkingFds. This may
	// be the events it received the last time and haven't been processed
	// yet. If it finds, it gets the connId and conn, clear the flag,
	// and then return.
	//
	timeout = false;
    if (getConnEvent(theTcp))
    {
		// 
		// Events are found. So we need to clear the flag and return.
		//
		mLock->lock();
        FD_CLR((size_t)theTcp->getSock(), &mWorkingFds);
		mLock->unlock();
        return true;
    }

	// 
	// This means there is no more event. The following will use the select(...)
	// to wait on new events. 
	//
	// int ret = 0;
    while (1)
    {
		if(mSock <= 0)
		{
			return false;
			// connection broken
		}
		//
		// In this loop, it first go 'select'. When it is waken up,
		// it checks whether the select was successful. If failed,
		// it can be something intermident (EINTR or EINVAL). Or
		// it is possible that the server connection is not okay.
		//
        mWorkingFds = mReadFds;

	    timeval timer;
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		int ret = OmnSocketSelect(mFdscnt, &mWorkingFds, 0, 0, &timer);
		if(ret == 0)
		{
			// timeout
			timeout = true;
			return false;
		}
		else if(ret < 0)
        {
			OmnAlarm << "Failed to select" << enderr;

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
						<< OmnGetStrError(OmnErrType::eSelect) << enderr;
                     return false; 
            }

        }

		//
        // This means the select is successful. Check if new connection
		// is present.
		//
        ret = checkEvent(&mWorkingFds);
		if(ret == 0)
        {
			//
			// It means that some events occured on mSock, which means 
			// that new connections arrived. 
			//
			OmnTraceComm << "New connection detected!" << endl;

			OmnTcpClientPtr newConn = acceptNewConn();

			//
			// New connection has been accepted. 
			// Add the connection and go back to select again.
			//
			if(newConn)
			{
				OmnTraceComm << "New connection accepted: "
				       << newConn->getSock() << endl;
				addConn(newConn);
			}
			//
			// Go back to the loop until all connection requests are processed.
			//
            continue;
        }
		else if(ret == -1)
		{
			// connection is broken
			return false;
		}
		//
		// It comes to this point, it means that there is no more connection
		// requests. Now it needs to check whether there are reading requests.
		//

		OmnTraceComm << "To check reading event" << endl;

        if (getConnEvent(theTcp))
        {
			//
			// It means that there are reading requests.
			// Clear the bit so that the next time, we do not get the same
			// event again.
			//
			mLock->lock();
            FD_CLR((size_t)theTcp->getSock(), &mWorkingFds);
			mLock->unlock();
            return true;
        }
    }

	OmnShouldNeverComeToThisPoint;
    return false;
}


OmnTcpClientPtr
OmnTcpServer::getClient(const int theSock) 
{
	mLock->lock();
    mConnections.reset();
	while (mConnections.hasMore())
	{
		if ((mConnections.crtValue())->getSock() == theSock)
        {
			OmnTcpClientPtr client = mConnections.crtValue();
			mLock->unlock();
            return client;
        }

		mConnections.next();
    }

	mLock->unlock();
    return 0;
}


void
OmnTcpServer::checkConns()
{
    //
    // This function is called when a bad file descriptor is found.
    // All connections are saved in mConnections. This function simply
    // go over all the connections in mConnections. If a connection is
    // bad, it removes it.
    //

	mLock->lock();
	mConnections.reset();
	OmnTcpClientPtr c;
    while (mConnections.hasMore())
    {
		c = mConnections.crtValue();
        if (!c->isConnGood())
        {
            OmnTrace << "Bad connection found: "
				<< (mConnections.crtValue())->getSock() 
				<< ", " << c.getPtr() << endl;

            FD_CLR((size_t)c->getSock(), &mReadFds);
            mConnections.eraseCrt();
        }
		else
		{
			mConnections.next();
		}
    }
	mLock->unlock();
}


OmnRslt
OmnTcpServer::connect(OmnString &err)
{
	//
	// This function is used to create the server sock and make connection.
	// If connection failed, it will try until either the connection is
	// established or timer "timerValue" expires.
	//
	//static int NormalIncrement = 10;
	//static int SmallIncrement = 2;
	//static int LargeWaiting = 15;

	//
	// Currently, we always use -1.
	// Chen Ding, 8/13/2002
	//
	//int timerValue = -1;

	if (OmnConn::isConnGood(mSock))
	{
		return true;
	}

	//bool connected = false;
	//int increment;
	//if (timerValue <= 0 || timerValue >= LargeWaiting)
	//{
	//	increment = NormalIncrement;
	//}
	//else
	//{
	//	increment = SmallIncrement;
	//}

	//int elapse = 0;
	//while (elapse < timerValue || timerValue < 0)
	//{
	//
	// Create the sock
	//
		
	if (mSock > 0)
	{
		OmnCloseSocket(__FILE__, __LINE__, mSock);
		mSock = -1;
	}
	
	if (mSockType == eTypeTcp) 
	{
		mSock = OmnCreateTcpSocket(__FILE__, __LINE__);
	}
	else if (mSockType == eTypeUnix)
	{
		mSock = OmnCreateUnixSocket(__FILE__, __LINE__);
	}

	if (mSock <=0)
	{
		err = "Can't open sock for TCP Server: ";
		err << mName
			<< ". Error no.:"
			<< OmnGetErrno();
		return OmnAlarm << err << enderr;
	}

	bool bindSuccess = false;
	if (mSockType == eTypeTcp)
	{
		struct sockaddr_in  serv_addr;

		for (int i=0; i<mLocalNumPorts; i++)
		{
			//
			// To bind the sock to mLocalCrtPort
			//
			OmnTrace << "To bind: " << mLocalCrtPort << endl;
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
						OmnTrace << "New port = "<<  mLocalCrtPort << endl;
					}
				} 
	
				OmnTrace << "Port to use: " << mLocalCrtPort << endl;
				break;
			}
		}
	}
	else if (mSockType == eTypeUnix)
	{
		struct sockaddr_un serv_addr;
		OmnString cmd = "chmod 777 ";
		cmd << mUnixPath;

		unlink(mUnixPath.getBuffer());

		memset((char *) &serv_addr, 0, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strncpy(serv_addr.sun_path, mUnixPath.getBuffer(), 
					sizeof(serv_addr.sun_path)-1);
		
		if (bind(mSock, (struct sockaddr *) &serv_addr, 
					sizeof(serv_addr)) < 0)
		{
			err = "Failed binding. Local address invalid: ";
			err << mUnixPath;
			return OmnWarn << OmnErrId::eWarnCommError << err << enderr;
		}
		
		//
		// Add the permission so as to let other access it
		//
		system(cmd.getBuffer());
		bindSuccess = true;
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
		
		// Chen Ding, 07/31/2012
		// close(mSock);
		closeConn();
		return OmnAlarm << err << enderr;
	}

	//
	// Listen to the newly created port. 
	//
	OmnTrace << "Begin to listen: " << mSock << endl;
	struct linger ling;
	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt(mSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
	setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	if(listen(mSock, 50) < 0)
	{
		err = "Listen failed. Errno: ";
		err << OmnGetErrno();
		return OmnAlarm << err << enderr;
	}
	else
	{
		OmnTrace << "Listen success on: " << mSock << endl;

		//
		// At this point, the connection has been established successfully.
		// Need to set the bitmap now.
		//
		FD_ZERO(&mReadFds);
		FD_ZERO(&mWorkingFds);
		FD_SET(mSock, &mReadFds);
		mFdscnt = mSock + 1 ;
		return true;
	}
}


OmnTcpClientPtr
OmnTcpServer::acceptNewConn()
{
    // 
    // This function is called to accept a new connection.
	// An instance of OmnTcpClient will be created for
	// this connection.
	// 
	// If successful, it returns the OmnTcpClient. If failed, 
	// it returns 0.
    //

	if(!mConnFlag)	
	{
		return 0;
	}
	
	struct sockaddr_in cli_addr;
	OmnSockLen clilen = (OmnSockLen)sizeof(cli_addr);
	memset((char *) &cli_addr, 0, sizeof(cli_addr));

    int newSock = ::accept(mSock, (struct sockaddr *) &cli_addr, &clilen);

    if (newSock < 0)
    {
		OmnAlarm << OmnErrId::eSocketAcceptError
			<< "Accept error err=" << OmnGetStrError(OmnErrType::eAccept) << enderr;
		return 0;
    }

    OmnIpAddr remoteIpAddr = OmnRetrieveIpAddr(cli_addr);
	int remotePort = ntohs(cli_addr.sin_port);

	OmnTrace << "Accepted new sock: " << newSock 
		<< " Remote: " << remoteIpAddr.toString() << ":" << remotePort << endl;
	OmnTcpClientPtr theTcp = OmnNew OmnTcpClient(
			OmnString("TcpServer:") << mName << ":Client", 
		remoteIpAddr, remotePort, 1, mLengthType);
    theTcp->setSock(newSock);

	if(mReader)
	{
		mReader->connAccepted(theTcp);			
	}

	OmnTrace << "New sock set: " << theTcp->getSock() << endl;
    return theTcp;
}


OmnString
OmnTcpServer::getConnInfo() const
{
	return OmnString("TCP Server Connection on port = ") <<
			OmnStrUtil::itoa(mLocalCrtPort);
}


OmnRslt
OmnTcpServer::closeConn()
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
    mConnections.reset();
    while (mConnections.hasMore())
    {
        (mConnections.crtValue())->closeConn();

		mConnections.next();
    }

	mLock->unlock();
	return true;
}


/*
OmnRslt     
OmnTcpServer::writeTo(const char *data, const int length, const int sock)
{
	OmnTcpClientPtr client = getConn(sock);
	return writeTo(data, length,client);
}
*/

/*
OmnRslt		
OmnTcpServer::writeTo(const char *data, const int length, const OmnTcpClientPtr client)
{
	if (client.isNull())
	{
		return OmnAlarm << "Failed to write: connection is broken!" << enderr;
	}
	
	return client->writeTo(data, length);
}
*/

/*
OmnRslt		
OmnTcpServer::writeTo(OmnString str, const OmnTcpClientPtr client)
{
	return writeTo(str.data(), str.length(), client);
}
*/

OmnRslt
OmnTcpServer::removeConn(const int sock)
{
	//
    // Need to close all member connections
    //
	OmnTraceComm << "To remove connection: " << sock << endl;
	mLock->lock();
    mConnections.reset();
	OmnTcpClientPtr c;
    while (mConnections.hasMore())
    {
		c = mConnections.crtValue();
        if (c->getSock() == sock)
		{
			mConnections.eraseCrt();
			FD_CLR(sock, &mReadFds);
			OmnTrace << "Connection: " << sock 
				<< " removed: " << c.getPtr() << endl;
			mLock->unlock();
			return true;
		}

		mConnections.next();
	}

	mLock->unlock();
	OmnWarn << "To remove connection: " << sock 
		<< " but not found!" << enderr;
    return false;
}


void
OmnTcpServer::setListener(const OmnTcpListenerPtr &listener)
{
	if (mListener)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mListener = listener;
}


void
OmnTcpServer::setReader(const OmnTcpMsgReaderPtr &reader)
{
	if (mReader)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mReader = reader;
}


void
OmnTcpServer::startReading()
{
	if (!mListener && !mReader)
	{
		OmnAlarm << "To start reading a TCP Server but no message processor" 
			<< enderr;
		return;
	}

	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "TcpServer", 0, false, true, __FILE__, __LINE__);
	}

	mThread->start();
}	


// 
// Chen Ding, 02/15/2007
//
void
OmnTcpServer::stopReading()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return;
	}

	mThread->stop();
}


bool 
OmnTcpServer::signal(const int threadLogicId)
{
	return true;
}


bool
OmnTcpServer::checkThread(OmnString &, const int tid) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
    return true;
}


void
OmnTcpServer::readFromNew(OmnConnBuffPtr &buffer, OmnTcpClientPtr &client)
{
	// If it is not connected yet, it raises an alarm and tries to reconnect.
	// If it still cannot connect, it will sleep for a second and reconnect. 
	// Otherwise, it listens on the port. If nothing, it waits.
	// If there is something on the connection, it reads the contents. If it
	// failed to read, it checks whether it is because the other side closed
	// the connection. If yes, it removes the connection. If it read an EOF, 
	// it closes the connection and removes the connection. Otherwise, 
	// it returns the buffer and the TCP connection through which the
	// data was read. 
	//
	bool timeout;
	//bool isEndOfFile;
	while (1)
	{
		// 
		// Chen Ding, 07/02/2007
		//
		while (mSock < 0)
		{
			OmnAlarm << "Server sock becomes negative. Try to reconnect ..." 
				<< enderr;

			OmnString errmsg;
			if (!connect(errmsg))
			{
				OmnAlarm << "Failed to reconnect. Will sleep one second and retry ..." 
					<< enderr;
				OmnSleep(1);
				continue;
			}

			// 
			// Connection re-connected
			//
			break;
		}

    	if(!waitOnEvent(client, timeout))
		{
			continue;
		}

		//
		// So found a connection which has something to be read. 
		// Go read it.
		//
		// Chen Ding, 05/17/2011
		// if (!client->readFrom1(buffer, connClosed, isEndOfFile))
		if (!client->smartRead(buffer))
    	{
			// 
			// The connection is broken. This is not necessarily
			// an error. Simply close it and go back to read again
			//
			// if (connClosed)
			// {
			// 	OmnTrace << "Remote closed the connection" << endl;
			// }
			// else
			// {
				OmnAlarm << "Read failed! Connection " 
					<< " is removed." << enderr;
			// }
			closeConn(client);
		}
		else
		{
			//if (isEndOfFile)
			//{
			//	closeConn(client);
			//}

			//
			// Otherwise, read successful
			//
			return;
		}
	}
}


bool		
OmnTcpServer::getClientByRemoteAddr(const OmnIpAddr addr, const int port, OmnTcpClientPtr & client)
{
	mLock->lock();
	mConnections.reset();
	while (mConnections.hasMore())
	{
		OmnTcpClientPtr curClient = mConnections.crtValue();
		if(curClient && 
		   curClient->getRemoteIpAddr() == addr &&
		   curClient->getRemotePort() == port)
		{
			mLock->unlock();
			client = curClient;
			return true;
		}
		mConnections.next();
	}

	mLock->unlock();

	return false;	
}


bool
OmnTcpServer::postProc()
{
	return closeConn();
}


void		
OmnTcpServer::setConnFlag(const bool connFlag)
{
	mConnFlag = connFlag;
	if(!mConnFlag)
	{
		closeConn();
	}
	else
	{
		OmnString err;
		connect(err);
	}
}


