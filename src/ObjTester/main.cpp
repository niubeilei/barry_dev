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

#include "App/App.h"
#include "DataStore/StoreMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "ObjTester/ObjTestSuite.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
 

int 
main(int argc, char **argv)
{
	cout << "To start: " << endl;

	OmnApp theApp(argc, argv);
	try
	{

		OmnDataStoreMySQL::startMySQL("chen", "ding", "repository");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Test1", "Try", "Chen Ding");
	testMgr->addSuite(OmnObjTestSuite::getSuite());
	testMgr->start();

	cout << "Finished. " << testMgr->getStat() << endl;

	testMgr = 0;

	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 


