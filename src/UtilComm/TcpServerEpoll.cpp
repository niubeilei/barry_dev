////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpServerEpoll.cpp
// Description:
//		This is the server side of a TCP connection. On construction
//	it takes a port. Then it creates an instance of TCP connection.
//  After that, it listens on the connection. When something happens,
//  it checks whether it is a connection request. If yes, it accepts
//  the connection, add the connection to its connection list. Then 
//  it checks whether there is anything on any existing connections
//  to read. 
//
//  An instance of OmnTcpServerEpoll is used to maintain the server side of
//  a TCP connection. When instantiating, we need to provide a port
//  number. It will make the connection at construction time. If failed
//  to connect, a flag "mIsGood" is set to FALSE.
//
//  In addition to make connection, it also provides a function to 
//  accept new connections.    
//
// Modification History:
// 07/19/2011	Created by Ice Yu
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/TcpServerEpoll.h"

#include "alarm_c/alarm.h"
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
#include "UtilComm/TcpEpollReader.h"
#include "Util1/Wait.h"

#include <sys/un.h>
#include <fcntl.h>

static OmnMutex sgLock;
const int cEventsLen = 10;
vector<OmnTcpServerEpollPtr>	OmnTcpServerEpoll::smEpollServers;


OmnTcpServerEpoll::OmnTcpServerEpoll(
		const OmnIpAddr &localIpAddr,
		const int localPort,
		const int localNumPorts,
		const OmnString &name,
		const AosTcpLengthType lt,
		const u32 maxConn)
:
OmnTcp(localIpAddr, localPort, localNumPorts, name, lt),
mLock(OmnNew OmnMutex()),
mEpollFd(-1),
mEvents(NULL),
mConns(NULL),
mMaxConns(maxConn),
mIsBlocking(false)
{
	addEpollServer();

	// Chen Ding, 06/06/2012
	if (mMaxConns < 1024) mMaxConns = 1024;
    OmnTraceComm << "OmnTcpServer on port: " << mLocalCrtPort << endl;
	initEpoll();
	
	// Chen Ding, 2013/01/10
	mReader = OmnNew AosTcpEpollReader(this);
}


OmnTcpServerEpoll::OmnTcpServerEpoll(
		AosAddrDef &addrDef,
		AosPortDef &ports,
		const OmnString &name,
		const AosTcpLengthType lt,
		const u32 maxConn)
:
OmnTcp(addrDef, ports, name, lt),
mLock(OmnNew OmnMutex()),
mEpollFd(-1),
mEvents(NULL),
mConns(NULL),
mMaxConns(maxConn),
mIsBlocking(false)
{
	addEpollServer();

	// Chen Ding, 06/06/2012
	if (mMaxConns < 1024) mMaxConns = 1024;
    OmnTraceComm << "OmnTcpServer on port: " << mLocalCrtPort << endl;
	initEpoll();
	mReader = OmnNew AosTcpEpollReader(this);
}


OmnTcpServerEpoll::OmnTcpServerEpoll(
		const OmnString &uPath,
		const OmnString &name,
		const AosTcpLengthType lt, 
		const u32 max_conns)
:
OmnTcp(uPath, name, lt),
mLock(OmnNew OmnMutex()),
mEpollFd(-1),
mEvents(NULL),
mConns(NULL),
mMaxConns(max_conns),
mIsBlocking(false)
{
	addEpollServer();
	// Chen Ding, 06/06/2012
	if (mMaxConns < 1024) mMaxConns = 1024;
    OmnTraceComm << "OmnTcpServerEpoll on path: " << uPath << ":" << this << endl;
	initEpoll();
	mReader = OmnNew AosTcpEpollReader(this);
}


OmnTcpServerEpoll::~OmnTcpServerEpoll()
{
    OmnTrace << "OmnTcpServerEpoll deleted: " << this << endl;
	// close(mEpollFd);
	OmnCloseSocket(__FILE__, __LINE__, mEpollFd);
	mEpollFd = -1;
	OmnDelete[] mEvents;
	OmnDelete[] mConns;
}


bool
OmnTcpServerEpoll::initEpoll()
{
	// Chen Ding, 07/31/2012
	// mEpollFd = epoll_create(mMaxConns);	
	mEpollFd = OmnCreateEpollSocket(__FILE__, __LINE__, mMaxConns);
	if (mEpollFd == -1)
	{
		OmnAlarm << "Epoll init failed" << enderr; 
		return false;
	}
	mEvents = OmnNew epoll_event[cEventsLen];
	mConns = OmnNew OmnTcpClientPtr[mMaxConns];
	memset(mEvents, 0, sizeof(epoll_event)*cEventsLen);
	return true;
}


