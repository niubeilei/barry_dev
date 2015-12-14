////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwMisc.cpp
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
 
// kevin 07/18/2006 
// parse the port
// format: port or port1:port2
int string_to_number_ll(const char *s, unsigned long long min, unsigned long long max, unsigned long long *ret)
{
	unsigned long long number;
	char *end;

	number = strtoull(s, &end, 0);
	if (*end == '\0' && end != s) {
		if ( min <= number && (!max || number <= max)) {
			*ret = number;
			return 0;
		}
	}
	return -1;
}

int string_to_number_l(const char *s, unsigned long min, unsigned long max, unsigned long *ret)
{
	int result;
	unsigned long long number;

	result = string_to_number_ll(s, min, max, &number);
	*ret = (unsigned long)number;

	return result;
}

int string_to_number(const char *s, unsigned int min, unsigned int max, unsigned int *ret)
{
	int result;
	unsigned long number;

	result = string_to_number_l(s, min, max, &number);
	*ret = (unsigned int)number;

	return result;
}

unsigned int parse_port(const char *port)
{
	unsigned int portnum;

	if ((string_to_number(port, 0, 65535, &portnum)) != -1 )
	{	
		return (int)portnum;
	}
	else
	{
		printf("Error: invalid port!\n");
	}
	return -1;	
}

unsigned int parse_multi_ports(char *portstring, unsigned int *ports)
{
	char *buffer, *cp, *next;
	unsigned int i;

	buffer = strdup(portstring);

	for (cp=buffer, i=0; cp && i<MAX_PORTS; cp=next,i++)
	{
		next=strchr(cp, ':');
		if (next) *next++='\0';
		ports[i] = parse_port(cp);
	}
	//if (cp) printf("too many ports specified");
	free(buffer);
	return i;
}

int portlen(int port)
{
	//unsigned int len;
	if (port < 10) {return 1;}
	else if (port < 100) {return 2;}
	else if (port < 1000) {return 3;}
	else if (port < 10000) {return 4;}
	else if (port < 65536) {return 5;}
	else { cout << "Error: The port is too large!" << endl; return -1;}	
}

int ratelen(int port)
{
	//unsigned int len;
	if (port < 10) {return 1;}
	else if (port < 100) {return 2;}
	else if (port < 1000) {return 3;}
	else if (port < 10000) {return 4;}
	else if (port < 100000) {return 5;}
	else if (port <= 600000) {return 6;}
	else { cout << "Error: The rate is too large!" << endl; return -1;}	
}

