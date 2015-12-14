////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliFwNat.cpp
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
AosCli::fwNatSnat(OmnString &rslt)
{
	OmnString interface;
	OmnString sip;
	OmnString smask;
	OmnString to;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,to);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
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
	if(OmnIpAddr(to) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The to ip address is invalid!" << endl;
		return -1;
	}
	
	fwNatSnat(interface, sip, smask, to, rslt);

	return 0;
}


int 
AosCli::fwNatSnat(OmnString &interface, OmnString &sip, OmnString &smask, OmnString &to, OmnString &rslt)
{
	OmnString fwNatSnatCmd;
	//OmnString fwNatSnatLogCmd;
	
	fwNatSnatCmd << "/sbin/iptables -t nat -A POSTROUTING ";
	if (interface == "incard")
	{
		fwNatSnatCmd << " -o eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatSnatCmd << " -o eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	fwNatSnatCmd << " -s " << sip << "/" << smask;
	fwNatSnatCmd << " -j SNAT ";
	fwNatSnatCmd << " --to " << to;	
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatSnat";
	data->interface = interface;
	data->sip = sip;	
	data->smask = smask;
	data->to = to;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	//cout << "the fwNatSnat cmd is : " << fwNatSnatCmd << endl;	
	OmnCliSysCmd::doShell(fwNatSnatCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwNatMasq(OmnString &rslt)
{
	OmnString interface;
	OmnString sip;
	OmnString smask;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
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
	
	fwNatMasq(interface, sip, smask, rslt);

	return 0;
}


int 
AosCli::fwNatMasq(OmnString &interface, OmnString &sip, OmnString &smask, OmnString &rslt)
{
	OmnString fwNatMasqCmd;
	//OmnString fwNatMasqLogCmd;
	
	fwNatMasqCmd << "/sbin/iptables -t nat -A POSTROUTING ";
	if (interface == "incard")
	{
		fwNatMasqCmd << " -o eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatMasqCmd << " -o eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	fwNatMasqCmd << " -s " << sip << "/" << smask;
	fwNatMasqCmd << " -j MASQUERADE ";
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatMasq";
	data->interface = interface;
	data->sip = sip;	
	data->smask = smask;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	//cout << "the fwNatMasq cmd is : " << fwNatMasqCmd << endl;	
	OmnCliSysCmd::doShell(fwNatMasqCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}

		
int 
AosCli::fwNatDnatPort(OmnString &rslt)
{
	OmnString interface;
	OmnString proto;
	OmnString sip;
	//OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dport;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	//int flag = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	//curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dport);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	//if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	//{
	//	cout << "Error: The source netmask is invalid!" << endl;
	//	return -1;
	//}
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
		cout << "Error: The destination ip address is invalid!" << endl;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The destination port is invalid!" << endl;
		return -1;
	}
		
	fwNatDnatPort(interface, proto, sip, sport, dip, dport, rslt);

	return 0;
}


int 
AosCli::fwNatDnatPort(OmnString &interface, OmnString &proto, OmnString &sip, OmnString &sport, OmnString &dip, OmnString &dport, OmnString &rslt)
{
	OmnString fwNatDnatPortCmd;
	//OmnString fwNatDnatLogCmd;
	
	fwNatDnatPortCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatDnatPortCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatDnatPortCmd << " -i eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}

	fwNatDnatPortCmd << " -p " << proto;
	//fwNatDnatPortCmd << " -d " << sip << "/" << smask << " --dport " << sport;
	fwNatDnatPortCmd << " -d " << sip << "/255.255.255.255" << " --dport " << sport;
	fwNatDnatPortCmd << " -j DNAT ";
	fwNatDnatPortCmd << " --to " << dip << ":" << dport;
			
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatDnatPort";
	data->interface = interface;
	data->proto = proto;
	data->sip = sip;	
	//data->smask = smask;
	data->sport = sport;
	data->dip = dip;
	data->dport = dport;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	//cout << "the fwNatDnatPort cmd is : " << fwNatDnatPortCmd << endl;	
	OmnCliSysCmd::doShell(fwNatDnatPortCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}
	

int 
AosCli::fwNatDnatIP(OmnString &rslt)
{
	OmnString interface;
	OmnString proto;
	OmnString sip;
	//OmnString smask;
	OmnString dip;
	int curPos = 0;
	//int len = 0;
	//int value = 0;
	//int flag = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	//curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp") && \
	   (proto != "icmp") && (proto !="all"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	//if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	//{
	//	cout << "Error: The source netmask is invalid!" << endl;
	//	return -1;
	//}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination ip address is invalid!" << endl;
		return -1;
	}

	fwNatDnatIP(interface, proto, sip, dip, rslt);

	return 0;
}


int 
AosCli::fwNatDnatIP(OmnString &interface, OmnString &proto, OmnString &sip, OmnString &dip, OmnString &rslt)
{
	OmnString fwNatDnatIPCmd;
	//OmnString fwNatDnatLogCmd;
	
	fwNatDnatIPCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatDnatIPCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatDnatIPCmd << " -i eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}

	fwNatDnatIPCmd << " -p " << proto;
	fwNatDnatIPCmd << " -d " << sip;
	fwNatDnatIPCmd << " -j DNAT ";
	fwNatDnatIPCmd << " --to " << dip;
			
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatDnatIP";
	data->interface = interface;
	data->proto = proto;
	data->sip = sip;	
	//data->smask = smask;
	data->dip = dip;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	//cout << "the fwNatDnatIP cmd is : " << fwNatDnatIPCmd << endl;	
	OmnCliSysCmd::doShell(fwNatDnatIPCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}


int 
AosCli::fwNatRedi(OmnString &rslt)
{
	OmnString interface;
	OmnString proto;
	OmnString fromport;
	OmnString toport;
	int curPos = 0;
	u32 len = 0;
	int value = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,fromport);
	curPos = mFileName.getWord(curPos,toport);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
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
	
	fwNatRedi(interface, proto, fromport, toport, rslt);

	return 0;
}


int 
AosCli::fwNatRedi(OmnString &interface, OmnString &proto, OmnString &fromport, OmnString &toport, OmnString &rslt)
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
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	
	fwNatCmd << " -p " << proto << " --dport " << fromport;;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toport;
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -1;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatRedi";
	data->interface = interface;
	data->proto = proto;	
	data->fromport = fromport;
	data->toport = toport;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	//cout << "the fwNatMasq cmd is : " << fwNatMasqCmd << endl;	
	OmnCliSysCmd::doShell(fwNatCmd, rslt);
	//OmnCliSysCmd::doShell("/sbin/iptables -nat -F", rslt);
	return 0;
}

int 
AosCli::fwNatDelSnat(OmnString &rslt)
{
	OmnString fwNatSnatCmd;
	OmnString interface;
	OmnString sip;
	OmnString smask;
	OmnString to;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,to);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
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
	if(OmnIpAddr(to) == OmnIpAddr::eInvalidIpAddr)
	{
		cout << "Error: The to ip address is invalid!" << endl;
		return -1;
	}

	fwNatSnatCmd << "/sbin/iptables -t nat -D POSTROUTING ";
	if (interface == "incard")
	{
		fwNatSnatCmd << " -o eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatSnatCmd << " -o eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	fwNatSnatCmd << " -s " << sip << "/" << smask;
	fwNatSnatCmd << " -j SNAT ";
	fwNatSnatCmd << " --to " << to;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "fwNatSnat") && (ptr->interface == interface) && \
			(ptr->sip == sip) && (ptr->smask == smask) && \
        	(ptr->to == to)) 
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
        OmnCliSysCmd::doShell(fwNatSnatCmd, rslt);
    }	

	return 0;
}


int 
AosCli::fwNatDelMasq(OmnString &rslt)
{
	OmnString fwNatMasqCmd;
	OmnString interface;
	OmnString sip;
	OmnString smask;
	int curPos = 0;
	
	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,sip);
	curPos = mFileName.getWord(curPos,smask);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
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
	
	fwNatMasqCmd << "/sbin/iptables -t nat -D POSTROUTING ";
	if (interface == "incard")
	{
		fwNatMasqCmd << " -o eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatMasqCmd << " -o eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	fwNatMasqCmd << " -s " << sip << "/" << smask;
	fwNatMasqCmd << " -j MASQUERADE ";
	
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "fwNatMasq") && (ptr->interface == interface) && \
			(ptr->sip == sip) && (ptr->smask == smask))
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
        OmnCliSysCmd::doShell(fwNatMasqCmd, rslt);
    }	

	return 0;
}

	
int 
AosCli::fwNatDelDnatPort(OmnString &rslt)
{
	OmnString fwNatDnatPortCmd;
	OmnString interface;
	OmnString proto;
	OmnString sip;
	//OmnString smask;
	OmnString sport;
	OmnString dip;
	OmnString dport;
	int curPos = 0;
	u32 len = 0;
	int value = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	//curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,sport);
	curPos = mFileName.getWord(curPos,dip);
	curPos = mFileName.getWord(curPos,dport);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	//if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	//{
	//	cout << "Error: The source netmask is invalid!" << endl;
	//	return -1;
	//}
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
		cout << "Error: The destination ip address is invalid!" << endl;
		return -1;
	}
	if((dport.retrieveInt(0, len, value)) && (portlen(value) == dport.length()) && \
		(value >= 0) && (value < 65535))
	{
		//ok;
	}
	else
	{
		cout << "Error: The destination port is invalid!" << endl;
		return -1;
	}
	
	fwNatDnatPortCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatDnatPortCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatDnatPortCmd << " -i eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}

	fwNatDnatPortCmd << " -p " << proto;
	fwNatDnatPortCmd << " -d " << sip << "/255.255.255.255" << " --dport " << sport;
	fwNatDnatPortCmd << " -j DNAT ";
	fwNatDnatPortCmd << " --to " << dip << ":" << dport;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "fwNatDnatPort") && (ptr->interface == interface) && \
			(ptr->proto == proto) && \
			(ptr->sip == sip) && (ptr->sport == sport) && \
			(ptr->dip == dip) && (ptr->dport == dport))
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
        OmnCliSysCmd::doShell(fwNatDnatPortCmd, rslt);
    }	

	return 0;
}
	

