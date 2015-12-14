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
//
// Modification History:
// 06/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "NetworkMgr/Testers/NetworkMgrTester.h"

#include "API/AosApi.h"
#include "NetworkMgr/NetworkMgr.h"
#include "NetworkMgr/Testers/CommandRunner.h"
#include "NetworkMgr/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosNetworkMgrTester::AosNetworkMgrTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
}


bool 
AosNetworkMgrTester::start()
{
	cout << "Start NetworkMgr Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosNetworkMgrTester::basicTest()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild("testers");
	aos_assert_r(tag, false);

	AosRundataPtr rdata = new AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(200);
	rdata->setUserid(300);

	int num_virtuals = AosGetNumCubes();
	for (int i=AosModuleId::eInvalid+1; i<AosModuleId::eMax; i++)
	{
		AosModuleId::E module_id = (AosModuleId::E)i;
		AosCommandRunnerObjPtr runner = new AosCommandRunnerTester(module_id);
		aos_assert_r(AosCommandRunnerObj::registerCommandRunner(module_id, runner), false);
	}

	for (int i=AosModuleId::eInvalid+1; i<AosModuleId::eMax; i++)
	{
		AosModuleId::E module_id = (AosModuleId::E)i;
		for (int k=AosNetCommand::eResponse+1; k<AosNetCommand::eMax; k++)
		{
			AosBuffPtr buff = new AosBuff(AosMemoryCheckerArgsBegin);
			AosNetCommand::E command = (AosNetCommand::E)k;
			buff->setU32(module_id);
			buff->setU32(command);
			AosXmlTagPtr response;

			for (int m=0; m<num_virtuals; m++)
			{
				AosNetworkMgr::getSelf()->runCommandOnFrontend(m, module_id, command, 
					buff, response, 5, rdata);
			}
		}
	}
	
	return true;
}

