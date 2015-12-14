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

#include "DataCollector/Testers/IILMergerTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "DataSort/DataSort.h"
#include "IILMgr/Ptrs.h"
#include "IILMerger/Ptrs.h"
#include "DataCollector/Ptrs.h"
#include "DataCollector/DataCollectorMgr.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"
#include "Util/BuffArray.h"
#include "Util1/Time.h"
#include "WordMgr/WordIdHash.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThreadShellMgr.h"
#include "Porting/Sleep.h"

#include <dirent.h>

OmnMutex sgLock;

multiset<string>  AosIILMergerTester::smCheck000;
//OmnMutexPtr AosIILMergerTester::sgLock = OmnNew OmnMutex();
vector<OmnThreadPtr> AosIILMergerTester::mThreads;
set<OmnString>  AosIILMergerTester::smCltid;
//bitset<10000000000> AosIILMergerTester::smBitSet;
int   AosIILMergerTester::smTotalEntries = 0;
static u64 t1 = OmnGetSecond();	
static int64_t iilnums = 5;
static int64_t thrds = 5;
static int trytest = 0;

int64_t loop1 = 1000;
int64_t loop2 = 100000;


AosIILMergerTester::AosIILMergerTester()
{
	OmnScreen << " AosIILMergerTester () " << endl;
}


AosIILMergerTester::~AosIILMergerTester()
{
	OmnScreen << " ~~~ AosIILMergerTester () " << endl;
}

char sg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
             'h', 'i', 'j', 'k', 'l', 'm', 'n',
             'o', 'p', 'q', 'r', 's', 't', 'u',
             'v', 'w', 'x', 'y', 'z'};

u64 sgNums[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


int sgnum = 0;
void randstr(char *buff, int maxlen)
{
sgLock.lock();
	int num = sgnum;
	sgnum++;
	if (sgnum % 1000 == 0)
	{
		sgnum = 0;
	}
sgLock.unlock();
	buff[5] = '_';
	buff[6] = sg[num%sizeof(sg)];
	buff[7] = '_';

	int len=sprintf(&buff[8], "%d", num);
	aos_assert(7+len<maxlen);
	u32 hashkey = AosGetHashKey(&buff[6], len+2);
	hashkey %= 100000;
	sprintf(&buff[0], "%05d", hashkey);
	buff[5] = '_';
	buff[6] = sg[num%sizeof(sg)];
}

bool
AosIILMergerTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	aos_assert_r(basicTest(thread->getLogicId()/iilnums, thread->getLogicId()), false);
	state = OmnThrdStatus::eStop;
	return true;
}


bool
AosIILMergerTester::start()
{
	trytest++;
	cout << "********    Start Tester  " << trytest << " ...   *******" << endl;
	mComp = OmnNew AosFunStrU641(30);
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
	OmnString iilname = "bb";
//	iilname << iilid;

	OmnString opr = "norm";
	aos_assert_r(opr != "", false);
	AosDataColOpr::E oper = AosDataColOpr::toEnum(opr);
	bool isvalid = AosDataColOpr::isValid(oper);
	aos_assert_r(isvalid, false);
	
	//mDataSort = OmnNew AosDataSort(iilname, NULL, mComp, oper, 100000000, 10000000, 100000000, 1500000, mRundata);
	mDataSort = OmnNew AosDataSort(iilname, NULL, mComp, oper, 1000, 10000, 1000000, 1500, mRundata);
	
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<thrds; i++)
	{
		OmnThreadPtr thread  = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		mThreads.push_back(thread);
		thread->start();
	}
	return true;
}	

bool 
AosIILMergerTester::basicTest(const int iilid, const int logicid)
{	
OmnScreen << " start basicTest .... " << endl;
static u64 stime = OmnGetSecond();
OmnScreen << " start time == " << stime << endl;
    
	static int64_t count = 0;
	int allcount = 0;

	for (int j=0; j< loop1; j++)
	{
		int num =  loop2;//rand()%100 + 100;
		AosBuffArrayPtr array = OmnNew AosBuffArray(mComp, false, false, 0);
		//sgLock->lock();
		sgLock.lock();
		count += num;
		allcount += num;

		if ((j%10) == 0)
		{
			u64 time = OmnGetSecond();
			if ((time - t1) != 0)
			{
				OmnScreen << "speed : total=" << count << "\t" << count / (OmnGetSecond()- t1) <<"(entriy/sec)" << endl;
			}
		}
		//sgLock->unlock();
		sgLock.unlock();
		for (int i=0; i<num; i++)
		{
			char buff[28];
			randstr(buff, 25);
			aos_assert_r(buff[0] != 0, false);
			array->addValue(buff, 0, NULL);
			
		}
		mDataSort->addDataSync(array, mRundata);
//		array.sort();
	}
	if (count == loop1 * loop2 * iilnums)
	{
OmnScreen << "finish entries : " << count << endl;
		mDataSort->finish(count, mRundata);
	}
	
u64 etime = OmnGetSecond();
OmnScreen << "entries : " << cout << " " << stime << ":" << etime << " spend time = " << etime-stime << endl;
	return true;
}

