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
// 08/04/2009: 	by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Book_Testers_ContainerTester_h
#define Aos_Book_Testers_ContainerTester_h

#include "Tester/TestPkg.h"


class AosContainerTester : public OmnTestPkg
{
private:

public:
	AosContainerTester() {mName = "WebbookTester";}
	~AosContainerTester() {}

	virtual bool		start();

private:
	bool	basicTest(u32);
};


#endif

