////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "rhcAPI/rhcApi.h"
#include "rhcAPI/SystemApi.h"
#define MAX_IP_STRING_LEN 15
#define MAX_MAC_STRING_LEN 17
#define MAX_NAME_STRING_LEN 64
#define MAX_STATUS_STRING_LEN 8


int 
main(int argc, char **argv)
{	
	// DHCP Server
	char result[1024];
	struct dhcpConfigEntry dhcp;
	struct dnsProxyConfigEntry dnsproxy;
	struct secureCmdEntry secureCmd;
	struct pppoeConfigEntry pppoe_info;
    struct macListEntry macclone_info;
    struct bridgeDevEntry bridgedev[20];
	struct bridgeConfigEntry bridgeconfig_info;
//	struct systemConfigListEntry system_info;
	// init the data
	AOS_INIT_LIST_HEAD(&sgDhcpIpBindList);
	AOS_INIT_LIST_HEAD(&sgSecureCmdList);
	
/*
	dhcpServerRouterSet("172.22.0.3", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	dhcpServerDnsSet("12.12.12.12", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

    dhcpServerLeaseTimeSet("7200", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );


    dhcpServerAddBind("a","00:16:d4:09:00:05","172.22.111.11", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	dhcpServerAddBind("b","00:16:d4:09:00:06","172.22.111.12", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	dhcpServerIpBlockSet("172.22.111.10","172.22.111.18", result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
    
	
	dhcpServerShowConfig(&dhcp, result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
  

	dhcpServerDelBind("a", result); 
	printf("the result is:%s\n", result);

    dhcpServerStart(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

    dhcpServerStop(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/

	dnsproxyAddGlobal(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	dnsproxySetIp("172.22.0.1",result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
    dnsproxyDelGlobal(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	dnsproxySetName("MyDns",result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	dnsproxySetLIp("172.22.1.3","outcard",result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

    dnsproxyStart(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	dnsproxyStop(result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	dnsproxyShow(&dnsproxy, result); 
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

/*
	secureAuthcmdCommandAdd("/usr/bin/cat", result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	secureAuthcmdCommandAdd("/usr/bin/ls", result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	secureAuthcmdCommandDel("/usr/bin/cat", result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	secureAuthcmdPolicy("deny",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/
	
/*	
 	secureAuthcmdCommandReset(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	secureAuthcmdCommandShow(&secureCmd,result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/
	
    pppoeUsernameSet("pppoe_name1",result);	
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

    pppoePasswordSet("123456",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	pppoeDnsSet("202.103.0.116","202.103.0.117",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	pppoeStart(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	pppoeShow(&pppoe_info, result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

	pppoeStop(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	pppoeShow(&pppoe_info, result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );


	//mac clone
	macSet("eth1","00:AE:97:b0:10:01",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	macShowConfig(&macclone_info ,result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	macSet("eth1","AE:AE:97:b0:10:B1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	macBackSet("eth1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	macShowConfig(&macclone_info ,result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
//system config
/*	
   systemDns("192.168.2.3",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
   systemIpAddress("incard","192.168.2.1","255.255.255.0",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
   systemUname("all",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
   showSystemConfig(&system_info,result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
   clearSystemConfig(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/
	
//BRIDGE SECTION
 
	bridgeGroupAdd("group1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

   bridgeGroupMemberAdd("group1","eth1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

   bridgeGroupIp("group1","192.22.0.1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

  bridgeGroupMemberIp("eth1","192.165.2.6",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

  bridgeGroupUp("group1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

 bridgeShowConfig(bridgedev,&bridgeconfig_info,result);	
	printf("------------------------------------------------------------------------------------------------\n" );
 
	bridgeGroupDown("group1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

 bridgeShowConfig(bridgedev,&bridgeconfig_info,result);	
	printf("------------------------------------------------------------------------------------------------\n" );

   bridgeGroupDel("group1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

   bridgeGroupMemberDel("group1","eth1",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );

 bridgeShowConfig(bridgedev,&bridgeconfig_info,result);	
	printf("------------------------------------------------------------------------------------------------\n" );

  bridgeClearConfig(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
   
 bridgeShowConfig(bridgedev,&bridgeconfig_info,result);	
	printf("------------------------------------------------------------------------------------------------\n" );
	


//firewall section
/*
fwMacfilAll("forward","12:34:56:78:90:12","192.168.1.1", "255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilTcp("forward","12:34:56:78:90:13","8000","192.168.1.2","255.255.255.255","8001","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilUdp("forward","12:34:56:78:90:14","8002","192.168.1.3","255.255.255.255","8003","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilIcmp("in","12:34:56:78:90:15","192.168.1.4","255.255.255.255","permit","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilAll("out","172.22.99.1","255.255.255.255","172.22.99.2","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilTcp("out","172.22.99.3","255.255.255.255","0:888","172.22.99.4","255.255.255.255","0:999","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilUdp("out","172.22.99.5","255.255.255.255","888:999","172.22.99.6","255.255.255.255","999:1000","permit","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilIcmp("out","172.22.99.7","255.255.255.255","172.22.99.8","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwTimefil("forward","tcp","10.1.50.1","255.255.255.255","10.1.50.2","255.255.255.255","deny","04:16","05:16","Sat","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatSnat("outcard","192.168.1.1","255.255.255.255","202.103.0.235",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatMasq("outcard","192.168.2.1","255.255.255.255",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDnatPort("incard","tcp","192.168.2.2","80","202.103.0.117","8080",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDnatIP("outcard","all","192.168.3.4","202.103.0.118",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatRedi("incard","udp","80","8080",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilStateNew("in","202.103.0.117","255.255.255.255","202.103.0.118","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilStateAck("in","202.103.0.119","255.255.255.255","202.103.0.120","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/
/*
fwDelTimefil("forward","tcp","10.1.50.1","255.255.255.255","10.1.50.2","255.255.255.255","deny","04:16","05:16","Sat","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDelSnat("outcard","192.168.1.1","255.255.255.255","202.103.0.235",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDelMasq("outcard","192.168.2.1","255.255.255.255",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDelDnatPort("incard","tcp","192.168.2.2","80","202.103.0.117","8080",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDelDnatIP("outcard","all","192.168.3.4","202.103.0.118",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwNatDelRedi("incard","udp","80","8080",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelStateNew("in","202.103.0.117","255.255.255.255","202.103.0.118","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelStateAck("in","202.103.0.119","255.255.255.255","202.103.0.120","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilDelAll("forward","12:34:56:78:90:12","192.168.1.1", "255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilDelTcp("forward","12:34:56:78:90:13","8000","192.168.1.2","255.255.255.255","8001","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilDelUdp("forward","12:34:56:78:90:14","8002","192.168.1.3","255.255.255.255","8003","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwMacfilDelIcmp("in","12:34:56:78:90:15","192.168.1.4","255.255.255.255","permit","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelAll("out","172.22.99.1","255.255.255.255","172.22.99.2","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelTcp("out","172.22.99.3","255.255.255.255","0:888","172.22.99.4","255.255.255.255","0:999","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelUdp("out","172.22.99.5","255.255.255.255","888:999","172.22.99.6","255.255.255.255","999:1000","permit","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwIpfilDelIcmp("out","172.22.99.7","255.255.255.255","172.22.99.8","255.255.255.255","deny","log",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
*/	
/*	
fwBlacklistLoadFile("/home/blacklist",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwBlacklistRemoveFile("/home/blacklist",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwBlacklistLoadUrl("ftp:/172.22.0.1/incoming/blacklist",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
fwBlacklistRemoveUrl("ftp:/172.22.0.1/incoming/blacklist",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
*/	
	
//Disk Quota Section

diskMgrQuotaStatus("on","/home",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
diskMgrQuotaList(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
diskMgrQuotaSet("/home","test","100M",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
diskMgrQuotaDelete("test","/home",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
diskMgrQuotaShow("","","",result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
diskMgrQuotaStatusShow(result);
	printf("the result is:%s\n", result);
	printf("------------------------------------------------------------------------------------------------\n" );
	
	return 0;
} 

