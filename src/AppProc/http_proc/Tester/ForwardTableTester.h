////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ForwardTableTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AppProc_http_proc_Tester_ForwardTableTester_h
#define Aos_AppProc_http_proc_Tester_ForwardTableTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosForwardTableTester : public OmnTestPkg
{
private:

public:
	AosForwardTableTester()
	{
		mName = "AosForwardTableTester";
	}
	~AosForwardTableTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};
#endif

