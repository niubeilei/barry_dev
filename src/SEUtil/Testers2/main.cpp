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
#include "Porting/Sleep.h"
#include "SEUtil/ProgressMonitor.h"
#include "SEUtil/Ptrs.h"
#include "StorageEngine/StorageEngineMgr.h"

int 
main(int argc, char **argv)
{
	cout << "1" << endl;
	AosProgressMonitorPtr prog = OmnNew AosProgressMonitor();
	prog->start();

	cout << "2" << endl;
	prog->startEvent("IIL Manager", 5);
	OmnSleep(2);

	cout << "3" << endl;
	prog->startEvent("StorageEngine", 5);
	OmnSleep(4);

	cout << "4" << endl;
	prog->startEvent("obj manager", 5);
	OmnSleep(7);

	cout << "5" << endl;
	prog->stop();
}
