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
//#include "Util/OmnNew.h"
//#include "XmlParser/XmlItem.h"

#include <string.h>
#include "Global.h"
#define L2TP_PORT		1701	/* UDP */

#define MAX_INTERFACE 16
#define MAX_LIMIT_RATE 600000
#define MAX_PORTS 16
int fwInit()
{
	sgFwVpnPass.ipsecStat = "off";
	sgFwVpnPass.pptpStat = "off";
	sgFwVpnPass.l2tpStat = "off";
	AOS_INIT_LIST_HEAD(&sgFwFilterList);
	AOS_INIT_LIST_HEAD(&sgFwNatList);

	return 0;
}

int fwMacfilAll(OmnString &chain, OmnString &smac, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		//cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p all ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilAll";
	data->proto = "all";
	data->chain = chain;
	data->smac = smac;	
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwMacfilAll(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	action = parms->mStrings[2];
	log = parms->mStrings[3];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];
	
	ret = fwMacfilAll(chain, smac, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwMacfilIcmp(OmnString &chain, OmnString &smac, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		//cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilIcmp";
	data->proto = "icmp";
	data->chain = chain;
	data->smac = smac;	
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwMacfilIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	action = parms->mStrings[2];
	log = parms->mStrings[3];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];
	
	ret = fwMacfilIcmp(chain, smac, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}

int fwMacfilTcp(OmnString &chain, OmnString &smac, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	fwCmd << " -p tcp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	else
	{
		log = "nolog";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}

	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwMacfilTcp";
	data1->proto = "tcp";
	data1->chain = chain;
	data1->smac = smac;
	data1->sport = sport;	
	data1->dip = dipStr;
	data1->dmask = dmaskStr;
	data1->dport = dport;
	data1->action = action;
	data1->log = log;
	aos_list_add_tail((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwMacfilTcp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	sport = parms->mStrings[2];
	dport = parms->mStrings[3];
	action = parms->mStrings[4];
	log = parms->mStrings[5];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	ret = fwMacfilTcp(chain, smac, sport, dip, dmask, dport, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}

int fwMacfilUdp(OmnString &chain, OmnString &smac, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	

	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	fwCmd << " -p udp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	else
	{
		log = "nolog";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}

	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwMacfilUdp";
	data1->proto = "udp";
	data1->chain = chain;
	data1->smac = smac;
	data1->sport = sport;	
	data1->dip = dipStr;
	data1->dmask = dmaskStr;
	data1->dport = dport;
	data1->action = action;
	data1->log = log;
	aos_list_add_tail((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwMacfilUdp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	sport = parms->mStrings[2];
	dport = parms->mStrings[3];
	action = parms->mStrings[4];
	log = parms->mStrings[5];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	ret = fwMacfilUdp(chain, smac, sport, dip, dmask, dport, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwMacfilDelAll(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip, dmask;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	action = parms->mStrings[2];
	log = parms->mStrings[3];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check valid
	/*
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		error = "Error: Invalid source MAC!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p all ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwMacfilAll") && (ptr->chain == chain) && \
				(ptr->smac == smac) && \
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwMacfilDelIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 dip, dmask;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	action = parms->mStrings[2];
	log = parms->mStrings[3];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check valid
	/*
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		error = "Error: Invalid source MAC!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwMacfilIcmp") && (ptr->chain == chain) && \
				(ptr->smac == smac) && \
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwMacfilDelTcp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 dip, dmask;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	sport = parms->mStrings[2];
	dport = parms->mStrings[3];
	action = parms->mStrings[4];
	log = parms->mStrings[5];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check valid
	/*
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		error = "Error: Invalid source MAC!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	free(buffer);
	if((sport.retrieveInt(0, len, value)) && (portlen(value) == sport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(sport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The source port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(dport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The destination port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	//end check
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwMacfilTcp") && (ptr->chain == chain) && 
				(ptr->smac == smac) && (ptr->sport == sport) &&  
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && (ptr->dport == dport) && 
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwMacfilDelUdp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 dip, dmask;
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	smac = parms->mStrings[1];
	sport = parms->mStrings[2];
	dport = parms->mStrings[3];
	action = parms->mStrings[4];
	log = parms->mStrings[5];
	dip = parms->mIntegers[0];
	dmask = parms->mIntegers[1];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check valid
	/*
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		error = "Error: Invalid source MAC!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	free(buffer);
	if((sport.retrieveInt(0, len, value)) && (portlen(value) == sport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(sport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The source port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(dport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The destination port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	//end check
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p udp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwMacfilUdp") && (ptr->chain == chain) && 
				(ptr->smac == smac) && (ptr->sport == sport) &&  
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && (ptr->dport == dport) && 
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwIpfilAll(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p all ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilAll";
	data->proto = "all";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwIpfilAll(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
	
	ret = fwIpfilAll(chain, sip, smask, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilIcmp(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilIcmp";
	data->proto = "icmp";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwIpfilIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString smac;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
	
	ret = fwIpfilIcmp(chain, sip, smask, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilTcp(OmnString &chain, u32 sip, u32 smask, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilTcp";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->sport = sport;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwIpfilTcp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	sport = parms->mStrings[1];
	dport = parms->mStrings[2];
	action = parms->mStrings[3];
	log = parms->mStrings[4];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	ret = fwIpfilTcp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilUdp(OmnString &chain, u32 sip, u32 smask, OmnString &sport, u32 dip, u32 dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p udp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilUdp";
	data->proto = "udp";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->sport = sport;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwIpfilUdp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	sport = parms->mStrings[1];
	dport = parms->mStrings[2];
	action = parms->mStrings[3];
	log = parms->mStrings[4];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	ret = fwIpfilUdp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilStateNew(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -m state --state NEW ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}		
			
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilStateNew";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	
	return 0;
}

int fwIpfilStateNew(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
	
	ret = fwIpfilStateNew(chain, sip, smask, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}

int fwIpfilStateAck(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	fwCmd << "/sbin/iptables -A ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -m state --state ESTABLISHED,RELATED ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilStateAck";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}

int fwIpfilStateAck(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
	
	ret = fwIpfilStateAck(chain, sip, smask, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilDelAll(OmnString &chain, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p all ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;
	
	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		rslt = "Error:Invalid action set!";
		return -eAosRc_InvalidFwAction;
	}
	
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilAll") && (ptr->chain == chain) && (ptr->sip == sipStr) && \
				(ptr->smask == smaskStr) && (ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		rslt = "Error: The rule not exist !";
		//strncpy(errmsg, error.data(),errlen-1);
		//errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	
	return 0;
}

int fwIpfilDelAll(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString chain;
	OmnString action;
	OmnString log;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
		
	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check
	*/
	
	ret = fwIpfilDelAll(chain, sip, smask, dip, dmask, action, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwIpfilDelIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString action;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;
	
	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilIcmp") && (ptr->chain == chain) && (ptr->sip == sipStr) && \
				(ptr->smask == smaskStr) && (ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwIpfilDelTcp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;
	
	chain = parms->mStrings[0];
	sport = parms->mStrings[1];
	dport = parms->mStrings[2];
	action = parms->mStrings[3];
	log = parms->mStrings[4];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((sport.retrieveInt(0, len, value)) && (portlen(value) == sport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(sport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The source port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(dport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The destination port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilTcp") && (ptr->chain == chain) && 
				(ptr->sip == sipStr) && (ptr->smask == smaskStr) && (ptr->sport == sport) &&  
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && (ptr->dport == dport) &&  
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwIpfilDelUdp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;
	
	chain = parms->mStrings[0];
	sport = parms->mStrings[1];
	dport = parms->mStrings[2];
	action = parms->mStrings[3];
	log = parms->mStrings[4];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((sport.retrieveInt(0, len, value)) && (portlen(value) == sport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(sport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The source port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(dport.data());

		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The destination port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -p udp ";
	fwCmd << " -s " << sipStr << "/" << smaskStr << " --sport " << sport;
	fwCmd << " -d " << dipStr << "/" << dmaskStr << " --dport " << dport;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilUdp") && (ptr->chain == chain) && 
				(ptr->sip == sipStr) && (ptr->smask == smaskStr) && (ptr->sport == sport) &&  
				(ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && (ptr->dport == dport) &&  
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwIpfilDelStateNew(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	*/
	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -m state --state NEW ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilStateNew") && (ptr->chain == chain) && (ptr->sip == sipStr) && \
				(ptr->smask == smaskStr) && (ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}


int fwIpfilDelStateAck(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sport;
	OmnString dport;
	OmnString action;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;

	chain = parms->mStrings[0];
	action = parms->mStrings[1];
	log = parms->mStrings[2];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination netmask is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		error = "Error: The action is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	*/

	fwCmd << "/sbin/iptables -D ";
	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		error = "Error:Invalid chain name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}
	fwCmd << " -m state --state ESTABLISHED,RELATED ";
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		error = "Error:Invalid action set!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwIpfilStateAck") && (ptr->chain == chain) && (ptr->sip == sipStr) && \
				(ptr->smask == smaskStr) && (ptr->dip == dipStr) && (ptr->dmask == dmaskStr) && \
				(ptr->action == action) && (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
		}
	}	

	*optlen = index;
	return 0;
}

int fwTimefil(OmnString &chain, OmnString &proto, u32 sip, u32 smask, u32 dip, u32 dmask, OmnString &action,OmnString &beginTime,OmnString &endTime,OmnString &week, OmnString &log, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	OmnString fwCmd;
	OmnString fwLogCmd;
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));
	
	fwCmd << "/sbin/iptables -A ";

	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		//cout << "Error:Invalid chain name!" << endl;
		return -eAosRc_InvalidChain;
	}

	if (proto == "tcp")
	{
		fwCmd << "-p tcp ";
	}
	else if (proto == "udp")
	{
		fwCmd << "-p udp";
	}
	else if (proto == "icmp")
	{
		fwCmd << "-p icmp";
	}
	else if (proto == "all")
	{
		fwCmd << "-p all";
	}
	else
	{
		cout << "Error:Invalid protocol" << endl;
		return -eAosRc_InvalidProtocol;
	}
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	fwCmd << " -m time" << " --timestart " << beginTime  << " --timestop " << endTime \
			<< " --days " << week ;

	if (log == "log")
	{
		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
		OmnCliSysCmd::doShell(fwLogCmd, rslt);
		//cout << fwLogCmd << endl;
	}
	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -eAosRc_InvalidFwAction;
	}

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwTimefil";
	data->proto = proto;
	data->chain = chain;
	data->sip = sipStr;
	data->smask = smaskStr;
	data->dip = dipStr;
	data->dmask = dmaskStr;
	data->action = action;
	data->beginTime = beginTime;
	data->endTime = endTime;
	data->week = week;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwCmd, rslt);

	return 0;
}


int fwTimefil(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString proto;
	OmnString chain;
	OmnString action;
	OmnString log;
	OmnString beginTime;
	OmnString endTime;
	OmnString week;
	u32 sip;
	u32 smask;
	u32 dip;
	u32 dmask;

	char* buf = CliUtil_getBuff(data);

	chain = parms->mStrings[0];
	proto = parms->mStrings[1];
	action = parms->mStrings[2];
	beginTime = parms->mStrings[3];
	endTime = parms->mStrings[4];
	week = parms->mStrings[5];
	log = parms->mStrings[6];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];
	
	ret = fwTimefil(chain, proto, sip, smask, dip, dmask, action, beginTime, endTime, week, log, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwDelTimefil(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString proto;
	OmnString action;
	OmnString beginTime;
	OmnString endTime;
	OmnString week;
	OmnString log;
	u32 sip, smask, dip, dmask;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dmaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	int len = 0;
	int value = 0;
	OmnString error;
	OmnString rslt;
	
	chain = parms->mStrings[0];
	proto = parms->mStrings[1];
	action = parms->mStrings[2];
	beginTime = parms->mStrings[3];
	endTime = parms->mStrings[4];
	week = parms->mStrings[5];
	log = parms->mStrings[6];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dmask = parms->mIntegers[3];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dmask;
	strcpy(dmaskStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		rslt << "Error: The source IP is invalid!" ;
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		rslt << "Error: The source netmask is invalid!" ;
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		rslt << "Error: The destination IP is invalid!" ;
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		rslt << "Error: The destination netmask is invalid!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((action == "permit") || (action == "deny") || (action == "reject")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		flag =1;
	}
	else
	{
		rslt << "Error: The action is invalid!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	//judge the time is set or not
	if((beginTime == 0) || (endTime == 0) || (week == 0) ||(week == "log"))
	{
		rslt << "Error:Time is not set well!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if ((week == "Mon") || (week == "Tue") || (week == "Wed") || (week == "Thu") || (week == "Fri") || (week == "Sat") || (week == "Sun")  || (week == "Everyday")  || (week == "everyday"))
	{
		// judge the week is valid	
	}
	else
	{   
		rslt << "Error:Week is not set well!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	// end check
	*/
	fwCmd << "/sbin/iptables -D ";

	if (chain == "in")
	{
		fwCmd << " INPUT ";
	}
	else if (chain == "out")
	{
		fwCmd << " OUTPUT ";
	}
	else if (chain == "forward")
	{
		fwCmd << " FORWARD ";
	}
	else
	{
		rslt<< "Error:Invalid chain name!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidChain;
	}

	if (proto == "tcp")
	{
		fwCmd << "-p TCP ";
	}
	else if (proto == "udp")
	{
		fwCmd << "-p UDP";
	}
	else if (proto == "icmp")
	{
		fwCmd << "-p icmp";
	}
	else if (proto == "all")
	{
		fwCmd << "-p all";
	}
	else
	{
		rslt << "Error:Invalid protocol";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidProtocol;
	}
	fwCmd << " -s " << sipStr << "/" << smaskStr;
	fwCmd << " -d " << dipStr << "/" << dmaskStr;

	if (log == "log")
	{
		fwLogCmd << fwCmd;
		if ((week == "everyday") || (week == "Everyday" ))
		{
			fwLogCmd << " -m time" << " --timestart " 
				<< beginTime  << " --timestop " << endTime ;

		}
		else 
		{
			fwLogCmd << " -m time" << " --timestart " 
				<< beginTime  << " --timestop " << endTime \
				<< " --days " << week ;
		}

		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
	}

	if (action == "permit")
	{
		fwCmd << " -j ACCEPT ";
	}
	else if (action == "deny")
	{
		fwCmd << " -j DROP ";
	}
	else if (action == "reject")
	{
		fwCmd << " -j REJECT ";		
	}
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		rslt << "Error:Invalid action set!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidFwAction;
	}

	if ((week == "everyday") || (week == "Everyday" ))
	{
		fwCmd << " -m time" << " --timestart " 
			<< beginTime  << " --timestop " << endTime ;

	}
	else 
	{
		fwCmd << " -m time" << " --timestart " 
			<< beginTime  << " --timestop " << endTime \
			<< " --days " << week ;
	}

	//cout << "the timefil cmd is : " << fwCmd << endl;
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwTimefil") && (ptr->proto == proto) 
				&& (ptr->chain == chain) && (ptr->sip == sipStr) && \
				(ptr->smask == smaskStr) && (ptr->dip == dipStr) && \
				(ptr->dmask == dmaskStr) && (ptr->action == action)\
				&& (ptr->beginTime == beginTime)&& (ptr->endTime == endTime)&& \
				(ptr->week == week)&& (ptr->log == log)) 
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		rslt<< "Error: The rule not exist !";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwCmd, rslt);
		//cout << "the fwTimefil cmd is : " << fwCmd;

		if (log == "log")
		{
			OmnCliSysCmd::doShell(fwLogCmd, rslt);
			//cout << "the fwTimefil cmd is : " << fwLogCmd;

		}
	}

	*optlen = index;
	return 0;
}


int fwAntiSyn(u32 dip, OmnString &dport, OmnString &rate, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	fwAntiInDenyCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -I FORWARD ";
	fwAntiInCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -I FORWARD ";

	fwAntiInDenyCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiForwardDenyCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiInCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiForwardCmd << "-p tcp -d " << dipStr << " --dport " << dport;

	fwAntiInDenyCmd << " -j DROP";
	fwAntiForwardDenyCmd << " -j DROP";
	if(rate == "")
	{
		fwAntiInCmd << " --syn -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " --syn -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
	}
	else
	{
		fwAntiInCmd << " --syn -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " --syn -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
	}
	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwAntiSyn";
	data1->dip = dipStr;
	data1->dport = dport;
	data1->rate = rate;
	aos_list_add((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);

	return 0;
}

int fwAntiDelIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	OmnString rate;
	u32 dip;
	char dipStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	OmnString error;
	OmnString rslt;

	rate = parms->mStrings[0];
	dip = parms->mIntegers[0];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The protect IP is invalid!" ;
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((rate.retrieveInt(0, len, value)) && (ratelen(value) == rate.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		//ok;
	}
	else
	{
		rate = "";
	}
	*/
	fwAntiInDenyCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -D FORWARD ";
	fwAntiInCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -D FORWARD ";

	fwAntiInDenyCmd << "-p icmp -d " << dipStr;
	fwAntiForwardDenyCmd << "-p icmp -d " << dipStr;
	fwAntiInCmd << "-p icmp -d " << dipStr;
	fwAntiForwardCmd << "-p icmp -d " << dipStr;

	fwAntiInDenyCmd << " -j DROP";
	fwAntiForwardDenyCmd << " -j DROP";
	if (rate == "")
	{
		fwAntiInCmd << " -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
	}
	else
	{
		fwAntiInCmd << " -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
	}
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwAntiIcmp") && (ptr->dip == dipStr) && (ptr->rate == rate))
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !" ;
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);
	}	

	*optlen = index;
	return 0;
}


int fwAntiIcmp(u32 dip, OmnString &rate, OmnString &rslt)
{
	char dipStr[MAX_IP_ADDR_LEN];
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	fwAntiInDenyCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -I FORWARD ";
	fwAntiInCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -I FORWARD ";

	fwAntiInDenyCmd << "-p icmp -d " << dipStr;
	fwAntiForwardDenyCmd << "-p icmp -d " << dipStr;
	fwAntiInCmd << "-p icmp -d " << dipStr;
	fwAntiForwardCmd << "-p icmp -d " << dipStr;

	fwAntiInDenyCmd << " -j DROP";
	fwAntiForwardDenyCmd << " -j DROP";
	if (rate == "")
	{
		fwAntiInCmd << " -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
	}
	else
	{
		fwAntiInCmd << " -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
	}
	struct fwFilterListEntry * data1;
	if((data1 = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data1, 0, sizeof(struct fwFilterListEntry));
	data1->type = "fwAntiIcmp";
	data1->dip = dipStr;
	data1->rate = rate;
	aos_list_add((struct aos_list_head *)&data1->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);
	return 0;
}

int fwAntiIcmp(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{	
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString rate;
	u32 dip;

	char* buf = CliUtil_getBuff(data);

	dip = parms->mIntegers[0];
	rate = parms->mStrings[0];
	
	ret = fwAntiIcmp(dip, rate, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwAntiSyn(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString rate;
	OmnString dport;
	u32 dip;

	char* buf = CliUtil_getBuff(data);

	dip = parms->mIntegers[0];
	dport = parms->mStrings[0];
	rate = parms->mStrings[1];
	
	ret = fwAntiSyn(dip, dport, rate, rslt);

	if(ret < 0)
	{
		strncpy(errmsg, rslt, errlen-1);
		errmsg[errlen-1] = 0;
	}
	else
	{
		strcpy(local, rslt);
		CliUtil_checkAndCopy(buf, &index, *optlen, local, strlen(local));
		*optlen = index;
	}
	return ret;
}


int fwAntiDelSyn(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	OmnString dport;
	OmnString rate;
	u32 dip;
	char dipStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;
	OmnString error;
	OmnString rslt;

	dport = parms->mStrings[0];
	rate = parms->mStrings[1];
	dip = parms->mIntegers[0];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The protect IP is invalid!" ;
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
			(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		error = "Error: The protect Port is invalid!" ;
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}

	if((rate.retrieveInt(0, len, value)) && (ratelen(value) == rate.length()) && \
			(value > 0) && (value <= MAX_LIMIT_RATE))
	{
		//ok;
	}
	else
	{
		rate = "";
	}
*/
	fwAntiInDenyCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -D FORWARD ";
	fwAntiInCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -D FORWARD ";

	fwAntiInDenyCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiForwardDenyCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiInCmd << "-p tcp -d " << dipStr << " --dport " << dport;
	fwAntiForwardCmd << "-p tcp -d " << dipStr << " --dport " << dport;

	fwAntiInDenyCmd << " -j DROP";
	fwAntiForwardDenyCmd << " -j DROP";
	if (rate == "")
	{
		fwAntiInCmd << " --syn -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " --syn -m limit --limit 2/m --limit-burst 1 -j ACCEPT";
	}
	else 
	{
		fwAntiInCmd << " --syn -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
		fwAntiForwardCmd << " --syn -m limit --limit " << rate << "/m --limit-burst 1 -j ACCEPT";
	}
	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwAntiSyn") && \
				(ptr->dip == dipStr) && (ptr->dport == dport) && (ptr->rate == rate))
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		error = "Error: The rule not exist !" ;
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
		OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);
	}	

	*optlen = index;
	return 0;
}

extern int fwOutcardPing(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString oncmd;
	OmnString deloncmd;
	OmnString offcmd;
	OmnString deloffcmd;
	int ret;
	OmnString status;

	status = parms->mStrings[0]; 
	
	offcmd = "iptables -I INPUT -i eth0 -p icmp -j DROP";
	deloffcmd = "iptables -D INPUT -i eth0 -p icmp -j DROP";
	oncmd = "iptables -I INPUT -i eth0 -p icmp -j ACCEPT";
	deloncmd = "iptables -D INPUT -i eth0 -p icmp -j ACCEPT";

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	bool isExist = false;
	//judge is exist
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	ptr->type == "fwOutcardPing") 
		{
			isExist = true;
			if(ptr->action == status)  
			return -eAosRc_RuleIsExist;
		}
	}
	
	if (status == "off")
	{
		if (isExist)
		{
			if ((ret = system(deloncmd)) != 0) return -eAosRc_SystemError;
		}
		ret = system(offcmd);
		if (ret != 0) return -eAosRc_SystemError;
	}
	else
	{
		if (isExist)
		{
			if ((ret = system(deloffcmd)) != 0) return -eAosRc_SystemError;
		}

		ret = system(oncmd);
		if (ret != 0) return -eAosRc_SystemError;
	}

	if (isExist)
	{
		aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
		{       
			if(	ptr->type == "fwOutcardPing") 
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
		dataptr->type = "fwOutcardPing";
		dataptr->action = status;
		aos_list_add_tail((struct aos_list_head *)&dataptr->datalist, &sgFwFilterList);
	}

	*optlen = 0;
	return 0;
}

extern int fwAntiAttackOutcard(char *data, unsigned int *optlen, 
		struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	OmnString cmd;
	OmnString dropCmd;
	OmnString delCmd;
	OmnString delCmd1;
	OmnString proto;
	OmnString tmpProto;
	OmnString rate;
	OmnString oldRate;

	proto = parms->mStrings[0];
	rate  = parms->mStrings[1];

	struct fwFilterListEntry * ptr;
	struct fwFilterListEntry * tmp;
	bool isExist = false;
	
	if ( rate == "")
	{
		rate = "1";
	}
	
	//cout << "rate is" << rate << endl;
	//judge is exist
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
	{       
		if(	(ptr->type == "fwAntiAttackOutcard") && (ptr->proto == proto)) 
		{
			isExist = true;
			if (ptr->rate == rate) 
			{
				return -eAosRc_RuleIsExist;
			}
			oldRate  = ptr->rate;
		}
	}

	cmd = "/sbin/iptables -I INPUT -i eth0 -p ";
	dropCmd = "/sbin/iptables -I INPUT -i eth0 -p ";
	
	if ( proto == "syn") 
	{
		tmpProto = "tcp --syn";
	}
	else
	{
		tmpProto = "icmp";
	}

	cmd << tmpProto << " -m limit --limit " << rate << "/m -j ACCEPT";
	dropCmd << tmpProto << " -j DROP";
 
	delCmd = "/sbin/iptables -D INPUT -i eth0 -p ";
	delCmd << tmpProto << " -m limit --limit " << oldRate << "/m -j ACCEPT";

	 delCmd1 << "/sbin/iptables -D INPUT -i eth0 -p " << tmpProto << " -j DROP";
	cout << delCmd << endl;
	if (isExist)
	{
		if ( system(delCmd1) != 0 ) return -eAosRc_SystemError;	
		if ( system(delCmd) != 0 ) return -eAosRc_SystemError;	
		if ( system(dropCmd) != 0 ) return -eAosRc_SystemError;
		if ( system(cmd) != 0 ) return -eAosRc_SystemError;

		aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
		{       
			if(	(ptr->type == "fwAntiAttackOutcard") && (ptr->proto == proto)) 
			{
				ptr->rate = rate;
			}
		}
	}
	else
	{
		if ( system(dropCmd) != 0 ) return -eAosRc_SystemError;
		if ( system(cmd) != 0 ) return -eAosRc_SystemError;
		struct fwFilterListEntry * dataptr;
		if((dataptr = (struct fwFilterListEntry *)
			aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
		{
			//printk("malloc error");
			return -eAosRc_MallocError;
		}
		memset(dataptr, 0, sizeof(struct fwFilterListEntry));
		dataptr->type = "fwAntiAttackOutcard";
		dataptr->proto = proto;
		dataptr->rate = rate;
		aos_list_add_tail((struct aos_list_head *)&dataptr->datalist, &sgFwFilterList);
	}

	*optlen = 0;	
	return 0;
}

int FwFilter_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("firewall macfil all", fwMacfilAll);
	ret |= CliUtil_regCliCmd("firewall macfil tcp", fwMacfilTcp);
	ret |= CliUtil_regCliCmd("firewall macfil udp", fwMacfilUdp);
	ret |= CliUtil_regCliCmd("firewall macfil icmp", fwMacfilIcmp);
	ret |= CliUtil_regCliCmd("firewall macfil del all", fwMacfilDelAll);
	ret |= CliUtil_regCliCmd("firewall macfil del tcp", fwMacfilDelTcp);
	ret |= CliUtil_regCliCmd("firewall macfil del udp", fwMacfilDelUdp);
	ret |= CliUtil_regCliCmd("firewall macfil del icmp", fwMacfilDelIcmp);	
	ret |= CliUtil_regCliCmd("firewall ipfil all", fwIpfilAll);
	ret |= CliUtil_regCliCmd("firewall ipfil tcp", fwIpfilTcp);
	ret |= CliUtil_regCliCmd("firewall ipfil udp", fwIpfilUdp);
	ret |= CliUtil_regCliCmd("firewall ipfil icmp", fwIpfilIcmp);
	ret |= CliUtil_regCliCmd("firewall ipfil state new", fwIpfilStateNew);
	ret |= CliUtil_regCliCmd("firewall ipfil state ack", fwIpfilStateAck);
	ret |= CliUtil_regCliCmd("firewall ipfil del all", fwIpfilDelAll);
	ret |= CliUtil_regCliCmd("firewall ipfil del tcp", fwIpfilDelTcp);
	ret |= CliUtil_regCliCmd("firewall ipfil del udp", fwIpfilDelUdp);
	ret |= CliUtil_regCliCmd("firewall ipfil del icmp", fwIpfilDelIcmp);
	ret |= CliUtil_regCliCmd("firewall ipfil del state new", fwIpfilDelStateNew);
	ret |= CliUtil_regCliCmd("firewall ipfil del state ack", fwIpfilDelStateAck);
	ret |= CliUtil_regCliCmd("firewall antiattack icmp", fwAntiIcmp);
	ret |= CliUtil_regCliCmd("firewall antiattack syn", fwAntiSyn);
	ret |= CliUtil_regCliCmd("firewall antiattack del icmp", fwAntiDelIcmp);
	ret |= CliUtil_regCliCmd("firewall antiattack del syn", fwAntiDelSyn);
	ret |= CliUtil_regCliCmd("firewall timefil", fwTimefil);
	ret |= CliUtil_regCliCmd("firewall del timefil", fwDelTimefil);
	ret |= CliUtil_regCliCmd("firewall outcard ping", fwOutcardPing);
	ret |= CliUtil_regCliCmd("firewall antiattack outcard", fwAntiAttackOutcard);
	return ret;
}