OmnRslt
OmnTcpServerEpoll::closeConn(const OmnTcpClientPtr &client)
{
	// To close the connection 'client'. When closing a connection,
	// need to remove the entry in mConne.
	//
	int theSock = client->getSock();

	OmnTraceComm << "To close connection: " << theSock << endl;
	if (theSock <= 0)
	{
		return true;
		//OmnAlarm << "To close a TCP Client, but the client has already been closed!"
		//	<< enderr;
		//return false;
	}
	
   	// OmnTrace << "Close Connection: " << theSock << endl;
	if (mListener)
	{
		mListener->connClosed(client);
	}

	aos_assert_r(theSock > 0 && theSock < mMaxConns, 0);
	mConns[theSock] = NULL;
	epoll_event ev;
	bzero(&ev, sizeof(epoll_event));
	bool rslt = epoll_ctl(mEpollFd, EPOLL_CTL_DEL, theSock, &ev);
	client->closeConn();
	//Jozhi 2014-11-05 no one to handle this result, not used
	//aos_assert_r(rslt, 0);
	if (!rslt)
	{
		return false;
	}
    return true;
}


bool
OmnTcpServerEpoll::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter TcpServerEpoll::threadFunc." << endl;

	OmnTcpClientPtr tcp;
	OmnConnBuffPtr buff;
	int nfds;

    while (state == OmnThrdStatus::eActive )
    {
		buff = 0;
		tcp = 0;

		while (mSock < 0)
		{
			OmnString errmsg;
			if (!connect(errmsg))
			{
				OmnAlarm << "Failed to reconnect. Will sleep one second and retry ..." 
					<< enderr;
				OmnSleep(5);
				continue;
			}
		}

		nfds = epoll_wait(mEpollFd, mEvents, cEventsLen, -1);

		if (nfds < 0 || nfds > cEventsLen)
		{
			// OmnAlarm << "Epoll Wait Error " << strerror(errno) << enderr;
			OmnSleep(1);
			continue;
		}

		for (int i = 0; i < nfds; i++)
		{
			//accept event
			if (mEvents[i].data.fd == mSock)
			{
//OmnScreen << "accept event " << mEvents[i].data.fd << endl;
				tcp = acceptNewConn();
				continue;
			}

			// Chen Ding, 2013/01/10
			if (mEvents[i].events & EPOLLIN)
			{
//OmnScreen << "read event " << mEvents[i].data.fd << endl;
				if (mEvents[i].data.fd <= 0 || mEvents[i].data.fd >= mMaxConns)
				{
					OmnAlarm << "Invalid fd: " << i << ":" << mEvents[i].data.fd << enderr;
					continue;
				}

				tcp = mConns[mEvents[i].data.fd];
				if (!tcp)
				{
					OmnScreen << "No connection found: " << i << ":" 
						<< mEvents[i].data.fd << endl;
					continue;
				}

				mReader->addConn(tcp);
			}

			// Chen Ding, 2013/01/10
			/*
			if (mEvents[i].events & EPOLLIN)
			{
				//read evetn
				// Chen Ding, 06/06/2012
				// if (mEvents[i].data.fd <= 0 || mEvents[i].data.fd >= mMaxConns)
				if (mEvents[i].data.fd <= 0)
				{
					OmnAlarm << "Invalid fd: " << i << ":" << mEvents[i].data.fd << enderr;
					continue;
				}

				tcp = mConns[mEvents[i].data.fd];
				if (!tcp)
				{
					OmnScreen << "No connection found: " << i << ":" 
						<< mEvents[i].data.fd << endl;
					continue;
				}

				if (tcp->smartRead(buff) == -1)
				{
					// Either the remote has closed the connection or something
					// wrong with the connection. We will close the connection.
					//
					closeConn(tcp);
				}                                                                
				else
				{
					if (tcp->isReadReady())
					{
						if (mListener)
						{
							mListener->msgRecved(tcp->getDataBuff(), tcp);
						}
						else
						{
							OmnAlarm << OmnErrId::eUnclaimedTcpPacket
								<< "Read a message but no message processor" << enderr;
						}

						tcp->clearBuffedData();
					}
				}
			}
			*/
//			if(mEvents[i].events&EPOLLOUT)
//			{
//				ev.data.fd = mEvents[i].data.fd;
//				ev.events=EPOLLIN|EPOLLET;
//				epoll_ctl(mEpollFd,EPOLL_CTL_MOD,mEvents[i].data.fd,&ev);
//				OmnString str = mConns[mEvents[i].data.fd]->sendmsg;
//				mConns[mEvents[i].data.fd]->smartSend(str.data(),str.length());
//			}
		}

	}
	return true;
}


