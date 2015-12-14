////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppServerTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_AppServerTester_h
#define Omn_CliTester_AppServerTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnAosAppServerTester : public OmnTestPkg
{
private:

public:
	OmnAosAppServerTester()
	{
		mName = "OmnAosAppServerTester";
	}
	~OmnAosAppServerTester() {}

	virtual bool		start();

private:
	bool	testCli();
	bool 	checkOneServer(const OmnString &cmd, 
						char *name, 
						char *type, 
						char *addr, 
						int port, 
						char *protocol, 
						int status);
};
#endif

