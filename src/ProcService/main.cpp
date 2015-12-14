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
#include "SEModules/SeRundata.h"
#include "ProcService/ProcService.h"
#include "ProcService/ProcRegister.h"
#include "ProcService/ProcControler.h"
#include "ProcService/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/MemMgr.h"
#include "util2/global_data.h"

#include "SmartDoc/SmartDoc.h"

#include <stdlib.h>

int gAosLogLevel = 1;



struct Omn_Str_Cmp
{
	bool operator() (const OmnString &p1, const OmnString &p2)
	{
		return p1==p2;
	};
};


int 
main(int argc, char **argv)
{

	map<OmnString, int> mymap;
	mymap.insert(pair<OmnString, int>("hello",4));
	aos_global_data_init();

	bool configflag = false;
	int index = 0;
	while (index < argc)
	{
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
		OmnApp::appStart(argc, argv);
		OmnApp::setVersion("0.10");
		OmnApp::setConfig(name);
	}
	AosXmlTagPtr appConfig = OmnApp::getAppConfig();

	OmnApp theApp(argc, argv);
	AosXmlTagPtr procservice = appConfig->getFirstChild("procservice");
	OmnSleep(10);
OmnScreen << "starting the client-----------" << endl;
	theApp.startSingleton(OmnNew AosProcRegisterSingleton());
	AosProcRegister::getSelf()->start(procservice);
	theApp.startSingleton(OmnNew AosProcControlerSingleton());
	AosProcControler::getSelf()->start(procservice);
	theApp.startSingleton(OmnNew AosProcServiceSingleton());
	AosProcService::getSelf()->start(procservice);
OmnScreen << "end connect--------" << endl;
OmnScreen << "end connect--------" << endl;
OmnScreen << "end connect--------" << endl;

	while (1)
	{
		OmnSleep(1000);
	}

	theApp.exitApp();
	return 0;
} 

