////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosBridgeTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_coreTester_aosBridgeTester_h
#define Omn_aos_coreTester_aosBridgeTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnAosBridgeTester : public OmnTestPkg
{
private:

public:
	OmnAosBridgeTester()
	{
		mName = "OmnAosBridgeTester";
	}
	~OmnAosBridgeTester() {}

	virtual bool		start();

private:
	bool	testCli();
};
#endif

