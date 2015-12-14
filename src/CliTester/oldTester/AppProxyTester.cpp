////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxyTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
#include "CliTester/AppProxyTester.h"

 * We will not test AppProxy for now.
 * Chen Ding, 08/25/2005
 *
#include "AAA/AaaServer.h"
#include "AppProxy/AppProxy.h"

#include <KernelSimu/in.h>
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "KernelUtil/KernelDebug.h"
#include "Servers/ServerGroup.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"



bool OmnAosAppProxyTester::start()
{
	// 
	// Test default constructor
	//
	testCli();
	return true;
}


bool OmnAosAppProxyTester::testCli()
{
	OmnString rslt;
	bool rt;
	OmnString cmd;
	aosAppProxy *app;

	OmnBeginTest << "Test AppProxy CLI";
	mTcNameRoot = "Test_CLI";

	// 
	// Clear all
	//
	aosRunCli("app proxy clear all", true);
	
	// 
	// Create app1
	//
	aosRunCli("app proxy add app1 dn1 10.1.1.1 5000 TCP ssl", true);
	
	// 
	// Create app2
	//
	aosRunCli("app proxy add app2 dn1 10.1.1.2 5000 TCP ssl", true);

	// 	
	// The same command should be ok
	//
	aosRunCli("app proxy add app2 dn1 10.1.1.2 5000 TCP ssl", true);

	// 	
	// Modify dn to "dn2"
	//
	aosRunCli("app proxy add app2 dn2 10.1.1.2 5000 TCP ssl", true);
	checkOneProxy(aosFileLine, cmd, "app2", "dn2", "10.1.1.2", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);

	// 
	// Modify the port to 6000
	//
	aosRunCli("app proxy add app2 dn1 10.1.1.2 6000 TCP ssl", true);
	checkOneProxy(aosFileLine, cmd, "app2", "dn1", "10.1.1.2", 6000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);

	// 
	// Modify the security
	//
	aosRunCli("app proxy add app2 dn1 10.1.1.2 6000 TCP ipsec", true);
	checkOneProxy(aosFileLine, cmd, "app2", "dn1", "10.1.1.2", 6000, IPPROTO_TCP, eAosAppProxy_IPSec, 0, 0, 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// Remove app1
	//
	aosRunCli("app proxy remove app1", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;
	app = aosAppProxy_get("app1");
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)app)) << endtc;
	
	// 
	// Clear all
	//
	aosRunCli("app proxy clear all", true);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// Add "app1" again
	//
	aosRunCli("app proxy add app1 dn1 10.1.1.1 5000 TCP ssl", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);

	// 
	// Add "app2" again
	//
	aosRunCli("app proxy add app2 dn1 10.1.1.2 5000 TCP ssl", true);
	checkOneProxy(aosFileLine, cmd, "app2", "dn1", "10.1.1.2", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// Turn app1 on
	//
	cmd = "app proxy set status app1 on";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 1, 0, 0);

	// 
	// Turn app1 off 
	//
	cmd = "app proxy set status app1 off";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(rt)) << cmd << endtc;
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// Create two AAA servers
	//
	cmd = "aaa server add aaa1 RADIUS 10.1.1.1 5000 icmp";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);
	cmd = "aaa server add aaa2 LDAP   10.1.1.2 5000 icmp";
	rt = OmnCliProc::getSelf()->runCli(cmd, rslt);

	// 
	// Add aaa1 and aaa2 to app1
	//
	aosRunCli(cmd = "app proxy add aaa server app1 aaa1 1000", true);
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", 
		"10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 1);
	checkAaaServer(cmd, "app1", "aaa1", 1000, 2);
	aosRunCli(cmd = "app proxy add aaa server app1 aaa2 200", true);
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", 
		"10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 2);
	checkAaaServer(cmd, "app1", "aaa1", 1000, 2);
	checkAaaServer(cmd, "app1", "aaa2", 200, 2);

	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	//
	// Since app1 holds aaa1 and aaa2, we should not be able to remove these two 
	// AAA servers.
	//
	aosRunCli(cmd = "aaa server remove aaa1", false);
	aosRunCli(cmd = "aaa server remove aaa2", false);
	aosRunCli(cmd = "aaa server clear all", false);
	
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// Remove app1. This should release the AAA servers. 
	// 
	aosRunCli(cmd = "app proxy remove app1", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;

	// 
	// There should be only one reference to aaa1 and aaa2 now
	//
	aosAaaServer *aaa = aosAaaServer_get("aaa1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(aaa != 0)) << endtc;
	if (aaa)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&aaa->refcnt))) << endtc;
		aosAaaServer_put(aaa);
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&aaa->refcnt))) << endtc;
	}

	aaa = aosAaaServer_get("aaa2");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(aaa != 0)) << endtc;
	if (aaa)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&aaa->refcnt))) << endtc;
		aosAaaServer_put(aaa);
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&aaa->refcnt))) << endtc;
	}
	
	aosRunCli(cmd = "aaa server remove aaa1", true);
	aosRunCli(cmd = "aaa server remove aaa2", true);
	OmnTC(OmnExpected<int>(1), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;
	
	// 
	// Add "app1" again
	//
	aosRunCli("app proxy add app1 dn1 10.1.1.1 5000 TCP ssl", true);
	OmnTC(OmnExpected<int>(2), OmnActual<int>(aosAppProxy_getNumAppProxies())) << endtc;
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 0);
	app = aosAppProxy_get("app1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
	if (app)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&app->refcnt))) << endtc;
		aosAppProxy_put(app); app = 0;
	}
	

	// 
	// Create two AAA servers
	//
	aosRunCli("aaa server add aaa1 RADIUS 10.1.1.1 5000 icmp", true);
	aosRunCli("aaa server add aaa2 LDAP   10.1.1.2 5000 icmp", true);

	// 
	// Add aaa1 to app1
	//
	aosRunCli("app proxy add aaa server app1 aaa1 1000", true);
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 1);
	checkAaaServer(cmd, "app1", "aaa1", 1000, 2);
	
	// 
	// Add aaa2 to app1
	//
	aosRunCli("app proxy add aaa server app1 aaa2 200", true);
	checkOneProxy(aosFileLine, cmd, "app1", "dn1", "10.1.1.1", 5000, IPPROTO_TCP, eAosAppProxy_SSL, 0, 0, 2);

	// 
	// Create two app servers
	//
	aosRunCli("app server create server1 telnet-s 192.168.0.82 192.168.1.2 9000 TCP", true);
	aosRunCli("app server create server2 telnet-s 192.168.0.82 192.168.2.2 7000 TCP", true);

	// 
	// Create two server groups
	//
	aosRunCli("server group add group1 rr", true);
	aosRunCli("server group add group2 rr", true);
	aosServerGroup *group = aosServerGroup_get("group1");
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(group != 0)) << endtc;
	if (group)
	{
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
	
		// 
		// Add server1 and server2 to group1
		//
		aosRunCli("server group member add group1 server1 500", true);
		aosRunCli("server group member add group1 server2 500", true);
	
		app = aosAppProxy_get("app1");
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&app->refcnt))) << endtc;
		aosAppProxy_put(app); app = 0;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;

		// 
		// Associate group1 with app1
		//
		aosRunCli("app proxy set server group app1 group1", true);
		app = aosAppProxy_get("app1");
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&app->refcnt))) << endtc;
		aosAppProxy_put(app); app = 0;
		OmnTC(OmnExpected<int>(3), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;

		// 
		// Remove group1 should fail
		//
		aosRunCli("server group remove group1", false);
		app = aosAppProxy_get("app1");
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&app->refcnt))) << endtc;
		aosAppProxy_put(app); app = 0;
		OmnTC(OmnExpected<int>(3), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
	
		// 
		// Reset group and then remove group1 should be okey
		//
		aosRunCli("app proxy reset server group app1", true);
		app = aosAppProxy_get("app1");
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&app->refcnt))) << endtc;
		OmnTC(OmnExpected<int>(2), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
		aosServerGroup_put(group); 
		OmnTC(OmnExpected<int>(1), OmnActual<int>(atomic_read(&group->mRefcnt))) << endtc;
		aosAppProxy_put(app); app = 0;
		aosRunCli("server group remove group1", true);
	}

	// 
	// Clear app proxy, clear server group, clear server, clear aaa server
	//
	aosRunCli("app proxy clear all", true);
	aosRunCli("server group clear all", true);
	aosRunCli("app server clear all", true);
	aosRunCli("aaa server clear all", true);
	
	// Create app1 and app2
	aosRunCli("app proxy add app1 dn1 10.1.1.1 5000 TCP ssl", true);
	aosRunCli("app proxy add app2 dn1 10.1.1.2 5000 TCP ssl", true);

	// Create an AAA server: LDAP-A
	aosRunCli("aaa server add LDAP-A LDAP 192.168.0.81 4000 icmp", true);

	// Add LDAP-A to app1
	aosRunCli("app proxy add aaa server app1 LDAP-A 500", true);

	// Create application server server-a
	aosRunCli("app server create server-a SMTP 192.168.0.82 192.168.0.168 6600 TCP", true);

	// Create application server server-b
	aosRunCli("app server create server-b SMTP 192.168.0.82 192.168.0.168 7700 TCP", true);

	// Create server group: group1
	aosRunCli("server group add group1 rr", true);

	// Add server-a to group1
	aosRunCli("server group member add group1 server-a 300", true);

	// Associate group1 to app1
	aosRunCli("app proxy set server group app1 group1", true);

	// Turn app1 on
	aosRunCli("app proxy set status app1 on", true);

	// Show conn
	aosRunCli("app proxy conn show", true);

	// 
	// Clear app proxy, clear server group, clear server, clear aaa server
	//
	aosRunCli("app proxy clear all", true);
	aosRunCli("server group member remove group1 server-a", true);
	aosRunCli("server group clear all", true);
	aosRunCli("app server clear all", true);
	aosRunCli("aaa server clear all", true);
	
	return true;
}


