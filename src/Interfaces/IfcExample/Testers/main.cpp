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
#include "API/AosApi.h"

#include "Interfaces/IfcExample/Testers/IfcExampleTester.h"
#include "Actions/AllActions.h"
#include "ValueSel/AllValueSel.h"
#include "SEInterfaces/ValueSelObj.h"
#include "SEInterfaces/DataCalcObj.h"
#include "SEInterfaces/DLLCondObj.h"
#include "Conds/AllConds.h"
#include "DataAssembler/DataAssembler.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "DataBlob/BlobRecord.h"
#include "DataField/DataFieldCreator.h"
#include "DataRecord/RecordFixBin.h"
#include "DataScanner/ScannerCreator.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataCollector/DataCollectorCreator.h"
#include "DataCollector/DataCollectorMgr.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterClt/CounterClt.h"
//#include "CounterServer/PhyCtnrSvr.h"
//#include "CounterServer/AllCounterProc.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "DataStore/StoreMgr.h"
#include "DbQuery/Query.h"
#include "Debug/Debug.h"
#include "DataCalc/DataCalc.h"
#include "DocProc/DocProc.h"
#include "DocClient/DocClient.h"
#include "DocMgr/Ptrs.h"
#include "DocMgr/DocMgr.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvr.h"
#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "Job/JobMgr.h"
#include "MultiLang/LangDictMgr.h"
#include "MySQL/Ptrs.h"
#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/AllTerms.h"
#include "Query/QueryReq.h"
#include "QueryClient/QueryClient.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "BitmapMgr/Bitmap.h"
#include "TransClient/ServerMgr.h"
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/FrontEndSvr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/SiteMgr.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataCollectorObj.h"
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
#include "SEInterfaces/CounterCltObj.h"
#include "SEInterfaces/CounterSvrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/DataCollectorObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SystemMonitor/SysMonitor.h"
#include "DataCollector/DataCollectorCreator.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SecUtil/Access.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "SoapServer/SoapServer.h"
#include "TaskMgr/TaskCreator.h"
#include "TaskMgr/TaskDataCreator.h"
#include "TaskMgr/TaskMgr.h"
#include "TransClient/TransClient.h"
#include "Util/OmnNew.h"
#include "Util/ExeReport.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILScanner/IILScanner.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransServer.h"
#include "TransServer/TransSvrConn.h"
#include "IILTransServer/IILTransServer.h"
#include "IILTransServer/Ptrs.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "StorageMgr/NetFileMgr.h"
//#include "StorageMgr/NetFileClient.h"
//#include "StorageMgr/NetFileServer.h"
#include "SmartDoc/SmartDocImpl.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/TcpServerEpoll.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordTransClient/WordTransClient.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
//#include "WordParser/WordParserEngine.h"
#include "VersionServer/VersionServer.h"
#include "UserMgmt/UserMgmt.h"
#include "Python/Python.h"
#include "Conds/AllConds.h"
#include "SEUtil/SEUtilInit.h"
#include "Thread/ThreadMgr.h"
//#include "Thread/ThreadShellMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "StoreQuery/StoreQueryMgr.h"
#include "SEServer/Local/AllTrans.h"
#include "ReliableFile/AllReplicPolicy.h"
#include "QueryClient/BatchQueryReader.h"

// Need ContecEcgProc
#include "CustomizedProcs/Contec/Decl.h"
#include "CustomizedProcs/INotify/INotifyMgr.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

bool AosExitSearchEngine();

static OmnString sgBuildNum = "2012-03-03.1";
static OmnString sgBuildTime = "2012/03/13 15:42";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = " 99 : /home/jackie/AOS/src/SEServer/Local/";

static bool needStop = false;
static int sgStatusFreq = 600;

int redirectOutPut();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	//cout << "value: " << value << endl;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012

//bool gSvrIsUp = false;           // Ketty 2012/11/28

bool sgIsBigIILServer = false;		// Ken Lee, 2012/07/17
bool gNeedBackup = false;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);

	OmnApp theApp(argc, argv);

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
		
		if (strcmp(argv[index], "-needbackup") == 0)
		{
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

		// Ken Lee, 2013/01/09
		if(strcmp(argv[index], "-memorycheck") == 0 && index < argc-1)
		{
			sgStatusFreq = atoi(argv[index+1]);
			if(sgStatusFreq <= 0) sgStatusFreq = 600;
			index += 2;
			continue;
		}

		index++;
	}

	AosAccess::init();
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);
	
	AosBitmapObj::setObject(OmnNew AosBitmap());

	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}

	// Chen Ding, 01/01/2013
	AosSEUtilInit::init(app_config);
	AosXmlTagPtr counter_client = app_config->getFirstChild("CounterClt");
	AosXmlTagPtr msgclient =  app_config->getFirstChild("msgclient");
	AosXmlTagPtr seLogClt =  app_config->getFirstChild("SeLogClt");
