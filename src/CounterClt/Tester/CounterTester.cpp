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
// 2011/06/15	Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "CounterClt/Tester/CounterTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "CounterClt/CounterClt.h"
#include "CounterTime/TimeInfo.h"
#include "CounterUtil/CounterName.h"

int AosCounterTester::mNumThreads = 1;
int AosCounterTester::mNumFinished = 0;


AosCounterTester::AosCounterTester()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mName("CounterTester")
{

	initSimplemData();
	initTimemData();
}


bool
AosCounterTester::initSimplemData()
{
	// This function tests a number of counters:
	mCnames[0] = "aaaa|$$|bbbb|$$|cccc|$$|docid|$|123456";
	mSegments[0][0] = "aaaa|$$|bbbb|$$|cccc|$$|docid|$|123456";
	mSegments[0][1] = "bbbb|$$|cccc|$$|docid|$|123456";
	mSegments[0][2] = "cccc|$$|docid|$|123456";
	
	mCnames[1] = "one|$$|two|$$|three|$$|docid|$|62345";
	mSegments[1][0] = "one|$$|two|$$|three|$$|docid|$|62345";
	mSegments[1][1] = "two|$$|three|$$|docid|$|62345";
	mSegments[1][2] = "three|$$|docid|$|62345";
	
	mCnames[2] = "计算机|$$|处理器|$$|内存|$$|docid|$|6341";
	mSegments[2][0] = "计算机|$$|处理器|$$|内存|$$|docid|$|6341";
	mSegments[2][0] = "处理器|$$|内存|$$|docid|$|6341";
	mSegments[2][0] = "内存|$$|docid|$|6341";

	mCnames[3] = "等等等|$$|docid|$|111";
	mSegments[3][0] = "等等等|$$|docid|$|111";

	mCnames[4] = "得到|$$|check|$$|考|$$|english|$$|docid|$|3647";
	mSegments[4][0] = "得到|$$|check|$$|考|$$|english|$$|docid|$|3647";
	mSegments[4][1] = "check|$$|考|$$|english|$$|docid|$|3647";
	mSegments[4][2] = "考|$$|english|$$|docid|$|3647";
	mSegments[4][3] = "english|$$|docid|$|3647";

	// init the mCounterMap
	for (int i = 0; i < eNumCounters; i++)
	{
		for(int j = 0; j < eMaxSegments; j++)
		{
			if (mSegments[i][j] != "")
			{
				 mCounterMap.insert(pair<OmnString, int64_t>(mSegments[i][j], 0));
			}
		}
	}
	return true;
}


bool
AosCounterTester::initTimemData()
{
	// construct the time0
	TimeStruct  time0;
	time0.year = 2000;
	time0.value = 0;
	for(int i = 0; i < 12; i++)
	{
		time0.month[i] = 0;
	}
	for(int j = 0; j < 366; j++)
	{
		time0.day[j] = 0;
	}

	// construct the time1
	TimeStruct  time1;
	time1.year = 2001;
	time1.value = 0;
	for(int i = 0; i < 12; i++)
	{
		time1.month[i] = 0;
	}
	for(int j = 0; j < 366; j++)
	{
		time1.day[j] = 0;
	}

	// construct the time2
	TimeStruct  time2;
	time2.year = 2002;
	time2.value = 0;
	for(int i = 0; i < 12; i++)
	{
		time2.month[i] = 0;
	}
	for(int j = 0; j < 366; j++)
	{
		time2.day[j] = 0;
	}
	
	// construct the record0
	CounterSturct record0;
	record0.cType = "YYYY";
	record0.cName = "结构体|$$|docid|$|654321";
	record0.value = 0;
	record0.times[0] = time0;
	record0.times[1] = time1;
	record0.times[2] = time2;

	// construct the record1
	CounterSturct record1;
	record1.cType = "MMYYYY";
	record1.cName = "苏州|$$|docid|$|654321";
	record1.value = 0;
	record1.times[0] = time0;
	record1.times[1] = time1;
	record1.times[2] = time2;

	// construct the record1
	CounterSturct record2;
	record2.cType = "DDMMYYYY";
	record2.cName = "成都|$$|docid|$|654321";
	record2.value = 0;
	record2.times[0] = time0;
	record2.times[1] = time1;
	record2.times[2] = time2;

	mRecordCounters[0] = record0;
	mRecordCounters[1] = record1;
	mRecordCounters[2] = record2;
	return true;
}


