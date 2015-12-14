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
#ifdef AOS_OLD_CLI
#include "CliFwMisc.h"

extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;

int 
AosCli::fwIpfilAll(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;
	
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
		return -1;
	}
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilAll";
	data->proto = "all";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwIpfilTcp(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwIpfilTcp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);
	
	return 0;
}
	

int 
AosCli::fwIpfilTcp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -s " << sip << "/" << smask << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
		return -1;
	}
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilTcp";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->sport = sport;
	data->dip = dip;
	data->dmask = dmask;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwIpfilUdp(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwIpfilUdp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);

	return 0;
}


int 
AosCli::fwIpfilUdp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p udp ";
	fwCmd << " -s " << sip << "/" << smask << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
		return -1;
	}
			
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilUdp";
	data->proto = "udp";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->sport = sport;
	data->dip = dip;
	data->dmask = dmask;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwIpfilIcmp(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);

	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	
	fwIpfilIcmp(chain, sip, smask, dip, dmask, action, log, rslt);
	
	return 0;
}


int 
AosCli::fwIpfilIcmp(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilIcmp";
	data->proto = "icmp";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwIpfilStateNew(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwIpfilStateNew(chain, sip, smask, dip, dmask, action, log, rslt);
	
	return 0;
}


int 
AosCli::fwIpfilStateNew(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -m state --state NEW ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}		
			
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilStateNew";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwIpfilStateAck(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwIpfilStateAck(chain, sip, smask, dip, dmask, action, log, rslt);
	
	return 0;
}


int 
AosCli::fwIpfilStateAck(OmnString &chain, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -m state --state ESTABLISHED,RELATED ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwIpfilStateAck";
	data->proto = "tcp";
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwMacfilAll(OmnString &rslt)
{
	OmnString chain;
	OmnString smac;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwMacfilAll(chain, smac, dip, dmask, action, log, rslt);

	return 0;
}


int 
AosCli::fwMacfilAll(OmnString &chain, OmnString &smac, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;
	
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
		return -1;
	}
	fwCmd << " -p all ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}		

	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilAll";
	data->proto = "all";
	data->chain = chain;
	data->smac = smac;	
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwMacfilTcp(OmnString &rslt)
{
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);

	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwMacfilTcp(chain, smac, sport, dip, dmask, dport, action, log, rslt);

	return 0;
}


int 
AosCli::fwMacfilTcp(OmnString &chain, OmnString &smac, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
		return -1;
	}		
	
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilTcp";
	data->proto = "tcp";
	data->chain = chain;
	data->smac = smac;
	data->sport = sport;	
	data->dip = dip;
	data->dmask = dmask;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwMacfilUdp(OmnString &rslt)
{
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwMacfilUdp(chain, smac, sport, dip, dmask, dport, action, log, rslt);

	return 0;
}


int 
AosCli::fwMacfilUdp(OmnString &chain, OmnString &smac, OmnString &sport, OmnString &dip, OmnString &dmask, OmnString &dport, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p udp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
		return -1;
	}		
	
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilUdp";
	data->proto = "udp";
	data->chain = chain;
	data->smac = smac;	
	data->sport = sport;
	data->dip = dip;
	data->dmask = dmask;
	data->dport = dport;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwMacfilIcmp(OmnString &rslt)
{
	OmnString chain;
	OmnString smac;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
		
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwMacfilIcmp(chain, smac, dip, dmask, action, log, rslt);

	return 0;
}


int 
AosCli::fwMacfilIcmp(OmnString &chain, OmnString &smac, OmnString &dip, OmnString &dmask, OmnString &action, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;

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
		return -1;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
		return -1;
	}		
		
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwMacfilIcmp";
	data->proto = "icmp";
	data->chain = chain;
	data->smac = smac;	
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);
	
	//cout << "the fwIpAll cmd is : " << fwCmd << endl;	
	OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}

int 
AosCli::fwIpfilDelAll(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check

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
		return -1;
	}
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
	
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilAll") && (ptr->chain == chain) && (ptr->sip == sip) && \
        	(ptr->smask == smask) && (ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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

	
