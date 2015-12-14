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
#include "Book/Testers/WebbookTester.h"

#include "Alarm/Alarm.h"
#include "Book/BookMgr.h"
#include "Book/Webbook.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"

static AosBookMgr sgBookMgr;

bool 
AosWebbookTester::start()
{
	cout << "    Start Webbook Tester ...";
	u32 tries = 1000;

	OmnDataStorePtr store = OmnStoreMgrSelf->getStore(OmnStoreId::eGeneral);
	OmnCreateTc << (store) << endtc;

	// Drop the table 'testbook'
	OmnRslt rslt = store->dropTable("testbook");

	// Create the table 'testbook'
	OmnString tabledef = 
		"create table testbook ("
		"       bookid          varchar(64) not null primary key,"
		"		type            char(1),"
		"		name            varchar(64),"
		"		keywords        varchar(128),"
		"		descrip         text,"
		"		xml             text,"
		"		status          char(1)"
		")";
	bool is_good = store->createTable(tabledef);
	OmnCreateTc << (is_good) << endtc;

	basicTest(tries);
	return true;
}

bool 
AosWebbookTester::basicTest(u32)
{
	// Create a book
	AosWebbook book;
	book.setTablename("testbook");
	book.setName("First Book");
	book.setDesc("first book example");
	book.setKeywords("k1, k2, k3");
	book.setXml("<book />");
	
	OmnRslt rslt = book.addToDb();
	OmnCreateTc << (rslt) << endtc;

	AosBookPtr bk1 = sgBookMgr.getBook("testbook_001");
	OmnCreateTc << (bk1) << endtc;
	OmnCreateTc << (bk1->getBookType() == AosBookType::eWebbook) << endtc;
	OmnCreateTc << (bk1->getBookId() == "testbook_001") << endtc;
	OmnCreateTc << (bk1->getName() == "First Book") << endtc;
	OmnCreateTc << (bk1->getDesc() == "first book example") << endtc;
	OmnCreateTc << (bk1->getXml() == "<book />") << endtc;
	OmnCreateTc << (bk1->getKeywords() == "k1, k2, k3") << endtc;

	cout << bk1->getNewAttrName() << endl;
	return true;
}
