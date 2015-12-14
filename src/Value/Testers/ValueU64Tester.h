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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Value_Testers_ValueU64Tester_h
#define Aos_Value_Testers_ValueU64Tester_h

#include "Value/Testers/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/BmpExeCaller.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/ThrdShellProc.h"
#include "Util/Opr.h"
#include "Util/Ptrs.h"
#include <vector>
using namespace std;


class AosValueU64Tester : public OmnTestPkg, public OmnThrdShellProc
{
private:
	int mTries;
	int mNumThreads;
	int mInstId;
	u64 mCounts[10];

public:
	AosValueU64Tester();
	AosValueU64Tester(const int inst_id);
	~AosValueU64Tester() {}

	virtual bool		start();

	// ThrdShellProc interface
	virtual bool        run();
	virtual bool        procFinished();

private:
	bool	init();
	bool	basicTest();
	bool	config();
};
#endif
