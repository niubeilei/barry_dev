////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 	Created: 12/15/2008 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataStore/Testers/DataStoreTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
AosDataStoreTester::start()
{
	cout << "    Start DataStore Tester ...";
	u32 tries = 1000;

	// Drop the table
	//
	basicTest(tries);
	testTableExist();
	return true;
}


bool 
AosDataStoreTester::basicTest(u32 tries)
{
	// 
	// Test OmnString(const char *d);
	//
	OmnBeginTest << "Test OmnString(const char *d)";
	mTcNameRoot = "DataStore-Basic-Test";
	
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore(OmnStoreId::eGeneral);
	OmnCreateTc << (store) << endtc;

	// List All Tables
	std::list<std::string> names;
	int num_tables = store->getAllTableNames(names);
	OmnCreateTc << (num_tables > 0) << endtc;
	OmnCreateTc << (names.size() > 0) << endtc;
	while (names.size() > 0)
	{
		std::string name = names.front();
		OmnCreateTc << (name.length() > 0) << endtc;
		cout << "Table name: " << name.data() << endl;
		names.pop_front();
	}

	// Check whether table "datastore_test" exist
	if (store->tableExist("datastore_test"))
	{
		OmnRslt rslt = store->dropTable("datastore_test");
		OmnCreateTc << (!rslt) << endtc;
	}

	OmnString tabledef = 
		"create table datastore_test ("
		"       id              varchar(64) not null primary key,"
		"		type            char(1),"
		"		name            varchar(64),"
		"		keywords        varchar(128),"
		"		descrip         text,"
		"		xml             text,"
		"		status          integer"
		")";
	bool is_good = store->createTable(tabledef);
	OmnCreateTc << (is_good) << endtc;

	// Get all the column names
	std::list<std::string> colnames;
	int num_cols = store->getColumnNames("datastore_test", colnames);
	OmnCreateTc << (num_cols == 7) << endtc;

	// Check all the names
	std::string colname = colnames.front();
	OmnCreateTc << (colname == "id") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "type") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "name") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "keywords") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "descrip") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "xml") << endtc;
	colnames.pop_front();

	colname = colnames.front();
	OmnCreateTc << (colname == "status") << endtc;
	colnames.pop_front();

	OmnCreateTc << (store->tableExist("datastore_test")) << endtc;

	OmnRslt rslt = store->dropTable("datastore_test");
	OmnCreateTc << (rslt) << endtc;

	OmnCreateTc << (!store->tableExist("datastore_test")) << endtc;
	return true;
}


bool
AosDataStoreTester::testTableExist()
{
	std::list<std::string> list;
	OmnDataStorePtr store = OmnStoreMgrSelf->getStore();
	OmnCreateTc << store << endtc;

	int num_tables = store->getAllTableNames(list);
	OmnCreateTc << (num_tables > 0) << endtc;
	return true;
}


