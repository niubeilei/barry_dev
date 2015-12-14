////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_SystemApi_h
#define aos_SystemApi_h

#include "rhcAPI/List.h"

#define MAX_IP_STRING_LEN 32
#define MAX_PORT_STRING_LEN 8
#define MAX_MAC_STRING_LEN 32
#define MAX_NAME_STRING_LEN 64
#define MAX_STATUS_STRING_LEN 8
#define MAX_MODID_STRING_LEN 32

#define CMD_TXT "cmd.txt"
#define CMD_TXT_FULLPATH "/usr/local/rhc/data/cmd.txt"

// DHCP Server
extern struct dhcpIpBindEntry
{
	char alias[MAX_NAME_STRING_LEN+1];
	char mac[MAX_IP_STRING_LEN+1];
	char ip[MAX_IP_STRING_LEN+1];
	struct aos_list_head datalist;
};
	
extern struct aos_list_head sgDhcpIpBindList;

extern struct dhcpConfigEntry
{
	char dft_leaseTime[8];
	char ip1[MAX_IP_STRING_LEN+1];
	char ip2[MAX_IP_STRING_LEN+1];
	char dns[MAX_IP_STRING_LEN+1];
	char router[MAX_IP_STRING_LEN+1];
	char status[MAX_STATUS_STRING_LEN+1];
	struct dhcpIpBindEntry dhcpIpBind;
} sgDhcp;	

extern int dhcpServerRouterSet(char *router, char *result); 
extern int dhcpServerAddBind(char *label,char *mac, char *ip, char *result); 
extern int dhcpServerDelBind(char *label,char *result); 
extern int dhcpServerIpBlockSet(char *ip1, char *ip2, char *result); 
extern int dhcpServerStart(char *result); 
extern int dhcpServerStop(char *result); 
extern int dhcpServerShowConfig(struct dhcpConfigEntry *dhcp, char *result); 
extern int dhcpServerLeaseTimeSet(char *leaseTime, char *result); 
extern int dhcpServerDnsSet(char *dns, char *result); 

//dnsproxy
extern struct dnsProxyConfigEntry
{
	char ip[MAX_IP_STRING_LEN+1];
	char lIp[MAX_IP_STRING_LEN+1];
	char status[MAX_STATUS_STRING_LEN+1];
	char dev[8];
	char name[10];
	char global[8];
}; 
extern int dnsproxyStart(char *result);
extern int dnsproxyStop(char *result);
extern int dnsproxySetName(char *lable, char *result);
extern int dnsproxySetIp(char *ip, char *result);
extern int dnsproxySetLIp(char *lip, char *dev, char *result);
extern int dnsproxyAddGlobal(char *result);
extern int dnsproxyDelGlobal(char *result);
extern int dnsproxyShow(struct dnsProxyConfigEntry *dnsproxy, char *result);
//secureAuthcmd
extern struct secureCmdEntry
{
	char path[MAX_NAME_STRING_LEN+1];
	char mac[MAX_NAME_STRING_LEN+1];
	struct aos_list_head datalist;
};
extern struct aos_list_head sgSecureCmdList;
extern int secureAuthcmdCommandReset(char *result);
extern int secureAuthcmdCommandDel(char *command, char *result);
extern int secureAuthcmdCommandAdd(char *command, char *result);
extern int secureAuthcmdPolicy(char *policy, char *result);
extern int secureAuthcmdCommandShow(struct secureCmdEntry *secureCmd, char *result);

//PPPOE
extern struct pppoeConfigEntry
{
   char username[10];
   char password[10];
   char dns1[MAX_IP_STRING_LEN+1];
   char dns2[MAX_IP_STRING_LEN+1];
   char status[MAX_STATUS_STRING_LEN+1];
};      
extern int pppoeStart(char *result);
extern int pppoeStop(char *result);
extern int pppoeUsernameSet(char *name,char *result);
extern int pppoePasswordSet(char *password,char *result);
extern int pppoeDnsSet(char *dns_ip,char *dns_ip2,char *result);
extern int pppoeShow(struct pppoeConfigEntry *pppoe_info,char *result);

//MAC CLONE
extern struct macListEntry
{
   char dev[8];
   char mac[MAX_NAME_STRING_LEN+1];
   char oldmac[MAX_NAME_STRING_LEN+1];
   struct aos_list_head datalist;
};
extern int macSet(char *dev,char *mac,char *result);
extern int macBackSet(char *dev,char *result);
extern int macShowConfig(struct macListEntry macclone_info[] ,char *result);

/*
//SYSTEN CONFIG
extern struct systemConfigListEntry
{                   
   char type[MAX_NAME_STRING_LEN+1];
   char interface[MAX_NAME_STRING_LEN+1];
   char ip[MAX_IP_STRING_LEN+1];
   char mask[MAX_IP_STRING_LEN+1];
   char dns[MAX_IP_STRING_LEN+1];
   char dip[MAX_IP_STRING_LEN+1];
   char dmask[MAX_IP_STRING_LEN+1];
   char gateway[MAX_IP_STRING_LEN+1];
   struct aos_list_head datalist;
};
extern int systemDns(char *dns,char *result);
extern int systemIpAddress(char *interface,char *ip,char *mask,char *result);
extern int systemUname(char *id,char *result);
extern int showSystemConfig(struct systemConfigListEntry *system_info,char *result);
extern int clearSystemConfig(char *result);
*/

