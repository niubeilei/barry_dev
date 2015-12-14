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
// 2013/06/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataProc/Tester/IILIndexingTester.h"

#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/GetTime.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/ThreadPool.h"
#include "Util/OmnNew.h"
#include "Util/Array3.h"
#include "XmlUtil/XmlTag.h"

static OmnMutex sgLock;
static vector<OmnThrdShellProcPtr> sgRunners;


AosIILIndexingTester::AosIILIndexingTester()
:
OmnThrdShellProc("test"),
mId(0),
mFinished(false)
{
	init();
}


AosIILIndexingTester::AosIILIndexingTester(const int id)
:
OmnThrdShellProc("test"),
mId(id)
{
	init();
}


AosIILIndexingTester::~AosIILIndexingTester()
{
}


bool 
AosIILIndexingTester::init()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(100);
	return true;
}


bool 
AosIILIndexingTester::start()
{
	return threadTesting();
}


bool
AosIILIndexingTester::threadTesting()
{
	/*
	vector<OmnThrdShellProcPtr> runners;
	mNumRunners = 10;
	for (int i=0; i<mNumRunners; i++)
	{
		runners.push_back(OmnNew AosIILIndexingTester(i));
	}

	OmnThreadPool pool("test");
	bool rslt = pool.procAsync(runners);
	aos_assert_r(rslt, false);

	while (1)
	{
		bool all_finished = true;
		for (int i=0; i<mNumRunners; i++)
		{
			if (!sgRunners[i]->isFinished())
			{
				all_finished = false;
				break;
			}
		}

		if (all_finished) return true;
	}
	*/
	return true;
}


bool		
AosIILIndexingTester::run()
{
	while (1)
	{
		readFile();
		procData();
	}
	return true;
}


bool
AosIILIndexingTester::readFile()
{
	// It reads in data from a file.
	OmnNotImplementedYet;
	return false;
}


bool
AosIILIndexingTester::procData()
{
	// It processes the data
	OmnNotImplementedYet;
	return false;
}


bool		
AosIILIndexingTester::procFinished()
{
	mFinished = true;
	return true;
}


