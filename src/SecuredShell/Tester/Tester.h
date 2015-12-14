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
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SecuredShell_Tester_AosSecuredShellTester_h
#define Aos_SecuredShell_Tester_AosSecuredShellTester_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosSecuredShellTestSuite
{
private:

public:
	AosSecuredShellTestSuite() {}
	~AosSecuredShellTestSuite() {}

	static OmnTestSuitePtr		getSuite();
};
#endif

