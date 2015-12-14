////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemFwNat.cpp
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

#define MAX_INTERFACE 16
#define MAX_LIMIT_RATE 600000
#define MAX_PORTS 16

//extern struct aos_list_head sgFwFilterList;
extern struct aos_list_head sgFwNatList;

int fwNatSnat(OmnString &interface, u32 sip, u32 smask, u32 to, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char toStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = to;
	strcpy(toStr, inet_ntoa(tmpaddr));
	
	fwNatCmd << "/sbin/iptables -t nat -A POSTROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -o br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -o eth0 ";
	}
	else
	{
		//cout << "Invalid interface name!" << endl;
		return -eAosRc_InvalidInterface;
	}
	fwNatCmd << " -s " << sipStr << "/" << smaskStr;
	fwNatCmd << " -j SNAT ";
	fwNatCmd << " --to " << toStr;	

	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatSnat";
	data->interface = interface;
	data->sip = sipStr;	
	data->smask = smaskStr;
	data->to = toStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	OmnCliSysCmd::doShell(fwNatCmd, rslt);
	
	return 0;
}

int fwNatSnat(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString interface;
	u32 sip;
	u32 smask;
	u32 to;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	to = parms->mIntegers[2];

	ret = fwNatSnat(interface, sip, smask, to, rslt);

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

int fwNatDelSnat(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;

	u32 sip, smask, to;
	OmnString interface;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	char toStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	interface = parms->mStrings[0];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];
	to = parms->mIntegers[2];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = to;
	strcpy(toStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		error = "Error: The interface is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
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
	if(OmnIpAddr(to) == OmnIpAddr::eInvalidIpAddr)
	{
		error = "Error: The to ip address is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	*/

	fwNatCmd << "/sbin/iptables -t nat -D POSTROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -o br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -o eth0 ";
	}
	else
	{
		error = "Invalid interface name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	fwNatCmd << " -s " << sipStr << "/" << smaskStr;
	fwNatCmd << " -j SNAT ";
	fwNatCmd << " --to " << toStr;

	struct fwNatListEntry * ptr;
	struct fwNatListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
	{       
		if(	(ptr->type == "fwNatSnat") && (ptr->interface == interface) && \
				(ptr->sip == sipStr) && (ptr->smask == smaskStr) && \
				(ptr->to == toStr)) 
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
		OmnCliSysCmd::doShell(fwNatCmd, rslt);
	}	

	*optlen = index;
	return 0;
}

int fwNatMasq(OmnString &interface, u32 sip, u32 smask, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));


	fwNatCmd << "/sbin/iptables -t nat -A POSTROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -o br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -o eth0 ";
	}
	else
	{
		//cout << "Invalid interface name!" << endl;
		return -eAosRc_InvalidInterface;
	}
	fwNatCmd << " -s " << sipStr << "/" << smaskStr;
	fwNatCmd << " -j MASQUERADE ";

	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatMasq";
	data->interface = interface;
	data->sip = sipStr;	
	data->smask = smaskStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	//cout << "the fwNatMasq cmd is : " << fwNatMasqCmd << endl;	
	OmnCliSysCmd::doShell(fwNatCmd, rslt);

	return 0;
}

int fwNatMasq(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString interface;
	u32 sip;
	u32 smask;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];

	ret = fwNatMasq(interface, sip, smask, rslt);

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

