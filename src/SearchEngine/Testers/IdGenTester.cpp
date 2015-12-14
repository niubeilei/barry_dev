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
// 2009/10/09	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SearchEngine/Testers/IdGenTester.h"

#include "alarm_c/alarm.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "IdGen/IdGen.h"
#include "SearchEngine/WordMgr.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool AosIdGenTester::start()
{
	cout << "    Start OmnString Tester ...";
	basicTest();
	testDocId();
	return true;
}


bool AosIdGenTester::basicTest()
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test OmnString(const char *d)";
	mTcNameRoot = "String-CTR_C";
	OmnString c1("");

	OmnString stmt = "update id_generators set "
		"crtid='0' where name='testdocid'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store->runSQL(stmt), false);	

	/*
	AosIdGen idgen("testid");
	for (int i=0; i<1000; i++)
	{
		cout << idgen.getNextId() << " ";
		// OmnCreateTc << (c1 == "") << endtc;
	}
	cout << endl;	
	*/
	return true;
}


bool
AosIdGenTester::testDocId()
{
	// 1. Clear the id_generators record
	OmnString stmt = "update id_generators set "
		"crtid='0' where name='testid'";
	OmnDataStorePtr store = OmnStoreMgr::getSelf()->getStore();
	aos_assert_r(store->runSQL(stmt), false);	

	AosIdGen idgen("testdocid");
	u64 crtId = 2;
	int tries = 1000;
	for (int i=0; i<tries; i++)
	{
		OmnString id = idgen.getNextId();
		u64 dd1 = AosIdGen::strToU64(id, 0);
		OmnCreateTc << (dd1 != 0) << endtc;
		u64 vv = idgen.convertDocIdToU64(dd1, 0);
		OmnCreateTc << (vv == crtId) << endtc;

		OmnString dd2 = AosIdGen::u64ToStr(dd1);
		OmnCreateTc << (dd2 == id) << endtc;
		
		cout << id << ":" << dd2 << ",";

		crtId++;
	}
	cout << endl;
	return true;
}

