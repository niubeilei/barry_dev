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
// 05/14/2011: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "Actions/ActAddAttr.h"
#include "AppMgr/App.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryWrap/QueryClient.h"
#include "SEUtil/SeXmlParser.h"
#include "SEUtil/XmlTag.h"
#include "SEUtil/XmlDoc.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LogMgr.h"
#include "SEModules/SiteMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "SearchEngine/DocServer.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "SmartDoc/SmartDoc.h"
#include "SmartDoc/AllSmartDocs.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "DocSelector/AllSelectors.h"
#include "ActOpr/AllActOpr.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "Actions/AllActions.h"
#include "ActOpr/AllActOpr.h"
#include "SEUtil/MonitorLogMgr.h"
#include "UserMgmt/Python.h"
//#include "Conds/AllConds.h"
#include "Valueset/AllValuesets.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "DocSelector/Testers/TestSuite.h"
#include "DocSelector/AllSelectors.h"

/*
#include "Alarm/Alarm.h"

#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Tester/Ptrs.h"
#include "SEUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "DocSelector/Testers/TestSuite.h"
#include "DocSelector/AllSelectors.h"
#include "Valueset/AllValuesets.h"
#include "ActOpr/AllActOpr.h"
#include "DocSelector/AllSelectors.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
*/

bool AosExitSearchEngine();

void aosSignalHandler(int value)
{
	if (value == 14) AosExitSearchEngine();
}


int gAosLogLevel = 1;
#include <dirent.h>

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);

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
		index++;
	}

	// Initialize IIL
	AosIIL::staticInit(theApp.getAppConfig());
	AosXmlDoc::staticInit(theApp.getAppConfig());
	AosWordParser::init(theApp.getAppConfig());

	try
	{
		theApp.startSingleton(OmnNew AosQueryClientSingleton());
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
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosXmlTagPtr testers = theApp.getAppConfig()->getFirstChild("testers");

//	if (tries < 1) tries = 1000;
	tries = 1000;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DocSelectorTester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosDocSelectorTestSuite::getSuite(testers));
	
	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	testMgr = 0;

	while (1)
	{
		OmnSleep(100);
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
