////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CommUtil.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UtilComm/CommUtil.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Debug/Error.h"
#include "Porting/GetHostName.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/Socket.h"
#include "Porting/ulong.h"
#include "Porting/IPv6.h"
#include "Porting/SystemError.h"
#include "Porting/GetErrno.h"
#include "Porting/SockLen.h"
#include "Porting/BindErr.h"


OmnString 
OmnCommUtil::getHostName()
{
    char theName[100];

    if (OmnGetHostName(theName, 100) == OmnFailedToGetHostName)
    {
		OmnAlarm << "Failed To Retrieve Host Name: " << theName << enderr;
		return "";
    }

    return theName;
}


OmnIpAddr		
OmnCommUtil::getLocalIpAddr()
{
	OmnString hostname = getHostName();
	if (hostname.length() == 0)
	{
		OmnAlarm << "Failed To Get Host By Name: " << hostname << enderr;
		return OmnIpAddr::eInvalidIpAddr;
	}

	OmnTrace << "hostname = " << hostname << endl;

	OmnIpAddr addr = OmnGetIPAddrByName(hostname);
	return addr;
}


bool
OmnCommUtil::isSelectErrorRecoverable(const OmnErrId::E errId)
{
	//
	// This is to check the select(...) error conditions. The following
	// are possible error conditions for select(...)
	//
	/*
	switch (errId)
	{
	case OmnErrId::eDidNotInitializeNetBeforeUsingSocket:		
		 //
		 // A successful WSAStartup must occur before using this function. 
		 //
		 return false;
	
	case OmnErrId::eReadBuffError:
		 //
		 //WSAEFAULT: The buf parameter is not completely contained 
		 // in a valid part of the user address space. 
		 // (I don't get this - Chen Ding).
		 return false;
	
	case OmnErrId::eNetworkSubsystemFailed:
		 //
		 // WSAENETDOWN: The network subsystem has failed. 
		 //
		 return false;
	
	case OmnErrId::eSocketNotBoundBeforeReading:
		 //
		 // WSAEINVAL:	The sock has not been bound with bind, or an unknown flag was specified, 
		 // or MSG_OOB was specified for a sock with SO_OOBINLINE enabled or (for byte 
		 // stream socks only) len was zero or negative. 
		 return false;
	
	case OmnErrId::eBlockingSocketReadCancelled:
		 //
		 // WSAEINTR: The (blocking) call was canceled through WSACancelBlockingCall. 
		 // Should not happen in our environment. 
		 return false;
	
	case OmnErrId::eBlockingReadingInProgress:
		 //
		 // WSAEINPROGRESS:	A blocking Windows Sockets 1.1 call is in progress, or the service 
		 // provider is still processing a callback function. 
		 return false;
	
	case OmnErrId::eInvalidSocketDescriptor:
		 //
		 // WSAENOTSOCK: The descriptor is not a sock. 
		 //
		 return false;

	default:
		 OmnError::log(OmnFileLine, 0, OmnErrId::eUnrecognizedSocketSelectError);
		 return false;
	}
	*/
	return false;
}


OmnRslt
OmnCommUtil::createUdpSock(int &sock, 
						   const OmnIpAddr &localIpAddr,
						   int &localPort,
						   OmnString &err, 
						   const bool raiseAlarm)
{
	//
	// This function creates a Udp Connection at 'localIpAddr' and 'localPort'.
	// If 'localPort' == 0, the system will allocate a port (1024-5000), and the
	// port is returned through 'localPort'. If successful, it returns true. 
	// Otherwise, it returns an errno. 
	//

	//
	// Create the sock.
	//
	OmnTraceComm << "Create UDP sock" << endl;
	// sock = socket(AF_INET, SOCK_DGRAM, 0);
	sock = OmnCreateUdpSocket(__FILE__, __LINE__);

	if (sock <= 0)
	{
		//
		// Failed to create the sock
		//
		err = "Failed to create UDP sock: ";
		err	<< localIpAddr.toString() << ":" << localPort
			<< " sock = " << sock
			<< ". Error: " << OmnGetStrError(OmnErrType::eSocket);
		if (raiseAlarm)
		{
			return OmnWarn << "Failed To Create Udp Conn: "
				<< err << enderr;
		}
		else
		{
			return false;
		}
	}

	OmnTraceComm << "Socket created successfully: " << sock << endl;

    struct linger ling;
    ling.l_onoff = 0;
    ling.l_linger = 0;

    setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*) &ling, sizeof (ling));
//    if (setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*) &ling, sizeof (ling)) != 0)
//	{
//		err = "Failed to set sockopt SO_LINGER on sock: ";
//		err << sock 
//			<< ". optname = " << SO_LINGER 
//			<< ":" << OmnGetStrError(OmnErrType::eSetSocketOpt);
//
//		if (raiseAlarm)
//		{
//			OmnWarn << "Set Sockopt Err: " << err << enderr;
//		}
//	}

    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) NULL, 0);

	//
	// Bind to any local address
	//
	struct sockaddr_in sockAdd;
	memset((char*) &sockAdd, 0, sizeof (sockAdd));

	sockAdd.sin_family      = AF_INET;
	OmnSetIpAddr(&sockAdd, localIpAddr);
	sockAdd.sin_port        = htons(localPort);
		
	OmnTraceComm << "To bind to: " << sock
		<< ", IP Address = " << localIpAddr.toString() 
		<< ", Local port = " << localPort << endl;

	int result = bind(sock, (struct sockaddr*)&sockAdd, sizeof(sockAdd));
	if (result != 0)
	{
		err = "UDP failed to bind UDP Client. Errno = ";
//		err	<< OmnGetStrError(OmnErrType::eBind)
		err	<< OmnGetStrError(OmnErrType::eSystem)
			<< ". Address: " << localIpAddr.toString()
			<< ":" << localPort;
		return OmnAlarm << "Socket Bind Error: "
			<< err << enderr;
	}

	OmnTraceComm << "Bind successfully!" << endl;

	//
	// Retrieve the port
	//
	OmnSockLen addrLen = sizeof(sockAdd);
	if (getsockname(sock, (struct sockaddr*)&sockAdd, &addrLen) == 0)
	{
		localPort = ntohs(sockAdd.sin_port);
		OmnTraceComm << "On port = " << localPort << endl;
	}
	else
	{
		err = "Failed to getsockname";
		return OmnWarn << OmnErrId::eWarnCommError << err << enderr;
	}
 
	return true;
}


// Chen Ding, 02/23/2012
void 
OmnCommUtil::reportGetSockNameError(const int err_no, const int sock)
{
	switch (err_no)
	{
	case EBADF:  
		 OmnAlarm << "The argument sockfd is not a valid descriptor: " << sock << enderr;
		 return;

	case EFAULT:
		 OmnAlarm << "The addr argument points to memory not in a valid part of the process address space: " 
			 << sock << enderr;
		 return;

	case EINVAL:
		 OmnAlarm << "addrlen is invalid (e.g., is negative): " << sock << enderr;
		 return;

	case ENOBUFS:
		 OmnAlarm << "Insufficient resources were available in the system to perform the operation: " 
			 << sock << enderr;
		 return;

	case ENOTSOCK:
		 OmnAlarm << "The argument sockfd is a file, not a sock: " << sock << enderr;
		 return;

	default:
		 OmnAlarm << "Unrecognized errno: " << err_no << ":" << sock << enderr;
		 return;
	}
}

