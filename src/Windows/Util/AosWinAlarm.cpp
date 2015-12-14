////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinAlarm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <stdafx.h>
#include "AosWinAlarm.h"
#include "AosWinModules.h"

char *AosWinAlarmLevelNames[eAosWinAlarmLevel_End+1] = 
{
	"Level-Start", 
	"Warn",
	"Alarm", 
	"ProgramError",
	"Fatal",
	"Level-End"
};

static char *sgAosWinAlarmErrHeader[eAosWinAlarmLevel_End+1] = 
{
	"",
	"********** Warn *********",
	"********** Alarm *********",
	"********** Program Error *********",
	"********** Fatal *********",
	""
};

// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
char *AosWinAlarmGetErrMsg(const char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosWinAlarmNumLocalBuf][eAosWinAlarmLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;
	int n;

	va_start( args, fmt );
	n = vsprintf( slBuffer[index], fmt, args );
	va_end( args );

	return slBuffer[index];
}


int AosWinRaiseAlarm(
	const char *file, 
	const int line, 
	int level, 
	int module, 
	int id,
	const char *msg)
{
	char c = '\0';
	int changed = 0;
	char local[eAosWinAlarmLocalBufSize+100];

	if ( level <= eAosWinAlarmLevel_Start || level >= eAosWinAlarmLevel_End )
	{
		level = eAosWinAlarmLevel_ProgErr;
	}

	if ( module <= eAosWinMD_Start || module >= eAosWinMD_End )
	{
		module = eAosWinMD_Start;
	}

	if (id <= eAosWinAlarmID_Start || id >= eAosAlarmID_End)
	{
		id = eAosWinAlarmID_InvalidAlarmId;
	}

	if ( strlen(msg) >= eAosWinAlarmLocalBufSize )
	{
		char *tmp = (char *)msg;
		changed = 1;
		c = tmp[eAosWinAlarmLocalBufSize];
		tmp[eAosWinAlarmLocalBufSize] = 0;
	}

	_snprintf(local,
		sizeof(local),
		"%s"
		"\nLocation: <%s:%d>"
		"\nLevel: %d"
		"\nModule: %s"
		"\nAlarm ID: %d"
		"\nError Message: %s"
		"\n**********************\n", 
		sgAosWinAlarmErrHeader[level], file, line, level, 
		AosWinModuleGetName(module), id, msg);

	if (changed)
	{
		char *tmp = (char *)msg;
		tmp[eAosWinAlarmLocalBufSize] = c;
	}

	OutputDebugString(local);
	return -id;
}
