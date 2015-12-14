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
#include "util2/global_data.h"
#include "AppMgr/App.h"
#include "SEUtil/SEUtilInit.h"
#include "aosDLL/DllMgr.h"
#include "Accesses/AllAccesses.h"
#include "Accesses/Access.h"
//#include "SEServer/Local/AllTrans.h"
#include "DataAssembler/DataAssembler.h"
#include "DataField/DataFieldBool.h"
#include "Actions/AllActions.h"
#include "Conds/AllConds.h"
#include "Conds/CondTrue.h"
#include "ValueSel/ValueSel.h"
#include "BitmapMgr/Bitmap.h"
#include "DataRecord/RecordFixBin.h"
#include "DataProc/DataProcCount.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataScanner/ScannerCreator.h"
#include "DataBlob/BlobRecord.h"
#include "QueryUtil/QueryContext.h"
#include "DataCollector/DataCollectorCreator.h"
#include "TaskMgr/TaskCreator.h"
#include "TaskMgr/TaskDataCreator.h"
#include "Job/JobMgr.h"
#include "Job/JobSplitterDir.h"
#include "Query/QueryReq.h"
#include "SEModules/ObjMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "DocSelector/DocSelObjImpl.h"
#include "SEModules/LoginMgr.h"
#include "UserMgmt/UserMgmt.h"
#include "DocMgr/DocMgr.h"
#include "IILClient/IILClient.h"
#include "DbQuery/Query.h"
#include "SeLogClient/SeLogClient.h"
#include "StorageEngine/SizeIdMgr.h"
#include "CounterClt/CounterClt.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "WordMgr/WordMgr1.h"
#include "StorageMgr/NetFileMgr.h"
#include "SvrProxyMgr/IpcClt.h"
#include "BitmapEngine/BitmapEngine.h"
#include "IILScanner/IILScanner.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "DocProc/DocProc.h"
#include "Security/SessionMgr.h"
#include "Python/Python.h"
#include "Security/SecurityMgr.h"
#include "Timer/TimerMgr.h"
#include "MsgClient/MsgClient.h"
#include "SEModules/SiteMgr.h"
#include "DataCollector/DataCollectorMgr.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "TaskMgr/TaskMgr.h"
#include "Schema/SchemaDummy.h"
#include "SchemaPicker/SchemaPickerDummy.h"
#include "Dataset/DatasetDummy.h"
#include "DataScanner/DataScannerDummy.h"
#include "DataCube/DataCubeDummy.h"
//#include "QueryClient/BatchQueryReader.h"
#include "StoreQuery/StoreQueryMgr.h"
#include "SEServer/SeReqProc.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Util/ExeReport.h"
#include "SystemMonitor/SysMonitor.h"
#include "QueryClient/QueryClient.h"
#include "UtilComm/TcpServerEpoll.h"
#include "VersionServer/VersionServer.h"
#include "IILMgr/IILMgr.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "DocServer/DocSvr.h"
#include "SEModules/FrontEndSvr.h"
#include "CounterServer/PhyCtnrSvr.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "TransClient/TransClient.h"
#include "TransServer/TransServer.h"

#include "SEInterfaces/ConditionObj.h"
#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataCollectorObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/JobSplitterObj.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/ObjMgrObj.h"
#include "SEInterfaces/SmartDocObj.h"
#include "SEInterfaces/DocSelObj.h"
#include "SEInterfaces/LoginMgrObj.h"
#include "SEInterfaces/UserMgmtObj.h"
#include "SEInterfaces/DocMgrObj.h"
#include "SEInterfaces/QueryObj.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/SizeIdMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/WordMgrObj.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/PhyLogSvrObj.h"
//#include "SEInterfaces/DatasetObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/DllMgrObj.h"

#include "SysMsg/SetProcStatusMsg.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"

#include "CustomizedProcs/INotify/INotifyMgr.h"

#include "DocTrans/AllDocTrans.h"
#include "IILTrans/AllIILTrans.h"
#include "LogTrans/AllLogTrans.h"
#include "JobTrans/AllJobTrans.h"
#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "CounterUtil/AllCounterTrans.h"
//#include "Schema/Schema.h"
//#include "SchemaPicker/SchemaPicker.h"
//#include "Dataset/DatasetByDataScanner.h"
#include "DataRecord/Recordset.h"

#include "Dataset/Testers/TestSuite.h"
#include "Tester/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>


bool AosExitSearchEngine();

