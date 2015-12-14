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
// 03/22/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ImportData/Testers/ImportDataTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"


AosImportDataTester::AosImportDataTester()
{
	mName = "ImportDataTester";
}


bool 
AosImportDataTester::start()
{
	cout << "Start ImportData Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosImportDataTester::basicTest()
{
	// It converts the data defined in config
	AosXmlTagPtr config = OmnApp::getAppConfig();
	aos_assert_r(config, false);

	AosXmlTagPtr tag = config->getFirstChild("raw_data");
	aos_assert_r(tag, false);

	return true;
}


