////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CharPtreeTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_CharPtreeTester_h
#define Omn_KernelUtil_CharPtreeTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosCharPtreeTester : public OmnTestPkg
{
private:

public:
	AosCharPtreeTester()
	{
		mName = "AosCharPtreeTester";
	}
	~AosCharPtreeTester() {}

	virtual bool		start();

private:
	bool	normalTest();
};
#endif

