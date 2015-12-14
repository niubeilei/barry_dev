////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rhcVpnApi.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "rhcVpnApi.h"

#include <stdio.h>
#include <ctype.h>

#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

#define PPTP_PORT		"1723"	/* TCP */
#define L2TP_PORT		"1701"	/* UDP */
#define ISAKMP_PORT		"500"		/* UDP */
#define ISAKMP_NATT_PORT		"4500"		/* UDP */

#define MAX_IPT_LINE	1024
#define MAX_RSLT_LEN	2048	

#define TCP_PROTO	"tcp"
#define MAX_LEN	256

int fwIpsecPass(char *command, char *rslt)
{
	char addcmd[MAX_IPT_LINE];
	char addcmd2[MAX_IPT_LINE];
	char addcmd3[MAX_IPT_LINE];
	char addcmd4[MAX_IPT_LINE];
	char delcmd[MAX_IPT_LINE];
	char delcmd2[MAX_IPT_LINE];
	char delcmd3[MAX_IPT_LINE];
	char delcmd4[MAX_IPT_LINE];
	int ret;
	
	char *onAction = "ACCEPT";
	char *offAction = "DROP" ;
	
	strcpy(addcmd, "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(addcmd, ISAKMP_PORT);
    strcat(addcmd, " -j ");
	strcpy(addcmd2, "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(addcmd2, ISAKMP_NATT_PORT);
    strcat(addcmd2, " -j ");
	strcpy(delcmd, "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(delcmd, ISAKMP_PORT);
    strcat(delcmd, " -j ");
	strcpy(delcmd2, "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(delcmd2, ISAKMP_NATT_PORT);
    strcat(delcmd2, " -j ");
    
    strcpy(addcmd3, "/sbin/iptables -I FORWARD -o eth0 -p esp -j ");
    strcpy(delcmd3, "/sbin/iptables -D FORWARD -o eth0 -p esp -j ");
    strcpy(addcmd4, "/sbin/iptables -I FORWARD -o eth0 -p ah -j ");
    strcpy(delcmd4, "/sbin/iptables -D FORWARD -o eth0 -p ah -j ");
	
	if(!strcasecmp(command,"on"))
	{
		strcat(delcmd, offAction);
		strcat(delcmd2, offAction);
		strcat(delcmd3, offAction);
		strcat(delcmd4, offAction);
		strcat(addcmd, onAction);
		strcat(addcmd2, onAction);
		strcat(addcmd3, onAction);
		strcat(addcmd4, onAction);
	}
	else
	{
		strcat(delcmd, onAction);
		strcat(delcmd2, onAction);
		strcat(delcmd3, onAction);
		strcat(delcmd4, onAction);
		strcat(addcmd, offAction);
		strcat(addcmd2, offAction);
		strcat(addcmd3, offAction);
		strcat(addcmd4, offAction);
	}
 
	//printf("The ipt rule is : %s | %s\n", delcmd, addcmd);
	ret = system(delcmd);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
	ret = system(delcmd2);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
    ret = system(delcmd3);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
    ret = system(delcmd4);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
	ret = system(addcmd);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the ipsec_pass rule!\n");
		return -1;
    }
	ret = system(addcmd2);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the ipsec_pass rule!\n");
		return -1;
    }
    ret = system(addcmd3);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the ipsec_pass rule!\n");
		return -1;
    }
    ret = system(addcmd4);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the ipsec_pass rule!\n");
		return -1;
    }
	
	return 0;
}

int fwL2tpPass(char *command, char *rslt)
{
	char addcmd[MAX_IPT_LINE];
	char delcmd[MAX_IPT_LINE];
	int ret;
	
	char *onAction = "ACCEPT";
	char *offAction = "DROP" ;
	
	strcpy(addcmd, "/sbin/iptables -I FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(addcmd, L2TP_PORT);
    strcat(addcmd, " -j ");
	strcpy(delcmd, "/sbin/iptables -D FORWARD -o eth0 -p udp -m udp --dport ");
    strcat(delcmd, L2TP_PORT);
    strcat(delcmd, " -j ");
	
	if(!strcasecmp(command,"on"))
	{
		strcat(delcmd, offAction);
		strcat(addcmd, onAction);
	}
	else
	{
		strcat(delcmd, onAction);
		strcat(addcmd, offAction);
	}
 
	//printf("The ipt rule is : %s | %s\n", delcmd, addcmd);
	ret = system(delcmd);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
	ret = system(addcmd);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the l2tp_pass rule!\n");
		return -1;
    }
		
	return 0;
}

int fwPptpPass(char *command, char *rslt)
{
	char addcmd[MAX_IPT_LINE];
	char delcmd[MAX_IPT_LINE];
	char addcmd2[MAX_IPT_LINE];
	char delcmd2[MAX_IPT_LINE];
	int ret;
	
	char *onAction = "ACCEPT";
	char *offAction = "DROP" ;
	
	strcpy(addcmd, "/sbin/iptables -I FORWARD -o eth0 -p tcp --dport ");
    strcat(addcmd, PPTP_PORT);
    strcat(addcmd, " -j ");
	strcpy(delcmd, "/sbin/iptables -D FORWARD -o eth0 -p tcp --dport ");
    strcat(delcmd, PPTP_PORT);
    strcat(delcmd, " -j ");
	strcpy(addcmd2, "/sbin/iptables -I FORWARD -o eth0 -p gre -j ");
	strcpy(delcmd2, "/sbin/iptables -D FORWARD -o eth0 -p gre -j ");
	
	if(!strcasecmp(command,"on"))
	{
		strcat(delcmd, offAction);
		strcat(addcmd, onAction);
		strcat(delcmd2, offAction);
		strcat(addcmd2, onAction);
	}
	else
	{
		strcat(delcmd, onAction);
		strcat(addcmd, offAction);
		strcat(delcmd2, onAction);
		strcat(addcmd2, offAction);
	}
 
	//printf("The ipt rule is : %s | %s\n", delcmd, addcmd);
	ret = system(delcmd);
	if (ret < 0) 
	{
		//strcpy("Failed to del the old l2tp_pass rule!\n");
		//return -1;
    }
	ret = system(addcmd);
	if (ret < 0) 
	{
		strcpy(rslt, "Failed to add the pptp_pass rule!\n");
		return -1;
    }
		
	return 0;
}

