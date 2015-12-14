////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GetErrno.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef OMN_PLATFORM_UNIX




#elif OMN_PLATFORM_MICROSOFT
#include "Porting/GetErrnoStr.h"

OmnString OmnGetSelectErrString(const int eno)
{
	OmnString err = strerror(WSANOTINITIALISED);

	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function.";

	case WSAEFAULT:
		 return " The Windows Sockets implementation was unable to allocate needed resources for its internal operations, or the readfds, writefds, exceptfds, or timeval parameters are not part of the user address space.";

	case WSAENETDOWN:
		 return " The network subsystem has failed.";

	case WSAEINVAL:
		 return " The time-out value is not valid, or all three descriptor parameters were null.";

	case WSAEINTR:
		 return " A blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall.";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";

	case WSAENOTSOCK:
		 return " One of the descriptor sets contains an entry that is not a socket.";

	default:
		 return OmnString("Unknown: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetSetSocketOptErrString(const int eno)
{
	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function";

	case WSAENETDOWN:
		 return "The network subsystem has failed";

	case WSAEFAULT: 
		 return "optval is not in a valid part of the process address space or optlen parameter is too small";

	case WSAEINPROGRESS: 
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function";

	case WSAEINVAL: 
		 return " level is not valid, or the information in optval is not valid";

	case WSAENETRESET: 
		 return " Connection has timed out when SO_KEEPALIVE is set";

	case WSAENOPROTOOPT: 
		 return " The option is unknown or unsupported for the specified provider or socket (see SO_GROUP_PRIORITY limitations)";

	case WSAENOTCONN: 
		 return " Connection has been reset when SO_KEEPALIVE is set";

	case WSAENOTSOCK: 
		 return " The descriptor is not a socket";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}

	return "Should never come to this point";
}


OmnString OmnGetSocketErrString(const int eno)
{
	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function.";

	case WSAENETDOWN:
		 return " The network subsystem or the associated service provider has failed.";

	case WSAEAFNOSUPPORT:
		 return " The specified address family is not supported.";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";

	case WSAEMFILE:
		 return " No more socket descriptors are available.";

	case WSAENOBUFS:
		 return " No buffer space is available. The socket cannot be created.";

	case WSAEPROTONOSUPPORT:
		 return " The specified protocol is not supported.";

	case WSAEPROTOTYPE:
		 return " The specified protocol is the wrong type for this socket.";

	case WSAESOCKTNOSUPPORT:
		 return " The specified socket type is not supported in this address family.";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetBindErrString(const int eno)
{
	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function.";

	case WSAENETDOWN:
		 return " The network subsystem has failed.";

	case WSAEACCES:
		 return " Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.";

	case WSAEADDRINUSE:
		 return " A process on the computer is already bound to the same fully-qualified address and the socket has not been marked to allow address reuse with SO_REUSEADDR. For example, the IP address and port are bound in the af_inet case). (See the SO_REUSEADDR socket option under setsockopt.)";

	case WSAEADDRNOTAVAIL:
		 return " The specified address is not a valid address for this computer.";

	case WSAEFAULT:
		 return " The name or namelen parameter is not a valid part of the user address space, the namelen parameter is too small, the name parameter contains an incorrect address format for the associated address family, or the first two bytes of the memory block specified by name does not match the address family associated with the socket descriptor s.";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";

	case WSAEINVAL:
		 return " The socket is already bound to an address.";

	case WSAENOBUFS:
		 return " Not enough buffers available, too many connections.";

	case WSAENOTSOCK:
		 return " The descriptor is not a socket.";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetSendtoErrString(const int eno)
{
	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function.";

	case WSAENETDOWN:
		 return " The network subsystem has failed. ";

	case WSAEACCES:
		 return " The requested address is a broadcast address, but the appropriate flag was not set. Call setsockopt with the SO_BROADCAST parameter to allow the use of the broadcast address. ";

	case WSAEINVAL:
		 return " An unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled. ";

	case WSAEINTR:
		 return " A blocking Windows Sockets 1.1 call was canceled through WSACancelBlockingCall. ";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";

	case WSAEFAULT:
		 return " The buf or to parameters are not part of the user address space, or the tolen parameter is too small. ";

	case WSAENETRESET:
		 return " The connection has been broken due to keep-alive activity detecting a failure while the operation was in progress. ";

	case WSAENOBUFS:
		 return " No buffer space is available. ";

	case WSAENOTCONN:
		 return " The socket is not connected (connection-oriented sockets only). ";

	case WSAENOTSOCK:
		 return " The descriptor is not a socket. ";

	case WSAEOPNOTSUPP:
		 return " MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only receive operations. ";

	case WSAESHUTDOWN:
		 return " The socket has been shut down; it is not possible to sendto on a socket after shutdown has been invoked with how set to SD_SEND or SD_BOTH. ";

	case WSAEWOULDBLOCK:
		 return " The socket is marked as nonblocking and the requested operation would block. ";

	case WSAEMSGSIZE:
		 return " The socket is message oriented, and the message is larger than the maximum supported by the underlying transport. ";

	case WSAEHOSTUNREACH:
		 return " The remote host cannot be reached from this host at this time. ";

	case WSAECONNABORTED:
		 return " The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable. ";

	case WSAECONNRESET:
		 return " The virtual circuit was reset by the remote side executing a hard or abortive close. For UPD sockets, the remote host was unable to deliver a previously sent UDP datagram and responded with a \"Port Unreachable\" ICMP packet. The application should close the socket as it is no longer usable. ";

	case WSAEADDRNOTAVAIL:
		 return " The remote address is not a valid address, for example, ADDR_ANY. ";

	case WSAEAFNOSUPPORT:
		 return " Addresses in the specified family cannot be used with this socket. ";

	case WSAEDESTADDRREQ:
		 return " A destination address is required. ";

	case WSAENETUNREACH:
		 return " The network cannot be reached from this host at this time. ";

	case WSAETIMEDOUT:
		 return " The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetRecvfromErrString(const int eno)
{
	switch (eno)
	{
	case SOCKET_ERROR:
		 return "is returned, and a specific error code can be retrieved by calling WSAGetLastError.Error code Meaning ";

	case WSANOTINITIALISED:
		 return " A successful WSAStartup call must occur before using this function. ";

	case WSAENETDOWN:
		 return " The network subsystem has failed. ";

	case WSAEFAULT:
		 return " The buf or from parameters are not part of the user address space, or the fromlen parameter is too small to accommodate the peer address. ";

	case WSAEINTR:
		 return " The (blocking) call was canceled through WSACancelBlockingCall. ";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";

	case WSAEINVAL:
		 return " The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled, or (for byte stream-style sockets only) len was zero or negative. ";

	case WSAEISCONN:
		 return " The socket is connected. This function is not permitted with a connected socket, whether the socket is connection oriented or connectionless. ";

	case WSAENETRESET:
		 return " The connection has been broken due to the keep-alive activity detecting a failure while the operation was in progress. ";

	case WSAENOTSOCK:
		 return " The descriptor is not a socket. ";

	case WSAEOPNOTSUPP:
		 return " MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations. ";

	case WSAESHUTDOWN:
		 return " The socket has been shut down; it is not possible to recvfrom on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH. ";

	case WSAEWOULDBLOCK:
		 return " The socket is marked as nonblocking and the recvfrom operation would block. ";

	case WSAEMSGSIZE:
		 return " The message was too large to fit into the specified buffer and was truncated. ";

	case WSAETIMEDOUT:
		 return " The connection has been dropped, because of a network failure or because the system on the other end went down without notice. ";

	case WSAECONNRESET:
		 return " The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket; it is no longer usable. On a UDP-datagram socket this error indicates a previous send operation resulted in an ICMP Port Unreachable message. ";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetConnectErrString(const int eno)
{
	switch (eno)
	{
	case WSANOTINITIALISED:
		 return "A successful WSAStartup call must occur before using this function.";

	case WSAENETDOWN:
		 return " The network subsystem has failed. ";

	case WSAEADDRINUSE:
		 return " The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wildcard address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function. ";

	case WSAEINTR:
		 return " The blocking Windows Socket 1.1 call was canceled through WSACancelBlockingCall. ";

	case WSAEINPROGRESS:
		 return " A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";

	case WSAEALREADY:
		 return " A nonblocking connect call is in progress on the specified socket. ";

	case WSAEADDRNOTAVAIL:
		 return " The remote address is not a valid address (such as ADDR_ANY). ";

	case WSAEAFNOSUPPORT:
		 return " Addresses in the specified family cannot be used with this socket. ";

	case WSAECONNREFUSED:
		 return " The attempt to connect was forcefully rejected. ";

	case WSAEFAULT:
		 return " The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family. ";

	case WSAEINVAL:
		 return " The parameter s is a listening socket. ";

	case WSAEISCONN:
		 return " The socket is already connected (connection-oriented sockets only). ";

	case WSAENETUNREACH:
		 return " The network cannot be reached from this host at this time. ";

	case WSAENOBUFS:
		 return " No buffer space is available. The socket cannot be connected. ";

	case WSAENOTSOCK:
		 return " The descriptor is not a socket. ";

	case WSAETIMEDOUT:
		 return " Attempt to connect timed out without establishing a connection. ";

	case WSAEWOULDBLOCK:
		 return " The socket is marked as nonblocking and the connection cannot be completed immediately. ";

	case WSAEACCES:
		 return " Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled. ";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnCreateDirectoryErrString(const int eno)
{
	switch (eno)
	{
	case ERROR_ALREADY_EXISTS:
		 return "The specified directory already exists.";

	case ERROR_PATH_NOT_FOUND:
		 return "One or more intermediate directories do not exist; this function will "
			 "only create the final directory in the path. To create all intermediate "
			 "directories on the path, use the SHCreateDirectoryEx function.";

	default:
		 return OmnString("Unknown errno: ") << eno;
	}
	return "Should never come to this point";
}


OmnString OmnGetAcceptErrString(const int eno)
{
	// 
	// Not implemented yet
	//
	return "Not implemented yet";
}


OmnString OmnGetReadErrString(const int eno)
{
	// 
	// Not implemented yet
	//
	return "Not implemented yet";
}

#endif

