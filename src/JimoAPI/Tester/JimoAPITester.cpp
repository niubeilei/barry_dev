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
// 2014/11/24 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoAPI/Tester/JimoAPITester.h"

#include "Debug/Debug.h"
#include "JimoAPI/Jimo.h"
#include "Rundata/Rundata.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <set>
#include <vector>
using namespace std;

AosJimoAPITester::AosJimoAPITester()
{
	mName = "JimoAPITester";
	mTries = 100;
}

bool 
AosJimoAPITester::start()
{
	basicTest();
	return true;
}


bool
AosJimoAPITester::basicTest()
{
	bool rslt = Jimo::helloJimo();
	aos_assert_r(rslt, false);
	return true;
}

