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
#ifndef Aos_Snapshot_TestersClt_SnapShotTester_h
#define Aos_Snapshot_TestersClt_SnapShotTester_h

#include "Random/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/IILObj.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Util/HashUtil.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include "IILMgr/IIL.h"
#include "Snapshot/TestersClt/Ptrs.h"
#include "Snapshot/TestersClt/IILProcTester.h"
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
		eDftTries = 1000000,
		eMaxNumIILs = 10,
		eMaxThrds = 20,
		eThreadNum = 2 
	};

private:

	OmnThreadPtr					mThread[eMaxThrds];
public:
	AosSnapShotTester();
	~AosSnapShotTester() {}

	virtual bool			start();

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) const{ return true;};
	virtual bool    signal(const int threadLogicId){ return true; };

private:
	AosIILProcTesterPtr		init(const AosRundataPtr &rdata, const u32 logicid);
	bool					proc(const AosRundataPtr &rdata, const u32 logicid);
	bool					config();
	bool					basicTest();
	AosIILProcTesterPtr		createIIL(const AosRundataPtr &rdata, const u32 logicid);
	bool					addEntry(const AosRundataPtr &rdata, const AosIILProcTesterPtr &proc);
	bool					addEntryByCreateSnap(const AosRundataPtr &rdata, const AosIILProcTesterPtr &proc);
	OmnString 				getRandomStr(u32 length);
	bool					createSnapShot(const AosIILProcTesterPtr &proc);
	bool					commitSnapShot(const AosIILProcTesterPtr &proc);
	bool					checkValue();
	bool					rollBackSnapShot(const AosIILProcTesterPtr &proc);
	bool					saveToFile(const AosIILProcTesterPtr &proc);
	bool					readFromFile(const AosIILProcTesterPtr &proc);
	OmnFilePtr				openFile(const int idx);
};
#endif

