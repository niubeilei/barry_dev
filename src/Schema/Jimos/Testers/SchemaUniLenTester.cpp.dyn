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
// 2013/10/31:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/Jimos/Testers/SchemaUniLenTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <map>

#include "API/AosApiS.h" 


AosSchemaUniLenTester::AosSchemaUniLenTester() 
{
}


bool AosSchemaUniLenTester::start()
{
	// Test default constructor
	cout << "    Start SchemaUniLen Tester..." << endl;
	basic();
	return true;
}


bool 
AosSchemaUniLenTester::basic()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	OmnString jimo_str = "<jimo zky_objid=\"chen_test001\" ";
	jimo_str << "zky_classname=\"AosSchemaUniLength\" "
		<< "zky_otype=\"zkyotp_jimo\" "
		<< "current_version=\"1.0\">"
		<< "<versions>"
		<< "<ver_0>libSchemaJimos.so</ver_0>"
		<< "</versions>"
		<< "</jimo>";
	AosXmlTagPtr jimo_doc = AosStr2Xml(rdata, jimo_str AosMemoryCheckerArgs);
	aos_assert_r(jimo_doc, false);

	OmnString worker_str = "<worker />";
	AosXmlTagPtr worker_doc = AosStr2Xml(rdata, worker_str AosMemoryCheckerArgs);
	aos_assert_r(worker_doc, false);

	AosJimoPtr jimo = AosCreateJimo(rdata, worker_doc, jimo_doc);
	aos_assert_r(jimo, false);

	jimo->run(rdata);

	return true;
}


bool    
AosSchemaUniLenTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1);
	}
	return true;
}


bool    
AosSchemaUniLenTester::signal(const int threadLogicId)
{
	return true;
}


