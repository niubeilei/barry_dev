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
#include "BitmapMgr/Bitmap.h"
#include "SEInterfaces/BitmapObj.h"
#include "SvrProxyMgr/Ptrs.h"
//#include "SvrProxyMgr/IpcClt.h"
#include "SvrProxyMgr/SockConn.h"
#include "SvrProxyMgr/NetSvr.h"
#include "TransBasic/AppMsg.h"
#include "SysTrans/SendReplicLogTrans.h"
#include "UtilComm/TcpServerEpoll.h"
#include "util2/global_data.h"

#include "ReliableFile/AllReplicPolicy.h"
#include "SEInterfaces/NetworkMgrObj.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

static bool needStop = false;
static int sgStatusFreq = 600;

int gAosShowNetTraffic = 1;         // Chen Ding, 06/08/2012
int redirectOutPut();
bool AosExitSearchEngine();


void aosSignalHandler(int value)
{
	if (value == SIGALRM) needStop = true;
	//cout << "value: " << value << endl;
	if (value == 14) needStop = true;
}


int 
main(int argc, char **argv)
{
	static AosSendReplicLogTrans		sgSendReplicLogTrans(true);

	signal(SIGALRM, aosSignalHandler);
	aos_global_data_init();

	OmnApp::appStart(argc, argv);
	OmnApp::setVersion("0.11");
	OmnApp theApp(argc, argv);

	OmnApp::setConfig("config_norm.txt");   // Ketty 2013/05/09
	redirectOutPut();   // Ketty 2013/05/09

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
	
	redirectOutPut();	// Ketty 2013/05/09

	AosXmlTagPtr app_config = OmnApp::getAppConfig();
	aos_assert_r(app_config, -1);
	
	AosBitmapObj::setObject(OmnNew AosBitmap());
	
	AosAllReplicPolicy::init();
	AosNetworkMgrObj::config(app_config);

	//AosIpcCltObj::setIpcClt(OmnNew AosIpcClt());	// Ketty 2013/03/12
	//AosIpcCltObj::getIpcClt()->config(app_config);
	//AosIpcCltObj::getIpcClt()->start();

	AosXmlTagPtr conf = app_config->getFirstChild("SvrProxy");
	aos_assert_r(conf, false);

	u64 max_cache_size = conf->getAttrU64(AOSCONFIG_TRANS_MAXCACHESIZE, 500) * 1000000;
	AosNetSvrPtr net_svr = OmnNew AosNetSvr(max_cache_size, false);
	net_svr->start();
	//net_svr->selfIsAvailable();

	//int aa = 1;
	//while(aa)
	//{
	//	aa = 1;
	//}
	
	AosSockConnPtr conn = net_svr->getConn(1);
	aos_assert_r(conn, false);

	int num = 0;
	//AosBuffPtr data = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
	//data->setDataLen(1000);
	static u32 s_num = 1;
	while (1)
	{
		u64 t1 = OmnGetTimestamp();
		
		s_num++;
		AosBuffPtr data = OmnNew AosBuff(1000, 0 AosMemoryCheckerArgs);
		u32 data_len = rand() % 1000 + 5;
		data->setDataLen(data_len);
		
		memset(data->data(), 'a', 5);

		//OmnScreen << "sendMsgOrig. num:" << s_num++ 
		//	<< "; orig_len:" << data_len << endl;

		AosRlbMsgPtr msg = OmnNew AosAppMsg(MsgType::eTrans,
				0, 1, ProcessType::eCube, 0, data);
		net_svr->sendMsg(msg);
		//OmnSleep(10);

		while(1)
		{
			AosRlbMsgPtr resp_msg = conn->smartReadSync();
			if(resp_msg->getMsgType() != MsgType::eResp)	continue; 
			//if(resp_msg->getRlbSeq() != s_num)
			//{
				OmnScreen << "main: recv Msg: msg_rlb_seq:" << resp_msg->getRlbSeq() << endl;
			//	continue;
			//}

			AosBuffPtr cont = resp_msg->getCont();
			aos_assert_r(cont, false);
	
			u64 t2 = OmnGetTimestamp();
		
			OmnScreen << "tttttttttttt:" << t2 - t1 << endl;
			break;
		}

		num++;
		if(num % 10000 == 0)
		{
			OmnSleep(1);
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
	OmnScreen << "To stop server!" << endl;

	OmnScreen << "To stop ServerProxy!" << endl;

	OmnScreen << "All Stopped!" << endl;
	OmnScreen << "Exiting!" << endl;

	return true;
}


int redirectOutPut()                                                        
{
	close(STDOUT_FILENO);   
	
	OmnString filename = OmnApp::getAppBaseDir();
	filename << "output_svr_0";

	//FILE* ff = popen("hostname", "r");
	//char buff[128];
	//int len = fread(buff, 1, 128, ff); 
	//buff[len-1]=0;
	//filename << buff << ".txt";
	//pclose(ff);
	//filename << OmnGetTimestamp() << ".txt";
	
	filename << getpid() << ".txt";
	//filename  = "/dev/null";
	int fd = open(filename.data(), O_RDWR|O_TRUNC|O_CREAT, S_IREAD|S_IWRITE|S_IROTH);
	int fd2 = dup2(fd, STDOUT_FILENO);
	if (fd != fd2)
	{
		close(fd);
	}

	return 0;
}
