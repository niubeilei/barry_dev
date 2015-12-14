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

#ifndef Omn_porting_getErrno_h
#define Omn_porting_getErrno_h

#include <errno.h>


#ifdef OMN_PLATFORM_UNIX
#define OMN_E_RESOURCE_N_A EWOULDBLOCK
static inline int aos_get_errno()
{
	return errno;
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

