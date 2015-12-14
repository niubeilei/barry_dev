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
// 01/03/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "QueryTorturer/Exe/TestSuite.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryTorturer/QueryTesterMgr.h"
#include "SEUtil/DocFileMgr.h"
#include "SEModules/Ptrs.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlDoc.h"
#include "XmlUtil/XmlTag.h"
#include "Util/ExeReport.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Buff.h"
#include "Util/StrSplit.h"
#include "util2/global_data.h"


#include <stdlib.h>

#include <ext/hash_map>
namespace std
{ 
	using namespace __gnu_cxx;
}

int gAosLogLevel = 1;
static OmnString sgSiteid = "100";
using namespace std;

#include <dirent.h>

static OmnString sgAosOpenLzHome;
AosXmlTagPtr gAosAppConfig;

int gAosShowNetTraffic = 1;
OmnString AosGetOpenLzHome()
{
	return sgAosOpenLzHome;
}


int 
main(int argc, char **argv)
{
	aos_global_data_init();
	
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.10");
	
	OmnApp theApp(argc, argv);

	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-dirname") == 0)
		{
			OmnString dirname = argv[index+1];
			index += 2;
			OmnString cmd = "rm -rf ";
			cmd << dirname;
			system(cmd.data());

			cmd = "mkdir ";
			cmd << dirname;
			system(cmd.data());
			continue;
		}

		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("StatisticsTorturer", "Try", "Levi");
	testMgr->setTries(tries);
	testMgr->addSuite(AosQueryTorturerTestSuite::getSuite());                             

	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	AosExeReport report(10);
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 

