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
#include "Conds/AllConds.h"
#include "DataGenSeqno/AllDataGenSeqno.h"
#include "DataBlob/BlobRecord.h"
#include "DataField/DataFieldCreator.h"
#include "DataRecord/RecordFixBin.h"
#include "DataScanner/ScannerCreator.h"
#include "DataCacher/DataCacherCreator.h"
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
//#include "DocServer/AllDocSvrProcs.h"

#include "MsgClient/MsgClient.h"
#include "IdGen/IdGenMgr.h"
#include "Job/JobMgr.h"


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

#include "SEInterfaces/DataFieldObj.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SEInterfaces/DataCacherObj.h"
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
#include "SEInterfaces/StorageEngineObj.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEModules/LoginMgr.h"
#include "Security/Session.h"
#include "Security/SecurityMgr.h"
#include "Security/SessionMgr.h"
#include "SecUtil/AllAccesses.h"
#include "SeLogSvr/PhyLogSvr.h"
#include "SmartDoc/SmartDoc.h"
#include "StorageEngine/SizeIdMgr.h"
#include "StorageMgr/StorageMgr.h"
#include "SoapServer/SoapServer.h"
#include "TransClient/TransClient.h"
#include "TaskMgr/TaskMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "VpdParser/VpdParser.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "IILUtil/IILScanner.h"
#include "IILIDIdGen/IILIDIdGen.h"
#include "TransServer/Ptrs.h"
#include "TransServer/TransServer.h"
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
#include "StorageEngine/AllStorageEngines.h"
#include "Util/FileWatcher.h"
#include "Util/MemoryChecker.h"
#include "UtilComm/Tcp.h"
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
#include "Thread/ThreadShellMgr.h"
#include "Thread/Thread.h"
#include "TransClient/TransClient.h"
#include "Timer/TimerMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

// Need ContecEcgProc
#include "TaskRunnerMgr/TRMAIN.h"
#include "TaskRunnerMgr/TaskRunner.h"
#include "TaskRunnerMgr/TaskRunnerMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <dirent.h>

// start taskrunner main
bool AosExitSearchEngine2();

bool needStop2 = false;
const static int sgStatusFreq2 = 0x30ffffff;


// start server main
bool AosExitSearchEngine();

bool needStop = false;
const static int sgStatusFreq = 0x30ffffff;

bool startFlag = false;

int redirectOutPut();


int 
childProcessMain(const AosXmlTagPtr &config)
{

OmnScreen << "***************+++++++++++ init the taskrunner 1 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 1 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 1 +++++++++++************" << endl;
	OmnApp::setConfig(config);
	bool isRepairing = false;

OmnScreen << "***************+++++++++++ init the taskrunner 2 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 2 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 2 +++++++++++************" << endl;
	//OmnApp theApp(argc, argv);
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);
	OmnString workingdir = OmnApp::getAppConfig()->getAttrStr(AOSCONFIG_WORKINGDIR, "");
	if(workingdir != "" && chdir(workingdir.data()) < 0)
	{
		perror("Faild to set Working directory! ");
		exit(-1);
	}

OmnScreen << "***************+++++++++++ init the taskrunner 3 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 3 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 3 +++++++++++************" << endl;
	// Chen Ding, 06/05/2012
	AosNetworkMgrObj::config();
	AosTransModuleClt::init(app_config->getFirstChild(AOSCONFIG_TRANS_CLIENT));

	OmnThreadShellMgr::getSelf()->start();

	AosMemoryChecker::getSelf()->start();

	if (!AosImgProc::staticInit(app_config))
	{
		OmnAlarm << "Image Processor not configured correctly!" << enderr;
		exit(-1);
	}
	
OmnScreen << "***************+++++++++++ init the taskrunner 4 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 4 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 4 +++++++++++************" << endl;
	AosXmlTagPtr counter_client = app_config->getFirstChild("CounterClt");
	AosXmlTagPtr msgclient =  app_config->getFirstChild("msgclient");
	AosXmlTagPtr seLogClt =  app_config->getFirstChild("SeLogClt");

	// Set Interface Objects
	OmnScreen << "...........Start ObjMgr........." << endl;
	AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());

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
OmnScreen << "***************+++++++++++ init the taskrunner 5 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 5 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 5 +++++++++++************" << endl;

	// Configure Singletons
	OmnScreen << "...........Config........." << endl;
	AosErrorMgr::getSelf()->config(app_config);
	AosQueryRsltMgr::getSelf()->config(app_config);
	AosStorageMgr::getSelf()->config(app_config);
	AosIILIDIdGen::getSelf()->config(app_config);
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
	AosSiteMgr::getSelf()->config(app_config);
	AosSizeIdMgr::getSelf()->config(app_config);
	AosAllStorageEngines::init();
	AosStorageEngineObj::storageEngineInited();

	AosIILScannerObj::setIILScannerObj(OmnNew AosIILScanner());
	AosAllActions::init();
	AosInitConditions();

	OmnScreen << "...........Start IILIDIdGen........." << endl;
	AosIILIDIdGen::getSelf()->start();
	OmnSleep(2);
	AosTransModuleClt::recover();

	OmnScreen << "...........Start DocProc........." << endl;
	AosDocProc::getSelf()->start();
	OmnScreen << "...........Start DocClient........." << endl;
	AosDocClient::getSelf()->start();
	OmnScreen << "...........Start IILClient........." << endl;
	AosIILClient::getSelf()->start();
