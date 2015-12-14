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
// 06/013/2011: Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/AllCounterProc.h"
#include "CounterTime/AllTimeProc.h"
#include "Debug/Debug.h"
#include "DocClient/DocClient.h"
#include "SearchEngine/DocServer1.h"
#include "IdGen/IdGenMgr.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEModules/SiteMgr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/Ptrs.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/AllSmartDocs.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "TransServer/Ptrs.h"
#include "UserMgmt/Python.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


static OmnString sgBuildNum = "1000";
static OmnString sgBuildTime = "2011/07/07 10:06";
static OmnString sgBuildBy = "lynch";
static OmnString sgBuildAuthBy = "lynch";
static OmnString sgLocation = "/home/Lynch/AOS/src/";

static bool needStop = false;

void aosSignalHandler(int value)
{
    if (value == 14) needStop = true;
}


int gAosLogLevel = 1;

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

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

		if (strcmp(argv[index], "-build") == 0)
		{
			cout << "Build Number: " 
				<< sgBuildNum << " on " 
				<< sgBuildTime << " by Engineer "
				<< sgBuildBy << " authorized by " 
				<< sgBuildAuthBy << ". \nFile location: "
				<< sgLocation << endl;
			exit(0);
		}

		index++;
	}

	OmnApp theApp(argc, argv);
	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(chdir(workingdir) < 0)
	{
	    perror("Faild to set Working directory! ");
	    exit(-1);
	}

	OmnApp::appStart(argc, argv);
	AosPhyCtnrSvrPtr proc = OmnNew  AosPhyCtnrSvr(OmnApp::getAppConfig());
	if (!proc)
	{
		OmnAlarm << "Failed to start the application " << enderr;
	    exit(-1);
	}

	try
	{
		// Initialize IIL
		AosIILClient::getSelf();
		AosIILClient::getSelf()->config(OmnApp::getAppConfig());

		AosIILMgr::getSelf();
		AosIILMgr::getSelf()->config(OmnApp::getAppConfig());
			
		AosSeIdGenMgr::getSelf();

		AosIILClient::getSelf()->start();
		AosIILMgr::getSelf()->start();
		AosSeIdGenMgr::getSelf()->start();
	
		if (need_reset_virtual) AosPhyCtnrSvr::resetVirtualServers();
		proc->start();

		//AosPhyCtnrSvr::config(OmnApp::getAppConfig());
		//if (need_reset_virtual) AosPhyCtnrSvr::resetVirtualServers();
		//AosXmlTagPtr transConfig = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_TRANS);
		//AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, &proc);
   		//conn->start();
	}
	catch (const OmnExcept &e)
	{
	    OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
	    theApp.exitApp();
	    return 0;
	}

	while (1)
	{
		OmnSleep(10);
		if (needStop)
		{
			proc->stop();
			exit(0);
		}
	
	}
	theApp.exitApp();
	exit(0);
} 