//	OmnDataStoreMySQL::startMySQL(argc, argv);

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	//AosDocFileMgrObj::setCreator(OmnNew AosDocFileMgr(AosMemoryCheckerArgsBegin));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false AosMemoryCheckerArgs));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());
	AosDataCollectorObj::setDataCollectorCreator(OmnNew AosDataCollectorCreator());
	AosTaskObj::setCreator(OmnNew AosTaskCreator());
	AosTaskDataObj::setCreator(OmnNew AosTaskDataCreator());
	AosJobMgrObj::setJobMgr(AosJobMgr::getSelf());

	AosQueryReqObj::setObject(OmnNew AosQueryReq());
	AosQueryContext::initQueryContext();
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(new AosSmartDoc("template", AosSdocId::eInvalid, false));
	// AosActionObj::setActionObj(new AosSdocActionObj());
	AosDocSelObj::setDocSelector(new AosDocSelObjImpl());
	AosLoginMgrObj::setLoginMgrObj(new AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(new AosUserMgmt());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosIILMgrObj::setIILMgr(AosIILMgr::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());			// Ketty 2013/03/12
	AosVfsMgrObj::setVfsMgr(AosVirtualFileSysMgr::getSelf());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());			// Ketty 2013/03/12
	AosTransClientObj::setTransClient(AosTransClient::getSelf());	// Ketty 2013/03/12
	AosTransServerObj::setTransServer(AosTransServer::getSelf());	// Ketty 2013/03/12
	AosTransSvrConnObj::setSvrConn(AosTransSvrConn::getSelf());		// Ketty 2013/03/12
	AosServerMgrObj::setServerMgr(AosServerMgr::getSelf());			// Ketty 2013/03/12
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());			// Ken Lee, 2013/04/15
	
	// Ketty, 2012/11/07
	//AosTransModuleSvr::init(app_config->getFirstChild(AOSCONFIG_TRANS_SERVER));
	//AosTransModuleSvr::start();
	//AosTransModuleClt::init(app_config->getFirstChild(AOSCONFIG_TRANS_CLIENT));
	//AosTransModuleClt::start();

	//AosVirtualFileSysMgr::getSelf()->config(app_config);
	//AosServerMgr::getSelf()->start();
	//AosVirtualFileSysMgr::getSelf()->start();

	//OmnThreadShellMgr::getSelf()->start();

	// Chen Ding, 2013/02/21
	//AosValueSel::init();

	//AosMemoryChecker::getSelf()->start();

	// Ketty 2013/03/25
	AosAllReplicPolicy::init();
	AosAllTrans::init();
	
	// Ken Lee, 2013/04/12
	AosDataAssembler::init(app_config);

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosNetworkMgrObj::config(app_config);
	AosVirtualFileSysMgr::getSelf()->config(app_config);
	AosTransClient::getSelf()->config(app_config);	// Ketty 2013/03/12
	AosTransServer::getSelf()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrConn::getSelf()->config(app_config);	// Ketty 2013/03/12
	
//	AosWordParserEngine::getSelf()->config(app_config);
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	//AosStorageMgr::getSelf()->config(app_config);
	AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosWordMgr1::getSelf()->config(app_config);
	AosIILMgr::getSelf()->config(app_config);
	AosFrontEndSvr::getSelf()->config(app_config);
	AosDocMgr1::getSelf()->config(app_config);
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
	AosMsgClient::getSelf()->config(app_config);
	AosSiteMgr::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
	//AosSengAdmin::getSelf()->config(app_config);
//	AosSoapServer::getSelf()->config(app_config);
	AosDataCollectorMgr::getSelf()->config(app_config);
	AosAllStorageEngines::init(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);
	AosPhyLogSvr::getSelf()->config(app_config);		// Ketty 2013/03/12
	//AosStorageEngineObj::storageEngineInited();

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
		counterserver = OmnNew AosPhyCtnrSvr();
		aos_assert_r(counterserver, 0);
		
		// Ketty 2013/03/28
		AosCounterSvrObj::setCounterSvr(counterserver);
		counterserver->config(app_config);
	}
	
	AosTransClient::getSelf()->start();
	AosTransServer::getSelf()->start();	// Ketty 2013/03/12
	AosTransSvrConn::getSelf()->start();	// Ketty 2013/03/12
	AosServerMgr::getSelf()->start();
	AosVirtualFileSysMgr::getSelf()->start();
	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();

	AosBatchQueryReader::getSelf()->start();

	//OmnThreadShellMgr::getSelf()->start();
	// Chen Ding, 2013/02/21
	AosValueSel::init();
	AosMemoryChecker::getSelf()->start();

