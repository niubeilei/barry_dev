////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tester.h
// Description:
//   
//
// Modification History:
// 12/06/2006   Created by Harry Long
//
////////////////////////////////////////////////////////////////////////////
#ifndef CommandMan_Tester_CommandManSuite_h
#define CommandMan_Tester_CommandManSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class CommandManSuite
{
private:

public:
        CommandManSuite() {}
        ~CommandManSuite() {}

        static OmnTestSuitePtr          getSuite(const OmnString  &FilePath);
};
#endif
