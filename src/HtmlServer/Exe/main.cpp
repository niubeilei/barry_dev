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
// 05/18/2010: Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "GICs/AllGics.h"
#include "HtmlServer/HtmlReqProc.h"
#include "HtmlServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SEModules/SeRundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "SearchEngineAdmin/SengAdmin.h"
#include "Util/OmnNew.h"
#include "Util/ExeReport.h"
#include "Util/File.h"
#include "Util1/MemMgr.h"
#include "util2/global_data.h"
#include "SEInterfaces/SecurityMgrObj.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

int gAosLogLevel = 1;
int gAosShowNetTraffic = 1;

int redirectOutPut();

bool aos_change_config = false;

bool sgIsBigIILServer = false;		// Ken Lee, 2012/07/17

void aosSignalHandler(int value)
{
	if (value == 14)
	{
		//to do
		aos_change_config = true;
	}
}

int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	OmnString filename;
	filename = "htmlserver_cout.txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}
int 
main(int argc, char **argv)
{
	aos_global_data_init();
	signal(SIGALRM, aosSignalHandler);

	int index = 0;
	OmnString config_name;
	while (index < argc)
	{
		if (strcmp(argv[index], "-rdcout") == 0)
		{
			redirectOutPut();
		}

		if (strcmp(argv[index], "-log") == 0)
		{
			gAosLogLevel = atoi(argv[index+1]);
			index++;
			continue;
		}

		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			config_name = argv[index+1];
			index += 2;
			continue;
		}

		index++;
	}
	if(config_name == "")
	{
		config_name = "config_norm.txt";
	}

	OmnApp::setConfig(OmnApp::readConfig(config_name.data()));

	// AosAppRundataPtr adata = OmnNew AosSeRundata();
	OmnApp::appStart(argc, argv);
	OmnApp app(argc, argv);
	AosXmlTagPtr appConfig = app.getAppConfig();
	if (!appConfig)
	{
		cout << "Faild to get config! " << endl; 
		exit(-1);
	}

	AosLangDictObj::setDictionary(AosLangDictMgr::getSelf()); 

	AosSecurityMgrObj::getSecurityMgr();
	AosSengAdmin::getSelf()->config(appConfig);
	bool rslt = AosHtmlReqProc::config(appConfig);
	if(!rslt) 
	{
		app.exitApp();
		exit(-1);
	}
	
	AosHtmlReqProcPtr proc = OmnNew AosHtmlReqProc();
	AosReqDistr reqDistr(proc);

	if (!reqDistr.config(appConfig))
	{
		OmnAlarm << "Failed the configuration: " << appConfig->toString() << enderr;
		app.exitApp();
		return -1;
	}
	reqDistr.start();

	while (1)
	{
		if (aos_change_config)
		{
			OmnApp::setConfig(OmnApp::readConfig("config_norm.txt"));
			AosXmlTagPtr appConfig = app.getAppConfig();
			rslt = proc->config(appConfig);
			if(!rslt)
			{
				app.exitApp();
				exit(-1);
			}
			aos_change_config = false;
		}
		OmnSleep(1);
	}
	app.exitApp();
	return 0;
} 