int fwNatDelMasq(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;

	u32 sip, smask;
	OmnString interface;
	char sipStr[MAX_IP_ADDR_LEN];
	char smaskStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	interface = parms->mStrings[0];
	sip = parms->mIntegers[0];
	smask = parms->mIntegers[1];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = smask;
	strcpy(smaskStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		error = "Error: The interface is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
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
	*/

	fwNatCmd << "/sbin/iptables -t nat -D POSTROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -o br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -o eth0 ";
	}
	else
	{
		error = "Invalid interface name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	fwNatCmd << " -s " << sipStr << "/" << smaskStr;
	fwNatCmd << " -j MASQUERADE ";

	struct fwNatListEntry * ptr;
	struct fwNatListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
	{       
		if(	(ptr->type == "fwNatMasq") && (ptr->interface == interface) && \
				(ptr->sip == sipStr) && (ptr->smask == smaskStr))
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
		OmnCliSysCmd::doShell(fwNatCmd, rslt);
	}	

	*optlen = index;
	return 0;
}

int fwNatDnatPort(OmnString &interface, OmnString &proto, u32 sip, u32 sport, u32 dip, u32 dport, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char sportStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dportStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	sprintf(sportStr, "%lu", sport);
	sprintf(dportStr, "%lu", dport);

	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		//cout << "Invalid interface name!" << endl;
		return -eAosRc_InvalidInterface;
	}

	fwNatCmd << " -p " << proto;
	fwNatCmd << " -d " << sipStr << "/255.255.255.255" << " --dport " << sportStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << dipStr << ":" << dportStr;

	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatDnatPort";
	data->interface = interface;
	data->proto = proto;
	data->sip = sipStr;	
	data->sport = sportStr;
	data->dip = dipStr;
	data->dport = dportStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	OmnCliSysCmd::doShell(fwNatCmd, rslt);

	return 0;
}

int fwNatDnatPort(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString interface;
	OmnString proto;
	u32 sip;
	u32 sport;
	u32 dip;
	u32 dport;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	sip = parms->mIntegers[0];
	sport = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dport = parms->mIntegers[3];

	ret = fwNatDnatPort(interface, proto, sip, sport, dip, dport, rslt);

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

int fwNatDelDnatPort(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;

	u32 sip, sport, dip, dport;
	OmnString interface;
	OmnString proto;
	char sipStr[MAX_IP_ADDR_LEN];
	char sportStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	char dportStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	sip = parms->mIntegers[0];
	sport = parms->mIntegers[1];
	dip = parms->mIntegers[2];
	dport = parms->mIntegers[3];

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	sprintf(sportStr, "%lu", sport);
	sprintf(dportStr, "%lu", dport);

	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		error = "Error: The interface is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		error = "Error: The proto is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
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
		error = "Error: The destination ip address is invalid!";
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
		error = "Error: The destination port is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
    */
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		error = "Invalid interface name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}

	fwNatCmd << " -p " << proto;
	fwNatCmd << " -d " << sipStr << "/255.255.255.255" << " --dport " << sportStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << dipStr << ":" << dportStr;

	struct fwNatListEntry * ptr;
	struct fwNatListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
	{       
		if(	(ptr->type == "fwNatDnatPort") && (ptr->interface == interface) && \
				(ptr->proto == proto) && \
				(ptr->sip == sipStr) && (ptr->sport == sportStr) && \
				(ptr->dip == dipStr) && (ptr->dport == dportStr))
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
		OmnCliSysCmd::doShell(fwNatCmd, rslt);
	}	

	*optlen = index;
	return 0;
}

int fwNatDnatIP(OmnString &interface, OmnString &proto, u32 sip, u32 dip, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		//cout << "Invalid interface name!" << endl;
		return -eAosRc_InvalidInterface;
	}

	fwNatCmd << " -p " << proto;
	fwNatCmd << " -d " << sipStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << dipStr;

	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatDnatIP";
	data->interface = interface;
	data->proto = proto;
	data->sip = sipStr;	
	data->dip = dipStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	OmnCliSysCmd::doShell(fwNatCmd, rslt);
	
	return 0;
}