OmnScreen << "***************+++++++++++ init the taskrunner 6 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 6 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 6 +++++++++++************" << endl;
	
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

	// Ketty 2012/05/23
	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->start();

	OmnScreen << "...........All Services Started........." << endl;

	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
OmnScreen << "***************+++++++++++ init the taskrunner 7 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 7 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 7 +++++++++++************" << endl;
	
	int num = 0; 
	while (1)
	{
		OmnSleep(1);
		if (num++ == sgStatusFreq2)
		{
			AosMemoryChecker::getSelf()->report();
			num = 0;
		}
		if (needStop2)
		{
			AosExitSearchEngine2();
			break;
		}
	}
OmnScreen << "***************+++++++++++ init the taskrunner 8 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 8 +++++++++++************" << endl;
OmnScreen << "***************+++++++++++ init the taskrunner 8 +++++++++++************" << endl;
	return 0;

}

bool AosExitSearchEngine2()
{

	OmnScreen << "To stop QueryMgr!" << endl;
	AosQueryClient::getSelf()->stop();


	OmnScreen << "To TransClients!" << endl;
	AosTransClient::sStop();
	

	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	return true;
}


int redirectOutPut2()                                                        
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




// start server main

int 
serverProcessMain(const AosXmlTagPtr &config)
{
	while(1)
	{
		if(startFlag)
		{
			break;
		}
		else
		{
			continue;
		}
	}

	bool isRepairing = false;
	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::setVersion("0.11");
	OmnApp::setFrontEndServer(true);
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
	AosDataFieldObj::setCreator(OmnNew AosDataFieldCreator());
	AosDataRecordObj::setCreator(OmnNew AosRecordFixBin(false));
	AosDataCacherObj::setCreator(OmnNew AosDataCacherCreator());
	AosDataScannerObj::setCreator(OmnNew AosDataScannerCreator());
	AosDataBlobObj::setDataBlobObj(OmnNew AosBlobRecord());

	AosQueryContext::initQueryContext();
	AosObjMgrObj::setObjMgr(AosObjMgr::getSelf());
	AosSmartDocObj::setSmartDocObj(OmnNew AosSmartDocImpl());
	// AosActionObj::setActionObj(OmnNew AosSdocActionObj());
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
//	AosSoapServer::getSelf()->start();
	AosSeReqProc::config(app_config);

	AosSeReqProcPtr proc = OmnNew AosSeReqProc();
	AosReqDistr reqDistr(proc);
	OmnScreen << "...........Start ReqDistr........." << endl;
	if (!reqDistr.config(app_config->getFirstChild("seserver")))
	{
		OmnAlarm << "Failed the configuration: " 
			<< app_config->toString() << enderr;
		return -1;
	}
	if (!isRepairing)
	{
		OmnScreen << "...........Start StartLog........." << endl;
		AosSeLogClient::startLog();
	}

	// Ketty 2012/05/23
	AosJobMgr::getSelf()->start();
	AosTaskMgr::getSelf()->config(app_config);
	AosTaskMgr::getSelf()->start();
/*
	int index = 1;
	while (index < argc)
	{

		//felicia,2012/04/05 datasync
		if (strcmp(argv[index], "-datasync") == 0)
		{
			//OmnString addr = argv[index+1];
			//OmnString port = argv[index+2];
			OmnString attrname = argv[index+1];
			OmnString tag = argv[index+2];
			OmnString sdocstr = "<smartdoc zky_otype=\"zky_smtdoc\" zky_sdoctp=\"DataSync\" zky_modify_sdoc=\"false\" remote_addr=\"192.168.99.88\" remote_port=\"5555\" zky_sync_ctnrs=\"";
			sdocstr << tag << "\" zky_attrname=\"" << attrname << "\"/>";
			
	//		OmnString sdocstr = "<smartdoc zky_otype=\"zky_smtdoc\" zky_sdoctp=\"DataSync\" zky_modify_sdoc=\"false\">";
	//		sdocstr << "<remoteAdrr zky_value_type=\"const\">" << addr << "</remoteAddr>"
	//				<< "<remotePort zky_value_type=\"const\">" << port << "</remotePort>"
	//				<< "<attrName zky_value_type=\"const\">" << attrname << "</attrName>"
	//				<< "<attrValue zky_value_type=\"const\">" << tag << "</attrValue>";
		
			AosXmlTagPtr sdoc = AosXmlParser::parse(sdocstr AosMemoryCheckerArgs);
			if (!sdoc) exit(0);
			AosRundataPtr rdata = OmnApp::getRundata();
			rdata->setReceivedDoc(sdoc, true);
			rdata->setUserid(AosObjMgr::getSuperUserDocid(rdata->getSiteid(), rdata));
			AosSmartDoc::runSmartdoc(sdoc, rdata);
			OmnScreen << "System Data Sync finished!" << endl;
		}
		index ++;
	}
*/
	reqDistr.start();
	
	OmnScreen << "...........All Services Started........." << endl;

	// Check whether it needs to create objects.
	AosObjMgrObj::createObjsAtStartupStatic();	
	
	int num = 0; 
	while (1)
	{
		OmnSleep(1);
		if (num++ == sgStatusFreq)
		{
			AosMemoryChecker::getSelf()->report();
			num = 0;
		}
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
	AosTransModuleClt::stop();
	OmnSleep(2);
	OmnScreen << "To TransServers!" << endl;
	AosTransModuleSvr::stop();
	
	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	OmnScreen << "Max Docid: " << AosDocClient::getSelf()->getRepairingMaxDocid() << endl;
	return true;
}

