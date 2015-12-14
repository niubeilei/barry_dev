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
// 05/01/2015: Created by Phil Pei
////////////////////////////////////////////////////////////////////////////
#include "AppMgr/App.h"
#include "Alarm/AlarmMgr.h"
#include "Accesses/AllAccesses.h"  
#include "Accesses/Access.h"       
#include "aosDLL/DllMgr.h"
#include "NetFile/NetFileMgr.h"
#include "NetworkMgr/NetworkMgr.h"
#include "ReplicMgr/ReplicMgr.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "SEInterfaces/VfsMgrObj.h"
#include "SEInterfaces/ClusterMgrObj.h"
#include "SEInterfaces/ClusterObj.h"
#include "SEInterfaces/CubeMapObj.h"
#include "SEInterfaces/DllMgrObj.h"
#include "SEInterfaces/IpcCltObj.h" 
#include "ClusterMgr/ClusterMgr.h"
#include "SEModules/SiteMgr.h"
#include "StorageEngine/AllStorageEngines.h"
#include "StorageMgr/VirtualFileSysMgr.h"
#include "SvrProxyMgr/IpcClt.h"
#include "TransClient/TransClient.h"    
#include "TransServer/CubeTransSvrMgr.h"
#include "DocClient/DocClient.h"
#include "JimoCallServer/JimoCallServer.h"
#include "JimoRaft/RaftNode.h"

int gAosShowNetTraffic = 0;
bool gSanityCheck = true;

int redirectOutPut(const char* fname);

int
main(int argc, char **argv)
{
	///if (argc != 3)
	///{
	///	cout << "Usage: $./raftServer.exe -c <config_file>" << endl;
	///	exit(0);
	///}

	//get command line parameters
	AosXmlTagPtr appConfig = 0;
	int index = 1;
	int tries = 0;
	while (index < argc)
	{
		if (strcmp(argv[index], "-c") == 0)
		{
				// '-config fname'
			appConfig = OmnApp::readConfig(argv[index+1]);
			aos_assert_r(appConfig, false);
			OmnApp::setConfig(appConfig);
			index += 2;
			continue;
		}
		if (strcmp(argv[index], "-r") == 0)
		{
				// '-file fname'
			char* rd_fname = argv[index+1];
			redirectOutPut(rd_fname);					
	
			index += 2;
			continue;
		}
		
		index++;
	}

	if (!appConfig)
	{
		cout << "Usage: $./raftServer.exe -c <config_file>" << endl;
		exit(0);
	}

	//init the application and relevant modules
	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.10");
	OmnApp theApp(argc, argv);
	AosNetFileMgr::getSelf(); 
	//aos_alarm_init(1);  
	//AosReplicMgrObj::getReplicMgr();
	AosReplicMgrObj::setReplicMgr(OmnNew AosReplicMgr());   
	AosNetworkMgrObj::setNetworkMgr(AosNetworkMgr::getSelf());
	AosNetworkMgrObj::getNetworkMgr()->configNetwork(appConfig); 
	AosVfsMgrObj::setVfsMgr(OmnNew AosVirtualFileSysMgr());  
	AosVfsMgrObj::getVfsMgr()->config(appConfig);
	OmnAlarmMgr::config();
	AosDllMgrObj::setDllMgr(OmnNew AosDllMgr()); 
	AosSiteMgrObj::setSiteMgr(AosSiteMgr::getSelf());
	//AosDocClientObj::setDocClient(AosDocClient::getSelf());
	AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());
	AosTransCltObj::setTransClt(OmnNew AosTransClient());
    AosCubeTransSvrObj::setTransSvr(OmnNew AosCubeTransSvrMgr());

	AosIpcCltObj::getIpcClt()->config(appConfig);
	AosTransCltObj::getTransClt()->config(appConfig);
	AosTransSvrObj::getTransSvr()->config(appConfig);

	AosTransCltObj::getTransClt()->start();
	AosTransSvrObj::getTransSvr()->start();
	//init members needed by raft micro service
	AosRundataPtr rdata = OmnApp::getRundata();
	AosAccess::init(rdata);
	rdata->setSiteid(100);

	//set cluster mgr
	AosClusterMgrObj::setClusterMgr(OmnNew AosClusterMgr(1));

	//start raft server
	OmnScreen << "...........Start JimoCallRaftServer........." << endl;
	AosJimoCallServer jimocallServer(rdata.getPtr());
	u32 serverId = jimocallServer.getEndpointId();

	//init peer map based on the configuration and cluster
	AosRaftNode *node;
	node = new AosRaftNode(rdata.getPtr(), serverId);
	AosRaftNode::setSelf(node);

	//raft node will run until system stop
	node->run(rdata);
	
	//end the raft server running
	theApp.appLoop();
	theApp.exitApp();

	return 0;
}

int redirectOutPut(const char *filename)
{
	close(STDOUT_FILENO);   
	//Jozhi 2014/09/18 change all printing to [base_dir]/proc_log directory
	OmnString base_dir = OmnApp::getAppBaseDir();
	OmnString parent_dir = OmnApp::getParentDir(base_dir);
	OmnString dir_name = OmnApp::getDirName(base_dir);
	//OmnString log_dir;
	//log_dir << parent_dir << "/proc_log/";
	//OmnApp::createDir(log_dir);
	//OmnString filename = fname;
	//filename << log_dir << dir_name << "_frontend";
	setbuf(stdout, 0);

	//char buff[128];
	//struct tm * ptm;
	//time_t rawtime;
	//time(&rawtime);
	//ptm = gmtime(&rawtime);
	//sprintf(buff, "%02d%02d%02d%02d%02d", ptm->tm_mon+1, 
	//		ptm->tm_mday, (ptm->tm_hour+8)%24, ptm->tm_min, ptm->tm_sec);
	//filename << buff << ".txt";
	int fd = open(filename, O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}
	return 0;
}
