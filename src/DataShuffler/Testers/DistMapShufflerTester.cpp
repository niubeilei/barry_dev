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
#include "DataShuffler/Testers/DistMapShufflerTester.h"

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
#include "Util/BuffArray.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"


AosDistMapShufflerTester::AosDistMapShufflerTester()
:mCrtId(0)
{
}


bool 
AosDistMapShufflerTester::start()
{
	cout << "    Start AosDataShuffler Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDistMapShufflerTester::basicTest()
{
	int tries = mTestMgr->getTries();	
	if (tries < 0) tries = 10000;
	AosRundataPtr rdata = OmnApp::getRundata();
	AosXmlTagPtr config = OmnApp::getAppConfig();
	config = config->getFirstChild("dist_map");
	for (int i = 0; i <tries; i++)
	{
		test(config, rdata);	
	}
	return true;
}


bool
AosDistMapShufflerTester::test(
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	aos_assert_r(createMap(rdata), false);
	if (rand() % 2 == 0)
	{
		AosXmlTagPtr child = config->getFirstChild("append_entry");
		AosXmlTagPtr dsconf = child->getFirstChild("data_shuffler");
		AosDataShufflerPtr dataShuffler = AosDataShuffler::getDataShuffler(dsconf, rdata);
		aos_assert_r(dataShuffler, false);
		aos_assert_r(appendEntry(dataShuffler, rdata), false);
	}
	else
	{
		AosXmlTagPtr child = config->getFirstChild("shuffle");
		AosXmlTagPtr dsconf = child->getFirstChild("data_shuffler");
		AosDataShufflerPtr dataShuffler = AosDataShuffler::getDataShuffler(dsconf, rdata);
		aos_assert_r(dataShuffler, false);
		aos_assert_r(shuffle(dataShuffler, child, rdata), false);
	}
	return true;
}

bool
AosDistMapShufflerTester::shuffle(
		const AosDataShufflerPtr &dataShuffler, 
		const AosXmlTagPtr &config, 
		const AosRundataPtr &rdata)
{
	AosXmlTagPtr dc_conf = config->getFirstChild("data_cacher");
	AosDataCacherPtr dataCacher = generateData(dc_conf, rdata); 
	aos_assert_r(dataCacher, false);

	int num_servers = config->getAttrInt("num_servers", -1);
	aos_assert_r(num_servers > 0, false);

	vector<AosDataCacherPtr> dataCachers;
	for (int i = 0; i < num_servers; i++)
	{
		AosDataCacherPtr dc = AosDataCacher::getDataCacher(dc_conf, rdata);
		dataCachers.push_back(dc);
	}

	dataShuffler->updateDistMap(mMap);
	aos_assert_r(dataShuffler->shuffle(dataCacher, dataCachers, rdata), false); 
	aos_assert_r(verifyData2(dataCachers, rdata), false);
	return true;
}


bool
AosDistMapShufflerTester::appendEntry(
		const AosDataShufflerPtr &dataShuffler,
		const AosRundataPtr &rdata)
{
	// shuffler type : docid
	dataShuffler->updateDistMap(mMap);
	OmnString str;
	for (int i = 0; i < eMaxEntries; i++)
	{
		str = generateEntryStr(); 
		int len = str.length();
		dataShuffler->appendEntry(str.data(), len, rdata);
	}

	vector<AosDataCacherPtr> dataCachers = dataShuffler->getDataCachers();
	aos_assert_r(verifyData2(dataCachers, rdata), false);
	return true;
}


bool
AosDistMapShufflerTester::verifyData1(const AosRundataPtr &rdata)
{
	return false;
}

bool
AosDistMapShufflerTester::verifyData2(
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
			aos_assert_r(verifyResult(record, i, rdata), false);
		}
			
	}
	return false;
}


bool
AosDistMapShufflerTester::verifyResult(
		const char *record,
		const int index,
		const AosRundataPtr &rdata)
{
	OmnString kk(&record[6], 5);
	int idx = route(kk);
	aos_assert_r(index == idx, false);
	return false;
}


//------------------------------ Util-------------------------------
AosDataCacherPtr
AosDistMapShufflerTester::generateData(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	// shuffler type : docid
	AosDataCacherPtr dc = AosDataCacher::getDataCacher(conf, rdata);
	OmnString str;
	for (int i = 0; i < eMaxEntries; i++)
	{
		str = generateEntryStr(); 
		int len = str.length();
		dc->appendEntry(str.data(), len, rdata);
	}
	return dc;
}


int
AosDistMapShufflerTester::route(const OmnString &key)
{
	return comp(key);
}


bool
AosDistMapShufflerTester::createMap(const AosRundataPtr &rdata)
{
//AosXmlTagPtr config = OmnApp::getAppConfig();
	int strLen = 100;
	OmnString array[11];
	AosCompareFunPtr cmp = OmnNew AosFunStrU641(strLen + sizeof(u64));
	mMap = OmnNew AosBuffArray(cmp, false);
//mMap = OmnNew AosBuffArray(config, rdata);
	for (int i = 0; i<10; i++)
	{
//		OmnString key = OmnRandom::letterStr(6, 6);
		OmnString key;
		OmnString c = OmnRandom::capitalStr(1, 1);
		bool found = false;
		int j = 0;
		while (j < i) {if (c == array[j++]) found = true;}
		if (!found)
		{
			OmnString ss;
			for (int k = 0; k < 5; k++) ss << c;	
			mArray.push_back(ss);
			key << ss;
OmnScreen << "kkkkkkk: " << key << endl;
			char *buff = mMap->addValue(key.data(), key.length(), rdata);
			buff[key.length()] = 0;
			array[i] = c;
		}
	}
OmnScreen << "=======================" << endl;
	mMap->sort();
	u32 entries = mMap->getNumEntries();
	int record_len;
	for (u32 i = 0; i< entries; i++)
	{
		char *data = mMap->getRecord(i, record_len);
//		OmnString key (&data[6], 5);
		OmnString key (data);
OmnScreen << "index: " << i << ", key : " << key << endl;
	}
	return true;
}


OmnString
AosDistMapShufflerTester::generateEntryStr()
{
	OmnString str = OmnRandom::letterStr(6, 6);	
	str << OmnRandom::capitalStr(5, 5);
	return str;
}


int
AosDistMapShufflerTester::comp(const OmnString key)
{
	for (u32 i = 0; i< mArray.size(); i++)
	{
		int result = strcmp(key.data(), mArray[i].data());
		if (result >= 0)
		{
			return i;
		}
	}
	return mArray.size() -1;
}

/*
int
AosDistMapShufflerTester::comp1(const OmnString key)
{
	int low = 0;
	ing high = mArray.size()-1;
	int half = (mArray.size()-1)/2;
	OmnString mid = mArray[half];
	while(low <= high)
	{
		int reslt = strcmp(key.data(), mid)
		if (reslt == 0) return half;
		if (reslt > 0)
		{
			low = half +1;
		}

		if (reslt < 0)
		{
			high = half -1;
		}
		half = low + (high - low) /2;
		mid = mArray[half];
	}
	return half;
}
*/

