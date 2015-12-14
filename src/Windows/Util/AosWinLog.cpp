////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinLog.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AosWinLog.h"

static int sgAosWinLogLevel = eAosWinLogLevel_Debug;

static struct AosWinLog sgAosWinLog[eAosWinLogLevel_End] =
{
	{eAosWinLogLevel_Start, "Sta"},
	{eAosWinLogLevel_NoLog, "Nlg"},
	{eAosWinLogLevel_Error, "Err"},
	{eAosWinLogLevel_Warning,"War"},
	{eAosWinLogLevel_Minimum,"Min"},
	{eAosWinLogLevel_Production,"Pdc"},
	{eAosWinLogLevel_Engineering,"Eng"},
	{eAosWinLogLevel_Debug, "Dbg"},
};
// 
// The caller should never modify the contents of the buffer
// returned by this function.
//
AFX_API_EXPORT char *AosWinLogGetMsg(char *fmt, ...)
{
	static int  slBufIndex = 0;
	static char slBuffer[eAosWinLogNumLocalBuf][eAosWinLogLocalBufSize];
	unsigned int index = (slBufIndex++) & 0x07;

	va_list args;
	int n;

	va_start(args, fmt);
	n = vsprintf(slBuffer[index], fmt, args);
	va_end(args);

	return slBuffer[index];
}

AFX_API_EXPORT int AosWinLogEntry(
	const char *file, 
	const int line, 
	int level, 
	int module, 
	char *msg)
{
	char local[eAosWinLogLocalBufSize+100];
	
	if ( level <= eAosWinLogLevel_Start || level >= eAosWinLogLevel_End )
	{
		level = eAosWinLogLevel_Minimum;
	}

	if ( module <= eAosWinMD_Start || module >= eAosWinMD_End )
	{
		module = eAosWinMD_Start;
	}


	// 
	// Check the filters
	//
	if ( level > sgAosWinLogLevel )
	{
		return 0;
	}

	_snprintf( local, sizeof(local), "<%s:%d:%s>:  %s\n", file, line, sgAosWinLog[level].name, msg );

	OutputDebugString( local );
	return 0;
}