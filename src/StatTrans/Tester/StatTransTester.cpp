////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 12/07/2014 Created by Phil 
////////////////////////////////////////////////////////////////////////////
#include "StatTrans/Tester/StatTransTester.h"

#include "StatTrans/BatchGetStatDocsTrans.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"
#include <vector>

extern AosXmlTagPtr testConf;

AosStatTransTester::AosStatTransTester()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin); 
	mTransConf = testConf;
}

AosStatTransTester::~AosStatTransTester()
{
}

bool
AosStatTransTester::start()
{
	AosXmlTagPtr doc;

	//mTransConf = OmnApp::getAppConfig();
	aos_assert_r(mTransConf, false);

	doc = mTransConf->getFirstChild("trans");
	aos_assert_r(doc, false);

	mTransName = doc->getNodeText();
	if (mTransName == "BatchGetStatDocsTrans")
		return testGetStatDocsTrans(mTransConf);
	
	//other trans test, just add more if .....
	
	return true;
}

bool 
AosStatTransTester::testGetStatDocsTrans(AosXmlTagPtr &conf)
{
	OmnTagFuncInfo << "start to test batchGetStatDocsTrans. " << endl;

	//get the logic pid firstly
	bool timeout = false;
	AosBuffPtr resp; 
	bool rslt;
	AosTransPtr trans;
	u32 tries = 5;
	AosXmlTagPtr doc, doc1;
	AosXmlTagPtr cubeConf;
	AosXmlTagPtr qryConf;
	AosXmlTagPtr docidConf;
	vector<u64>	docids;
	AosAsyncRespCallerPtr respCaller;
	int cubeId;
	int reqId = 10;

	cubeConf = conf->getFirstChild("cube");
	aos_assert_r(cubeConf, false);

	qryConf = conf->getFirstChild("stat_qry");
	aos_assert_r(qryConf, false);

	docidConf = conf->getFirstChild("docid");
	aos_assert_r(docidConf, false);

	//generate docids, it could be a long list
	docids.clear();
	doc = docidConf->getFirstChild("docid_list");
	OmnString str;
	u64 start, end, random;
	while (doc)
	{
		start = end = random = 0;

		doc1 = doc->getFirstChild("start");
		aos_assert_r(doc1, false);
		str = doc1->getNodeText();
		start = str.toU64(0);
		
		doc1 = doc->getFirstChild("end");
		aos_assert_r(doc1, false);
		str = doc1->getNodeText();
		end = str.toInt();
		aos_assert_r(end >= start, false);
		
		doc1 = doc->getFirstChild("random_select");
		if (doc1)
		{
			str = doc1->getNodeText();
			random = str.toU64(0);
			aos_assert_r(random <= (end - start + 1), false);
		}

		//not handling random for now. Just reset random to zero
		//Will add random handling later on
		random = 0;
		if (random > 0)
		{
			//randomly select docids between start and end
			//todo later
		}
		else
		{
			//add docids to the list
			for (u64 i = start; i <= end; i++)
				docids.push_back(i);
		}
		
		doc = docidConf->getNextChild("docid_list");
	}
	
	//sometimes we need to wait for downstream tasks started
	cubeId = cubeConf->getAttrInt("cube_id", 0);
	trans = OmnNew AosBatchGetStatDocsTrans(cubeId, cubeConf,
			docids, qryConf, reqId, respCaller);
	rslt = AosSendTrans(mRundata, trans, timeout, resp);
	if (timeout)
	{
		//AosSetErrorU(rdata, "Faild to add the trans, timeout");
		//OmnAlarm << rdata->getErrmsg() << enderr;
		//return false;
		OmnScreen << "sendtrans timeout! " << endl;
		timeout = false;
	}
	else
	{
		//the result buffer is in resp
		OmnScreen << "batchGetDocs trans returned buffer with size: "
			<< resp->dataLen() << endl;
	}

	return rslt;
}