static OmnString sgBuildNum = "2012-03-03.1";
static OmnString sgBuildTime = "2012/03/13 15:42";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = " 99 : /home/jackie/AOS/src/SEServer/Local/";

static bool needStop = false;
static int sgStatusFreq = 600;

int redirectOutPut();
void transInit();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;
int gAosShowNetTraffic = 1;

bool sgIsBigIILServer = false;
bool gNeedBackup = false;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"


static bool sgNeedCleanCache = false;
static int sgCleanCacheTime = 30 * 60;

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	//aos_alarm_init(1);

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");

	OmnApp theApp(argc, argv);

	int index = 1;
	bool isRepairing = false;
	while (index < argc)
	{
		if (strcmp(argv[index], "-rdcout") == 0)
		{
			redirectOutPut();
			break;
		}
		index++;
	};

	index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-debug_wait") == 0)
		{
			int i = 60;
			while(i)
			{
				OmnScreen << "Debug waiting : pid = " << getpid() << endl;
				i--;
				OmnSleep(1);
			}
			break;
		}
		index++;
	};


	index = 1;                                                   
	AosXmlTagPtr app_config;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			break;
		}
		index++;
	}

	aos_assert_r(app_config, -1);

	index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-log") == 0 && index < argc-1)
		{
			app_config->setAttr("log_level", atoi(argv[index+1]));
			index += 2;
			continue;
		}

		if(strcmp(argv[index], "-memorycheck") == 0 && index < argc-1)
		{
			app_config->setAttr("memorycheck", atoi(argv[index+1]));
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-alarm") == 0)
		{
			aos_alarm_init(1);
			index ++;
			continue;
		}

		index++;
	}

	OmnApp::setConfig(app_config);

	gAosLogLevel = app_config->getAttrInt("log_level", 1);
	sgStatusFreq = app_config->getAttrInt("memorycheck", 3600);

	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}
	AosRundataPtr rdata = OmnApp::getRundata();
	aos_assert_r(rdata, -1);
	AosAccess::init(rdata);
	AosSEUtilInit::init(app_config);
	AosQueryContext::initQueryContext();
	//AosAllTrans::init();
	transInit();
	AosDataAssembler::init(app_config);
	AosAllStorageEngines::init(app_config);
	AosAllActions::init();
	AosValueSel::init();
	AosInitConditions();

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	AosDataField::init();
	AosDllMgrObj::setDllMgr(AosDllMgr::getSelf());

	//AosSchemaPickerObj::setObject(OmnNew AosSchemaPicker(false));
	//AosSchemaObj::setObject(OmnNew AosSchema());
