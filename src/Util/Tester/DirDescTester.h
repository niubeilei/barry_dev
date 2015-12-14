////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: DirDescTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_DirDescTester_h
#define Omn_TestUtil_DirDescTester_h

#include "Tester/TestPkg.h"


class OmnDirDescTester : public OmnTestPkg
{
private:

public:
	OmnDirDescTester() {mName = "OmnDirDescTester";}
	~OmnDirDescTester() {}

	virtual bool		start();

private:
};


#endif

