
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
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "DataBlob/BlobRecord.h"
#include "DataField/DataFieldBool.h"
#include "DataProc/DataProcCount.h"
#include "DataRecord/RecordVariable.h"
#include "DataScanner/ScannerCreator.h"
//#include "DataCacher/DataCacherCreator.h"
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
#include "DocClient/DocProc.h"
#include "DocClient/DocClient.h"
#include "DocMgr/Ptrs.h"
#include "DocMgr/DocMgr.h"
#include "DocFileMgr/DocFileMgr.h"
#include "DocSelector/DocSelObjImpl.h"
#include "DocSelector/Torturer/DocSelRandUtil.h"
#include "DocServer/DocSvr.h"
#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "MultiLang/LangDictMgr.h"
#include "MySQL/Ptrs.h"
//#include "MySQL/DataStoreMySQL.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "Query/AllTerms.h"
#include "Query/QueryReq.h"
#include "Query/QueryReqNew.h"
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
//#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/DataCollectorObj.h"
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
#include "SEInterfaces/CounterCltObj.h"
#include "SEInterfaces/CounterSvrObj.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "SEInterfaces/DataCollectorObj.h"
#include "SEInterfaces/DocFileMgrObj.h"
#include "SEInterfaces/BitmapObj.h"
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
#include "StorageEngine/SizeIdMgr.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "SoapServer/SoapServer.h"
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
#include "SEServer/Ptrs.h"
#include "SEModules/ImgProc.h"
#include "SEServer/SeReqProc.h"
#include "ShortMsgClt/ShortMsgClt.h"
#include "NetFile/NetFileClt.h"
#include "NetFile/NetFileMgr.h"
#include "TransClient/TransClient.h"
#include "TransServer/TransServer.h"
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
#include "SEUtil/SEUtilInit.h"
#include "Thread/ThreadMgr.h"
#include "Thread/Thread.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "StoreQuery/StoreQueryMgr.h"
#include "SEServer/Admin/AllTrans.h"
#include "QueryClient/BatchQueryReader.h"
#include "DocFileMgr/DfmLog.h"
#include "SysMsg/AddServerMsg.h"
#include "SysMsg/AddClusterMsg.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
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
static int sgStatusFreq = 3600;

static bool sgMemoryCheckOpen = true;
static int sgMemoryCheckFreq = 30;


int redirectOutPut();
bool printOn();
bool printOff();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	if (value == 14) needStop = true;
}

int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012

bool gNeedBackup = false;
bool gAosIsNormQuery = true;
#include <dirent.h>

#include "UtilTime/TimeFormat.h"
#include "UtilTime/TimeUtil.h"


