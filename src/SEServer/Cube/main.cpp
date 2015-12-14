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
#include "BitmapMgr/Bitmap.h"
#include "BitmapMgr/BitmapMgr.h"
#include "DataAssembler/DataAssembler.h"
#include "Actions/AllActions.h"
#include "AggrFuncs/AggrFuncMgr.h"
#include "BitmapTrans/AllBitmapTrans.h"
#include "Conds/AllConds.h"
#include "Conds/CondTrue.h"
#include "DocClient/DocClient.h"
#include "ValueSel/ValueSel.h"
#include "DocFileMgr/DfmLog.h"
#include "DataRecord/RecordFixBin.h"
#include "DataRecord/RecordCSV.h"
#include "DataProc/DataProcCount.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataScanner/ScannerCreator.h"
#include "DataBlob/BlobRecord.h"
#include "QueryUtil/QueryDistrMap.h"
#include "QueryUtil/QueryContext.h"
#include "QueryUtil/QueryConfig.h"
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
#include "NetFile/NetFileClt.h"
#include "NetFile/NetFileMgr.h"
#include "SvrProxyMgr/IpcClt.h"
#include "IILScanner/IILScanner.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "DocClient/DocProc.h"
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
#include "QueryClient/BatchQueryReader.h"
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
#include "TransServer/CubeTransSvrMgr.h"
#include "SEInterfaces/QueryCacherObj.h"
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
#include "SEInterfaces/CubeTransSvrObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/IILScannerObj.h"
#include "SEInterfaces/VersionServerObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/PhyLogSvrObj.h"
#include "SEInterfaces/StatSvrObj.h"
#include "SEInterfaces/DocidMgrObj.h"
#include "DocClient/DocidMgr.h"
#include "SysMsg/SetProcStatusMsg.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
#include "StatServer/StatSvr.h"
#include "StatUtil/StatTimeUnit.h"
#include "StreamFS/SmallDocStore.h"
#include "JQLParser/JQLParser.h"

#include "DocTrans/AllDocTrans.h"
#include "IILTrans/AllIILTrans.h"
#include "LogTrans/AllLogTrans.h"
#include "JobTrans/AllJobTrans.h"
#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "CounterUtil/AllCounterTrans.h"
#include "StatTrans/AllStatTrans.h"


// Young, 2015/09/22
#include "SEInterfaces/SyncEngineObj.h"
#include "SyncEngine/SyncEngine.h"      
#include "JimoCallServer/JimoCallServer.h"
#include "DataField/DataFieldBool.h"
#include "ClusterMgr/ClusterMgr.h"
#include "SEInterfaces/ClusterObj.h"

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

extern AosJQLParser gAosJQLParser;

int redirectOutPut();
void transInit();
bool runTest(AosXmlTagPtr &test_config);

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;
int gAosShowNetTraffic = 1;

bool gNeedBackup = false;
bool gAosIsNormQuery = true;

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"


