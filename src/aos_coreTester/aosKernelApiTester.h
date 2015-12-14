////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelApiTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_coreTester_aosKernelApiTester_h
#define Omn_aos_coreTester_aosKernelApiTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnAosKernelApiTester : public OmnTestPkg
{
private:

public:
	OmnAosKernelApiTester()
	{
		mName = "OmnAosKernelApiTester";
	}
	~OmnAosKernelApiTester() {}

	virtual bool		start();

private:
};
#endif

