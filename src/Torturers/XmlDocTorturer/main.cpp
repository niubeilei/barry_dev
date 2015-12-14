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
////////////////////////////////////////////////////////////////////////////

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "aosDLL/DllMgr.h"
#include "AccessServer/AccessReqProc.h"
#include "Debug/Debug.h"
#include "DataField/DataFieldBool.h"
#include "GICs/AllGics.h"
#include "HtmlServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "Proggie/ReqDistr/ReqDistr.h"
#include "SengTorUtil/SengTesterMgr.h"
#include "SEInterfaces/SecurityMgrObj.h"
#include "SEModules/SeRundata.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "Util/OmnNew.h"
#include "Util/ExeReport.h"
#include "Util/File.h"
#include "Util/SPtr.h"
#include "Util1/MemMgr.h"
#include "util2/global_data.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <iostream>

#include "Thrift/AosJimoAPI.h"
#include "ThriftClientCPP/Headers.h"
#include "Torturers/XmlDocTorturer/TesterLogin.h"
#include "Torturers/XmlDocTorturer/TesterCreateObj.h"
#include "Torturers/XmlDocTorturer/TesterModifyObj.h"
#include "Torturers/XmlDocTorturer/TesterDeleteObj.h"
#include "Torturers/XmlDocTorturer/TesterLogout.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"


using namespace std;
 


shared_ptr<AosJimoAPIClient> gThriftClient;

int gAosLogLevel = 1;
int gAosShowNetTraffic = 1;
bool gAosIsNormQuery = true;

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
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, aosSignalHandler);


	OmnApp theApp(argc, argv);
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
			AosXmlTagPtr app_config = OmnApp::readConfig(config_name.data());
			aos_assert_r(app_config, false);
			OmnApp::setConfig(app_config);
			index += 2;
			continue;
		}

		index++;
	}
	if(config_name == "")
	{
		config_name = "config_norm.txt";
	}

	OmnApp::appStart(argc, argv);
	OmnApp app(argc, argv);
	AosXmlTagPtr appConfig = app.getAppConfig();
	if (!appConfig)
	{
		cout << "Faild to get config! " << endl; 
		exit(-1);
	}
	AosXmlTagPtr servers = appConfig->getFirstChild("thrift_servers"); 
	aos_assert_r(servers, false);
	AosXmlTagPtr child = servers->getFirstChild(true);
	aos_assert_r(child, false);
	OmnString ip = child->getAttrStr("ip", "");
	int prot = child->getAttrInt("port", 0);

	AosJimoAPIClientMgr::start(ip, prot); 
  	gThriftClient = AosJimoAPIClientMgr::getSelf()->getClient();

	if (!gThriftClient)
	{
		OmnScreen << "*********** Failed creating the client" << endl;
		return -1;
	}

	int num_threads = 1;
	int tries = 100;
	AosSengTesterMgr test_mgr(num_threads, tries);

	//AosSengTesterPtr tester_login= OmnNew AosLoginTester();  test_mgr.addTester(tester_login);
	AosSengTesterPtr tester_crt = OmnNew AosCreateObjTester();  test_mgr.addTester(tester_crt);
	//AosSengTesterPtr tester_del = OmnNew AosDeleteObjTester();  test_mgr.addTester(tester_del);
	//AosSengTesterPtr tester_mdy = OmnNew AosModifyObjTester();  test_mgr.addTester(tester_mdy);
	//AosSengTesterPtr tester_logout= OmnNew AosLogoutTester();  test_mgr.addTester(tester_logout);
	
	test_mgr.config(OmnApp::getAppConfig());
	test_mgr.startTest();

	AosExeReport report(10);
	theApp.appLoop();
	theApp.exitApp();
	return 0;
} 


