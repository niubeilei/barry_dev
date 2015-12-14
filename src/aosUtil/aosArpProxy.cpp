////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosArpProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include "aosUtil/aosArpProxy.h"
#include "aos/aosKernelApi.h"
#include "Porting/InetAddr.h"

#include <linux/seq_file.h>
#include <linux/fs.h>

#include <linux/stat.h>
#include <linux/init.h>
#include <linux/net.h>

#include <net/arp.h>

#include <linux/if_arp.h>
#include <net/neighbour.h>

/*
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/config.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/errno.h>
#include <linux/in.h>
#include <linux/mm.h>
#include <linux/inet.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/fddidevice.h>
#include <linux/if_arp.h>
#include <linux/trdevice.h>
#include <linux/skbuff.h>
#include <linux/proc_fs.h>
#include <net/ip.h>
#include <net/icmp.h>
#include <net/route.h>
#include <net/protocol.h>
#include <net/tcp.h>
#include <net/sock.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/netfilter_arp.h>
*/

//extern struct seq_operations arp_seq_ops;

extern  int arp_req_set(struct arpreq *r, struct net_device * dev);
extern int arp_req_delete(struct arpreq *r, struct net_device * dev);
extern struct net_device *dev_get_by_name(const char *name);
extern long OmnInetAddr(const char *ipAddrDotNotation);

char *safe_strncpy(char *dst, const char *src, size_t size)
{   
    	dst[size-1] = '\0';	
   	return strncpy(dst,src,size-1);   
}


int AosArp_add(u32 ip, const char *dev_name, int flag, u32 netmask)
{

	
	struct arpreq myarpreq;	
	struct net_device *dev;
	struct sockaddr_in mysin;
	
	
	bzero((char *)&mysin,sizeof(mysin));
	mysin.sin_family=AF_INET;
	mysin.sin_addr.s_addr=ip;
	memcpy(&myarpreq.arp_pa,&mysin,sizeof(mysin));

	myarpreq.arp_flags=flag;	

	mysin.sin_family=AF_INET;
	mysin.sin_addr.s_addr=netmask;
	memcpy(&myarpreq.arp_netmask,&mysin,sizeof(mysin));

	dev = dev_get_by_name(dev_name);
	if (!dev)
	{
		//printk("<0>Failed to retrieve the device: %s\n", dev_name);
		return -1;
	}	
	myarpreq.arp_ha.sa_family=dev->type;
	//safe_strncpy(myarpreq.arp_ha.sa_data, dev->dev_addr, dev->addr_len);
	memcpy(myarpreq.arp_ha.sa_data,dev->dev_addr, dev->addr_len);
	safe_strncpy(myarpreq.arp_dev, dev_name, strlen(dev_name));
	
	arp_req_set(&myarpreq, dev);

	//printk("<0>finished the ARP set\n");

	return 0;

}

int AosArp_del(u32 ip, const char *dev_name, int flag, u32 netmask)
{

	
	struct arpreq myarpreq;	
	struct net_device *dev=NULL;
	struct sockaddr_in mysin;
		
	bzero((char *)&mysin,sizeof(mysin));
	mysin.sin_family=AF_INET;
	mysin.sin_addr.s_addr=ip;
	memcpy(&myarpreq.arp_pa,&mysin,sizeof(mysin));

	myarpreq.arp_flags=flag;	

	mysin.sin_family=AF_INET;
	mysin.sin_addr.s_addr=netmask;
	memcpy(&myarpreq.arp_netmask,&mysin,sizeof(mysin));

	dev = dev_get_by_name(dev_name);
	if (!dev)
	{
		//printk("<0>Failed to retrieve the device: %s\n", dev_name);
		return -1;
	}	
	myarpreq.arp_ha.sa_family=dev->type;
	//safe_strncpy(myarpreq.arp_ha.sa_data, dev->dev_addr, dev->addr_len);
	memcpy(myarpreq.arp_ha.sa_data,dev->dev_addr, dev->addr_len);
	safe_strncpy(myarpreq.arp_dev, dev_name, strlen(dev_name));

	arp_req_delete(&myarpreq, dev);

	return 0;

}


