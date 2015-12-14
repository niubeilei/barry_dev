////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SrchStrTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_SrchStrTester_h
#define Omn_TestUtil_SrchStrTester_h

#include "Tester/TestPkg.h"


class OmnSrchStrTester : public OmnTestPkg
{
private:

public:
	OmnSrchStrTester() {mName = "OmnSrchStrTester";}
	~OmnSrchStrTester() {}

	virtual bool		start();

private:
};


#endif

