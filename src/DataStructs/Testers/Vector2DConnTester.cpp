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
// How to torture: 
//
// Modification History:
// 2014/08/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStructs/Testers/Vector2DConnTester.h"

#include "API/AosApi.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/Ptrs.h"
#include "Util/ValueRslt.h"
#include "Util/DataTypes.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "SEInterfaces/TaskObj.h"

#include "DataStructs/Vector2D.h"
#include "DataStructs/Ptrs.h"
#include "DataStructs/StatIdIDGen.h"
#include "DataStructs/StatIdExtIDGen.h"


AosVector2DConnTester::AosVector2DConnTester()
{
}


AosVector2DConnTester::~AosVector2DConnTester()
{
}


bool 
AosVector2DConnTester::start()
{
	cout << "Start Vector2DConn Tester ..." << endl;
	config();
	basicTest();
	return true;
}


bool
AosVector2DConnTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("testers");
	if (!tag) return true;

	return true;
}


bool 
AosVector2DConnTester::basicTest()
{
	return performanceTest();
}


bool 
AosVector2DConnTester::performanceTest()
{
	u32 **data;
	const int array_size = 100000;
	data = new u32 *[array_size];
	for (u32 i=0; i<array_size; i++)
	{
		data[i] = new u32[array_size];
		memset(data[i], 0, sizeof(u32)*array_size);
	}

	int tries = mTries;
	bool rslt;
	while (tries--)
	{
		rslt = addEntries(); aos_assert_r(rslt, false);
		rslt = verifyResults(); aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosVector2DConnTester::addEntries()
{
	int num = OmnRandom::intByRange(
				1, 1, 10,
				2, 10, 20,
				11, 100, 50,
				101, 100000, 100,
				100001, 1000*1000, 20);

	int time_id = rand() / mMa
	for (int i=0; i<num; i++)
	{
	}
}