//BRIDGE SECTION  (added by ricky chen 01092007)

struct bridgeDevEntry
{
   char  dev[8]; 
   char ip[MAX_IP_STRING_LEN+1];
   struct aos_list_head datalist;
   //struct bridgeDevEntry *next;  
};                  
                    
struct bridgeConfigEntry
{
   char  bridgeName[16];
   char ip[MAX_IP_STRING_LEN+1];
   char status[MAX_STATUS_STRING_LEN+1];
   struct bridgeDevEntry bridgeDev[16];
   //struct aos_list_head bridgeDevHeader;
   struct aos_list_head datalist;
};

extern int bridgeGroupAdd(char *group_name,char *result); 
extern int bridgeGroupDel(char *group_name,char *result); 
extern int bridgeGroupMemberAdd(char *bridge_name,char *dev,char *result); 
extern int bridgeGroupIp(char *bridge_name,char *ip,char *result); 
extern int bridgeGroupMemberDel(char *bridge_name,char *dev,char *result); 
extern int bridgeClearConfig(char *result); 
extern int bridgeGroupMemberIp(char *dev,char *ip,char *result); 
extern int bridgeGroupUp(char *bridge_name,char *result); 
extern int bridgeGroupDown(char *bridge_name,char *result); 
//extern int bridgeShowConfig(struct bridgeDevEntry *bridgedev,struct bridgeConfigEntry *bridgeconfig_info,char *result); 
extern int bridgeShowConfig(struct bridgeDevEntry bridgedev[],struct bridgeConfigEntry *bridgeconfig_info,char *result);



//Firewall API
//Firewall API
struct fwFilterListEntry
{
	char type[32];
	char chain[32];
	char proto[32];
	char sip[32];
	char smask[32];
	char smac[32];
	char sport[32];
	char dip[32];
	char dmask[32];
	char dport[32];
	char action[32];
	char beginTime[32];
	char endTime[32];
	char week[32];
	char rate[32];
	char log[32];
	struct aos_list_head datalist;
};

struct fwNatListEntry
{
	char type[32];
	char interface[32];
	char proto[32];
	char sip[32];
	char smask[32];
	char sport[32];
	char dip[32];
	char dmask[32];
	char dport[32];	
	char to[32];	
	char fromport[32];	
	char toport[32];	
	struct aos_list_head datalist;
}; 

extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;

extern int fwMacfilAll(char *chain,char *smac,char *dip,char *dmask,char *action,char *log,char *result);
extern int fwMacfilTcp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                       char *action,char *log,char *result);
extern int fwMacfilUdp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                       char *action,char *log,char *result);
extern int fwMacfilIcmp(char *chain,char *smac,char *dip,char *dmask,
                        char *action,char *log,char *result);
extern int fwMacfilDelAll(char *chain,char *smac,char *dip,char *dmask,char *action,char *log,char *result);
extern int fwMacfilDelTcp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                          char *action,char *log,char *result);
extern int fwMacfilDelUdp(char *chain,char *smac,char *sport,char *dip,char *dmask,char *dport,
                        char *action,char *log,char *result);
extern int fwMacfilDelIcmp(char *chain,char *smac,char *dip,char *dmask,
                       char *action,char *log,char *result);
       
extern int fwIpfilAll(char *chain,char *sip,char *smask,char *dip,char *dmask,
                     char *action,char *log,char *result);
extern int fwIpfilDelAll(char *chain,char *sip,char *smask,char *dip,char *dmask,
                    char *action,char *log,char *result);
extern int fwIpfilTcp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                     char *dport,char *action,char *log,char *result);
extern int fwIpfilDelTcp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                    char *dport,char *action,char *log,char *result);
extern int fwIpfilUdp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                    char *dport,char *action,char *log,char *result);
extern int fwIpfilDelUdp(char *chain,char *sip,char *smask,char *sport,char *dip,char *dmask,
                    char *dport,char *action,char *log,char *result);
extern int fwIpfilIcmp(char *chain,char *sip,char *smask,char *dip,char *dmask,
                    char *action,char *log,char *result);
extern int fwIpfilDelIcmp(char *chain,char *sip,char *smask,char *dip,char *dmask,
                   char *action,char *log,char *result);
extern int fwIpfilStateNew(char *chain,char *sip,char *smask,char *dip,char *dmask,
                    char *action,char *log,char *result);
extern int fwIpfilDelStateNew(char *chain,char *sip,char *smask,char *dip,char *dmask,
                   char *action,char *log,char *result);
extern int fwIpfilStateAck(char *chain,char *sip,char *smask,char *dip,char *dmask,
                    char *action,char *log,char *result);
extern int fwIpfilDelStateAck(char *chain,char *sip,char *smask,char *dip,char *dmask,
                       char *action,char *log,char *result);
                       