int fwNatDnatIP(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString interface;
	OmnString proto;
	u32 sip;
	u32 dip;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	sip = parms->mIntegers[0];
	dip = parms->mIntegers[1];

	ret = fwNatDnatIP(interface, proto, sip, dip, rslt);

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

int fwNatDelDnatIP(OmnString &interface, OmnString &proto, u32 sip, u32 dip, OmnString &rslt)
{
	char sipStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	tmpaddr.s_addr = sip;
	strcpy(sipStr, inet_ntoa(tmpaddr));

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		error = "Error: The interface is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	if((proto != "tcp") && (proto != "udp") && \
			(proto != "icmp") && (proto !="all"))
	{
		error = "Error: The proto is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	if((sip != "0") && (sip != "0.0.0.0") && (OmnIpAddr(sip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The source IP is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	//if((smask != "0") && (smask != "0.0.0.0") && (OmnIpAddr(smask) == OmnIpAddr::eInvalidIpAddr))
	//{
	//	cout << "Error: The source netmask is invalid!" << endl;
	//	return -1;
	//}
	if((dip != "0") && (dip != "0.0.0.0") && (OmnIpAddr(dip) == OmnIpAddr::eInvalidIpAddr))
	{
		error = "Error: The destination ip address is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	*/
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		//error = "Invalid interface name!";
		//strncpy(errmsg, error.data(),errlen-1);
		//errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}

	fwNatCmd << " -p " << proto;
	fwNatCmd << " -d " << sipStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << dipStr;

	struct fwNatListEntry * ptr;
	struct fwNatListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
	{       
		if(	(ptr->type == "fwNatDnatIP") && (ptr->interface == interface) && \
				(ptr->proto == proto) && (ptr->sip == sipStr) && (ptr->dip == dipStr))
		{       
			aos_list_del(&ptr->datalist);
			aos_free(ptr);
			existflag = 1;
			break;
		}            
	}
	if (existflag == 0) 
	{   
		//error = "Error: The rule not exist !";
		//strncpy(errmsg, error.data(),errlen-1);
		//errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;
	}
	else
	{
		OmnCliSysCmd::doShell(fwNatCmd, rslt);
	}	

	return 0;
}


int fwNatDelDnatIP(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;

	u32 sip, dip;
	OmnString interface;
	OmnString proto;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	sip = parms->mIntegers[0];
	dip = parms->mIntegers[1];

	ret = fwNatDelDnatIP(interface, proto, sip, dip, rslt);

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

int fwNatRedi(OmnString &interface, OmnString &proto, u32 fromport, u32 toport, OmnString &rslt)
{
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr;

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);

	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		//cout << "Invalid interface name!" << endl;
		return -eAosRc_InvalidInterface;
	}

	fwNatCmd << " -p " << proto << " --dport " << fromportStr;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toportStr;

	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "fwNatRedi";
	data->interface = interface;
	data->proto = proto;	
	data->fromport = fromportStr;
	data->toport = toportStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);

	OmnCliSysCmd::doShell(fwNatCmd, rslt);

	return 0;
}

int fwNatRedi(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString interface;
	OmnString proto;
	u32 fromport;
	u32 toport;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	fromport = parms->mIntegers[0];
	toport = parms->mIntegers[1];

	ret = fwNatRedi(interface, proto, fromport, toport, rslt);

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

int fwNatDelRedi(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;

	u32 fromport, toport;
	OmnString interface;
	OmnString proto;
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];

	interface = parms->mStrings[0];
	proto = parms->mStrings[1];
	fromport = parms->mIntegers[0];
	toport = parms->mIntegers[1];

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);

	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		error = "Error: The interface is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	if((proto != "tcp") && (proto != "udp"))
	{
		error = "Error: The proto is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
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
				(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
				(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			error = "Error: The from port is invalid!";
			strncpy(errmsg, error.data(),errlen-1);
			errmsg[errlen-1] = 0;
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
		error = "Error: The to port is invalid!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -1;
	}
	*/
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		error = "Invalid interface name!";
		strncpy(errmsg, error.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_InvalidInterface;
	}
	fwNatCmd << " -p " << proto << " --dport " << fromportStr;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toportStr;

	struct fwNatListEntry * ptr;
	struct fwNatListEntry * tmp;
	int existflag = 0;
	aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
	{       
		if(	(ptr->type == "fwNatRedi") && (ptr->interface == interface) && \
				(ptr->proto == proto) && (ptr->fromport == fromportStr) && (ptr->toport == toportStr)) 
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
		OmnCliSysCmd::doShell(fwNatCmd, rslt);
	}	

	*optlen = index;
	return 0;
}

int webwallRedi(OmnString &interface, u32 fromport, u32 toport, u32 dip, OmnString &rslt)
{
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));
	
	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
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
	
	fwNatCmd << " -p tcp --dport " << fromportStr << " -d ! " << dipStr;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toportStr;
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "webwallRedi";
	data->interface = interface;
	data->dip = dipStr;	
	data->fromport = fromportStr;
	data->toport = toportStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	OmnCliSysCmd::doShell(fwNatCmd, rslt);

	return 0;
}


