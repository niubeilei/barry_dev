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
// 12/07/2009 	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_BuffTester_h
#define Omn_TestUtil_BuffTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnBuffTester : public OmnTestPkg
{
private:
	int		mTries;

public:
	OmnBuffTester();
	~OmnBuffTester() {}

	virtual bool		start();

private:
	OmnString	getRandStr();
	bool		torturer();
};
#endif

