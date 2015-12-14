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
// 06/29/2011: Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtil/DocFileMgr.h"
#include "SEModules/LogMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageMgr/StorageMgr.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "SEModules/SeRundata.h"
#include "UserMgmt/Python.h"
#include "DocServer/DocSvrConnMgr.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "TransServer/TransSvrConnSync.h"
#include "DocServer/AllDocSvrProcs.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();
int gAosLogLevel = 3;
#include <dirent.h>

static bool needStop = false;
void aosSignalHandler(int value)
{
	if (value == 14) needStop = true;
} 

int
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	//bool isRepairing = false;

	int index = 1;

	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
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
		
		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
		    gAosLogLevel = atoi(argv[index+1]);
		    index += 2;
		    continue;
		}

				
		index++;
	}
	
	OmnApp theApp(argc, argv);
	//OmnThread thread(pthread_self(), "main");
	//OmnThreadMgr::setThread(&thread);
	//
	
	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(chdir(workingdir) < 0)
	{
	    perror("Faild to set Working directory! ");
	    exit(-1);
	}
		
	try
	{
		bool flag = false;
		if (flag)
		{
			AosTransProcPtr transProc = OmnNew AosDocSvrConnMgr();
			AosXmlTagPtr docserverConfig = OmnApp::getAppConfig()->getFirstChild("docserver");
			AosXmlTagPtr transConfig = docserverConfig->getFirstChild(AOSCONFIG_TRANS);
			AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, transProc);
		
			// Ketty 2012/03/03
			AosTransServerPtr transServer = conn->getTransServer();
			transServer->recover();
		}
		
		//AosIILClient::getSelf();

		AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
		AosStorageMgr::getSelf()->start();

		AosDocSvr::getSelf()->config(OmnApp::getAppConfig());
		AosDocSvr::getSelf()->start();
	}

	catch (const OmnExcept &e)
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

	//OmnScreen << "To stop IdGenMgr!" << endl;
	//AosIdGenMgr::getSelf()->stop();
	//OmnSleep(1);

	OmnScreen << "To stop DocServer!" << endl;
	AosDocSvr::getSelf()->stop();
	OmnSleep(1);

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

	exit(0);
	return true;
}
