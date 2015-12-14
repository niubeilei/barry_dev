////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MemoryTester.h
// Description:
//   
//
// Modification History:
// 02/15/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef SEClient_TcpClientTester_TcpClientTester_h
#define SEClient_TcpClientTester_TcpClientTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "UtilComm/TcpClient.h"


class AosTcpClientTester : public OmnTestPkg
{
private:

public:
	AosTcpClientTester()
	{
	}
	~AosTcpClientTester() {}
	virtual bool        start();

private:
	bool	basicTest();
};
#endif

