////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Cli.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef AOS_OLD_CLI
#ifndef aos_KernelInterface_Cli_h
#define aos_KernelInterface_Cli_h

#include "aosUtil/Alarm.h"
#include "Util/Ptrs.h"
#include "Util/String.h"
#include "rhcInclude/sqlite3.h"
#include "aosUtil/List.h"  

enum 
{
	eAosAlarm_FailedToLoadConfig = eAosAlarm_CLIStart+1
};

class AosCli : public OmnRCObject
{
	OmnDefineRCObject;


	private:
	OmnString		mFileName;
	OmnFilePtr		mFile;

	public:
	AosCli(const OmnString &fn);
	AosCli();
	~AosCli(){}
	int 	saveConfig(OmnString &rslt);
	int 	loadConfig(OmnString &rslt);
	int		clearConfig(OmnString &rslt);

	int		clearConfig(const char *moduleName, OmnString &rslt);

	//kevin 07/03/2006
	int 	saveFwConfig(OmnString &rslt);
	int 	saveOnlyFwConfig(OmnString &rslt);
	int 	loadFwConfig(OmnString &rslt);
	int		clearFwConfig(OmnString &rslt);
	int		showFwConfig(OmnString &rslt);
	int		showFwLog(OmnString &rslt);
	int		initFwConfig(OmnString &rslt);
	int		initFwMaxsecConfig(OmnString &rslt);
	int		initFwMidsecConfig(OmnString &rslt);
	int		initFwMinsecConfig(OmnString &rslt);
	int		fwRunCommand(OmnString &cmd, OmnString &rslt);
	int		fwRules(OmnString &rslt);

	int		fwTimefil(OmnString &rslt);
	int		fwDelTimefil(OmnString &rslt);

	int		fwIpfilAll(OmnString &rslt);
	int		fwIpfilTcp(OmnString &rslt);
	int		fwIpfilUdp(OmnString &rslt);
	int		fwIpfilIcmp(OmnString &rslt);
	int		fwIpfilStateNew(OmnString &rslt);
	int		fwIpfilStateAck(OmnString &rslt);

	int		fwMacfilAll(OmnString &rslt);
	int		fwMacfilTcp(OmnString &rslt);
	int		fwMacfilUdp(OmnString &rslt);
	int		fwMacfilIcmp(OmnString &rslt);

	int		fwNatSnat(OmnString &rslt);
	int		fwNatDnatPort(OmnString &rslt);
	int		fwNatDnatIP(OmnString &rslt);
	int		fwNatMasq(OmnString &rslt);
	int		fwNatRedi(OmnString &rslt);

	int		fwIpfilDelAll(OmnString &rslt);
	int		fwIpfilDelTcp(OmnString &rslt);
	int		fwIpfilDelUdp(OmnString &rslt);
	int		fwIpfilDelIcmp(OmnString &rslt);
	int		fwIpfilDelStateNew(OmnString &rslt);
	int		fwIpfilDelStateAck(OmnString &rslt);

	int		fwMacfilDelAll(OmnString &rslt);
	int		fwMacfilDelTcp(OmnString &rslt);
	int		fwMacfilDelUdp(OmnString &rslt);
	int		fwMacfilDelIcmp(OmnString &rslt);

	int		fwNatDelSnat(OmnString &rslt);
	int		fwNatDelDnatPort(OmnString &rslt);
	int		fwNatDelDnatIP(OmnString &rslt);
	int		fwNatDelMasq(OmnString &rslt);
	int		fwNatDelRedi(OmnString &rslt);

	int		fwAntiSyn(OmnString &rslt);
	int		fwAntiIcmp(OmnString &rslt);
	int		fwAntiDelSyn(OmnString &rslt);
	int		fwAntiDelIcmp(OmnString &rslt);

	int		fwBlacklistLoadFile(OmnString &rslt);
	int		fwBlacklistLoadUrl(OmnString &rslt);
	int		fwBlacklistRemoveFile(OmnString &rslt);
	int		fwBlacklistRemoveUrl(OmnString &rslt);


	int		webwallRedi(OmnString &rslt);
	int		webwallDelRedi(OmnString &rslt);
	//BinGong 11/14/2006

	//kevin 07/03/2006
	int		saveSystemConfig(OmnString &rslt);
	int		loadSystemConfig(OmnString &rslt);
	int		clearSystemConfig(OmnString &rslt);
	int		showSystemConfig(OmnString &rslt);
	int		systemRunCommand(OmnString &cmd, OmnString &rslt);

