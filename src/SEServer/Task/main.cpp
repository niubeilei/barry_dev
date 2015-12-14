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
#include "Alarm/AlarmProc.h"
#include "util2/global_data.h"
#include "AppMgr/App.h"
#include "aosDLL/DllMgr.h"
#include "SEUtil/SEUtilInit.h"
#include "Accesses/AllAccesses.h"
#include "Accesses/Access.h"
#include "AggrFuncs/AggrFuncMgr.h"

#include "SyncEngineClient/SyncEngineClient.h"
#include "SEInterfaces/SyncEngineObj.h"
#include "SyncEngine/SyncEngine.h"      
#include "JimoCallServer/JimoCallServer.h"
#include "ClusterMgr/ClusterMgr.h"
#include "SEInterfaces/ClusterObj.h"

#include "DataAssembler/DataAssembler.h"
#include "DataAssembler/DataAssemblerSort.h"
#include "DataField/DataFieldBool.h"
#include "DocClient/DocClient.h"
#include "Actions/AllActions.h"
#include "AggrFuncs/AggrFuncMgr.h"
#include "Conds/AllConds.h"
#include "Conds/CondTrue.h"
#include "ValueSel/ValueSel.h"
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/BitmapMgr.h"
#include "BitmapTrans/AllBitmapTrans.h"
#include "DataRecord/RecordVariable.h"
#include "DataRecord/RecordFixBin.h"
#include "DataRecord/RecordCSV.h"
#include "DataProc/DataProcCount.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataScanner/ScannerCreator.h"
#include "DataBlob/BlobRecord.h"
#include "GroupQuery/MultiGroupQuery.h"
#include "QueryUtil/QueryContext.h"
#include "QueryUtil/QueryConfig.h"
#include "DataCollector/DataCollectorCreator.h"
#include "TaskMgr/TaskCreator.h"
#include "TaskMgr/TaskDataCreator.h"
#include "Job/JobMgr.h"
#include "Job/JobSplitterDir.h"
#include "JQLParser/JQLParser.h"
#include "Query/QueryReq.h"
#include "Query/QueryReqNew.h"
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
#include "WordMgr/WordMgr1.h"
#include "NetFile/NetFileClt.h"
#include "SvrProxyMgr/IpcClt.h"
#include "IILScanner/IILScanner.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "DocClient/DocProc.h"
#include "Sequence/SequenceMgr.h"
#include "Security/SessionMgr.h"
#include "Security/SecurityMgr.h"
#include "Timer/TimerMgr.h"
#include "SEModules/SiteMgr.h"
#include "DataCollector/DataCollectorMgr.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "TaskMgr/TaskMgr.h"
#include "Schema/SchemaDummy.h"
#include "Dataset/DatasetDummy.h"
#include "DataScanner/DataScannerDummy.h"
#include "DataCube/DataCubeDummy.h"
#include "DataSplitter/DataSplitterDummy.h"
#include "DataRecord/Recordset.h"

#include "StoreQuery/StoreQueryMgr.h"
//#include "SEServer/SeReqProc.h"
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
//#include "StatClient/StatClt.h"

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
#include "SEInterfaces/WordMgrObj.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/PhyLogSvrObj.h"
#include "SEInterfaces/RecordsetObj.h"
#include "SysMsg/NotifyProcIsUp.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
#include "BitmapTrans/AllBitmapTrans.h"
#include "DocTrans/AllDocTrans.h"
#include "IILTrans/AllIILTrans.h"
#include "LogTrans/AllLogTrans.h"
#include "JobTrans/AllJobTrans.h"
#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "CounterUtil/AllCounterTrans.h"
#include "StatTrans/AllStatTrans.h"
#include "Funcs/DummyFunc.h"

#include "DataCube/ReadDocUtil/ReadDocNoSortUnit.h"
#include "DataCube/ReadDocUtil/ReadDocBySortUnit.h"
//#include "StatUtil/StatRcdParser.h"
//#include "StatUtil/StatGrpbyRslt.h"
#include "JQLExpr/ExprString.h" 

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
static int sgStatusFreq = 3600;

static bool sgMemoryCheckOpen = true;
static int sgMemoryCheckFreq = 30;


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

