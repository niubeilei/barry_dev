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
#include "IdGen/IdGenMgr.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "QueryRslt/QueryRsltMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "DocServer/AllDocSvrProcs.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadMgr.h"
#include "TransClient/Tester/TestSuite.h"
#include "Util/OmnNew.h"
#include "util2/global_data.h"
#include "Util1/MemMgr.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <dirent.h>
#include <sys/times.h>

int gAosLogLevel = 5;
int redirectOutPut();

int 
//clientMain(int argc, char **argv)
main(int argc, char **argv)
{
	//tms *t= new tms;
	//clock_t start;
	//cout << t << endl;
	//start = times(t);
	
	aos_global_data_init();

	//int t=1;
	//while(t)
	//{
	//	t =1;
	//}
	
	OmnApp::appStart(argc, argv);
	OmnApp::setConfig("config_client.txt");
	//OmnApp::setConfig("config_norm_local.txt");

	//redirectOutPut();
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
	OmnApp theApp(argc, argv);

	AosQueryRsltMgr::getSelf();

	int tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("TransClient/TransClientTester", "Try", "Ketty");
	testMgr->setTries(tries);
	OmnString test = "TransClient";
	testMgr->addSuite(AosTransClientTestSuite::getSuite(test));

	cout << "Start Testing ..." << endl;
	testMgr->start();
	cout << "\nFinished. " << (testMgr->getStat()).data() << endl;

	testMgr = 0;
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
	filename = "client_cout.txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);                                                                   
	}

	return 0;
}
