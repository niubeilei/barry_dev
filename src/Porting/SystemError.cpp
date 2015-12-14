////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemError.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

// 
// This file is no longer useful. 
//
#if 0

#include "Porting/SystemError.h"

#include "Porting/Socket.h"

#ifdef OMN_PLATFORM_UNIX
#include <unistd.h>
#endif

OmnString	
OmnSystemError::translateSystemError(const int errcode)
{
#ifdef OMN_PLATFORM_UNIX
	switch (errcode)
	{
    case E2BIG:
		 return "Arg list too long";

    case EACCES:
		 return "Permission denied";

    case EAGAIN:
		 return "Resource temporarily unavailable";

    case EBADF: 
		 return "Bad file descriptor";

    case EBADMSG:
		 return "Bad message";

    case EBUSY: 
		 return "Resource busy";

    case ECANCELED:
		 return "Operation canceled";

    case ECHILD:
		 return "No child processes";

    case EDEADLK: 
		 return "Resource deadlock avoided";

    case EDOM:  
		 return "Domain error";

    case EEXIST:
		 return "File exists";

    case EFAULT:
		 return "Bad address";

    case EFBIG: 
		 return "File too large";

    case EINPROGRESS:
		 return "Operation in progress";

    case EINTR: 
		 return "Interrupted function call";

    case EINVAL:
		 return "Invalid argument";

    case EIO:   
		 return "Input/output error";

    case EISDIR:
		 return "Is a directory";

    case EMFILE:
		 return "Too many open files";

    case EMLINK:
		 return "Too many links";

    case EMSGSIZE:
		 return "Inappropriate message buffer length";

    case ENAMETOOLONG:
		 return "Filename too long";

    case ENFILE:
		 return "Too many open files in system";

    case ENODEV:
		 return "No such device";

    case ENOENT:
		 return "No such file or directory";

    case ENOEXEC:
		 return "Exec format error";

    case ENOLCK:
		 return "No locks available";

    case ENOMEM:
		 return "Not enough space";

    case ENOSPC:
		 return "No space left on device";

    case ENOSYS:
		 return "Function not implemented";

    case ENOTDIR:
		 return "Not a directory";

    case ENOTEMPTY:
		 return "Directory not empty";

    case ENOTSUP:
		 return "Not supported";

    case ENOTTY:
		 return "Inappropriate I/O control operation";

    case ENXIO: 
		 return "No such device or address";

    case EPERM: 
		 return "Operation not permitted";

    case EPIPE: 
		 return "Broken pipe";

    case ERANGE:
		 return "Result too large";

    case EROFS: 
		 return "Read-only file system";

    case ESPIPE:
		 return "Invalid seek";

    case ESRCH: 
		 return "No such process";

    case ETIMEDOUT:
		 return "Operation timed out";

    case EXDEV: 
		 return "Improper link";

	default :
		 return OmnString("Unrecognized error code: ")
				<<  OmnStrUtil::itoa(errcode);
	}

#elif OMN_PLATFORM_MICROSOFT


	//
	// This is the part for Microsoft
	//
	switch (errcode)
	{
	case WSAENETDOWN:
		 return "WSAENETDOWN: The network subsystem has failed.";

	case WSAEACCES:
		 return "WSAEACCES: The requested address is a broadcast address, but the "
			 "appropriate flag was not set. Call setsockopt with the SO_BROADCAST "
			 "parameter to allow the use of the broadcast address.";

	case WSAEINVAL:
		 return "WSAEINVAL: An unknown flag was specified, or MSG_OOB was specified "
			 "for a socket with SO_OOBINLINE enabled.";

	case WSAEINTR:
		 return "WSAEINTR: A blocking Windows Sockets 1.1 call was canceled through "
			 "WSACancelBlockingCall.";

	case WSAEINPROGRESS:
		 return "WSAEINPROGRESS: A blocking Windows Sockets 1.1 call is in progress, "
			 "or the service provider is still processing a callback function.";

	case WSAEFAULT:
		 return "WSAEFAULT: The buf or to parameters are not part of the user address "
			 "space, or the tolen parameter is too small.";

	case WSAENETRESET:
		 return "WSAENETRESET: The connection has been broken due to keep-alive "
			 "activity detecting a failure while the operation was in progress.";

	case WSAENOBUFS:
		 return "USAENOBUFS: No buffer space is available.";

	case WSAENOTCONN:
		 return "WSAENOTCONN: The socket is not connected (connection-oriented sockets "
			 "only).";

	case WSAENOTSOCK:
		 return "WSAENOTSOCK: The descriptor is not a socket.";

	case WSAEOPNOTSUPP:
		 return "WSAEOPNOTSUPP: MSG_OOB was specified, but the socket is not "
			 "stream-style such as type SOCK_STREAM, OOB data is not supported in "
			 "the communication domain associated with this socket, or the socket "
			 "is unidirectional and supports only receive operations.";

	case WSAESHUTDOWN:
		 return "WSAESHUTDOWN: The socket has been shut down; it is not possible to "
			 "sendto on a socket after shutdown has been invoked with how set to "
			 "SD_SEND or SD_BOTH.";

	case WSAEWOULDBLOCK:
		 return "WSAEWOULDBLOCK: The socket is marked as nonblocking and the requested "
			 "operation would block.";

	case WSAEMSGSIZE:
		 return "WSAEMSGSIZE: The socket is message oriented, and the message is "
			 "larger than the maximum supported by the underlying transport.";

	case WSAEHOSTUNREACH:
		 return "WSAEHOSTUNREACH: The remote host cannot be reached from this host at "
			 "this time.";

	case WSAECONNABORTED:
		 return "WSAECONNABORTED: The virtual circuit was terminated due to a time-out "
			 "or other failure. The application should close the socket as it is no "
			 "longer usable.";

	case WSAECONNRESET:
		 return "WSAECONNRESET:The virtual circuit was reset by the remote side "
			 "executing a hard or abortive close. For UPD sockets, the remote host was "
			 "unable to deliver a previously sent UDP datagram and responded with a "
			 "\"Port Unreachable\" ICMP packet. The application should close the socket "
			 "as it is no longer usable.";

	case WSAEADDRNOTAVAIL:
		 return "WSAEADDRNOTAVAIL: The requested address is not valid in its context. "
			 "Normally results from an attempt to bind to an address that is not valid for "
			 "the local machine. This can also result from connect, sendto, WSAConnect, "
			 "WSAJoinLeaf, or WSASendTo when the remote address or port is not valid for "
			 "a remote machine (e.g. address or port 0)";

	case WSAEAFNOSUPPORT:
		 return "WSAEAFNOSUPPORT: Addresses in the specified family cannot be used with " 
			 "this socket.";

	case WSAEDESTADDRREQ:
		 return "WSAEDESTADDRREQ: A destination address is required.";

	case WSAENETUNREACH:
		 return "WSAENETUNREACH: The network cannot be reached from this host at this "
			 "time.";

	case WSAETIMEDOUT:
		 return "WSAETIMEDOUT: The connection has been dropped, because of a network "
			 "failure or because the system on the other end went down without notice.";

	case WSANOTINITIALISED:
		 return "WSANOTINITIALISED: A successful WSAStartup call must occur"
			 " before using this function.";

	case WSAEADDRINUSE:
		 return "WSAEADDRINUSE: A process on the machine is already bound to the "
			 "same fully-qualified address and the socket has not been marked to "
			 "allow address re-use with SO_REUSEADDR. For example, IP address and "
			 "port are bound in the af_inet case) . (See the SO_REUSEADDR socket "
			 "option under setsockopt.)";


	default:
		 return OmnString("Error code: ")
				<< errcode << " not found!";
	}
#endif
}

