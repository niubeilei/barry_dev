/////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Global.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef system_Global_Cli_h
#define system_Global_Cli_h

#include "Util/String.h"
#include "Util/OmnNew.h"

#include "aosUtil/List.h"  
#include "ReturnCode.h"  



#define MAX_IP_ADDR_LEN 16 



extern int dhcpServerDnsSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerRouterSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerLeaseTimeSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerIpBlockSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerAddBind(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerDelBind(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpServerStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpClientShowIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dhcpClientGetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int saveDhcpServerConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
		
extern int DhcpServerClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int loadDhcpServerConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);					


extern int cpuMgrShowProcess(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int cpuMgrShowCpu(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// DHCP Server
struct dhcpIpBindEntry
{
	OmnString alias;
	OmnString mac;
	OmnString ip;
	struct aos_list_head datalist;
};

struct dhcpConfigEntry
{
	int dft_leaseTime;
	OmnString ip1;
	OmnString ip2;
	OmnString dns;
	OmnString router;
	OmnString status;
	struct dhcpIpBindEntry dhcpIpBind;
};	

extern int dhcpInit();

//Dns Proxy
extern int dnsproxyStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxyStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxyShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
		
extern int dnsproxySaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
		
int dnsproxyClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxySetName(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxySetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxyAddGlobal(char *data, unsigned int *optlen, 

		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxyDelGlobal(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int dnsproxySetLIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

struct dnsProxyConfigEntry
{
	OmnString ip;
	OmnString lIp;
	OmnString dev;
	OmnString name;
	OmnString status;
	OmnString global;
}; 

extern int dnsproxyInit();

//secure command authenication

extern int secureAuthcmdPolicy(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int secureAuthcmdCommandAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int secureAuthcmdCommandDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int secureAuthcmdCommandShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int secureAuthcmdCommandReset(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

//firewall
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

struct fwVpnPass
{
	OmnString ipsecStat;
	OmnString pptpStat;
	OmnString l2tpStat;
};

extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;
extern struct fwVpnPass sgFwVpnPass;

extern int fwInit();

// mac cloning
struct macListEntry
{
	OmnString dev;
	OmnString mac;
	OmnString oldMac;
	struct aos_list_head datalist;
};

extern struct aos_list_head sgMacList;
extern int macCloningInit();

extern int fwSetSyncookie(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int showFwConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistLoadFile(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistRemoveFile(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistLoadUrl(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistRemoveUrl(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilAll(OmnString &chain, OmnString &smac, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwMacfilAll(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilTcp(OmnString &chain, OmnString &smac, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwMacfilTcp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilUdp(OmnString &chain, OmnString &smac, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwMacfilUdp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

int fwMacfilIcmp(OmnString &chain, OmnString &smac, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwMacfilIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilDelAll(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilDelTcp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilDelUdp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwMacfilDelIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilAll(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilAll(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilTcp(OmnString &chain, u32 sip, u32 smask, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilTcp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilUdp(OmnString &chain, u32 sip, u32 smask, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilUdp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilIcmp(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilStateNew(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilStateNew(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilStateAck(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilStateAck(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelAll(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt);

extern int fwIpfilDelAll(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelTcp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelUdp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelStateNew(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpfilDelStateAck(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// firewall NAT
extern int fwNatSnat(OmnString &interface, u32 sip, u32 smask, u32 to, OmnString &rslt);

extern int fwNatSnat(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDelSnat(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

int fwNatMasq(OmnString &interface, u32 sip, u32 smask, OmnString &rslt);

extern int fwNatMasq(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDelMasq(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDnatPort(OmnString &interface, OmnString &proto, u32 sip, u32 sport, u32 dip, u32 dport, OmnString &rslt);

extern int fwNatDnatPort(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDelDnatPort(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDnatIP(OmnString &interface, OmnString &proto, u32 sip, u32 dip, OmnString &rslt);

extern int fwNatDnatIP(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDelDnatIP(OmnString &interface, OmnString &proto, u32 sip, u32 dip, OmnString &rslt);

extern int fwNatDelDnatIP(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatRedi(OmnString &interface, OmnString &proto, u32 fromport, u32 toport, OmnString &rslt);

extern int fwNatRedi(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwNatDelRedi(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int webwallRedi(OmnString &interface, u32 fromport, u32 toport, u32 dip, OmnString &rslt);

extern int webwallRedi(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int webwallDelRedi(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int webwallDnat(OmnString &interface, u32 fromport, u32 toport, u32 dip, u32 toip, OmnString &rslt);

extern int webwallDnat(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int webwallDelDnat(OmnString &interface, u32 fromport, u32 toport, u32 dip, u32 toip, OmnString &rslt);

extern int webwallDelDnat(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// firewall ANTI ATTACK
extern int fwAntiSyn(u32 dip, OmnString &dport, OmnString &rate, OmnString &rslt);

extern int fwAntiAttackOutcard(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwOutcardPing(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwAntiIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwAntiIcmp(u32 dip, OmnString &rate, OmnString &rslt);

extern int fwAntiSyn(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwAntiDelIcmp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwAntiDelSyn(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern unsigned int parse_port(const char *port);

extern unsigned int parse_multi_ports(char *portstring, unsigned int *ports);

extern int parse_mac(char *mac, int *macarray);

extern int portlen(int port);

extern int ratelen(int port);

/*firewall vpn pass*/
extern int fwL2tpPass(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwPptpPass(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwIpsecPass(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);					

extern int fwBlacklistLoadFile(const OmnString fileName, OmnString &rslt);
	
extern int fwBlacklistLoadFile(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistRemoveFile(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistLoadUrl(const OmnString fileName, OmnString &rslt);

extern int fwBlacklistLoadUrl(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwBlacklistRemoveUrl(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int initFwMaxsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int initFwMinsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int initFwMidsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int initFwConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int clearFwConfig(OmnString &rslt);
		
extern int clearFwConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int showFwLog(OmnString &rslt);

extern int showFwLog(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);


extern int fwTimefil(OmnString &chain, OmnString &proto, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action,OmnString &beginTime,OmnString &endTime,OmnString &week, OmnString &log, OmnString &rslt);

extern int fwTimefil(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int fwDelTimefil(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern struct pppoeConfigEntry
{
	OmnString username;
	OmnString password;
	OmnString dns1;
	OmnString dns2;
	OmnString status;
} sgPppoe;	

/*typedef struct pppoeConfigEntry_t
  {
  OmnString username;
  OmnString password;
  OmnString dns1;
  OmnString dns2;
  OmnString status;
  } pppoeConfigEntry;
  extern pppoeConfigEntry sgPppoe;

  extern int pppoeInit();
  */
extern int pppoeInit();

extern int pppoeStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeUsernameSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeUsernameSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoePasswordSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeDnsSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int pppoeSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
		
extern int pppoeClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);		

extern struct aos_list_head sgMacList;

extern int 	macSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	macGetCurrent(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	macBackSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	macShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	macSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);	
		
extern int 	macClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);				

// Bridge Config Module Beging
// 2006-12-27 By CHK
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

extern int  bridgeInit();

extern int 	bridgeGroupAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupMemberAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupMemberDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupMemberIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupUp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	bridgeGroupDown(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// Bridge Config Module End


// CPU Manager Module Beging
// 2006-12-27 By CHK
// switch on/off
// here create or stop the thread
extern int	cpuMgrSwitchOnOff(char *data, unsigned int *optlen, 
					struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen);

extern int 	cpuMgrSetRecordTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	cpuMgrSetRecordTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	cpuMgrSetRecordStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	cpuMgrShowCpu(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	cpuMgrShowProcess(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// System nice 
// system nice set <pid> <nice-value>
extern int system_nice_set(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// CPU Manager Module End

// Memory Manager Module Begin
// 2007-01-04 By CHK
extern int 	memMgrSetRecordTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	memMgrSetRecordStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int 	memMgrShowMem(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// Disk Manager Module Begin
// 2007-01-04 By CHK
// disk mgr set record time <interval> <record time>
extern int 	diskMgrSetRecordTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr set record stop
extern int 	diskMgrSetRecordStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr show statistics
extern int 	diskMgrShowStatistics(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr file type add <type>
extern int	diskMgrFileTypeAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr file type remove <type>
extern int	diskMgrFileTypeRemove(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr file type show
extern int	diskMgrFileTypeShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// disk mgr file type clear
extern int	diskMgrFileTypeClear(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// Disk Manager Module End

// snmp agent Module Start
// By CHK 2007-01-19
#define SNMPAGNT_CONTENT_MAX_LEN 20480
#define SNMPAGNT_TIME_MAX_LEN 20

struct snmpConfigEntry
{
	char zLoadTime[SNMPAGNT_TIME_MAX_LEN];   /* Time string for load time Format: %Y%m%d-%T */
	char zContent[SNMPAGNT_CONTENT_MAX_LEN]; /* Tmp all of the information from file snmp.conf */
};
#define snmpConfigEntry_t struct snmpConfigEntry
extern snmpConfigEntry_t * sgSnmpEntryPtr;

// snmp agent start
extern int	snmpAgentStart(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// snmp agent stop
extern int	snmpAgentStop(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// snmp show config
extern int	snmpShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// snmp agent Module End

//System begin
struct systemConfigListEntry
{
	OmnString type;
	OmnString interface;
	OmnString ip;
	OmnString mask;
	OmnString dns;
	OmnString dip;
	OmnString dmask;
	OmnString gateway;
	struct aos_list_head datalist;
}; 

extern struct aos_list_head sgSystemConfigList;

extern int systemInit();

extern int systemIpAddress(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);


extern int systemDns(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);


extern int systemUname(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int clearSystemConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);



extern int showSystemConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int systemDevRoute(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int systemDelDevRoute(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int systemArpProxyAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int systemArpProxyDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

//router statistic
struct RouterStatis 
{
	char Nic[64];
	unsigned long RecPack;
	unsigned long RecError;
	unsigned long RecDrop;
	unsigned long RecByte;
	unsigned long SendPack;
	unsigned long SendByte;
	unsigned long SendError;
	unsigned long SendDrop;
};

extern int routerStatisShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int routerStatisStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int routerStatisSetRecordTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

//wan connnection manager
extern int wanAdslUserSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanAdslPasswdSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanDnsSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanIpAddressSet(u32 ip, u32 mask, OmnString rslt); 
		
extern int wanIpAddressSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanIpAddressShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanConnectMethodShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanGetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanHealthCheckSetTime(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanHealthCheckSetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
		
extern int wanLoadConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);				

extern int wanSetStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int wanPppoe(OmnString &username, OmnString &password, OmnString &dns1, OmnString &dns2);

extern int wanStatic(OmnString &ip, OmnString &netmask);

extern  int wanDhcp();

extern int wanStatusOff();
// Disk Quota
// By xyb
// ============ Beg =========
// Append "Disk Quota Manager" commands
// disk mgr quota status <on|off> <partition>
extern int diskMgrQuotaStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota status show
extern int diskMgrQuotaStatusShow(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota list
extern int diskMgrQuotaList(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota add user <mount point> <user> <size>
extern int diskMgrQuotaAddUser(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota delete user <partition> <user>
extern int diskMgrQuotaDeleteUser(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota add  <mount point> <user> <size>
extern int diskMgrQuotaAddDir(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota delete dir <partition> <dir>
extern int diskMgrQuotaDeleteDir(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota show user <user>
extern int diskMgrQuotaShowUser(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota show dir <dir>
extern int diskMgrQuotaShowDir(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota show config 
extern int diskMgrQuotaShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota clear config 
extern int diskMgrQuotaClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
// disk mgr quota save config 
extern int diskMgrQuotaSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

//
//ros os system cli command
//
//ros os system reboot
extern int rosOsSystemReboot(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);
//ros os system shutdown
extern int rosOsSystemShutdown(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

// function get br0 ip address
extern char* get_ip_address(void);

// ============ End =========
// Disk Quota  
/*struct vlanIdEntry
{
	int  id;
	int  ip;
	struct aos_list_head datalist;
};
*/
struct vlanConfigEntry
{
	OmnString dev;
	OmnString id;
	OmnString ip;	
	struct aos_list_head datalist;
};	

extern struct vlanSwitchEntry
{
	OmnString status;
	OmnString reset;
	OmnString vlanName;
	OmnString port;
} sgVlanSwitch;

extern struct aos_list_head sgVlanConfigList;

extern int vlanInit();

extern int vlanDeviceAdd(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanDeviceDel(OmnString &dev, OmnString&id, OmnString &rslt); 

extern int vlanDeviceDel(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanDeviceDelAllTag(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanDeviceDelAllIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanDelIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSetIp(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSetIp(OmnString &dev, OmnString &id, OmnString &ip, OmnString &rslt);


extern int vlanClearConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSaveConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanLoadConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanShowConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSwitchStatus(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSwitchStatus(OmnString &status, OmnString &rslt);

extern int vlanSwitchReset(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSwitchReset(OmnString isReset, OmnString &rslt);

extern int vlanSwitchSet(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen);

extern int vlanSwitchSet(OmnString &vlanName, OmnString &ports, OmnString &rslt);

// get ip adderss
//
extern char* get_ip_address(void);

/*Cli Reg Func*/
extern int FwVpnPass_regCliCmd(void);
extern int WanConnectionMgr_regCliCmd(void);
extern int RouterStat_regCliCmd(void);
extern int SystemConfig_regCliCmd(void);
extern int QuotaMgr_regCliCmd(void);
extern int RosOS_regCliCmd(void);
extern int Snmp_regCliCmd(void);
extern int ResMgr_regCliCmd(void);
extern int DiskMgr_regCliCmd(void);
extern int MemMgr_regCliCmd(void);
extern int CpuMgr_regCliCmd(void);
extern int Bridge_regCliCmd(void);
extern int FwNat_regCliCmd(void);
extern int FwFilter_regCliCmd(void);
extern int FwConfig_regCliCmd(void);
extern int FwBlacklist_regCliCmd(void);
extern int MacClone_regCliCmd(void);
extern int Pppoe_regCliCmd(void);
extern int SecureAuth_regCliCmd(void);
extern int SecuredShell_regCliCmd(void);
extern int DnsProxy_regCliCmd(void);
extern int Dhcp_regCliCmd(void);
extern int ArpProxy_regCliCmd(void);
extern int Vlan_regCliCmd(void);
extern int Dnsmasq_regCliCmd(void);
extern int Dmz_regCliCmd(void);
extern int Router_regCliCmd(void);
extern int Pptp_regCliCmd(void);
extern int SystemMisc_regCliCmd(void);
extern int SystemTime_regCliCmd(void);


// dnsmasq
// add by Kevin 03/08/2007

extern int dnsmasqInit();

struct dnsmasqBindEntry
{
	OmnString alias;
	OmnString mac;
	u32 ip;
	struct aos_list_head datalist;
};

struct dnsmasqHostsEntry
{
	OmnString alias;
	u32 ip;
	OmnString domain;
	struct aos_list_head datalist;
};

struct dnsmasqConfigEntry
{
	OmnString hostname;
	OmnString exceptDev;
	u32 ip1;
	u32 ip2;
	u32 netmask;
	OmnString status;
	OmnString leaseTime;
	u32 dns1;
	u32 dns2;
	//OmnString ip;
	//OmnString mac;
	struct dnsmasqHostsEntry dnsmasqHosts;
	struct dnsmasqBindEntry dnsmasqBind;
}; 

int dnsmasqSetName(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqAddHosts(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqDelHosts(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqAddBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetBindByMac(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetBindByLabel(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetBindByIp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqDelBind(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqSetDns(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetDns(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqSetIpRange(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqgetIpRange(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqSetExceptDev(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqSetLeaseTime(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqGetLeaseTime(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dnsmasqSetExceptDev(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dnsmasqShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dnsmasqShowLog(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dnsmasqSaveConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dnsmasqLoadConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dnsmasqClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);


// dmz
// add by Kevin 03/15/2007

extern int dmzInit();

struct dmzMachineEntry
{
	OmnString alias;
	u32 pub_ip;
	u32 dmz_ip;
	struct aos_list_head datalist;
};

struct dmzConfigEntry
{
	OmnString status;
	struct dmzMachineEntry dmzMachine;
}; 

int dmzSetStatus(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dmzAddMachine(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int dmzDelMachine(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dmzShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dmzSaveConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dmzLoadConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int dmzClearFwConfig(OmnString &rslt);
extern int dmzClearConfig(OmnString &rslt);

extern int dmzClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);


// router
// add by Kevin 03/16/2007

extern int routerInit();

struct routerEntry
{
	OmnString alias;
	u32 dip;
	u32 dmask;
	u32 gateway;
	OmnString interface;
	struct aos_list_head datalist;
};

struct routerConfigEntry
{
	OmnString status;
	struct routerEntry router;
}; 

int routerSetStatus(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int routerAddEntry(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

int routerDelEntry(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int routerShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int routerSaveConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int routerLoadConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int routerClearRTConfig(OmnString &rslt);

extern int routerClearConfig(OmnString &rslt);

extern int routerClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpStart(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpStop(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpIpLocal(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpIpRange(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpAddUser(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpDelUser(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int pptpShowConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int savePptpConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int PptpClearConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

struct pptpUserEntry
{
	OmnString  username;
	OmnString  password;
	u32 ip;
	struct aos_list_head datalist;	
};

extern struct pptpConfigEntry
{
	u32 ipLocal;
	u32 ip1;
	u32 ip2;
	OmnString status;
	struct pptpUserEntry pptpUser;
} sgPptp;	

extern struct aos_list_head sgPptpUserList;

extern int pptpInit();

extern int SystemShowInfo(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int SystemSetDomain(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

extern int SystemSetHostname(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);


#endif

