////////////////////////////////////////////////////////////////////////////
////
//// Copyright (C) 2005
//// Packet Engineering, Inc. All rights reserved.
////
//// Redistribution and use in source and binary forms, with or without
//// modification is not permitted unless authorized in writing by a duly
//// appointed officer of Packet Engineering, Inc. or its derivatives
////
//// File Name: TorturerRandomSuite.h
//// Description:
////   
////
//// Modification History:
//// 11/27/2006   Created by Chen Ding
////
//////////////////////////////////////////////////////////////////////////////
#ifndef Random_Tester_TorturerRandomSuite_h
#define Random_Tester_TorturerRandomSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosTorturerRandomSuite
{
private:

public:
    AosTorturerRandomSuite() {}
    ~AosTorturerRandomSuite() {}

    static OmnTestSuitePtr          getSuite();
};
#endif