bool 
OmnSystemError::isSelectErrorRecoverable(const int errcode)
{
	//
	// When select(...) fails, we need to determine whether it should
	// continue trying or quit. Depending on which platform the program 
	// runs, the processing is different.
	//
#ifdef OMN_PLTFORM_UNIX
	// EBADF: invalid file descriptor, not recoverable
	// EINTR: interrupt, should give it another try
	// EINVAL: n is negative, not recoverable
	// ENOMEM: no memory for internal tables, not recoverable

	if ( errcode == EINTR )
	{
		return true;
	}
	else
	{
		return false;
	}
#elif OMN_PLATFORM_MICROSOFT
	//
	// It will return the following
	// error codes for select(...). Refer to the above member function
	// for their meaning.
	//
	// WSANOTINITIALISED: not recoverable
	// WSAEFAULT: should give it another try
	// WSAENETDOWN: should give it another try
	// WSAEINVAL: not recoverable
	// WSAEINTR: should give it another try
	// WSAEINPROGRESS: should give it another try
	// WSAENOTSOCK: not recoverable
	//
	if (errcode == WSAEFAULT ||
		errcode == WSAENETDOWN ||
		errcode == WSAEINTR ||
		errcode == WSAEINPROGRESS)
	{
		return true;
	}
	else
	{
		return false;
	}
#endif

	return false;
}


