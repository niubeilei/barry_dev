////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TortureRouterSuite.h
// Description:
//   
//
// Modification History:
// 11/17/2006 	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Torturer_Tester_TorturerRouterSuite_h
#define Torturer_Tester_TorturerRouterSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosRouterSuite
{
private:

public:
	AosRouterSuite();
	virtual ~AosRouterSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

