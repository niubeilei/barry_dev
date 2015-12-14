////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestTcpApiU.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"

int TestTcpApiUsr(const char *IP, unsigned short port)
{
    OmnString rslt;
    char cmd[100];

    //
    // Prepare the command
    //
    sprintf(cmd, "TestAosTcpApi %s %d", IP, port);

	OmnTrace << "TestTcpApiUsr entry" << endl;	

    //
    // Call the function through the KAPI
    //
    if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
    {
        //
        // The KAPI call failed. Raise an alarm and return null.
        //
        OmnAlarm << "Failed to run KAPI: TestAosTcpApi" << enderr;
        return 0;
    }

	return 0;
}