bool 
AosCounterTester::start()
{
	OmnScreen << "    Start Counter Tester ..." << endl;

	AosCounterTesterPtr thisptr(this, false);

	for (int i=0; i<mNumThreads; i++)
	{
		//AosCounterTesterPtr tester = clone();
		AosCounterTesterThrdPtr thrd = OmnNew AosCounterTesterThrd(thisptr, i);
		//tester->setTestMgr(mTestMgr);
		mThreads.push_back(thrd);
	}

	mLock->lock();
	mCondVar->wait(mLock);
	aos_assert_rl(mNumFinished == mNumThreads, mLock, false);
	mLock->unlock();
	return true;
}


bool 
AosCounterTester::basicTest(const int thrdid)
{
    int tries = mTestMgr->getTries();
    OmnScreen << "Tries: " << tries << endl;

    if (tries <= 0) tries = eDefaultTries;
	
	for(int i = 0; i< tries; i++)
	{
		aos_assert_r(counterTest(), false);
	}
	return true;
}


bool
AosCounterTester::counterTest()
{
	static int i = 0;

	if (++i % 1000 == 0)
	{
		OmnScreen << "counter test i:" << i << endl;
	}
	int randomValue = rand() % 100;

	if (randomValue < 40)
	{
		aos_assert_r(simpleCounterTest(), false);
		return true;
	}
	if (randomValue < 95)
	{
		aos_assert_r(timeCounterTest(), false);
		return true;
	}

	//aos_assert_r(incorrectCounterTest(), false);
	return true;
}


bool
AosCounterTester::simpleCounterTest()
{
	AosRundataPtr rdata = OmnNew AosRundata();
	static int number = 0;
	int realNumber = 0;
	mLock->lock();
	number++;
	realNumber = number;
	mLock->unlock();
	int idx = rand() % eNumCounters;

	//int64_t temp = rand();
	//int64_t value = temp % 10 < 8 ? temp | 0x80000000 : temp;
	int64_t value = 1;
	mLock->lock();
	bool rslt = AosCounterClt::getSelf()->procCounter(mCnames[idx], value, "", "", eTimeOut, rdata);
	
	if (!rslt)
	{
		OmnScreen << "Failed processing counter(result): " << idx << " : " << mCnames[idx] << " : " << value << endl;
	}
	if (!rdata->isOk())
	{
		OmnScreen << "Failed processing counter(rdata status): " << idx << " : " << mCnames[idx] << " : " << value << endl;
	}
	//AosTC(rslt) << "Failed processing counter(result):  " << i << " : " << idx << " : " << value << endtc;
	//AosTC(rdata->isOk()) << "Failed processing counter(rdata status):  " << i << " : " << idx << " : " << value << endtc;

	updateCounters(idx, value);
	
	verifyCounters(realNumber, rdata);

	mLock->unlock();
	return true;
}


bool
AosCounterTester::updateCounters(const int idx, const int64_t &value)
{
	// The counter 'cname' is updated by the value 'value'.
	for (int i=0; i<eMaxSegments; i++)
	{
		OmnString name = mSegments[idx][i];
		if (name == "") 
		{
			return true;
		}

		map<OmnString, int64_t>::iterator itr = mCounterMap.find(name);
		if (itr != mCounterMap.end())
		{
			itr->second += value;
		}
		else
		{
			mCounterMap.insert(pair<OmnString, int64_t>(name, value));
		}
	}
	return true;
}


