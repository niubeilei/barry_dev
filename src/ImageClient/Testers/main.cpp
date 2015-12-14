////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is automatically generated by the TorturerGen facility.  
//
// Modification History:
// 3/19/2007: Created by TorturerGen facility
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "Porting/Sleep.h"
#include "Debug/Debug.h" 
#include "Porting/GetTime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "TransPest/Testers/TestSuite.h"
#include "TransPest/Testers/TransPestTester.h"
#include "Util/OmnNew.h"
  

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	const char * name = "config_norm.txt";
	OmnApp::setConfig(name);

	OmnTestMgr testMgr("Util/Tester", "Try", "Chen Ding");
	testMgr.addSuite(AosTestSuite::getSuite());
	cout << "Start Testing ..." << endl;

	
	testMgr.start();
	theApp.exitApp();
	return 0;
} 

 
