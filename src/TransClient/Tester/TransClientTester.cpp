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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TransClient/Tester/TransClientTester.h"

#include "alarm_c/alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "TransClient/TransClient.h"
#include "TransClient/Tester/TransSvrProcLocal.h"
#include "TransUtil/RoundRobin.h"
#include "TransServer/Torturer/TransCheck.h"
#include "TransServer/Ptrs.h"
#include "Porting/Sleep.h"
#include "SEUtil/DocTags.h"
#include "XmlUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

#include <deque>
#include <vector>
#include <sys/times.h>

AosTransClientTester::AosTransClientTester()
:
mLock(OmnNew OmnMutex())
{
	mName = "TransClientTester";
}


AosTransClientTester::~AosTransClientTester()
{
}


bool 
AosTransClientTester::start()
{
	cout << "Start AosTransClient Tester ..." << endl;
	
	AosXmlTagPtr config = OmnApp::getAppConfig()->getFirstChild(AOSCONFIG_TRANS);
	aos_assert_r(config, false);
	AosTransDistributorPtr roundRobin = OmnNew AosRoundRobin();
	//OmnString idFileFname = "/home/ice/AOS/src/TransClient/Tester/Data/TaskSubId";
	//AosTransClient::openTaskSubIdFile(idFileFname);
	mTransClient = OmnNew AosTransClient(config, roundRobin);
	mTransClient->recover();
	// local trans
	//AosXmlTagPtr config = OmnApp::getAppConfig();
	//aos_assert_r(config, false);
	//AosTransProcPtr transProc = OmnNew AosTransSvrProcLocal();     //this is transserver proc
	//mTransClient = OmnNew AosTransClient(config, transProc);

	// start thread
	OmnThreadedObjPtr thisPtr(this, false);
	u32 threadNum = 10;
	OmnThreadPtr thread[threadNum];
	for(u32 i=0; i<threadNum; i++)
	{
		thread[i] = OmnNew OmnThread(thisPtr, "Thrd", i, true, true);
		thread[i]->start();
	}
	
	while(1)
	{
		OmnSleep(10000);
	}

	return true;
}

bool
AosTransClientTester::threadFunc(
		OmnThrdStatus::E &state,
		const OmnThreadPtr &thread)
{
	u32 tid = thread->getLogicId();
	u32 i = 0;
OmnScreen << "---------client add Trans Thrd" << endl;

	static u64 t1 = OmnGetSecond(); 
	static u32 total=0;
	while(state == OmnThrdStatus::eActive)
	{
	
		mLock->lock();
		if(total % 50000 == 0)
		{
			u64 t2 = OmnGetSecond();
			if(total) OmnScreen << "each 50000 trans times:" << t2-t1 
							<< "; each secnod:" << total/(t2-t1)	
							<< endl;
			//OmnSleep(2);
			//t1 = t2;
		}
		total++;
		mLock->unlock();
	
		bool continu = AosTransCheckModifyTries();
		if(continu)
		{
			basicTest(tid, i++);
		}
		else
		{
			state = OmnThrdStatus::eStop;
		}
	}

	OmnScreen << "Thread:" << tid
			  << "Existing!" << endl;
	return true;
}


bool 
AosTransClientTester::basicTest(u32 tid, u32 tries)
{
	OmnString log_str;
	AosXmlTagPtr log;

	log_str = "<log";
	log_str << "_thread_" << tid << ">"
			<< tries
			<< "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			<< "</log_thread_" << tid << ">";
	
	AosXmlParser parser;
	log = parser.parse(log_str, "" AosMemoryCheckerArgs);
	aos_assert_r(log, false);
	AosRundataPtr rdata = OmnApp::getRundata(); 
	rdata->setSiteid(100);	
	
	/*
	mTransClient->addTrans(rdata, log, true, tries);
	*/

	AosXmlTagPtr response;
	u32 timer = 1000;
	bool timeout = false;
	
	u64 t1 = OmnGetTimestamp(); 
	//struct tms tmsStart, tmsEnd;
	//clock_t start, end;
	//start = times(&tmsStart);

	mTransClient->addTrans(rdata, log, response, timeout, false, tries);
	if(!timeout)
	{
	//	end = times(&tmsEnd);
	//	OmnScreen << "proc successful; timer:" << (OmnGetTimestamp() - t1)
			//	<< "; resp" << response->toString()
	//			<< "; usr_time" << (tmsEnd.tms_utime - tmsStart.tms_utime)
	//			<< "; sys_time" << (tmsEnd.tms_stime - tmsStart.tms_stime)
	//			<< endl;
		if(!response)
		{
			OmnAlarm << "missing response!" << enderr;
			return true;
		}
		//OmnScreen << "proc successful !"
		//		<< "; resp:" << response->toString()	
		//		<< endl;
	}
	return true;
}
