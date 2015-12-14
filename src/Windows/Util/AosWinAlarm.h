////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinAlarm.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOSWINUTIL_AOSWINALARM_H__
#define __AOSWINUTIL_AOSWINALARM_H__

#include "AosWinModules.h"

enum AosWinAlarmPriv 
{
	eAosWinAlarmNameMaxLen = 20,
	eAosWinAlarmNumLocalBuf = 10,
	eAosWinAlarmLocalBufSize = 1000
};

enum AosWinAlarmLevel
{
	eAosWinAlarmLevel_Start = 0,

	eAosWinAlarmLevel_Warn,
	eAosWinAlarmLevel_Alarm,
	eAosWinAlarmLevel_ProgErr,
	eAosWinAlarmLevel_Fatal,

	eAosWinAlarmLevel_End
};

enum AosWinModuleAlarmStarts
{
	eAosWinAlarm_PlatformStart = 10000,

	eAosWinAlarm_TcpProxyStart  = 11000,
	eAosWinAlarm_SSLStart 		= 12000,
	eAosWinAlarm_AppProcStart 	= 13000,
};


enum AosWinAlarmIDs
{
	eAosWinAlarmID_Start = 0,

	eAosWinAlarmID_InvalidAlarmId,

	eAosWinAlarmID_CliErr,
	eAosWinAlarmID_FailedToConnect,
	eAosWinAlarmID_FailedToOpenFile,
	eAosWinAlarmID_FailedToReadFile,
	eAosWinAlarmID_FailedToSaveConfig,
	eAosWinAlarmID_FailedToWriteConfig,
	eAosWinAlarmID_General,
	eAosWinAlarmID_MemErr,
	eAosWinAlarmID_NotImplemented,
	eAosWinAlarmID_NullPointer,
	eAosWinAlarmID_OutOfBound,
	eAosWinAlarmID_Platform,
	eAosWinAlarmID_ProgErr,
	eAosWinAlarmID_TestErr,
	eAosWinAlarmID_UtilCommErr,

	eAosAlarmID_End
};

#ifndef AosWinWarn
#define AosWinWarn(xxx, yyy, fmt) 								\
	AosWinRaiseAlarm(__FILE__, __LINE__, 						\
	eAosWinAlarmLevel_Warn, (xxx), (yyy), AosWinAlarmGetErrMsg fmt)
#endif 

#ifndef AosWinAlarm
#define AosWinAlarm(xxx, yyy, fmt) 								\
	AosWinRaiseAlarm(__FILE__, __LINE__, 						\
	eAosWinAlarmLevel_Alarm, (xxx), (yyy), AosWinAlarmGetErrMsg fmt)
#endif

#ifndef AosWinProgErr
#define AosWinProgErr(xxx, yyy, fmt) 							\
	AosWinRaiseAlarm(__FILE__, __LINE__, 						\
	eAosWinAlarmLevel_ProgErr, (xxx), (yyy), AosWinAlarmGetErrMsg fmt)
#endif

#ifndef AosWinProgErrD
#define AosWinProgErrD(fmt)										\
	AosWinRaiseAlarm(__FILE__, __LINE__, 						\
	eAosWinAlarmLevel_ProgErr, 									\
	eAosWinAlarm_Platform, eAosWinAlarm_ProgErr, AosWinAlarmGetErrMsg fmt)
#endif

#ifndef AosWinFatal
#define AosWinFatal(xxx, yyy, fmt) 								\
	AosWinRaiseAlarm(__FILE__, __LINE__, 						\
	eAosWinAlarmLevel_Fatal, (xxx), (yyy), AosWinAlarmGetErrMsg fmt)
#endif

#ifndef AowWinNotImplementedYet
#define AowWinNotImplementedYet AosWinRaiseAlarm(__FILE__, __LINE__,\
	eAosWinAlarmLevel_Alarm, eAosMD_Platform, 						\
	eAosWinAlarm_NotImplemented, "Not implemented yet")
#endif

#ifndef AosWinAssert0
#define AosWinAssert0(cond)										\
	if (!(cond))												\
	{															\
		AosWinRaiseAlarm(__FILE__, __LINE__,					\
		eAosWinAlarmLevel_ProgErr, 								\
		eAosWinAlarmID_Platform, eAosWinAlarm_ProgErr, "");		\
		return;													\
	}
#endif

#ifndef AosWinAssert1
#define AosWinAssert1(cond)										\
	if (!(cond))												\
	{															\
		AosWinRaiseAlarm(__FILE__, __LINE__, 					\
		eAosWinAlarmLevel_ProgErr, 								\
		eAosWinAlarmID_Platform, eAosWinAlarm_ProgErr, "");		\
		return -eAosWinAlarm_ProgErr;							\
	}
#endif

#ifndef AosWinAssert2
#define AosWinAssert2(cond, format)								\
	if (!(cond))												\
	{															\
		return AosWinRaiseAlarm(__FILE__, __LINE__, 			\
		eAosWinAlarmLevel_ProgErr, 								\
		eAosWinAlarmID_Platform, eAosWinAlarm_ProgErr,			\
		AosWinAlarmGetErrMsg format);							\
	}
#endif

#ifndef AosWinAssert3
#define AosWinAssert3(cond, errcode, format)					\
	if (!(cond))												\
	{															\
		return AosWinRaiseAlarm(__FILE__, __LINE__, 			\
		eAosWinAlarmLevel_ProgErr, 								\
		eAosWinAlarmID_Platform, (errcode),						\
		AosWinAlarmGetErrMsg format);							\
	}
#endif

extern AFX_API_EXPORT char *AosWinAlarmGetErrMsg(const char *fmt, ...);
extern AFX_API_EXPORT int AosWinRaiseAlarm(
	const char *file, 
	const int line, 
	int level, 
	int module, 
	int id,
	const char *msg);
#endif
