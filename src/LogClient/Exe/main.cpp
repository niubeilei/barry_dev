////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 04/14/2009: Created by Sharon Shen
//////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"
#include "Client/LogClient.h"
#include "Client/LogEntry.h"
#include "Client/Ptrs.h"
#include "Util/String.h"

#include <iostream.h>
#include <stdio.h>


int main(int argc, char **argv)
{ 
	// OmnApp app(argc, argv);
	OmnAppPtr app = OmnApp::getSelf(argc, argv);
	AosStartLog();
	
	//Create logs.
	logClient->createCreateLog();
	logClient->createAddLogEntry();
	logClient->createAppStopped();

	while(1)
	{
		sleep(1);
	}
	AosStopLog();
	return 0;	
}