bool
AosCounterTester::verifyCounters(const int number, const AosRundataPtr &rdata)
{
	int idx = rand() % eNumCounters;

	for (int j=0; j<eMaxSegments; j++)
	{
		OmnString name = mSegments[idx][j];
		if (name == "") break;

		int64_t value = 0;
		bool rslt = AosCounterClt::getSelf()->getCounter(name, "", "", value, eTimeOut, rdata);
		if (!rslt)
		{
			OmnScreen << "Failed get counter value(result): " << name << " : " << j << " : " << endl;
		}
		if (!rdata->isOk())
		{
			OmnScreen << "Failed get counter(rdata status):" << name << " : " << j << " : " << endl;
		}

		//AosTC(rslt) << "Failed get counter value(result): " << name << " : " << j << " : " << endtc;
		//AosTC(rdata->isOk()) << "Failed get counter(rdata status): " << name << " : " << j << " : Errmsg: " << rdata->getErrmsg() << endtc;
		map<OmnString, int64_t>::iterator itr = mCounterMap.find(name);

		if (itr == mCounterMap.end())
		{
			OmnScreen << "Failed finding the counter(torturer map): " << name << endl;
		}
		if (itr->second != value)
		{
			OmnScreen << "Counter mismatch(torturer): MapValue:" << itr->second << "  DbValue: " << value << " number : " << number << " name : " << name << endl;
		}
		//cout << "Verfiy counter name : " << name << " : DbValue " << value << " : MapValue " << itr->second << endl; 
		
		//AosTC(itr != mCounterMap.end()) << "Failed finding the counter(torturer map): " << name << endtc;
		//AosTC(itr->second == value) << "Counter mismatch(torturer): DbValue:" << itr->second << "  MapValue: " << value << endtc;
	}
	return true;
}


bool
AosCounterTester::timeCounterTest()
{
	AosRundataPtr rdata = OmnNew AosRundata();

	int idxName = rand() % eNumTimeCounters;
	aos_assert_r(idxName >=0 && idxName < eNumTimeCounters, false);
	CounterSturct record = mRecordCounters[idxName];    // a record of the mRecordCounters array

	int idxTime = rand() % eMaxTimeSegments;
	aos_assert_r(idxTime >=0 && idxTime < eMaxTimeSegments, false);
	TimeStruct time = record.times[idxTime];			// a time record of the mRecordCounters array's times array

	int idxMonth = rand() % 12;

	int idxRealDay = rand() % 28;

	OmnString timeStr;
	timeStr << time.year << "-" << idxMonth+1 << "-" << idxRealDay+1;

	AosTimeInfo date(timeStr);  // construct date

	int64_t value = 1;
	mLock->lock();
	bool rslt = AosCounterClt::getSelf()->procCounter(record.cName, value, 
			record.cType, timeStr, eTimeOut, rdata);

	if (!rslt)
	{
		OmnScreen << "Failed processing counter(result): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
	}
	if (!rdata->isOk())
	{
		OmnScreen << "Failed processing counter(rdata): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
	}
	
	updateTimeCounter(idxName, idxTime, idxMonth, date.getSpendDay(), value);

	//verifyTimeCounter();
	verifyTimeCounter(idxName, idxTime, idxMonth, date);
	mLock->unlock();
	return true;
}


bool
AosCounterTester::verifyTimeCounter()
{
	AosRundataPtr rdata = OmnNew AosRundata();

	int idxName = rand() % eNumTimeCounters;
	aos_assert_r(idxName >=0 && idxName < eNumTimeCounters, false);
	CounterSturct record = mRecordCounters[idxName];

	int idxTime = rand() % eMaxTimeSegments;
	aos_assert_r(idxTime >=0 && idxTime < eMaxTimeSegments, false);
	TimeStruct time = record.times[idxTime];

	int idxMonth = rand() % 12;

	int idxRealDay = rand() % 28;

	OmnString timeStr;
	timeStr << time.year << "-" << idxMonth+1 << "-" << idxRealDay+1;
	AosTimeInfo date(timeStr);

	int64_t dbValue = 0;

	bool rslt = AosCounterClt::getSelf()->getCounter(record.cName, 
            			record.cType, timeStr, dbValue, 100, rdata);

    if (!rslt)
    {
        OmnScreen << "Failed get counter(result): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
    }
    if (!rdata->isOk())
    {
        OmnScreen << "Failed get counter(rdata): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
    }

	int64_t memValue = 0;
	// verify counter
	if (record.cType == "YYYY")
	{
		memValue = time.value ;
	}
	else if (record.cType == "MMYYYY")
	{
		memValue = time.month[idxMonth];
	}
	else
	{
		memValue = time.day[date.getSpendDay()];
	}

	if (dbValue != memValue)
	{
		OmnScreen << " mismatch(base on time) " << record.cName << " : " << record.cType << " : " << timeStr << " mem: " << memValue << " db: " << dbValue  << endl;
	}
	return true;
}


