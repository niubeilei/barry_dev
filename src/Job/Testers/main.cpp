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
// 03/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/AllActions.h"
#include "Job/Testers/TestSuite.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "IILTrans/AllIILTrans.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterClt/CounterClt.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/AllCounterProc.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "DataStore/StoreMgr.h"
#include "DbQuery/Query.h"
#include "Debug/Debug.h"
#include "DocProc/DocProc.h"
#include "DocClient/DocClient.h"
#include "DocMgr/Ptrs.h"
#include "DocMgr/DocMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvrConnMgr.h"
#include "DocServer/AllDocSvrProcs.h"
#include "Job/JobMgr.h"
#include "TaskMgr/TaskMgr.h"
#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "IILTrans/AllIILTrans.h"
#include "MultiLang/LangDictMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/AllTerms.h"
#include "QueryClient/QueryClient.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/FrontEndSvr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/SiteMgr.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/LangDictObj.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/DocSelRandObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageMgr/StorageMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "SmartDoc/SmartDocImpl.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordTransClient/WordTransClient.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "VersionServer/VersionServer.h"
#include "UserMgmt/UserMgmt.h"
#include "Python/Python.h"
#include "Conds/AllConds.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "ImportData/ImportDataCtlr.h"
#include "ImportData/ImportListener.h"
#include "ImportData/Testers/TestSuite.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();

static OmnString sgBuildNum = "2011-12-13.1";
static OmnString sgBuildTime = "2011/12/13 15:18";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = "/home/ice/AOS/src/SEServer";

static bool needStop = false;
const static int sgStatusFreq = 0x30ffffff;

