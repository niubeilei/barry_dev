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
// 2013/02/12	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/BuffArrayVarTesterNew.h"

#if 0
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "Random/RandomUtil.h"
#include "Util/BuffArrayVarNew.h"
#include "Util/CompareFun.h"
#include "Util/SortNew.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosBuffArrayVarTesterNew::AosBuffArrayVarTesterNew()
{
	mName = "AosBuffArrayVarTesterNew";
	mTries = 100000;
	mRundata = OmnNew AosRundata();
}

struct u64comp
{
	bool operator()(const char *s1, const char *s2)
	{
		return (*(u64 *)s1) < (*(u64 *)s2);
	}
};


bool 
AosBuffArrayVarTesterNew::start()
{
	createBuffArray();
	config();
	return basicTest();
}


bool
AosBuffArrayVarTesterNew::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	if (!conf) return true;

	AosXmlTagPtr tag = conf->getFirstChild("testers");
	if (!tag) return true;

	return true;
}


bool
AosBuffArrayVarTesterNew::basicTest()
{
	bool rslt;
	while (mTries--)
	{
		//rslt = checkAppend();
		//rslt = checkInsert();
		rslt = checkmodifyEntries();
		aos_assert_r(rslt, false);
	}
	return true;
}




bool
AosBuffArrayVarTesterNew::createBuffArray()
{
	OmnString str;

	str << "<zky_buffarray zky_stable = \"false\" >"
		<< "<CompareFun cmpfun_alphabetic=\"false\" cmpfun_reserve=\"false\" cmpfun_type=\"varstr\" cmpfun_size=\"4\"></CompareFun>"
		<< 	"</zky_buffarray>";

	AosXmlTagPtr cmp_tag = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(cmp_tag, false);

	AosRundataPtr rdata = OmnApp::getRundata();
	mBuffArrayVarNew = AosBuffArrayVarNew::create(cmp_tag, rdata);
	aos_assert_r(mBuffArrayVarNew, false);

	return true;
}


bool
AosBuffArrayVarTesterNew::checkAppend()
{
	i64 t1=OmnGetTimestamp();
	int total = rand() % 1000 + 1000;
	for (int i=0; i<total; i++)
	{
		OmnString str = "tester";
		str << (int)(1000000 + rand()%8000000);
		//OmnRandom::printableStr(1, 100, false);
		//string str = OmnRandom::digitStr(1, 100);
		int len = str.length();
		mBuffArrayVarNew->appendEntry(mRundata, str.data(), len);
		mVector.push_back(str);
	}

	int numrcds = mBuffArrayVarNew->getNumEntries();
	aos_assert_r(numrcds == mVector.size(), false);

	bool rslt = true;
	for (int i=0; i<numrcds; i++)
	{
		char * data;
		int len = 0;
		rslt = mBuffArrayVarNew->getEntry(mRundata, i, data, len);
		aos_assert_r(rslt, false);

		OmnString ss(data, len);
		aos_assert_r(ss == mVector[i], false);
	}
	
	//sort
	mBuffArrayVarNew->sort();
	sort(mVector.begin(), mVector.end());


	numrcds = mBuffArrayVarNew->getNumEntries();
	aos_assert_r(numrcds == mVector.size(), false);

	rslt = true;
	for (int i=0; i<numrcds; i++)
	{
		char * data;
		int len = 0;
		rslt = mBuffArrayVarNew->getEntry(mRundata, i, data, len);
		aos_assert_r(rslt, false);
		
		OmnString ss(data, len);
		aos_assert_r(ss == mVector[i], false);
	}
	
	i64 t2=OmnGetTimestamp();
	OmnScreen << "append, time cost :" << (t2-t1) << ", num:" << numrcds << endl;
	return true;
}



bool 
AosBuffArrayVarTesterNew::checkInsert()
{	
	i64 t1=OmnGetTimestamp();
	int total = rand() % 1000 + 1000;
	for (int i=0; i<total; i++)
	{
		OmnString str = "tester";
		str << (int)(1000000 + rand()%8000000);
		//string str = OmnRandom::printableStr(1, 100, false);
		int len = str.length();
		mBuffArrayVarNew->insertEntry(mRundata, str.data(), len);
		mVector.push_back(str);
	}

	int numrcds = mBuffArrayVarNew->getNumEntries();
	aos_assert_r(numrcds == mVector.size(), false);
	
	sort(mVector.begin(), mVector.end());

	bool rslt = true;
	for (int i=0; i<numrcds; i++)
	{
		char * data;
		int len = 0;
		rslt = mBuffArrayVarNew->getEntry(mRundata, i, data, len);
		aos_assert_r(rslt, false);

		OmnString ss(data, len);
		aos_assert_r(ss == mVector[i], false);
	}
	
	i64 t2=OmnGetTimestamp();
	OmnScreen << "insert, time cost :" << (t2-t1) << ", num:" << numrcds << endl;
	return true;
}




bool
AosBuffArrayVarTesterNew::checkmodifyEntries()
{
	int total = rand() % 1000 + 1000;
	for (int i=0; i<total; i++)
	{
		OmnString str = "tester";
		str << (int)(1000 + rand()%80000000);
		//OmnRandom::printableStr(1, 100, false);
		//string str = OmnRandom::digitStr(1, 100);
		int len = str.length();
		mBuffArrayVarNew->appendEntry(mRundata, str.data(), len);
		mVector.push_back(str);
	}

	int pos = rand()%mVector.size();
	OmnString newdata = "zky_levixxxxx";
	
	int len = 0;
	char* data;
	mBuffArrayVarNew->getEntry(mRundata, pos, data, len);
	cout <<" ==============data==========" << data << endl;
	
	//modify the data
	mBuffArrayVarNew->modifyEntry(mRundata, pos, newdata.data(), newdata.length());
	
	//get the modify data
	mBuffArrayVarNew->getEntry(mRundata, pos, data, len);
	cout << "====== modifydata: ======" << data; 
	aos_assert_r(strncmp(newdata.data(), data, len) == 0, false);

	return true;
}


#endif
