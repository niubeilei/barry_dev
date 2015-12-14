////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FileTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_FileTester_h
#define Omn_TestUtil_FileTester_h

#include "Tester/TestPkg.h"


class OmnFileTester : public OmnTestPkg
{
private:

public:
	OmnFileTester() {mName = "OmnFileTester";}
	~OmnFileTester() {}

	virtual bool		start();

private:
	bool	basicTest();
	bool	testGetFiles();
	bool	testSetContents();
};


#endif

