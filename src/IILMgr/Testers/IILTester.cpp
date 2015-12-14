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
// 08/09/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#include "IILMgr/Testers/IILTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "IILMgr/IILMgr.h"
#include "IILMgr/Ptrs.h"
#include "IILMgr/IILStr.h"
#include "IILTrans/AllIILTrans.h"
#include "SEUtil/IILIdx.h"
#include "Random/RandomBuffArray.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/IILMgrObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"

static char *sgStrings[AosIILTester::eNumStrings];
static OmnString sgDftIILName = "__zt44_test";
static u64 sgDocid = 10000;

AosIILTester::AosIILTester()
{
}


bool 
AosIILTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	//mRundata->setSiteid(AosGetDftSiteId());
	mRundata->setSiteid(100);
	OmnScreen << "    Start Tester ..." << endl;

	config();
	init();
	grandTorturer();
	return true;
}


bool
AosIILTester::init()
{
	return true;
}


bool
AosIILTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("iil_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	
	return true;
}


bool 
AosIILTester::basicTest()
{
	return true;
}


bool 
AosIILTester::grandTorturer()
{
OmnScreen << "Now, begin to torturer!" << endl;
	// 1. random illname
	vector<OmnString> iil_names;
	vector<u64> iil_ids;
	u32 num_iils = 1;
	for (u32 i=0; i<num_iils; i++)
	{
		u32 length = random()%5 + 5;
		OmnString tmpIILName = getRandomStr(length);
		u64 tmpIILId = AosIILMgr::getSelf()->getIILID(tmpIILName, true, mRundata);
		aos_assert_r(tmpIILId > 0, false);
		iil_names.push_back(tmpIILName);
		iil_ids.push_back(tmpIILId);
	}
	
	// 2. append entry to ill
	u32 group = 0;
	u64 start_time = OmnGetTimestamp();
	for (u32 j=0; j<10000000; j++)
	{
		u32 pos = random()%iil_names.size();
		AosIILPtr iil = getIIL(iil_names[pos], iil_ids[pos]);	
		aos_assert_r(iil, false);
		bool rslt = addEntries(iil);
		aos_assert_r(rslt, false);
		if (++group%10000 == 0) 
		{
			group = 0;
			OmnScreen << "for loop : " << j 
				<< ". Time: " << OmnGetTimestamp() - start_time << endl;
			iil->saveToFileSafe(mRundata);
		}
		AosIILMgr::getSelf()->returnIILPublic(iil, mRundata);
	}

	return true;
}


OmnString
AosIILTester::getRandomStr(u32 length)
{
	OmnString str = "";
	for (u32 i=0; i<length; i++)
	{
		str << (char)(random()%26 + 'a');	
	}

	return str;
}


AosIILPtr
AosIILTester::getIIL(const OmnString &iilname, const u64 &iilid)
{
	//u64 iilid = AosIILMgr::getSelf()->getIILID(iilname, true, mRundata);
	//aos_assert_r(iilid > 0, 0);

	OmnString value = "test1";
	u64 docid = 10000;
	AosIILTransPtr trans;// = OmnNew AosIILTransStrAddValueDocByName(
	//	iilname, false, value, docid, false, false, true, false);
	
	AosIILPtr iil;// = AosIILMgr::getSelf()->getIILPublic(
	//	iilid, 100, trans, mRundata);
	//aos_assert_r(iil, NULL);

	return iil;
}


bool
AosIILTester::addEntries(AosIILPtr &iil)
{
	//while (1)
	//{
	//	pick an iil;
	//	OmnString str;
	//	u64 docid;
	//	iil->addDocSafe(str, docid, mRundata);
	//}
	
	OmnString str = getRandomStr(5);
	u64 docid = random()%100000000;
	aos_assert_r(iil, false);
	AosIILStr *iilstr = dynamic_cast<AosIILStr *>(iil.getPtr()); 
	iilstr->addDocSafe(str, docid, false, false, mRundata);
	return true;
}


