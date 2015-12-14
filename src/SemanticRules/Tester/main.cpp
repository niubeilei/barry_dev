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
// 12/06/2007	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestMgr.h"
          
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h" 
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "SemanticRules/RuleMgr.h"
#include "SemanticRules/Tester/TestSuite.h"
#include "Semantics/SemanticsRuntime.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include "semanopr/arrayopr.h"
  
 
static void func();

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnStoreMgrSingleton());
		theApp.startSingleton(OmnNew AosSemanticsRuntimeSingleton());
		theApp.startSingleton(OmnNew AosRuleMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int aa[10];
	int value = 12;
	aa[0] = 10;
	aa[1] = value = 13;
	aos_array_modify(aa, 1, value), aa[2] = 2;
	// (__aos_array_modify(__FILE__, __LINE__, aa, 1), aa[1] = value);

	int a = aos_array_read(aa, 0); 
	int *b = &aos_array_read(aa, 1);
	cout << "========================== a:" << a << endl;
	cout << "========================== b:" << *b << endl;

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Tester", "Try", "Chen Ding");
	testMgr->addSuite(AosSemanticRuleTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

void func() {}