extern int fwTimefil(char *chain,char *proto,char *sip,char *smask,char *dip,char *dmask,
                     char *action,char *beginTime,char *endTime,
                     char *week,char *log,char *result);                
extern int fwDelTimefil(char *chain,char *proto,char *sip,char *smask,char *dip,char *dmask,
                     char *action,char *beginTime,char *endTime,
                     char *week,char *log,char *result);
       
extern int fwNatSnat(char *interface,char *sip,char *smask,char *to,char *result);
extern int fwNatDelSnat(char *interface,char *sip,char *smask,char *to,char *result);
extern int fwNatMasq(char *interface,char *sip,char *smask,char *result);
extern int fwNatDelMasq(char *interface,char *sip,char *smask,char *result);
extern int fwNatDnatPort(char *interface,char *proto,char *sip,char *sport,
                     char *dip,char *dport,char *result);
extern int fwNatDelDnatPort(char *interface,char *proto,char *sip,char *sport,
                     char *dip,char *dport,char *result);
extern int fwNatDnatIP(char *interface,char *proto,char *sip,char *dip,char *result);
extern int fwNatDelDnatIP(char *interface,char *proto,char *sip,char *dip,char *result);
extern int fwNatRedi(char *interface,char *proto,char *fromport,char *toport,char *result);
extern int fwNatDelRedi(char *interface,char *proto,char *fromport,char *toport,char *result);
 
extern int fwBlacklistLoadFile(char *fileName,char *result);
extern int fwBlacklistRemoveFile(char *fileName,char *result);
extern int fwBlacklistLoadUrl(char *fileName,char *result);
extern int fwBlacklistRemoveUrl(char *fileName,char *result);

//Disk Quota Section (added by Ricky Chen 20070111)
extern int diskMgrQuotaStatus(char *status,char *partition,char *result);
extern int diskMgrQuotaList(char *result);
extern int diskMgrQuotaSet(char *username,char *size,char *mountpoint,char *result);
extern int diskMgrQuotaDelete(char *username,char *partition,char *result);
extern int diskMgrQuotaShow(char *starttime,char *endtime,char *times,char *result);
extern int diskMgrQuotaStatusShow(char *result);













extern int firewallShowConfig(struct fwFilterListEntry *filter,struct fwNatListEntry *nat,char *result);
//extern int showFwConfig(char *result);


// By CHK 2007-01-17 
// Resource Manager's APIs
// Cpu Mgr APIs
int ros_cpu_mgr_set_record_time(char **zRsltPtr, const char* zDuration);
int ros_cpu_mgr_set_record_stop(char **zRsltPtr);
int ros_cpu_mgr_show_cpu(char **zRsltPtr, const int nRsltLen, const int nIntervalSec, const char * zBegTime, const char * zEndTime, const char * zIntervalTime);
int ros_cpu_mgr_show_process(char **zRsltPtr, const int nRsltLen, const int nIntervalSec, const char * zBegTime, const char * zEndTime, const int nTopProcNum); 

// Memory Mgr APIs
int ros_mem_mgr_set_record_time(char **zRsltPtr, const char* zDuration);
int ros_mem_mgr_set_record_stop(char **zRsltPtr);
int ros_mem_mgr_show_mem(char **zRsltPtr, const int nRsltLen, const char * zBegTime, const char * zEndTime, const char * zIntervalTime);

// Disk Mgr APIs
#define DISKMGR_PARTITION_FILE 	"/proc/partitions"
#define DISKMGR_MOUNTS_FILE 	"/proc/mounts"
#define DISKMGR_PARTITION_MAX_NUM 	32
#define DISKMGR_HDISK_MAX_NUM 		32
struct ros_harddisk_info
{
	unsigned char hd_id;
	unsigned char num_partitions;
	unsigned long total_space;
	unsigned long space_used;
};
#define ros_harddisk_info_t struct ros_harddisk_info

struct ros_partition_info 
{
	unsigned char partition_id;
	unsigned char hd_id;
	unsigned char is_mounted;
	char mount_path[256];  /* mount from path */
	char mount_dir[256];   /* mount to dir */
	unsigned long space_total;   /* total data blocks in file system */
	unsigned long space_used;    /* other from free blocks in fs */
	unsigned long space_avail;   /* free blocks avail to non-superuser */
};
#define ros_partition_info_t struct ros_partition_info

// Disk Mgr APIs
int ros_disk_mgr_set_record_time(char **zRsltPtr, const int nRsltLen, const char* zDuration);
int ros_disk_mgr_set_record_stop(char **zRsltPtr);
int ros_disk_mgr_file_type_add(char **zRsltPtr, const char* zType);
int ros_disk_mgr_file_type_remove(char **zRsltPtr, const char* zType);
int ros_disk_mgr_file_type_clear(char **zRsltPtr);
int ros_disk_mgr_file_type_show(char **zRsltPtr);

int ros_get_num_hd();
int ros_get_num_partition(int hd);
int ros_get_harddisk_info(int hd, struct ros_harddisk_info *diskinfo);
int ros_get_partition_info(int hd, int partition, struct ros_partition_info * p );


#endif

