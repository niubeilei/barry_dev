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
          
#include "Alarm/AlarmMgr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h" 
#include "MySQL/Ptrs.h"
#include "NMS/HouseKp.h"
#include "NMS/Ptrs.h"
#include "Porting/GetTime.h"
#include "Porting/Process.h"
#include "SearchEngine/WordMgr.h"
#include "SearchEngine/IILMgr.h"
#include "SearchEngine/IIL.h"
#include "SearchEngine/DocServer.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/SeXmlParser.h"
#include "SETorturer/TestSuite.h"
#include "SingletonClass/SingletonMgr.h"    
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
  

AosXmlTagPtr gAosAppConfig;

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	try
	{
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}

	int index = 1;
	int tries = 100;
	int rport = 5565;
	OmnString raddr = "127.0.0.1";
	int max_alarms = 10;
	bool restart = false;
	int createthrds = 0, totalcreate = 0;
	int querythrds = 0, totalquery = 0;
	int modthrds = 0, totalmod = 0;
	int delthrds = 0, totaldel = 0;

	while (index < argc)
	{
		if (strcmp(argv[index], "-restart") == 0 && index < argc)
		{
			restart = true;
			index++;
			continue;
		}

		if (strcmp(argv[index], "-n") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-createthrds") == 0 && index < argc-1)
		{
			createthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalcreate") == 0 && index < argc-1)
		{
			totalcreate = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-delthrds") == 0 && index < argc-1)
		{
			delthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totaldel") == 0 && index < argc-1)
		{
			totaldel = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-modthrds") == 0 && index < argc-1)
		{
			modthrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalmod") == 0 && index < argc-1)
		{
			totalmod = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-querythrds") == 0 && index < argc-1)
		{
			querythrds = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-totalquery") == 0 && index < argc-1)
		{
			totalquery = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-wn") == 0 && index < argc-1)
		{
			max_alarms = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rport") == 0 && index < argc-1)
		{
			rport = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-raddr") == 0 && index < argc-1)
		{
			raddr = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-f") == 0 && index < argc-1)
		{
			// Reading the config
			OmnFile file(argv[index+1], OmnFile::eReadOnly);
			if (!file.isGood())
			{
				cout << "********** Config file invalid!" << endl;
				return -1;
			}

			OmnString str;
			file.readToString(str);
			AosXmlParser parser;
			gAosAppConfig = parser.parse(str, "");
			if (!gAosAppConfig)
			{
				cout << "********** Invalid config file!" << endl;
				return -1;
			}

			index += 2;
			continue;
		}

		index++;
	}

	if (raddr == "")
	{
		cout << "*********** Missing remote address!" << endl;
		return -1;
	}

	if (rport < 1)
	{
		cout << "*********** Missing remote port!" << endl;
		return -1;
	}

	OmnAlarmMgr::setMaxAlarms(max_alarms);
	if (!gAosAppConfig)
	{
		AosXmlParser parser;
		OmnString str = "<config />";
		gAosAppConfig = parser.parse(str, "");
		aos_assert_r(gAosAppConfig, -1);
	}

	gAosAppConfig->setAttr("raddr", raddr);
	gAosAppConfig->setAttr("rport", rport);
	gAosAppConfig->setAttr("createthrds", createthrds);
	gAosAppConfig->setAttr("totalcreate", totalcreate);
	gAosAppConfig->setAttr("querythrds", querythrds);
	gAosAppConfig->setAttr("totalquery", totalquery);
	gAosAppConfig->setAttr("modthrds", modthrds);
	gAosAppConfig->setAttr("totalmod", totalmod);
	gAosAppConfig->setAttr("delthrds", delthrds);
	gAosAppConfig->setAttr("totaldel", totaldel);
	if (restart) gAosAppConfig->setAttr("restart", "true");

	// 
	// This is the application part
	//
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("SETorturer", "Try", "Yuhui");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSETorturerTestSuite::getSuite());

	cout << "Start Testing ..." << endl;
 
	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat() << endl;

	testMgr = 0;
 
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

