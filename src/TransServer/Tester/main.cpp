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
// 06/03/2011: Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "Security/SessionMgr.h"
#include "SmartDoc/SmartDoc.h"
#include "SEModules/SeRundata.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "TransServer/Ptrs.h"
#include "TransServer/Tester/Ptrs.h"
#include "TransServer/TransSvrConnAsync.h"
#include "TransServer/TransSvrConnSync.h"
#include "TransServer/Tester/TransSvrProc.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "util2/global_data.h"
#include "Actions/AllActions.h"
#include "Util1/MemMgr.h"
#include "WordMgr/WordMgr1.h"
#include "WordParser/Ptrs.h"
#include "WordParser/WordParser.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int t1;
//int gAosLogLevel = 5;
#include <dirent.h>

int redirectOutPut(); 

int 
main(int argc, char **argv)
//serverMain(int argc, char **argv)
{
	aos_global_data_init();

	//int t = 1;
	//while(t)
	//{
	//	t = 1;
	//}
	OmnApp::appStart(argc, argv);
	
	OmnApp::setConfig("config_server.txt");

	int index = 1;

	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			OmnApp::setConfig(argv[index+1]);
			index += 2;
			continue;
		}
		
		index++;
	}

	//redirectOutPut(); 

	OmnApp theApp(argc, argv);

	AosQueryRsltMgr::getSelf();
	try
	{
		
		//theApp.startSingleton(OmnNew AosSessionMgrSingleton());
		//AosSessionMgrSelf->start(OmnApp::getAppConfig());
	}
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}

	AosTransSvrProcPtr transProc = OmnNew AosTransSvrProc();
	AosXmlTagPtr transConfig = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_TRANS);
	AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, transProc);
	AosTransServerPtr server = conn->getTransServer();
	server->recover();
	conn->start();

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 

int redirectOutPut()                                                        
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

