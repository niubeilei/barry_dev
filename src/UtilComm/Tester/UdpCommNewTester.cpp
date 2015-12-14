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
// 2013/06/15 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilComm/Tester/UdpCommNewTester.h"

#include "AppMgr/App.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThrdShellProc.h"
#include "Util/OmnNew.h"
#include "UtilComm/UdpCommNew.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"
#include "aosUtil/Memory.h"
#include <vector>
using namespace std;

static vector<OmnThrdShellProcPtr> sgRunners;


AosUdpCommTester::AosUdpCommTester()
:
mTries(eDefaultTries),
mId(-1),
mNumRunners(0),
mPort(-1)
{
	mName = "AosUdpCommTester";
}


AosUdpCommTester::AosUdpCommTester(
		const int id, 
		const AosXmlTagPtr &conf)
:
mTries(eDefaultTries),
mId(id),
mNumRunners(0),
mPort(-1)
{
	mName = conf->getAttrStr("name", "name");
	if (mName == "name") mName << mId;

	mAddr = OmnIpAddr(conf->getAttrStr("addr"));
	if (!mAddr.isValid())
	{
		OmnAlarm << "Invalid address: " << conf->toString() << enderr;
		return false;
	}

	mPort = conf->getAttrInt("port", -1);
	if (mPort <= 0)
	{
		OmnAlarm << "Invalid port" << enderr;
		return false;
	}

	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mServer = OmnNew AosUdpComm(mRundata, mName, mAddr, mPort);
	OmnCommListenerPtr thisptr(this, false);
	mServer->startReading(thisptr);
}


AosUdpCommTester::~AosUdpCommTester()
{
}


bool 
AosUdpCommTester::start()
{
	config();
	basicTest();
	return true;
}


bool
AosUdpCommTester::config()
{
	return true;
}


bool 
AosUdpCommTester::basicTest()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild("utilcomm_tester");
	if (!tag)
	{
		OmnAlarm << "Missing <utilcomm_tester>" << enderr;
		return false;
	}

	mTries = tag->getAttrInt("tries", eDefaultTries);
	if (mTries < 1) mTries = eDefaultTries;

	tag = tag->getFirstChild("udpcommnew_tester");
	if (!tag)
	{
		OmnAlarm << "Missing udpcommnew_tester tag" << enderr;
		return false;
	}

	OmnBeginTest << "Test AosUdpComm";
	mTcNameRoot = "UdpCommTest";

	AosXmlTagPtr cfg = tag->getFirstChild();
	if (!cfg)
	{
		OmnAlarm << "Missing thread" << enderr;
		return false;
	}

	int num_runners = 0;
	while (cfg)
	{
		sgRunners.push_back(OmnNew AosUdpCommTester(num_runners, cfg));
		cfg = tag->getNextChild();
	}

	OmnThreadPool pool("test");
	bool rslt = pool.procAsync(sgRunners);
	aos_assert_r(rslt, false);

	while (1)
	{
		bool all_finished = true;
		for (int i=0; i<num_runners; i++)
		{
			if (!sgRunners[i]->isFinished())
			{
				all_finished = false;
				break;
			}
		}

		if (all_finished) return true;
	}
	return true;
}


bool		
AosUdpCommTester::msgRead(const OmnConnBuffPtr &buff)
{
	return true;
}


OmnString	
AosUdpCommTester::getCommListenerName() const
{
	return "udpcomm_tester";
}



void 		
AosUdpCommTester::readingFailed()
{
	OmnAlarm << "Failed reading" << enderr;
}


bool	
AosUdpCommTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	u32 num_runners = sgRunners.size();
	aos_assert_r(num_runners, false);
	while (state == OmnThrdStatus::eActive)
	{
		mThreadStatus = true;

		int idx = rand() % num_runners;
		AosUdpCommPtr comm = sgRunners[idx];
		OmnIpAddr addr = comm->getLocalAddr();
		int port = comm->getLocalPort();


		mServer->sendTo(
	}
	return true;
}


bool	
AosUdpCommTester::signal(const int threadLogicId)
{
}

