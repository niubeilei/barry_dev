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
// Modification History:
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "GICs/AllGics.h"
#include "GICs/Testers/TestSuite.h"
#include "HtmlServer/HtmlReqProc.h"
#include "HtmlServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEModules/SeRundata.h"
#include "SmartDoc/SmartDoc.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SiteMgr/SiteMgr.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/MemMgr.h"
#include "util2/global_data.h"


#include <stdlib.h>

int gAosLogLevel = 1;


int 
main(int argc, char **argv)
{
	map<OmnString, int> mymap;
	mymap.insert(pair<OmnString, int>("hello",4));
	cout << mymap["hello"] << endl;
	aos_global_data_init();
	bool configflag = false;

	int tries = 100;
	int index = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index++;
			continue;
		}

		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			configflag = true;
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	if(!configflag)
	{
		const char * name = "config_norm.txt";
		OmnApp::setConfig(name);
	}
	OmnApp app(argc, argv);
	AosXmlTagPtr appConfig = app.getAppConfig();

	AosSengAdmin::getSelf();

	OmnString test = app.getAppConfig()->getAttrStr("tester");

	if (tries < 1) tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("GICs", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosGicsTestSuite::getSuite(test));
	
	cout << "Start Testing ..." << endl;
	
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	testMgr = 0;

	while (1)
	{
		OmnSleep(10000);
	}

	app.exitApp();
	return 0;
} 

