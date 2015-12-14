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
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/QueryMgr.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LogMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SiteMgr/SiteMgr.h"
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
	signal(SIGALRM, aosSignalHandler);

	aos_global_data_init();
	OmnApp theApp(argc, argv);

	int index = 1;
	bool restart = false;
	OmnString domainaddr;

	bool isRepairing = false;
	int docid_start = -1;
	int docid_bsize = -1;
	int iilid_start = -1;
	int iilid_bsize = -1;

	while (index < argc)
	{
		if (strcmp(argv[index], "-iilmgr_checkptr") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_IILMGR_CHECKPTR, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-iilmgr_checkref") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_IILMGR_CHECKREF, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-init_idgens") == 0)
		{
			// '-init_idgens passwd'
			AosXmlTagPtr def = theApp.getAppConfig()->getFirstChild(AOSCONFIG_IDGENS);
			aos_assert_r(def, -1);
			def->setAttr(AOSCONFIG_INIT_IDGENS, "true");
			def->setAttr(AOSCONFIG_PASSWD, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-setdocid") == 0)
		{
			// -setdocid docid blocksize
			AosSeIdGenMgr::getSelf();
			AosSeIdGenMgr::getSelf()->setDocid(argv[index+1], argv[index+2]);
			return 0;
		}

		if (strcmp(argv[index], "-setcid") == 0)
		{
			// -setcid cid blocksize
			AosIdGenMgr::getSelf();
			AosSeIdGenMgr::getSelf()->setCid(argv[index+1], argv[index+2]);
			return 0;
		}

		if (strcmp(argv[index], "-setcompid") == 0)
		{
			// -setcid cid blocksize
			AosIdGenMgr::getSelf();
			AosSeIdGenMgr::getSelf()->setCompid(argv[index+1], argv[index+2]);
			return 0;
		}

		if (strcmp(argv[index], "-setimageid") == 0)
		{
			// -setcid cid blocksize
			AosIdGenMgr::getSelf();
			AosSeIdGenMgr::getSelf()->setImageid(argv[index+1], argv[index+2]);
			return 0;
		}

		if (strcmp(argv[index], "-iilmgr_sanitycheck") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_IILMGR_SANITYCHECK, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-iilmgr_showlog") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_IILMGR_SHOWLOG, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-repairing") == 0)
		{
			isRepairing = true;
			index++;
			continue;
		}

		if (strcmp(argv[index], "-resetid") == 0)
		{
			docid_start = atoi(argv[index+1]);	
			docid_bsize = atoi(argv[index+2]);	
			iilid_start = atoi(argv[index+3]);	
			iilid_bsize = atoi(argv[index+4]);	
			index += 5;
			continue;
		}

		if (strcmp(argv[index], "-server_showlog") == 0)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_SERVER_SHOWLOG, "true");
			index++;
			continue;
		}

		if (strcmp(argv[index], "-dirname") == 0 && index < argc-1)
		{
			theApp.getAppConfig()->setAttr(AOSCONFIG_DIRNAME, argv[index+1]);
			AosXmlTagPtr admintag = theApp.getAppConfig()->getFirstChild(AOSCONFIG_SYSTEMADMIN);
			aos_assert_r(admintag, -1);
			admintag->setAttr(AOSCONFIG_DATAPATH, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-backupdir") == 0 && index < argc-1)
		{
			AosXmlTagPtr admintag = theApp.getAppConfig()->getFirstChild(AOSCONFIG_SYSTEMADMIN);
			aos_assert_r(admintag, -1);
			admintag->setAttr(AOSCONFIG_BACKUPDIR, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-backupfreq") == 0 && index < argc-1)
		{
			AosXmlTagPtr admintag = theApp.getAppConfig()->getFirstChild(AOSCONFIG_SYSTEMADMIN);
			aos_assert_r(admintag, -1);
			admintag->setAttr(AOSCONFIG_BACKUPFREQ, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-domain") == 0 && index < argc-1)
		{
			domainaddr = argv[index+1];
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-restart") == 0 && index < argc)
		{
			restart = true;
			index += 1;
			continue;
		}

		if (strcmp(argv[index], "-allow_create_superuser") == 0 && 
			index < argc)
		{
			theApp.getAppConfig()->setAttr(AOSTAG_ALLOW_CRTSPUSR, "true");
			restart = true;
			index += 1;
			continue;
		}

		index++;
	}

	OmnApp::setConfig("config_norm.txt");
	if (domainaddr != "") theApp.getAppConfig()->setAttr(AOSCONFIG_DOMAINADDR, domainaddr);
	
	// Initialize IIL
	AosIIL::staticInit(theApp.getAppConfig());
	AosXmlDoc::staticInit(theApp.getAppConfig());
	AosWordParser::init(theApp.getAppConfig());

	try
	{
		theApp.startSingleton(OmnNew AosQueryMgrSingleton());
		theApp.startSingleton(OmnNew AosWordMgr1Singleton());
		theApp.startSingleton(OmnNew AosDocServerSingleton());
		theApp.startSingleton(OmnNew AosIILMgrSingleton());
		theApp.startSingleton(OmnNew AosLogMgrSingleton());
		theApp.startSingleton(OmnNew AosSecurityMgrSingleton());
		theApp.startSingleton(OmnNew AosSessionMgrSingleton());  
		theApp.startSingleton(OmnNew AosDocMgrSingleton());  
		
		AosIILMgrSelf->start(theApp.getAppConfig());
		AosDocServerSelf->start(theApp.getAppConfig());
		OmnString dirname = theApp.getAppConfig()->getAttrStr(AOSCONFIG_DIRNAME);
		OmnString logfname = theApp.getAppConfig()->getAttrStr("logfilename");
		OmnString wordhashName = theApp.getAppConfig()->getAttrStr(AOSCONFIG_WORDID_HASHNAME);
		u32 wordidTablesize = theApp.getAppConfig()->getAttrU64(AOSCONFIG_WORDID_TABLESIZE, 0);
    		AosWordMgr1Self->start(dirname, wordhashName, wordidTablesize);
		AosLogMgrSelf->start(dirname, logfname);
		AosSecurityMgrSelf->start(theApp.getAppConfig());
		AosSessionMgrSelf->start(theApp.getAppConfig());

		theApp.startSingleton(OmnNew AosSeIdGenMgrSingleton());  
		AosDocServerSelf->setRepairing(isRepairing);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	// AosWSProcCmdLine(argc, argv);
	int tries = 10000;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Util/DocServerTester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosSearchEngineTestSuite::getSuite());
	
	cout << "Start Testing ..." << endl;
	
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	testMgr = 0;

	while (1)
	{
		OmnSleep(10000);
		// char contents[100];
		// cin >> contents;
		// if (strcmp(contents, "exit0318") == 0)
		// {
		// 	AosExitSearchEngine();
		// 	break;
		// }
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

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryMgrSelf->stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocServerSelf->exitSearchEngine();

	OmnScreen << "All Stopped!" << endl;
	OmnSleep(3);
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocServer::getSelf()->getRepairingMaxDocid() << endl;

	AosDocServerSelf = 0;
    AosWordMgr1Self = 0;
	AosLogMgrSelf = 0;

	exit(0);
	return true;
}