	int 	systemUname(OmnString &rslt);
	int 	systemArpProxyAdd(OmnString &rslt);
	int 	systemArpProxyDel(OmnString &rslt);
	int 	systemIpAddress(OmnString &rslt);
	int 	systemDns(OmnString &rslt);
	int 	systemRouteCommon(OmnString &rslt);
	int 	systemRouteDefault(OmnString &rslt);
	int 	systemDelRouteCommon(OmnString &rslt);
	int 	systemDelRouteDefault(OmnString &rslt);
	int 	systemRouteShow(OmnString &rslt);
	int 	systemDevRoute(OmnString &rslt);
	int 	systemDelDevRoute(OmnString &rslt);

	//xia
	int 	systemUpdate(OmnString &rslt);

	//dxr
	int 	dhcpStart(OmnString &rslt);
	int 	dhcpStop(OmnString &rslt);
	int 	dhcpServerShowConfig(OmnString &rslt);
	int 	dhcpServerLeaseTimeSet(OmnString &rslt);
	int 	dhcpServerRouterSet(OmnString &rslt);
	int 	dhcpServerDnsSet(OmnString &rslt);
	int 	dhcpServerIpBlock(OmnString &rslt);
	int 	dhcpServerAddBind(OmnString &rslt);
	int 	dhcpServerDelBind(OmnString &rslt);
	int 	saveDhcpServerConfig(OmnString &rslt);
	int 	saveOnlyDhcpServerConfig(OmnString &rslt);
	int 	loadDhcpServerConfig(OmnString &rslt);
	int 	dhcpClientGetIp(OmnString &rslt);
	int 	dhcpClientShowIp(OmnString &rslt);

	int 	secureAuthcmdPolicy(OmnString &rslt);
	int 	secureAuthcmdCommandAdd(OmnString &rslt);
	int 	secureAuthcmdCommandDel(OmnString &rslt);
	int 	secureAuthcmdCommandShow(OmnString &rslt);
	int 	secureAuthcmdCommandReset(OmnString &rslt);

	//GB
	int 	dnsproxyStart(OmnString &rslt);
	int 	dnsproxyStop(OmnString &rslt);
	int 	dnsproxyRestart(OmnString &rslt);
	int 	dnsproxySetName(OmnString &rslt);
	int 	dnsproxySetIp(OmnString &rslt);
	int 	dnsproxySetLIp(OmnString &rslt);
	int 	dnsproxyAddGlobal(OmnString &rslt);
	int 	dnsproxyDelGlobal(OmnString &rslt);
	int 	dnsproxyShow(OmnString &rslt);
	int 	saveDnsproxyConfig(OmnString &rslt);
	int 	saveOnlyDnsproxyConfig(OmnString &rslt);
	int 	loadDnsproxyConfig(OmnString &rslt);


	int 	dnsproxyShowIp(OmnString &rslt);

	//GB
	int 	pppoeStart(OmnString &rslt);
	int 	pppoeStop(OmnString &rslt);
	int 	pppoeStatus(OmnString &rslt);
	int 	pppoeShow(OmnString &rslt);
	int 	pppoeUsernameSet(OmnString &rslt);
	int 	pppoePasswordSet(OmnString &rslt);
	int 	pppoeDnsSet(OmnString &rslt);
	int 	savePppoeConfig(OmnString &rslt);
	int 	saveOnlyPppoeConfig(OmnString &rslt);
	int 	loadPppoeConfig(OmnString &rslt);

	//xyb
	int 	cpuMgrSetRecordTime(OmnString &rslt);
	int 	cpuMgrSetRecordStop(OmnString &rslt);
	int 	cpuMgrShowCpu(OmnString &rslt);
	int 	cpuMgrShowProcess(OmnString &rslt);

	int 	diskMgrSetRecordTime(OmnString &rslt);
	int 	diskMgrSetRecordStop(OmnString &rslt);
	int 	diskMgrShowStatistics(OmnString &rslt);
	int 	diskMgrFileTypeAdd(OmnString &rslt);
	int 	diskMgrFileTypeRemove(OmnString &rslt);
	int 	diskMgrFileTypeClear(OmnString &rslt);
	int 	diskMgrFileTypeShow(OmnString &rslt);

	int 	memMgrSetRecordTime(OmnString &rslt);
	int 	memMgrSetRecordStop(OmnString &rslt);
	int 	memMgrShowMem(OmnString &rslt);

