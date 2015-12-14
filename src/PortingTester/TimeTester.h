////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimeTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_PortingTester_TimeTester_h
#define Omn_PortingTester_TimeTester_h

#include "Tester/TestPkg.h"


class OmnTestMgr;

class OmnTimeTester : public OmnTestPkg
{
public:
	OmnTimeTester();
	~OmnTimeTester();

	virtual bool		start();
};
#endif

