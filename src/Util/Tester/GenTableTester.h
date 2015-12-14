////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 06/30/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_GenTableTester_h
#define Omn_TestUtil_GenTableTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnGenTableTester : public OmnTestPkg
{
private:

public:
	OmnGenTableTester()
	{
		mName = "OmnGenTableTester";
	}
	~OmnGenTableTester() {}

	virtual bool		start();

private:
	bool	basicTest(const u32 tries);
};
#endif

