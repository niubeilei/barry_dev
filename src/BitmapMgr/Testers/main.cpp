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
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "BitmapMgr/Bitmap.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "SearchEngine/DocMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LogMgr.h"
#include "SEModules/SeRundata.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "BitmapMgr/Testers/TestSuite.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SmartDoc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util1/MemMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();

void aosSignalHandler(int value)
{
	if (value == 14) AosExitSearchEngine();
}


int gAosLogLevel = 1;
#include <dirent.h>

i64 sgNumber = 0;
i64 sgTime = 0;
i64	sgMaxRoundTime = 0;
i64 sgMaxMemory = 0;
i64 sgAlarmFlag = 0;
i64	sgRound = 0;
bool sgNeedFinish = false;

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();
	OmnApp theApp(argc, argv);
      
	OmnApp::appStart(argc, argv);
	// OmnApp::setConfig("config_norm.txt");

	int index = 1;
	while(index < argc)
	{
		if(strcmp(argv[index], "-number") == 0)
		{
			sgNumber = OmnString(argv[index+1]).toI64(0);
		}
		if(strcmp(argv[index], "-time") == 0)
		{
			sgTime = OmnString(argv[index+1]).toI64(0);
		}
		if(strcmp(argv[index], "-maxroundtime") == 0)
		{
			sgMaxRoundTime = OmnString(argv[index+1]).toI64(0);
		}
		if(strcmp(argv[index], "-maxmem") == 0)
		{
			sgMaxMemory = OmnString(argv[index+1]).toI64(0);
		}
		if(strcmp(argv[index], "-alarmflag") == 0)
		{
			sgAlarmFlag = (bool)OmnString(argv[index+1]).toI64(0);
		}
		index+=2;
	}

	AosBitmapObj::setObject(OmnNew AosBitmap());
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("QueryRsltTester", "Try", "Brian Zhang");
	testMgr->addSuite(AosBitmapMgrTestSuite::getSuite());
	
	cout << "Start Testing ..." << endl;
	
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	sgNeedFinish = true;
	OmnSleep(1);

	testMgr = 0;

	theApp.exitApp();
	return 0;
} 


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;

	exit(0);
	return true;
}