int parse_mac(char *mac, int *macarray)
{
	unsigned int i = 0;

	if (strlen(mac) != 6*3-1)
	{
		cout << "Error: Invalid mac address(Length)!" << endl;
		return -1;
	}
	
	for (i = 0; i < 6; i++) 
	{
		long number;
		char *end;
		number = strtol(mac + i*3, &end, 16);
		if (end == mac + i*3 + 2 && number >= 0 && number <= 255)
			macarray[i] = number;
		else
		{
			cout << "Error: Invalid mac address!" << endl;
			return -1;
		}
	}
	return 0;
}
int 
AosCli::fwRunCommand(OmnString &cmd, OmnString &rslt)
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
	OmnString log;
	OmnString interface;
	OmnString to;	

	OmnString contents, fwPrefix, subPrefix;
	//bool err;

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
							fwIpfilAll(chain, sip, smask, dip, dmask, action, log, rslt);
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
							fwIpfilTcp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);
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
							fwIpfilUdp(chain, sip, smask, sport, dip, dmask, dport, action, log, rslt);
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
							fwIpfilIcmp(chain, sip, smask, dip, dmask, action, log, rslt);
						}
						else 
						{
							cout << "Error:The Ipfil SubPrefix is wrong!" << endl;
							return -1;
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
							fwMacfilAll(chain, smac, dip, dmask, action, log, rslt);
						}
						else if(subPrefix == "tcp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilTcp(chain, smac, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "udp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilUdp(chain, smac, sport, dip, dmask, dport, action, log, rslt);
						}
						else if(subPrefix == "icmp")
						{
							curPos = cmd.getWord(curPos,chain);
							curPos = cmd.getWord(curPos,smac);
							curPos = cmd.getWord(curPos,dip);
							curPos = cmd.getWord(curPos,dmask);
							curPos = cmd.getWord(curPos,action);
							curPos = cmd.getWord(curPos,log);
							fwMacfilIcmp(chain, smac, dip, dmask, action, log, rslt);
						}
						else 
						{
							cout << "Error:The Macfil SubPrefix is wrong!" << endl;
							return -1;
						}
					}
					else if(subPrefix == "snat")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						curPos = cmd.getWord(curPos,to);
						fwNatSnat(interface, sip, smask, to, rslt);
					}
					else if(subPrefix == "masq")
					{
						curPos = cmd.getWord(curPos,interface);
						curPos = cmd.getWord(curPos,sip);
						curPos = cmd.getWord(curPos,smask);
						fwNatMasq(interface, sip, smask, rslt);
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
							fwNatDnatIP(interface, proto, sip, dip, rslt);
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
							fwNatDnatPort(interface, proto, sip, sport, dip, dport, rslt);
						}
						else
						{
							cout << "Error:The Dnat SubPrefix is wrong!" << endl;
							return -1;
						}
					}
					else 
					{
						cout << "Error:The First SubPrefix is wrong!" << endl;
						return -1;
					}
				}
				else
				{	
					cout << "Error:This is a bad firewall command!" << endl;
					return -1;
				}


	return 0;
}



int 
AosCli::fwRules(OmnString &rslt)
{
	//
	// Operate the firewall's rules, include the filter rules, NAT rules and the policy
	//
	OmnString ruleShellCmd;
	ruleShellCmd << "/sbin/iptables " << mFileName;
	//cout << "the load cmd is : " << ruleShellCmd << endl;	
	OmnCliSysCmd::doShell(ruleShellCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}

int 
AosCli::fwAntiSyn(OmnString &rslt)
{
	OmnString dip;
	OmnString dport;
	OmnString rate;
	int curPos = 0;
	int value = 0;
	u32 len = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,rate);

	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The protect IP is invalid!" << endl;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The protect Port is invalid!" << endl;
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
	fwAntiSyn(dip, dport, rate, rslt);

	return 0;
}


int 
AosCli::fwAntiSyn(OmnString &dip, OmnString &dport, OmnString &rate, OmnString &rslt)
{
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	
	fwAntiInDenyCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -I FORWARD ";
	fwAntiInCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -I FORWARD ";
	
	fwAntiInDenyCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiForwardDenyCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiInCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiForwardCmd << "-p tcp -d " << dip << " --dport " << dport;

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
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwAntiSyn";
	data->dip = dip;
	data->dport = dport;
	data->rate = rate;
	aos_list_add((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);

	return 0;
}
	

int 
AosCli::fwAntiIcmp(OmnString &rslt)
{
	OmnString dip;
	OmnString rate;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,rate);
	
	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The protect IP is invalid!" << endl;
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

	fwAntiIcmp(dip, rate, rslt);

	return 0;
}


