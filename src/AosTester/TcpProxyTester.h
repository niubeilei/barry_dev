////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpProxyTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AosTester_TcpProxyTester_h
#define AosTester_TcpProxyTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosTcpProxyTester : public OmnTestPkg
{
private:

public:
	AosTcpProxyTester()
	{
		mName = "AosTcpProxyTester";
	}
	~AosTcpProxyTester() {}

	virtual bool		start();

private:
	bool	testCli();
};
#endif