int 
AosCli::fwNatDelDnatIP(OmnString &rslt)
{
	OmnString fwNatDnatIPCmd;
	OmnString interface;
	OmnString proto;
	OmnString sip;
	//OmnString smask;
	OmnString dip;
	int curPos = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,sip);
	//curPos = mFileName.getWord(curPos,smask);
	curPos = mFileName.getWord(curPos,dip);

	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp") && \
	   (proto != "icmp") && (proto !="all"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The source IP is invalid!" << endl;
		return -1;
	}
	//if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	//{
	//	cout << "Error: The source netmask is invalid!" << endl;
	//	return -1;
	//}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		cout << "Error: The destination ip address is invalid!" << endl;
		return -1;
	}
	
	fwNatDnatIPCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatDnatIPCmd << " -i eth1 ";
	}
	else if (interface == "outcard")
	{
		fwNatDnatIPCmd << " -i eth0 ";
	}
	else
	{
		cout << "Invalid interface name!" << endl;
		return -1;
	}

	fwNatDnatIPCmd << " -p " << proto;
	fwNatDnatIPCmd << " -d " << sip;
	fwNatDnatIPCmd << " -j DNAT ";
	fwNatDnatIPCmd << " --to " << dip;
					
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "fwNatDnatIP") && (ptr->interface == interface) && \
			(ptr->proto == proto) && (ptr->sip == sip) && (ptr->dip == dip))
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
        OmnCliSysCmd::doShell(fwNatDnatIPCmd, rslt);
    }	

	return 0;
}


int 
AosCli::fwNatDelRedi(OmnString &rslt)
{
	OmnString fwNatCmd;
	OmnString interface;
	OmnString proto;
	OmnString fromport;
	OmnString toport;
	int curPos = 0;
	u32 len = 0;
	int value = 0;

	curPos = mFileName.getWord(curPos,interface);
	curPos = mFileName.getWord(curPos,proto);
	curPos = mFileName.getWord(curPos,fromport);
	curPos = mFileName.getWord(curPos,toport);
	
	//check the valid
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
		return -1;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		cout << "Error: The proto is invalid!" << endl;
		return -1;
	}
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
		cout << "Invalid interface name!" << endl;
		return -1;
	}
	fwNatCmd << " -p " << proto << " --dport " << fromport;;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toport;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "fwNatRedi") && (ptr->interface == interface) && \
			(ptr->proto == proto) && (ptr->fromport == fromport) && (ptr->toport == toport)) 
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
        OmnCliSysCmd::doShell(fwNatCmd, rslt);
    }	

	return 0;
}
#endif