int AosSystemArp_AddCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system arp add <ip_addr> <dev_name> [<netmask>]
	//

	u32 ip_addr_n,netmask_n;

	char *ip_addr_a = parms->mStrings[0];
	char *dev_name = parms->mStrings[1];
	//char *flag = parms->mStrings[2];
	char *netmask_a;
	int flag = ATF_PERM;

	if (parms->mNumStrings == 3)
	{
		flag = ATF_NETMASK;
		netmask_a = parms->mStrings[2];
		netmask_n = OmnInetAddr(netmask_a);
	}
	else
	{
		netmask_n = 0xFFFFFFFF;
	}

	ip_addr_n = OmnInetAddr(ip_addr_a);
	//inline long OmnInetAddr(const char *ipAddrDotNotation);
	//mAddr = OmnInetAddr("127.0.0.1");
	
	AosArp_add(ip_addr_n, dev_name, flag, netmask_n);
		
	*length = 0;

	return 0;
}
int AosSystemArp_DelCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system arp del <ip_addr> <dev_name> [<netmask>]
	//

	u32 ip_addr_n,netmask_n;

	char *ip_addr_a = parms->mStrings[0];
	char *dev_name = parms->mStrings[1];
	//char *flag = parms->mStrings[2];
	char *netmask_a;
	int flag = ATF_PERM;

	if (parms->mNumStrings == 3)
	{
		netmask_a = parms->mStrings[2];
		netmask_n = OmnInetAddr(netmask_a);
	}
	else
	{
		netmask_n = 0xFFFFFFFF;
	}

	ip_addr_n = OmnInetAddr(ip_addr_a);
	//inline long OmnInetAddr(const char *ipAddrDotNotation);
	//mAddr = OmnInetAddr("127.0.0.1");
	
	AosArp_del(ip_addr_n, dev_name, flag, netmask_n);
	
	*length = 0;

	return 0;
}

int AosSystemArp_ShowCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	// 
	// system arp show
	//

/*
	printk("<0>have enter the show cli!\n");
	struct seq_file *seq;

	struct arp_iter_state {
		int is_pneigh, bucket;
	};

	int rc = -ENOMEM;
	struct arp_iter_state *s = kmalloc(sizeof(*s), GFP_KERNEL);
       
	if (!s)
		goto out;
	struct file *file;
	rc = seq_open(file, &arp_seq_ops);

	if (rc)
		goto out_kfree;

	seq	     = file->private_data;
	seq->private = s;
	out:
		return rc;
	out_kfree:
		kfree(s);
	goto out;
*/
	
/*
	char local[2000];
	char *rsltBuff = aosKernelApi_getBuff(data);	// Retrieve the beginning of the return buffer
	unsigned int rsltIndex = 0;						// Return buffer index;
	unsigned int optlen = *length;
	int tmpindex=0;

	sprintf(local, "System Statistics Log List\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "-------------------------------------------------\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "--LOG-------------CPU(%s)-------------MEM(k)-----\n","%");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "-index-------idle---user---system-----free mem---\n");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	while(tmpindex<logstateinfotableindex)
	{
		sprintf(local, "log[%4d]: %6d %6d %6d %16lu\n",tmpindex, \
				log_system_stat_info_table[tmpindex].cpu_idle_rate,\
				log_system_stat_info_table[tmpindex].cpu_user_rate,\
				log_system_stat_info_table[tmpindex].cpu_system_rate,\
				log_system_stat_info_table[tmpindex].mem_idle/1010);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		tmpindex++;
	}
	
	*length = rsltIndex;	
*/
	return 0;
}


int AosSystemArp_init()
{
	OmnKernelApi_addCliCmd("sysarp_add", AosSystemArp_AddCli);	
	OmnKernelApi_addCliCmd("sysarp_del", AosSystemArp_DelCli);
	OmnKernelApi_addCliCmd("sysarp_show", AosSystemArp_ShowCli);
	return 0;
}

#endif
