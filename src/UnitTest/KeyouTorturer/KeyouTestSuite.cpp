////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KeyouTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "UnitTest/KeyouTorturer/KeyouTestSuite.h"
#include "UnitTest/KeyouTorturer/KeyouTester.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

OmnKeyouTestSuite::OmnKeyouTestSuite(const OmnString &suiteId, 
				 	  				 const OmnString &desc)
:
OmnObject(OmnClassId::eOmnTestSuite),
OmnDbObj(OmnClassId::eOmnTestSuite, OmnStoreId::eTest),
OmnTestSuite(suiteId,desc),
mLock(OmnNew OmnMutex()),
mFinished(false)
{
	if (mThread.isNull())
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "TcpCltGrp", 0, false, true);
	}	
}

OmnKeyouTestSuite::~OmnKeyouTestSuite()
{
}

OmnTestSuitePtr		
OmnKeyouTestSuite::getSuite(const OmnIpAddr &localAddr,
							const OmnIpAddr &serverIp,
							const int		&ftpPort,
							const int		&ftpDataPort,
							const int		&telnetPort)
{
	OmnTestSuitePtr suite = OmnNew OmnKeyouTestSuite("UnitTestSuite", "Unit Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosKeyouTester(localAddr,
											serverIp,
											ftpPort,
											ftpDataPort,
											telnetPort));
	return suite;
}

int
OmnKeyouTestSuite::start(const OmnTestMgrPtr &testMgr)
{
	mTestMgr = testMgr;
	if(mThread)
	{
		mThread->start();
	}else
	{
		OmnAlarm << "Thread is null!!!" << enderr;
	}
	
	return 0;
}

int
OmnKeyouTestSuite::run()
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

		mTestMgr->setCrtPkg(package);
		package->setTestMgr(mTestMgr);
		package->start();
		mTestMgr->setCrtPkg(0);

		mTotalTc += package->getTotalTcs();
		mTotalFailed += package->getTotalFailedTcs();
	}

	mTestMgr->addRslt(mTotalTc, mTotalFailed);
	return mTotalTc;
}

bool
OmnKeyouTestSuite::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    //
    //
    OmnTrace << "Enter OmnKeyouTestSuite::threadFunc." << endl;

    while ( state == OmnThrdStatus::eActive )
    {
    	run();    	
    	mFinished = true;
    	state = OmnThrdStatus::eStop;
	}

	return true;
}

bool 
OmnKeyouTestSuite::signal(const int threadLogicId)
{
	return true;
}


void
OmnKeyouTestSuite::heartbeat()
{
	//
	// Currently not managed by ThreadMgr. Do nothing.
	//
}


bool
OmnKeyouTestSuite::checkThread(OmnString &err, const int thrdLogicId) const
{
	//
	// Currently not managed by ThreadMgr. Always return true
	//
    return true;
}
