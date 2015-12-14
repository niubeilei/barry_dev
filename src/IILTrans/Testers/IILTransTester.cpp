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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/Testers/IILTransTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "IILClient/IILClient.h"
#include "IILTrans/Testers/IILTransTestThrd.h"
#include "IILTrans/Testers/Ptrs.h"
#include "IILTransServer/IILTransServer.h"
#include "TransClient/TransClient.h"
#include "TransServer/TransSvrConn.h"
#include "Random/RandomUtil.h"
#include "SearchEngine/SeCommon.h"
#include "SearchEngine/DocServer.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"


AosIILTransTester::AosIILTransTester()
{
}


AosIILTransTester::~AosIILTransTester()
{
}


bool 
AosIILTransTester::start()
{
	cout << "    Start Tester ..." << endl;
			
	AosTransProcPtr proc = OmnNew AosIILTransServer();
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);
	AosXmlTagPtr iilTrans = config->getFirstChild("iil_client");
	AosTransClientPtr iil_client = OmnNew AosTransClient(iilTrans, proc);
	AosIILClient::getSelf()->setTransClient(iil_client);

	AosXmlTagPtr transConfig = OmnApp::getAppConfig()->getFirstChild("iil_server");
	AosTransSvrConnPtr conn = AosTransSvrConn::init(transConfig, proc);
	AosTransServerPtr server = conn->getTransServer();
	// Ketty 2012/03/03
	server->recover();
	conn->start();

	AosIILTransTestThrdPtr	threads[mNumThreads];

	for (int i=0; i<mNumThreads; i++)
	{
		threads[i] = OmnNew AosIILTransTestThrd(i, server);
	}

	while (1)
	{
		OmnSleep(10000000);
	}
	return true;
}

