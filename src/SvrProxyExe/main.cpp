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
// 04/17/2013: Created by Ketty 
////////////////////////////////////////////////////////////////////////////

#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "NetworkMgr/NetworkMgr.h"
#include "SEInterfaces/NetworkMgrObj.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "SvrProxyMgr/Ptrs.h"
#include "UtilComm/TcpServerEpoll.h"
#include "util2/global_data.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"

#include "SysTrans/AllSysTrans.h"
#include "SysMsg/AllSysMsg.h"
#include "RlbTester/TesterTrans/AllRlbTesterTrans.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int gAosShowNetTraffic = 1;         // Chen Ding, 06/08/2012
int redirectOutPut();
void transInit();

void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	//cout << "value: " << value << endl;
	if (value == 14) needStop = true;
}

int 
main(int argc, char **argv)
{
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();
	//aos_alarm_init(0);

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp theApp(argc, argv);
	OmnApp::setIsSvrProxy();

	int index = 1;
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
	AosXmlTagPtr app_config;
	while (index < argc)
	{
		if (strcmp(argv[index], "-config") == 0)
		{
			// '-config fname'
			app_config = OmnApp::readConfig(argv[index+1]);
			index += 2;
			continue;
		}

		index++;
	}

	if (!app_config) app_config = OmnApp::readConfig("config_proxy.txt");
	aos_assert_r(app_config, -1);
	OmnString basedir = app_config->getAttrStr("base_dir", "./");
	basedir << "/proxy";
	app_config->setAttr("base_dir", basedir);
	OmnApp::setConfig(app_config);

	sgStatusFreq = app_config->getAttrInt("status_freq", 3600);

	AosMemoryChecker::getSelf()->config(app_config);
	AosMemoryChecker::getSelf()->start();
	sgMemoryCheckOpen = AosMemoryChecker::getCheckOpen();
	sgMemoryCheckFreq = AosMemoryChecker::getCheckFreq();

	redirectOutPut();

	transInit();

	AosSvrProxyObj::setSvrProxy(OmnNew AosSvrProxy());	// Ketty 2013/03/12
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());

	AosNetworkMgrObjPtr network_mgr = OmnNew AosNetworkMgr();
	//network_mgr->configNetwork(app_config);
	AosNetworkMgrObj::setNetworkMgr(network_mgr);
	AosSvrProxyObj::getSvrProxy()->config(app_config);
	AosSvrProxyObj::getSvrProxy()->start(argc, argv);

	int num = 0;
	int num_memory_check = 0;
	while (1)
	{
		OmnSleep(1);

		if (sgMemoryCheckOpen && num_memory_check++ >= sgMemoryCheckFreq)
		{
			num_memory_check = 0;
			AosMemoryChecker::getSelf()->report();
		}

		if (num++ == sgStatusFreq)
		{
			// Print Epoll Server stat
			//vector<OmnTcpServerEpollPtr> servers = OmnTcpServerEpoll::getEpollServers();
			//if (servers.size() > 0)
			//{
			//	for (u32 i=0; i<servers.size(); i++)
			//	{
			//		servers[i]->printStatus();
			//	}
			//}
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
	OmnScreen << "To stop SvrProxy!" << endl;
	AosSvrProxyObj::getSvrProxy()->stop();

	OmnScreen << "SvrProxy Stopped!" << endl;
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
	filename << log_dir << "proxy";
	setbuf(stdout, 0);

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

void transInit()
{
	AosAllSysTrans::init();
	AosAllSysMsg::init();
	//AosAllRlbTesterTrans::init();
}

