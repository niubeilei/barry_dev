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
// 04/11/2013: Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "AppMgr/App.h"
#include "RlbTester/DiskErrTorturer/DiskErrTester.h"
#include "Porting/Sleep.h"
#include "SvrProxyMgr/IpcClt.h"
#include "TransClient/TransClient.h"
#include "TransServer/TransServer.h"
#include "XmlUtil/XmlTag.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"

#include "SEInterfaces/NetworkMgrObj.h"
#include "SEInterfaces/IpcCltObj.h"
#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/TransSvrObj.h"

#include "SysMsg/AllSysMsg.h"
#include "RlbTester/TesterTrans/AllRlbTesterTrans.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int redirectOutPut();
void transInit();

int 
main(int argc, char **argv)
{
	//int ss = 1;
	//while(ss)
	//{
	//	ss = 1;
	//}

	aos_alarm_init(1);
	
	int index = 1;
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

	OmnApp theApp(argc, argv);
	aos_assert_r(app_config, -1);
	OmnApp::setConfig(app_config);
	redirectOutPut();

	transInit();
	
	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	AosTransCltObj::setTransClt(OmnNew AosTransClient());	// Ketty 2013/03/12
	AosTransSvrObj::setTransSvr(OmnNew AosTransServer());	// Ketty 2013/03/12
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosIpcCltObj::getIpcClt()->config(app_config);
	AosTransCltObj::getTransClt()->config(app_config);	// Ketty 2013/03/12
	AosTransSvrObj::getTransSvr()->config(app_config);	// Ketty 2013/03/12
	
	AosIpcCltObj::getIpcClt()->start();
	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();		// will init crt master.
	
	cout << "Start Testing ..." << endl;
	AosDiskErrTesterPtr tester = OmnNew AosDiskErrTester();
	tester->start();
	cout << "\nFinished. " << endl;

	while (1)
	{
		OmnSleep(10000);
	}

	theApp.exitApp();
	return 0;
}


int redirectOutPut()
{
	OmnString filename = OmnApp::getAppBaseDir();
	filename << "DiskTester";

	filename << OmnGetTimestamp() << ".txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}

void transInit()
{
	//AosAllSysTrans::init();
	AosAllSysMsg::init();
	AosAllRlbTesterTrans::init();
}