int 
AosCli::fwIpfilDelTcp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check

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
		return -1;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -s " << sip << "/" << smask << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
		
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilTcp") && (ptr->chain == chain) && 
			(ptr->sip == sip) && (ptr->smask == smask) && (ptr->sport == sport) &&  
			(ptr->dip == dip) && (ptr->dmask == dmask) && (ptr->dport == dport) &&  
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
        cout << "Error: The rule not exist !" << endl;
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
	

int 
AosCli::fwIpfilDelUdp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	//end check
	
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
		return -1;
	}
	fwCmd << " -p udp ";
	fwCmd << " -s " << sip << "/" << smask << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
			
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilUdp") && (ptr->chain == chain) && 
        	(ptr->sip == sip) && (ptr->smask == smask) && (ptr->sport == sport) &&  
			(ptr->dip == dip) && (ptr->dmask == dmask) && (ptr->dport == dport) &&  
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwIpfilDelIcmp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
			
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check

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
		return -1;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
		
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilIcmp") && (ptr->chain == chain) && (ptr->sip == sip) && \
        	(ptr->smask == smask) && (ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwIpfilDelStateNew(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

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
		return -1;
	}
	fwCmd << " -m state --state NEW ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
		
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilStateNew") && (ptr->chain == chain) && (ptr->sip == sip) && \
        	(ptr->smask == smask) && (ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwIpfilDelStateAck(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
		
	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

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
		return -1;
	}
	fwCmd << " -m state --state ESTABLISHED,RELATED ";
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
		
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwIpfilStateAck") && (ptr->chain == chain) && (ptr->sip == sip) && \
        	(ptr->smask == smask) && (ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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


//begin fw_mac_filter
int 
AosCli::fwMacfilDelAll(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
		
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

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
		return -1;
	}
	fwCmd << " -p all ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
			
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwMacfilAll") && (ptr->chain == chain) && \
			(ptr->smac == smac) && \
        	(ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwMacfilDelTcp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
	
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	//end check
		
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
		return -1;
	}
	fwCmd << " -p tcp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
				
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwMacfilTcp") && (ptr->chain == chain) && 
        	(ptr->smac == smac) && (ptr->sport == sport) &&  
			(ptr->dip == dip) && (ptr->dmask == dmask) && (ptr->dport == dport) && 
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwMacfilDelUdp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString sport;
	OmnString dip;
	OmnString dmask;
	OmnString dport;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
		
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The source port is invalid!" << endl;
			return -1;
		}
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
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
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The destination port is invalid!" << endl;
			return -1;
		}
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	//end check
		
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
		return -1;
	}
	fwCmd << " -p udp ";
	fwCmd << " -m mac --mac-source " << smac << " --sport " << sport;
	fwCmd << " -d " << dip << "/" << dmask << " --dport " << dport;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
					
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwMacfilUdp") && (ptr->chain == chain) && 
        	(ptr->smac == smac) && (ptr->sport == sport) &&  
			(ptr->dip == dip) && (ptr->dmask == dmask) && (ptr->dport == dport) && 
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
        cout << "Error: The rule not exist !" << endl;
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


int 
AosCli::fwMacfilDelIcmp(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString chain;
	OmnString smac;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;
	
	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,smac);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);
		
	//check valid
	int macArray[6];
	char * buffer = strdup(smac.data());
	if(parse_mac(buffer, macArray) != 0)
	{
		free(buffer);
		cout << "Error: Invalid source MAC!" << endl;
		return -1;
	}
	free(buffer);
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

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
		return -1;
	}
	fwCmd << " -p icmp ";
	fwCmd << " -m mac --mac-source " << smac;
	fwCmd << " -d " << dip << "/" << dmask;
	
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
				
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwMacfilIcmp") && (ptr->chain == chain) && \
			(ptr->smac == smac) && \
        	(ptr->dip == dip) && (ptr->dmask == dmask) && \
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
        cout << "Error: The rule not exist !" << endl;
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

int 
AosCli::fwIpfilAll(OmnString &rslt)
{
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,log);

	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwIpfilAll(chain, sip, smask, dip, dmask, action, log, rslt);

	return 0;
}


