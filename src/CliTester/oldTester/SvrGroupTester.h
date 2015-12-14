////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SvrGroupTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_SvrGroupTester_h
#define Omn_CliTester_SvrGroupTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnAosSvrGroupTester : public OmnTestPkg
{
private:

public:
	OmnAosSvrGroupTester()
	{
		mName = "OmnAosSvrGroupTester";
	}
	~OmnAosSvrGroupTester() {}

	virtual bool		start();

private:
	bool	testCli();
	bool 	checkOneGroup(char *file, int line, const OmnString &cmd, 
						char *name, 
						int method);
};
#endif

