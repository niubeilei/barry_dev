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

#include "Sorter/Testers/SorterTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "IILMgr/Ptrs.h"
//#include "Sorter/Ptrs.h"
#include "DataSort/DataSort.h"
#include "DataSort/Ptrs.h"
#include "Sorter/MergeSorter.h"
#include "Sorter/MultiFileSorter.h"
#include "DataTypes/DataColOpr.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/DataScannerObj.h"
#include "SearchEngine/Ptrs.h"
#include "Random/RandomUtil.h"
#include "Porting/Sleep.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/BuffArray.h"
#include "Util1/Time.h"
#include "WordMgr/WordIdHash.h"
#include "XmlUtil/XmlTag.h"
#include "Thread/ThreadShellMgr.h"
#include "Porting/Sleep.h"
#include <vector>
//#include <stdlib.h>
//#include <set>
//using namespace std;

multiset<string>  AosSorterTester::smCheck000;
OmnMutex sgLock;
//extern multiset<OmnString>  smCheck000;
//static u64 t1 = OmnGetSecond();	

AosSorterTester::AosSorterTester()
{
	OmnThreadedObjPtr thisPtr(this, false);
	for (int i=0; i<0; i++)
	{
		OmnThreadPtr thread  = OmnNew OmnThread(thisPtr, "Thrd", i, true, true, __FILE__, __LINE__);
		mThreads.push_back(thread);
		thread->start();
	}
}


AosSorterTester::~AosSorterTester()
{
}

bool dataCheck(
		OmnString const&, 
		OmnString const&, 
		OmnSPtr<AosCompareFun> const&)
{
	return true;
}

static int cnt = 0;

char sg[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g',
             'h', 'i', 'j', 'k', 'l', 'm', 'n',
             'o', 'p', 'q', 'r', 's', 't', 'u',
             'v', 'w', 'x', 'y', 'z'};

int sgnum = 0;
void randstr(char *buff, int maxlen)
{
//sgLock.lock();
	int num = sgnum;
	sgnum++;
//sgLock.unlock();
	buff[5] = '_';
	buff[6] = sg[num%sizeof(sg)];
	buff[7] = '_';

	int len=sprintf(&buff[8], "%d", num);
	aos_assert(7+len<maxlen);
	u32 hashkey = AosGetHashKey(&buff[6], len+2);
	hashkey %= 100000;
	sprintf(&buff[0], "%05d", hashkey);
	buff[5] = '_';
}


bool
AosSorterTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	start();
	state = OmnThrdStatus::eStop;
}

