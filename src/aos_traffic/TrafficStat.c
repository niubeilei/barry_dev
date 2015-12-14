////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SecureAuthCmd.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include <linux/module.h>
#include <net/snmp.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#include <linux/proc_fs.h>

static char *traffic_show_ip_stat(char *ifname)//ip traffic
{
	char local[512];
	memset(local, 0, sizeof(local));
	struct net_device  *dev = __dev_get_by_name(ifname);
	if (dev) {
		struct net_device_stats *stats = dev->get_stats(dev);

		sprintf(local, "devices = %s:reciveByte = %lu recivePackets = %lu" 
				"sendBytes = %lu sendPackets = %lu\n",
				dev->name, 
				stats->rx_bytes,
				stats->rx_packets,
				stats->tx_bytes, 
				stats->tx_packets
				);
	}
	return local;
}

/*long tcp_traffic_in_bytes;
long tcp_traffic_out_bytes;
long udp_traffic_in_bytes;
long udp_traffic_out_bytes;
long icmp_traffic_bytes;*/
int aosTraffic_StatisticsShowCli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	char local[1024];
	char *rsltBuff = aosKernelApi_getBuff(data);	// Retrieve the beginning of the return buffer
	unsigned int rsltIndex = 0;						// Return buffer index;
	unsigned int optlen = *length;

	memset(local, 0, sizeof(local));

	sprintf(local, "%s\n\nicmp = %ld tcpInPut = %ld, TcpOutPut = %ld UdpInPut = %ld UdpOutPut = %ld", 
			traffic_show_ip_stat("eth0"),
			icmp_traffic_bytes,
			tcp_traffic_in_bytes,
			tcp_traffic_out_bytes,
			udp_traffic_in_bytes,
			udp_traffic_out_bytes
			);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;

	return 0;
}


// kevin, 01/28/2007
static int aosTraffic_procRead(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	//struct tcp_vs_service *svc;
	//struct tcp_vs_conn * conn;
	//struct list_head *l;
	//struct list_head *lc;
	int end = 0;

	if(offset>0)
		return 0;

	//sprintf(&buf[end], "------------------------- The App Proxy User List ------------------------\n", atomic_read(&aos_tcpvs_curconns));
	//end=strlen(buf);
	// for keyou HAC
	//sprintf(&buf[end], "Name\t\tProxy-Name\tLogin-Time\n");
	//end=strlen(buf);
	//sprintf(&buf[end], "--------------------------------------------------------------------------\n");
	//end=strlen(buf);

  	sprintf(&buf[end], "aos traffic information\n");
	end=strlen(buf);
  	sprintf(&buf[end], "-----------------------\n");
	end=strlen(buf);
	
  	sprintf(&buf[end], "icmp : %lu \n", icmp_traffic_bytes);
	end=strlen(buf);

    sprintf(&buf[end], "tcp  : %lu ", tcp_traffic_in_bytes);
	end=strlen(buf);

    sprintf(&buf[end], "%lu \n", tcp_traffic_out_bytes);
	end=strlen(buf);

    sprintf(&buf[end], "udp  : %lu ", udp_traffic_in_bytes);
	end=strlen(buf);

    sprintf(&buf[end], "%lu\n", udp_traffic_out_bytes);
	end=strlen(buf);

	if (end > (1024 * 4 - 99 - 1))   // the proc file limit is 1024*4
	{
		return end;
	}
	
	return end;
}


static int aosTraffic_registerCli(void)
{
	int ret=0;
	
	ret |= OmnKernelApi_addCliCmd("traffic_statistics_show", aosTraffic_StatisticsShowCli);	

	return ret;
}

static int aosTraffic_unregisterCli(void)
{
	OmnKernelApi_delCliCmd("traffic_statistics_show");
	return 0;
}

static int aosTraffic_registerProc(void)
{
	create_proc_read_entry("aos_traffic",0444,proc_net,aosTraffic_procRead,0);
	return 0;
}


static int aosTraffic_unregisterProc(void)
{

	remove_proc_entry("aos_traffic",proc_net);
	return 0;
}



static __init int 
aosTraffic_initModule(void)
{
	//init
	aosTraffic_registerCli();
	aosTraffic_registerProc();
	return 0;
}

static __exit void 
aosTraffic_exitModule(void)
{
	aosTraffic_unregisterCli();
	aosTraffic_unregisterProc();
}

module_init(aosTraffic_initModule);
module_exit(aosTraffic_exitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kevin");


