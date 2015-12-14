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
// 05/06/2007 Created by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef Aos_CliTorturer_Tester_AosApiTorturerTester_h
#define Aos_CliTorturer_Tester_AosApiTorturerTester_h

#include "CliTorturer/ApiTorturer.h"
#include "CliTorturer/Tester/Ptrs.h"
#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Torturer/Ptrs.h"
#include "Util/String.h"
#include "Util/Ptrs.h"


class AosApiTorturerTester : public OmnTestPkg 
{
private:

public:
    AosApiTorturerTester();
    ~AosApiTorturerTester();

    virtual bool    start();

private:
	bool	basicTest(const u32 tries);
};
#endif