int webwallRedi(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	char local[1024];
	int ret;
	OmnString rslt;
	OmnString error;
	OmnString interface;
	OmnString proto;
	u32 fromport, toport, dip;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	fromport = parms->mIntegers[0];
	toport = parms->mIntegers[1];
	dip = parms->mIntegers[2];

	ret = webwallRedi(interface, fromport, toport, dip, rslt);

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


int webwallDnat(OmnString &interface, u32 fromport, u32 toport, u32 dip, u32 toip, OmnString &rslt)
{
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];
	char if_ipStr[MAX_IP_ADDR_LEN];
	char to_ipStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);

	tmpaddr.s_addr = dip;
	strcpy(if_ipStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = toip;
	strcpy(to_ipStr, inet_ntoa(tmpaddr));
	
	fwNatCmd << "/sbin/iptables -t nat -A PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
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
	
	fwNatCmd << " -p tcp --dport " << fromportStr << " -d ! " << if_ipStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << to_ipStr;
	fwNatCmd << ":" << toportStr;
		
	struct fwNatListEntry * data;
	if((data = (struct fwNatListEntry *)aos_malloc(sizeof(struct fwNatListEntry)))==NULL)
	{
		//printk("malloc error");
		return -eAosRc_MallocError;
	}
	memset(data, 0, sizeof(struct fwNatListEntry));
	data->type = "webwallDnat";
	data->interface = interface;
	data->dip = if_ipStr;	
	data->fromport = fromportStr;
	data->toport = toportStr;
	data->to = to_ipStr;
	aos_list_add_tail((struct aos_list_head *)&data->datalist, &sgFwNatList);
	
	OmnCliSysCmd::doShell(fwNatCmd, rslt);

	return 0;
}


int webwallDnat(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)

{
	unsigned int index = 0;
	OmnString rslt;
	OmnString interface;
	u32 dip,toip, fromport, toport;

    interface= parms->mStrings[0];
    fromport = parms->mIntegers[0];
    toport = parms->mIntegers[1];
    dip = parms->mIntegers[2];
    toip = parms->mIntegers[3];
	if (toip == 0)
		toip = dip;
	
	webwallDnat(interface, fromport, toport, dip, toip, rslt);

	*optlen= index;
	return 0;
}

int webwallDelDnat(OmnString &interface, u32 fromport, u32 toport, u32 dip, u32 toip, OmnString &rslt)
{
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];
	char if_ipStr[MAX_IP_ADDR_LEN];
	char to_ipStr[MAX_IP_ADDR_LEN];
	OmnString fwNatCmd;
	struct in_addr tmpaddr;

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);

	tmpaddr.s_addr = dip;
	strcpy(if_ipStr, inet_ntoa(tmpaddr));
	
	tmpaddr.s_addr = toip;
	strcpy(to_ipStr, inet_ntoa(tmpaddr));
//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
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
	
		if((parse_multi_ports(buffer, ports) == 2) 
			&& (ports[0] <= ports[1]) 
			&& (strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1]) + 1)) 
			&& (ports[0] < 65536) 
			&& (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			rslt = "Error: The from port is invalid!" ;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;

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
		rslt = "Error: The to port is invalid!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return -1;
	}
	*/
	
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		fwNatCmd << " -i " << interface;
	}

	fwNatCmd << " -p tcp --dport " << fromportStr << " -d ! " << if_ipStr;
	fwNatCmd << " -j DNAT ";
	fwNatCmd << " --to " << to_ipStr;
	fwNatCmd << ":" << toportStr;
	//cout <<fwNatCmd <<endl;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "webwallDnat") && (ptr->interface == interface) && \
			(ptr->dip == if_ipStr) && (ptr->fromport == fromportStr) && (ptr->toport == toportStr)) 
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
		//strncpy(errmsg, rslt.data(),errlen-1);
		//errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;

    }
    else
    {
	  	OmnCliSysCmd::doShell(fwNatCmd, rslt);
    }

	return 0;
}
	
