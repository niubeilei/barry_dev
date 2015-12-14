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
// 2010/10/24	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/Tester/TransBucketTester.h"

#include "alarm_c/alarm.h"
#include "IDTransMap/TransBucket.h"
#include "IDTransMap/TransBktMgr.h"
#include "IDTransMap/IDTransMap.h"
#include "IDTransMap/Ptrs.h"
#include "IILTrans/HitAddDocByIdTrans.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Time.h"

//AosTransBucketPtr  sgTransBkt; 
AosTransBktMgr* sgTransBkt = NULL;

int sgNumBucketEntries = 100;
int sgNumBuckets = 10;
int sgStartId = 0;
vector<AosIDTransVectorPtr> sgIdVector(sgNumBucketEntries * sgNumBuckets);

AosTransBucketTester::AosTransBucketTester()
{
	mName = "TransBucketTester";
	
}


bool 
AosTransBucketTester::start()
{
	cout << "Start TransBucket Tester ..." << endl;

	//OmnFilePtr file = OmnNew OmnFile("test.data", OmnFile::eCreate);
	//AosRundataPtr rdata;
	//AosTransBucket::config(sgNumBucketEntries, sgNumBuckets, 0xfff0, 0x400); //64k, 1k
	//sgTransBkt = OmnNew AosTransBucket(sgStartId, file, rdata);
	 
	sgTransBkt = OmnNew AosTransBktMgr();
	OmnString configstr = "<a><transbktmgr iils_per_bucket='";
	configstr << sgNumBucketEntries << "' ";
	configstr << "max_buckets='" << sgNumBuckets << "'/></a>";
	AosXmlParser parser;
	AosXmlTagPtr config = parser.parse(configstr, "" AosMemoryCheckerArgs);
	sgTransBkt->config(config);

	basicTest();
	return true;
}


bool 
AosTransBucketTester::basicTest()
{

	int tries = 1000000000;
	for (int i=0; i<tries; i++)
	{
		if ((i%100) == 0)
			OmnScreen << "Tring num: " << i << endl;
		doOneTest();
	}
	return true;
}


bool
AosTransBucketTester::doOneTest()
{
	AosRundataPtr rdata;
	int num = rand()%10;
	AosIDTransVectorPtr transes = OmnNew AosIDTransVector();
	u64 id = rand()%(sgNumBuckets*sgNumBucketEntries);
	if (rand()%2)
	{
		for (int i=0; i<num; i++)
		{
			AosIILTransPtr trans = OmnNew AosIILTransHitAddDocById(1, i, false, 1, rdata);
			transes->push_back(trans);
		}

		sgTransBkt->addTrans(id, transes, rdata);
		if (sgIdVector[id])
		{
			sgIdVector[id]->insert(sgIdVector[id]->end(), transes->begin(), transes->end());
		}
		else
		{
			sgIdVector[id] = transes;
		}
	}
	else
	{
		sgTransBkt->procTrans(id, transes, rdata);
		if(!sgIdVector[id])
		{
			if (!transes->empty())
			{
				OmnAlarm << "invalid trans:" << (int)transes->size() << enderr;
			}
		}
		else
		{
			if (transes->size() != sgIdVector[id]->size())
			{
				OmnAlarm << "invalid trans:" << (int)transes->size() << ","  <<
						 (int)sgIdVector[id]->size() << enderr;
			}
			sgIdVector[id] = NULL;
		}
	}
	return true;
}
