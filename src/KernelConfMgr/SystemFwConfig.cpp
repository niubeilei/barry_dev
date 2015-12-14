////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwFilter.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"
//#include "KernelInterface/CliProc.h"
//#include "KernelInterface/CliCmd.h"
//#include "KernelInterface/CliSysCmd.h"
#include "Tracer/Tracer.h"
#include "Util/File.h"
//#include "Util/IpAddr.h"
#include "Util/OmnNew.h"
#include "XmlParser/XmlItem.h"

#include <string.h>
#define RHC_FIREWALL_CONFIG "/usr/local/rhc/config/firewall.conf"
#include "Global.h"

int initFwConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{

	unsigned int index = 0;
	OmnString rslt;
	// Init the firewall's configurations 
	//
	//Step 1: Load Modules
	//Step 2: Set the params NOTE:must be ROOT
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);

	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
	struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwFilterList);

	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
	{       
		aos_list_del(&nptr->datalist);
		aos_free(nptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Step 6: Open the Rule which sip is 127.0.0.1
	OmnCliSysCmd::doShell("/sbin/iptables -I INPUT -s 127.0.0.1 -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I OUTPUT -s 127.0.0.1 -j ACCEPT", rslt);

	//
	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	*optlen = index;
	return 0;
}



int showFwConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	//
	// Show the firewall current configurations 
	//
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	char local[10000];

	OmnString contents;
	struct fwFilterListEntry * ptr;
	struct fwNatListEntry * nptr;	
	int filRuleNum = 0;
	int natRuleNum = 0;

	contents << "Firewall Information\n"
		<< "----------------------------\n";

	contents << "\nVpn Pass Through:";
	contents << "\n\tipsec_pass status:" << sgFwVpnPass.ipsecStat;
	contents << "\n\t pptp_pass status:" << sgFwVpnPass.pptpStat;
	contents << "\n\t l2tp_pass status:" << sgFwVpnPass.l2tpStat;

	contents << "\n\nFilter Rules:\n";
	aos_list_for_each_entry(ptr, &sgFwFilterList, datalist)
	{
		filRuleNum ++;
		contents << "<Cmd>firewall ";
		if ((ptr->type == "fwIpfilAll") || (ptr->type == "fwIpfilIcmp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateNew")
		{
			contents << "ipfil state new " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateAck")
		{
			contents << "ipfil state ack " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwIpfilTcp") || (ptr->type == "fwIpfilUdp"))
		{
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilAll") || (ptr->type == "fwMacfilIcmp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilTcp") || (ptr->type == "fwMacfilUdp"))
		{
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiSyn")
		{
			contents << "antiattack syn " << ptr->dip << " " << ptr->dport << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiIcmp")
		{
			contents << "antiattack icmp " << ptr->dip << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwTimefil")   
		{   
			contents << "timefil " << ptr->chain << " " << ptr->proto << " ";   
			contents << ptr->sip << " " << ptr->smask << " ";   
			contents << ptr->dip << " " << ptr->dmask << " ";   
			contents << ptr->action << " " << ptr->beginTime<< " ";   
			contents << ptr->endTime << " " << ptr->week << " " ;   
			contents << ptr->log << "</Cmd>\n";   
		} 
		else if (ptr->type == "fwBlacklistFile") 
		{
			contents << "blacklist load file " << ptr->proto << "</Cmd>\n";
		}
		else if (ptr->type == "fwBlacklistUrl")
		{
			contents << "blacklist load url " << ptr->proto << "</Cmd>\n";
		}
		else if (ptr->type == "fwOutcardPing")
		{
			contents << "firewall outcard ping " << ptr->action << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiAttackOutcard")
		{
			contents << "firewall antiattack outcard " << ptr->proto
			<< " " << ptr->rate << "</Cmd>\n";
		}
		else if (ptr->type == "fwSetSyncookie")
		{
			contents << "firewall set syncookie " << ptr->action << "</Cmd>\n";
		}
	
		//printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		//printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;

	contents << "\nNat Rules:\n";
	aos_list_for_each_entry(nptr, &sgFwNatList, datalist)
	{
		natRuleNum ++;
		if (nptr->type == "fwNatSnat")
		{
			contents << "<Cmd>firewall snat " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->to;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatMasq")
		{
			contents << "<Cmd>firewall masq " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatIP")
		{
			contents << "<Cmd>firewall dnat ip " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " ";
			contents << nptr->dip;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatPort")
		{
			contents << "<Cmd>firewall dnat port " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->sport << " ";
			contents << nptr->dip << " " << nptr->dport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "fwNatRedi")
		{
			contents << "<Cmd>firewall redi " << nptr->interface << " " << nptr->proto << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "webwallRedi")
		{
			contents << "<Cmd>webwall redi " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "webwallDnat")
		{
			contents << "<Cmd>webwall dnat " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << " " << nptr->to << "</Cmd>\n";
		}
		else
		{	
			cout << "Error: the nat list data is wrong!\n";
		}
	}
	//	cout << "------config : firewall------\n" << contents << endl;

	strncpy(local, contents.data(), 10000);

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
	*optlen = index;

	return 0;
}

int initFwMaxsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P FORWARD ACCEPT", rslt);

	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
	struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwFilterList);

	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
	{       
		aos_list_del(&nptr->datalist);
		aos_free(nptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Step 6: Open the Rule which sip is 127.0.0.1
	OmnCliSysCmd::doShell("/sbin/iptables -I INPUT -s 127.0.0.1 -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I OUTPUT -s 127.0.0.1 -j ACCEPT", rslt);


	//Step 7: Allow state ack
	OmnCliSysCmd::doShell("/sbin/iptables -I FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I OUTPUT -m state --state ESTABLISHED,RELATED -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I INPUT -m state --state ESTABLISHED,RELATED -j ACCEPT", rslt);

	*optlen = index;
	return 0;
}

extern int initFwMinsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);

	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
	struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwFilterList);

	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
	{       
		aos_list_del(&nptr->datalist);
		aos_free(nptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Step 6: Open the Rule which sip is 127.0.0.1
	OmnCliSysCmd::doShell("/sbin/iptables -I INPUT -s 127.0.0.1 -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I OUTPUT -s 127.0.0.1 -j ACCEPT", rslt);

	*optlen = index;
	return 0;
}

extern int initFwMidsecConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_forward", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/ip_dynaddr", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/tcp_syncookies", rslt);
	OmnCliSysCmd::doShell("/bin/echo \"1\" > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts", rslt);
	//Step 3: Set the policies
	OmnCliSysCmd::doShell("/sbin/iptables -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -P FORWARD DROP", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -P OUTPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P PREROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P INPUT ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P POSTROUTING ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P FORWARD ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -P OUTPUT ACCEPT", rslt);

	//Step 4: Clear the rules
	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -F", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -X", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -t mangle -X", rslt);
	//cout <<"clear fw test ===result:" << rslt;

	//step 5: Clear the LIST
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
	struct fwNatListEntry * ntmp;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwFilterList);

	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
	{       
		aos_list_del(&nptr->datalist);
		aos_free(nptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	//Step 6: Open the Rule which sip is 127.0.0.1
	OmnCliSysCmd::doShell("/sbin/iptables -I INPUT -s 127.0.0.1 -j ACCEPT", rslt);
	OmnCliSysCmd::doShell("/sbin/iptables -I OUTPUT -s 127.0.0.1 -j ACCEPT", rslt);

	//Step 7: Allow state ack	
	OmnCliSysCmd::doShell("/sbin/iptables -I FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT", rslt);
	//Show the current config
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);
	*optlen = index;
	return 0;

}

extern int clearFwConfig(OmnString &rslt)
{
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	struct fwNatListEntry * nptr;
	struct fwNatListEntry * ntmp;

	sgFwVpnPass.ipsecStat = "off";
	sgFwVpnPass.pptpStat = "off";
	sgFwVpnPass.l2tpStat = "off";

	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		aos_list_del(&ptr->datalist);
		aos_free(ptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwFilterList);

	aos_list_for_each_entry_safe(nptr, ntmp, &sgFwNatList, datalist)
	{       
		aos_list_del(&nptr->datalist);
		aos_free(nptr);
	}
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	OmnCliSysCmd::doShell("/sbin/iptables -F", rslt);
	//cout <<"clear fw test ===result:" << rslt;
	OmnCliSysCmd::doShell("/sbin/iptables -t nat -F", rslt);
	//cout <<"clear fw test ===result:" << rslt;
	//OmnCliSysCmd::doShell("/sbin/iptables -L", rslt);

	return 0;
}

extern int clearFwConfig(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt;
	//
	// Clear the firewall's configurations 
	//

	clearFwConfig(rslt);

	*optlen = index;
	return 0;
}

int saveFwConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString rslt, fn;
	OmnFilePtr mFile;

	// kevin 03/06/2007
	/*
	   fn = RHC_FIREWALL_CONFIG;

	   if (!mFile)
	   {
	   OmnString tfn = fn;
	   tfn << ".active";
	   mFile = OmnNew OmnFile(tfn, OmnFile::eCreate);
	   if (!mFile || !mFile->isGood())
	   {
	   rslt = "Failed to open configure file: ";
	   rslt << tfn;
	   return aos_alarm(eAosMD_Platform, eAosAlarm_FailedToOpenFile, 
	   "%s", rslt.data());
	   }
	   }
	   */

	//unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	//char dns[20];
	//char local[256];

	//sgDhcp.ip1 = parms->mStrings[0];
	//sgDhcp.ip2 = parms->mStrings[1];

	//strcpy(local, sgDhcp.dns.getBuffer());
	//CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	//*optlen = index;
	//return 0;

	OmnString contents;

	struct fwFilterListEntry * ptr;
	struct fwNatListEntry * nptr;	
	int filRuleNum = 0;
	int natRuleNum = 0;

	if (sgFwVpnPass.ipsecStat == "on" || sgFwVpnPass.ipsecStat == "On")
		contents << "<Cmd>firewall ipsec_pass on</Cmd>\n";
	if (sgFwVpnPass.pptpStat == "on" || sgFwVpnPass.pptpStat == "On")
		contents << "<Cmd>firewall pptp_pass on</Cmd>\n";
	if (sgFwVpnPass.l2tpStat == "on" || sgFwVpnPass.l2tpStat == "On")
		contents << "<Cmd>firewall l2tp_pass on</Cmd>\n";

	aos_list_for_each_entry(ptr, &sgFwFilterList, datalist)
	{
		filRuleNum ++;
		if ((ptr->type == "fwIpfilAll") || (ptr->type == "fwIpfilIcmp"))
		{
			contents << "<Cmd>firewall ";
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateNew")
		{
			contents << "<Cmd>firewall ";
			contents << "ipfil state new " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwIpfilStateAck")
		{
			contents << "<Cmd>firewall ";
			contents << "ipfil state ack " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwIpfilTcp") || (ptr->type == "fwIpfilUdp"))
		{
			contents << "<Cmd>firewall ";
			contents << "ipfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->sip << " " << ptr->smask << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilAll") || (ptr->type == "fwMacfilIcmp"))
		{
			contents << "<Cmd>firewall ";
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " ";
			contents << ptr->dip << " " << ptr->dmask << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if ((ptr->type == "fwMacfilTcp") || (ptr->type == "fwMacfilUdp"))
		{
			contents << "<Cmd>firewall ";
			contents << "macfil " << ptr->proto << " " << ptr->chain << " ";
			contents << ptr->smac << " " << ptr->sport << " ";
			contents << ptr->dip << " " << ptr->dmask << " " << ptr->dport << " ";
			contents << ptr->action << " " << ptr->log << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiSyn")
		{
			contents << "<Cmd>firewall ";
			contents << "antiattack syn " << ptr->dip << " " << ptr->dport << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiIcmp")
		{
			contents << "<Cmd>firewall ";
			contents << "antiattack icmp " << ptr->dip << " " << ptr->rate;
			contents << "</Cmd>\n";
		}
		else if (ptr->type == "fwTimefil")   
		{   
			contents << "<Cmd>firewall ";
			contents << "timefil " << ptr->chain << " " << ptr->proto << " ";   
			contents << ptr->sip << " " << ptr->smask << " ";   
			contents << ptr->dip << " " << ptr->dmask << " ";   
			contents << ptr->action << " " << ptr->beginTime<< " ";   
			contents << ptr->endTime << " " << ptr->week << " " ;   
			contents << ptr->log << "</Cmd>\n";   
		} 
		else if (ptr->type == "fwBlacklistFile")  
		{   
			contents << "<Cmd>firewall ";
			contents << "blacklist load file " << ptr->proto << " " << "</Cmd>\n";   
		}
		else if (ptr->type == "fwBlacklistUrl")  
		{   
			contents << "<Cmd>firewall ";
			contents << "blacklist load url " << ptr->proto << " " << "</Cmd>\n";   
		}
		else if (ptr->type == "fwOutcardPing")
		{
			contents << "<Cmd>firewall outcard ping " << ptr->action << "</Cmd>\n";
		}
		else if (ptr->type == "fwAntiAttackOutcard")
		{
			contents << "<Cmd>firewall antiattack outcard " << ptr->proto
			<< " " << ptr->rate << "</Cmd>\n";
		}
		else if (ptr->type == "fwSetSyncookie")
		{
			contents << "<Cmd>firewall set syncookie " << ptr->action << "</Cmd>\n";
		}
		//printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		//printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;

	aos_list_for_each_entry(nptr, &sgFwNatList, datalist)
	{
		natRuleNum ++;
		if (nptr->type == "fwNatSnat")
		{
			contents << "<Cmd>firewall snat " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->to;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatMasq")
		{
			contents << "<Cmd>firewall masq " << nptr->interface << " ";
			contents << nptr->sip << " " << nptr->smask;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatIP")
		{
			contents << "<Cmd>firewall dnat ip " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->smask << " ";
			contents << nptr->dip;
			contents << "</Cmd>\n";
		}
		else if (nptr->type == "fwNatDnatPort")
		{
			contents << "<Cmd>firewall dnat port " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->sip << " " << nptr->smask << " " << nptr->sport << " ";
			contents << nptr->dip << " " << nptr->dport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "fwNatRedi")
		{
			contents << "<Cmd>firewall redi " << nptr->interface << " ";
			contents << nptr->proto << " " << nptr->fromport << " " << nptr->toport;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "webwallRedi")
		{
			contents << "<Cmd>webwall redi " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << "</Cmd>\n";
		}	
		else if (nptr->type == "webwallDnat")
		{
			contents << "<Cmd>webwall dnat " << nptr->interface << " ";
			contents << nptr->fromport << " " << nptr->toport << " " << nptr->dip;
			contents << " " << nptr->to <<"</Cmd>\n";
		}	
		else
		{	
			cout << "Error: the nat list data is wrong!\n";
			//		return -1;
		}
		//printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		//printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		//
	}
	//cout << "The filter rules num is: " << ruleNum << "and the list is:" << contents << endl;

	// kevin 03/06/2007
	/*
	   char local[1000];
	   sprintf(local, "------AosCliBlock: firewall------\n");
	   if (!mFile->put(local, false) ||
	   !mFile->put("<Commands>\n", false) ||
	   !mFile->put(contents, false) ||
	   !mFile->put("</Commands>\n", false) ||
	   !mFile->put("------EndAosCliBlock------\n\n", false))
	   {
	   rslt = "Failed to write to config: ";
	   rslt << fn;
	   ret = aos_alarm(eAosMD_Platform, eAosAlarm_FailedToWriteConfig,
	   "%s", rslt.data());
	   }

	// 
	// Back up the current config
	//
	OmnFile file(fn, OmnFile::eReadOnly);
	OmnString cmd;
	if (file.isGood())
	{
	OmnString newFn = fn;
	newFn << ".bak";
	cmd << "cp " << fn << " " << newFn;
	OmnTrace << "To run command: " << cmd << endl;
	system(cmd);
	}

	mFile->closeFile();
	mFile = 0;

	cmd = "mv -f ";

	cmd << fn << ".active " << fn;
	system(cmd);
	OmnTrace << "To run command: " << cmd << endl;
	*/

	char local[4096];
	strcpy(local, contents.data());
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;

}

int loadFwConfig(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	// Clear the old config
	OmnFilePtr mFile;
	OmnString rslt;
	OmnString fn;

	fn = RHC_FIREWALL_CONFIG;

	OmnString loadShellCmd;

	if (!mFile)
	{
		mFile = OmnNew OmnFile(fn, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << fn;
			return eAosAlarm_FailedToOpenFile;
		}
	}

	OmnString type;
	OmnString beginTime;   
	OmnString endTime;   
	OmnString week; 
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
	OmnString rate;
	OmnString log;
	OmnString interface;
	OmnString to;	
	OmnString toip;	
	OmnString fromport;	
	OmnString toport;	

	OmnString contents, cmd, fwPrefix, subPrefix;
	OmnString start = "------AosCliBlock: ";
	start << "firewall" << "------\n";
	OmnString end = "------EndAosCliBlock------\n";
	bool err;

	if (!mFile->readBlock(start, end, contents, err))
	{
		if (err)
		{
			rslt << "********** Failed to read config for: ";
			rslt << "firewall" << "\n";
			return -eAosAlarm_FailedToReadFile;
		}

		return 0;
	}

	try
	{
		// 
		// Clear the module's config first
		//
		clearFwConfig(rslt);
		//cout << "The old config be cleared!" << endl;

		OmnXmlItem config(contents);
		config.reset();
		while (config.hasMore())
		{
			OmnXmlItemPtr item = config.next();
			try
			{
				cmd = item->getStr();
				//cout << "Found command: " << cmd << endl;
				//			if (!OmnCliProc::getSelf()->runCli(cmd, rslt))
				int curPos = 0;
				curPos = cmd.getWord(curPos,fwPrefix);
				if (fwPrefix == "firewall")
				{
					curPos = cmd.getWord(curPos,subPrefix);

					if(subPrefix == "ipfil")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "all")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilAll(chain, inet_addr(sip), inet_addr(smask), inet_addr(dip), inet_addr(dmask), action, log, rslt);
						}
						else if(subPrefix == "tcp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilTcp(chain, inet_addr(sip), inet_addr(smask), sport, inet_addr(dip), inet_addr(dmask), dport, action, log, rslt);
						}
						else if(subPrefix == "udp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilUdp(chain, inet_addr(sip), inet_addr(smask), sport, inet_addr(dip), inet_addr(dmask), dport, action, log, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwIpfilIcmp(chain, inet_addr(sip), inet_addr(smask), inet_addr(dip), inet_addr(dmask), action, log, rslt);
						}
						else if(subPrefix == "state")
						{
							curPos = cmd.getWord(curPos,subPrefix);
							if(subPrefix == "new")
							{
								curPos = cmd.getWord(curPos,chain);
								curPos = cmd.getWord(curPos,sip);
								curPos = cmd.getWord(curPos,smask);
								curPos = cmd.getWord(curPos,dip);
								curPos = cmd.getWord(curPos,dmask);
								curPos = cmd.getWord(curPos,action);
								curPos = cmd.getWord(curPos,log);
								fwIpfilStateNew(chain, inet_addr(sip), inet_addr(smask), inet_addr(dip), inet_addr(dmask), action, log, rslt);
							}
							else if(subPrefix == "ack")
							{
								curPos = cmd.getWord(curPos,chain);
								curPos = cmd.getWord(curPos,sip);
								curPos = cmd.getWord(curPos,smask);
								curPos = cmd.getWord(curPos,dip);
								curPos = cmd.getWord(curPos,dmask);
								curPos = cmd.getWord(curPos,action);
								curPos = cmd.getWord(curPos,log);
								fwIpfilStateAck(chain, inet_addr(sip), inet_addr(smask), inet_addr(dip), inet_addr(dmask), action, log, rslt);
							}
							else
							{
								cout << "Error:The State SubPrefix is wrong!" << endl;
								continue;
							}
						}
						else 
						{
							cout << "Error:The Ipfil SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "macfil")
					{	
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "all")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilAll(chain, smac, inet_addr(dip), inet_addr(dmask), action, log, rslt);
						}
						else if(subPrefix == "tcp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,sport);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,dport);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilTcp(chain, smac, sport, inet_addr(dip), inet_addr(dmask), dport, action, log, rslt);
						}
						else if(subPrefix == "udp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilUdp(chain, smac, sport, inet_addr(dip), inet_addr(dmask), dport, action, log, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilIcmp(chain, smac, inet_addr(dip), inet_addr(dmask), action, log, rslt);
						}
						else 
						{
							cout << "Error:The Macfil SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "snat")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						curPos = cmd.getWord(curPos,to);
						fwNatSnat(interface, inet_addr(sip), inet_addr(smask), inet_addr(to), rslt);
					}
					else if(subPrefix == "masq")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						fwNatMasq(interface, inet_addr(sip), inet_addr(smask), rslt);
					}
					else if(subPrefix == "dnat")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "ip")
						{
							curPos = cmd.getWord(curPos,interface);
							curPos = cmd.getWord(curPos,proto);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,dip);
							fwNatDnatIP(interface, proto, inet_addr(sip), inet_addr(dip), rslt);
						}
						else if(subPrefix == "port")
						{
							curPos = cmd.getWord(curPos,interface);
							curPos = cmd.getWord(curPos,proto);
							curPos = cmd.getWord(curPos,sip);
							curPos = cmd.getWord(curPos,smask);
							curPos = cmd.getWord(curPos,sport);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dport);
							fwNatDnatPort(interface, proto, inet_addr(sip), atoi(sport), inet_addr(dip), atoi(dport), rslt);
						}
						else
						{
							cout << "Error:The Dnat SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else if(subPrefix == "timefil")   
					{   
						curPos = cmd.getWord(curPos,chain);   
						curPos = cmd.getWord(curPos,proto);   
						curPos = cmd.getWord(curPos,sip);   
						curPos = cmd.getWord(curPos,smask);   
						curPos = cmd.getWord(curPos,dip);   
						curPos = cmd.getWord(curPos,dmask);   
						curPos = cmd.getWord(curPos,action);   
						curPos = cmd.getWord(curPos,beginTime);   
						curPos = cmd.getWord(curPos,endTime);   
						curPos = cmd.getWord(curPos,week);   
						curPos = cmd.getWord(curPos,log);   
						/* cout << chain << " " << proto << " " <<  sip << " " << smask << " " << dip << " " << dmask \   
						   << " " << action << " " << beginTime << " " << endTime << " " << week << " " << log << endl;   */
						fwTimefil(chain, proto, inet_addr(sip), inet_addr(smask), inet_addr(dip), inet_addr(dmask), action, beginTime, endTime, week,log, rslt);   
					}
					else if(subPrefix == "redi")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,proto);
						curPos = cmd.getWord(curPos,fromport);
						curPos = cmd.getWord(curPos,toport);
						fwNatRedi(interface, proto, atoi(fromport), atoi(toport), rslt);
					}
					else if (subPrefix == "fwBlacklistFile")
					{
						curPos = cmd.getWord(curPos,proto);
						fwBlacklistLoadFile(proto, rslt);
					}
					else if (subPrefix == "fwBlacklistUrl")
					{
						curPos = cmd.getWord(curPos,proto);
						fwBlacklistLoadUrl(proto, rslt);
					}
					else if(subPrefix == "antiattack")
					{
						curPos = cmd.getWord(curPos,subPrefix);
						if(subPrefix == "syn")
						{
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dport);
							curPos = cmd.getWord(curPos,rate);
							fwAntiSyn(inet_addr(dip), dport, rate, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,rate);
							fwAntiIcmp(inet_addr(dip), rate, rslt);
						}
						else
						{
							cout << "Error:The Second SubPrefix is wrong!" << endl;
							continue;
						}
					}
					else 
					{
						cout << "Error:The First SubPrefix is wrong!" << endl;
						continue;
					}

				}
				else if (fwPrefix == "webwall")
				{
					curPos = cmd.getWord(curPos,subPrefix);
					if(subPrefix == "redi")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,fromport);
						curPos = cmd.getWord(curPos,toport);
						curPos = cmd.getWord(curPos,dip);
						webwallRedi(interface, atoi(fromport), atoi(toport), inet_addr(dip), rslt);
					}
					else if(subPrefix == "dnat")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,fromport);
						curPos = cmd.getWord(curPos,toport);
						curPos = cmd.getWord(curPos,dip);
						curPos = cmd.getWord(curPos,toip);
						webwallDnat(interface, atoi(fromport), atoi(toport), inet_addr(dip), inet_addr(toip), rslt);

					}

				}
				else
				{	
					cout << "Error:This is a bad firewall command!" << endl;
					continue;
				}
			}

			catch (const OmnExcept &e)
			{
				rslt = "Failed to read command: ";
				rslt << item->toString();
				//return -eAosAlarm_FailedToLoadConfig;
				return -1;
			}
		}
	}

	catch (const OmnExcept &e)
	{
		cout << "Failed to load configure for module: " 
			<< "firewall" << endl;
		//return -eAosAlarm_FailedToLoadConfig;
		return -1;
	}

	mFile->closeFile();
	mFile = 0;

	char* buf = CliUtil_getBuff(data);
	char local[1000];
	strcpy(local, "Firewall's config load ok!");
	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;

}

int showFwLog(OmnString &rslt)
{                       
	// for Gentoo FC, and Debian
	OmnCliSysCmd::doShell("/bin/more /var/log/messages | grep -m 100 AOS_FW_LOG", rslt);
	// NOTE: the -m NUM is the MAX_LINE will be printed
	return 0;       
}


int showFwLog(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString rslt;
	unsigned int index = 0;
	char* buf = CliUtil_getBuff(data);
	char local[10000];

	showFwLog(rslt);
	//char dns[20];

	strncpy(local, rslt.data(), 10000);

	CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));

	*optlen = index;
	return 0;
}

int fwSetSyncookie(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;	
	int ret;
	OmnString status;
	OmnString cmd;
	bool isExist = false;

	status = parms->mStrings[0];

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	//judge is exist
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if( ptr->type == "fwSetSyncookie") 
		{
			isExist = true;
			if (ptr->action == status) 
			{
				return -eAosRc_RuleIsExist;
			}
		}
	}
	if ( status == "on")
	{
		cmd = " echo \"1\" >/proc/sys/net/ipv4/tcp_syncookies";	
	}
	else
	{
		cmd = " echo \"0\" >/proc/sys/net/ipv4/tcp_syncookies";	
	}

	if ( (ret = system(cmd)) != 0) return -eAosRc_SystemError;	
	
	if (isExist)
	{
		aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
		{       
			if(ptr->type == "fwSetSyncookie") 
			{
				ptr->action = status; 
			}
		}
	}
	else
	{
		struct fwFilterListEntry * dataptr;
		if((dataptr = (struct fwFilterListEntry *)
			aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
		{
			//printk("malloc error");
			return -eAosRc_MallocError;
		}
		memset(dataptr, 0, sizeof(struct fwFilterListEntry));
		dataptr->type = "fwSetSyncookie";
		dataptr->action = status;
		aos_list_add_tail((struct aos_list_head *)&dataptr->datalist, &sgFwFilterList);
	}
	
	*optlen = index;
	return 0; 
}

int FwConfig_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("firewall show config", showFwConfig);
	ret |= CliUtil_regCliCmd("firewall clear config",clearFwConfig);
	ret |= CliUtil_regCliCmd("firewall save config", saveFwConfig);
	ret |= CliUtil_regCliCmd("firewall load config", loadFwConfig);
	ret |= CliUtil_regCliCmd("firewall maxsec init", initFwMaxsecConfig);
	ret |= CliUtil_regCliCmd("firewall midsec init", initFwMidsecConfig);
	ret |= CliUtil_regCliCmd("firewall minsec init", initFwMinsecConfig);
	ret |= CliUtil_regCliCmd("firewall init", initFwConfig);
	ret |= CliUtil_regCliCmd("firewall show log", showFwLog);
	ret |= CliUtil_regCliCmd("firewall set syncookie", fwSetSyncookie);

	return ret;
}
