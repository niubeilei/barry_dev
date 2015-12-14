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
#include "DocFileMgr/Tester/TestSuite.h"
#include "Porting/Sleep.h"
#include "FmtMgr/FmtMgr.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
#include "StorageMgr/VirtualFileSysMgr.h"

#include "SEInterfaces/NetworkMgrObj.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "XmlUtil/XmlTag.h"

int redirectOutPut();

int 
main(int argc, char **argv)
{
	OmnApp theApp(argc, argv);
	OmnApp::setConfig("config_norm.txt");

	redirectOutPut();

	int index = 1;
	while (index < argc)
	{
		if (strcmp(argv[index], "-rdcout") == 0)
		{
			redirectOutPut();
			index ++;
			continue;
		}
		index++;
	}
	
	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);
	
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());		// Ketty 2013/01/23
	AosFmtMgrObj::setFmtMgr(OmnNew AosFmtMgr()); 
	
	OmnScreen << "...........Config........." << endl;
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(app_config);
	AosFmtMgrObj::getFmtMgr()->config(app_config);
	AosVfsMgrObj::getVfsMgr()->config(app_config);
	
	
	OmnScreen << "...........Start ........." << endl;
	AosFmtMgrObj::getFmtMgr()->start();
	AosVfsMgrObj::getVfsMgr()->start();

	int tries = 1;
	OmnTestMgrPtr testMgr = OmnNew OmnTestMgr("DocFileMgr/StorageMgrTester", "Try", "Ketty");
	testMgr->setTries(tries);
	OmnString test = "Storage";
	testMgr->addSuite(AosDocFileMgrTestSuite::getSuite(test));

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
	OmnString filename = "output_";

	//FILE* ff = popen("hostname", "r");
	//char buff[128];
	//int len = fread(buff, 1, 128, ff); 
	//buff[len-1]=0;
	//filename << buff << ".txt";
	//pclose(ff);
	filename << OmnGetTimestamp() << ".txt";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}

