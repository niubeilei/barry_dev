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
#include "Actions/AllActions.h"
#include "ValueSel/AllValueSel.h"
#include "SEInterfaces/ValueSelObj.h"
#include "Conds/AllConds.h"
#include "Conds/CondTrue.h"
#include "DataAssembler/DataAssembler.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "DataBlob/BlobRecord.h"
#include "DataField/DataFieldCreator.h"
#include "DataProc/DataProcCount.h"
#include "DataRecord/RecordFixBin.h"
#include "DataScanner/ScannerCreator.h"
#include "DataCacher/DataCacherCreator.h"
#include "DataCollector/DataCollectorCreator.h"
#include "DataCollector/DataCollectorMgr.h"
#include "SeReqProc/AllSeReqProcs.h"
#include "Alarm/Alarm.h"
#include "Actions/SdocActionObj.h"
#include "AppMgr/App.h"
#include "BitmapEngine/BitmapEngine.h"
#include "CounterServer/Ptrs.h"
#include "CounterServer/VirCtnrSvr.h"
#include "CounterClt/CounterClt.h"
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
#include "RemoteBackupClt/RemoteBackupClt.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "SEModules/ObjMgr.h"
#include "SEModules/FrontEndSvr.h"
#include "SEModules/SeRundata.h"
#include "SEModules/SiteMgr.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "SEInterfaces/BitmapEngineObj.h"
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
#include "SEInterfaces/JobSplitterObj.h"
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
#include "Util/OmnNew.h"
#include "Util/ExeReport.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILClient/IILClient.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "IILScanner/IILScanner.h"
#include "SvrProxyMgr/IpcClt.h"
#include "SeLogClient/SeLogClient.h"
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "StorageMgr/NetFileMgr.h"
#include "SmartDoc/SmartDocImpl.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "UtilComm/Tcp.h"
#include "UtilComm/TcpServerEpoll.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"
#include "VersionServer/VersionServer.h"
#include "UserMgmt/UserMgmt.h"
#include "Python/Python.h"
#include "SEUtil/SEUtilInit.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "StoreQuery/StoreQueryMgr.h"
#include "SEServer/Local/AllTrans.h"
#include "ReliableFile/AllReplicPolicy.h"
#include "QueryClient/BatchQueryReader.h"
#include "Job/JobSplitterDir.h"
#include "CustomizedProcs/INotify/INotifyMgr.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>
#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"

bool AosExitSearchEngine();

static OmnString sgBuildNum = "2012-03-03.1";
static OmnString sgBuildTime = "2012/03/13 15:42";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = " 99 : /home/jackie/AOS/src/SEServer/Local/";
static bool needStop = false;
static int sgStatusFreq = 600;

static int redirectOutPut();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;
int gAosShowNetTraffic = 1;