AosXmlTagPtr
getAdminConfig(const AosXmlTagPtr &svr_proxy_config)
{
	OmnString net_str;
	net_str << "<networkmgr "
			<< "zky_procid=\"1\" "
			<< "replic_policy=\"nobkp\" "
			<< "cube_grp_id=\"0\" "
			<< "proc_type=\"admin\" "
			<< "vids=\"0\"><servers></servers></networkmgr>";
	AosXmlParser parser;
	AosXmlTagPtr admin_config = parser.parse(net_str, "" AosMemoryCheckerArgs);
	aos_assert_r(admin_config, 0);

	OmnString svr_str;
	svr_str << "<server>"
			<< "<Process proc_id=\"1\" "
			<< "proc_type=\"admin\" "
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
	OmnString config_dir = OmnApp::getConfigDir();
	OmnString config_path_net;
	if (config_dir != "")
	{
		config_path_net << config_dir << "/config_network.txt";
	}
	else
	{
		config_path_net << "config_network.txt";
	}
	OmnFilePtr file = OmnNew OmnFile(config_path_net,
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

	OmnString config_dir = OmnApp::getConfigDir();
	OmnString config_path_cluster;
	OmnString config_path_norm;
	if (config_dir != "")
	{
		config_path_cluster << config_dir << "/config_cluster.txt";
		config_path_norm << config_dir << "/config_norm.txt";
	}
	else
	{
		config_path_cluster << "config_cluster.txt";
		config_path_norm << "config_norm.txt";
	}


	OmnFilePtr file = OmnNew OmnFile(config_path_cluster, OmnFile::eReadOnly AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		return false;
	}
OmnScreen << "****************************create cluster***************************" << endl;
	OmnString str;
	file->readToString(str);
	AosXmlParser parser;
	AosXmlTagPtr cluster_config = parser.parse(str, "" AosMemoryCheckerArgs);
	
	file = OmnNew OmnFile(config_path_norm, OmnFile::eReadOnly AosMemoryCheckerArgs);
	file->readToString(str);
	AosXmlTagPtr norm_config = parser.parse(str, "" AosMemoryCheckerArgs);

	// just need send to svr 0. Ketty 2014/03/24
	AosAppMsgPtr msg = OmnNew AosAddClusterMsg(
			args, cluster_config, norm_config, 0, 0);
	aos_assert_r(msg, false);
	AosSendMsg(msg);
	return true;
}

int 
main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
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
	sgStatusFreq = app_config->getAttrInt("status_freq", 3600);

	AosMemoryChecker::getSelf()->config(app_config);
	AosMemoryChecker::getSelf()->start();
	sgMemoryCheckOpen = AosMemoryChecker::getCheckOpen();
	sgMemoryCheckFreq = AosMemoryChecker::getCheckFreq();

	redirectOutPut();

	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}

	// Chen Ding, 01/01/2013
	AosSEUtilInit::init(app_config);
	// Set Interface Objects
	OmnScreen << "...........Set Interface Objects........." << endl;
	AosBitmapObj::setObject(OmnNew AosBitmap());
	AosConditionObj::setCondition(OmnNew AosCondTrue(false));
	//AosDataFieldObj::setCreator(OmnNew AosDataFieldBool(false));
	//AosDataField::init();

	//AosDataRecordObj::setCreator(OmnNew AosRecordVariable(false AosMemoryCheckerArgs));
	//AosDataProcObj::setDataProc(OmnNew AosDataProcCount(false));
	//AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataCollectorObj::setDataCollectorCreator(NULL);//OmnNew AosDataCollectorCreator());

	//AosQueryReqObj::setObject(OmnNew AosQueryReq());
	//AosQueryReqObj::setObjectNew(OmnNew AosQueryReqNew());
	AosQueryContext::initQueryContext();
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosDocSelObj::setDocSelector(new AosDocSelObjImpl());
	AosLoginMgrObj::setLoginMgrObj(new AosLoginMgr(app_config, true));
	AosUserMgmtObj::setUserMgmt(new AosUserMgmt());
	AosVersionServerObj::setVersionServer(AosVersionServer::getSelf());
	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf());
	AosDocMgrObj::setDocMgr(AosDocMgr1::getSelf());
	AosIILClientObj::setIILClient(AosIILClient::getSelf());
	//AosDocClientObj::setDocClient(AosDocClient::getSelf());
	//AosQueryObj::setQueryObj(AosQuery::getSelf());
	AosSizeIdMgrObj::setSizeIdMgr(AosSizeIdMgr::getSelf());
	AosCounterCltObj::setCounterClt(AosCounterClt::getSelf());
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	AosWordMgrObj::setWordMgr(AosWordMgr1::getSelf());			// Ketty 2013/03/12

	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12

	// Ketty 2013/03/25
	AosAllTrans::init();

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosVfsMgrObj::getVfsMgr()->config(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);

	AosDocSvr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
	AosWordMgr1::getSelf()->config(app_config);
	AosFrontEndSvr::getSelf()->config(app_config);
	AosDocMgr1::getSelf()->config(app_config);
	//AosDocClient::getSelf()->setRepairing(isRepairing);
	//AosDocClient::getSelf()->config(app_config);
	//AosDocProc::getSelf()->config(app_config);
	AosIILClient::getSelf()->config(app_config);
	//AosSessionMgr::getSelf()->config(app_config);  
	//AosSecurityMgr::getSelf()->config(app_config);
	AosCounterClt::getSelf()->config(app_config);
	AosTimerMgr::getSelf()->config(app_config);
	AosSiteMgr::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
	//AosDataCollectorMgr::getSelf()->config(app_config);
	AosAllStorageEngines::init(app_config);
	AosStorageEngineMgr::getSelf()->config(app_config);

	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	AosInitConditions();
	AosDfmLog::config(app_config);
	//----------start----------
	AosVfsMgrObj::getVfsMgr()->start();
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();

	// Chen Ding, 2013/02/21
	AosValueSel::init();
	AosMemoryChecker::getSelf()->start();

	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnScreen << "...........StartWordMgr1........." << endl;
	AosWordMgr1::getSelf()->start();

	OmnScreen << "...........Start DocSvr........." << endl;
	AosDocSvr::getSelf()->start();

	OmnScreen << "...........Start DocProc........." << endl;
	//AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	//AosDocClient::getSelf()->start();