OmnRslt
OmnTcpServerEpoll::connect(OmnString &err)
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
	
	if (mSockType == eTypeTcp) 
	{
		// mSock = ::socket( AF_INET, SOCK_STREAM, 0 );
		mSock = OmnCreateTcpSocket(__FILE__, __LINE__);
	}
	else if (mSockType == eTypeUnix)
	{
		// mSock = ::socket( AF_UNIX, SOCK_STREAM, 0 );
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

	epoll_event ev;

	// Chen Ding, 2013/01/10
	//ev.events = EPOLLIN | EPOLLET;	
	ev.events = EPOLLIN;
	ev.data.fd = mSock;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mSock, &ev) < 0)
	{
		OmnAlarm << "Failed adding port" << enderr;
		return false;
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
			if (bind(mSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) != 0)
			{
				// Failed the binding. If local port is 0, it must be
				// the case that the IP address is not local. Abort.
				if (mLocalCrtPort == 0)
				{
					err = "Failed binding. Local address invalid: ";
					err << mLocalIpAddr.toString();
					return OmnWarn << OmnErrId::eWarnCommError << err << enderr;
				}

				// Try the next port
				mLocalCrtPort++;
				if (mLocalCrtPort >= mLocalStartPort + mLocalNumPorts)
				{
					mLocalCrtPort = mLocalStartPort;
					continue;
				}
			}
			else
			{
				// Note that if mLocalCrtPort is 0, it means that we want 
				// the system to assign a port instead of us requesting 
				// the use of a specific port.
				// In this case, we need to retrieve the port number.
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
	
				//OmnTrace << "Port to use: " << mLocalCrtPort << endl;
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
		// Failed binding. 
		err = "Can't bind local addresss port=";
		err	<< mLocalIpAddr.toString() << ":" << mLocalCrtPort 
			<< ". Errno = " 
			<< strerror(OmnGetErrno());
		
		// Chen Ding, 07/31/2012
		// close(mSock);
		closeConn();
		return OmnAlarm << err << enderr;
	}


	// Listen to the newly created port. 
	//OmnTrace << "Begin to listen: " << mSock << endl;
	struct linger ling;
	ling.l_onoff = 0;
	ling.l_linger = 0;
	setsockopt(mSock, SOL_SOCKET, SO_LINGER, (char *) &ling, sizeof(ling));
	setsockopt(mSock, SOL_SOCKET, SO_REUSEADDR, (char *) NULL, 0);

	if(listen(mSock, 100) < 0)
	{
		err = "Listen failed. Errno: ";
		err << OmnGetErrno();
		return OmnAlarm << err << enderr;
	}
	
	//OmnTrace << "Listen success on: " << mSock << endl;
	// At this point, the connection has been established successfully.
	// Need to set the bitmap now.
	return true;
}


