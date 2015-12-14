////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MutexU.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelAPI/UserLandWrapper/MutexU.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"



int aosMutexWrapper()
{
    OmnString rslt;
    char cmd[100];

    //
    // Prepare the command
    //
    sprintf(cmd, "mutex test");

    OmnTrace << "Mutex Test" << endl;

    //
    // Call the function through the KAPI
    //
    bool ret = OmnCliProc::getSelf()->runCli("mutex test", rslt);
	if (!ret)
    {
        //
        // The KAPI call failed. Raise an alarm and return null.
        //
        OmnAlarm << "Failed to run KAPI: " << rslt << enderr;
        return -1;
    }

    return 0;
}

