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
// 08/09/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/SnapShotTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "DocClient/DocClient.h"
#include "Snapshot/Testers/DfmObject.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

AosSnapShotTester::AosSnapShotTester()
{
}

AosSnapShotTester::~AosSnapShotTester()
{
}


bool 
AosSnapShotTester::start()
{
	OmnScreen << "    Start Tester ..." << endl;

	basicTest();
	return true;
}


bool
AosSnapShotTester::config()
{
	return true;
}

bool
AosSnapShotTester::threadFunc(
		  OmnThrdStatus::E &state,
		  const OmnThreadPtr &thread)
{
	bool wait = false;
	while(state == OmnThrdStatus::eActive)
	{
		if (wait) 
		{
			OmnSleep(100);
			continue;
		}
		wait = true; 

		u32 logicid = thread->getLogicId();
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		rdata->setSiteid(100);
		rdata->setUserid(307);
		AosDfmObjectPtr dfm_test;
		if (!mObject[logicid])
		{
			readFromFile(rdata, logicid, dfm_test);
			mObject[logicid] = dfm_test;
		}
		else
		{
			dfm_test = mObject[logicid];
		}
		aos_assert_r(dfm_test, false);

		for (int i = 0; i< 10; i++)
		{
			dfm_test->run(logicid, rdata);
		}

		saveToFile(logicid, dfm_test);
	}
	return true;
}


bool 
AosSnapShotTester::basicTest()
{
OmnScreen << "Now, begin to torturer!" << endl;

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	rdata->setSiteid(100);
	rdata->setUserid(307);
	OmnString docstr = "<ctnr ";
	docstr << "zky_objid =\"" << AOSCTNR_SIZEID << "\" "
		<< "zky_public_doc=\"true\" "
		<< "zky_otype=\"zky_ctnr\" "
		<< "zky_public_ctnr=\"true\"> </ctnr>";
	AosXmlTagPtr dd = AosDocClient::getSelf()->createDoc(docstr, true, rdata);
	aos_assert_r(dd, false);
	OmnSleep(10);

	OmnThreadedObjPtr thisptr(this, false); 
	for (int i = 0; i < eThreadNum; i++)
	{
		mObject[i] = 0;
		mThread[i] = OmnNew OmnThread(thisptr, "SnapShotTester", i, false, true, __FILE__, __LINE__);
		mThread[i]->start(); 
	}


	return true;
}


bool
AosSnapShotTester::saveToFile(
		const u32 logicid,
		const AosDfmObjectPtr &dfm_test)
{	
	OmnScreen << "CCCCCCCCCCC SaveToFile logicid:" << logicid << endl;
	OmnFilePtr file = openFile(logicid);
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	dfm_test->serializeTo(buff);
	file->put(0, buff->data(), buff->dataLen(), true); 
	OmnScreen << "CCCCCCCCCCC SaveToFile End logicid:" << logicid << endl;
	return true;
}


bool
AosSnapShotTester::readFromFile(
		const AosRundataPtr &rdata,
		const u32 logicid,
		AosDfmObjectPtr &dfm_test)
{
	dfm_test = OmnNew AosDfmObject();
	OmnFilePtr file = openFile(logicid, true);
	if (!file) 
	{
		return true;
	}

	u32 buff_len = file->getLength();
	AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	if(!buff_len) return true;

	file->readToBuff(buff, buff_len + 1);
	dfm_test->serializeFrom(rdata, buff);
	return true;
}

	
OmnFilePtr
AosSnapShotTester::openFile(const int idx, const bool read)
{
	OmnString fname = "./SnapShot_Tester_Data";
	fname << "_" << idx;
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!file->isGood())
	{
		if (read) return 0;
		file = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert_r(file && file->isGood(), 0);
	return file;
}