OmnTcpClientPtr
OmnTcpServerEpoll::acceptNewConn()
{
    // This function is called to accept a new connection.
	// An instance of OmnTcpClient will be created for
	// this connection.
	// 
	// If successful, it returns the OmnTcpClient. If failed, 
	// it returns 0.

	struct sockaddr_in cli_addr;
	OmnSockLen clilen = (OmnSockLen)sizeof(cli_addr);
	memset((char *) &cli_addr, 0, sizeof(cli_addr));

    // int newSock = ::accept(mSock, (struct sockaddr *) &cli_addr, &clilen);
    int newSock = OmnAcceptSocket(__FILE__, __LINE__, mSock, (struct sockaddr *) &cli_addr, &clilen);

	//int opts;
	//opts=fcntl(newSock,F_GETFL);
	//opts = opts|O_NONBLOCK;
	//fcntl(newSock,F_SETFL,opts);

    if (newSock < 0)
    {
		OmnAlarm << OmnErrId::eSocketAcceptError
			<< "Accept error err=" << OmnGetStrError(OmnErrType::eAccept) << enderr;
		return 0;
    }

    OmnIpAddr remoteIpAddr = OmnRetrieveIpAddr(cli_addr);
	int remotePort = ntohs(cli_addr.sin_port);

	// OmnTrace << "Accepted new sock: " << newSock 
	// 	<< " Remote: " << remoteIpAddr.toString() << ":" << remotePort << endl;
	OmnTcpClientPtr theTcp;
	if (mSockType == eTypeTcp) 
	{
		theTcp = OmnNew OmnTcpClient(
			OmnString("TcpServerEpoll:") << mName << ":Client", 
		remoteIpAddr, remotePort, 1, mLengthType);
	}
	else if (mSockType == eTypeUnix)
	{
		theTcp = OmnNew OmnTcpClient(
			OmnString("TcpServerEpoll:") << mName << ":Client", 
		mUnixPath, mLengthType);
	}
	
	//OmnScreen << "set sock:" << theTcp.getPtr() << ":" << newSock << endl;
	// 2013/01/08 Ice
	// Connections created by this class must not block in reading. 
	// We need to set a timer to the connection (currently 10 seconds).
	// If the connection fails reading, it will time out and the data, 
	// if any, are ignored. The read function will close the connection
	// and returns -1. 
	//struct timeval tv = {10, 0};
	//setsockopt(newSock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    theTcp->setSock(newSock);

	int opts;
	opts=fcntl(newSock,F_GETFL);
	if (!mIsBlocking)
	{
		opts = opts|O_NONBLOCK;
	}
	fcntl(newSock,F_SETFL,opts);
	theTcp->setBlockingType(mIsBlocking);

	epoll_event ev;
	aos_assert_r(newSock > 0 && newSock < mMaxConns, 0);
	//aos_assert_r(newSock > 0, 0);

	// Chen Ding, 2013/01/10
	ev.events = EPOLLIN | EPOLLET;
	//ev.events = EPOLLIN;
	ev.data.fd = newSock;
	if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, newSock, &ev) < 0)
	{
		OmnAlarm << "Failed adding port: " << mEpollFd << ":" << newSock << enderr;
		return 0;
	}
	
	aos_assert_r(newSock < mMaxConns, 0);
	mConns[newSock] = theTcp;
	if (mListener)
	{
		mListener->connCreated(theTcp);
	}
	// OmnTrace << "New sock set: " << theTcp->getSock() << endl;
    return theTcp;
}


OmnString
OmnTcpServerEpoll::getConnInfo() const
{
	return OmnString("TCP Server Connection on port = ") <<
			OmnStrUtil::itoa(mLocalCrtPort);
}


OmnRslt
OmnTcpServerEpoll::closeConn()
{
    if (mSock !=-1)
    {
        OmnTraceComm << "Closing sock: " << mSock << endl;
        OmnCloseSocket(__FILE__, __LINE__, mSock);
        mSock = -1;
        return true;
    }

	// Need to close all member connections
	for (int i = 0; i < mMaxConns; i++)
	{
		if (mConns[i])
		{
			mConns[i]->closeConn();
			mConns[i] = NULL;
		}
	}
	return true;
}


void
OmnTcpServerEpoll::setListener(const OmnTcpListenerPtr &listener)
{
	if (mListener)
	{
		OmnAlarm << "To set TCP Server listener, but there is already one!" 
			<< enderr;
	}

	mListener = listener;
	if (mReader) mReader->setListener(listener);
}


void
OmnTcpServerEpoll::startReading()
{
	if (!mListener)
	{
		OmnAlarm << "To start reading a TCP Server but no message processor" 
			<< enderr;
		return;
	}

	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "TcpServerEpoll", 0, false, true, __FILE__, __LINE__);
	}

	mThread->start();
}	


void
OmnTcpServerEpoll::stopReading()
{
	if (!mThread)
	{
		OmnAlarm << "To stop reading but thread is null" << enderr;
		return;
	}

	mThread->stop();
}


bool 
OmnTcpServerEpoll::signal(const int threadLogicId)
{
	return true;
}


bool
OmnTcpServerEpoll::checkThread(OmnString &, const int tid) const
{
	// Currently not managed by ThreadMgr. Always return true
    return true;
}


void
OmnTcpServerEpoll::addEpollServer()
{
	sgLock.lock();
	if (smEpollServers.size() >= eMaxEpollServers)
	{
		sgLock.unlock();
		return;
	}

	smEpollServers.push_back(this);
	sgLock.unlock();
}


vector<OmnTcpServerEpollPtr>
OmnTcpServerEpoll::getEpollServers()
{
	return smEpollServers;
}


void
OmnTcpServerEpoll::printStatus()
{
	if (!mReader) return;
	mReader->printStatus();
}