// Chen Ding, 2014/07/30
extern AosJQLParser gAosJQLParser;

bool gNeedBackup = false;
bool gAosIsNormQuery = true;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"



int 
main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
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

		if (strcmp(argv[index], "-alarm") == 0)
		{
			aos_alarm_init(1);
			index ++;
			continue;
		}

		index++;
	}

	OmnApp::setConfig(app_config);

	gAosJQLParser.reset();
	gAosLogLevel = app_config->getAttrInt("log_level", 2);
	sgStatusFreq = app_config->getAttrInt("status_freq", 3600);

	AosMemoryChecker::getSelf()->config(app_config);
	AosMemoryChecker::getSelf()->start();
	sgMemoryCheckOpen = AosMemoryChecker::getCheckOpen();
	sgMemoryCheckFreq = AosMemoryChecker::getCheckFreq();

	redirectOutPut();

	AosRundataPtr rdata = OmnApp::getRundata();
	aos_assert_r(rdata, -1);
	AosAccess::init(rdata);
	AosSEUtilInit::init(app_config);
	AosQueryContext::initQueryContext();
	transInit();
	AosDataAssembler::init(app_config);
	AosAllStorageEngines::init(app_config);
	AosAllActions::init();
	AosValueSel::init();
	AosInitConditions();

	AosDummyFunc();

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosBitmapMgrObj::setBitmapMgr(AosBitmapMgr::getSelf());
	AosBitmapMgr::getSelf()->config(app_config);
	AosQueryConfig::config(app_config);

	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	AosDataField::init();
	AosSequenceMgrObj::setSequenceMgr(OmnNew AosSequenceMgr());
	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());		// Ketty 2013/12/27

	AosDataAssemblerObj::setDataAssembler(OmnNew AosDataAssemblerSort("sort", 0));
	AosRecordsetObj::setObject(OmnNew AosRecordset());
	//AosDataRecordObj::setCreator(OmnNew AosRecordVariable(false AosMemoryCheckerArgs));
	AosDataRecordObj::setCreator(OmnNew AosRecordCSV(false AosMemoryCheckerArgs));
	AosDataProcObj::setDataProc(OmnNew AosDataProcCount(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());
	AosDataCollectorObj::setDataCollectorCreator(NULL);//OmnNew AosDataCollectorCreator());
	AosTaskObj::setCreator(OmnNew AosTaskCreator());
	AosTaskDataObj::setCreator(OmnNew AosTaskDataCreator());
	AosJobMgrObj::setJobMgr(AosJobMgr::getSelf());
	AosTaskMgrObj::setTaskMgr(AosTaskMgr::getSelf());
	//AosJobSplitterObj::setJobSplitter(OmnNew AosJobSplitterDir(false));
	AosQueryReqObj::setObject(OmnNew AosQueryReq());
	AosQueryReqObj::setObjectNew(OmnNew AosQueryReqNew());
	AosGroupQueryObj::setObject(OmnNew AosMultiGroupQuery(0));
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(OmnNew AosSmartDoc("template", AosSdocId::eInvalid, false));
	AosDocSelObj::setDocSelector(OmnNew AosDocSelObjImpl());
	AosLoginMgrObj::setLoginMgrObj(OmnNew AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(OmnNew AosUserMgmt());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());

	// 2015/11/21
	AosExprObj::setExprStatic(OmnNew AosExprString("seed"));
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosSyncEngineClientObj::setSyncEngineClient(AosSyncEngineClient::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	//AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());
	AosNetFileCltObj::setObj(AosNetFileClt::getSelf());
	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	//AosAggrFuncObj::setAggrFuncObj(OmnNew AosAggrFuncMgr());

	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12
	//AosStatCltObj::setStatClt(OmnNew AosStatClt());	// Ketty 2013/03/12

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12

	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosDocMgr1::getSelf()->config(app_config);
	AosDocClient::getSelf()->setRepairing(isRepairing);
	AosDocClient::getSelf()->config(app_config);
	AosDocProc::getSelf()->config(app_config);
	AosIILClient::getSelf()->config(app_config);
	AosSessionMgr::getSelf()->config(app_config);  
	AosSecurityMgr::getSelf()->config(app_config);
	AosCounterClt::getSelf()->config(app_config);
	AosSeLogClient::getSelf()->config(app_config);
	//AosTimerMgr::getSelf()->config(app_config);
	AosSiteMgr::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
	//AosDataCollectorMgr::getSelf()->config(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);

	AosDatasetDummy dataset(0);
	AosDataScannerDummy datascanner(0);
	AosDataCubeDummy datacube(0);
	AosDataSplitterDummy datasplitter(0);

	AosReadDocNoSortUnit::include();
	AosReadDocBySortUnit::include();
	//AosStatRcdParser::include();
	//AosStatGrpbyRslt::include();

	//------------start----------
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();

	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();

//	OmnScreen << "...........Start IILClient........." << endl;
//	AosIILClient::getSelf()->start();
	
	OmnScreen << "...........Start LogClient........." << endl;
	AosSeLogClient::getSelf()->start();

	OmnScreen << "...........Start CounterClient........." << endl;
	AosCounterClt::getSelf()->start();

	OmnScreen << "...........Start SessionMgr........." << endl;
	AosSessionMgr::getSelf()->start();  

	OmnScreen << "...........Start Securitymgr........." << endl;
	AosSecurityMgr::getSelf()->start();

	OmnScreen << "...........Start BitmapMgr........." << endl;
	AosBitmapMgr::getSelf()->start();

	AosStoreQueryMgr::getSelf()->start();

	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
	}
	
	app_config->setAttr("module_name", "task");
	OmnAlarmProc::getSelf()->config(app_config);
	OmnAlarmProc::getSelf()->start();

	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
	AosExeReport report(sgStatusFreq, 500000);

	OmnScreen << "...........Notify task is up..........." << endl;
	AosAppMsgPtr msg = OmnNew AosNotifyProcIsUp(getpid());
	bool rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);




	// cluster 
	//AosClusterMgrObj::setClusterMgr(OmnNew AosClusterMgr(1)); 

	//OmnScreen << "...........Start JimoCallServer........." << endl;
	//AosJimoCallServer::setSelf(OmnNew AosJimoCallServer(rdata.getPtr()));

	//OmnScreen << "...........Start SyncEngineClient........." << endl;
	//AosSyncEngineClient::getSelf()->start();

	//// SyncEngine 
	//AosSyncEngineObj::setSyncEngine(OmnNew AosSyncEngine(1));     
	//AosSyncEngineObj::getSyncEngine()->config(app_config);

	//OmnScreen << "...........Start SyncEngine........." << endl;
	//AosSyncEngineObj::getSyncEngine()->start();


	OmnScreen << "...........All Services Started........." << endl;

	int num = 0;
	int num_memory_check = 0;
	while (1)
	{
		OmnSleep(1);
		if (getppid() ==1) exit(0);

		if (sgMemoryCheckOpen && num_memory_check++ >= sgMemoryCheckFreq)
		{
			num_memory_check = 0;
			AosMemoryChecker::getSelf()->report();
		}

		if (num++ == sgStatusFreq)
		{
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

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();

	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;
	return true;
}


int redirectOutPut()                                                        
{
	//JIMODB-1063
	//barry 2015/11/11
	return true;
	close(STDOUT_FILENO);   

	//Jozhi 2014/09/18 change all printing to [base_dir]/proc_log directory
	OmnString base_dir = OmnApp::getAppBaseDir();
	OmnString parent_dir = OmnApp::getParentDir(base_dir);
	OmnString dir_name = OmnApp::getDirName(base_dir);
	OmnString log_dir;
	log_dir << parent_dir << "/proc_log/";
	OmnApp::createDir(log_dir);
	OmnString filename;
	filename << log_dir << dir_name << "_task";
	setbuf(stdout, 0);

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
	AosAllBitmapTrans::init();
	AosAllDocTrans::init();
	AosAllIILTrans::init();
	AosAllLogTrans::init();
	AosAllJobTrans::init();
	AosAllSysTrans::init();
	AosAllSysMsg::init();
	AosAllStatTrans::init();
	AosAllCounterTrans::init();
}

