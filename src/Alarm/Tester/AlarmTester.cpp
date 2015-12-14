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
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Alarm/Tester/AlarmTester.h"

#include "alarm_c/alarm.h"
#include "ActOpr/ValueRslt.h"
#include "Debug/Debug.h"
#include "Valueset/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/Test.h"
#include "Random/RandomUtil.h"
#include "SEUtil/ValueDefs.h"
#include "XmlUtil/SeXmlParser.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "Util1/Time.h"
#include "AppMgr/App.h"
#include "Alarm/Alarm.h"


AosAlarmTester::AosAlarmTester()
{
}


bool AosAlarmTester::start()
{
	cout << "   Start AosAlarmTester ..." << endl;
	basicTest();
	return true;
}


bool AosAlarmTester::basicTest()
{
	int tries = mTestMgr->getTries();
	for (int i=0; i<tries; i++)
	{
		OmnAlarm <<"i: " << i << enderr; 
	}
	return true;
}
