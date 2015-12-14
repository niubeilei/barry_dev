////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestSuite.cpp
// Description:
//	A TestSuite is a collection of Testers. The creator of a 
//  Test Suite shall determine which testers to be included
//  in the suite.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestSuite.h"

#include "Alarm/Alarm.h"
#include "Database/DbRecord.h"
#include "DataStore/StoreMgr.h"
#include "DataStore/DataStore.h"
#include "Debug/Debug.h"
#include "Tester/SuiteEntry.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"



bool	
OmnTestSuite::addTestPkg(const OmnTestPkgPtr &package)
{
	mPackages.append(package);
	return true;
}


#ifdef AOS_DB_SUPPORT
OmnRslt
OmnTestSuite::serializeFromDb()
{
	// 
	// This function retrieves the test suite execution result from 
	// suite_report table. The entry is identified by mTestId and 
	// mSuiteName. 
	//

	//
	// Construct the query statement.
	//
	OmnString stmt = "select ";
    stmt << sqlAllFields() << " from suite_report where test_id='"
		 << mTestId << "', and suite_id = '"
		 << mSuiteName << "'";

	// 
	// Query the database. 
	//
	OmnTraceDb << "Retrieve suite record: " << stmt << endl;
    OmnDbRecordPtr record;
    OmnRslt rslt = retrieveRecord(stmt, record);

    if (!rslt)
    {
		// 
		// Didn't found the record. 
		//
		OmnAlarm << "Failed to retrieve suite result from db: " 
			<< rslt.toString() << enderr;
		return rslt;
    }

	// 
	// Found the record. 
	//
	rslt = serializeFromRecord(record);
	if (!rslt)
	{
		// 
		// failed the serialization. 
		//
		OmnAlarm << "Failed to serialize from record: " 
			<< rslt.getErrmsg() << enderr;
		return rslt;
	}

	return true;
}


OmnRslt	
OmnTestSuite::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnNotImplementedYet;
	return false;
}


OmnString
OmnTestSuite::sqlAllFields() const
{
	return "test_id,"
		   "suite_name,"
		   "description,"
		   "num_packages,"
		   "total_tc,"
		   "total_failed_tc";
}


OmnString
OmnTestSuite::updateStmt() const
{
    OmnString stmt = "update suite_report set ";
    stmt << "', description='" << mDesc
		 << "', num_packages=" << mPackages.entries()
         << ",  total_tc=" << mTotalTc
		 << ",  total_failed_tc=" << mTotalFailed
		 << " where test_id='" << mTestId 
		 << "' and suite_name='" << mSuiteName;
    return stmt;
}


OmnString
OmnTestSuite::existStmt() const
{
	OmnString stmt = "select * from suite_report where ";
	stmt << "test_id='" << mTestId << "' and '"
		 << "suite_name='" << mSuiteName << "'";
	return stmt;
}


OmnString
OmnTestSuite::insertStmt() const
{
    //
    // It creates a SQL statement to insert a test suite execution result
	// into the table 'suite_report'.
    //
    OmnString stmt = "insert into suite_report("
		"test_id,"
        "suite_name,"
        "description,"
        "num_packages,"
		"total_tc,"
		"total_failed_tc"
        ") values ( '";

    stmt << mTestId				<< "','"
		 << mSuiteName   		<< "','"
         << mDesc 				<< "',"
         << mPackages.entries() << ","
		 << mTotalTc			<< ","
		 << mTotalFailed		<< ")";

    return stmt;
}


OmnString
OmnTestSuite::removeStmt() const
{
    //
    // Suite execution results are stored in 'suite_rslt'. Entries in this
	// table is identified by 'test_id' and 'suite_name'. 
    //
	OmnString stmt = "delete from suite_rslt where test_id='";
	stmt << mTestId << "' and suite_name='"
		 << mSuiteName << "'";
    return stmt;
}


OmnDbObjPtr	
OmnTestSuite::getNotInDbObj() const
{
	OmnTestSuitePtr obj = OmnNew OmnTestSuite(mSuiteName, "");
	obj.setDelFlag(false);

	return obj.getPtr();
}


OmnDbObjPtr	
OmnTestSuite::clone() const
{
	OmnTestSuitePtr obj = OmnNew OmnTestSuite(mSuiteName, mDesc);
	obj.setDelFlag(false);

	obj->mTestId = mTestId;
	obj->mPackages = mPackages;
	obj->mTotalTc = mTotalTc;
	obj->mTotalFailed = mTotalFailed;

	return obj.getPtr();
}


bool
OmnTestSuite::hasSameObjId(const OmnDbObjPtr &rhs) const
{
	if (rhs->getClassId() != OmnClassId::eOmnTestSuite)
	{
		return false;
	}

	return mSuiteName == reinterpret_cast<OmnTestSuite*>(rhs.getPtr())->mSuiteName;
}

#endif


OmnString
OmnTestSuite::toString() const 
{
	OmnString str = "OmnTestSuite: ";
	str << "\n    SuiteId:    " << mSuiteName
		<< "\n    Desc:       " << mDesc
		<< "\n    NumEntries: " << mPackages.entries();
	return str;
}


int
OmnTestSuite::start(const OmnTestMgrPtr &testMgr)
{
	OmnTrace << "Suite: " << mSuiteName << " with "
		<< mPackages.entries() << " packages!" << endl;

	mPackages.reset();
	OmnTestPkgPtr package;

	mTotalTc = 0;
	mTotalFailed = 0;
	while (mPackages.hasMore() && mTotalFailed < eMaxFailed)
	{
		package = mPackages.crtValue();
		mPackages.next();
OmnTrace << "To start package: " << package->getPkgName() << endl;
		testMgr->setCrtPkg(package);
		package->setTestMgr(testMgr);
		package->start();
		testMgr->setCrtPkg(0);

		mTotalTc += package->getTotalTcs();
		mTotalFailed += package->getTotalFailedTcs();
	}

//	testMgr->addRslt(mTotalTc, mTotalFailed);
	return mTotalTc;
}


int 
OmnTestSuite::objSize() const
{
	return mTestId.objSize() + mSuiteName.objSize()
		+ mDesc.objSize() + mPackages.objSize();
}


