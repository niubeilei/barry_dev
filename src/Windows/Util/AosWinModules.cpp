////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinModules.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AosWinModules.h"

static char sgAosWinModuleNames[eAosWinMD_End+1][eAosWinModuleNameMaxLen]={
	{"Start"},
	{"Platform"},
	{"Tracer"},
	{"TcpProxy"},
	{"SSL"},
	{"AppProc"},
	{"End"}
};

char *AosWinModuleGetName(int code)
{
	if (code <= eAosWinMD_Start || code >= eAosWinMD_End)
	{
		code = eAosWinMD_Start;
	}
	return sgAosWinModuleNames[code];
}