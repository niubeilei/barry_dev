////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SuiteEntry.cpp
// Description:
//	A SuiteEntry is a collection of Tester.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/SuiteEntry.h"

/*
#include "Alarm/Alarm.h"
#include "Database/DbTable.h"
#include "Database/DbRecord.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"


OmnRslt
OmnSuiteEntry::serializeFromDb()
{
	OmnAlarm << "Not implement yet!" << enderr;
	return false;
}


OmnString
OmnSuiteEntry::insertStmt() const
{
    //
    // It creates a SQL statement to insert a suite entry
    //
    OmnString stmt = "insert into suite_entry("
        "suite_name,"
        "package_name"
        ") values ( '";

    stmt << mSuiteName 		<< "','"
		 << mPackageName	<< "')";

    return stmt;
}


OmnString
OmnSuiteEntry::removeStmt() const
{
	OmnString stmt = "delete from suite_entry where suite_name='";
	stmt << mSuiteName << "' and package_name='"
		 << mPackageName << "'";
	return stmt;
}


OmnString
OmnSuiteEntry::toString() const
{
	OmnString str = "OmnSuiteEntry: ";
	str << "\n    SuiteId:     " << mSuiteName
		<< "\n    PackageName: " << mPackageName;
	return str;
}


OmnString
OmnSuiteEntry::sqlAllFields() const
{
	return "suite_name,"
		   "package_name";
}


OmnRslt
OmnSuiteEntry::retrieveEntriesForSuite(const OmnString &suiteId,
									   OmnSmartList<OmnTestPkgPtr> &packages) const
{
	// 
	// Construct the query statement
	//
	OmnString stmt = "select ";
    stmt << sqlAllFields() << " from suite_entries where suite_id = '"
		 << suiteId << "'";

	// 
	// Query the database
	//
	OmnDbTablePtr table;
	OmnRslt rslt = retrieveRecords(stmt, table);
	if (!rslt)
	{
		// 
		// Failed to retrieve the table. 
		//
		OmnAlarm << "Failed to retrieve suite entries: " 
			<< rslt.getErrmsg() << enderr;
		return rslt;
	}
    OmnTraceDb << "Num of Suite entries queried: \n" << table->entries() << endl;

	table->reset();
	OmnDbRecordPtr record;
	while (table->hasMore())
	{
		record = table->next();
		// OmnTestPkgPtr package = OmnNew OmnTextPkg
	}

	return true;
}
*/

