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
#include "Actions/AllActions.h"
#include "Conds/AllConds.h"
#include "Debug/Debug.h"
#include "DocProc/DocProc.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "TransServer/TransServer.h"
#include "TransServer/Ptrs.h"
#include "EmailServer/EmailServer.h"
#include "EmailServer/Ptrs.h"
#include "util2/global_data.h"
#include "TransServer/TransSvrConn.h"
#include "IILMgr/IILMgr.h"
#include "IILClient/IILClient.h"
#include "WordMgr/WordMgr1.h"
#include "XmlUtil/XmlTag.h"
#include "SearchEngine/DocServer1.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <dirent.h>


int gAosLogLevel = 1;

	
int 
main(int argc, char **argv)
{
	aos_global_data_init();

	OmnApp::setConfig("config_norm.txt");
	OmnApp theApp(argc, argv);
	
	try
	{
		AosDocServer::getSelf();
		AosDocProc::getSelf();
		AosIILMgr::getSelf();
		AosIILClient::getSelf();
	}
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		theApp.exitApp();
		return 0;
	}
	
	
	AosXmlTagPtr config = OmnApp::getAppConfig();
	if(!config) exit(1);
	AosTransProcPtr email = OmnNew AosEmailServer(config);
	AosXmlTagPtr transConfig = config->getFirstChild(AOSCONFIG_TRANS);
	AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, email);
	
	// Ketty 2012/03/03
	AosTransServerPtr transServer = conn->getTransServer();
	transServer->recover();
	conn->start();
	
	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
} 

