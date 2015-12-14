////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: GetHostName.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_GetHostName_h
#define Omn_Porting_getHostName_h

#ifdef OMN_PLATFORM_UNIX
#include <unistd.h>
#define OmnFailedToGetHostName -1

#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#define OmnFailedToGetHostName SOCKET_ERROR 
#endif 

inline int OmnGetHostName(char *buf, int length) 
{
	return gethostname(buf, length);
}

#endif

