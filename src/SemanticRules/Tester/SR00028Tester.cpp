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
// 
////////////////////////////////////////////////////////////////////////////
#include "SemanticRules/Tester/SR00028Tester.h"

#include "alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "SemanticRules/SemanticRule.h"
#include "SemanticRules/RuleCreator.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool AosSR00028Tester::start()
{
	cout << "    Start AosSR00028 Tester ...";
	basicTest();
	return true;
}


bool 
AosSR00028Tester::basicTest()
{
	OmnMutex lock("lock1");

cout << "+++++++++++++" << endl;
	lock.setSemanFlag(true);
	aos_create_rule(AosRuleId::eSR00028, &lock, "", "lock1");

	lock.lock();
	lock.unlock();

	return true;
}


