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
// 04/27/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SQLite/Tester/DbTester.h"

#include "Debug/Debug.h"
#include "SQLite/DataStoreSQLite.h"
#include "SQLite/Tester/User.h"
#include "SQLite/Tester/Ptrs.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/File.h"
#include "Util/OmnNew.h"

bool OmnDbTester::start()
{
	// 
	// Test default constructor
	//
	try
	{
		OmnDataStoreSQLite::startSQLite("test.db");
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to start the database: " << e.toString() << enderr;
		return false;
	}

	// Remove all records from the table
	AosUser theUser;
	OmnTCTrue(theUser.removeAllFromDb()) << endtc;
	
	AosUserPtr user = OmnNew AosUser(100, "chending", "Chen", "Ding", "chending@hotmail.com", "111", "222", 'c');
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(user->addToDb())) << endtc;

	AosUserPtr user1 = OmnNew AosUser(100);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(user1->serializeFromDb())) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(user1->getUserId() == 100)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(user1->getUserName() == "chending")) << endtc;

	cout << "User ID: " << user1->getUserId() << endl;
	cout << "User Name: " << user1->getUserName() << endl;
	return true;
}

