////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AaaServerTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
 * We will test it later
 * Chen Ding, 08/25/2005
 *
#include "CliTester/AaaServerTester.h"

#include "AAA/AaaServer.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool OmnAosAaaServerTester::start()
{
	// 
	// Test default constructor
	//
	testCli();
	return true;
}


bool OmnAosAaaServerTester::testCli()
{
	OmnString rslt;
	OmnString cmd;

	OmnBeginTest << "Test AaaServer CLI";
	mTcNameRoot = "Test_CLI";

	// 
	// Clear all
	//
	aosRunCli("aaa server clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;
	
	// 
	// Create "server1"
	//
	aosRunCli(cmd = "aaa server add server1 RADIUS 10.1.1.1 5000 icmp", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;
	checkOneServer(cmd, "server1", "RADIUS", "10.1.1.1", 5000, "icmp", 0, 1, __FILE__, __LINE__);
	
	// 
	// Create "server2"
	//
	aosRunCli(cmd = "aaa server add server2 LDAP 10.10.10.10 6000 icmp", true);
	checkOneServer(cmd, "server2", "LDAP", "10.10.10.10", 6000, "icmp", 0, 1, __FILE__, __LINE__);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 
	// Recreated. Should be okey 
	//
	aosRunCli(cmd = "aaa server add server2 LDAP 10.10.10.10 6000 icmp", true);
	checkOneServer(cmd, "server2", "LDAP", "10.10.10.10", 6000, "icmp", 0, 1, __FILE__, __LINE__);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 	
	// Modify server2's type to "RADIUS"
	//
	aosRunCli(cmd = "aaa server add server2 RADIUS 10.10.10.10 6000 icmp", true);
	checkOneServer(cmd, "server2", "RADIUS", "10.10.10.10", 6000, "icmp", 0, 1, __FILE__, __LINE__);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 	
	// Modify server2's address to "10.10.20.20"
	//
	aosRunCli(cmd = "aaa server add server2 RADIUS 10.10.20.20 6000 icmp", true);
	checkOneServer(cmd, "server2", "RADIUS", "10.10.20.20", 6000, "icmp", 0, 1, __FILE__, __LINE__);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 	
	// Modify server2's port to 9000
	//
	aosRunCli(cmd = "aaa server add server2 RADIUS 10.10.20.20 9000 icmp", true);
	checkOneServer(cmd, "server2", "RADIUS", "10.10.20.20", 9000, "icmp", 0, 1, __FILE__, __LINE__);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 
	// Remove server1 
	//
	aosRunCli(cmd = "aaa server remove server1", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;
	aosAaaServer *server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server == 0)) << endtc;
	server = aosAaaServer_get("server2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
	}
	
	// 
	// Remove server2 
	//
	aosRunCli(cmd = "aaa server remove server2", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;
	server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)server)) << endtc;
	server = aosAaaServer_get("server2");
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)server)) << endtc;

	// 
	// Create "server1" and "server2" again
	//
	aosRunCli(cmd = "aaa server add server1 RADIUS 10.1.1.1 5000 icmp", true);
	aosRunCli(cmd = "aaa server add server2 RADIUS 10.1.1.2 5000 icmp", true);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 
	// Clear all
	//
	aosRunCli(cmd = "aaa server clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 
	// Create "server1" and "server2" again
	//
	aosRunCli(cmd = "aaa server add server1 RADIUS 10.1.1.1 5000 icmp", true);
	aosRunCli(cmd = "aaa server add server2 RADIUS 10.1.1.2 5000 icmp", true);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAaaServer_getNumServers())) << endtc;

	// 
	// Turn server1 on
	//
	aosRunCli(cmd = "aaa server set status server1 on", true);
	checkOneServer(cmd, "server1", "RADIUS", "10.1.1.1", 5000, "icmp", 1, 1, __FILE__, __LINE__);

	// 
	// Turn server1 off 
	//
	aosRunCli(cmd = "aaa server set status server1 off", true);
	checkOneServer(cmd, "server1", "RADIUS", "10.1.1.1", 5000, "icmp", 0, 1, __FILE__, __LINE__);

	// 
	// Reset statistics
	//
	server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		server->connAttempts = 100;
		server->successConns = 200;
		server->numRequests = 300;
		server->numAcceptedReqs = 400;
	}

	aosRunCli(cmd = "aaa server stat reset server1", true);
	checkOneServer(cmd, "server1", "RADIUS", "10.1.1.1", 5000, "icmp", 0, 1, __FILE__, __LINE__);
	server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(server->connAttempts)) << cmd << endtc;
	}

	server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		server->connAttempts = 100;
		server->successConns = 200;
		server->numRequests = 300;
		server->numAcceptedReqs = 400;
	}

	server = aosAaaServer_get("server2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		server->connAttempts = 1100;
		server->successConns = 1200;
		server->numRequests = 1300;
		server->numAcceptedReqs = 1400;
	}

	aosRunCli(cmd = "aaa server stat clear all", true);
	checkOneServer(cmd, "server1", "RADIUS", "10.1.1.1", 5000, "icmp", 0, 1, __FILE__, __LINE__);
	server = aosAaaServer_get("server1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->refcnt))) << cmd << endtc;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(server->connAttempts)) << cmd << endtc;
	}

	server = aosAaaServer_get("server2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		aosAaaServer_put(server);
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->refcnt))) << cmd << endtc;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(server->successConns)) << cmd << endtc;
	}

	// 
	// Clear all
	//
	aosRunCli("aaa server clear all", true);
	
	return true;
}


bool OmnAosAaaServerTester::checkOneServer(const OmnString &cmd, 
										 char *name, 
										 char *type, 
										 char *addr, 
										 int port, 
										 char *hc, 
										 int status, 
										 int refcnt, 
										 char *filename, 
										 int line)
{
	OmnString msg;
	msg << filename << ":" << line << ": " << cmd;
	struct aosAaaServer *app = aosAaaServer_get(name);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << cmd << endtc;
	if (!app)
	{
		return false;
	}

	OmnTC(OmnExpected<OmnString>(name), OmnActual<OmnString>(app->name)) << msg << endtc;
	OmnTC(OmnExpected<OmnString>(type), OmnActual<OmnString>(app->type)) << msg << endtc;
	OmnTC(OmnExpected<int>(OmnIpAddr(addr).toInt()), OmnActual<int>(app->addr)) << msg << endtc;
	OmnTC(OmnExpected<int>(port), OmnActual<int>(app->port)) << msg << endtc;
	OmnTC(OmnExpected<OmnString>(hc), OmnActual<OmnString>(app->hc)) << msg << endtc;
	OmnTC(OmnExpected<int>(status), OmnActual<int>(app->status)) << msg << endtc;
	OmnTC(OmnExpected<int>(refcnt+1), OmnActual<int>(atomic_read(&app->refcnt))) << msg << endtc;

	aosAaaServer_put(app);
	return true;
}

*/

