////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AaaServerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_AaaServerTester_h
#define Omn_CliTester_AaaServerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"
#include "Util/String.h"


class OmnAosAaaServerTester : public OmnTestPkg
{
private:

public:
	OmnAosAaaServerTester()
	{
		mName = "OmnAosAaaServerTester";
	}
	~OmnAosAaaServerTester() {}

	virtual bool		start();

private:
	bool	testCli();
	bool 	checkOneServer(const OmnString &cmd, 
						char *name, 
						char *type, 
						char *addr, 
						int port, 
						char *hc, 
						int status, 
						int refcnt, 
						char *filename, 
						int line);
};
#endif

