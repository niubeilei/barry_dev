////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrParserTester.h
// Description:
//   
//
// Modification History:
// 05/19/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_StrParserTester_h
#define Omn_TestUtil_StrParserTester_h

#include "Tester/TestPkg.h"


class OmnStrParserTester : public OmnTestPkg
{
private:

public:
	OmnStrParserTester() {mName = "OmnSIVTester";}
	~OmnStrParserTester() {}

	virtual bool		start();

private:
	bool	testGetNameValuePair(const u32 tries);
	bool	testGetSubstr(const u32 tries);
};


#endif

