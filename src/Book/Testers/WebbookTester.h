////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
//	Created: 12/15/2008 by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Book_Testers_WebbookTester_h
#define Aos_Book_Testers_WebbookTester_h

#include "Tester/TestPkg.h"


class AosWebbookTester : public OmnTestPkg
{
private:

public:
	AosWebbookTester() {mName = "WebbookTester";}
	~AosWebbookTester() {}

	virtual bool		start();

private:
	bool	basicTest(u32);
};


#endif

