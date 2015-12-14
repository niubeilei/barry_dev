////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//   
//
// Modification History:
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "Book/Testers/BookTestSuite.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Random/RandomUtil.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
  
 
int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
		OmnDataStoreMySQL::startMySQL("root", "chen0318",
			"192.168.1.144", 3306, "torturer");

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
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Book/Tester", "Try", "Chen Ding");
	testMgr->addSuite(AosBookTestSuite::getSuite());
	OmnRandom rr;

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