bool OmnAosAppProxyTester::checkOneProxy(char *file, int line, const OmnString &cmd, 
										 char *name, 
										 char *dn, 
										 char *addr, 
										 int port, 
										 int protocol, 
										 int security, 
										 int status, 
										 int group,
										 int servers)
{
	OmnString errmsg;
	errmsg << "<" << file << ":" << line << "> " << cmd;

	aosAppProxy *app = aosAppProxy_get(name);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << errmsg << endtc;
	if (!app)
	{
		return false;
	}

	OmnTC(OmnExpected<OmnString>(name), OmnActual<OmnString>(app->name)) << errmsg << endtc;
	OmnTC(OmnExpected<OmnString>(dn), OmnActual<OmnString>(app->dn)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(OmnIpAddr(addr).toInt()), OmnActual<int>(app->addr)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(port), OmnActual<int>(app->port)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(protocol), OmnActual<int>(app->protocol)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(security), OmnActual<int>(app->security)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(status), OmnActual<int>(app->status)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(group), OmnActual<int>((int)app->group)) << errmsg << endtc;
	OmnTC(OmnExpected<int>(servers), OmnActual<int>(app->aaaServers.qlen)) << errmsg << endtc;

	aosAppProxy_put(app);
	return true;
}


bool OmnAosAppProxyTester::checkAaaServer(const OmnString &cmd, 
										char *appname, 
										char *aaaname, 
										int priority, 
										int refcnt)
{
	aosAppProxy *app = aosAppProxy_get(appname);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(app != 0)) << endtc;
	aosAaaServer *aaa = aosAaaServer_get(aaaname);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(aaa != 0)) << endtc;

	if (!app || !aaa)
	{
		return false;
	}

	bool found = false;
	
	aosAaaServerEntry *a = (aosAaaServerEntry *)app->aaaServers.next;
	while (a != (aosAaaServerEntry *)&app->aaaServers)
	{
		if (a->server == aaa)
		{
			found = true;
			break;
		}

		a = (aosAaaServerEntry *)a->next;
	}

	aosAppProxy_put(app);

	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(found)) << endtc;
	OmnTC(OmnExpected<int>(priority), OmnActual<int>(a->priority)) << endtc;
	OmnTC(OmnExpected<int>(refcnt+1), OmnActual<int>(atomic_read(&a->server->refcnt))) << endtc;

	aosAaaServer_put(aaa);
	return found;
}

*/
