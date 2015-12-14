////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Socket.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Porting/Socket.h"

//-------------------------------------------------------------
// UNIX Portion
//-------------------------------------------------------------
#ifdef	OMN_PLATFORM_UNIX
#include "Thread/Mutex.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"


static OmnMutex 	sgLock;
const int sgMaxConns = 10000;
static OmnString 	sgSockStatusFile[sgMaxConns];
static int			sgSockStatusLine[sgMaxConns];
static bool			sgSockStatusInited = false;


static void OmnCheckSock(const char *file, const int line, const int sock)
{
	if (!sgSockStatusInited) 
	{
		for (int i=0; i<sgMaxConns; i++) sgSockStatusLine[i] = -1;
		sgSockStatusInited = true;
	}

	if (sock <= 0 || sock >= sgMaxConns)
	{
		return;
	}

	if (sgSockStatusLine[sock] != -1)
	{
		OmnAlarm << "Internal error: " << sgSockStatusFile[sock] 
			<< ":" << sgSockStatusLine[sock] 
			<< ":" << sock << enderr;
	}

	sgSockStatusFile[sock] = file;
	sgSockStatusLine[sock] = line;
}


int OmnCreateTcpSocket(const char *file, const int line)
{
	sgLock.lock();
	int sock = ::socket(AF_INET, SOCK_STREAM, 0);
	OmnCheckSock(file, line, sock);
	sgLock.unlock();
	return sock;
}


int OmnCreateUdpSocket(const char *file, const int line)
{
	sgLock.lock();
	int sock = ::socket(AF_INET, SOCK_DGRAM, 0);
	OmnCheckSock(file, line, sock);
	sgLock.unlock();
	return sock;
}


int OmnCreateUnixSocket(const char *file, const int line)
{
	sgLock.lock();
	int sock = ::socket(AF_UNIX, SOCK_STREAM, 0);
	OmnCheckSock(file, line, sock);
	sgLock.unlock();
	return sock;
}


int OmnCreateEpollSocket(const char *file, const int line, const int max_conns)
{
	sgLock.lock();
	int sock = ::epoll_create(max_conns);
	OmnCheckSock(file, line, sock);
	sgLock.unlock();
	return sock;
}


int OmnCloseSocket(const char *file, const int line, const int ss)
{
	if (ss <= 0) return 0;
	sgLock.lock();
	//cout << "Close socket: " << file << ":" << line << ":" << ss << endl;
	int rslt = ::close(ss);
	if (ss > 0 && ss < sgMaxConns)
	{
		if (sgSockStatusLine[ss] < 0)
		{
			OmnAlarm << "internal error: " 
				<< file << ":" << line
				<< ":" << sgSockStatusLine[ss] 
				<< ":" << sgSockStatusFile[ss] 
				<< ":" << ss << enderr;
		}
	}
	sgSockStatusLine[ss] = -1;
	sgLock.unlock();
	return rslt;
}

int OmnAcceptSocket(
		const char *file, 
		const int line, 
		const int server_sock, 
		struct sockaddr * cli_addr, 
		socklen_t *clilen)
{
	sgLock.lock();
	int newSock = ::accept(server_sock, cli_addr, clilen);
	OmnCheckSock(file, line, newSock);
	sgLock.unlock();
	return newSock;
}

/*
OmnErrId::E OmnCheckReadSocketError(const int errcode)
{
	//
	// Call this function if one called recv(...) and it failed.
	// This function enumerates all the error codes this function
	// may return (based on Microsoft document). It translates
	// into our internal error code, which is platform 
	// independent.
	//
	switch (errcode)
	{
	case EFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case ENOTCONN:			// The socket is not connected. 
		 return OmnErrId::eSocketNotConnectedBeforeReading;

	case EINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case ENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case EINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case EBADF:
	case ECONNREFUSED:
	case EAGAIN:

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
}


OmnErrId::E
OmnCheckWriteSocketError(const int errcode)
{
	//
	// Call this function if one called send(...) and it failed.
	// This function enumerates all the error codes this function
	// may return (based on Microsoft document). It translates
	// into our internal error code, which is platform 
	// independent.
	//
	switch (errcode)
	{
	case EINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case EFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case ENOBUFS:			// No buffer space is available. 
		 return OmnErrId::eNoBufferWhenWritingToSocket;

	case ENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case EPIPE:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

	case EWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
	//case EAGAIN:
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case EMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case EINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case EBADF:
	case ENOMEM:

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
}


OmnErrId::E
OmnCheckSocketSelectError(const int errcode)
{
	switch (errcode)
	{
	case EINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;
	
	case EINTR:				    // The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;
	
	case EBADF:					// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case ENOMEM:
	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedSocketSelectError;
	}
}


OmnErrId::E
OmnCheckRecvFromError(const int errcode)
{
	switch (errcode)
	{
	case EFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case EINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case EINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case ENOTSOCK:			// The descriptor is not a socket. 
	case EBADF:
		 return OmnErrId::eInvalidSocketDescriptor;

	case EAGAIN:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case ECONNREFUSED:
	case ENOTCONN:

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
}


OmnErrId::E
OmnCheckSendToError(const int errcode)
{
	//
	// Following are all the error conditions from sendTo(...)
	//
	switch (errcode)
	{
	case EINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case EINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case EFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case ENOBUFS:			// No buffer space is available. 
		 return OmnErrId::eNoBufferWhenWritingToSocket;

	case ENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case EPIPE:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

//	case EAGAIN:
	case EWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case EMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case ENOMEM:
	case EBADF:
	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
}
*/