int webwallDelDnat(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;
	char local[1024];
	int ret;

	u32 fromport, toport, if_ip, to_ip;
	OmnString interface;

	char* buf = CliUtil_getBuff(data);

	interface = parms->mStrings[0];
	fromport = parms->mIntegers[0];
	toport = parms->mIntegers[1];
	if_ip = parms->mIntegers[2];
	to_ip = parms->mIntegers[3];
	if (to_ip == 0)
		to_ip = if_ip;

	ret = webwallDelDnat(interface, fromport, toport, if_ip, to_ip, rslt);

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
	
int webwallDelRedi(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
		char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString fwNatCmd;
	OmnString rslt;
	OmnString error;

	u32 fromport, toport, dip;
	OmnString interface;
	char fromportStr[MAX_IP_ADDR_LEN];
	char toportStr[MAX_IP_ADDR_LEN];
	char dipStr[MAX_IP_ADDR_LEN];
	struct in_addr tmpaddr;

	interface = parms->mStrings[0];
	fromport = parms->mIntegers[0];
	toport = parms->mIntegers[1];
	dip = parms->mIntegers[2];

	tmpaddr.s_addr = dip;
	strcpy(dipStr, inet_ntoa(tmpaddr));

	sprintf(fromportStr, "%lu", fromport);
	sprintf(toportStr, "%lu", toport);
	
	//check the valid
	/*
	if((interface != "incard") && (interface != "outcard"))
	{
		cout << "Error: The interface is invalid!" << endl;
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
			(strlen(buffer) == (unsigned int)(portlen(ports[0]) + portlen(ports[1])) + 1) && \
			(ports[0] < 65536) && (ports[1] < 65536))
		{
			//ok
			free(buffer);
		}
		else
		{
			free(buffer);
			rslt = "Error: The from port is invalid!" ;
			strncpy(errmsg, rslt.data(),errlen-1);
			errmsg[errlen-1] = 0;

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
		rslt = "Error: The to port is invalid!";
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;

		return -1;
	}
	*/
	fwNatCmd << "/sbin/iptables -t nat -D PREROUTING ";
	if (interface == "incard")
	{
		fwNatCmd << " -i br0 ";
	}
	else if (interface == "outcard")
	{
		fwNatCmd << " -i eth0 ";
	}
	else
	{
		fwNatCmd << " -i " << interface;
	}

	fwNatCmd << " -p tcp --dport " << fromportStr << " -d ! " << dipStr;
	fwNatCmd << " -j REDIRECT ";
	fwNatCmd << " --to-ports " << toportStr;
				
	struct fwNatListEntry * ptr;
    struct fwNatListEntry * tmp;
    int existflag = 0;
    aos_list_for_each_entry_safe(ptr, tmp, &sgFwNatList, datalist)
    {       
    	if(	(ptr->type == "webwallRedi") && (ptr->interface == interface) && \
			(ptr->dip == dipStr) && (ptr->fromport == fromportStr) && (ptr->toport == toportStr)) 
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
		strncpy(errmsg, rslt.data(),errlen-1);
		errmsg[errlen-1] = 0;
		return -eAosRc_DelRuleNotExist;

    }
    else
    {
        OmnCliSysCmd::doShell(fwNatCmd, rslt);
    }

    *optlen = index;
	return 0;
}

int FwNat_regCliCmd(void)
{	
	int ret;

	ret = CliUtil_regCliCmd("firewall snat", fwNatSnat);
	ret |= CliUtil_regCliCmd("firewall del snat", fwNatDelSnat);
	ret |= CliUtil_regCliCmd("firewall masq", fwNatMasq);
	ret |= CliUtil_regCliCmd("firewall del masq", fwNatDelMasq);
	ret |= CliUtil_regCliCmd("firewall dnat port", fwNatDnatPort);
	ret |= CliUtil_regCliCmd("firewall del dnat port", fwNatDelDnatPort);
	ret |= CliUtil_regCliCmd("firewall dnat ip", fwNatDnatIP);
	ret |= CliUtil_regCliCmd("firewall del dnat ip", fwNatDelDnatIP);
	ret |= CliUtil_regCliCmd("firewall redi", fwNatRedi);
	ret |= CliUtil_regCliCmd("firewall del redi", fwNatDelRedi);
	ret |= CliUtil_regCliCmd("webwall redi", webwallRedi);
	ret |= CliUtil_regCliCmd("webwall del redi", webwallDelRedi);
	ret |= CliUtil_regCliCmd("webwall dnat", webwallDnat);
	ret |= CliUtil_regCliCmd("webwall del dnat", webwallDelDnat);

	return ret;
}
