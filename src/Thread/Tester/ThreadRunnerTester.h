//////////////////////////////////////////////////////////////////////////
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
// Modification History:
// 03/24/2009	Created by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef Aos_Thread_Tester_ThreadRunnerTester_h
#define Aos_Thread_Tester_ThreadRunnerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadRunnerObj.h"



class AosThreadRunnerTester : public OmnTestPkg, public AosThreadRunnerObj
{
private:
    int         		mTries;
	AosThreadRunnerPtr	mThreadRunner;

public:
    AosThreadRunnerTester();
    ~AosThreadRunnerTester();

    virtual bool    start();

	virtual bool	doThreadFunc(const OmnThreadPtr &thread);

private:
    bool basicTest();
};
#endif

