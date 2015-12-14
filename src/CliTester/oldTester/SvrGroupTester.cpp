////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SvrGroupTester.cpp
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
#include "CliTester/SvrGroupTester.h"

#include "Servers/ServerGroup.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/AppServer.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool OmnAosSvrGroupTester::start()
{
	// 
	// Test default constructor
	//
	testCli();
	return true;
}


bool OmnAosSvrGroupTester::testCli()
{
	OmnString rslt;
	bool rt;
	OmnString cmd;
	int ret;

	OmnBeginTest << "Test SvrGroup CLI";
	mTcNameRoot = "Test_CLI";

	cmd = "app server clear all";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);

	// 
	// Clear all
	//
	aosRunCli("server group clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosServerGroup_getNumGroups())) << endtc;
	
	// 
	// Create group1
	//
	aosRunCli("server group add group1 rr", true);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	checkOneGroup(aosFileLine, cmd, "group1", eAosServerGroupMethod_RR);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosServerGroup_getNumGroups())) << endtc;

	// 
	// Create group2
	//
	aosRunCli("server group add group2 rr", true);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	checkOneGroup(aosFileLine, cmd, "group2", eAosServerGroupMethod_RR);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosServerGroup_getNumGroups())) << endtc;

	// 
	// Remove group1
	// 
	cmd = "server group remove group1";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	checkOneGroup(aosFileLine, cmd, "group2", eAosServerGroupMethod_RR);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosServerGroup_getNumGroups())) << endtc;

	// 
	// Remove group2
	// 
	cmd = "server group remove group2";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosServerGroup_getNumGroups())) << endtc;

	// 
	// Create server "app1"
	// 
	aosRunCli("app server create app1 telnet-s 192.168.0.82 192.168.1.2 9000 TCP", true);
	aosAppServer *server = aosAppServer_get("app1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;
	}
	aosAppServer_put(server);

	// 
	// Create server "app2"
	// 
	aosRunCli("app server create app2 telnet-s 192.168.0.82 192.168.1.2 9000 TCP", true);
	server = aosAppServer_get("app2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	if (server)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;
	}
	aosAppServer_put(server);
	server = 0;

	// Create group1 and group2
	aosRunCli("server group add group1 rr", true);
	aosRunCli("server group add group2 rr", true);
	aosServerGroup *group = aosServerGroup_get("group1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(group != 0)) << endtc;
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
	aosServerGroup_put(group);
	group = 0;

	group = aosServerGroup_get("group2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(group != 0)) << endtc;
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
	aosServerGroup_put(group);
	group = 0;

	// Add 'app1' to group1
	server = aosAppServer_get("app1");
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;
	aosRunCli("server group member add group1 app1 500", true);
	OmnTC(OmnExpected<int>(3), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;

	// Add 'app2' to group1
	aosRunCli("server group member add group1 app2 600", true);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;

	// Get the member
	// group = aosServerGroup_get("group1");
	// ret = aosServerGroup_getNextServer(group, &server);
	// OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
//	if (server)
//	{
//		aosAppServer_put(server);
//		server = 0;
//	}

//	aosServerGroup_put(group);
//	group = 0;

	// 
	// Remove app1 should fail
	//
	cmd = "app server remove app1";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(false), OmnActual<bool>(rt)) << cmd << endtc;
	OmnTC(OmnExpected<int>(3), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;
	aosAppServer_put(server);
	server = 0;

	// 
	// But if it is removed from the group, we should be able to remove app1
	//
	aosRunCli("server group member remove group1 app1", true);

	server = aosAppServer_get("app1");
	OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << endtc;
	aosAppServer_put(server);
	server = 0;

	// Now we should be able to remove the app1
	aosRunCli("app server remove app1", true);

	server = aosAppServer_get("app1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server == 0)) << cmd << endtc;
		

	// 
	// Clear app server should fail since app2 is still held by group1
	//
	cmd = "app server clear all";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(false), OmnActual<bool>(rt)) << cmd << endtc;
	server = aosAppServer_get("app2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << cmd << endtc;
	if (server)
	{
		OmnTC(OmnExpected<int>(3), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;

		// 
		// Remove app2 from group1, and clear all app server should success
		//
		cmd = "server group member remove group1 app2";
		rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
		aosAppServer_put(server);
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&server->mRefcnt))) << cmd << endtc;
		server = 0;
	}

	aosRunCli("app server clear all", true);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	server = aosAppServer_get("app1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server == 0)) << cmd << endtc;
	server = aosAppServer_get("app2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server == 0)) << cmd << endtc;

	// 
	// Clear all groups
	//
	aosRunCli("server group clear all", true);
	aosRunCli("app server clear all", true);

	// Create gruop1, server1, add server1 to group1
	aosRunCli("server group add group1 rr", true);
	aosRunCli("app server create server1 telnet-s 192.168.0.82 192.168.1.2 9000 TCP", true);
	aosRunCli("server group member add group1 server1 500", true);
	group = aosServerGroup_get("group1");

	aosAppServer *server1 = aosAppServer_get("server1");

	ret = aosServerGroup_getNextServer(group, &server);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server != 0)) << endtc;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(server == server1)) << endtc;

	// Remove server1 from group1
	aosRunCli("server group member remove group1 server1", true);

	aosServerGroup_put(group);
	group = 0;
	aosAppServer_put(server1);
	server1 = 0;

	// 
	// Clear all groups
	//
	aosRunCli("server group clear all", true);
	aosRunCli("app server clear all", true);
	
	return true;
}


bool OmnAosSvrGroupTester::checkOneGroup(char *file, int line, const OmnString &cmd, 
										 char *name, 
										 int method) 
{
	OmnString errmsg;
	errmsg << "<" << file << ":" << line << "> " << cmd;

	aosServerGroup *group = aosServerGroup_get(name);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(group != 0)) << errmsg << endtc;
	if (!group)
	{
		return false;
	}
	OmnTC(OmnExpected<int>(method), OmnActual<int>(group->mMethod)) << errmsg << endtc;

	aosServerGroup_put(group);
	return true;
}

*/

