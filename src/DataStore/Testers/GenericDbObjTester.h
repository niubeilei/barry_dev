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
//	Created: 12/22/2008 by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_DataStore_Testers_GenericDbObjTester_h
#define Aos_DataStore_Testers_GenericDbObjTester_h

#include "Tester/TestPkg.h"


class AosGenericDbObjTester : public OmnTestPkg
{
private:

public:
	AosGenericDbObjTester() {mName = "GenericDbObjTester";}
	~AosGenericDbObjTester() {}

	virtual bool		start();

private:
	bool	basicTest(u32);
};


#endif

