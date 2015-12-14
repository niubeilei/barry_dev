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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataShuffler/Testers/DocidShufflerTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "DataShuffler/DataShuffler.h"
#include "DataShuffler/Ptrs.h"
#include "DataCacher/DataCacher.h"
#include "DataCacher/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"


AosDocidShufflerTester::AosDocidShufflerTester()
:mCrtId(0)
{
}


bool 
AosDocidShufflerTester::start()
{
	cout << "    Start AosDataShuffler Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDocidShufflerTester::basicTest()
{
	int tries = mTestMgr->getTries();	
	if (tries < 0) tries = 1;
	AosRundataPtr rdata = OmnApp::getRundata();
	AosXmlTagPtr config = OmnApp::getAppConfig();
	config = config->getFirstChild("docid");
	for (int i = 0; i <tries; i++)
	{
		cout << "tries: " << i << endl;
		test(config, rdata);	
	}
	return true;
}


bool
AosDocidShufflerTester::test(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	int vv = rand() % 2;
	vv = 0;
	if (vv == 0)
	{
OmnScreen << "Test appendEntry Func !" << endl;
		AosXmlTagPtr child = config->getFirstChild("append_entry");
		AosXmlTagPtr dsconf = child->getFirstChild("data_shuffler");
		mNumServers = dsconf->getAttrInt("num_servers", -1);
		mNumVirtuals = dsconf->getAttrInt("num_virtuals", -1);
		AosDataShufflerPtr dataShuffler = AosDataShuffler::getDataShuffler(dsconf, rdata);
		aos_assert_r(dataShuffler, false);
		aos_assert_r(appendEntry(dataShuffler, rdata), false);
	}
	else
	{
OmnScreen << "Test shuffle Func !" << endl;
		AosXmlTagPtr child = config->getFirstChild("shuffle");
		AosXmlTagPtr dsconf = child->getFirstChild("data_shuffler");
		mNumServers = dsconf->getAttrInt("num_servers", -1);
		mNumVirtuals = dsconf->getAttrInt("num_virtuals", -1);
		AosDataShufflerPtr dataShuffler = AosDataShuffler::getDataShuffler(dsconf, rdata);
		aos_assert_r(dataShuffler, false);
		aos_assert_r(shuffle(dataShuffler, child, rdata), false);
	}
	return true;
}

bool
AosDocidShufflerTester::shuffle(
		const AosDataShufflerPtr &dataShuffler, 
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr dc_conf = config->getFirstChild("data_cacher");
	AosDataCacherPtr dataCacher = generateData(dc_conf, rdata); 
	aos_assert_r(dataCacher, false);

	vector<AosDataCacherPtr> dataCachers;
	for (int i = 0; i < mNumServers; i++)
	{
		AosDataCacherPtr dc = AosDataCacher::getDataCacher(dc_conf, rdata);
		dataCachers.push_back(dc);
	}

	aos_assert_r(dataShuffler->shuffle(dataCacher, dataCachers, rdata), false); 
	aos_assert_r(verifyData2(dataCachers, rdata), false);
	return true;
}


bool
AosDocidShufflerTester::appendEntry(
		const AosDataShufflerPtr &dataShuffler,
		const AosRundataPtr &rdata)
{
	// shuffler type : docid
	//char *str = OmnNew char[sizeof(u64)];
	OmnString str;
	for (int i = 0; i < eMaxEntries; i++)
	{
		//*(u64 *)str = ++ mCrtId; 
		int len = sizeof(u64);
		str << ++ mCrtId;
		dataShuffler->appendEntry(str.data(), len, rdata);
		str  = "";
	}
	//OmnDelete [] str;

	vector<AosDataCacherPtr> dataCachers = dataShuffler->getDataCachers();
	aos_assert_r(verifyData2(dataCachers, rdata), false);
	aos_assert_rr(dataCachers.size() == (u32)mNumServers, rdata, false);
	return true;
}


bool
AosDocidShufflerTester::verifyData1(const AosRundataPtr &rdata)
{
	return false;
}

bool
AosDocidShufflerTester::verifyData2(
		const vector<AosDataCacherPtr> &dataCachers,
		const AosRundataPtr &rdata)
{
	for (u32 i = 0; i < dataCachers.size(); i++)
	{
		AosDataBlobPtr blob = dataCachers[i]->getBlob(); 
		char * record;
		int len;
		while ((record = blob->nextValue(len)))
		{
			aos_assert_r(verifyDocid(record, i, rdata), false);
		}
			
	}
	return true;
}


bool
AosDocidShufflerTester::verifyDocid(
		const char *record,
		const int index,
		const AosRundataPtr &rdata)
{
	OmnString kk(record, 4);
	u64 distid = atoll(kk.data());
	int idx = route(distid);
	aos_assert_r(index == idx, false);
	return true;
}


//------------------------------ Util-------------------------------
AosDataCacherPtr
AosDocidShufflerTester::generateData(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	// shuffler type : docid
	AosDataCacherPtr dc = AosDataCacher::getDataCacher(conf, rdata);
	char* str = OmnNew char[sizeof(u64)];
	for (int i = 0; i < eMaxEntries; i++)
	{
		//str << ++ mCrtId; 
		//int len = str.length();
		int len = sizeof(u64);
		*(u64*)str = ++ mCrtId;
		dc->appendEntry(str, len, rdata);	
	}
	OmnDelete []str;
	return dc;
}


int
AosDocidShufflerTester::route(const u64 &distid)
{
	int idx = -1;
	if (distid == 0)
	{
		idx = mCrtVirServerNum++ % mNumServers;
		if(mCrtVirServerNum >= mNumVirtuals)
		{
			mCrtVirServerNum = 0;
		}
	}
	else
	{
		idx = distid % mNumVirtuals % mNumServers;
	}
	return idx;
}

