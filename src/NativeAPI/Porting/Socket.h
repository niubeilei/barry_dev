////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Socket.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Socket_h
#define Omn_Porting_Socket_h

#include "Debug/ErrId.h"
#include "Porting/GetErrno.h"


//-------------------------------------------------------------
// Linux Portion
//-------------------------------------------------------------
#ifdef	OMN_PLATFORM_UNIX

#include <sys/types.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define OmnOpenSocketRefused	ECONNREFUSED
#define SOCKET int


typedef struct iovec OmnIoBuff;

extern int OmnCreateTcpSocket(const char *file, const int line);
extern int OmnCreateUdpSocket(const char *file, const int line);
extern int OmnCreateUnixSocket(const char *file, const int line);
extern int OmnCreateEpollSocket(const char *file, const int line, const int max_conns);
extern int OmnAcceptSocket(const char *file, const int line, const int server_sock, struct sockaddr * cli_addr, socklen_t *clilen);
extern int OmnCloseSocket(const char*file, const int line, int s);

inline int 
OmnWriteSock(int fd, const OmnIoBuff *buff, int len)
{ 
	//return ::write (fd, buff, len);
	return ::writev (fd, buff, len);
}

inline int 
OmnSendToSock(int fd, const char *buff, int len,bool isBlocking)
{ 
	return isBlocking?(::write (fd, buff, len)):(::send(fd,buff,len,MSG_DONTWAIT));
}
inline ssize_t 
OmnReadSock(int sock, void *buf, int nbytes)
{
	return ::read(sock, buf, nbytes);
}

inline bool OmnIsTcpConnectionShutDown(const int returnedValue)
{
	//
	// On UNIX, we can't tell from recv()
	//
	return false;
}

inline int OmnRecvFrom(const SOCKET sock,
						char *data,
						const int dataLength,
						sockaddr *addr,
						int *addrLen)
{
	int flags = 0;
	return recvfrom(sock, data, dataLength, flags, addr, (socklen_t*)addrLen);
}
//-------------------------------------------------------------
// Microsoft Portion
//-------------------------------------------------------------
#elif OMN_PLATFORM_MICROSOFT

#include <winsock2.h>
#include <windef.h>

#define	OmnOpenSocketRefused	WSAECONNREFUSED

inline int	
OmnCloseSocket(SOCKET s) 
{ 
	return ::closesocket(s);
}

inline int  
OmnWriteSock(SOCKET sock, const char FAR *buf, int len) 
{
	return ::send(sock, buf, len, 0);
}

inline int  
OmnReadSock(SOCKET sock, char FAR* buf, int len)
{
	return ::recv(sock, buf, len, 0);
}

inline bool OmnIsTcpConnectionShutDown(const int returnedValue)
{
	//
	// In Microsoft, when calling recv(...) and it returns 0, it means
	// the remote side has gracefully shut down the connection. In this
	// case, this function returns true. Otherwise, it returns false.
	//
	return (returnedValue == 0);
}


inline int OmnRecvFrom(const SOCKET sock,
						char *data,
						const int dataLength,
						sockaddr *addr,
						int *addrLen)
{
	int flags = 0;
	return recvfrom(sock, data, dataLength, flags, addr, addrLen);
}

#endif

inline int OmnSendTo(const SOCKET sock, 
					 const char * const data,
					 const int length,
					 const struct sockaddr *addr,
					 const int addrLen)
{
	//
	// This function is normally used by UDP to send a data to 
	// the specified address.
	//
	int flags = 0;
	return ::sendto(sock, data, length, flags, addr, addrLen);
}


// extern OmnErrId::E OmnCheckReadSocketError(const int errcode);
// extern OmnErrId::E OmnCheckWriteSocketError(const int errcode);
// extern OmnErrId::E OmnCheckSocketSelectError(const int errcode);
// extern OmnErrId::E OmnCheckRecvFromError(const int errcode);
// extern OmnErrId::E OmnCheckSendToError(const int errcode);

#endif

