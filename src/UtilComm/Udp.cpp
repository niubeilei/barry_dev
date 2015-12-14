////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Udp.cpp
// Description:
//   
//
// Modification History:
// 		Changed OmnSystemError::translateSystemError(errcode)
// 		to "OmnGetStrError()" 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/Udp.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/GetHostName.h"
#include "Porting/Socket.h"
#include "Porting/Select.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/Sleep.h"
#include "Porting/IPv6.h"
#include "Porting/SystemError.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommUtil.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/CommGroup.h"


int OmnUdp::csMaxSelectTry = 5;
int	OmnUdp::csSelectTryIntervalSec = 2;

OmnUdp::OmnUdp(const OmnString &name,
			   const OmnIpAddr &localIpAddr,
			   const int localPort)
: 
mSock(-1),
mLock(OmnNew OmnMutex()),
mLocalIpAddr(localIpAddr),
mLocalPort(localPort),
mName(name),
mDebug(true)
{
	OmnTraceComm << "Create UDP Connection.\n"
		<< "Name = " << mName
		<< ". Local IP = " << mLocalIpAddr.toString()
		<< ". Local Port = " << mLocalPort << endl;
}


OmnUdp::~OmnUdp ()
{
	OmnTrace << "Delete UDP Connection.\n"
		<< "Name = " << mName
		<< ". Local IP = " << mLocalIpAddr.toString()
		<< ". Local Port = " << mLocalPort << endl;
	closeConn();
}


bool 
OmnUdp::closeConn()
{
	OmnTrace << "Close UDP connection: " << mSock << endl;
	if (mSock > 0)
	{
     	OmnCloseSocket(__FILE__, __LINE__, mSock);
	}
	mSock = -1;
	return true;
}

	
OmnRslt
OmnUdp::readFrom(OmnConnBuffPtr &buffer,
				 const int timerSec,
				 const int timeruSec, 
				 bool &isTimeout)
{
	//
	// It reads a message from mSock using "select()". If "select()" 
	// fails, which means it is a connection error. It checks the 
	// type of error and may try maximum three times. If after three 
	// times trial, the problem is still there,
	// it is an uncure problem. It reports the problem and returns false.
	//
	// Otherwise, if there is nothing in mSock, it waits until either there are 
	// something or "timerValue" timeout. If it is timer out, it returns true
	// and 'length' is set to 0. 
	// Otherwise, it returns the contents read.
	//
	// If 'timerSec' < 0, it waits forever.
	//

	//
	// Since this is a time-based reading, we have to use select.
	//

	isTimeout = false;

	if (mSock < 0 || mSock >= OmnCommTypes::eMaxSockToSelect)
	{
		return OmnAlarm << OmnErrId::eSocketTooBig
			<< "To read a sock, but the sock is too big "
			<< "to select or negative: " 
			<< mSock
			<< ". " << toString() << enderr;
	}

	char data[OmnCommTypes::eMaxUdpPacketSize];
	int length = OmnCommTypes::eMaxUdpPacketSize;
	buffer = 0;
	
	fd_set fd; 	
	FD_ZERO(&fd);
	FD_SET(mSock, &fd);
	OmnRslt result(true);
	int tries = 0;
	while (tries < csMaxSelectTry)
	{
		int rslt;
		if (timerSec <= 0)
		{
			OmnTraceComm << "To select: no timer: " << mSock << endl;
			rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, 0);
		}
		else
		{
			OmnTraceComm << "To select with timer: " << timerSec 
				<< ". Sock = " << mSock << endl;
			timeval timer;
			timer.tv_sec = timerSec;
			timer.tv_usec = timeruSec;
			rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, &timer);
		}

		if (rslt < 0)
		{
			//
			// It means select failed. 'errCode' is the error code. 
			//
			// int errcode = OmnGetErrno();
			result = OmnAlarm << OmnErrId::eSocketSelectError
				<< "Select error: " << OmnGetStrError(OmnErrType::eSelect) << enderr;

			//
			// It is recoverable. 
			//
			tries++;
			OmnSleep(csSelectTryIntervalSec);
			continue;
		}

		if (rslt == 0)
		{
			//
			// It means timeout.
			//
			length = 0;
			OmnTraceComm << "Reading timeout on: " 
				<< mLocalIpAddr.toString() << ":" 
				<< mLocalPort << endl;
			mLastReadTimeout = true;
			isTimeout = true;
			return true;
		}

		//
		// Otherwise, there should be something to read
		//

		//
		// Select successful. Read the data
		//
		struct sockaddr_in  addr;
		addr.sin_family = AF_INET;
		int addrLen = sizeof(addr);

		OmnTraceComm << "To read from " << mLocalIpAddr.toString() 
			<< ":" << mLocalPort << ":" << mSock 
			<< ":" << rslt << endl;
		int bytesRead = OmnRecvFrom(mSock, data, length, 
			(sockaddr *)&addr, &addrLen);
		if (bytesRead > 0)
		{
			//
			// Reading successful. 
			//
			OmnIpAddr remoteIpAddr = OmnRetrieveIpAddr(addr);
			int remotePort = ntohs(addr.sin_port);

			try
			{
				buffer = OmnNew OmnConnBuff(data, bytesRead, false);
			}
			catch(...)
			{
				closeConn();
				return false;
			}

			buffer->setAddr(mLocalIpAddr, mLocalPort, remoteIpAddr, remotePort);

			OmnTraceComm << "Read from: " << remoteIpAddr.toString()
				<< ":" << remotePort 
				<< " through local: " 
				<< mLocalIpAddr.toString() 
				<< ":" << mLocalPort << endl;
			return true;
		}

		if (bytesRead == 0)
		{
			//
			// It did not read anything. The remote side has gracefully closed 
			// the connection.
			//
			length = 0;

			// 
			// Chen Ding, 08/14/2003, 2003-0091
			// 	
			return OmnWarn << OmnErrId::eSocketReadError
				<< "Failed to read anything: " 
				<< mLocalIpAddr.toString() << ":" << mLocalPort << ", "
				<< OmnRetrieveIpAddr(addr).toString()
				<< ":" << ntohs(addr.sin_port) << enderr;
		}

		//
		// Reading failed. The error code is returned through "errcode"
		//
		int errcode = OmnGetErrno();

		if (OmnPortUnreachableError(errcode))
		{
			// 
			// This means that the reading fails because the previous sending resulted in
			// a 'Port Unreachable' ICMP message. This is nothing to worry about. 
			// Go back and to read again.
			//
			continue;
		}

		OmnAlarm << "Reading failed: " << errcode
			<< ":" << mLocalIpAddr.toString() 
			<< ":" << mLocalPort << ":" << mSock 
			<< ":" << bytesRead
			<< ". " << OmnGetStrError(OmnErrType::eRecvfrom) << enderr;
		return false;
	}

	//
	// It should never come to this point.
	//
	OmnShouldNeverComeToThisPoint;
	return false;
}


