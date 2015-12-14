////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestPkg.cpp
// Description:
//	A Test Package is a group of test cases. It is different
//  from a test suite. A test suite is a group of test packages. 
//  Normally, a test package is a group of test cases to test
//  a specific module/class.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Tester/TestPkg.h"

#include "Debug/Debug.h"
#include "Tester/TestMgr.h"
#include "Tracer/Tracer.h"



void	     
OmnTestPkg::operator << (const OmnEndTc &etc)
{
	mLock->lock();
	if (!mCrtTc.isValid())
	{
		// 
		// It is not a valid test case. Add it to the test manager.
		//
		if (mTestMgr)
		{
			mTestMgr->addFailedTc(mCrtTc, mPreTc);
		}
		else
		{
			OmnAlarm << "TestMgr is null: "
				<< mCrtTc.toString()
				<< ". Previous: " << mPreTc.toString() << enderr;
		}
	}
	else
	{
		// 
		// It is a valid test case.
		//
		mNumTcs++;
		if (!mCrtTc.isSuccess())
		{
			mNumTcsFailed++;
		}

		if (mTestMgr)
		{
			mTestMgr->tcFinished(mCrtTc);
		}
		else
		{
			OmnAlarm << "TestMgr is null: " << mCrtTc.toString() << enderr;
		}
	}

	mPreTc = mCrtTc;
	mCrtTc.reset();
	mLock->unlock();
}


OmnString
OmnTestPkg::nextTcName()
{
	// 
	// The next test case name is constructed as:
	//	mTcNameRoot + "-" + mTcSeqno
	//
	OmnString str = mTcNameRoot;
	str << "-" << mTcSeqno++;
	return str;
}


void
OmnTestPkg::setTestMgr(const OmnTestMgrPtr &testMgr)
{
	mTestMgr = testMgr;
}


int
OmnTestPkg::objSize() const
{
	return sizeof(*this) + 
		mName.objSize() + 
		mTcNameRoot.objSize() + 
		mTcContextDesc.objSize();
}

	
OmnTestMgrPtr	
OmnTestPkg::getTestMgr() const
{
	return mTestMgr;
}


// void
// OmnTestPkg::addTcCounts(const bool flag)
// {
// 	aos_assert(mTestMgr);
// 
// 	mTestMgr->addRslt(mNumTcs, mNumTcsFailed);
// 	if (flag)
// 	{
// 		mNumTcs = 0;
// 		mNumTcsFailed = 0;
// 	}
// }

void
OmnTestPkg::updateSeed()
{
	if(mTestMgr)
	{
		mTestMgr->updateSeed();
	}
}


// Chen Ding, 2013/02/09
int 
OmnTestPkg::pickOperation(const vector<int> &weights)
{
	// 'weights' is an array of weights:
	// 		weight[0] = weight 0
	// 		weight[1] = weight 0 + weight 1
	// 		weight[2] = weight 0 + weight 1 + weight 2
	// 		...
	// This function randomly generates an integer [0, weight[size-1]]. 
	// It then returns the index of the first element:
	// 		number <= weight[index]
	if (weights.size() <= 0) return 0;
	int total = weights[weights.size()-1];
	if (total <= 0) return 0;

	int nn = rand() % total;
	for (u32 i=0; i<weights.size(); i++)
	{
		if (nn <= weights[i]) return i;
	}

	return weights.size()-1;
}