//	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
	
	OmnScreen << "...........Start SessionMgr........." << endl;
	//AosSessionMgr::getSelf()->start();  
	//OmnScreen << "...........Start Securitymgr........." << endl;
	//AosSecurityMgr::getSelf()->start();

	AosXmlTagPtr seconfig = app_config->getFirstChild("seserver");
	aos_assert_r(seconfig, false);
	//AosSeReqProc::config(seconfig);

	AosStoreQueryMgr::getSelf()->start();

	//Jozhi 2014/09/12 commont out admin's database feathure
	//OmnScreen << "...........Start SiteMgr........." << endl;
	//AosSiteMgr::getSelf()->start();
	/*
	// Young. 2013/12/02
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);
	OmnString container = "";
	container << "<container " << "zky_public_doc=\"true\" zky_public_ctnr=\"true\" zky_otype =\"zky_ctnr\">"
		<< "</container>";
	AosXmlParser parser;
	AosXmlTagPtr container_xml = parser.parse(container, "" AosMemoryCheckerArgs);
	OmnString str = container_xml->toString();
	AosXmlTagPtr ctr = AosCreateDoc(str, true, rdata);
	// -----------------------------------------------
	*/

	//Jozhi 2014/09/12 commont out admin's database feathure
	//AosSeReqProcPtr proc = OmnNew AosSeReqProc();
	//AosReqDistr reqDistr(proc);
	//OmnScreen << "...........Start ReqDistr........." << endl;
	//if (!reqDistr.config(seconfig))
	//{
	//	OmnAlarm << "Failed the configuration: " 
	//		<< app_config->toString() << enderr;
	//	theApp.exitApp();
	//	return -1;
	//}
	//reqDistr.start();
	AosObjMgrObj::createObjsAtStartupStatic();	
	AosExeReport report(sgStatusFreq, 500000);
	OmnScreen << "...........All Services Started........." << endl;

	bool isWebStart = app_config->getAttrBool("isWebStart", false);
	if (!isWebStart)
	{
		if (AosGetSelfServerId() == 0)
		{
OmnScreen << ">>>>>>>>>>use admin to start cluster" << endl;
			AosRundataPtr rdata = OmnApp::getRundata();
			rdata->setSiteid(100);
			bool rslt = true;
			//if you start by browser you should comment out [addServer, createCluster]
			rslt = addServer(rdata);
			rslt = createCluster(app_config, rdata, argc, argv);
		}
	}
	else
	{
OmnScreen << ">>>>>>>>>>use web to start cluster" << endl;
	}

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
	OmnScreen << "To stop IdGenMgr!" << endl;
	AosIdGenMgr::getSelf()->stop();
	OmnSleep(1);

	OmnScreen << "To stop VirtualFileSysMgr!" << endl;
	AosVfsMgrObj::getVfsMgr()->stop();

	OmnScreen << "To IpcClt!" << endl;
	AosIpcCltObj::getIpcClt()->stop();
	
	OmnScreen << "To stop ThreadMgr!" << endl;
	OmnThreadMgr::getSelf()->stop();

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
	filename << log_dir << dir_name << "_admin";

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
