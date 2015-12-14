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
// 01/01/2013 Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/U64CacherTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/U64Cacher.h"
#include "Util1/Time.h"



bool AosU64CacherTester::start()
{
	cout << "    Start U64Cacher Tester ...";

	basicTest();
	return true;
}


bool
AosU64CacherTester::basicTest()
{
	OmnScreen << "To test cacher" << endl;
	try
	{
		AosU64Cacher<u64, u64_hash, u64_cmp> cacher(10);
		for (int i=0; i<15; i++)
		{
			cacher.push_back(0, (u64)i);
			aos_assert_r(cacher.size() == (u64)i+1, false);
		}
	}

	catch (...)
	{
		OmnAlarm << "failed creating cacher" << enderr;
		return false;
	}
	return true;
}

#endif
