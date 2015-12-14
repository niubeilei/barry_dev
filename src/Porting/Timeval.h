////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timeval.h
// Description:
//	This is used to include the file for struct timeval. Microsoft
//  and Unix put the file differently.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Porting_Timeval_h
#define Omn_Porting_Timeval_h


#ifdef OMN_PLATFORM_UNIX
#include <sys/time.h>
#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#endif

#endif




