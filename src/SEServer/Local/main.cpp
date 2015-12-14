/////////////////////////////////////////////////////////////////////////////
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
#include "DataField/DataFieldBool.h"
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
#include "DocFileMgr/DocFileMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvr.h"
#include "FmtMgr/FmtMgr.h"
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
#include "SEInterfaces/TransSvrObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SystemMonitor/SysMonitor.h"
#include "SystemMonitor/ProcessMonitor.h"
#include "DataCollector/DataCollectorCreator.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "Accesses/AllAccesses.h"
#include "Accesses/Access.h"
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
#include "IILMgr/IILMgr.h"
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
#include "TransClient/TransClient.h"
#include "TransServer/TransServer.h"
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
#include "UserMgmt/UserDomain.h"
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
#include "QueryClient/BatchQueryReader.h"
#include "Job/JobSplitterDir.h"
#include "DocFileMgr/DfmLog.h"
#include "SysMsg/AddServerMsg.h"
#include "SysMsg/StartProcessMsg.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"

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
static int sgStatusFreq = 30;

int redirectOutPut();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012


bool sgIsBigIILServer = false;		// Ken Lee, 2012/07/17
bool gNeedBackup = false;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"


// Ken Lee, 2013/05/17
static bool sgNeedCleanCache = true;
static int sgCleanCacheTime = 30 * 60;

AosXmlTagPtr
getAdminConfig(const AosXmlTagPtr &svr_proxy_config)
{
	OmnString net_str;
	net_str << "<networkmgr "
			<< "zky_procid=\"1\" "
			<< "replic_policy=\"nobkp\" "
			<< "cube_grp_id=\"0\" "
			<< "proc_type=\"localserver\" "
			<< "vids=\"0\"><servers></servers></networkmgr>";
	AosXmlParser parser;
	AosXmlTagPtr admin_config = parser.parse(net_str, "" AosMemoryCheckerArgs);
	aos_assert_r(admin_config, 0);

	OmnString svr_str;
	svr_str << "<server>"
			<< "<Process proc_id=\"1\" "
			<< "proc_type=\"localserver\" "
			<< "cube_grp_id=\"0\"/></server>";
	AosXmlTagPtr svr_config = parser.parse(svr_str, "" AosMemoryCheckerArgs);
	aos_assert_r(svr_config, 0);


	AosXmlTagPtr servers_tag = admin_config->getFirstChild("servers");
	AosXmlTagPtr tags = svr_proxy_config->getFirstChild("servers");
	aos_assert_r(tags, 0);
	AosXmlTagPtr tag = tags->getFirstChild();
	while(tag)
	{
		int svr_id = tag->getAttrInt("server_id", -1);
		aos_assert_r(svr_id != -1, 0);

		AosXmlTagPtr clone_svr_tag = svr_config->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(clone_svr_tag, 0);
		clone_svr_tag->setAttr("server_id", svr_id);

		servers_tag->addNode(clone_svr_tag);

		tag = tags->getNextChild();
	}
	return admin_config;
}


bool
addServer(const AosRundataPtr &rdata)
{
	OmnFilePtr file = OmnNew OmnFile("config_network.txt",
			OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		return false;
	}
OmnScreen << "****************************add physical server***************************" << endl;
	OmnString str;
	file->readToString(str);
	AosXmlParser parser;
	AosXmlTagPtr svr_proxy_config = parser.parse(str, "" AosMemoryCheckerArgs);
	AosXmlTagPtr admin_config = getAdminConfig(svr_proxy_config);
	aos_assert_r(admin_config, false);

	bool rslt = false;
	AosXmlTagPtr sconfig, aconfig;
	sconfig = svr_proxy_config->clone(AosMemoryCheckerArgsBegin);
	aconfig = admin_config->clone(AosMemoryCheckerArgsBegin);
	
	u32 svr_id = AosGetSelfServerId();
	u32 clt_id = AosGetSelfClientId();
	sconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	sconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, svr_id);

	aconfig->setAttr(AOSTAG_PHYSICAL_SERVER_ID, svr_id);
	aconfig->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, clt_id);

	//rslt = AosNetworkMgrObj::getNetworkMgr()->configSelfSvr(aconfig);
	//aos_assert_r(rslt, false);
	rslt = AosNetworkMgrObj::getNetworkMgr()->configServers(sconfig);
	aos_assert_r(rslt, false);

	AosAppMsgPtr msg = OmnNew AosAddServerMsg(
			sconfig, aconfig, AosGetSelfServerId(), 0);
	rslt = AosSendMsg(msg);
	aos_assert_r(rslt, false);
}

