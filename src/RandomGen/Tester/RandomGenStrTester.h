////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomSelStrTester.h  
// Description:
//   
//
// Modification History:
// 05/10/2007 Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_Tester_RandomGenStrTester_h
#define Omn_Random_Tester_RandomGenStrTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/ValueRslt.h"
#include "Rundata/Rundata.h"
#include "RandomGen/RandomGenStr.h"
#include "Random/Ptrs.h"


class AosRandomGenStrTester : public OmnTestPkg
{
private:
	AosRandomGenObjPtr mRandomStr;

public:
    AosRandomGenStrTester();
    ~AosRandomGenStrTester() {}

    virtual bool            start();

private:
	bool	basicTest1(u32 tries);
	bool	basicTest2(u32 tries);
};
#endif