OmnString
OmnSystemError::connectError(const int errcode)
{
#ifdef OMN_PLATFORM_UNIX
	switch (errcode)
	{
    case EBADF:
		 return "The file descriptor is not a valid index in the descriptor table.";

    case EFAULT:
		 return "The socket structure address is outside the user's address space.";

    case ENOTSOCK:
         return "The file descriptor is not associated with a socket.";

    case EISCONN:
         return "The socket is already connected.";

    case ECONNREFUSED:
         return "No one listening on the remote address.";

    case ETIMEDOUT:
         return "Timeout  while attempting connection. The server may be too busy to accept new "
              "connections. Note that for IP sockets the timeout may  be  very  long  "
			  "when  syncookies  are enabled on the server.";

    case ENETUNREACH:
         return "Network is unreachable.";

    case EADDRINUSE:
         return "Local address is already in use.";

    case EINPROGRESS:
         return "The  socket is non-blocking and the connection cannot be completed immediately.  "
			  "It is possible to select(2) or poll(2) for completion by selecting the socket  "
			  "for  writing. After  select  indicates writability, use getsockopt(2) to read "
			  "the SO_ERROR option at level SOL_SOCKET to determine whether  connect  completed "
			  "successfully  (SO_ERROR  is zero)  or  unsuccessfully  (SO_ERROR  is  one  of  the  "
			  "usual error codes listed here, explaining the reason for the failure).";

    case EALREADY:
         return "The socket is non-blocking and a previous connection attempt has  not  yet "
			  "been  completed.";

    case EAGAIN:
		 return "No more free local ports or insufficient entries in the routing cache. For "
			  "PF_INET see the net.ipv4.ip_local_port_range sysctl in ip(7) on how  to  "
			  "increase  the  number  of local ports.";

    case EAFNOSUPPORT:
         return "The passed address did not have the correct address family in its sa_family field.";

    case EACCES:
	case EPERM:
         return "The  user  tried to connect to a broadcast address without having the socket "
			  "broadcast flag enabled or the connection request failed because of a local "
			  "firewall rule.";

	default:
		 return OmnString("Unrecognized error code: ") << OmnStrUtil::itoa(errcode);
	}
#elif OMN_PLATFORM_MICROSOFT

	return "Not implemented!";
#endif
}


OmnString
OmnSystemError::writeError(const int errcode, OmnErrId::E &uniErrcode)
{
#ifdef OMN_PLATFORM_UNIX

	switch (errcode)
	{
    case EBADF:
		 uniErrcode = OmnErrId::eCommInvalidSock;
		 return "fd is not a valid file descriptor or is not open for writing.";

    //case EINVA:
	//	 return "fd is attached to an object which is unsuitable for writing.";

    //case EFAUL:
	//	 return "buf is outside your accessible address space.";

    case EFBIG:
		 uniErrcode = OmnErrId::eCommPacketTooBig;
		 return "An  attempt  was  made to write a file that exceeds the "
			  "implementation-defined maximum file size or the process' file size "
			  "limit, or to write at a  position  past  than  the maximum allowed offset.";

    case EPIPE:
		 uniErrcode = OmnErrId::eCommRemoteClosed;
		 return "fd  is  connected  to a pipe or socket whose reading end is closed.  "
			  "When this happens the writing process will receive a SIGPIPE signal; "
			  "if it catches,  blocks  or  ignores this the error EPIPE is returned.";

    //case EAGAI:
	//	 return "Non-blocking I/O has been selected using O_NONBLOCK and the write would block.";

    case EINTR:
		 uniErrcode = OmnErrId::eCommWriteInterrupt;
		 return "The call was interrupted by a signal before any data was written.";

    //case ENOSP:
	//	 return "The device containing the file referred to by fd has no room for the data.";

    case EIO  :
		 uniErrcode = OmnErrId::eCommLowLevelError;
		 return "A low-level I/O error occurred while modifying the inode.";
	
	default:
		 uniErrcode = OmnErrId::eCommUnknown;
		 return OmnString("Unrecognized errcode: ") << OmnStrUtil::itoa(errcode);
	}
#elif OMN_PLATFORM_MICROSOFT

	return "Not implemented!";
#endif
}


/*
OmnString
OmnSystemError::readFileError(const int errcode)
{
	switch (errcode)
	{
    case EINTR:
		 return "The call was interrupted by a signal before any data was read.";

    case EAGAIN:
		 return "Non-blocking I/O has been selected using O_NONBLOCK  and  no  data  was"
              	" immediately available for reading.";

    case EIO:
		 return "I/O  error. This will happen for example when the process is in a back"
              "ground process group, tries to  read  from  its  controlling  tty,  and"
              " either  it  is  ignoring  or  blocking  SIGTTIN or its process group is"
              " orphaned.  It may also occur when there is a low-level I/O error  while"
              " reading from a disk or tape.";

    case EISDIR:
		 return "fd refers to a directory.";

    case EBADF:
		 return "fd is not a valid file descriptor or is not open for reading.";

    case EINVAL:
		 return "fd is attached to an object which is unsuitable for reading.";

    case EFAULT:
		 return "buf is outside your accessible address space.";

	default:
		 return OmnString("Unknown errno: ") << errcode;
	}
}
*/

#endif

