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
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "IILTrans/AllIILTrans.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterTime/AllTimeProc.h"
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
#include "SEInterfaces/SizeIdMgrObj.h"
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
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/TaskObj.h"
#include "SEUtilServer/SeIdGenMgr.h"

//ken 2012/05/22
#include "IILUtil/IILScanner.h"
#include "SEInterfaces/IILScannerObj.h"

#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "StorageMgr/StorageMgr.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageEngine/AllStorageEngines.h"
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
#include "Thread/ThreadShellMgr.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "TransClient/TransClient.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Actions/Testers/TestSuite.h"

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
bool sgIsBigIILServer = false;

int redirectOutPut();

void aosSignalHandler(int value)
{
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;
int gAosShowNetTraffic = 1;
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

	while (index < argc)
	{
		if (strcmp(argv[index], "-rdcout") == 0)
		{
			redirectOutPut();
			index ++;
			continue;
		}

		if (strcmp(argv[index], "-convert_upn") == 0)
		{
		    AosUnicomConvertUpnPrefix();
		    return 0;
		}

		if (strcmp(argv[index], "-convert_cpn") == 0)
		{
		    AosUnicomConvertCpnPrefix();
		    return 0;
		}

		if (strcmp(argv[index], "-build") == 0)
		{
			cout << "Build Number: " 
				<< sgBuildNum.data() << " on " 
				<< sgBuildTime.data() << " by Engineer "
				<< sgBuildBy.data() << " authorized by " 
				<< sgBuildAuthBy.data() << ". \nFile location: "
				<< sgLocation.data() << endl;
			exit(0);
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

		if (strcmp(argv[index], "-repairing") == 0)
		{
			isRepairing = true;
			index++;
			continue;
		}
		
		if (strcmp(argv[index], "-isbigiil") == 0)
		{
			sgIsBigIILServer = true;
			index++;
			continue;
		}

		if (strcmp(argv[index], "-backupdir") == 0 && index < argc-1)
		{
			AosXmlTagPtr admintag = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_SYSTEMADMIN);
			aos_assert_r(admintag, -1);
			admintag->setAttr(AOSCONFIG_BACKUPDIR, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-backupfreq") == 0 && index < argc-1)
		{
			AosXmlTagPtr admintag = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_SYSTEMADMIN);
			aos_assert_r(admintag, -1);
			admintag->setAttr(AOSCONFIG_BACKUPFREQ, argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		// Chen Ding, 06/08/2012
		if (strcmp(argv[index], "-show_net") == 0 && index < argc-1)
		{
			gAosShowNetTraffic = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		// Chen Ding, 06/11/2012
		if (strcmp(argv[index], "-ignored_ports") == 0 && index < argc-1)
		{
			OmnTcp::setIgnoredPorts(argv[index+1]);
			index += 2;
			continue;
		}

		// Chen Ding, 06/07/2012
		if (strcmp(argv[index], "-iillog") == 0 && index < argc-1)
		{
			gAosIILLogLevel = atoi(argv[index+1]);
			index += 2;
			continue;
		}
		
		if (strcmp(argv[index], "-allow_create_superuser") == 0 && 
			index < argc)
		{
			OmnApp::getAppConfig()->setAttr(AOSTAG_ALLOW_CRTSPUSR, "true");
			index += 1;
			continue;
		}

		index++;
	}

	OmnApp theApp(argc, argv);
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);

	// Chen Ding, 06/05/2012
	AosNetworkMgrObj::config();
	AosTransModuleClt::init(app_config->getFirstChild(AOSCONFIG_TRANS_CLIENT));
	AosTransModuleSvr::init(app_config->getFirstChild(AOSCONFIG_TRANS_SERVER));

	OmnThreadShellMgr::getSelf()->start();

	AosMemoryChecker::getSelf()->start();

	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}
	
	AosXmlTagPtr counter_client = app_config->getFirstChild("CounterClt");
	AosXmlTagPtr msgclient =  app_config->getFirstChild("msgclient");
	AosXmlTagPtr seLogClt =  app_config->getFirstChild("SeLogClt");
//	OmnDataStoreMySQL::startMySQL(argc, argv);

	// Set Interface Objects
	OmnScreen << "...........Start ObjMgr........." << endl;
	//AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());
	//AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false));
//	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
//	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
//	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());

	AosQueryContext::initQueryContext();
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(OmnNew AosSmartDocImpl());
	AosDocSelObj::setDocSelector(OmnNew AosDocSelObjImpl());
	AosDocSelRandObj::setDocSelRand(OmnNew AosDocSelRandUtil());
	AosLoginMgrObj::setLoginMgrObj(OmnNew AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(OmnNew AosUserMgmt());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	AosStorageMgr::getSelf()->config(app_config);
	AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosWordMgr1::getSelf()->config(app_config);
	AosIILMgr::getSelf()->config(app_config);
	AosFrontEndSvr::getSelf()->config(app_config);
	AosDocClient::getSelf()->setRepairing(isRepairing);
	AosDocClient::getSelf()->config(app_config);
	AosDocProc::getSelf()->config(app_config);
	AosIILClient::getSelf()->config(app_config);
	AosSessionMgr::getSelf()->config(app_config);  
	AosPython::getSelf()->config(app_config);  
	AosSecurityMgr::getSelf()->config(app_config);
	AosCounterClt::getSelf()->config(app_config);
	AosSeLogClient::getSelf()->config(app_config);
	AosTimerMgr::getSelf()->config(app_config);
	//AosMsgClient::getSelf()->config(app_config);
	AosSiteMgr::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
//	AosSoapServer::getSelf()->config(app_config);
	AosAllStorageEngines::init();
	AosStorageEngineObj::storageEngineInited();

	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	AosAllActions::init();
	AosInitConditions();
	AosPhyCtnrSvrPtr counterserver;
	if (counter_client)
	{
		/*
		AosCounterClt::getSelf();
		AosTransProcPtr counterserver = OmnNew AosPhyCtnrSvr(app_config);
		AosTransClientPtr counter_trans = 
			OmnNew AosTransClient(counter_client, counterserver);
		AosCounterClt::getSelf()->setTransClient(counter_trans);
		*/
		counterserver = OmnNew AosPhyCtnrSvr(app_config);
		aos_assert_r(counterserver, 0);

	}

	AosTransModuleSvr::recover();

	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	// Init SeLogSvr.
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvrPtr logsvr = OmnNew AosPhyLogSvr(app_config);
	aos_assert_r(logsvr, 0);


	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();
	OmnScreen << "...........Start IILMgr........." << endl;
	AosIILMgr::getSelf()->start();
	OmnScreen << "...........Start IILMgr Finished........." << endl;
	// moved by Ketty 2012/05/10
	//logsvr->start();
	//if (counterserver)
	//{
	//	counterserver->start();
	//}
	AosTransModuleSvr::start();

	OmnSleep(2);
	AosTransModuleClt::recover();

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();
	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	// Ketty 2012/05/10
	logsvr->start();
	if (counterserver)
	{
		counterserver->start();
	}

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
	AosTransModuleClt::start();

		
	OmnScreen << "...........Start SessionMgr........." << endl;
	AosSessionMgr::getSelf()->start();  
	OmnScreen << "...........Start Python........." << endl;
	AosPython::getSelf()->start();  
	OmnScreen << "...........Start Securitymgr........." << endl;
	AosSecurityMgr::getSelf()->start();
	OmnScreen << "...........Start SiteMgr........." << endl;
	AosSiteMgr::getSelf()->start();

	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
	}
	//AosJobMgr::getSelf()->start();
	//AosTaskMgr::getSelf()->start();

	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	//OmnScreen << "...........All Services Started........." << endl;

	AosObjMgrObj::createObjsAtStartupStatic();
	
	AosXmlTagPtr testers = theApp.getAppConfig()->getFirstChild("testers");
	if (!testers)
	{
		OmnAlarm << "Missing testers" << enderr;
		return -1;
	}

	OmnString testname = testers->getAttrStr("name");
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("Actions", "Try", "Chen Ding");
	testMgr->setTries(1);
	testMgr->addSuite(AosActionsTestSuite::getSuite(testname));
	testMgr->start();

	while (1)
	{
		OmnSleep(100);
		if (needStop)
		{
			AosExitSearchEngine();
			break;
		}
	}
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
	//AosTransClient::sStop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;
	return true;
}


int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	OmnString filename;
	filename = "server_cout.txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}