int 
AosCli::fwAntiIcmp(OmnString &dip, OmnString &rate, OmnString &rslt)
{
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	
	fwAntiInDenyCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -I FORWARD ";
	fwAntiInCmd << "/sbin/iptables -I INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -I FORWARD ";
	
	fwAntiInDenyCmd << "-p icmp -d " << dip;
	fwAntiForwardDenyCmd << "-p icmp -d " << dip;
	fwAntiInCmd << "-p icmp -d " << dip;
	fwAntiForwardCmd << "-p icmp -d " << dip;

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
	struct fwFilterListEntry * data;
	if((data = (struct fwFilterListEntry *)aos_malloc(sizeof(struct fwFilterListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwFilterListEntry));
	data->type = "fwAntiIcmp";
	data->dip = dip;
	data->rate = rate;
	aos_list_add((struct aos_list_head *)&data->datalist, &sgFwFilterList);

	OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
	OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);

	return 0;
}


//kevin the del part
int 
AosCli::fwAntiDelSyn(OmnString &rslt)
{
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	OmnString dip;
	OmnString dport;
	OmnString rate;
	int curPos = 0;
	//int flag = 0;
	u32 len = 0;
	int value = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dport);
	curPos = mFileName.getWord(curPos,rate);
	
	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The protect IP is invalid!" << endl;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The protect Port is invalid!" << endl;
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

	fwAntiInDenyCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -D FORWARD ";
	fwAntiInCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -D FORWARD ";
	
	fwAntiInDenyCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiForwardDenyCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiInCmd << "-p tcp -d " << dip << " --dport " << dport;
	fwAntiForwardCmd << "-p tcp -d " << dip << " --dport " << dport;

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
        	(ptr->dip == dip) && (ptr->dport == dport) && (ptr->rate == rate))
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
        OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);
    }	

	return 0;
}


int 
AosCli::fwAntiDelIcmp(OmnString &rslt)
{
	OmnString fwAntiInDenyCmd;
	OmnString fwAntiForwardDenyCmd;
	OmnString fwAntiInCmd;
	OmnString fwAntiForwardCmd;
	OmnString dip;
	OmnString rate;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,rate);
		
	//check the valid
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The protect IP is invalid!" << endl;
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

	
	fwAntiInDenyCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardDenyCmd << "/sbin/iptables -D FORWARD ";
	fwAntiInCmd << "/sbin/iptables -D INPUT ";
	fwAntiForwardCmd << "/sbin/iptables -D FORWARD ";
	
	fwAntiInDenyCmd << "-p icmp -d " << dip;
	fwAntiForwardDenyCmd << "-p icmp -d " << dip;
	fwAntiInCmd << "-p icmp -d " << dip;
	fwAntiForwardCmd << "-p icmp -d " << dip;

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
    	if(	(ptr->type == "fwAntiIcmp") && (ptr->dip == dip) && (ptr->rate == rate))
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
        OmnCliSysCmd::doShell(fwAntiInDenyCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiForwardDenyCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiInCmd, rslt);
        OmnCliSysCmd::doShell(fwAntiForwardCmd, rslt);
    }	

	return 0;
}

	


