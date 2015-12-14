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
// 2011/07/12	: Created by Ken Lee 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "TransServer/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "WordTransServer/WordTransServer.h"
#include "WordTransServer/Ptrs.h"
#include "SEUtil/MonitorLogMgr.h"
#include "UserMgmt/Python.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SmartDoc/SmartDoc.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();

static bool needStop = false;

void aosSignalHandler(int value)
{
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
#include <dirent.h>

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	
	int index = 1;
	bool need_reset_virtual = false;
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
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}
		if (strcmp(argv[index], "-reset_virtual") == 0)
        {
            need_reset_virtual = true;
            index++;
            continue;
        }

		if (strcmp(argv[index], "-init_idgens") == 0)
		{
			// '-init_idgens passwd'
			AosXmlTagPtr def = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
			aos_assert_r(def, -1);
			def->setAttr(AOSCONFIG_INIT_IDGENS, "true");
			def->setAttr(AOSCONFIG_PASSWD, argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	OmnApp theApp(argc, argv);
	try
	{
		AosWordTransServerPtr transProc = OmnNew AosWordTransServer();
		AosXmlTagPtr transConfig = OmnApp::getAppConfig()->getFirstChild("word_server");
		AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, transProc);
		AosWordMgr1::getSelf();
		
		AosTransServerPtr server = conn->getTransServer();
		server->recover();
		conn->start();
	}
	catch(const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}
	
	while (1)
	{
		OmnSleep(3);
		if (needStop) AosExitSearchEngine();
	}
	theApp.exitApp();
	return 0;
} 


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;

	OmnScreen << "To stop WordMgr!" << endl;
	AosWordMgr1Self->stop();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

    AosWordMgr1Self = 0;

	exit(0);
}

