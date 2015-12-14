////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxyTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_CliTester_AppProxyTester_h
#define Omn_CliTester_AppProxyTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class OmnAosAppProxyTester : public OmnTestPkg
{
private:

public:
	OmnAosAppProxyTester()
	{
		mName = "OmnAosAppProxyTester";
	}
	~OmnAosAppProxyTester() {}

	virtual bool		start();

private:
	bool	testCli();
	bool 	checkOneProxy(char *file, int line, const OmnString &cmd, 
						  char *name, 
						  char *dn, 
						  char *addr, 
						  int port, 
						  int protocol, 
						  int security, 
						  int status, 
						  int group, 
						  int servers);
	bool 	checkAaaServer(const OmnString &cmd, 
						char *appname, 
						char *aaaname, 
						int priority, 
						int refcnt);
};
#endif

