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
// 06/15/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataBlob/Testers/BlobTester.h"

#include "API/AosApi.h"
#include "DataBlob/Ptrs.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/ActionObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/StrU64Array.h"
#include "Util/CompareFun.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "DataBlob/Testers/RecordTester.h"
#include "DataBlob/Testers/Ptrs.h"


AosBlobTester::AosBlobTester()
{
}


AosBlobTester::~AosBlobTester()
{
}


bool 
AosBlobTester::start()
{
	cout << "Start DataBlob Tester ..." << endl;
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	basicTest();
	return true;
}


bool 
AosBlobTester::basicTest()
{
	AosXmlTagPtr appconf = OmnApp::getAppConfig();
	aos_assert_r(appconf, false);
	
	AosXmlTagPtr conf = appconf->getFirstChild("testers");
	aos_assert_r(conf, false);

	AosRecordTesterPtr record = OmnNew AosRecordTester();
	aos_assert_r(record->run(mRundata), false);
	return true;
}


