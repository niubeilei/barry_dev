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

#ifndef Omn_Porting_Sleep_h
#define Omn_Porting_Sleep_h


#ifdef	OMN_PLATFORM_UNIX

#include <unistd.h>

inline void OmnSleep(int seconds) {sleep(seconds);}
inline void OmnSleepSec(int sec) {sleep(sec);}
inline void OmnMsSleep(int ms) {usleep(ms * 1000);}
inline void OmnUsSleep(int us) {usleep(us);}


#elif OMN_PLATFORM_MICROSOFT
#include <winsock2.h>
#include <windows.h>
#include <winbase.h>

// 
// Chen Ding, 01/17/2007
//
// inline void OmnSleep(int seconds) {Sleep((DWORD)(seconds << 10));}
inline void OmnSleep(int seconds) {Sleep((DWORD)(seconds * 1000));}
inline void OmnMsSleep(int ms) {Sleep((DWORD)ms);}

#endif

#endif

