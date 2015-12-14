////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include "aos/aosKernelAlarm.h"
#include <sys/file.h>
#include "Alarm/Alarm.h"
#include "AppMgr/App.h"
#include "Debug/Debug.h"
#include "KernelAPI/UserLandWrapper/TestTcpApiU.h"
#include "KernelSimu/KernelSimu.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonMgr.h"
#include "Util/OmnNew.h"
#include "Util/IpAddr.h"
#include "KernelConfMgr/Global.h"
#include "KernelConfMgr/SecuredShellCli.h"
#include "ParentCtrl/ParentCtrl.h"
#include "KernelConfMgr/SystemSaveConfig.h"

#include "rhcUtil/aosResMgrToolkit.h"

static char *appname[16] = {"system"};

int 
main(int argc, char **argv)
{
	/* check the privilege */
	if (getuid() != 0)
	{
		cout << "*****Root privilege needed. Quit" << endl;
		OmnAlarm << "Root privilege needed. Quit" << enderr;
		return 0;
	}
		
	/* Daemon mode */

	if (argc==1)
	{
		daemon(1, 0);

	}

    /* First create a socket */
	OmnApp theApp(argc, argv);
	try
	{
		theApp.startSingleton(OmnNew OmnCliUtilProcSingleton());
	}

	catch (const OmnExcept &e)
	{
		cout << "****** Failed to start the application: " << e.toString() << endl;
		OmnAlarm << "Failed to start the application: " << e.toString() << enderr;
		return 0;
	}
	
	/* if port is 0, it will choose a port from cmd.txt */
	if (CliUtil_initCli((char**)appname, 1, 0) < 0)
	{
		cout << "****** Failed to start the application: can't init CLI " << endl;
		OmnAlarm << "Failed to start the application: " << enderr;
		theApp.exitApp();
		return -1;
	}
	
	//
	// init the glabol structure
	//
	systemInit();
	pppoeInit();
	dnsproxyInit();
	dhcpInit();	
	macCloningInit();	
	fwInit();
	bridgeInit();
	vlanInit();
	dnsmasqInit();
	dmzInit();
	routerInit();
	pptpInit();

	// register every CLI command implementation functions	

	/*Arp Proxy Cli*/
	ArpProxy_regCliCmd();
	
	/*DHCP Cli*/
	Dhcp_regCliCmd();
	
	/*DNS proxy Cli*/
	DnsProxy_regCliCmd();

	/*Secured Shell Cli*/
	SecuredShell_regCliCmd();
	
	/*Secure Authentication Cli*/
	SecureAuth_regCliCmd();
	
	/*PPPoE Cli*/
	Pppoe_regCliCmd();
	
	/*Mac Clone Cli*/
	MacClone_regCliCmd();

	/*Firewall Cli*/
	FwBlacklist_regCliCmd();
	FwConfig_regCliCmd();
	FwFilter_regCliCmd();
	FwNat_regCliCmd();
	
	// 2006-12-27 By CHK
	// Append "bridge config" commands
	Bridge_regCliCmd();
	
	// 2006-12-27 By CHK
	// Append "CPU Manager" commands
	CpuMgr_regCliCmd();
	
	// 2007-01-04 By CHK
	// Append "Memory Manager" commands
	MemMgr_regCliCmd();
	
	// 2007-01-11 By CHK
	// Append "Disk Manager" commands
	DiskMgr_regCliCmd();
	
	// 2006-03-09 By CHK
	// Append "Resource Manager" commands
	ResMgr_regCliCmd();

	// 2007-01-19 By CHK
	// initialize data for sgSnmpEntryPtr
	sgSnmpEntryPtr = NULL;
	// Append "snmp agent" commands
	Snmp_regCliCmd();
	
	// 2007-01-11 By XYB
	// Append "Disk Quota Manager" commands
	QuotaMgr_regCliCmd();

	//2007-04-10 by xyb
	//ros os cli command
	RosOS_regCliCmd();
	
	/*System config Cli*/
	SystemConfig_regCliCmd();

	/*register Parent Control moudle CLI*/
	ParentCtrl_regCliCmd();

	/*Router statistics CLI*/
	RouterStat_regCliCmd();
	
	/*Wan connection manager Cli*/	
	WanConnectionMgr_regCliCmd();

	/*Firewall VPN pass Cli*/
	FwVpnPass_regCliCmd();
	
	Vlan_regCliCmd();
	
	/*Dnsmasq Cli*/
	Dnsmasq_regCliCmd();
	
	/*dmz Cli*/
	Dmz_regCliCmd();
	
	/*router Cli*/
	Router_regCliCmd();

	Pptp_regCliCmd();
	
	SystemMisc_regCliCmd();
	
	SystemTime_regCliCmd();
	/* system cli register*/
	system_save_config_register_clis();
	
	theApp.appLoop();
	theApp.exitApp();

	return 0;
} 

