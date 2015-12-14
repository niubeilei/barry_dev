////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TortureDnsmasqSuite.h
// Description:
//   
//
// Modification History:
// 11/17/2006 	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#ifndef Torturer_Tester_TorturerDnsmasqSuite_h
#define Torturer_Tester_TorturerDnsmasqSuite_h

#include "Tester/Ptrs.h"
#include "Util/String.h"


class AosDnsmasqSuite
{
private:

public:
	AosDnsmasqSuite();
	virtual ~AosDnsmasqSuite();

	static OmnTestSuitePtr		getSuite();
};
#endif