int redirectOutPut();

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

	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);

	int index = 1;
	bool isRepairing = false;

	int tries = 1000000;
	OmnString tester_name;
	while (index < argc)
	{
		if (strcmp(argv[index], "-tries") == 0 && index < argc-1)
		{
			tries = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-tester") == 0 && index < argc-1)
		{
			tester_name = argv[index+1];
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

		if (strcmp(argv[index], "-version") == 0)
		{
			// '-version version'
			OmnApp::setVersion(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index += 2;
			continue;
		}
        
		//felicia,2012/04/05 datasync
		if (strcmp(argv[index], "-datasync") == 0)
		{
		 	OmnString attrname = argv[index+1];
		 	OmnString tag = argv[index+2];
		 	OmnString sdocstr = "<smartdoc zky_otype=\"zky_smtdoc\" zky_sdoctp=\"DataSync\" zky_modify_sdoc=\"false\" remote_addr=\"192.168.99.88\" remote_port=\"5555\" zky_sync_ctnrs=\"";
		 	sdocstr << tag << "\" zky_attrname=\"" << attrname << "\"/>";
		 
		 	AosXmlTagPtr sdoc = AosXmlParser::parse(sdocstr AosMemoryCheckerArgs);
		 	if (!sdoc) exit(0);
		 	AosRundataPtr rdata = OmnApp::getRundata();
		 	rdata->setReceivedDoc(sdoc, true);
		 	rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
		 	AosSmartDoc::runSmartdoc(sdoc, rdata);
		 	OmnScreen << "System Data Sync finished!" << endl;
		}

		index++;
	}

	if (!OmnApp::getAppConfig())
	{
		cout << "Faild to get config! " << endl;
		exit(-1);
	}

	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if (workingdir != "" && chdir(workingdir.data()) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}
	AosMemoryChecker::getSelf()->start();

	OmnApp theApp(argc, argv);
	
	AosXmlTagPtr counter_client =  OmnApp::getAppConfig()->getFirstChild("CounterClt");
	AosXmlTagPtr msgclient =  OmnApp::getAppConfig()->getFirstChild("msgclient");
	AosXmlTagPtr seLogClt =  OmnApp::getAppConfig()->getFirstChild("SeLogClt");

	// Set Interface Objects
	OmnScreen << "...........Start ObjMgr........." << endl;
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(new AosSmartDocImpl());
	// AosActionObj::setActionObj(new AosSdocActionObj());
	AosDocSelObj::setDocSelector(new AosDocSelObjImpl());
	AosDocSelRandObj::setDocSelRand(new AosDocSelRandUtil());
	AosLoginMgrObj::setLoginMgrObj(new AosLoginMgr(OmnApp::getAppConfig(), true));
	AosUserMgmtObj::setUserMgmt(new AosUserMgmt());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosErrorMgr::getSelf()->config(OmnApp::getAppConfig());
	AosQueryRsltMgr::getSelf()->config(OmnApp::getAppConfig());
	AosStorageMgr::getSelf()->config(OmnApp::getAppConfig());
	AosDocSvr::getSelf()->config(OmnApp::getAppConfig());
	AosIILIDIdGen::getSelf()->config(OmnApp::getAppConfig());
	AosWordMgr1::getSelf()->config(OmnApp::getAppConfig());
	AosIILMgr::getSelf()->config(OmnApp::getAppConfig());
	AosFrontEndSvr::getSelf()->config(OmnApp::getAppConfig());
	AosDocClient::getSelf()->setRepairing(isRepairing);
	AosDocClient::getSelf()->config(OmnApp::getAppConfig());
	AosDocProc::getSelf()->config(OmnApp::getAppConfig());
	AosIILClient::getSelf()->config(OmnApp::getAppConfig());
	AosSessionMgr::getSelf()->config(OmnApp::getAppConfig());  
	AosPython::getSelf()->config(OmnApp::getAppConfig());  
	AosSecurityMgr::getSelf()->config(OmnApp::getAppConfig());
	AosCounterClt::getSelf()->config(OmnApp::getAppConfig());
	AosSeLogClient::getSelf()->config(OmnApp::getAppConfig());
	AosTimerMgr::getSelf()->config(OmnApp::getAppConfig());
	AosMsgClient::getSelf()->config(OmnApp::getAppConfig());
	AosSiteMgr::getSelf()->config(OmnApp::getAppConfig());

	AosPhyCtnrSvrPtr counterserver;
	if (counter_client)
	{
		counterserver = OmnNew AosPhyCtnrSvr(OmnApp::getAppConfig());
		aos_assert_r(counterserver, 0);

	}
	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	// Init SeLogSvr.
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvrPtr logsvr = OmnNew AosPhyLogSvr(OmnApp::getAppConfig());
	aos_assert_r(logsvr, 0);

	// Ketty 2012/01/10

	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();
	OmnScreen << "...........Start IILMgr........." << endl;
	AosIILMgr::getSelf()->start();
	logsvr->start();
	if (counterserver)
	{
		counterserver->start();
	}

	OmnSleep(1);
	AosTransClient::sRecover();

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();
	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();

	if (seLogClt)
	{
		OmnScreen << "...........Start LogClient........." << endl;
		AosSeLogClient::getSelf()->start();
	}
	if (counter_client)
	{
		OmnScreen << "...........Start CounterClient........." << endl;
		AosCounterClt::getSelf()->start();
	}
		
	OmnScreen << "...........Start SessionMgr........." << endl;
	AosSessionMgr::getSelf()->start();  
	OmnScreen << "...........Start Python........." << endl;
	AosPython::getSelf()->start();  
	OmnScreen << "...........Start Securitymgr........." << endl;
	AosSecurityMgr::getSelf()->start();
	OmnScreen << "...........Start SiteMgr........." << endl;
	AosSiteMgr::getSelf()->start();

	AosSeReqProc::config(OmnApp::getAppConfig());

	OmnScreen << "...........Start StartLog........." << endl;
	AosSeLogClient::startLog();
	
	OmnScreen << "...........All Services Started........." << endl;
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	
	OmnScreen << "...........All Services Started........." << endl;

	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();

	/*
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Job/Tester", "Try", "Chen Ding");
	testMgr->setTries(tries);
	testMgr->addSuite(AosJobTestSuite::getSuite(tester_name));

	cout << "Start Testing ..." << endl;

	testMgr->start();

	cout << "\nFinished. " << testMgr->getStat().data() << endl;

	testMgr = 0;
	*/

	while (1)
	{
		OmnSleep(20);
	}	

	theApp.appLoop();
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
	AosQueryClient::getSelf()->stop();

	OmnScreen << "To stop Requester!" << endl;
	AosSeReqProc::stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	OmnScreen << "To stop IILMgr!" << endl;
	AosIILMgr::getSelf()->stop();
	
	OmnScreen << "To TransClients!" << endl;
	AosTransClient::sStop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;
	return true;
}

