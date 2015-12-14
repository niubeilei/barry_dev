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
// 2013/01/18 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_OmnNewTester_h
#define Omn_TestUtil_OmnNewTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class AosNewTester : public OmnTestPkg
{
private:

public:
	AosNewTester()
	{
		mName = "AosNewester";
	}
	~AosNewTester() {}

	virtual bool		start();

private:
};




#endif

