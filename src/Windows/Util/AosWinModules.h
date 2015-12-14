////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AosWinModules.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOSWINUTIL_AOSWINMODULES_H__
#define __AOSWINUTIL_AOSWINMODULES_H__

enum
{
	eAosWinModuleNameMaxLen = 30
};

enum AosWinModules
{
	eAosWinMD_Start = 0,

	eAosWinMD_Platform,
	eAosWinMD_Tracer,
	eAosWinMD_TcpProxy,
	eAosWinMD_SSL,
	eAosWinMD_AppProc,

	eAosWinMD_End
};

extern char *AosWinModuleGetName(int code);
#endif //#ifndef __AOSWINUTIL_AOSWINMODULES_H__