static bool sgNeedCleanCache = true;
static int sgCleanCacheTime = 10 * 60;

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
		
		if(strcmp(argv[index], "-crt_master") == 0 && index < argc-1)
		{
			AosXmlTagPtr net_conf = app_config->getFirstChild("networkmgr");
			aos_assert_r(net_conf, -1);
			
			net_conf->setAttr("crt_master", atoi(argv[index+1]));
			index += 2;
			continue;
		}

		index++;
	}

	OmnApp::setConfig(app_config);

	gAosLogLevel = app_config->getAttrInt("log_level", 2);
	sgStatusFreq = app_config->getAttrInt("status_freq", 3600);

	AosMemoryChecker::getSelf()->config(app_config);
	AosMemoryChecker::getSelf()->start();
	sgMemoryCheckOpen = AosMemoryChecker::getCheckOpen();
	sgMemoryCheckFreq = AosMemoryChecker::getCheckFreq();

	redirectOutPut();

	gAosJQLParser.lock();
	gAosJQLParser.unlock();

	AosSEUtilInit::init(app_config);
	AosQueryContext::initQueryContext();
	transInit();
	AosAllStorageEngines::init(app_config);
	AosInitConditions();

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosDocSvrObj::setDocSvr(AosDocSvr::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosLoginMgrObj::setLoginMgrObj(new AosLoginMgr(app_config, true));
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosIILMgrObj::setIILMgr(AosIILMgr::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSmallDocStoreObj::setSmallDocStoreObj(AosSmallDocStore::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());
	AosNetFileCltObj::setObj(AosNetFileClt::getSelf());
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());
	AosQueryDistrMapObj::setObject(OmnNew AosQueryDistrMap());
	AosBitmapMgrObj::setBitmapMgr(AosBitmapMgr::getSelf());
	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr());
	AosSiteMgrObj::setSiteMgr(AosSiteMgr::getSelf());
	AosBitmapMgr::getSelf()->config(app_config);
	AosQueryConfig::config(app_config); 
	AosCounterSvrObj::setCounterSvr(OmnNew AosPhyCtnrSvr());
	//AosAggrFuncObj::setAggrFuncObj(OmnNew AosAggrFuncMgr());
	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosCubeTransSvrObj::setTransSvr(OmnNew AosCubeTransSvrMgr());	// Ketty 2013/03/12
	AosStatSvrObj::setStatSvr(OmnNew AosStatSvr());			// Ketty 2014/02/24
	AosDataRecordObj::setCreator(OmnNew AosRecordCSV(false AosMemoryCheckerArgs));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	AosDataField::init();

	AosDocidMgrObj::setDocidMgr(OmnNew AosDocidMgr());

	AosStatTimeUnit::init();
	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosNetFileMgr::getSelf();
	AosVfsMgrObj::getVfsMgr()->config(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosWordMgr1::getSelf()->config(app_config);
	AosIILMgr::getSelf()->config(app_config);
	AosDocMgr1::getSelf()->config(app_config);
	AosDocClient::getSelf()->setRepairing(isRepairing);
	AosDocClient::getSelf()->config(app_config);
	AosIILClient::getSelf()->config(app_config);
	AosCounterClt::getSelf()->config(app_config);
	AosSeLogClient::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);
	AosPhyLogSvr::getSelf()->config(app_config);
	AosDfmLog::config(app_config);
	AosDocidMgrObj::getDocidMgr()->config(app_config);

	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();
	AosBitmapMgr::getSelf()->start();


	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();

	OmnScreen << "...........Start IILMgr........." << endl;
	AosIILMgr::getSelf()->start();
	OmnScreen << "...........Start IILMgr Finished........." << endl;

	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();

//	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvr::getSelf()->start();

	OmnScreen << "...........Start LogClient........." << endl;
	AosSeLogClient::getSelf()->start();

	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
	}
	
	app_config->setAttr("module_name", "cube");
	OmnAlarmProc::getSelf()->config(app_config);
	OmnAlarmProc::getSelf()->start();


	// cluster 
	//AosClusterMgrObj::setClusterMgr(OmnNew AosClusterMgr(1)); 

	//AosRundataPtr rdata = OmnApp::getRundata();
	//OmnScreen << "...........Start JimoCallServer........." << endl;
	//AosJimoCallServer::setSelf(OmnNew AosJimoCallServer(rdata.getPtr()));

	//// SyncEngine 
	//AosSyncEngineObj::setSyncEngine(OmnNew AosSyncEngine(1));     
	//AosSyncEngineObj::getSyncEngine()->config(app_config);

	//OmnScreen << "...........Start SyncEngine........." << endl;
	//AosSyncEngineObj::getSyncEngine()->start();


	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
	AosExeReport report(sgStatusFreq, 500000);
	OmnScreen << "...........All Services Started........." << endl;


	//check if it is for testing
	AosXmlTagPtr test_config;
	test_config = app_config->getFirstChild("test_config");
	if (test_config)
	{
		//this run is for testing purpose
		return runTest(test_config);
	}

	int num = 0;
	int num_memory_check = 0;
	int num_cleancache = 0;
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
		
		// Ken Lee, 2013/05/17
		if (sgNeedCleanCache && num_cleancache++ >= sgCleanCacheTime)
		{
			num_cleancache = 0;
			sgNeedCleanCache = false;
OmnScreen << "cleanCache=======================================================" << endl;
			AosIILMgr::getSelf()->cleanCache();
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

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryClient::getSelf()->stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	OmnScreen << "To stop IILMgr!" << endl;
	AosIILMgr::getSelf()->stop();

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();

	OmnScreen << "To TransServer!" << endl;
	AosTransSvrObj::getTransSvr()->stop();	// Ketty 2013/07/22
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;

	OmnSleep(1);

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
	filename << log_dir << dir_name << "_cubesever";
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


//test header files
//#include "StatServer/DynTester/StatServerTester.h"

bool runTest(AosXmlTagPtr &test_config)
{
	/*
	OmnString module;

	aos_assert_r(test_config, false);
	module = test_config->getAttrStr("module");
	if (module == "StatEngine")
	{
		//run stat engine testing
		AosStatServerTester tester;

		tester.setConf(test_config);
		return tester.start();
	}
	*/

	return true;
}