//-------------------------------------------------------------
// Microsoft Portion
//-------------------------------------------------------------
#elif OMN_PLATFORM_MICROSOFT


OmnErrId::E OmnCheckReadSocketError(const int errcode)
{
	//
	// Call this function if one called recv(...) and it failed.
	// This function enumerates all the error codes this function
	// may return (based on Microsoft document). It translates
	// into our internal error code, which is platform 
	// independent.
	//

	/*
	switch (errcode)
	{
	case WSANOTINITIALISED:		// A successful WSAStartup must occur before using this function. 
		 return OmnErrId::eCommError;

	case WSAENETDOWN:			// The network subsystem has failed. 
		 return OmnErrId::eCommError;

	case WSAEFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eCommError;

	case WSAENOTCONN:			// The socket is not connected. 
		 return OmnErrId::eSocketNotConnectedBeforeReading;

	case WSAEINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case WSAEINPROGRESS:		// A blocking Windows Sockets 1.1 call is in progress, or the service 
								// provider is still processing a callback function. 
		 return OmnErrId::eBlockingReadingInProgress;

	case WSAENETRESET:			// The connection has been broken due to the keep-alive activity 
								// detecting a failure while the operation was in progress. 
		 return OmnErrId::eSocketConnectionBroken;

	case WSAENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case WSAEOPNOTSUPP:			// MSG_OOB was specified, but the socket is not stream-style such as 
								// type SOCK_STREAM, out-of-band data is not supported in the 
								// communication domain associated with this socket, or the socket is 
								// unidirectional and supports only send operations. 
		 return OmnErrId::eOutOfBandSocketReadingError;

	case WSAESHUTDOWN:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

	case WSAEWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case WSAEMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case WSAEINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case WSAECONNABORTED:		// The virtual circuit was terminated due to a time-out or other failure. 
								// The application should close the socket as it is no longer usable. 
		 return OmnErrId::eSocketNotUsableWhenReading;

	case WSAETIMEDOUT:			// The connection has been dropped because of a network failure or because 
								// the peer system failed to respond. 
		 return OmnErrId::eSocketHasBeenDropped;

	case WSAECONNRESET:			// The virtual circuit was reset by the remote side executing a "hard" or 
								// "abortive" close. The application should close the socket as it is no 
								// longer usable. On a UDP datagram socket this error would indicate that 
								// a previous send operation resulted in an ICMP "Port Unreachable" message. 
		 return OmnErrId::eSocketClosedByRemoteSideWhileReading;

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
	*/

	return OmnErrId::eCommError;
}