bool
createCluster(
		const AosXmlTagPtr &app_config, 
		const AosRundataPtr &rdata, 
		int argc, char**argv)
{
	if (!app_config->getAttrBool("runcluster", false))
	{
		return false;
	}
	OmnString args;
	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-rdcout") == 0)
		{
			index++;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
		{
			index+=2;
			continue;
		}

		if (strcmp(argv[index], "-alarm") == 0)
		{
			index++;
			continue;
		}

		args << argv[index] << " ";
		index++;
	};

	args << "-config";

	OmnFilePtr file = OmnNew OmnFile("config_cluster.txt",OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		return false;
	}
OmnScreen << "****************************create cluster***************************" << endl;
	OmnString str;
	file->readToString(str);
	AosXmlParser parser;
	AosXmlTagPtr cluster_config = parser.parse(str, "" AosMemoryCheckerArgs);
	
	file = OmnNew OmnFile("config_norm.txt", OmnFile::eReadOnly AosMemoryCheckerArgs);
	file->readToString(str);
	AosXmlTagPtr norm_config = parser.parse(str, "" AosMemoryCheckerArgs);

	vector<u32> svr_ids = AosGetServerIds();
	for (u32 i=0; i<svr_ids.size(); i++)
	{
		//AosTransPtr trans_0 = OmnNew AosAdminStartProcessTrans(
		//		args, cluster_config, norm_config, svr_ids[i], false, false);
		//AosSendTrans(rdata, trans_0);

		AosAppMsgPtr msg = OmnNew AosStartProcessMsg(
				args, cluster_config, norm_config, svr_ids[i], 0);
		aos_assert_r(msg, false);
		AosSendMsg(msg);
	}
	return true;
}

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	//aos_alarm_init(1);

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

	if (!app_config) app_config = OmnApp::readConfig("config_admin.txt");
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

		if(strcmp(argv[index], "-base_dir") == 0 && index < argc-1)
		{
			OmnString basedir = app_config->getAttrStr("base_dir");
			if (basedir == "")
			{
				OmnString basedir = argv[index+1];
				basedir << "/admin";
				app_config->setAttr("base_dir", basedir);
			}
			index += 2;
			continue;
		}
		
		
		if(strcmp(argv[index], "-tmp_data_dir") == 0 && index < argc-1)
		{
			// Ketty 2013/11/04
			AosXmlTagPtr ipc_clt_conf = app_config->getFirstChild("IpcClt");
			bool add = false;
			if (!ipc_clt_conf)
			{
				AosXmlParser parser;
				ipc_clt_conf = parser.parse("<IpcClt/>", "" AosMemoryCheckerArgs);
				add = true;
			}
			aos_assert_r(ipc_clt_conf, false);
	
			ipc_clt_conf->setAttr("tmp_data_dir", argv[index+1]);
			if(add) app_config->addNode(ipc_clt_conf);	
		
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

	gAosLogLevel = app_config->getAttrInt("log_level", 2);
	sgStatusFreq = app_config->getAttrInt("memorycheck", 3600);

	AosRundataPtr rdata = OmnApp::getRundata();
	AosAccess::init(rdata);
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

	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	AosDataField::init();

	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false AosMemoryCheckerArgs));
	AosDataProcObj::setDataProc(OmnNew AosDataProcCount(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataCollectorObj::setDataCollectorCreator(OmnNew AosDataCollectorCreator());
	AosTaskObj::setCreator(OmnNew AosTaskCreator());
	AosTaskDataObj::setCreator(OmnNew AosTaskDataCreator());
	AosJobMgrObj::setJobMgr(AosJobMgr::getSelf());
	AosTaskMgrObj::setTaskMgr(AosTaskMgr::getSelf());
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
	AosIILMgrObj::setIILMgr(AosIILMgr::getSelf());
	AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSeLogClientObj::setLogClient(AosSeLogClient::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	AosPhyLogSvrObj::setPhyLogSvr(AosPhyLogSvr::getSelf());			// Ketty 2013/03/12
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());			// Ketty 2013/03/12
	AosNetFileMgr::getSelf();			// Ken Lee, 2013/04/15
	AosNetFileMgrObj::setObj(AosNetFileMgr::getSelf());			// Ken Lee, 2013/04/15

	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12
	AosFmtMgrObj::setFmtMgr(OmnNew AosFmtMgr());

	// Ketty 2013/03/25
	AosAllTrans::init();

	// Ken Lee, 2013/04/12
	AosDataAssembler::init(app_config);

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosVfsMgrObj::getVfsMgr()->config(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12
	AosFmtMgrObj::getFmtMgr()->config(app_config);

	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
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
	AosDataCollectorMgr::getSelf()->config(app_config);
	AosAllStorageEngines::init(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);
	AosPhyLogSvr::getSelf()->config(app_config);		// Ketty 2013/03/12
	//AosSysMonitor::getSelf()->config(app_config);

	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	AosAllActions::init();
	AosInitConditions();
	AosPhyCtnrSvrPtr counterserver;
	if (counter_client)
	{
		counterserver = OmnNew AosPhyCtnrSvr();
		aos_assert_r(counterserver, 0);
		
		// Ketty 2013/03/28
		AosCounterSvrObj::setCounterSvr(counterserver);
		counterserver->config(app_config);
	}
	AosDfmLog::config(app_config);


	//----------start----------
	AosVfsMgrObj::getVfsMgr()->start();
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();
	AosFmtMgrObj::getFmtMgr()->start();

	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();

	//AosBatchQueryReader::getSelf()->start();

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
	
	OmnScreen << "...........Start PhyLogSvr........." << endl;
	AosPhyLogSvr::getSelf()->start();
	
	if (counterserver)
	{
		counterserver->start();
	}

	//if (seLogClt)
	//{
	//	OmnScreen << "...........Start LogClient........." << endl;
	//	AosSeLogClient::getSelf()->start();
	//}
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

	// Ketty 2013/09/27.
	//OmnScreen << "...........Start SiteMgr........." << endl;
	//AosSiteMgr::getSelf()->start();
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
	reqDistr.start();
	AosObjMgrObj::createObjsAtStartupStatic();	
	AosExeReport report(sgStatusFreq, 500000);
	OmnScreen << "...........All Services Started........." << endl;

	if (AosGetSelfServerId() == 0)
	{
		AosRundataPtr rdata = OmnApp::getRundata();
		rdata->setSiteid(100);
		bool rslt = true;
		//if you start by browser you should comment out [addServer, createCluster]
		rslt = addServer(rdata);
		rslt = createCluster(app_config, rdata, argc, argv);
		if (rslt)
		{
			OmnScreen << "...........Start SiteMgr........." << endl;
			AosSiteMgr::getSelf()->start();
			
			if (seLogClt)
			{
				OmnScreen << "...........Start LogClient........." << endl;
				AosSeLogClient::getSelf()->start();
				if (!isRepairing)
				{
					OmnScreen << "...........Start StartLog........." << endl;
					AosSeLogClient::startLog();
				}
			}
		}
		
	}
	
	int num = 0;
	int num_cleancache = 0;
	while (1)
	{
		OmnSleep(1);
		if (num++ == sgStatusFreq)
		{
			AosMemoryChecker::getSelf()->report();
//			AOS_MEMORY_STATISTIC_PRINT_ALL;
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

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;

	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

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
