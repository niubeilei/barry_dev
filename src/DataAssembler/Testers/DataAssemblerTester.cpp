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
// 06/12/2012 Created by Xu Lei
////////////////////////////////////////////////////////////////////////////
#include "DataAssembler/Testers/DataAssemblerTester.h"

#include "API/AosApi.h"
#include "DataAssembler/Ptrs.h"
#include "DataAssembler/AssemblerNorm.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosDataAssemblerTester::AosDataAssemblerTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
}


bool 
AosDataAssemblerTester::start()
{
	cout << "Start DataAssembler Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosDataAssemblerTester::basicTest()
{
	AosAssemblerNorm assembler;

	OmnScreen << "Hello World!" << endl;
	return true;

	// for (int i=0; i<1000; i++)
	// {
	//   
	// 	assembler.appendEntry("aaa", i, mRundata);
	// }
}

