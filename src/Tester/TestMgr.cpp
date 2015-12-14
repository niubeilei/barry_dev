////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Tester/TestMgr.h"

#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "DataStore/DataStore.h"
#include "DataStore/StoreMgr.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Random/RandomUtil.h"
#include "Tester/Testcase.h"
#include "Tester/TestRslt.h"
#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"



OmnTestMgr::OmnTestMgr(const OmnString &testId, 
					   const OmnString &desc, 
					   const OmnString &initiator)
:
OmnObject(OmnClassId::eOmnTestMgr),
#ifdef AOS_DB_SUPPORT
OmnDbObj(OmnClassId::eOmnTestMgr, OmnStoreId::eTest),
#endif
mTcs(0),
mFailedTcs(0),
mTestId(testId),
mDesc(desc),
mInitiator(initiator),
mStartTime("NoTime"),
mEndTime("NoTime"),
mNeedToRecordGoodTcs(false),
mSaveToDb(false),
mSeedType(eNoInit),
mOrigSeed(0),
mCrtSeed(0)
{
	OmnTrace << "To create TestMgr: " 
		<< testId << ":" << desc << ":" << initiator << endl;
}


OmnTestMgr::~OmnTestMgr()
{
}


/*
OmnRslt
OmnTestMgr::addStat(const OmnString &suiteName,
					const OmnString &packageName, 
					const int duration,
				    const int successTcs, 
				    const int failedTcs)
{
	OmnTestRslt testRslt(mTestId, suiteName, packageName, duration, successTcs, failedTcs); 

	OmnRslt rslt = testRslt.serializeToDb();
	if (!rslt)
	{
		// 
		// Something is wrong. 
		//
		OmnAlarm << "Failed to store testcase statistics in db: "
			<< rslt.getErrmsg() << enderr;
		return rslt;
	}

	return true;
}
*/


bool
OmnTestMgr::start()
{
	initSeed();
	mStartTime = OmnGetTime(AosLocale::getDftLocale());

#ifdef AOS_DB_SUPPORT
	// 
	// Remove all the test results for this test.
	//
	OmnTestcase tc;
	tc.removeRslts(mTestId);
#endif

	// 
	// It loops on all the test suites.
	//
	mSuites.reset();
	OmnTestMgrPtr thisPtr(this, false);
	mTcs = 0;
	mFailedTcs = 0;
	while (mSuites.hasMore())
	{
		mCrtSuite = mSuites.next();
		mCrtSuite->setTestId(mTestId);

		OmnTrace << "To test suite: " << mCrtSuite->getSuiteName() << endl;
		mCrtSuite->start(thisPtr);
		OmnTrace << "Finished test suite: " << mCrtSuite->getSuiteName() << endl;
	}

	mEndTime = OmnGetTime(AosLocale::getDftLocale()); 
	return true;
}


void
OmnTestMgr::setCrtSuite(const OmnTestSuitePtr &suite)
{
	mSuites.append(suite);
	mCrtSuite = suite;
}

void
OmnTestMgr::addSuite(const OmnTestSuitePtr &suite)
{
	mSuites.append(suite);
}


OmnString
OmnTestMgr::toString() const
{	
	OmnString str = "OmnTestMgr";
	str << "\n    TestId:      " << mTestId
		<< "\n    Description: " << mDesc
		<< "\n    Initiator:   " << mInitiator
		<< "\n    StartTime:   " << mStartTime
		<< "\n    EndTime:     " << mEndTime;
	return str;
}


void
OmnTestMgr::tcFinished(OmnTestcase &tc)
{
	mTcs++;

	// 
	// It finished one test case. If the test case is passed, it checks
	// whether it needs to record it. If it failed, it will record it.
	//
	if (tc.isSuccess())
	{
		if (mNeedToRecordGoodTcs)
		{
			// 
			// It is a good test case and it needs to record it.
			//
			saveTcRslt(tc);
		}
	}
	else
	{
		// 
		// The test case failed. Record it.
		//
		saveTcRslt(tc);
		mFailedTcs++;

	}
}


void
OmnTestMgr::saveTcRslt(OmnTestcase &tc)
{
	tc.setTestId(mTestId);
	tc.setSuiteName(mCrtSuite->getSuiteName());

	if (mCrtPkg)
	{
		tc.setPkgName(mCrtPkg->getPkgName());
	}

	if (mSaveToDb)
	{
#ifdef AOS_DB_SUPPORT
		OmnRslt rslt = tc.addToDb();
#endif
	}
	else
	{
		//OmnTrace << tc.toString() << endl;
		cout << tc.toString().data() << endl;
	}
}


