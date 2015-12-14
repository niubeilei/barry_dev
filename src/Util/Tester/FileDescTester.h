////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileDescTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_FileDescTester_h
#define Omn_TestUtil_FileDescTester_h

#include "Tester/TestPkg.h"


class OmnFileDescTester : public OmnTestPkg
{
private:

public:
	OmnFileDescTester() {mName = "OmnFileDescTester";}
	~OmnFileDescTester() {}

	virtual bool		start();

private:
};


#endif

