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
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "DocClient/DocClient.h"
#include "DocProc/DocProc.h"
#include "IdGen/IdGenMgr.h"
#include "IILMgr/IIL.h"
#include "IILMgr/IILMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryClient/QueryClient.h"
#include "SearchEngine/DocMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LogMgr.h"
#include "SEModules/SeRundata.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SiteMgr/SiteMgr.h"
#include "SearchEngine/TesterNew/TestSuite.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
//#include "SearchEngine/DocServer.h"
#include "SmartDoc/SmartDoc.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util1/MemMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
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


/*
static bool setConfig(const char *fname)
{
	if (!fname)
	{
		OmnAlarm << "Missing file name!" << enderr;
		exit(-1);
	}

	OmnFile f(fname, OmnFile::eReadOnly);
	if (!f.isGood())
	{
		OmnAlarm << "Failed to open the config file: " << fname << enderr;
		exit(-1);
	}

	OmnString contents;
	if (!f.readToString(contents))
	{
		OmnAlarm << "Failed to read config: " << fname << enderr;
		exit(-1);
	}

	AosXmlParser parser;
	gAosAppConfig = parser.parse(contents, "");
	if (!gAosAppConfig)
	{
		OmnAlarm << "Configuration file incorrect: " << fname << enderr;
		exit(-1);
	}

	gAosAppConfig->setAttr(AOSTAG_ALLOW_CRTSPUSR, "false");
	return true;
}
*/


int 
main(int argc, char **argv)
{
	system("rm -rf Data/Data/*");
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();
	OmnApp theApp(argc, argv);


	//AosAppRundataPtr adata = OmnNew AosSeRundata();
	//OmnApp::appStart(argc, argv, adata);
	OmnApp::appStart(argc, argv);

	int index = 1;
	OmnApp::setConfig("config_norm.txt");
	int tries = 0;

	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		if (strcmp(argv[index], "-iilmgr_showlog") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_IILMGR_SHOWLOG, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-server_showlog") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_SERVER_SHOWLOG, "true");
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

	// Initialize IIL
	AosIIL::staticInit(theApp.getAppConfig());
	AosXmlDoc::staticInit(theApp.getAppConfig());
	//  AosWordParser::init(theApp.getAppConfig());

	try
	{

		


		theApp.startSingleton(OmnNew AosQueryClientSingleton());
		theApp.startSingleton(OmnNew AosWordMgr1Singleton());
		theApp.startSingleton(OmnNew AosDocProcSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
//shawn		theApp.startSingleton(OmnNew AosLogMgrSingleton());
//		AosLogMgr::getSelf();
		theApp.startSingleton(OmnNew AosSecurityMgrSingleton());
		theApp.startSingleton(OmnNew AosSessionMgrSingleton());  
//shawn		theApp.startSingleton(OmnNew AosDocMgrSingleton());  
		AosDocProc::getSelf();  
		
//		AosIILMgrSelf->start(theApp.getAppConfig());
		AosIILMgr::getSelf(); 
		AosSeIdGenMgr::getSelf();
		AosIILMgr::getSelf()->setSaveBatch(false); 

//shawn		AosDocServerSelf->start(theApp.getAppConfig());
		AosDocClient::getSelf();

		OmnString dirname = theApp.getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString logfname = theApp.getAppConfig()->getAttrStr("logfilename");
		OmnString wordhashName = theApp.getAppConfig()->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
		u32 wordidTablesize = theApp.getAppConfig()->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0);

//shawn    		AosWordMgr1Self->start(dirname, wordhashName, wordidTablesize);
		AosWordMgr1::getSelf();
		
//shawn		AosLogMgrSelf->start(dirname, logfname);
//shawn		AosSecurityMgrSelf->start(theApp.getAppConfig());
//shawn		AosSessionMgrSelf->start(theApp.getAppConfig());
		AosSessionMgr::getSelf();
		//theApp.startSingleton(OmnNew AosSeIdGenMgrSingleton());  
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	OmnString test = theApp.getAppConfig()->getAttrStr("tester");
	if (test == "")
	{
		cout <<"Missing tester Type!";
		return false;
	}

	if (tries < 1) tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("IIL/getUniqueValueTester", "Try", "Ken Lee");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSearchEngineTestSuite::getSuite(test));
	
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


bool AosExitSearchEngine()
{
	OmnScreen << "To stop server!" << endl;

	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgr::getSelf()->stop();
	OmnSleep(1);

	AosQueryClient::getSelf()->stop();	

	OmnScreen << "To stop DocServer!" << endl;
//shawn 	AosDocServerSelf->exitSearchEngine();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

//	OmnScreen << "Max Docid: " << AosDocServer::getSelf()->getRepairingMaxDocid() << endl;

//	AosDocServerSelf = 0;
    AosWordMgr1Self = 0;
//shawn	AosLogMgrSelf = 0;

	exit(0);
	return true;
}
