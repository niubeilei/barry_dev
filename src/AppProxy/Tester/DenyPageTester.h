////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DenyPageTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_SslTester_DenyPageTester_h
#define Omn_SslTester_DenyPageTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class AosDenyPageTester : public OmnTestPkg
{
private:

public:
	AosDenyPageTester()
	{
		mName = "AosDenyPageTester";
	}
	~AosDenyPageTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

