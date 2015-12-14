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
// Created By Ken Lee, 2014/09/11
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/Testers/JimoTableTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/Ptrs.h"
#include "IILMgr/IILStr.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"

static OmnString sgDftIILName = "_zt44_test";


AosJimoTableTester::AosJimoTableTester()
{
}


bool 
AosJimoTableTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
	OmnScreen << "    Start Tester ..." << endl;

	config();
	//singleAdd();
	batchAdd();
	return true;
}


bool
AosJimoTableTester::config()
{
	mIILName = sgDftIILName;
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("iil_testers");
	if (!tag) return true;

	mIILName = tag->getAttrStr("iilname", sgDftIILName);

	return true;
}


bool 
AosJimoTableTester::singleAdd()
{
	OmnScreen << "Now, begin to torturer!" << endl;

	bool rslt = getIIL();
	aos_assert_r(rslt, false);

	u64 t1 = OmnGetTimestamp();

	for (i64 i=0; i<10000000000LL; i++)
	{
		rslt = addEntries(i);
		aos_assert_r(rslt, false);

		if (i%10000 == 0) 
		{
			u64 t2 = OmnGetTimestamp();

			mIIL->saveToFileSafe(mRundata);

			u64 t3 = OmnGetTimestamp();

			AosIILMgr::getSelf()->returnIILPublic(mIIL, mRundata);

			u64 t4 = OmnGetTimestamp();

			AosIILType type = eAosIILType_JimoTable;
			AosIILObjPtr iil = AosIILMgr::getSelf()->loadIILPublic(mIILID, 100, 0, type, mRundata);
			aos_assert_r(iil, false);

			mIIL = dynamic_cast<AosJimoTable *>(iil.getPtr());

			u64 t5 = OmnGetTimestamp();

			OmnScreen << "for loop : " << i 
				<< ". Time: " << AosTimestampToHumanRead(t5 - t1)
				<< ". include:"
				<< ", add:" << AosTimestampToHumanRead(t2 - t1)
				<< ", save:" << AosTimestampToHumanRead(t3 - t2)
				<< ", return:" << AosTimestampToHumanRead(t4 - t3)
				<< ", load:" << AosTimestampToHumanRead(t5 - t4)
				<< endl;
			t1 = t5;
		}
	}

	return true;
}


OmnString
AosJimoTableTester::getRandomStr(const i64 &idx)
{
	//OmnString str = "";
	//for (u32 i=0; i<length; i++)
	//{
	//	str << (char)(random()%26 + 'a');	
	//}
	OmnString str = "iiltest";
	str << "_" << idx;

	return str;
}


bool
AosJimoTableTester::getIIL()
{
	OmnString str;
	str << "<zky_buffarray zky_stable=\"false\">"
		<< "<CompareFun cmpfun_alphabetic=\"false\" cmpfun_reserve=\"false\" cmpfun_type=\"varstr\" cmpfun_size=\"4\" />"
		<< "</zky_buffarray>";
	                                            
	mBAVConfig = AosXmlParser::parse(str AosMemoryCheckerArgs);
	AosIILObjPtr iil = AosIILMgr::getSelf()->createJimoTable(
		mIILName, mBAVConfig, mRundata);
	aos_assert_r(iil, false);

	mIILID = iil->getIILID();
	mIIL = dynamic_cast<AosJimoTable *>(iil.getPtr());
	return true;
}


bool
AosJimoTableTester::addEntries(const i64 &idx)
{
	OmnString str = getRandomStr(idx);
	bool rslt = mIIL->addDocSafe(str, true, mRundata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosJimoTableTester::batchAdd()
{
	OmnScreen << "Now, begin to torturer!" << endl;

	bool rslt = getIIL();
	aos_assert_r(rslt, false);

	u64 t1 = OmnGetTimestamp();

	for (i64 i=0; i<10000LL; i++)
	{
		rslt = addBuffArrayVar();
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosJimoTableTester::addBuffArrayVar()
{
	AosBuffArrayVarPtr array = createBuffArrayVar();
	aos_assert_r(array, false);

	bool rslt = mIIL->batchAddSafe(array, mRundata);
	aos_assert_r(rslt, false);

	return true;
}


AosBuffArrayVarPtr
AosJimoTableTester::createBuffArrayVar()
{
	AosBuffArrayVarPtr array = AosBuffArrayVar::create(mBAVConfig, mRundata);
	aos_assert_r(array, 0);

	static i64 mm = 0;
	i64 num = rand()%1000000 + 1000000;

	int ii;
	bool rslt;
	OmnString str;
	for (i64 i=0; i<num; i++)
	{
		mm++;
		str = getRandomStr(mm);

		rslt = array->appendEntry(ii, str.data(), str.length());
		aos_assert_r(rslt, 0);
	}

	rslt = array->sort();
	aos_assert_r(rslt, 0);

	return array;
}

