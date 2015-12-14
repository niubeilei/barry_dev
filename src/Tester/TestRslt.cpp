////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestRslt.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestRslt.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"


OmnRslt
OmnTestRslt::serializeFromDb()
{
	// 
	// This function retrieves the test rslts identified by "test ID",
	// "suite ID", and "Package ID". 
	//

	//
	// Construct the query statement.
	//
	OmnString stmt = "select ";
    stmt << sqlAllFields() << " from test_rslts where test_id = '"
		 << mTestId << "' and suite_id = '"
		 << mSuiteName << "' and package_id = '"
		 << mPackageName << "'";

	// 
	// Retrieve the data store.
	//
    OmnDataStorePtr store = OmnStoreMgrSelf->getStore(mStoreId);
	if (store.isNull())
	{
		OmnString errmsg = OmnString("Store: ") << mStoreId << " not found!";
		OmnAlarm << errmsg << enderr;
		return OmnRslt(OmnErrId::eDbError, errmsg);
	}
		
    OmnTraceDb << "Query database: \n" << stmt 
		<< " from store: " << store->getName() << endl;

	// 
	// Query the database. 
	//
    OmnDbRecordPtr record;
    OmnRslt rslt = store->query(stmt, record);
    if (!rslt)
    {
		// 
		// Didn't found the record. 
		//
		OmnRslt rslt(OmnErrId::eObjectNotFound, "Object not found!");
		return rslt;
    }

	// 
	// Found the record. 
	//
	return serializeFromRecord(record);
}


OmnString
OmnTestRslt::insertStmt() const
{
    //
    // It creates a SQL statement to insert an endpoint into the SQL database
    //
    OmnString stmt = "insert into test_rslts("
        "test_id,"
		"suite_name,"
		"package_name,"
		"duration,"
		"success_tcs,"
        "failed_tcs"
        ") values ( '";

    stmt << mTestId   	<< "','"
		 << mSuiteName  << "','"
		 << mPackageName << "',"
		 << mDuration	 << ","
		 << mSuccessTcs  << ","
		 << mFailedTcs	 << ")";

    return stmt;
}


OmnString
OmnTestRslt::sqlAllFields() const
{
	return  "test_id,"
        	"suite_name,"
		 	"package_name,"
			"duration,"
			"success_tcs,"
			"failed_tcs";
}


OmnString
OmnTestRslt::removeStmt() const
{
	OmnString stmt = "delete from test_rslt where test_id='";
	stmt << mTestId << "' and suite_name='"
		 << mSuiteName << "' and package_name='"
		 << mPackageName << "'";
	return stmt;
}


OmnRslt
OmnTestRslt::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnRslt rslt(true);
	mTestId = record->getStr(0, "NoTestId", rslt);
	mSuiteName = record->getStr(1, "NoName", rslt);
	mPackageName = record->getStr(2, "NoName", rslt);
	mDuration = record->getInt(3, -1, rslt);
	mSuccessTcs = record->getInt(4, -1, rslt);
	mFailedTcs = record->getInt(5, -1, rslt);
	return true;
}