	int 	diskMgrQuotaStatus(OmnString &rslt);
	int 	diskMgrQuotaStatusShow(OmnString &rslt);
	int 	diskMgrQuotaList(OmnString &rslt);
	int 	diskMgrQuotaSet(OmnString &rslt);
	int 	diskMgrQuotaDelete(OmnString &rslt);
	int 	diskMgrQuotaShow(OmnString &rslt);

	int 	watchdogSetStatus(OmnString &rslt);
	int 	watchdogStop(OmnString &rslt);

	//GB
	int 	pptpStart(OmnString &rslt);
	int 	pptpStop(OmnString &rslt);
	int 	pptpIpLocal(OmnString &rslt);
	int 	pptpIpRange(OmnString &rslt);
	int 	pptpAddUser(OmnString &rslt);
	int 	pptpDelUser(OmnString &rslt);
	int 	pptpShowConfig(OmnString &rslt);
	int 	savePptpConfig(OmnString &rslt);
	int 	saveOnlyPptpConfig(OmnString &rslt);
	int 	loadPptpConfig(OmnString &rslt);

	int 	macSet(OmnString &rslt);
	int 	macBackSet(OmnString &rslt);
	int 	saveMacConfig(OmnString &rslt);
	int 	saveOnlyMacConfig(OmnString &rslt);
	int 	macShowConfig(OmnString &rslt);
	int 	loadMacConfig(OmnString &rslt);

	int 	bridgeGroupAdd(OmnString &rslt);
	int 	bridgeGroupDel(OmnString &rslt);
	int     bridgeGroupMemberAdd(OmnString &rslt);
	int     bridgeGroupMemberDel(OmnString &rslt);
	int     bridgeGroupMemberIp(OmnString &rslt);
	int     bridgeGroupIp(OmnString &rslt);
	int     bridgeShowConfig(OmnString &rslt);
	int     bridgeClearConfig(OmnString &rslt);
	int     bridgeGroupUp(OmnString &rslt);
	int     bridgeGroupDown(OmnString &rslt);
	// CHK 2006-12-12
	int     saveOnlyBridgeConfig(OmnString &rslt);
	int 	saveBridgeConfig(OmnString &rslt);
	int 	loadBridgeConfig(OmnString &rslt);


	bool 	startSecuredShell(OmnString &rslt);
	bool 	stopSecuredShell(OmnString &rslt);
	bool	username(OmnString &rslt);
	bool	createSecuredShell(OmnString &rslt);
	bool	removeShellCommands(OmnString &rslt);
	bool	restoreShellCommands(OmnString &rslt);
	bool	clearActiveShells(OmnString &rslt);

	private:
	int		saveConfig(const char *moduleName, OmnString &rslt);
	int		loadConfig(const char *moduleName, OmnString &rslt);

	//kevin 07/10/2006
	int 	systemUname(OmnString &id, OmnString &rslt);
	int 	systemArpProxyAdd(OmnString &interface, OmnString &rslt);
	int 	systemArpProxyDel(OmnString &interface, OmnString &rslt);
	int 	systemIpAddress(OmnString &interface, OmnString &ip, OmnString &mask, OmnString &rslt);
	int 	systemDns(OmnString &dns, OmnString &rslt);
	int 	systemRouteCommon(OmnString &dip, OmnString &dmask, OmnString &gateway, OmnString &rslt);
	int 	systemRouteDefault(OmnString &gateway, OmnString &rslt);
	int		systemUpdate(OmnString &url, OmnString &rslt);