#if AOSCUSTOMIZEPROC_CONTEC
#include "CustomizedProcs/Contec/ContecEcgProc.h"
#include "CustomizedProcs/Contec/ProcEcg.h"
#include "CustomizedProcs/Contec/SendToMsgService.h"
	static AosProcEcg lsAosProcEcg(true);
	static AosSendToMsgService lsAosSendToMsgService(true);
#endif

	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();
	OmnScreen << "...........Start IILMgr........." << endl;
	AosIILMgr::getSelf()->start();
	OmnScreen << "...........Start IILMgr Finished........." << endl;

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();
	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	// Ketty 2013/03/12
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvr::getSelf()->start();
	//AosPhyLogSvrPtr logsvr = OmnNew AosPhyLogSvr(app_config);
	//aos_assert_r(logsvr, 0);
	//logsvr->start();
	
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

		
	OmnScreen << "...........Start SessionMgr........." << endl;
	AosSessionMgr::getSelf()->start();  
	OmnScreen << "...........Start Python........." << endl;
	AosPython::getSelf()->start();  
	OmnScreen << "...........Start Securitymgr........." << endl;
	AosSecurityMgr::getSelf()->start();

	// Ketty 2012/11/28
	OmnScreen << "Self Is Up wait other svrs.!!" << endl;
	AosServerMgr::getSelf()->notifyIamUp();
	//gSvrIsUp = true;
	//AosServerMgr::getSelf()->waitAllSvrUp();
	
	OmnScreen << "ServerIsUp !!" << endl;
	

	OmnScreen << "...........Start SiteMgr........." << endl;
	AosSiteMgr::getSelf()->start();
	//AosSoapServer::getSelf()->start();
	AosXmlTagPtr seconfig = app_config->getFirstChild("seserver");
	aos_assert_r(seconfig, false);
	AosSeReqProc::config(seconfig);
	AosINotifyMgr::getSelf()->config(app_config);

	AosStoreQueryMgr::getSelf()->start();

	AosSeReqProcPtr proc = OmnNew AosSeReqProc();
	AosReqDistr reqDistr(proc);
	OmnScreen << "...........Start ReqDistr........." << endl;
	if (!reqDistr.config(seconfig))
	{
		OmnAlarm << "Failed the configuration: " 
			<< app_config->toString() << enderr;
		theApp.exitApp();
		return -1;
	}
	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
		//AosSysMonitor::getSelf()->start();
	}


	index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-recoverword") == 0)                              
		{
			// '-recoverword word'
			AosWordMgr1::getSelf()->recoverWord(argv[index+1], strlen(argv[index+1]));
			index += 2;
			continue;
		}

		index ++;
	}

	reqDistr.start();
	
	OmnScreen << "...........All Services Started........." << endl;

	AosObjMgrObj::createObjsAtStartupStatic();	
	
	AosExeReport report(sgStatusFreq, 500000);

	AosIfcExampleTester();

	int num = 0; 
	while (1)
	{
		OmnSleep(1);
		if (num++ == sgStatusFreq)
		{
			AosMemoryChecker::getSelf()->report();

			// Print Epoll Server stat
			vector<OmnTcpServerEpollPtr> servers = OmnTcpServerEpoll::getEpollServers();
			if (servers.size() > 0)
			{
				for (u32 i=0; i<servers.size(); i++)
				{
					servers[i]->printStatus();
				}
			}
			num = 0;
		}
		if (needStop)
		{
			AosExitSearchEngine();
			break;
		}
	}

	theApp.exitApp();
	return 0;
}


bool AosExitSearchEngine()
{
	AosServerMgr::getSelf()->svrIsStopping(AosGetSelfServerId());

	OmnScreen << "To stop server!" << endl;
	AosSysMonitor::getSelf()->stop();

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

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVirtualFileSysMgr::getSelf()->stop();

	OmnScreen << "To TransClient!" << endl;
	AosTransClient::getSelf()->stop();	
	OmnSleep(2);
	OmnScreen << "To TransServer!" << endl;
	AosTransSvrConn::getSelf()->stop();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;
	return true;
}


int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	OmnString filename = "output_";

	FILE* ff = popen("hostname", "r");
	char buff[128];
	int len = fread(buff, 1, 128, ff); 
	buff[len-1]=0;
	filename << buff << ".txt";
	pclose(ff);
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}
