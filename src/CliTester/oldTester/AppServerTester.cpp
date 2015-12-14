////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppServerTester.cpp
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
#include "CliTester/AppServerTester.h"

#include "Servers/AppServer.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool OmnAosAppServerTester::start()
{
	// 
	// Test default constructor
	//
	testCli();
	return true;
}


bool OmnAosAppServerTester::testCli()
{
	OmnString rslt;
	OmnString cmd;

	OmnBeginTest << "Test AppServer CLI";
	mTcNameRoot = "Test_CLI";

	// 
	// Clear all
	//
	aosRunCli("app server clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAppServer_getNumServers())) << endtc;
	
	// 
	// Create "app1"
	//
	aosRunCli(cmd = "app server create app1 ftp 192.168.0.81 10.10.10.10 7000 TCP", true);
	checkOneServer(cmd, "app1", "ftp", "10.10.10.10", 7000, "TCP", 0);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Create "app2"
	//
	aosRunCli(cmd = "app server create app2 telnet 192.168.0.81 10.10.10.11 8000 TCP", true);
	checkOneServer(cmd, "app2", "telnet", "10.10.10.11", 8000, "TCP", 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Change the type to "telnet-s"
	//
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 10.10.10.11 8000 TCP", true);
	checkOneServer(cmd, "app2", "telnet-s", "10.10.10.11", 8000, "TCP", 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Change address to '192.168.1.2'
	//
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 192.168.1.2 8000 TCP", true);
	checkOneServer(cmd, "app2", "telnet-s", "192.168.1.2", 8000, "TCP", 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Change port to 9000
	//
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	checkOneServer(cmd, "app2", "telnet-s", "192.168.1.2", 9000, "TCP", 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Remove app1
	// 
	aosRunCli(cmd = "app server remove app1", true);
	checkOneServer(cmd, "app2", "telnet-s", "192.168.1.2", 9000, "TCP", 0);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Remove app2
	// 
	aosRunCli(cmd = "app server remove app2", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Add app1, app2, clear all
	// 
	aosRunCli(cmd = "app server create app1 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppServer_getNumServers())) << endtc;
	aosRunCli(cmd = "app server clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAppServer_getNumServers())) << endtc;

	// 
	// Add app1, app2, check the refcnt
	// 
	aosRunCli(cmd = "app server create app1 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	aosAppServer *server = aosAppServer_get("app1");
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;

	// 
	// Check put and hold
	//
	aosAppServer_put(server);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
	aosAppServer_hold(server);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;

	// 
	// Since 'server' is holding a reference, we should not be able to remove 'app1'
	//
	aosRunCli("app server remove app1", false);
	aosRunCli("app server clear all", false);
 
	// 
	// Release the refcnt, then remove
	//
	aosAppServer_put(server);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
	aosRunCli("app server remove app1", true);
	aosRunCli("app server clear all", true);
		
	// 
	// Add app1, app2, hold a ref, clear should fail
	// 
	aosRunCli(cmd = "app server create app1 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	aosRunCli(cmd = "app server create app2 telnet-s 192.168.0.81 192.168.1.2 9000 TCP", true);
	server = aosAppServer_get("app1");
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
	aosRunCli("app server clear all", false);

	// 
	// Release the reference, clear should ok
	//
	aosAppServer_put(server);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
	aosRunCli("app server clear all", true);
	
	return true;
}


bool OmnAosAppServerTester::checkOneServer(const OmnString &cmd, 
										 char *name, 
										 char *type, 
										 char *addr, 
										 int port, 
										 char *protocol, 
										 int status) 
{
	aosAppServer *app = aosAppServer_get(name);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << cmd << endtc;
	if (!app)
	{
		return false;
	}

	OmnTC(OmnExpected<OmnString>(name), OmnActual<OmnString>(app->mName)) << endtc;
	OmnTC(OmnExpected<OmnString>(type), OmnActual<OmnString>(app->mType)) << endtc;
	OmnTC(OmnExpected<int>(OmnIpAddr(addr).toInt()), OmnActual<int>(app->mRemoteAddr)) << endtc;
	OmnTC(OmnExpected<int>(port), OmnActual<int>(app->mRemotePort)) << endtc;
	OmnTC(OmnExpected<OmnString>(protocol), OmnActual<OmnString>(app->mProtocol)) << endtc;
	OmnTC(OmnExpected<int>(status), OmnActual<int>(app->mStatus)) << endtc;

	aosAppServer_put(app);
	return true;
}

*/

