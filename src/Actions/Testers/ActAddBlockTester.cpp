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
// 1. Create a table of records randomly.
// 2. Determine one column as the 'driving' column. This is the matched key.
// 3. Separate one or more columns into one IIL. 
//   
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Actions/Testers/ActAddBlockTester.h"

#include "API/AosApi.h"
#include "Actions/Ptrs.h"
#include "Actions/SdocAction.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "Database/DbTable.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "IILClient/Ptrs.h"
#include "IILClient/IILClient.h"
#include "IILAssembler/IILAssembler.h"
#include "Job/Job.h"
#include "QueryUtil/QueryContext.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "TaskMgr/TaskAction.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"
#include "Util/ValueRslt.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"
#include "Porting/Sleep.h"

#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


AosActAddBlockTester::AosActAddBlockTester()
:
mRundata(OmnNew AosRundata(AosMemoryCheckerArgsBegin))
{
	mName = "ActAddBlockTester";
}


AosActAddBlockTester::~AosActAddBlockTester()
{
}


bool 
AosActAddBlockTester::start()
{
	cout << "Start Actions Tester ..." << endl;
	basicTest();
	return true;
}


bool 
AosActAddBlockTester::basicTest()
{
	aos_assert_r(config(), false);
	aos_assert_r(createData(), false);
	return true;
}


bool
AosActAddBlockTester::config()
{
	mRundata->setSiteid(100);
	AosXmlTagPtr app_conf = OmnApp::getAppConfig();
	aos_assert_r(app_conf, false);
	AosXmlTagPtr conf = app_conf->getFirstChild("testers");
	aos_assert_r(conf, false);

	// Create table1 IIL assember
	AosXmlTagPtr table_def = conf->getFirstChild("table");
	aos_assert_r(table_def, false);
	mTableIILAssembler = AosIILAssembler::createIILAssembler(0, table_def, mRundata);
	aos_assert_r(mTableIILAssembler, false);
	
	return true;
}	


bool
AosActAddBlockTester::createData()
{
	u64 start = clock();
	mTableIILAssembler->sendStart(mRundata);
	for (u64 i=0; i<10000000; i++)
	{
		OmnString key = OmnRandom::letterStr(10, 10);
		aos_assert_r(key != "", false);
		u64 docid = rand();
		AosValueRslt value_rslt;
		value_rslt.setKeyValue(key, false, docid);
		mTableIILAssembler->appendEntry(value_rslt, mRundata);
	}
	mTableIILAssembler->sendFinish(mRundata);
	u64 finish = clock();

	cout << "------------------------------------------------------" << endl;
	cout << "finished all need: " << (finish-start)/CLOCKS_PER_SEC << endl;
	cout << "------------------------------------------------------" << endl;
	return true;
}