int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	aos_alarm_init(1);

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);

	OmnApp theApp(argc, argv);

	int index = 1;
	bool isRepairing = false;

	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-rdcout") == 0)
		{
			redirectOutPut();
			index ++;
			continue;
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

		if (strcmp(argv[index], "-show_net") == 0 && index < argc-1)
		{
			gAosShowNetTraffic = atoi(argv[index+1]);
			index += 2;
			continue;
		}

		if (strcmp(argv[index], "-ignored_ports") == 0 && index < argc-1)
		{
			OmnTcp::setIgnoredPorts(argv[index+1]);
			index += 2;
			continue;
		}

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
	
	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}

	AosSEUtilInit::init(app_config);
	AosXmlTagPtr counter_client = app_config->getFirstChild("CounterClt");
	AosXmlTagPtr msgclient =  app_config->getFirstChild("msgclient");
	AosXmlTagPtr seLogClt =  app_config->getFirstChild("SeLogClt");

	OmnScreen << "...........Set Interface Objects........." << endl;
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false AosMemoryCheckerArgs));
	AosDataProcObj::setDataProc(OmnNew AosDataProcCount(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());
	AosDataCollectorObj::setDataCollectorCreator(OmnNew AosDataCollectorCreator());
	AosTaskObj::setCreator(OmnNew AosTaskCreator());
	AosTaskDataObj::setCreator(OmnNew AosTaskDataCreator());
	AosJobMgrObj::setJobMgr(AosJobMgr::getSelf());
	AosJobSplitterObj::setJobSplitter(OmnNew AosJobSplitterDir(false));
	AosQueryReqObj::setObject(OmnNew AosQueryReq());
	AosQueryContext::initQueryContext();
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(new AosSmartDoc("template", AosSdocId::eInvalid, false));
	AosDocSelObj::setDocSelector(new AosDocSelObjImpl());
	AosLoginMgrObj::setLoginMgrObj(new AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(new AosUserMgmt());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());			// Ketty 2013/03/12
	AosVfsMgrObj::setVfsMgr(AosVirtualFileSysMgr::getSelf());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());			// Ketty 2013/03/12
	AosNetFileMgr::getSelf();			// Ken Lee, 2013/04/15
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());			// Ken Lee, 2013/04/15
	
	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosAllReplicPolicy::init();
	AosAllTrans::init();
	AosDataAssembler::init(app_config);

	OmnScreen << "...........Config........." << endl;
	AosNetworkMgrObj::config(app_config);
	AosVirtualFileSysMgr::getSelf()->config(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosWordMgr1::getSelf()->config(app_config);
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
	AosDataCollectorMgr::getSelf()->config(app_config);
	AosAllStorageEngines::init(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);
	AosPhyLogSvr::getSelf()->config(app_config);		// Ketty 2013/03/12
	AosBitmapEngineObj::setBitmapEngine(AosBitmapEngine::getSelf());
	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	AosAllActions::init();
	AosInitConditions();
	AosIpcCltObj::getIpcClt()->start();
	AosVirtualFileSysMgr::getSelf()->start();
	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();
	AosBatchQueryReader::getSelf()->start();
	AosValueSel::init();
	AosMemoryChecker::getSelf()->start();

	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();
	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvr::getSelf()->start();
	
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
	}

	index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-datasync") == 0)
		{
			OmnString attrname = argv[index+1];
			OmnString tag = argv[index+2];
			OmnString savefile = argv[index+3];
			OmnString sdocstr = "<smartdoc zky_otype=\"zky_smtdoc\" zky_sdoctp=\"DataSync\" zky_modify_sdoc=\"false\" remote_addr=\"112.25.131.18\" remote_port=\"5555\" zky_sync_ctnrs=\"";
			sdocstr << tag << "\" zky_attrname=\"" << attrname 
				<< "\" zky_sync_save=\"" << savefile << "\"/>";
			
			AosXmlTagPtr sdoc = AosXmlParser::parse(sdocstr AosMemoryCheckerArgs);
			if (!sdoc) exit(0);
			AosRundataPtr rdata = OmnApp::getRundata();
			rdata->setReceivedDoc(sdoc, true);
			rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
			rdata->setSiteid(100);		// Ketty 2013/01/25
			AosSmartDocObj::runSmartdocStatic(sdoc, rdata);
			OmnScreen << "System Data Sync finished!" << endl;
		}

		if (strcmp(argv[index], "-savedocs") == 0)
		{
			// recover fixed length docs
			AosRundataPtr rdata = OmnApp::getRundata();
			rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata)); 
			rdata->setSiteid(100);
			vector<OmnString> dir_path;
			int idx = index +1;
			while (idx < argc)
			{
				OmnString path = argv[idx];
				dir_path.push_back(path);
				idx ++;
			}
			//AosStorageEngineMgr::getSelf()->recoverFixedLengthDoc(dir_path, rdata);
			OmnScreen << "save doc finished!" << endl;
		}

		index ++;
	}

	reqDistr.start();
	
	OmnScreen << "...........All Services Started........." << endl;
	AosObjMgrObj::createObjsAtStartupStatic();	
	
	AosExeReport report(sgStatusFreq, 500000);

	int num = 0;
	while (1)
	{
		OmnSleep(1);
		if (num++ == sgStatusFreq)
		{
			AosMemoryChecker::getSelf()->report();

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
	
	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVirtualFileSysMgr::getSelf()->stop();

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();
	
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
	filename << "output_";
	filename << getpid() << ".txt";

	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}

