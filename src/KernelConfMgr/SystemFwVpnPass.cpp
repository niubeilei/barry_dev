////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemVpnPass.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Global.h"

#include "KernelInterface/CliSysCmd.h"
#include "CliUtil/CliUtil.h"
#include "CliUtil/CliUtilProc.h"

#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "Debug/Debug.h"

#include "Tracer/Tracer.h"
#include "Util/File.h"

#include <string.h>

#define PPTP_PORT		1723	/* TCP */
#define L2TP_PORT		1701	/* UDP */
#define ISAKMP_PORT		500		/* UDP */
#define ISAKMP_NATT_PORT		4500		/* UDP */

int fwL2tpPass(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString status;
	OmnString addcmd;
	OmnString delcmd;
	OmnString rslt;
	OmnString loga;
	OmnString logd;
	status = parms->mStrings[0];
	
	loga << "ACCEPT" ;
	logd << "DROP" ;
	
	addcmd << "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport " << L2TP_PORT 
	       << " -j " ;
	delcmd << "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport " << L2TP_PORT 
	       << " -j " ;
	
	
	if( status == "on" )
	{
		sgFwVpnPass.l2tpStat = "on";
		delcmd << logd;
		addcmd << loga; 	
	}
	else
	{
		sgFwVpnPass.l2tpStat = "off";
		delcmd << loga;
		addcmd << logd;
	}
 
	OmnCliSysCmd::doShell(delcmd, rslt);
	OmnCliSysCmd::doShell(addcmd, rslt);
		
	*optlen = index;
	return 0;
}

int fwPptpPass(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString status;
	OmnString addcmd;
	OmnString delcmd;
	OmnString addcmd1;
	OmnString delcmd1;
	OmnString rslt;
	OmnString loga;
	OmnString logd;
	status = parms->mStrings[0];
	
	loga << "ACCEPT" ;
	logd << "DROP" ;
	
	addcmd << "/sbin/iptables -I FORWARD -o eth0 -p tcp --dport " << PPTP_PORT  
	       << " -j " ;
	delcmd << "/sbin/iptables -D FORWARD -o eth0 -p tcp --dport " << PPTP_PORT 
	       << " -j " ;
	addcmd1 << "/sbin/iptables -I FORWARD -o eth0 -p gre -j " ;
	delcmd1 << "/sbin/iptables -D FORWARD -o eth0 -p gre -j " ;
	
	if( status == "on" )
	{
		sgFwVpnPass.pptpStat = "on";
		delcmd << logd;
		addcmd << loga;
		delcmd1 << logd;
		addcmd1 << loga;			
	}
	else
	{
		sgFwVpnPass.pptpStat = "off";
		delcmd << loga;
		addcmd << logd;
		delcmd1 << loga;
		addcmd1 << logd;
	}

	OmnCliSysCmd::doShell(delcmd, rslt);
	OmnCliSysCmd::doShell(addcmd, rslt);	
	OmnCliSysCmd::doShell(delcmd1, rslt);
	OmnCliSysCmd::doShell(addcmd1, rslt);	
		
	*optlen = index;
	return 0;

//  save2file
//	    ("-I FORWARD -o %s  -p tcp --dport %d -j ACCEPT\n",
//	     wanface, nvram_safe_get ("lan_ipaddr"), PPTP_PORT);
//  save2file ("-I FORWARD -o %s -p gre -j ACCEPT\n", wanface,
//		     nvram_safe_get ("lan_ipaddr"));
}

int fwIpsecPass(char *data, unsigned int *optlen, struct aosUserLandApiParms *parms, 
					char *errmsg, const int errlen)
{
	unsigned int index = 0;
	OmnString status;
	OmnString addcmd;
	OmnString delcmd;
	OmnString addcmd1;
	OmnString delcmd1;
	OmnString addcmd2;
	OmnString delcmd2;
	OmnString addcmd3;
	OmnString delcmd3;
	
	OmnString rslt;
	OmnString loga;
	OmnString logd;
	status = parms->mStrings[0];
	
	loga << "ACCEPT" ;
	logd << "DROP" ;
	
	addcmd << "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport " << ISAKMP_PORT 
	       << " -j " ;
	delcmd << "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport " << ISAKMP_PORT 
	       << " -j " ;
	addcmd1 << "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport " << ISAKMP_NATT_PORT 
	       << " -j " ;
	delcmd1 << "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport " << ISAKMP_NATT_PORT 
	       << " -j " ;
	addcmd2 << "/sbin/iptables -I FORWARD -o eth0 -p esp -j " ;
	delcmd2 << "/sbin/iptables -D FORWARD -o eth0 -p esp -j " ;
	addcmd3 << "/sbin/iptables -I FORWARD -o eth0 -p ah -j " ;
	delcmd3 << "/sbin/iptables -D FORWARD -o eth0 -p ah -j " ;
	
	if( status == "on" )
	{
		sgFwVpnPass.ipsecStat = "on";
		delcmd << logd;
		addcmd << loga; 	
		delcmd1 << logd;
		addcmd1 << loga;			
		delcmd2 << logd;
		addcmd2 << loga;
		delcmd3 << logd;
		addcmd3 << loga;		
	}
	else
	{
		sgFwVpnPass.ipsecStat = "off";
		delcmd << loga;
		addcmd << logd;
		delcmd1 << loga;
		addcmd1 << logd;
		delcmd2 << loga;
		addcmd2 << logd;
		delcmd3 << loga;
		addcmd3 << logd;
	}
 
	OmnCliSysCmd::doShell(delcmd, rslt);
	OmnCliSysCmd::doShell(addcmd, rslt);
	OmnCliSysCmd::doShell(delcmd1, rslt);
	OmnCliSysCmd::doShell(addcmd1, rslt);
	OmnCliSysCmd::doShell(delcmd2, rslt);
	OmnCliSysCmd::doShell(addcmd2, rslt);
	OmnCliSysCmd::doShell(delcmd3, rslt);
	OmnCliSysCmd::doShell(addcmd3, rslt);			
	
	*optlen = index;
	return 0;

//  save2file ("-I FORWARD -o %s -p gre -j ACCEPT\n", wanface,
//		     nvram_safe_get ("lan_ipaddr"));

//save2file ("-A FORWARD -o %s -p udp -m udp --dport %d -j %s\n",
//		   wanface, ISAKMP_PORT, log_drop);
}

int FwVpnPass_regCliCmd(void)
{	
	int ret;
	
	ret = CliUtil_regCliCmd("firewall l2tp_pass",fwL2tpPass);
	ret |= CliUtil_regCliCmd("firewall pptp_pass",fwPptpPass);
	ret |= CliUtil_regCliCmd("firewall ipsec_pass",fwIpsecPass);
	
	return ret;
}
