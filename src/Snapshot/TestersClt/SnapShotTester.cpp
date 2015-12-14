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
#include "Snapshot/TestersClt/SnapShotTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/Ptrs.h"
#include "IILMgr/IILStr.h"
#include "IILTrans/AllIILTrans.h"
#include "SEUtil/IILIdx.h"
#include "Porting/Sleep.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Snapshot/TestersClt/IILProcTester.h"
#include "Thread/Mutex.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

AosSnapShotTester::AosSnapShotTester()
{
}


bool 
AosSnapShotTester::start()
{
	OmnScreen << "    Start Tester ..." << endl;

	config();
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
		proc(rdata, logicid);
	}
	return true;
}


bool
AosSnapShotTester::proc(
		const AosRundataPtr &rdata,
		const u32 logicid)
{
	AosIILProcTesterPtr proc = init(rdata, logicid);
	if (!proc)
	{
		proc = createIIL(rdata, logicid);
	}

	addEntry(rdata, proc);
	return true;
}


AosIILProcTesterPtr
AosSnapShotTester::init(
		const AosRundataPtr &rdata,
		const u32 logicid)
{
	OmnFilePtr file = openFile(logicid);
	u32 buff_len = file->getLength();
	AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	if(!buff_len) return 0;

	file->readToBuff(buff, buff_len + 1);
	OmnString iilname = buff->getOmnStr("");
	aos_assert_r(iilname != "", 0);

	AosIILProcTesterPtr proc = OmnNew AosIILProcTester(rdata, iilname, logicid);
	aos_assert_r(proc, 0);
	proc->serializeFrom(buff);
	return proc;
}


bool 
AosSnapShotTester::basicTest()
{
OmnScreen << "Now, begin to torturer!" << endl;
	OmnThreadedObjPtr thisptr(this, false); 
	for (int i = 0; i < eThreadNum; i++)
	{
		mThread[i] = OmnNew OmnThread(thisptr, "SnapShotTester", i, false, true, __FILE__, __LINE__);
		mThread[i]->start(); 
	}
	return true;
}


AosIILProcTesterPtr
AosSnapShotTester::createIIL(
		const AosRundataPtr &rdata,
		const u32 logicid)
{
	u32 length = random()%5 + 5;
	OmnString iilname = getRandomStr(length);
	iilname << "_" << logicid;
	AosIILProcTesterPtr proc = OmnNew AosIILProcTester(rdata, iilname, logicid);
	aos_assert_r(proc, 0);
	return proc;
}

bool
AosSnapShotTester::addEntryByCreateSnap(
		const AosRundataPtr &rdata,
		const AosIILProcTesterPtr &proc)
{
	int tries = 0;
	while(++tries <= 100)
	{
		bool snap = false;
		if (rand() % 4 != 0) snap = true;
		if (tries == 1) snap = false;

		OmnScreen << "=======================  logicid :"<< proc->getIdx() << " , tries: " << tries << " is createSnapShot: " << (snap?"true":"false")<< endl;

		if (snap) createSnapShot(proc);

		int i = 0;
		int r = rand() % 7000;
		while(i++ < r) proc->basicTest();

		if (snap) 
		{
			if (rand() %5 != 0)
			{
				OmnScreen << "commitSnapShot commitSnapShot " << endl;
				commitSnapShot(proc);
			}
			else
			{
				OmnScreen << "rollBackSnapShot rollBackSnapShot" << endl;
				rollBackSnapShot(proc);
			}
		}

		saveToFile(proc);
		proc->checkValue();
	}
	return true;
}


bool
AosSnapShotTester::addEntry(
		const AosRundataPtr &rdata,
		const AosIILProcTesterPtr &proc)
{
	int tries = 0;
	while(++tries <= 100)
	{
		int r = rand() % 7000;
		OmnScreen << "=======================  logicid :"<< proc->getIdx() << " , tries: " << tries << " loop: " << r << endl;

		int i = 0;
		while(i++ < r) proc->basicTest();
		saveToFile(proc);
		proc->checkValue();
	}
	return true;
}


OmnString
AosSnapShotTester::getRandomStr(u32 length)
{
	OmnString str = "";
	for (u32 i=0; i<length; i++)
	{
		str << (char)(random()%26 + 'a');	
	}
	return str;
}


bool
AosSnapShotTester::createSnapShot(const AosIILProcTesterPtr &proc)
{
	//int index = rand() % mIILInfo.size();
	//AosIILProcTesterPtr proc = mIILInfo[index];
	//saveToFile(proc);
	proc->createSnapShot();
	return true;
}


bool
AosSnapShotTester::commitSnapShot(const AosIILProcTesterPtr &proc)
{
	//int index = rand() % mIILInfo.size();
	//AosIILProcTesterPtr proc = mIILInfo[index];
	proc->commitSnapShot();
	//saveToFile(proc);
	return true;
}


bool
AosSnapShotTester::rollBackSnapShot(const AosIILProcTesterPtr &proc)
{
	//int index = rand() % mIILInfo.size();
	//AosIILProcTesterPtr proc = mIILInfo[index];
	proc->rollBackSnapShot();
	proc->clearMap();
	readFromFile(proc);
	return true;
}


bool
AosSnapShotTester::checkValue()
{
	//int index = rand() % mIILInfo.size();
	//AosIILProcTesterPtr proc = mIILInfo[index];
	//proc->checkValue();
	return true;
}


bool
AosSnapShotTester::saveToFile(const AosIILProcTesterPtr &proc)
{
	OmnFilePtr file = openFile(proc->getIdx());
	AosBuffPtr buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	buff->setOmnStr(proc->getIILName());
	proc->serializeTo(buff);
	file->put(0, buff->data(), buff->dataLen(), true); 
	return true;
}


bool
AosSnapShotTester::readFromFile(const AosIILProcTesterPtr &proc)
{
	OmnFilePtr file = openFile(proc->getIdx());
	u32 buff_len = file->getLength();
	AosBuffPtr buff = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	if(!buff_len) return true;

	file->readToBuff(buff, buff_len + 1);
	OmnString iilname = buff->getOmnStr("");
	aos_assert_r(iilname == proc->getIILName(), false);
	proc->serializeFrom(buff);
	return true;
}

	
OmnFilePtr
AosSnapShotTester::openFile(const int idx)
{
	OmnString fname = "./SnapShot_Tester_Data";
	fname << "_" << idx;
	OmnFilePtr file = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if (!file->isGood())
	{
		file = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
	}
	aos_assert_r(file && file->isGood(), 0);
	return file;
}




