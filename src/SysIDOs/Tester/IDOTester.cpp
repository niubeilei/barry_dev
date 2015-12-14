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
// 2015/04/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysIDOs/Tester/IDOTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IDOJimoProgObj.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
AosIDOTester::start()
{
	cout << "    Start IDO Tester ...";

	AosRundata rdata;
	AosJimoPtr jimo = AosCreateJimoByClassname(&rdata, "AosIDOJimoProg", 1);
	aos_assert_r(jimo, false);
	AosIDOJimoProgObjPtr jimo_prog = dynamic_cast<AosIDOJimoProgObj*>(jimo.getPtr());
	aos_assert_r(jimo_prog, false);
	bool rslt = jimo_prog->setStr(&rdata, "ObjName", "123");
	aos_assert_r(rslt, false);
	return true;
}


