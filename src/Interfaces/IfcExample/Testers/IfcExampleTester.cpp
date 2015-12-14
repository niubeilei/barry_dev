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
// 2013/06/02: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "API/AosApi.h"
#include "Interfaces/IfcExample/Testers/IfcExampleTest.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


bool AosIfcExampleTester()
{
	// Run the DLL
	AosRundataPtr rdata = OmnApp::getRundata();
	rdata->setSiteid(100);

	OmnString ss = "<worker name=\"Chen Ding\" "
		"zky_method=\"next_value\" "
		"type=\"random_str\" "
		"prefix_charset=\"97:108:100\" "
		"postfix_charset=\"65:76:100\" "
		"body_charset=\"48:57:100\" "
		"prefix_length=\"5:5:100\" "
		"postfix_length=\"2:2:100\" "
		"total_length=\"10:10:100\" "
		"/>";
	AosXmlTagPtr worker_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(worker_doc, false);

	ss = "<jimo zky_libname=\"libJimoRandomStrGen.so\" "
		"zky_classname=\"AosJimoRandomStrGen\"/>";
	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata, ss AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, false);

	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	aos_assert_r(jimo, false);
	OmnScreen << "Jimo created" << endl;

	int tries = 1000000;
	u64 time1 = OmnGetTimestamp();
	u64 last_time = time1;
	for (int i=0; i<tries; i++)
	{
		if (i % 100000 == 0 && i)  
		{
			u64 time2 = OmnGetTimestamp();
			cout << "Finished: " << i << ". Time: " << time2 - last_time << endl;
			last_time = time2;
		}
		jimo->run(rdata, worker_doc);
	}

	u64 time3 = OmnGetTimestamp();
	cout << "Total finished: " << time3 - time1 << endl;

	// Testing the static case
	time1 = OmnGetTimestamp();
	last_time = time1;
	AosJimoRandomStrGenTest* jj = new AosJimoRandomStrGenTest(rdata, worker_doc, jimo_doc);
	for (int i=0; i<tries; i++)
	{
		if (i % 100000 == 0 && i)  
		{
			u64 time2 = OmnGetTimestamp();
			cout << "Finished: " << i << ". Time: " << time2 - last_time << endl;
			last_time = time2;
		}
		AosJimoRandomStrGenTest_nextValue(rdata, jj, worker_doc);
	}
	time3 = OmnGetTimestamp();
	cout << "Total finished: " << time3 - time1 << endl;


	time1 = OmnGetTimestamp();
	last_time = time1;
	AosJimoRandomStrGenTest *jm = (AosJimoRandomStrGenTest*)jj;
	for (int i=0; i<tries; i++)
	{
		if (i % 100000 == 0 && i)  
		{
			u64 time2 = OmnGetTimestamp();
			cout << "Finished: " << i << ". Time: " << time2 - last_time << endl;
			last_time = time2;
		}

		size_t loop = 10;
		u32 prefixLen = jm->randomData(jm->mPrefixLenList);
		u32 postfixLen = jm->randomData(jm->mPostfixLenList);
		u32 totalLen = jm->randomData(jm->mTotalLenList);
		while((prefixLen + postfixLen) > totalLen && loop)
		{
			prefixLen = jm->randomData(jm->mPrefixLenList);
			postfixLen = jm->randomData(jm->mPostfixLenList);
			loop--;
		}
		u32 bodyLen = totalLen - postfixLen - prefixLen;
	
		string prefix = "";
		jm->createStr(prefix, prefixLen, jm->mPrefixList);
		string body = "";
		jm->createStr(body, bodyLen, jm->mBodyList);
		string postfix = "";
		jm->createStr(postfix, postfixLen, jm->mPostfixList);
	
		jm->mNewStr << prefix << body << postfix; 
		aos_assert_r(jm->mNewStr != "", false);
	}
	time3 = OmnGetTimestamp();
	cout << "Total finished: " << time3 - time1 << endl;

	return true;
}

