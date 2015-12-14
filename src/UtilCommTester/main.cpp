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

#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "ScvsTester/ScvsTestSuite.h"
#include "SingletonClass/SingletonMgr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "UtilCommTester/UtilCommTestSuite.h"

  
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
	testMgr->addSuite(OmnUtilCommTestSuite::getSuite(argc, argv));
	testMgr->start();

	cout << "Finished. " << testMgr->getStat() << endl;

	timeval tp;
	OmnGetTimeOfDay(&tp); 
	float v = tp.tv_sec + (tp.tv_usec / 1000000.0);
	cout << "Starting: " << tp.tv_sec << ":" << tp.tv_usec << ":" << v << endl;

	OmnTestMgrPtr m11, m12, m13, m14, m15, m16, m17, m18, m19;
	for (int i=0; i<1000000; i++)
	{
		OmnTestMgrPtr m = OmnNew OmnTestMgr("Test1", "Try", "Chen Ding");
		OmnTestMgrPtr m1 = m;
		OmnTestMgrPtr m2;
		m2 = m1;
		m11 = m;
		m12 = m;
		m13 = m;
		m14 = m;
		m15 = m;
		m16 = m;
		m17 = m;
		m18 = m;
		m19 = m;
	}

	cout << "Finished. " << testMgr->getStat() << endl;
	OmnGetTimeOfDay(&tp);
	v = tp.tv_sec + (tp.tv_usec / 1000000.0);
	cout << "Ending: " << tp.tv_sec << ":" << tp.tv_usec << ":" << v << endl;

	return 0;
	testMgr = 0;

	theApp.appLoop();
	theApp.exitApp();
	return 0;              
} 
 

