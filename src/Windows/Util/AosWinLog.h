////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinLog.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOS_WIN_LOG_H__
#define __AOS_WIN_LOG_H__

#include "AosWinModules.h"

enum AosWinLogBuffer
{
	eAosWinLogNumLocalBuf = 8,
	eAosWinLogLocalBufSize = 1000
};

enum AosWinLogLevel
{
	eAosWinLogLevel_Start = 0,

	eAosWinLogLevel_NoLog,
	eAosWinLogLevel_Error,
	eAosWinLogLevel_Warning,
	eAosWinLogLevel_Minimum,
	eAosWinLogLevel_Production,
	eAosWinLogLevel_Engineering,
	eAosWinLogLevel_Debug,

	eAosWinLogLevel_End
};

typedef struct AosWinLog
{
	int level;
	char* name;
}AosWinLog_t, *PAosWinLog_t;

// Usage: AosWinMinLog( eAosWinMD_SSL, ("%s, %d", "this is a mini log", 1) );

#ifndef AosWinErrLog
#define AosWinErrLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Error, \
	module, AosWinLogGetMsg x )
#endif

#ifndef AosWinWarLog
#define AosWinWarLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Warning, \
	module, AosWinLogGetMsg x )
#endif

#ifndef AosWinMinLog
#define AosWinMinLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Minimum, \
	module, AosWinLogGetMsg x )
#endif

#ifndef AosWinProdLog
#define AosWinProdLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Production, \
	module, AosWinLogGetMsg x)
#endif

#ifndef AosWinEngLog
#define AosWinEngLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Engineering, \
	module, AosWinLogGetMsg x)
#endif

#ifndef AosWinDebugLog
#define AosWinDebugLog(module, x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Debug, \
	module, AosWinLogGetMsg x)
#endif

#ifndef AosWinDebug
#define AosWinDebug(x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Debug, \
	eAosWinMD_Platform, AosWinLogGetMsg x)
#endif

#ifndef AosWinTrace
#define AosWinTrace(x) \
	AosWinLogEntry(__FILE__, __LINE__, eAosWinLogLevel_Debug, \
	eAosWinMD_Tracer, AosWinLogGetMsg x)
#endif


extern AFX_API_EXPORT char *AosWinLogGetMsg(char *fmt, ...);
extern AFX_API_EXPORT int AosWinLogEntry(
	const char *file, 
	const int line, 
	int level, 
	int module, 
	char *msg);
#endif // __AOS_WIN_LOG_H__
