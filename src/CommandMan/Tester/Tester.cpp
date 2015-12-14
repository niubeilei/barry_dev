////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tester.cpp
// Description:
//   
//
// Modification History:
// 12/06/2006   Created by Harry Long
//
////////////////////////////////////////////////////////////////////////////
#include "CommandMan/Tester/Tester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "CommandMan/Tester/CommandManTorturer.h"


OmnTestSuitePtr
CommandManSuite::getSuite(const OmnString &FilePath)
{
        OmnTestSuitePtr suite = OmnNew 
                OmnTestSuite("CommandManSuite", "aosCommandMan Library Test Suite");

        // 
        // Now add all the testers
        //
        suite->addTestPkg(OmnNew CommandManTorturer(FilePath));

        return suite;
}
