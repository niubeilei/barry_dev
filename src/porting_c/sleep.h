////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Sleep.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_porting_sleep_h
#define Omn_porting_sleep_h


#ifdef	OMN_PLATFORM_UNIX

#include <unistd.h>

static inline void aos_sleep(int seconds) {sleep(seconds);}
static inline void aos_ms_sleep(int ms) {usleep(ms * 1000);}


#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

// 
// Chen Ding, 01/17/2007
//
inline void aos_sleep(int seconds) {Sleep((DWORD)(seconds * 1000));}
inline void aos_ms_sleep(int ms) {Sleep((DWORD)ms);}

#endif

#endif