OmnErrId::E
OmnCheckWriteSocketError(const int errcode)
{
	//
	// Call this function if one called send(...) and it failed.
	// This function enumerates all the error codes this function
	// may return (based on Microsoft document). It translates
	// into our internal error code, which is platform 
	// independent.
	//
	/*
	switch (errcode)
	{
	case WSANOTINITIALISED:		// A successful WSAStartup must occur before using this function. 
		 return OmnErrId::eDidNotInitializeNetBeforeUsingSocket;

	case WSAENETDOWN:			// The network subsystem has failed. 
		 return OmnErrId::eNetworkSubsystemFailed;

	case WSAEACCES:				// The requested address is a broadcast address, but the appropriate flag 
								// was not set. Call setsockopt with the SO_BROADCAST parameter to allow 
								// the use of the broadcast address. 
		 return OmnErrId::eWriteToABroadcastSocketError;

	case WSAEINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case WSAEINPROGRESS:		// A blocking Windows Sockets 1.1 call is in progress, or the service 
								// provider is still processing a callback function. 
		 return OmnErrId::eBlockingReadingInProgress;

	case WSAEFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case WSAENETRESET:			// The connection has been broken due to the keep-alive activity 
								// detecting a failure while the operation was in progress. 
		 return OmnErrId::eSocketConnectionBroken;

	case WSAENOBUFS:			// No buffer space is available. 
		 return OmnErrId::eNoBufferWhenWritingToSocket;

	case WSAENOTCONN:			// The socket is not connected. 
		 return OmnErrId::eSocketNotConnectedBeforeReading;

	case WSAENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case WSAEOPNOTSUPP:			// MSG_OOB was specified, but the socket is not stream-style such as 
								// type SOCK_STREAM, out-of-band data is not supported in the 
								// communication domain associated with this socket, or the socket is 
								// unidirectional and supports only send operations. 
		 return OmnErrId::eOutOfBandSocketReadingError;

	case WSAESHUTDOWN:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

	case WSAEWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case WSAEMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case WSAEHOSTUNREACH:		// The remote host cannot be reached from this host at this time. 
		 return OmnErrId::eRemoteHostNotReachableWhenWriting;

	case WSAEINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case WSAECONNABORTED:		// The virtual circuit was terminated due to a time-out or other failure. 
								// The application should close the socket as it is no longer usable. 
		 return OmnErrId::eSocketNotUsableWhenReading;

	case WSAECONNRESET:			// The virtual circuit was reset by the remote side executing a "hard" or 
								// "abortive" close. The application should close the socket as it is no 
								// longer usable. On a UDP datagram socket this error would indicate that 
								// a previous send operation resulted in an ICMP "Port Unreachable" message. 
		 return OmnErrId::eSocketClosedByRemoteSideWhileReading;

	case WSAETIMEDOUT:			// The connection has been dropped because of a network failure or because 
								// the peer system failed to respond. 
		 return OmnErrId::eSocketHasBeenDropped;

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
	*/

	return OmnErrId::eCommError;
}


OmnErrId::E
OmnCheckSocketSelectError(const int errcode)
{
	/*
	switch (errcode)
	{
	case WSANOTINITIALISED:		// A successful WSAStartup must occur before using this function. 
		 return OmnErrId::eDidNotInitializeNetBeforeUsingSocket;
	
	case WSAEFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;
	
	case WSAENETDOWN:			// The network subsystem has failed. 
		 return OmnErrId::eNetworkSubsystemFailed;
	
	case WSAEINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;
	
	case WSAEINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;
	
	case WSAEINPROGRESS:		// A blocking Windows Sockets 1.1 call is in progress, or the service 
								// provider is still processing a callback function. 
		 return OmnErrId::eBlockingReadingInProgress;
	
	case WSAENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
	*/
	return OmnErrId::eCommError;
}


OmnErrId::E
OmnCheckRecvFromError(const int errcode)
{
	/*
	switch (errcode)
	{
	case WSANOTINITIALISED:		// A successful WSAStartup must occur before using this function. 
		 return OmnErrId::eDidNotInitializeNetBeforeUsingSocket;

	case WSAENETDOWN:			// The network subsystem has failed. 
		 return OmnErrId::eNetworkSubsystemFailed;

	case WSAEFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case WSAEINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case WSAEINPROGRESS:		// A blocking Windows Sockets 1.1 call is in progress, or the service 
								// provider is still processing a callback function. 
		 return OmnErrId::eBlockingReadingInProgress;

	case WSAEINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case WSAEISCONN:			// The socket is connected. This function is not permitted with a 
								// connected socket, whether the socket is connection-oriented or connectionless. 
		 return OmnErrId::eReadUDPFromAConnectedSockNotAllowed;

	case WSAENETRESET:			// The connection has been broken due to the keep-alive activity 
								// detecting a failure while the operation was in progress. 
		 return OmnErrId::eSocketConnectionBroken;

	case WSAENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case WSAEOPNOTSUPP:			// MSG_OOB was specified, but the socket is not stream-style such as 
								// type SOCK_STREAM, out-of-band data is not supported in the 
								// communication domain associated with this socket, or the socket is 
								// unidirectional and supports only send operations. 
		 return OmnErrId::eOutOfBandSocketReadingError;

	case WSAESHUTDOWN:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

	case WSAEWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case WSAEMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case WSAETIMEDOUT:			// The connection has been dropped because of a network failure or because 
								// the peer system failed to respond. 
		 return OmnErrId::eSocketHasBeenDropped;

	case WSAECONNRESET:			// The virtual circuit was reset by the remote side executing a "hard" or 
								// "abortive" close. The application should close the socket as it is no 
								// longer usable. On a UDP datagram socket this error would indicate that 
								// a previous send operation resulted in an ICMP "Port Unreachable" message. 
		 return OmnErrId::eSocketClosedByRemoteSideWhileReading;

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
	*/

	return OmnErrId::eCommError;
}


