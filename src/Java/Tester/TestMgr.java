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
// 
////////////////////////////////////////////////////////////////////////////

AosTestMgr::AosTestMgr(String &testId, 
					   OmnString &desc, 
					   OmnString &initiator)
{
	print << "To create TestMgr: " 
		<< testId << ":" << desc << ":" << initiator << endl;
}


OmnTestMgr::~OmnTestMgr()
{
}


bool
OmnTestMgr::start()
{
	initSeed();
	mStartTime = xxx;

	for (...)
	{
		mSuites[i]->start();
	}

	mEndTime = OmnGetTime(AosLocale::getDftLocale()); 
	return true;
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