//	AosDatasetObj::setObject(OmnNew AosDatasetByDataScanner(false));
	AosRecordsetObj::setObject(OmnNew AosRecordset());
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false AosMemoryCheckerArgs));
	AosDataProcObj::setDataProc(OmnNew AosDataProcCount(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
//	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());
	AosDataCollectorObj::setDataCollectorCreator(OmnNew AosDataCollectorCreator());
	AosTaskObj::setCreator(OmnNew AosTaskCreator());
	AosTaskDataObj::setCreator(OmnNew AosTaskDataCreator());
	AosJobMgrObj::setJobMgr(AosJobMgr::getSelf());
	AosTaskMgrObj::setTaskMgr(AosTaskMgr::getSelf());
	AosJobSplitterObj::setJobSplitter(OmnNew AosJobSplitterDir(false));
	AosQueryReqObj::setObject(OmnNew AosQueryReq());
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(OmnNew AosSmartDoc("template", AosSdocId::eInvalid, false));
	AosDocSelObj::setDocSelector(OmnNew AosDocSelObjImpl());

	AosLoginMgrObj::setLoginMgrObj(OmnNew AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(OmnNew AosUserMgmt());
	//AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	//AosIILMgrObj::setIILMgr(AosIILMgr::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	//AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	//AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());
	AosBitmapEngineObj::setBitmapEngine(AosBitmapEngine::getSelf());
	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	//AosCounterSvrObj::setCounterSvr(OmnNew AosPhyCtnrSvr());
	//(AosCounterSvrObj::getCounterSvr())->config(app_config);

	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosDllMgr::getSelf()->config(app_config);
	AosNetFileMgr::getSelf();
	AosVfsMgrObj::getVfsMgr()->config(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12
	
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	//AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	//AosWordMgr1::getSelf()->config(app_config);
	//AosIILMgr::getSelf()->config(app_config);
	//AosFrontEndSvr::getSelf()->config(app_config);
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
	AosDataCollectorMgr::getSelf()->config(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);
	//AosPhyLogSvr::getSelf()->config(app_config);
	AosINotifyMgr::getSelf()->config(app_config);

	AosSchemaDummy schema(0);
	AosSchemaPickerDummy schema_picker(0);
	AosDatasetDummy dataset(0);
	AosDataScannerDummy datascanner(0);
	AosDataCubeDummy datacube(0);

	//------------start----------
	AosVfsMgrObj::getVfsMgr()->start();
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();

	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();
	//AosBatchQueryReader::getSelf()->start();
	AosMemoryChecker::getSelf()->start();

	//OmnScreen << "...........Start IILIDIdGen........." << endl;
	//AosIILIDIdGen::getSelf()->start();

	//OmnScreen << "...........StartWordMgr1........." << endl;
	//AosWordMgr1::getSelf()->start();

	//OmnScreen << "...........Start DocSvr........." << endl;
	//AosDocSvr::getSelf()->start();

	//OmnScreen << "...........Start IILMgr........." << endl;
	//AosIILMgr::getSelf()->start();
	//OmnScreen << "...........Start IILMgr Finished........." << endl;

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();

	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();

	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	//OmnScreen << "...........Start PhyLogSvr........." << endl;
	//AosPhyLogSvr::getSelf()->start();

	OmnScreen << "...........Start LogClient........." << endl;
	AosSeLogClient::getSelf()->start();

	OmnScreen << "...........Start CounterClient........." << endl;
	AosCounterClt::getSelf()->start();

	OmnScreen << "...........Start SessionMgr........." << endl;
	AosSessionMgr::getSelf()->start();  

	OmnScreen << "...........Start Python........." << endl;
	AosPython::getSelf()->start();  

	OmnScreen << "...........Start Securitymgr........." << endl;
	AosSecurityMgr::getSelf()->start();

	OmnScreen << "...........Start SiteMgr........." << endl;
	AosSiteMgr::getSelf()->start();

	AosStoreQueryMgr::getSelf()->start();

	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
	}

	AosXmlTagPtr seconfig = app_config->getFirstChild("seserver");
	aos_assert_r(seconfig, false);
	AosSeReqProc::config(seconfig);

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

	if (AosGetSelfServerId() == 0)
	{
		reqDistr.start();
	}
	
	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
	AosExeReport report(sgStatusFreq, 500000);
	OmnScreen << "...........All Services Started........." << endl;

//	AosAppMsgPtr msg = OmnNew AosSetProcStatusMsg(AosProcInfo::eStarted, 0, 1);
//	AosSendMsg(msg);
	
	
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DatasetTester", "Try", "Felicia");
	testMgr->addSuite(AosDatasetTestSuite::getSuite());
	
	cout << "Start Testing ..." << endl;
	
	testMgr->start();
	
	cout << "\nFinished. " << testMgr->getStat() << endl;
	
	testMgr = 0;

	int num = 0;
	int num_cleancache = 0;
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
	OmnScreen << "To stop TransServer!" << endl;
	AosTransSvrObj::getTransSvr()->stop();	// Ketty 2013/03/12

	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgr::getSelf()->stop();
	OmnSleep(1);

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryClient::getSelf()->stop();

	OmnScreen << "To stop Requester!" << endl;
	AosSeReqProc::stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	//OmnScreen << "To stop IILMgr!" << endl;
	//AosIILMgr::getSelf()->stop();

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;

//	AosAppMsgPtr msg = OmnNew AosSetProcStatusMsg(AosProcInfo::eStopped, 0, 1);
//	AosSendMsg(msg);
	OmnSleep(1);

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();

	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;
	return true;
}


int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   

	OmnString filename = OmnApp::getAppBaseDir();
	filename << "frontend";

	char buff[128];
	struct tm * ptm;
	time_t rawtime;
	time(&rawtime);
	ptm = gmtime(&rawtime);
	sprintf(buff, "%02d%02d%02d%02d%02d", ptm->tm_mon+1, 
			ptm->tm_mday, (ptm->tm_hour+8)%24, ptm->tm_min, ptm->tm_sec);
	filename << buff << ".txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}
	return 0;
}

void transInit()
{
	AosAllDocTrans::init();
	AosAllIILTrans::init();
	AosAllLogTrans::init();
	AosAllJobTrans::init();
	AosAllSysTrans::init();
	AosAllSysMsg::init();
	AosAllCounterTrans::init();
}

