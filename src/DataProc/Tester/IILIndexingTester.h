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
#ifndef Omn_DataProc_Tester_IILIndexingTester_h
#define Omn_DataProc_Tester_IILIndexingTester_h

#include "Debug/Debug.h"
#include "DataProc/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosIILIndexingTester : public OmnTestPkg, public OmnThrdShellProc
{
private:
	int				mId;
	int				mNumRunners;
	bool			mFinished;
	AosRundataPtr 	mRundata;

public:
	AosIILIndexingTester();
	AosIILIndexingTester(const int id);
	~AosIILIndexingTester();

	virtual bool		start();

	// ThrdShellProc interface
	virtual bool		run();
	virtual bool		procFinished();

	bool isFinished() const {return mFinished;}

private:
	bool init();
	bool threadTesting();
	bool readFile();
	bool procData();
};
#endif