// create by Weiwang
int
AosCli::fwBlacklistLoadFile(OmnString &rslt)
{
	if (!mFile)
	{		
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward");
		OmnString tmpIp1("0.0.0.0");
		OmnString tmpMask("0.0.0.0");
		OmnString tmpMethod("deny");
		OmnString tmpLogFlag("nolog");
		/*mFileName  << tmpIp1 << tmpMask << ipAddress << " " << \
			netMask << " " << tmpMethod << " " <<tmpLogFlag;
	        //cout<< mFileName << endl;*/
		
		fwIpfilAll(forward,tmpIp1,tmpMask,ipAddress, netMask, tmpMethod, tmpLogFlag, rslt);
		line1 = mFile->getLine(isFileFinished);
//		fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
}

// create by Bin Gong
int
AosCli::fwBlacklistLoadUrl(OmnString &rslt)
{
	OmnString systemCmd;
	systemCmd << "wget -q " << mFileName;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	OmnString shortName;
	unsigned char *startPos;
	unsigned char *curPos;
	unsigned char *endPos;
	startPos = (unsigned char*)mFileName.getBuffer();
	endPos = startPos + mFileName.length();
	curPos = endPos;
	while(curPos >= startPos)
	{
		if(*curPos == '\\' || *curPos == '/')
		{
			shortName.assign((char*)(curPos + 1),endPos - curPos); 
			break;
		}
		curPos --;
	}
	if(shortName == "")
	{
		// pop an alarm
		return false;
	}
	//cout << "shortname is:" << shortName << endl;

	
	if (!mFile)
	{
		mFile = OmnNew OmnFile(shortName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	OmnString line1 = mFile->getLine(isFileFinished);
	while(!isFileFinished)
	{
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward");
		OmnString tmpIp1("0.0.0.0");
		OmnString tmpMask("0.0.0.0");
		OmnString tmpMethod("deny");
		OmnString tmpLogFlag("nolog");
		/*mFileName  << tmpIp1 << tmpMask << ipAddress << " " << \
			netMask << " " << tmpMethod << " " <<tmpLogFlag;
	        //cout<< mFileName << endl;*/
		
		fwIpfilAll(forward,tmpIp1,tmpMask,ipAddress, netMask, tmpMethod, tmpLogFlag, rslt);
		line1 = mFile->getLine(isFileFinished);
//		fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
	
	OmnString rmCmd;
	rmCmd << "rm -f " << shortName;
	OmnCliSysCmd::doShell(rmCmd, rslt);
	
    return 0;
}


//create by Bin Gong
int
AosCli::fwBlacklistRemoveFile(OmnString &rslt)
{
	if (!mFile)
	{
		mFile = OmnNew OmnFile(mFileName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt <<mFileName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << mFileName;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	while(!isFileFinished)
	{
		OmnString line1 = mFile->getLine(isFileFinished);
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward ");
		OmnString tmpIp1("0.0.0.0 ");
		OmnString tmpMask("0.0.0.0 ");
		OmnString tmpMethod("deny ");
		OmnString tmpLogFlag("nolog");
		mFileName = forward;
		mFileName  << tmpIp1 << tmpMask << ipAddress << " " << \
			netMask << " " << tmpMethod << " " <<tmpLogFlag;
	        //cout<< mFileName << endl;
		fwIpfilDelAll(rslt);
                //fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}
}


//create by Bin Gong 11-14 
int
AosCli::fwBlacklistRemoveUrl(OmnString &rslt)
{
    OmnString systemCmd;
	systemCmd << "wget -q " << mFileName;
	OmnCliSysCmd::doShell(systemCmd, rslt);

	OmnString shortName;
	unsigned char *startPos;
	unsigned char *curPos;
	unsigned char *endPos;
	startPos = (unsigned char*)mFileName.getBuffer();
	endPos = startPos + mFileName.length();
	curPos = endPos;
	while(curPos >= startPos)
	{
		if(*curPos == '\\' || *curPos == '/')
		{
			shortName.assign((char*)(curPos + 1),endPos - curPos); 
			break;
		}
		curPos --;
	}
	if(shortName == "")
	{
		// pop an alarm
		return false;
	}
	if (!mFile)
	{
		mFile = OmnNew OmnFile(shortName, OmnFile::eReadOnly);
		if (!mFile)
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}

		if (!mFile->isGood())
		{
			rslt = "Failed to open configure file: ";
			rslt << shortName;
			return eAosAlarm_FailedToOpenFile;
		}
	}
	
	bool isFileFinished = false;
	while(!isFileFinished)
	{
		OmnString line1 = mFile->getLine(isFileFinished);
		OmnString domainName;
		OmnString ipAddress;
		OmnString netMask;
		//cout << line1 << endl;
		int curPos = 0;
		curPos = line1.getWord(curPos,domainName);
		curPos = line1.getWord(curPos,ipAddress);
		curPos = line1.getWord(curPos,netMask);
		//fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", ipAddress, netMask, "deny", "no log", rslt);
		OmnString forward("forward ");
		OmnString tmpIp1("0.0.0.0 ");
		OmnString tmpMask("0.0.0.0 ");
		OmnString tmpMethod("deny ");
		OmnString tmpLogFlag("nolog");
		mFileName = forward;
		mFileName  << tmpIp1 << tmpMask << ipAddress << " " << \
			netMask << " " << tmpMethod << " " <<tmpLogFlag;
	        //cout<< mFileName << endl;
		fwIpfilDelAll(rslt);
                //fwIpfilAll("forward", "0.0.0.0", "0.0.0.0", "", "", "deny", "no log", rslt);
	}

	OmnString rmCmd;
	rmCmd << "rm -f " << shortName;
	OmnCliSysCmd::doShell(rmCmd, rslt);
	
    return 0;
}
#endif

