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
// This file is automatically generated by the TorturerGen facility. 
//
// Modification History:
// 5/27/2007 Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#ifndef Aos_Util1_Tester_AosWaitTester_h
#define Aos_Util1_Tester_AosWaitTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/Ptrs.h"



class AosWaitTester : public OmnTestPkg
{
private:
    int              mBasicTestRepeat;
    AosGenTablePtr   mTable;

public:
    AosWaitTester();
    ~AosWaitTester();

    virtual bool     start();


private:
    bool basicTest(const u32 repeat, const AosGenTablePtr &table);
};
#endif