OmnErrId::E
OmnCheckSendToError(const int errcode)
{
	//
	// Following are all the error conditions from sendTo(...)
	//
	/*
	switch (errcode)
	{
	case WSANOTINITIALISED:		// A successful WSAStartup must occur before using this function. 
		 return OmnErrId::eDidNotInitializeNetBeforeUsingSocket;

	case WSAENETDOWN:			// The network subsystem has failed. 
		 return OmnErrId::eNetworkSubsystemFailed;

	case WSAEACCES:				// The requested address is a broadcast address, but the appropriate flag 
								// was not set. Call setsockopt with the SO_BROADCAST parameter to allow 
								// the use of the broadcast address. 
		 return OmnErrId::eWriteToABroadcastSocketError;

	case WSAEINVAL:				// The socket has not been bound with bind, or an unknown flag was specified, 
								// or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte 
								// stream sockets only) len was zero or negative. 
		 return OmnErrId::eSocketNotBoundBeforeReading;

	case WSAEINTR:				// The (blocking) call was canceled through WSACancelBlockingCall. 
								// Should not happen in our environment. 
		 return OmnErrId::eBlockingSocketReadCancelled;

	case WSAEINPROGRESS:		// A blocking Windows Sockets 1.1 call is in progress, or the service 
								// provider is still processing a callback function. 
		 return OmnErrId::eBlockingReadingInProgress;

	case WSAEFAULT:				// The buf parameter is not completely contained 
								// in a valid part of the user address space. 
								// (I don't get this - Chen Ding).
		 return OmnErrId::eReadBuffError;

	case WSAENETRESET:			// The connection has been broken due to the keep-alive activity 
								// detecting a failure while the operation was in progress. 
		 return OmnErrId::eSocketConnectionBroken;

	case WSAENOBUFS:			// No buffer space is available. 
		 return OmnErrId::eNoBufferWhenWritingToSocket;

	case WSAENOTCONN:			// The socket is not connected. 
		 return OmnErrId::eSocketNotConnectedBeforeReading;

	case WSAENOTSOCK:			// The descriptor is not a socket. 
		 return OmnErrId::eInvalidSocketDescriptor;

	case WSAEOPNOTSUPP:			// MSG_OOB was specified, but the socket is not stream-style such as 
								// type SOCK_STREAM, out-of-band data is not supported in the 
								// communication domain associated with this socket, or the socket is 
								// unidirectional and supports only send operations. 
		 return OmnErrId::eOutOfBandSocketReadingError;

	case WSAESHUTDOWN:			// The socket has been shut down; it is not possible to recv on 
								// a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. 
		 return OmnErrId::eSocketHasBeenShutdownWhenReading;

	case WSAEWOULDBLOCK:		// The socket is marked as nonblocking and the receive operation would block. 
		 return OmnErrId::eBlockReadingButSocketIsNotBlocking;

	case WSAEMSGSIZE:			// The message was too large to fit into the specified buffer and was truncated. 
		 return OmnErrId::eSocketReadBufferTooSmall;

	case WSAEHOSTUNREACH:		// The remote host cannot be reached from this host at this time. 
		 return OmnErrId::eRemoteHostNotReachableWhenWriting;

	case WSAECONNABORTED:		// The virtual circuit was terminated due to a time-out or other failure. 
								// The application should close the socket as it is no longer usable. 
		 return OmnErrId::eSocketNotUsableWhenReading;

	case WSAECONNRESET:			// The virtual circuit was reset by the remote side executing a "hard" or 
								// "abortive" close. The application should close the socket as it is no 
								// longer usable. On a UDP datagram socket this error would indicate that 
								// a previous send operation resulted in an ICMP "Port Unreachable" message. 
		 return OmnErrId::eSocketClosedByRemoteSideWhileReading;

	case WSAEADDRNOTAVAIL:		// The remote address is not a valid address, for example, ADDR_ANY. 
		 return OmnErrId::eInvalidRemoteAddrInSendTo;

	case WSAEAFNOSUPPORT:		// Addresses in the specified family cannot be used with this socket. 
		 return OmnErrId::eRemoteAddrNotAllowedInSendTo;

	case WSAEDESTADDRREQ:		// A destination address is required. 
		 return OmnErrId::eMissingRemoteAddrInSendTo;

	case WSAENETUNREACH:		// The network cannot be reached from this host at this time. 
		 return OmnErrId::eNetworkNotReachedInSendTo;

	case WSAETIMEDOUT:			// The connection has been dropped because of a network failure or because 
								// the peer system failed to respond. 
		 return OmnErrId::eSocketHasBeenDropped;

	default:
		 //
		 // Unrecognized error code
		 //
		 return OmnErrId::eUnrecognizedMicrosoftErrorCode;
	}
	*/

	return OmnErrId::eCommError;
}



#endif