OmnRslt
OmnUdp::writeTo(const char *data,
				const int length,
				const OmnIpAddr &ipAddr, 
				const int port)
{
	//
	// This function writes 'data' to the Ip address "ipAddr" and 
	// port "port". 
	//
	if (!ipAddr.isValid() || port <= 0)
	{
		return OmnAlarm << "Invalid address to send: " 
			<< ipAddr.toString() << ":" << port
			<< enderr;
	}

	//
	// Prepare to send
	//
	sockaddr_in address;
	address.sin_family = AF_INET;
	OmnSetIpAddr(&address, ipAddr);

	address.sin_port = htons(port);

	// OmnTrace << "To write ("
	// 	<< mLocalIpAddr.toString() << ":" << mLocalPort 
	// 	<< ") to " << ipAddr.toString() 
	// 	<< ":" << port << ":" << mSock << endl;
	if (!data)
	{
		//
		// Not data to send. This is an error.
		//
		return OmnAlarm << "Null pointer passed in" << enderr;
	}

	//
	// Call the function to actually send the data
	//
	int bytesLeft = length;
	const char *dataToWrite = data;
	while (bytesLeft > 0)
	{
		int bytesSent = OmnSendTo(mSock, dataToWrite, bytesLeft, 
				(sockaddr*) &address, sizeof(address));

		if (bytesSent <= 0)
		{
			//
			// Failed to read. Check the errors and return.
			//
			int errcode = OmnGetErrno();
			return OmnAlarm << "Failed to write UDP: " << errcode
				<< ". " << OmnGetStrError(OmnErrType::eSendto) << ": "
				<< mLocalIpAddr.toString() << ":" << mLocalPort
				<< ", to " << ipAddr.toString() << ":" << port 
				<< ":" << mSock 
				<< ":" << bytesSent << ":" << bytesLeft << enderr;
		}

		//
		// Sent successfully, but it doesn't mean that all data has been sent.
		// Check the number and determine what has happened.
		//
		bytesLeft -= bytesSent;
     	dataToWrite = dataToWrite + bytesSent;
    }

	//
	// All data has been sent out successfully
	//
	if (mDebug)
	{
		OmnTraceComm << "Write to: " << ipAddr.toString() << ":" << port
			<< ":" 
			<< mLocalIpAddr.toString() << ":" 
			<< mLocalPort << ". Length = " << length
			<< ", " << data << endl;
	}
	return true;
}


OmnRslt
OmnUdp::connect(OmnString &err)
{
	return OmnCommUtil::createUdpSock(mSock, mLocalIpAddr, mLocalPort, err, true);
}


OmnString	
OmnUdp::toString() const
{
	return OmnString("UDP Connection. Connection Name = ") << mName 
		<< " on Local IP: " << mLocalIpAddr.toString()
		<< OmnString(" and Port: " ) << mLocalPort;
}


bool
OmnUdp::isConnGood() const
{
	return OmnConn::isConnGood(mSock);
}


OmnRslt
OmnUdp::reconnect(OmnString &err)
{
	if (OmnConn::isConnGood(mSock))
	{
		//
		// The connection is good
		//
		return OmnAlarm << "Reconnect ButConn Is Good: " << toString() << enderr;
	}

	//
	// The connection is not good. 
	//
	closeConn();

	//
	// Keep on reconnecting until it connects successfully
	//
	while (1)
	{
		connect(err);
	
		if (isConnGood())
		{
			//
			// Reconnected
			//
			return true;
		}

		//
		// Failed to reconnect. Sleep a while and retry it.
		//
		OmnSleep(OmnCommTypes::eReconnectTimer);
	}
}


bool        
OmnUdp::hasPacketToRead()
{
	if (mSock <= 0 || mSock >= OmnCommTypes::eMaxSockToSelect)
	{
		//
		// The sock is not correct
		//
		OmnAlarm << "To poll an invalid sock: " << mSock << enderr;
		return false;
	}

    fd_set fd;
    FD_ZERO(&fd);
    FD_SET(mSock, &fd);

    timeval timer;
    timer.tv_sec = 0;
    timer.tv_usec = 0;
    int rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, &timer);
	return (rslt > 0);	
}
