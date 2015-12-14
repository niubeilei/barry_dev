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
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestMgr.h"
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "SmartObj/SlotMgr/Tester/TestSuite.h" 
#include "XmlUtil/XmlTag.h"

int gAosLogLevel = 1;

int 
main(int argc, char **argv)
{
	int tries = 0;
	int index = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
        {
            OmnApp::setConfig(argv[index+1]);
            index += 2;
            continue;
        }

		if (strcmp(argv[index], "-reset") == 0)
		{
			OmnApp::getAppConfig()->setAttr("needReset", "true");
			index++;
			continue;
		}

		index++;
	}

	OmnApp theApp(argc, argv);
	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SlotMgr/Tester", "Try", "Lynch");
	testMgr->setTries(tries);

	testMgr->addSuite(AosSlotMgrTestSuite::getSuite(theApp.getAppConfig()));

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 	theApp.exitApp();
	return 0;
} 
