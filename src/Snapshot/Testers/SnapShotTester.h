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
// 08/09/2013	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Snapshot_Testers_SnapShotTester_h
#define Aos_Snapshot_Testers_SnapShotTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "Snapshot/Testers/Ptrs.h"
#include "Tester/TestPkg.h"
#include "Thread/ThreadedObj.h"
#include <vector>
using namespace std;


class AosSnapShotTester : public OmnTestPkg,
						  public OmnThreadedObj
{
public:
	enum
	{
		eMaxThrds = 20,
		eThreadNum = 5 
	};

private:

	OmnThreadPtr					mThread[eMaxThrds];
	AosDfmObjectPtr					mObject[eMaxThrds];
public:
	AosSnapShotTester();
	~AosSnapShotTester();

	virtual bool			start();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	bool					config();
	bool					basicTest();
	bool					saveToFile(const u32 logicid, const AosDfmObjectPtr &dfm_test);
	bool					readFromFile(const AosRundataPtr &rdata, const u32 logicid, AosDfmObjectPtr &dfm_test);
	OmnFilePtr				openFile(const int idx, const bool read = false);
};
#endif