int 
AosCli::fwTimefil(OmnString &rslt)
{
	OmnString proto;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString beginTime;
	OmnString endTime;
	OmnString week;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,beginTime);
	curPos = mFileName.getWord(curPos,endTime);
	curPos = mFileName.getWord(curPos,week);
	curPos = mFileName.getWord(curPos,log);

	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
	{
		//ok
	}
	else if((action.retrieveInt(0, len, value)) && (ratelen(value) == action.length()))
	{
		flag =1;
	}
	else
	{
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}

	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}

	fwTimefil(chain, proto, sip, smask, dip, dmask, action, beginTime, endTime, week, log, rslt);

	return 0;
}


int 
AosCli::fwTimefil(OmnString &chain, OmnString &proto, OmnString &sip, OmnString &smask, OmnString &dip, OmnString &dmask, OmnString &action,OmnString &beginTime,OmnString &endTime,OmnString &week, OmnString &log, OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	u32 len = 0;
	int value = 0;
	
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
		return -1;
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
	else
	{
		cout << "Error:Invalid protocol" << endl;
		return -1;
	}
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
	if (log == "log")
	{
		fwCmd << " -m time" << " --timestart " << beginTime  << " --timestop " << endTime \
	<< " --days " << week ;

		fwLogCmd << fwCmd; 
		fwLogCmd << " -j LOG --log-prefix AOS_FW_LOG: ";
//		OmnCliSysCmd::doShell(fwLogCmd, rslt);
        cout << fwLogCmd << endl;
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
		return -1;
	}
		
	fwCmd << " -m time" << " --timestart " << beginTime  << " --timestop " << endTime \
	<< " --days " << week ;
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwTimefil";
	data->proto = proto;
	data->chain = chain;
	data->sip = sip;
	data->smask = smask;
	data->dip = dip;
	data->dmask = dmask;
	data->action = action;
	data->beginTime = beginTime;
	data->endTime = endTime;
	data->week = week;
	data->log = log;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	cout << "the fwIpAll cmd is : " << fwCmd << endl;	
    //OmnCliSysCmd::doShell(fwCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::webwallRedi(OmnString &rslt)
{
	OmnString interface;
	OmnString fromport;
	OmnString toport;
	OmnString dip;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,fromport);
	curPos = mFileName.getWord(curPos,toport);
	curPos = mFileName.getWord(curPos,dip);
	
	//check the valid
	/*
  	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	*/
	if((fromport.retrieveInt(0, len, value)) && (portlen(value) == fromport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(fromport.data());
	
		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The from port is invalid!" << endl;
			return -1;
		}
	}
	if((toport.retrieveInt(0, len, value)) && (portlen(value) == toport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The to port is invalid!" << endl;
		return -1;
	}
	
	webwallRedi(interface, fromport, toport, dip, rslt);

	return 0;
}


int 
AosCli::webwallRedi(OmnString &interface, OmnString &fromport, OmnString &toport, OmnString &dip, OmnString &rslt)
{
	OmnString fwNatCmd;
	
	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		fwNatCmd << " -i " << interface;
		//cout << "Invalid interface name!" << endl;
		//return -1;
	}
	
	fwNatCmd << " -p tcp --dport " << fromport << " -d ! " << dip;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toport;
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "webwallRedi";
	data->interface = interface;
	data->dip = dip;	
	data->fromport = fromport;
	data->toport = toport;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	//cout << "the webwallRedi cmd is : " << fwNatCmd << endl;	
	cout << "webwallRedi " <<endl;	
	//OmnCliSysCmd::doShell(fwNatCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}

int 
AosCli::webwallDelRedi(OmnString &rslt)
{
	OmnString fwNatCmd;
	OmnString interface;
	OmnString dip;
	OmnString fromport;
	OmnString toport;
	int curPos = 0;
	u32 len = 0;
	int value = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,fromport);
	curPos = mFileName.getWord(curPos,toport);
	curPos = mFileName.getWord(curPos,dip);
	
	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	*/
	if((fromport.retrieveInt(0, len, value)) && (portlen(value) == fromport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		unsigned int ports[MAX_PORTS];
		char * buffer = strdup(fromport.data());
	
		if((parse_multi_ports(buffer, ports) == 2) && (ports[0] <= ports[1]) && \
			(strlen(buffer) == portlen(ports[0]) + portlen(ports[1]) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			cout << "Error: The from port is invalid!" << endl;
			return -1;
		}
	}
	if((toport.retrieveInt(0, len, value)) && (portlen(value) == toport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The to port is invalid!" << endl;
		return -1;
	}
	
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		fwNatCmd << " -i " << interface;
	}

	fwNatCmd << " -p tcp --dport " << fromport << " -d ! " << dip;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toport;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "webwallRedi") && (ptr->interface == interface) && \
			(ptr->dip == dip) && (ptr->fromport == fromport) && (ptr->toport == toport)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
	cout << "webwall Del Redi " <<endl;	
    /*if (existflag == 0) 
    {  
        cout << "Error: The rule not exist !" << endl;
    }
    else
    {
        OmnCliSysCmd::doShell(fwNatCmd, rslt);
    }*/	

	return 0;
}


int 
AosCli::fwDelTimefil(OmnString &rslt)
{
	OmnString fwCmd;
	OmnString fwLogCmd;
	OmnString proto;
	OmnString chain;
	OmnString sip;
	OmnString smask;
	OmnString dip;
	OmnString dmask;
	OmnString action;
	OmnString beginTime;
	OmnString endTime;
	OmnString week;
	OmnString log;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	int flag = 0;

	curPos = mFileName.getWord(curPos,chain);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dmask);
	curPos = mFileName.getWord(curPos,action);
	curPos = mFileName.getWord(curPos,beginTime);
	curPos = mFileName.getWord(curPos,endTime);
	curPos = mFileName.getWord(curPos,week);
	curPos = mFileName.getWord(curPos,log);

	//check the valid
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source netmask is invalid!" << endl;
		return -1;
	}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination IP is invalid!" << endl;
		return -1;
	}
	if((dmask != "0") && (dmask != "0.0.0.0") && (OmnIpAddr(dmask) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination netmask is invalid!" << endl;
		return -1;
	}
	if((action == "permit") || (action == "deny")) 
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
		cout << "Error: The action is invalid!" << endl;
		return -1;
	}
	if((log != "log")||(flag == 1))
	{
		log = "nolog";
	}
	// end check

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
		return -1;
	}

    if (proto == "tcp")
	{
		fwCmd << "-p TCP ";
	}
	else if (chain == "udp")
	{
		fwCmd << "-p UDP";
	}
    else if (chain == "all")
	{
		fwCmd << "-p all";
	}
	else
	{
		cout << "Error:Invalid protocol" << endl;
		return -1;
	}
	fwCmd << " -s " << sip << "/" << smask;
	fwCmd << " -d " << dip << "/" << dmask;
	
	if (log == "log")
	{
	 	fwCmd << " -m time" << " --timestart " << beginTime  << " --timestop " << endTime \
	<< " --days " << week ;
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
	else if(action.retrieveInt(0, len, value))
	{
		fwCmd << " -m limit --limit " << action << "/m";
		fwCmd << " -j ACCEPT ";
	}
	else
	{
		cout << "Error:Invalid action set!" << endl;
		return -1;
	}
	
	fwCmd << " -m time" << " --timestart " << beginTime  << " --timestop " << endTime \
	<< " --days " << week ;
	
cout << "the fwIpAll cmd is : " << fwCmd << endl;
	struct fwFilterListEntry * ptr;
    struct fwFilterListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwFilterList, datalist)
    {       
    	if(	(ptr->type == "fwTimefil") && (ptr->proto == proto) && (ptr->chain == chain)\
		&& (ptr->sip == sip) && \
 	(ptr->smask == smask) && (ptr->dip == dip) && (ptr->dmask == dmask) && \
        	(ptr->action == action) && (ptr->beginTime == beginTime)&& (ptr->endTime == endTime)&& (ptr->week == week)&& (ptr->log == log)) 
   		{       
        	aos_list_del(&ptr->datalist);
    	    aos_free(ptr);
	        existflag = 1;
			break;
      	}            
    }
    if (existflag == 0) 
    {   
        cout << "Error: The rule not exist !" << endl;
    }
    else
    {
        //OmnCliSysCmd::doShell(fwCmd, rslt);
        cout << "the fwIpAll cmd is : " << fwCmd << endl;
        
		if (log == "log")
		{
			//OmnCliSysCmd::doShell(fwLogCmd, rslt);
			cout << "the fwIpAll cmd is : " << fwLogCmd << endl;
			
		}
    }	

	return 0;
}
#endif

