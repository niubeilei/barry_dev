////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMgrTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AppMgrTester_AppMgrTester_h
#define Omn_AppMgrTester_AppMgrTester_h

#include "Tester/TestPkg.h"


class OmnTestMgr;

class OmnAppMgrTester : public OmnTestPkg
{
public:
	OmnAppMgrTester();
	~OmnAppMgrTester();

	virtual bool		start();

private:
	bool	testDefaultConstructor();
	bool	testIsConnected();
};
#endif

