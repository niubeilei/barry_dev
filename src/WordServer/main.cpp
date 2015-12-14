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
// 11/16/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SiteMgr/SiteMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "Util1/MemMgr.h"
#include "WordServer/Ptrs.h"
#include "WordServer/WordReqProc.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILDocid.h"
#include "SearchEngine/DocServer.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"


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

int test123() {return 0;}

int gAosLogLevel = 1;
#include <dirent.h>


int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();
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

		index++;
	}

	// Initialize IIL
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew AosWordMgr1Singleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
		theApp.startSingleton(OmnNew AosIdGenMgrSingleton());

		
		AosIILMgrSelf->start(OmnApp::getAppConfig());
		OmnString dirname = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString wordhashName = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
		u32 wordidTablesize = OmnApp::getAppConfig()->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0);
		AosWordMgr1::getSelf()->start(dirname, wordhashName, wordidTablesize);
		theApp.startSingleton(OmnNew AosSeIdGenMgrSingleton());
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosWordReqProc::config(OmnApp::getAppConfig());
	AosWordReqProcPtr proc = OmnNew AosWordReqProc();
	AosReqDistr reqDistr(proc);
	if (!reqDistr.config(OmnApp::getAppConfig()->toString()))
	{
		OmnAlarm << "Failed the configuration: " 
			<< OmnApp::getAppConfig()->toString() << enderr;
		theApp.exitApp();
		return -1;
	}
	reqDistr.start();

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;

	AosWordMgr1::getSelf()->stop();
	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

    AosWordMgr1Self = 0;
	exit(0);
	return true;
}