bool
AosCounterTester::verifyTimeCounter(
			const int idxName,
			const int idxTime,
			const int idxMonth,
			const AosTimeInfo &date)
{
	AosRundataPtr rdata = OmnNew AosRundata();

	aos_assert_r(idxName >=0 && idxName < eNumTimeCounters, false);
	CounterSturct record = mRecordCounters[idxName];

	aos_assert_r(idxTime >=0 && idxTime < eMaxTimeSegments, false);
	TimeStruct time = record.times[idxTime];

	int idxRealDay = date.getDay();

	OmnString timeStr;
	timeStr << time.year << "-" << idxMonth+1 << "-" << idxRealDay;

	int64_t dbValue = 0;

	bool rslt = AosCounterClt::getSelf()->getCounter(record.cName, 
            			record.cType, timeStr, dbValue, 100, rdata);

    if (!rslt)
    {
        OmnScreen << "Failed get counter(result): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
    }
    if (!rdata->isOk())
    {
        OmnScreen << "Failed get counter(rdata): " << record.cName << " : " << record.cType << " : " << timeStr << endl;
    }

	int64_t memValue = 0;
	// verify counter
	if (record.cType == "YYYY")
	{
		memValue = time.value ;
	}
	else if (record.cType == "MMYYYY")
	{
		memValue = time.month[idxMonth];
	}
	else
	{
		memValue = time.day[date.getSpendDay()];
	}

	if (dbValue != memValue)
	{
		OmnScreen << " mismatch(base on time) " << record.cName << " : " << record.cType << " : " << timeStr << " mem: " << memValue << " db: " << dbValue  << endl;
	}
	/*
	else
	{
		OmnScreen << " match(base on time) " << record.cName << " : " << record.cType << " : " << timeStr << " mem: " << time.value << " db: " << dbValue  << endl;
	}
	*/
	return true;
}


bool 
AosCounterTester::updateTimeCounter(
							const int idxName,
							const int idxTime,
							const int idxMonth,
							const int idxSpendDay,
							const int64_t value)
{
	aos_assert_r(idxName >= 0 && idxName < eNumTimeCounters, false);
	aos_assert_r(idxTime >= 0 && idxTime < eMaxTimeSegments, false);
	aos_assert_r(idxMonth >= 0 && idxMonth < 12, false);
	aos_assert_r(idxSpendDay >= 0 && idxSpendDay < 366, false);

	CounterSturct &record = mRecordCounters[idxName];
	TimeStruct &time = record.times[idxTime];
	OmnString cType = record.cType;
	if (cType == "YYYY")
	{
		record.value += value;
		time.value += value;
		return true;
	}
	if (cType == "MMYYYY")
	{
		record.value += value;
		time.value += value;
		time.month[idxMonth] += value;
		return true;
	}
	if (cType == "DDMMYYYY")
	{
		record.value += value;
		time.value += value;
		time.month[idxMonth] += value;
		time.day[idxSpendDay] += value;
		return true;
	}
	return true;

}


bool
AosCounterTester::incorrectCounterTest()
{
	return true;
}


bool
AosCounterTester::threadFinished(const int thrdid)
{
	return true;
	//mLock->lock();
	//aos_assert_rl(thrdid >= 0 && thrdid < (signed int)mThreads.size(), mLock, false);
	//aos_assert_rl(mThreads[thrdid], mLock, false);
	//aos_assert_r(thrdid >= 0 && thrdid < (signed int)mThreads.size(), false);
	//aos_assert_r(mThreads[thrdid], false);
	//mThreads[thrdid] = 0;
	//mNumFinished++;
	if (mNumFinished >= mNumThreads * mTestMgr->getTries())
	{
		mCondVar->signal();
	}
	//mLock->unlock();
	return true;
}

