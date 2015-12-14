////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrUtilTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/StrUtilTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/StringUtil.h"



bool AosStrUtilTester::start()
{
	// 
	// Test default constructor
	//
	binIncTest();
	return true;
}


bool AosStrUtilTester::binIncTest()
{
	OmnBeginTest << "Test AosStrUtil";
	mTcNameRoot = "AosStrUtil_binInc";

	int len = 3;
	unsigned char str[3];
	memset(str, 0, len);

	int ret;
	u32 value = 0;
	for (u32 i=0; i<=0xffffff; i++)
	// for (u32 i=0; i<=2; i++)
	{
		ret = AosStrUtil_binInc((uint8*)str, len);
		value++;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(
			(value & 0xff) == str[2] &&
			((value >> 8) & 0xff) == str[1] &&
			((value >> 16) & 0xff) == str[0]))
			<< endtc;
	}

	return true;
}


