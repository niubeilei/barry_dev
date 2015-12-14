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
#include "Debug/Debug.h" 
#include "KernelInterface/KernelInterface.h"
#include "KernelInterface/CliProc.h"
#include "KernelSimu/KernelSimu.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "PKCSTester/PkcsTestSuite.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"
  
 

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
 
		OmnDataStoreMySQL::startMySQL("root", "chen0318", "repository");
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
		theApp.startSingleton(OmnNew OmnKernelInterfaceSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " 
			<< e.toString() << enderr;
		return 0;
	}


	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("PKCSTester", "Try", "Chen Ding");
	testMgr->addSuite(OmnPkcsTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

 
