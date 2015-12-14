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
//
#include "util2/global_data.h"
#include "AppMgr/App.h"
#include "SEUtil/SEUtilInit.h"
#include "Accesses/AllAccesses.h"
#include "Accesses/Access.h"
//#include "SEServer/Local/AllTrans.h"
#include "DataAssembler/DataAssembler.h"
#include "Actions/AllActions.h"
#include "Conds/AllConds.h"
#include "Conds/CondTrue.h"
#include "ValueSel/ValueSel.h"
#include "BitmapMgr/Bitmap.h"
#include "DocFileMgr/DfmLog.h"
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
#include "QueryClient/BatchQueryReader.h"
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
#include "TransServer/CubeTransSvr.h"
#include "FmtMgr/FmtMgr.h"
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

#include "DocTrans/AllDocTrans.h"
#include "IILTrans/AllIILTrans.h"
#include "LogTrans/AllLogTrans.h"
#include "JobTrans/AllJobTrans.h"
#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "CounterUtil/AllCounterTrans.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "DocFileMgr/DfmLogTester/TestSuite.h"

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

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"


static bool sgNeedCleanCache = true;
static int sgCleanCacheTime = 30 * 60;

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	aos_alarm_init(1);

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

		index++;
	}

	OmnApp::setConfig(app_config);

	gAosLogLevel = app_config->getAttrInt("log_level", 2);
	sgStatusFreq = app_config->getAttrInt("memorycheck", 3600);


	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}
	AosSEUtilInit::init(app_config);
	AosQueryContext::initQueryContext();
	//AosAllTrans::init();
	transInit();
	AosAllStorageEngines::init(app_config);

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosNetworkMgrObj::config(app_config);
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosIILMgrObj::setIILMgr(AosIILMgr::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());
	AosFmtMgrObj::setFmtMgr(OmnNew AosFmtMgr()); // Linda 2013/08/16
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());
	AosBitmapEngineObj::setBitmapEngine(AosBitmapEngine::getSelf());
	AosCounterSvrObj::setCounterSvr(OmnNew AosPhyCtnrSvr());
	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	//AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosCubeTransSvr());	// Ketty 2013/03/12

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosNetFileMgr::getSelf();
	AosFmtMgrObj::getFmtMgr()->config(app_config);
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


	//------------start----------
	//AosVfsMgrObj::getVfsMgr()->start();
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();		// will init crt master.
	AosFmtMgrObj::getFmtMgr()->start();
	AosMemoryChecker::getSelf()->start();

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

	OmnScreen << "...........Start IILClient........." << endl;
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

	OmnScreen << "...........All Services Started........." << endl;

	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
	
	AosExeReport report(sgStatusFreq, 500000);

	int tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DocFileMgr/StorageMgrTester", "Try", "Ketty");
	testMgr->setTries(tries);
	OmnString test = "Storage";
	testMgr->addSuite(AosDocFileMgrTestSuite::getSuite(test));

	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << (testMgr->getStat()).data() << endl;

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
	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryClient::getSelf()->stop();

	OmnScreen << "To stop DocServer!" << endl;
	AosDocClient::getSelf()->exitSearchEngine();
	
	OmnScreen << "To stop IILMgr!" << endl;
	AosIILMgr::getSelf()->stop();

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();

	OmnScreen << "To stop FmtMgr!" << endl;
	AosFmtMgrObj::getFmtMgr()->stop();

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();
	
	OmnScreen << "To TransServer!" << endl;
	AosTransSvrObj::getTransSvr()->stop();	// Ketty 2013/07/22
	
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

	OmnString filename = OmnApp::getAppBaseDir();
	filename << "cubesever";

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

