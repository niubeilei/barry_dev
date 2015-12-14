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
// 04/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/BinarySearchTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Random/random_util.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util/BinarySearch.h"



bool AosBinarySearchTester::start()
{
	cout << "    Start Binary Search Tester ...";

	test1();
	vector<OmnString> values;
	values.push_back("aaaa");
	values.push_back("ffffff");
	values.push_back("kkkkk");
	values.push_back("bbbbb");
	values.push_back("ccccc");

	sort(values.begin(), values.end(), AosOmnStrLessAlpha);
	vector<OmnString>::iterator itr = values.begin();
	for (; itr != values.end(); itr++) OmnScreen << "Value: " << (*itr).data() << endl;
	bool rslt = AosBinaryInsert(values, "eeee", AosOrder::eAlphabetic);
	aos_assert_r(rslt, false);
	OmnScreen << "After insert: " << endl;
	itr = values.begin();
	for (; itr != values.end(); itr++) OmnScreen << "Value: " << (*itr).data() << endl;

	u32 pos = AosBinaryFindPos(values, "eeee", AosOrder::eAlphabetic);
	OmnScreen << "pos eeee: " << pos << endl;

	pos = AosBinaryFindPos(values, "gggggggg", AosOrder::eAlphabetic);
	OmnScreen << "pos gg: " << pos << endl;
	return true;
}


bool AosBinarySearchTester::test1()
{
	cout << "    Start Binary Search Tester ...";

	OmnString values[10];
	values[0] = "aaaaa";
	values[1] = "kkkkk";
	values[2] = "kkkkk";
	values[3] = "kkkkk";
	values[4] = "mmmmm";

	u64 docids[5];
	docids[0] = 10;
	docids[1] = 100;
	docids[2] = 101;
	docids[3] = 102;
	docids[4] = 200;

	// u32 pos = AosBinaryFindPos(values, docids, 5, "kkkkk", 101, AosOrder::eAlphabetic);
	return true;
}



