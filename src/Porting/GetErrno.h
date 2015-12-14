////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GetErrno.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_GetErrno_h
#define Omn_Porting_GetErrno_h

#include <errno.h>

class OmnErrType
{
public:
	enum E
	{
		eUnknown,
		eAccept,
		eSelect,
		eSocket,
		eSetSocketOpt,
		eBind,
		eSendto,
		eRecvfrom,
		eRead,
		eConnect,
		eSystem,
		eCreateDirectory
	};
};

#ifdef OMN_PLATFORM_UNIX
#define OMN_E_RESOURCE_N_A EWOULDBLOCK
inline int OmnGetErrno()
{
	return errno;
}

bool inline OmnPortUnreachableError(const int errcode)
{
	return false;
}


#elif OMN_PLATFORM_MICROSOFT
#define OMN_E_RESOURCE_N_A EDEADLOCK
#include <winsock2.h>
#include <windows.h>

inline int OmnGetErrno()
{
	return WSAGetLastError();
}


bool inline OmnPortUnreachableError(const int errcode)
{
	return errcode == WSAECONNRESET;
}


#endif
#endif