void
OmnTestMgr::setCrtPkg(const OmnTestPkgPtr &pkg)
{
	mCrtPkg = pkg;
}


void	
OmnTestMgr::addFailedTc(const OmnTestcase &tc, const OmnTestcase &preTc)
{
	OmnNotImplementedYet;
}


OmnString
OmnTestMgr::getStat() const
{
	OmnString str;
	str << "\n    Test ID:      " << mTestId
		<< "\n    Initiator:    " << mInitiator
		<< "\n    Description:  " << mDesc
		<< "\n    Started at:   " << mStartTime
		<< "\n    Finished at:  " << mEndTime
		<< "\n    Total TC:     " << mTcs
		<< "\n    Total Failed: " << mFailedTcs;
	return str;
}


int
OmnTestMgr::objSize() const
{
	int size = sizeof(*this) + 
		mTestId.objSize() + 
		mDesc.objSize() + 
		mInitiator.objSize() +
		mStartTime.objSize() + 
		mEndTime.objSize() +
		mSuites.objSize();

	if (mCrtSuite) size += mCrtSuite->objSize();
	if (mCrtPkg) size += mCrtPkg->objSize();
	return size;
}


void					
OmnTestMgr::setSeed(int &argc, char** &argv)
{
    int index = 1;
    while (index < argc)
    {
        if (strcmp(argv[index], "-random") == 0)
        {
			mSeedType = eRandom;
			return;
        }

        if (strcmp(argv[index], "-seed") == 0)
        {
			if(index +1 >=  argc)
			{
				// no seed specified
				return;
			}
			mOrigSeed = atoi(argv[index+1]);
            continue;
        }
 	}	
	return;
}


void
OmnTestMgr::initSeed()
{
	switch(mSeedType)
	{
	case eRandom:
		 srand((unsigned int)time(0));
		 mOrigSeed = rand();
		 break;

	case eNoInit:
		 mOrigSeed = rand();
		 break;

	case eSpecify:
		 // already set
		 break;

	default:
		 mOrigSeed = rand();
		 break;
	}	
	OmnRandom::setSeed(mOrigSeed);
}


void
OmnTestMgr::updateSeed()
{
	mCrtSeed = rand();
	cout << "originate seed: " << mOrigSeed << endl;
	cout << "current seed:   " << mCrtSeed << endl;
	OmnRandom::setSeed(mCrtSeed);
}


OmnTestSuitePtr
OmnTestMgr::getCrtSuite() const
{
	return mCrtSuite;
}


#ifdef AOS_DB_SUPPORT
OmnString
OmnTestMgr::insertStmt() const
{
    //
    // It creates a SQL statement to insert a suite entry
    //
    OmnString stmt = "insert into test_rslts("
        "suite_id,"
        "tester_name,"
		"tcs,"
        "failed_tcs"
        ") values ( '";

    stmt << mTestId   	<< "','"
		 << mDesc		<< "','"
		 << mInitiator	<< "','"
		 << mStartTime	<< "','"
		 << mEndTime	<< "',"
		 << mTcs		<< ","
		 << mFailedTcs	<< ")";

    return stmt;
}


OmnDbObjPtr	
OmnTestMgr::clone() const
{
	OmnNotImplementedYet;
	return 0;
}


OmnRslt
OmnTestMgr::serializeFromDb()
{
	OmnNotImplementedYet;
	return false;
}


OmnDbObjPtr	
OmnTestMgr::getNotInDbObj() const
{
	OmnTestMgrPtr obj = OmnNew OmnTestMgr(mTestId, "", "");
	obj.setDelFlag(false);

	return obj.getPtr();
}


bool		
OmnTestMgr::hasSameObjId(const OmnDbObjPtr &rhs) const
{
	if (rhs->getClassId() != OmnClassId::eOmnTestMgr)
	{
		return false;
	}

	return mTestId == reinterpret_cast<OmnTestMgr*>(rhs.getPtr())->mTestId;
}


OmnRslt		
OmnTestMgr::serializeFromRecord(const OmnDbRecordPtr &record)
{
	OmnNotImplementedYet;
	return false;
}


OmnString
OmnTestMgr::existStmt() const
{
	OmnNotImplementedYet;
	return "";
}


OmnString
OmnTestMgr::removeStmt() const
{
	// OmnString stmt = "delete from test_rslt where test_id='";
	// stmt << mTestId << "' and suite_name='"
	//	 << mSuiteName << "' and package_name='"
	//	 << mPackageName << "'";
	//return stmt;
	return "";
}


OmnString
OmnTestMgr::updateStmt() const
{
	OmnNotImplementedYet;
	return "";
}

#endif
