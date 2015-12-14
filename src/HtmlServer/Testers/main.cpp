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
// 2011/01/15 	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "HtmlServer/Testers/TestSuite.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "WordMgr/WordMgr1.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILDocid.h"
#include "SearchEngine/DocMgr.h"
#include "SearchEngine/DocServer.h"
#include "IILMgr/IILMgr.h"
#include "SearchEngine/TesterStr/TestSuite.h"
#include "SearchEngine/TesterStr/DocMgrTester.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util1/MemMgr.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "SearchEngineAdmin/SengAdmin.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();

void aosSignalHandler(int value)
{
}


int gStartDocid= 1;
int gAosLogLevel = 1;
#include <dirent.h>

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();
	//AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp theApp(argc, argv);

	int index = 0;

	while (index < argc)
	{
		
		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}
		
		if (strcmp(argv[index], "-start") == 0)
		{
			gStartDocid = atoi(argv[index+1]);
			if (gStartDocid<=5000) gStartDocid = 5000;
			index++;
			continue;
		}
		index++;
	}

	OmnString domainaddr;

	OmnApp::setConfig(OmnApp::readConfig("config_norm.txt"));

	//AosSengAdmin::getSelf()->start(OmnApp::getAppConfig());
	AosSengAdmin::getSelf()->start();


	if (domainaddr != "") theApp.getAppConfig()->setAttr(AOSCONFIG_DOMAINADDR, domainaddr);
	

	int tries = 10000;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("HTML Server", "Try", "Lynch");
	testMgr->setTries(tries);
	testMgr->addSuite(AosHtmlServerTestSuite::getSuite());
	
	cout << "Start Testing ..." << endl;
	
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	testMgr = 0;

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 
