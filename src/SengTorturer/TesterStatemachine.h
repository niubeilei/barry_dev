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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AosSengTester_StatemachineTester_h
#define AosSengTester_StatemachineTester_h

#include "Rundata/Ptrs.h"
#include "SengTorturer/SengTester.h"
#include "Tester/TestMgr.h"
#include "Thread/Ptrs.h"
#include "SengTorturer/StUtil.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"


class AosStatemachineTester : virtual public AosSengTester
{
public:

private:
	int mWtCreateStatemachine;
	int mWtRunStatemachine;
	int mWtModifyStatemachine;
	int mWtAccessCheck;
	int mWtStopStatemachine;

public:
	AosStatemachineTester(const bool regflag);
	AosStatemachineTester();
	~AosStatemachineTester();

	virtual bool test();
	AosSengTesterPtr clone()
	{
		return OmnNew AosStatemachineTester();
	}

private:
	bool	basicTest();
	bool 	createStatemachine();
	bool 	runStatemachine();
	bool 	modifyStatemachine();
	bool 	stopStatemachine();
	bool 	statemachineAccessCheck();
};
#endif