bool
AosSorterTester::checkSort(const AosBuffPtr &new_buff, const AosBuffPtr &tmp_buff, const AosCompareFunPtr &comp)
{
	aos_assert_r(new_buff->dataLen() == tmp_buff->dataLen(), false);	
	char *crt = new_buff->data();
	char *tmp = tmp_buff->data();
	int size = new_buff->dataLen() / comp->size;
	for (int i=0; i<size ; i++)
	{
		if (crt[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
		aos_assert_r(strcmp(crt, tmp) == 0, false);
		crt += comp->size;
		tmp += comp->size;
	}
	return true;
}

bool
AosSorterTester::checkSort(const AosBuffPtr buff, const AosCompareFunPtr &comp)
{
	int size = buff->dataLen() / comp->size;
	char *data = buff->data(); 
	for(int i=0; i<size; i++)
	{
		// entrie must not empty
		if(data[0] == 0)
		{
			OmnAlarm << "the entry is empty" << enderr;
		}
	
		// entries must sorted
	    if (i > 0)
		{
			aos_assert_r(comp->cmp(data, data-comp->size) >= 0, false);
		}
		
		data += comp->size;
	}
	return true;
}

bool 
AosSorterTester::start()
{
/*	cout << "    Start Tester ..." << endl;
	AosCompareFunPtr cmp1 = OmnNew AosFunStrU641(30);
	int64_t minsize = 8000000;
	int64_t maxsize = 800000000;
	int64_t buffsize = 1500000;
	AosDataSortPtr sort;
	vector<u64> fields;
	AosMultiFileSorter *sorter = OmnNew AosMultiFileSorter(sort, minsize, maxsize, buffsize, AosDataColOpr::eNoUpdate, cmp1, fields, NULL);
	sorter->sort(NULL);                         
	OmnScreen << "Finished1" << endl;
	OmnSleep(10000);
	return 0;
*/
	int record_size = 30;
	AosCompareFunPtr cmp = OmnNew AosFunStrU641(record_size);
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);//OmnNew AosRundata();
	AosBuffArrayPtr array1 = OmnNew AosBuffArray(cmp, false, false, 0);
	int num = 2500;
	for(int i=0; i<num; i++)
	{
		char buff[24];
		randstr(buff, 23);
		array1->appendEntry(buff, 0, NULL);
		aos_assert_r(buff[0] != 0, false);
	}
	array1->sort();
	AosBuffArrayPtr array2 = OmnNew AosBuffArray(cmp, false, false, 0);
	AosBuffArrayPtr array3 = OmnNew AosBuffArray(cmp, false, false, 0);
/*u64 ct1 = OmnGetTimestamp();
OmnScreen << " start create data entries : " << num << endl;

	for(int i=0; i<num; i++)
	{
		char buff[24];
		randstr(buff, 23);
		array2->addValue(buff, 0, NULL);
		aos_assert_r(buff[0] != 0, false);
	}
	array2->sort();

	for(int i=0; i<num; i++)
	{
		char buff[24];
		randstr(buff, 23);
		array3->addValue(buff, 0, NULL);
		aos_assert_r(buff[0] != 0, false);
	}
	array3->sort();
OmnScreen << " end create data spend : " << OmnGetTimestamp() - ct1 << endl;
*/

	int array_num = 1000;
	u64 total_size = (array_num+1) * num * record_size;
	
	u64 t3 = OmnGetTimestamp();
	for(int j=0; j<array_num; j++)
	{
		array2 = OmnNew AosBuffArray(cmp, false, false, 0);
		for(int i=0; i<num; i++)
		{
			char buff[24];
			randstr(buff, 23);
			array2->appendEntry(buff, 0, NULL);
			aos_assert_r(buff[0] != 0, false);
		}
		array2->sort();
		//array1->merge(rdata, array2);
	}

	OmnScreen << "######  Merge Sort spend : " << OmnGetTimestamp() - t3 << "  us  ######" << endl;
	int64_t size = array1->size();
	aos_assert_r(size == total_size, false);
	aos_assert_r(checkSort(array1->getBuff(), cmp), false);

	AosBuffPtr new_buff = OmnNew AosBuff(total_size AosMemoryCheckerArgs);
	char *crt = new_buff->data();
	memcpy(crt, array1->first(), num * record_size);
	crt += num * record_size;
	
	memcpy(crt, array2->first(), num * record_size);
	new_buff->setDataLen(total_size);
	
	AosBuffArray array(new_buff, cmp, false);

	u64 t1 = OmnGetTimestamp();
	array.sort();
	u64 t2 = OmnGetTimestamp();
	OmnScreen << "######  Norm Sort spend : " << t2 - t1 << "  us  ######" << ":" << t1 << ":" << t2 << endl;

	aos_assert_r(checkSort(new_buff, cmp), false);
	
	AosDataColOpr::E opr = AosDataColOpr::toEnum("index");
	//SorterTesterPtr thisptr(this, false);
	
	AosMergeSorter<AosBuffArrayPtr> mergeSorter(array1, array2, opr, cmp.getPtr(), NULL, 0, (void*)false, rdata);
	//AosMergeSorterPtr mergeSorter = OmnNew AosMergeSorter(array1, array2, opr);
	t3 = OmnGetTimestamp();
	//AosBuffPtr buff = mergeSorter->sort(rdata);
	AosBuffPtr buff = mergeSorter.sort(rdata);
	aos_assert_r(checkSort(buff, cmp), false);
	return true;
}

