////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_porting_select_h
#define Omn_porting_select_h


//--------------------------------------------------------------
// Linux Portion
//--------------------------------------------------------------
#ifdef OMN_PLATFORM_UNIX

#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


static inline int aos_socket_select(
				int fileDescriptorCount, 
				fd_set *readFD,
				fd_set *writeFD,
				fd_set *exceptFD,
				struct timeval *timer)
{
	//
	// The return values are defined as:
	// > 0: 	The number of sockets that have been signaled
	//   0: 	Timed out
	//  -1: 	Error
	//
	return select(fileDescriptorCount, readFD, writeFD, exceptFD, timer);
}


//--------------------------------------------------------------
// Microsoft Portion
//--------------------------------------------------------------
#elif OMN_PLATFORM_MICROSOFT


/*
inline int
OmnSocketSelect(int fileDescriptorCount, 
				fd_set *readFD,
				fd_set *writeFD,
				fd_set *exceptFD,
				timeval *timer)
{
	//
	// This is a platform independent function for socket select
	// For Microsoft, the first parameter is ignored. 
	// The second parameter contains the bitmap for the sockets to be selected.
	// 
	return select(FD_SETSIZE, readFD, writeFD, exceptFD, timer);
}
*/


#endif
#endif

