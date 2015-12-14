////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: main.cpp
//// Description:
////   
////
//// Modification History:
//// 11/17/2006   Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "KernelAPI/KernelAPI.h"
#include "KernelSimu/KernelSimu.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "Util1/TimeDriver.h"
#include "XmlParser/XmlItem.h"
		    
#include "TimeMgr/Tester/Tester.h"
			   

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
    }

    catch (const OmnExcept &e) 
	{
		OmnAlarm << "Failed to start the application: " 
   				<< e.toString() << enderr;											
		return 0;
	}
    // 
	//     // This is the application part
	//         
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("TimeMgr Testing", "Try", "Troy Jia");
	testMgr->addSuite(AosTorturerTimeMgrSuite::getSuite());
	
	cout << "Start Testing ..." << endl;
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
					 
//	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

