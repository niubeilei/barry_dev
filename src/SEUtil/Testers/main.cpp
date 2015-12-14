////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          

int 
main(int argc, char **argv)
{
	AosProgressMonitorPtr prog = OmnNew AosProgressMonitor();
	prog->start();

	prog->startEvent("IIL Manager", 5);
	AosIILMGr::getSelf();
	prog->startEvent("StorageEngine", 5);
	AosStorageEngine::getSelf();
	prog->stop();
}
