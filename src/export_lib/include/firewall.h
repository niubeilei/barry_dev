#ifndef _INCLUDE_FIREWALL_H
#define _INCLUDE_FIREWALL_H
# include "aosApi.h"
//#include "common.h"

enum
{
	eAosRc_InvalidInterface = eAosRc_FirewallStart+1,
	eAosRc_InvalidChain,
	eAosRc_InvalidRate,
	eAosRc_LoadConfigError,
	eAosRc_SaveConfigError,
	eAosRc_ClearConfigError,
	eAosRc_InitError,
	eAosRc_MinInitError,
	eAosRc_MidInitError,
	eAosRc_MaxInitError,
	eAosRc_DelRuleNotExist,
	eAosRc_InvalidFwAction,
	eAosRc_InvalidFileFormat,
	eAosRc_RuleIsExist,
	//eAosRc_FileNotFound,
	//eAosRc_FileFailRead,
};

//firewall
struct fwFilterListEntry
{
	char type[32];
	char chain[16];
	char proto[16];
	char sip[32];
	char smask[32];
	char smac[64];
	char sport[32];
	char dip[32];
	char dmask[32];
	char dport[32];
	char action[32];
	char beginTime[64];
	char endTime[64];
	char week[32];
	char rate[32];
	char log[16];
	//struct aos_list_head datalist;
};

struct fwNatListEntry
{
	char type[32];
	char interface[16];
	char proto[16];
	char sip[32];
	char smask[32];
	char sport[64];
	char dip[32];
	char dmask[32];
	char dport[64];	
	char to[32];	
	char fromport[64];	
	char toport[64];	
	//struct aos_list_head datalist;
}; 

int aos_firewall_retrieve_config(char* buf, int bufsize);
int aos_firewall_clear_config();
int aos_firewall_init(u8 level);

int aos_firewall_syncookie(u8 status);

int aos_firewall_vpn_pass_ipsec(u8 status);
int aos_firewall_vpn_pass_pptp(u8 status);
int aos_firewall_vpn_pass_l2tp(u8 status);

int aos_firewall_outcard_ping(u8 status);

int aos_firewall_add_filter(struct fwFilterListEntry *rules);
int aos_firewall_del_filter(struct fwFilterListEntry *rules);
int aos_firewall_add_nat(struct fwNatListEntry *rules);
int aos_firewall_del_nat(struct fwNatListEntry *rules);

int aos_firewall_anti_syn_add(char *dip, u32 dport, u32 rate);
int aos_firewall_anti_syn_del(char *dip, u32 dport, u32 rate);
int aos_firewall_anti_icmp_add(char *dip, u32 rate);
int aos_firewall_anti_icmp_del(char *dip, u32 rate);

int aos_firewall_antiattack_outcard(char *dip, u32 rate);

int aos_firewall_blacklist_file_add(char *fname);
int aos_firewall_blacklist_file_del(char *fname);
int aos_firewall_blacklist_url_add(char *url);
int aos_firewall_blacklist_url_del(char *url);

#endif

