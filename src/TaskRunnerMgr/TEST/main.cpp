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
#include "DocServer/AllDocSvrProcs.h"
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
#include "TaskRunnerMgr/TaskRunnerMgr.h"

// Need ContecEcgProc
#include "CustomizedProcs/Contec/Decl.h"


#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


static OmnString sgBuildNum = "2012-03-03.1";
static OmnString sgBuildTime = "2012/03/13 15:42";
static OmnString sgBuildBy = "Jackie Zhao";
static OmnString sgBuildAuthBy = "Ice Yu";
static OmnString sgLocation = " 99 : /home/jackie/AOS/src/SEServer/Local/";

int redirectOutPut();
bool sgIsBigIILServer = false;		// Ken Lee, 2012/07/17
int gAosLogLevel = 1;
int gAosIILLogLevel = 1;			// Chen Ding, 06/07/2012
int gAosShowNetTraffic = 1;			// Chen Ding, 06/08/2012
bool gNeedBackup = false;			// Brian Zhang 2012/08/06


#include <dirent.h>

void aosSignalHandler(int value)
{
	AosTaskRunnerMgr::getSelf()->stopServer(value);
}

int 
main(int argc, char **argv)
{
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

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

	AosTaskRunnerMgr::getSelf()->config(app_config);
	AosTaskRunnerMgr::getSelf()->start();

	while (1)
	{
		OmnSleep(10);
	}

	theApp.exitApp();
	return 0;
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
