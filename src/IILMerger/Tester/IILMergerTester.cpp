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
#include "IILMerger/Tester/IILMergerTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "IILMgr/Ptrs.h"
#include "IILMerger/Ptrs.h"
#include "IILMerger/IILMergerMgr.h"
#include "IILMerger/IILMergerHandlerStrAddAttrBatch.h"
#include "SEUtil/DocTags.h"
#include "SearchEngine/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/BuffArray.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThreadShellMgr.h"
#include "Porting/Sleep.h"

multiset<string>  AosIILMergerTester::smCheck000;
OmnMutexPtr AosIILMergerTester::sgLock = OmnNew OmnMutex();
vector<OmnThreadPtr> AosIILMergerTester::mThreads;
static u64 t1 = OmnGetSecond();	
static int thrds = 1;
static int trytest = 0;


AosIILMergerTester::AosIILMergerTester()
{
	OmnScreen << " AosIILMergerTester () " << endl;
}


AosIILMergerTester::~AosIILMergerTester()
{
	OmnScreen << " ~~~ AosIILMergerTester () " << endl;
}

void procset(string &name)
{
	AosIILMergerTester::sgLock->lock();
	multiset<string>::iterator itr;
	if ((itr = AosIILMergerTester::smCheck000.find(name)) == AosIILMergerTester::smCheck000.end())
	{
		OmnAlarm << "proc error!" << enderr;
	}
	AosIILMergerTester::smCheck000.erase(itr);
	AosIILMergerTester::sgLock->unlock();
}

void procFinished()
{
	if (!AosIILMergerTester::smCheck000.empty())
	{
		OmnAlarm << "proc error!" << enderr;
		return;
	}
	
	AosIILMergerTester::smCheck000.clear();
	trytest++;
	OmnScreen << "********    Start Tester  " << trytest << " ...   *******" << endl;
	if (trytest > 200) return;
	for (u32 i=0; i<AosIILMergerTester::mThreads.size(); i++)
	{
		AosIILMergerTester::mThreads[i]->start();
	}
}

char sg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
             'h', 'i', 'j', 'k', 'l', 'm', 'n',
             'o', 'p', 'q', 'r', 's', 't', 'u',
             'v', 'w', 'x', 'y', 'z'};

string sgstr[] = {"aaaaaa", "bbbbbb", "cccccc", "dddddd"};

OmnString entry[] = {"20,100,1000", "40,1000,10000", "80,10000,100000", "100, 100000, 1000000"};

void randstr(char *buff, int maxlen)
{
	int len = rand()%maxlen +1;
	for (int i=0; i<len; i++)
	{
		buff[i] = sg[rand()%26];
	}
	buff[len] = 0;
}

void randsamestr(char *buff, int maxlen)
{
	int len = maxlen;
	for (int i=0; i<len; i++)
	{
		buff[i] = 'a';
	}
	buff[len] = 0;
}

void randnumstr(char *buff)
{
	string str = sgstr[rand()%4];
	for(int i=0; i<6; i++)
	{
		buff[i] = str[i];
	}
	buff[6] = 0;
}

bool
AosIILMergerTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	aos_assert_r(basicTest(), false);
	state = OmnThrdStatus::eStop;
	return true;
}


bool
AosIILMergerTester::start()
{
	trytest++;
	cout << "********    Start Tester  " << trytest << " ...   *******" << endl;
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<thrds; i++)
	{
		OmnThreadPtr thread  = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		mThreads.push_back(thread);
		thread->start();
	}
	return true;
}	

void
AosIILMergerTester::randString(char* buff, int i, int style)
{
	// 0. all the data is the same
	// 1. all the data is random
	// 2. some data is same, some data is random
	// 3. there is only some kinds of data
	switch(style)
	{
		case 0:
			randsamestr(buff, 10);
			break;
		case 1:
			randstr(buff, 10);
			break;
		case 2:
			if (i%3 == 0)
			{
				randsamestr(buff, 10);
			}
			else
			{
				randstr(buff, 10);
			}
			break;
		case 3:
			randnumstr(buff);
			break;
		default:
			OmnAlarm << " is out of the boundary ! " << enderr;
			break;
	}
}

void
AosIILMergerTester::getEntryConf(int &bktnum, u32 &mementries, u32 &maxentries)
{
	OmnString str = entry[rand()%3];	
	if (str == "") return;	
	AosStrSplit split;
	bool finished;
	OmnString pair[3];
	split.splitStr(str.data(),",", pair, 3, finished);
	bktnum = atoi(pair[0].data());
	mementries = atoi(pair[1].data());
	maxentries = atoi(pair[2].data());
}

bool 
AosIILMergerTester::basicTest()
{	

    static int64_t count = 0;
	int allcount = 0;
	AosCompareFunPtr cmp = OmnNew AosFunStrU641(24);
	AosIILMergerHandlerPtr handler = OmnNew AosIILMergerHandlerStrAddAttrBatch
		(cmp, false, 0, 0, false, false, (AosOrder::E)0);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	
	u32 mementries = 0;
	u32 maxentries = 0;
	int bktnum = 0;
	getEntryConf(bktnum, mementries, maxentries);
	aos_assert_r(bktnum != 0, false);
	aos_assert_r(mementries != 0, false);
	aos_assert_r(maxentries != 0, false);

	AosIILMergerMgr::create("aa", handler, thrds, bktnum, mementries, maxentries, rdata);

	int style = rand() % 4;
	switch(style)
	{
		case 0:
			OmnScreen << " rand same string " << endl;
			break;
		case 1:
			OmnScreen << "rand string " << endl;
			break;
		case 2:
			OmnScreen << "rand same string and rand string " << endl;
			break;
		case 3:
			OmnScreen << " rand num kinds " << endl;
			break;
		default:
			OmnAlarm << " is out of the boundary ! " << enderr;
			break;
	}
	int tries = rand() % 10;
	tries *= bktnum;
	tries += 1;
	for (int j=0; j < tries; j++)
	{
		int num = rand()%maxentries + 1;
		AosBuffArray array(cmp, false, false);
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
		for (int i=0; i<num; i++)
		{
			char buff[16];
			randString(buff, i, style);
			array.addValue(buff, 0, NULL);
			sgLock->lock();
			smCheck000.insert(buff);
			sgLock->unlock();
			aos_assert_r(buff[0] != 0, false);
		}
	  	array.sort();
	 	aos_assert_r(AosIILMergerMgr::add("aa", array.getBuff(), rdata), false);
	}
OmnScreen << "smcheck000 size = " << smCheck000.size() << endl;
	AosIILMergerMgr::finish("aa", allcount, "", 0, 0, "", rdata);
	return true;
}

