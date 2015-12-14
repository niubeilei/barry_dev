////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringTester.cpp
// Description:
//   
//
// Modification History:
// 08/01/2009:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Proggie/TaggedData/Testers/TaggedData.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Proggie/TaggedData/TaggedData.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool AosTaggedDataTester::start()
{
	cout << "    Start ...";
	return basicTest();
	return true;
}


bool AosTaggedDataTester::basicTest()
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test Tagged Data";
	mTcNameRoot = "TaggedData";

	AosTaggedData td;
	OmnString data;
	OmnTrace << "To get data: " << endl;
	AosXmlRc errcode;
	OmnString errmsg;
	u32 num = td.getData("image|$|796", "dataid, creator", 0, 5, 
		"", "order by dataid", true, data, errcode, errmsg);
	OmnCreateTc << (num > 0) << endtc;
	OmnCreateTc << (data.length() > 0) << endtc;
	cout << "Retrieved data: " << data << endl;
	return true;
}
		
