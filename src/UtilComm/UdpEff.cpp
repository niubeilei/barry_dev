////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UdpEff.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/UdpEff.h"

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
#include "Porting/GetErrno.h"
#include "Porting/Sleep.h"
#include "Porting/IPv6.h"
#include "Porting/SystemError.h"
#include "Thread/Mutex.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/CommUtil.h"
#include "UtilComm/CommGroup.h"



OmnUdpEff::OmnUdpEff(const OmnString &name,
			   const OmnIpAddr &localIpAddr,
			   const int localPort)
: 
mSock(-1),
mLocalIpAddr(localIpAddr),
mLocalPort(localPort),
mName(name)
{
	OmnTraceComm << "Create UDP Connection.\n"
		<< "Name = " << mName
		<< ". Local IP = " << mLocalIpAddr.toString()
		<< ". Local Port = " << mLocalPort << endl;
}


OmnUdpEff::~OmnUdpEff ()
{
	OmnTraceComm << "Delete UDP Connection.\n"
		<< "Name = " << mName
		<< ". Local IP = " << mLocalIpAddr.toString()
		<< ". Local Port = " << mLocalPort << endl;
	closeConn();
}


bool 
OmnUdpEff::closeConn()
{
	OmnTraceComm << "Close UDP connection: " << mSock << endl;
	if (mSock > 0)
	{
     	OmnCloseSocket(__FILE__, __LINE__, mSock);
	}
	mSock = -1;
	return true;
}

	
OmnRslt
OmnUdpEff::readFrom(char *data, 
				 int &length, 
				 OmnIpAddr &remoteIpAddr,
				 int &remotePort)
{
	//
	// It reads a message from mSock.
	//

	//
	// Since this is a time-based reading, we have to use select.
	//
	struct sockaddr_in  addr;
	addr.sin_family = AF_INET;
	int addrLen = sizeof(addr);

	OmnTraceComm << "To read ("
		<< mLocalIpAddr.toString() << ":" << mLocalPort
		<< ") from " << mLocalIpAddr.toString() 
		<< ":" << mLocalPort << ":" << mSock << endl;
	int bytesRead = OmnRecvFrom(mSock, data, length, 
			(sockaddr *)&addr, &addrLen);
	if (bytesRead > 0)
	{
		//
		// Reading successful. 
		//
		length = bytesRead;
		remoteIpAddr = OmnRetrieveIpAddr(addr);
		remotePort = ntohs(addr.sin_port);
		return true;
	}

	//
	// Reading failed. The error code is returned through "errcode"
	//
	length = 0;
	return OmnAlarm << "Failed to read UDP. Sock = "
		<< mSock
		<< " from local: " << mLocalIpAddr.toString() 
		<< ":" << mLocalPort << enderr;
}


OmnRslt
OmnUdpEff::writeTo(const char *data,
				const int length,
				const OmnIpAddr &ipAddr, 
				const int port)
{
	//
	// This function writes 'data' to the Ip address "ipAddr" and 
	// port "port". 
	//

	//
	// Prepare to send
	//
	sockaddr_in address;
	address.sin_family = AF_INET;
	OmnSetIpAddr(&address, ipAddr);

	address.sin_port = htons(port);

	OmnTraceWrite << "To write ("
		<< mLocalIpAddr.toString() << ":" << mLocalPort 
		<< ") to " << ipAddr.toString() 
		<< ":" << length << ":" << port << ":" << mSock << endl;

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
			return OmnAlarm << "Failed to write UDP. Error code: " 
				<< errcode << " Sock = " << mSock
				<< " from " << mLocalIpAddr.toString() << ":" 
				<< mLocalPort
				<< " to " << ipAddr.toString() << ":" << port << enderr;
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
	return true;
}


OmnRslt
OmnUdpEff::connect(OmnString &err)
{
	return OmnCommUtil::createUdpSock(mSock, mLocalIpAddr, mLocalPort, err, true);
}


OmnString	
OmnUdpEff::toString() const
{
	return OmnString("UDP Connection. Connection Name = ") << mName 
		<< " on Local IP: " << mLocalIpAddr.toString()
		<< OmnString(" and Port: " ) << mLocalPort;
}


bool
OmnUdpEff::select(const int sec, const int usec, bool &timeout)
{
	//
	// It is used to select on its sock. 
	// If it detects something, it returns true. If timeout, it returns
	// false and timeout is true. Otherwise, it returns false.
	//

	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mSock, &fd);
	timeval timer;
	timer.tv_sec = sec;
	timer.tv_usec = usec;
	int rslt = OmnSocketSelect(mSock+1, &fd, 0, 0, &timer);

	if (rslt < 0)
	{
	    //
        // It means select failed. 'errCode' is the error code.
        //
        int errcode = OmnGetErrno();
        OmnAlarm << "Select error: " << errcode << enderr;

		timeout = false;
		return false;
	}

    if (rslt == 0)
    {
        //
        // It means timeout.
        //
		timeout = true;
		return false;
	}

	return true;
}

