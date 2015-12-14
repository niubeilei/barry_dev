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
#include "QueryRslt/QueryRsltMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "TransServer/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "ValueSel/AllValueSel.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "SmartDoc/SMDMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "SEModules/Ptrs.h"
#include "SEModules/SeRundata.h"
#include "StorageMgr/StorageMgr.h"
#include "UserMgmt/Python.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int gAosLogLevel = 1;
#include <dirent.h>

bool AosExitSearchEngine();

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
	
	//AosAppRundataPtr adata = OmnNew AosSeRundata();
	//OmnApp::appStart(argc, argv, adata);
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

		index++;
	}

	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(chdir(workingdir) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}

	OmnApp theApp(argc, argv);
	try
	{
		// Add By Brian Zhang
		// 2012/02/02
		AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
		
		// add by shawn 2012/01/16
		AosQueryRsltMgr::getSelf()->config(OmnApp::getAppConfig());
		AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
		AosStorageMgr::getSelf()->start();

		//AosIILTransServerPtr transServer = OmnNew AosIILTransServer();
		//AosXmlTagPtr transConfig = OmnApp::getAppConfig()->getFirstChild("iil_server");
		//AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, transServer);
		
		AosWordMgr1::getSelf()->config(OmnApp::getAppConfig());
		AosWordMgr1::getSelf()->start();
		AosIILMgr::getSelf()->config(OmnApp::getAppConfig());
		AosIILMgr::getSelf()->start();
		AosIILIDIdGen::getSelf()->config(OmnApp::getAppConfig());
		AosIILIDIdGen::getSelf()->start();
		
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

	//OmnScreen << "To stop IdGenMgr!" << endl;
	//AosIdGenMgr::getSelf()->stop();
	//OmnSleep(1);

	OmnScreen << "To stop IILMgr!" << endl;
	AosIILMgrSelf->stop();
	AosIIL::staticStop();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

	exit(0);
}