	int		fwIpfilAll(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwIpfilTcp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwIpfilUdp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwIpfilIcmp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwIpfilStateNew(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwIpfilStateAck(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwMacfilAll(OmnString &chain, OmnString &smac, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwMacfilTcp(OmnString &chain, OmnString &smac, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwMacfilUdp(OmnString &chain, OmnString &smac, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwMacfilIcmp(OmnString &chain, OmnString &smac, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt);
	int		fwAntiSyn(OmnString &dip, OmnString &dport, OmnString &rate, OmnString &rslt);
	int		fwAntiIcmp(OmnString &dip, OmnString &rate, OmnString &rslt);

	int		fwNatSnat(OmnString &interface, OmnString &sip, OmnString &smask, OmnString &to, OmnString &rslt);
	int		fwNatMasq(OmnString &interface, OmnString &sip, OmnString &smask, OmnString &rslt);
	int		fwNatDnatPort(OmnString &interface, OmnString &proto, OmnString &sip, OmnString &sport, OmnString &dip, OmnString &dport, OmnString &rslt);
	int		fwNatDnatIP(OmnString &interface, OmnString &proto, OmnString &sip, OmnString &dip, OmnString &rslt);
	int		fwNatRedi(OmnString &interface, OmnString &proto, OmnString &sip, OmnString &smask, OmnString &rslt);


	int		fwTimefil(OmnString &chain, OmnString &proto, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &beginTime, OmnString &endTime, OmnString &week, OmnString &log, OmnString &rslt);


	int		webwallRedi(OmnString &interface, OmnString &fromport, OmnString &toport, OmnString &dip, OmnString &rslt);
	private:
};


//
// The part of the modules global variables
//
// Dns Proxy
extern struct dnsProxyConfigEntry
{
	OmnString ip;
	OmnString lIp;
	OmnString dev;
	OmnString name;
	OmnString status;
	OmnString global;
} sgDnsproxy; 

// PPPoE

extern struct pppoeConfigEntry
{
	OmnString username;
	OmnString password;
	OmnString dns1;
	OmnString dns2;
	OmnString status;
} sgPppoe;	

extern struct vlanSwitchEntry
{
	OmnString vlanName;
	OmnString status;
	OmnString reset;
	OmnString port;
} sgVlanSwitch;

// Wan

extern struct wanConfigEntry
{
	OmnString ip;
	OmnString netmask;
	OmnString method;
	OmnString hc_time;
	OmnString hc_ip;
	OmnString status;
} sgWanCM; 

// DHCP Server
struct dhcpIpBindEntry
{
	OmnString alias;
	OmnString mac;
	OmnString ip;
	struct aos_list_head datalist;
};

extern struct aos_list_head sgDhcpIpBindList;

extern struct dhcpConfigEntry
{
	OmnString dft_leaseTime;
	OmnString ip1;
	OmnString ip2;
	OmnString dns;
	OmnString router;
	OmnString status;
	struct dhcpIpBindEntry dhcpIpBind;
} sgDhcp;	

//DHCP Client
// PPTP
struct pptpUserEntry
{
	OmnString  username;
	OmnString  password;
	OmnString  ip;
	struct aos_list_head datalist;	
};

extern struct pptpConfigEntry
{
	OmnString ipLocal;
	OmnString ip1;
	OmnString ip2;
	OmnString status;
	struct pptpUserEntry pptpUser;
} sgPptp;	

extern struct aos_list_head sgPptpUserList;


// SNMP

// Router

// Firewall

struct fwFilterListEntry
{
	OmnString type;
	OmnString chain;
	OmnString proto;
	OmnString sip;
	OmnString smask;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString beginTime;
	OmnString endTime;
	OmnString week;
	OmnString rate;
	OmnString log;
	struct aos_list_head datalist;
};

struct fwNatListEntry
{
	OmnString type;
	OmnString interface;
	OmnString proto;
	OmnString sip;
	OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;	
	OmnString to;	
	OmnString fromport;	
	OmnString toport;	
	struct aos_list_head datalist;
}; 

extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;

struct macListEntry
{
	OmnString dev;
	OmnString mac;
	OmnString oldMac;
	struct aos_list_head datalist;
};
extern struct aos_list_head sgMacList;

struct bridgeDevEntry
{
	OmnString  dev;
	OmnString  ip;
	struct aos_list_head datalist;
	//struct bridgeDevEntry *next;	
};

struct bridgeConfigEntry
{
	OmnString bridgeName;
	OmnString ip;
	OmnString status;
	struct bridgeDevEntry bridgeDev[16];
	//struct aos_list_head bridgeDevHeader;
	struct aos_list_head datalist;
};	

extern struct aos_list_head sgBridgeConfigList;

//xyb 11/12/2006
struct file_stats {
	unsigned char hour;	/* (0-23) */
	unsigned char minute;		/* (0-59) */
	unsigned char second;		/* (0-59) */
	unsigned long ust_time;

	unsigned long uptime;
	unsigned long processes;
	unsigned int  context_swtch;

	unsigned int  cpu_user;
	unsigned int  cpu_nice;
	unsigned int  cpu_system;
	unsigned long cpu_idle;
	unsigned long cpu_iowait;
	unsigned int  pid_num;
	unsigned int  max_pid;
};

#define FILE_STATS_SIZE	(sizeof(struct file_stats))


struct pid_stats {
	unsigned long pid;
	char			 name[16];
	unsigned long utime;
	unsigned long stime;
};

#define PID_STATS_SIZE	(sizeof(struct pid_stats))

#define STOR_DIR  "/var/log/CPU"

#endif
#endif
