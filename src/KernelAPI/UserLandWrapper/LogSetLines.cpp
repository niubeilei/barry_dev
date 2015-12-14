////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogSetLines.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "LogSetLines.h"

bool AosLogSetLines(int numlines)
{
	OmnString rslt;
	char cmd[100];

	//
	//	Prepare the command
	//
	sprintf(cmd,"set log lines %d", numlines);

	//
	//	Call the function through the KAPI
	//
	if (!OmnCliProc::getSelf()->runCli(cmd,rslt))
	{
		OmnAlarm << "Failed to run KAPI: set_log_lines" << enderr;
		return false;
	}
	return true;
}

