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
// 01/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conditions/Tester/CondTester.h"

#include "alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Conditions/Condition.h"
#include "Conditions/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "XmlLib/tinyxml.h"


bool AosCondTester::start()
{
	cout << "    Start AosCond Tester ...";
	u32 tries = 1000;
	basicTest(tries);
	return true;
}


bool AosCondTester::basicTest(const u32 tries)
{
	std::string conf = "<CondEqual>"
						"<CondDataConst>"
							"<ValueType>int32_t</ValueType>"
							"<ByteString>00 00 00 a1</ByteString>"
						"</CondDataConst>"
						"<CondDataConst>"
							"<ValueType>int32_t</ValueType>"
							"<ByteString>00 00 00 a1</ByteString>"
						"</CondDataConst>"
					   "</CondEqual>";
	TiXmlElement node(conf);

	AosConditionPtr cond = AosCondition::factory(&node);
	return true;
}

