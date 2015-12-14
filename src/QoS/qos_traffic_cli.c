////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_cli.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/kernel.h"
#include "KernelSimu/proc_fs.h"
#include "KernelSimu/string.h"
#include "KernelSimu/dev.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/module.h"
#include "aosUtil/List.h"
#include "QoS/qos_traffic.h"
#include "QoS/hashtab.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
#include "QoS/qos_bw_man.h"
#include "QoS/qos_cli_check_parms.h"
#include "QoS/qos_logstat.h"

int is_ip_in_range(u32 ip1,u32 ip2,u32 mask)
{
	return ((ntohl(ip1) & mask) != (ntohl(ip2) & mask));
}

static int is_port_in_range(u16 port,u16 port_rule)
{
	if (port == port_rule) {
		return 0;
	}
	return (port_rule != 0);
}

static int show_chbw_cmp(struct _qos_traffic_channel *d,struct bw_monitor_show_arg *args)
{
	int ret;

	u32 src_mask_num;
	u32 dst_mask_num;
	u8  src_mask_bit;
	u8  dst_mask_bit;

	ret = 0;
	src_mask_bit = args->parms.chdata.src_maskbit;
	dst_mask_bit = args->parms.chdata.dst_maskbit;
	src_mask_num = (src_mask_bit == 0 || src_mask_bit == 32)? 0xFFFFFFFF: (((1 << src_mask_bit) - 1) << (32 - src_mask_bit));
	dst_mask_num = (dst_mask_bit == 0 || dst_mask_bit == 32)? 0xFFFFFFFF: (((1 << dst_mask_bit) - 1) << (32 - dst_mask_bit));

	if (d->protocol != args->parms.chdata.proto) {
		return 1;
	}
	ret = is_ip_in_range(d->src_ip,args->parms.chdata.src_ip,src_mask_num);
	if (ret != 0) {
		return ret;
	}
	ret = is_ip_in_range(d->dst_ip,args->parms.chdata.dst_ip,dst_mask_num);
	if (ret != 0) {
		return ret;
	}
	ret = is_port_in_range(d->src_port,args->parms.chdata.src_port);
	if (ret != 0) {
		return ret;
	}
	ret = is_port_in_range(d->dst_port,args->parms.chdata.dst_port);
	if (ret != 0) {
		return ret;
	}
	
	return 0;
}

static void qos_show_channel(struct _qos_traffic_channel *cell,struct bw_monitor_show_arg *args)
{
	char *msg, msg2[64];
	char ifname[IFNAMSIZ];
	struct net_device *dev;
	int if_index, len;

	/* proection of overflow */
	#define MAX_CHANNEL_BUFFER_LENGTH 10000
	if (args->rsltIndex > MAX_CHANNEL_BUFFER_LENGTH)
		return;
	
	aos_assert0(cell);

	msg = (char*)aos_malloc_atomic(256);
	aos_assert0(msg);

	if_index = cell->ifid;
	dev = dev_get_by_index(if_index);
	if (!dev)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "%s no such net device which net id is %d", __FUNCTION__, if_index);
		aos_free(msg);
		return;
	}
	if (cell->status != QOS_CHANNEL_ACTIVE) return;	
	
	strcpy(ifname, dev->name);
	
	len = sprintf(msg, "bw:%-9u ", cell->record.total_band);
	sprintf(msg2, "src:%u.%u.%u.%u:%u ", NIPQUAD(cell->src_ip), ntohs(cell->src_port));
	len += sprintf(msg+len, "%-28s", msg2);
	sprintf(msg2, "dst:%u.%u.%u.%u:%u ", NIPQUAD(cell->dst_ip), ntohs(cell->dst_port));
	len += sprintf(msg+len, "%-28s", msg2);
	
	len += sprintf(msg+len,"proto:%u ifname:%s priority:%u\n", cell->protocol, ifname, cell->priority);

	if (sgqos_stat)
	{
		sprintf(msg+len,"packet_rcvd:%-9u packet_dropped:%-9u packet_sent:%-9u\n"
				        " bytes_rcvd:%-9u  bytes_dropped:%-9u  bytes_sent:%-9u\n\n", 
				cell->record.packet_rcvd, cell->record.packet_dropped, cell->record.packet_sent, 
				cell->record.bytes_rcvd, cell->record.bytes_dropped, cell->record.bytes_sent);
	}

	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, msg, strlen(msg));
	
	aos_free(msg);
	
	if (dev)
		dev_put(dev);
}

static void qos_show_priority(struct qos_bw_ctrl_data *cell, struct bw_monitor_show_arg *args,char * ifname,u8 priority)
{
	int i = 0;
	char msg[1024];


	sprintf(msg,"===========priority info for %s\n",ifname);
	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, msg, strlen(msg));

	if (priority == eROS_QOS_PRIORITY_MAX) {
		for (i = 0; i < eROS_QOS_PRIORITY_MAX; ++i) {
			qos_traffic_update(&(cell->record[i]), 0);
			sprintf(msg,"bw:%-9u priority:%d\n", cell->record[i].total_band, i);
			aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, msg, strlen(msg));
		}
	} else {
			qos_traffic_update(&(cell->record[i]), 0);
			sprintf(msg,"bw:%-9u priority:%d\n", cell->record[i].total_band, priority);
			aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, msg, strlen(msg));
	}
	return;
}


static void qos_show_dev(struct _qos_traffic_channel_record *record, struct bw_monitor_show_arg *args,u8 direction,char *ifname)
{
	char msg[1024];
	char out_direction[8];

	if (direction == 0) 
		strcpy(out_direction,"in");
	else
		strcpy(out_direction,"out");

	qos_traffic_update(record, 0);
	sprintf(msg,"bw:%-9u direction:%s dev:%s\n", record->total_band, out_direction,ifname);
	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, msg, strlen(msg));

	return;
}


static int qos_show_group_chbw_on_hashtab(void *k,void *d,void *args)
{
	int flag;
	struct _qos_traffic_channel *cell = (struct _qos_traffic_channel *)d;

	flag = show_chbw_cmp(cell,(struct bw_monitor_show_arg *)args);

	if (flag == 0) {
		qos_show_channel(cell,(struct bw_monitor_show_arg *)args);
	}

	return 0;
}


static void qos_show_group_pribw_on_hashtab(struct bw_monitor_show_arg *args)
{
	u8 if_index;
	u8 local_priority;
	struct qos_bw_ctrl_data *cell;
	struct prihashkey k;
	char ifname[IFNAMSIZ];
	struct net_device *dev;

	local_priority = args->parms.pridata.priority;
	if_index = args->parms.pridata.if_index;
	dev = dev_get_by_index(if_index);

	strcpy(ifname,dev->name);

	cell = match_priority_hash(if_index,0,local_priority,&k);
	if (cell != NULL) {
		qos_show_priority(cell,args,ifname,local_priority);
	}
	
	if (dev)
		dev_put(dev);
}


static void qos_show_group_ethbw_on_hashtab(struct bw_monitor_show_arg *args)
{
	u8 local_direction;
	int if_index;
	struct _qos_traffic_channel_record *record  = NULL;
	struct ethhashkey k;
	char ifname[IFNAMSIZ];
	struct net_device *dev;

	local_direction = args->parms.ethdata.direction;
	if_index = args->parms.ethdata.if_index;
	dev = dev_get_by_index(if_index);

	strcpy(ifname,dev->name);

	if ((local_direction & direction_in) == direction_in) {
		record = match_ifhash(if_index,0,&k);
		if (record != NULL) {
			qos_show_dev(record,args,0,ifname);
		}
	}

	if ((local_direction & direction_out) == direction_out) {
		record = match_ifhash(if_index,1,&k);
		if (record != NULL) {
			qos_show_dev(record,args,1,ifname);
		}
	}
	if (dev)
		dev_put(dev);
}


static int qos_show_all_chbw_on_hashtab(void *k,void *d,void *args)
{
	struct _qos_traffic_channel *cell = (struct _qos_traffic_channel *)d;
	qos_show_channel(cell,(struct bw_monitor_show_arg *)args);

	return 0;
}


static int qos_show_all_pribw_on_hashtab(void *k,void *d,void *args)
{

	int if_index;
	char ifname[IFNAMSIZ];
	struct net_device *dev;

	if_index = *((int *)k);
	aos_trace("if_index%d",if_index);
	dev = dev_get_by_index(if_index);
	aos_trace("dev pointer %p",dev);

	strcpy(ifname,dev->name);
	aos_trace("dev name%p",ifname);

	qos_show_priority((struct qos_bw_ctrl_data *)d,(struct bw_monitor_show_arg *)args,ifname,eROS_QOS_PRIORITY_MAX);
	if (dev)
		dev_put(dev);
		
	return 0;
}


static int qos_show_all_ethbw_on_hashtab(void *k,void *d,void *args)
{
	char ifname[IFNAMSIZ];
	u8 direction;
	int if_index;
	struct net_device *dev;

	if_index = *((int *)k);
	aos_trace("if_index%d",if_index);
	dev = dev_get_by_index(if_index);
	if (dev == NULL) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"No such interface");
	}
	aos_trace("dev pointer %p",dev);
	aos_trace("dev name %p",ifname);
	strcpy(ifname,dev->name);
	aos_trace("hehe");

	direction = *((u8*)((char *)k + sizeof(int)));
	qos_show_dev((struct _qos_traffic_channel_record*)d,(struct bw_monitor_show_arg *)args,direction,ifname);
	if (dev)
		dev_put(dev);
	
	return 0;
}


static inline void qos_show_bandwidth_details(struct bw_monitor_show_arg *args,int allflag)
{
	if (allflag == 0) {
		switch (args->type) {
			case bw_show_ch:
				hashtab_map(qos_traffic_hash_gen,qos_show_group_chbw_on_hashtab,args);
				break;
			case bw_show_pri:
				qos_show_group_pribw_on_hashtab(args);
				break;
			case bw_show_eth:
				qos_show_group_ethbw_on_hashtab(args);
				break;
			default:
				aos_assert0(0);
				break;
		}
	} else {
		switch (args->type) {
			case bw_show_ch:
				hashtab_map(qos_traffic_hash_gen,qos_show_all_chbw_on_hashtab,args);
				break;
			case bw_show_pri:
				hashtab_map(qos_traffic_hash_pri,qos_show_all_pribw_on_hashtab,args);
				break;
			case bw_show_eth:
				hashtab_map(qos_traffic_hash_eth,qos_show_all_ethbw_on_hashtab,args);
				break;
			default:
				aos_assert0(0);
				break;
		}
	}
	return;
}

/////////////////////////////////////////////////////////////////////////
//

void __qos_show_history_details(struct _qos_traffic_channel_record *record, struct bw_history_show_arg *args)
{
	u16 interval; 
	u16 start_time;
	u16 end_time;
	unsigned int average = 0;
	struct list_head *cur;
	struct _qos_traffic_history_cell *elem;
	unsigned long temp_jiffy;
	unsigned long cal_jiffy;
	int flag = 0;
	char local[64];
	int count = 0, break_flag = 0, diff_time = 0;
	
	cal_jiffy = QOS_GET_JIFFIES();
	temp_jiffy = cal_jiffy; 

	interval = args->interval;
	start_time = args->start_time;
	end_time = args->end_time;

	aos_assert0(interval > 0);

	cur = record->channel_history_queue_head.prev;

	while (cur != &record->channel_history_queue_head) 
	{
		elem = (struct _qos_traffic_history_cell *)list_entry(cur,struct _qos_traffic_history_cell,cell_head);

		if ((start_time != 0) && (cal_jiffy > (elem->start_jiffy+start_time*HZ))) 
		{
			break_flag = 1;
			break;
		}

		if (count != 0 && count%10 == 0) 
		{
			sprintf(local,"\n%-3ds-%-3ds: ", count*interval+diff_time, (count+10)*interval+diff_time); 
			aosCheckAndCopy(args->rsltBuff, &args->rsltIndex, args->optlen, local, strlen(local));
		}

		if (flag == 0) 
		{
			if (temp_jiffy >= (elem->start_jiffy + end_time * HZ)) 
			{
				diff_time = (int)((temp_jiffy - (elem->start_jiffy+end_time*HZ))/HZ);
				sprintf(local,"\n%-3ds-%-3ds: ", count*interval+diff_time, (count+10)*interval+diff_time); 
				aosCheckAndCopy(args->rsltBuff, &args->rsltIndex, args->optlen, local, strlen(local));
				flag = 1;
				temp_jiffy = elem->start_jiffy;
				sprintf(local,"%-8u ", elem->history_traffic);
				aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
				average += elem->history_traffic;
				++count;
			}
		} 
		else 
		{
			if (temp_jiffy >= (elem->start_jiffy + interval * HZ))
			{
				sprintf(local,"%-8u ", elem->history_traffic); 
				aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
				temp_jiffy = elem->start_jiffy;
			}
			average += elem->history_traffic;
			++count;
		}
		
		//if (count%10 == 9) 
		//	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, "\n", strlen("\n"));

		cur = cur->prev; 
	}

	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, "\n", strlen("\n"));
	
	if (start_time !=0 && break_flag == 0) 
	{
		sprintf(local,"In more previous time, the channel has not been set up\n");
		aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
	}
	if (count != 0) 
	{
		average /= count;

		sprintf(local,"The average bandwidth from %u seconds ago is %u for the %u seconds\n", end_time,average,count);
	} 
	else 
	{
		sprintf(local, "There is no relative history record.\n");
	}

	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
	
	return;
}


void output_no_ch_history(struct bw_history_show_arg *args)
{
	char local[64];
	sprintf(local,"The specific channel does not exist\n");
	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
	return;
}


void output_no_pri_history(struct bw_history_show_arg *args)
{
	char local[64];
	sprintf(local,"The specific dev has not any streams monitored\n");
	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
	return;
}


void output_no_eth_history(struct bw_history_show_arg *args)
{
	char local[64];
	sprintf(local,"The specific dev has not any streams monitored in any direction\n");
	aosCheckAndCopy(args->rsltBuff,&args->rsltIndex, args->optlen, local, strlen(local));
	return;
}


struct _qos_traffic_channel *isthere_such_channel(struct bw_history_show_arg *args)
{
	struct chhashkey k;
	struct _qos_traffic_channel *ret;

	u16 dst_port;
	u16 src_port;
	u32 src_ip;
	u32 dst_ip;
	u8  proto;

	src_port = args->parms.chhisdata.src_port;
	dst_port = args->parms.chhisdata.dst_port;
	src_ip = args->parms.chhisdata.src_ip;
	dst_ip = args->parms.chhisdata.dst_ip;
	proto = args->parms.chhisdata.proto;

	aos_trace("src_ip,%u.%u.%u.%u",NIPQUAD(src_ip));
	aos_trace("dst_ip,%u.%u.%u.%u",NIPQUAD(dst_ip));
	aos_trace("src_port:%X,%u",src_port,src_port);
	aos_trace("dst_port:%X,%u",dst_port,dst_port);

	ret = match_channel_hashtable(src_ip,dst_ip,src_port,dst_port,proto,&k);

	return ret;
}


struct qos_bw_ctrl_data *isthere_such_dev_priority_info (struct bw_history_show_arg *args)
{
	struct qos_bw_ctrl_data *ret;
	struct prihashkey k;
	int ifid;

	ifid = args->parms.prihisdata.if_index;
	ret = match_priority_hash(ifid,0,0,&k);

	return ret;
}


struct _qos_traffic_channel_record *isthere_dev_direction_record (struct bw_history_show_arg *args,u8 direction)
{
	struct ethhashkey k;
	struct _qos_traffic_channel_record *ret;
	int ifid;
	
	ifid = args->parms.ethhisdata.if_index;

	ret = match_ifhash(ifid,direction,&k);

	return ret;
}


static inline void qos_show_history_details(struct bw_history_show_arg *args)
{
	struct _qos_traffic_channel *channel;
	struct qos_bw_ctrl_data *priority_data;
	struct _qos_traffic_channel_record *record;
	char local[128];
	int count;
	int i;

	count = 0;

	switch (args->type) {
		case bw_show_ch:
			channel = isthere_such_channel(args);
			if (channel != NULL) {
				__qos_show_history_details(&channel->record,args);
			} else {
				output_no_ch_history(args);
			}
			break;
		case bw_show_pri:
			priority_data = isthere_such_dev_priority_info(args);
			if (priority_data != NULL) {
				if (args->parms.prihisdata.priority == eROS_QOS_PRIORITY_MAX) {
					for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++) {
						sprintf(local, "\n======= history for priority %s ======\n", get_pri_str(i));
						aosCheckAndCopy(args->rsltBuff, &args->rsltIndex, args->optlen, local, strlen(local));
						__qos_show_history_details(&priority_data->record[i], args);
					}
				} else {
					__qos_show_history_details(&priority_data->record[args->parms.prihisdata.priority],args);
				}
			} else {
				output_no_pri_history(args);
			}
			break;
		case bw_show_eth:
			if ((args->parms.ethhisdata.direction & direction_in) == direction_in) {
				record = isthere_dev_direction_record(args,0);
				if (record != NULL) {
					__qos_show_history_details(record,args);
					++count;
				}
			}
			if ((args->parms.ethhisdata.direction & direction_out) == direction_out) {
				record = isthere_dev_direction_record(args,1);
				if (record != NULL) {
					__qos_show_history_details(record,args);
					++count;
				}
			}
			if (count == 0) {
				output_no_eth_history(args);
			}
			break;
		default:
			aos_assert0(0);
	}
	return;
}


static int AosQoSTrafficCli_setTrafficStatus(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int	ret=0;
	int setvalue;
	char *status = parms->mStrings[0];
	*length = 0;

	ret = is_status_valid(status,&setvalue,errmsg);
	if (ret != 0) {
		ret = eAosRc_InvalidParam;
		goto out;
	}
	ret = qos_traffic_set_status(setvalue);
	if (ret != 0) {
		ret = (strcmp(status, "on")==0) ? eAosRc_FailedToEnable: eAosRc_FailedToDisable;
		goto out;
	}

	ros_qos_config_log(parms->mCmd);
	return ret;
out:
	return ret;
}


static int AosQoSTrafficCli_addMonitorDirection(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int setvalue;
	char *direction = parms->mStrings[0];

	*length = 0;

	ret = is_direction_valid(direction,&setvalue,errmsg);

	if (ret != 0) {
		goto out;
	}

	monitor_test_direction |= setvalue;	

	ros_qos_config_log(parms->mCmd);
out:
	return ret;
}


static int AosQoSTrafficCli_delMonitorDirection(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int setvalue;
	char *direction = parms->mStrings[0];

	*length = 0;

	ret = is_direction_valid(direction,&setvalue,errmsg);

	if (ret != 0) {
		goto out;
	}

	monitor_test_direction &= ~setvalue;	

	ros_qos_config_log(parms->mCmd);
out:
	return ret;
}


static int AosQoSTrafficCli_ShowMonitorDirection(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	char direction[8];
	char local[64];

	ret = 0;

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);

	switch (monitor_test_direction) {
		case monitor_direction_none:
			strcpy(direction,"none");
			break;
		case monitor_direction_in:
			strcpy(direction,"in");
			break;
		case monitor_direction_out:
			strcpy(direction,"out");
			break;
		case monitor_direction_both:
			strcpy(direction,"both");
			break;
		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid priority");
			return -1;
	}

	sprintf(local,"Now the direction of monitoring is %s", direction);

	aosCheckAndCopy(rsltBuff,&rsltIndex,
					optlen,
					local,
					strlen(local));

	*length = rsltIndex;

	return ret;
}


static int AosQoSCli_BandwidthMonitorShowChannel(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	struct bw_monitor_show_arg args;	
	u8 proto;
	u32 src_ip;
	u8 src_mask_bit;
	u8 dst_mask_bit;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;

	char msg[1024];


	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);

	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	//TBD
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		sprintf(msg,"proto name is invalid.\n");
		ret = -eAosRc_InvalidProtocol;
		goto fail;
	}
	
	ret = is_valid_ip_with_mask_bit(parms->mStrings[1],&src_ip,&src_mask_bit);
	if (ret != 0) {
		sprintf(msg,"source ip/mask_bit is not valid.\n");
		ret = -eAosRc_InvalidIPAddress;
		goto fail;
	}

	ret = is_valid_ip_with_mask_bit(parms->mStrings[2],&dst_ip,&dst_mask_bit);
	if (ret != 0) {
		sprintf(msg,"destination ip/mask_bit is not valid.\n");
		ret = -eAosRc_InvalidIPAddress;
		goto fail;
	}

	ret = is_valid_port_with_wild(parms->mIntegers[0],&src_port);
	if (ret != 0) {
		sprintf(msg,"source port number is not valid.\n");
		ret = -eAosRc_InvalidPort;
		goto fail;
	}

	ret = is_valid_port_with_wild(parms->mIntegers[1],&dst_port);
	if (ret != 0) {
		sprintf(msg,"destination port number is not valid.\n");
		ret = -eAosRc_InvalidPort;
		goto fail;
	}

	//Argument check has been finished

	args.type = bw_show_ch;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;
	args.rsltBuff = rsltBuff;
	args.parms.chdata.src_ip = src_ip;
	args.parms.chdata.dst_ip = dst_ip;
	args.parms.chdata.src_port = htons(src_port);
	args.parms.chdata.dst_port = htons(dst_port);
	args.parms.chdata.proto = proto;
	args.parms.chdata.src_maskbit = src_mask_bit;
	args.parms.chdata.dst_maskbit = dst_mask_bit;

#ifdef __KERNEL__//CLI run at bottomhalf
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,0);             //0 for group
#ifdef __KERNEL__
	local_bh_enable();
#endif

	*length = args.rsltIndex;
	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowChannelHistory(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	struct bw_history_show_arg args;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  proto;
	u16 start_time;
	u16 end_time;
	u16 interval;
	char msg[1024];

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		sprintf(msg,"proto name is invalid.\n");
		ret = -eAosRc_InvalidProtocol;
		goto fail;
	}

	ret = is_valid_ip_address(parms->mIntegers[0],&src_ip);
	if (ret != 0) {
		sprintf(msg,"Invalid source IP address\n");
		ret = -eAosRc_InvalidIPAddress;
		goto fail;
	}

	ret = is_valid_ip_address(parms->mIntegers[2],&dst_ip);
	if (ret != 0) {
		sprintf(msg,"Invalid destination IP address\n");
		ret = -eAosRc_InvalidIPAddress;
		goto fail;
	}
	ret = is_valid_port_without_wild(parms->mIntegers[1],&src_port);
	if (ret != 0) {
		sprintf(msg,"Invalid source port number\n");
		ret = -eAosRc_InvalidPort;
		goto fail;
	}

	ret = is_valid_port_without_wild(parms->mIntegers[3],&dst_port);
	if (ret != 0) {
		sprintf(msg,"Invalid destin port number\n");
		ret = -eAosRc_InvalidPort;
		goto fail;
	}

	ret = is_valid_past_time(parms->mStrings[1],&start_time);
	if (ret != 0) {
		sprintf(msg,"Invalid start time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}

	ret = is_valid_past_time(parms->mStrings[2],&end_time);//0 as default
	if (ret !=0) {
		sprintf(msg,"Invalid end time\n");
		ret = -eAosRc_InvalidEndTime;
		goto fail;
	}
	if (start_time < end_time) {
		sprintf(msg, "start time must be equal or more than end time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}	
	//Reviewed Compare relation
	ret = is_valid_interval(parms->mIntegers[4],&interval);//1 as default
	if (ret !=0) {
		sprintf(msg,"Invalid end time\n");
		ret = -eAosRc_InvalidInterval;
		goto fail;
	}


	args.type = bw_show_ch;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;
	args.rsltBuff = rsltBuff;
	args.start_time = start_time;
	args.end_time = end_time;
	args.interval = interval;
	args.parms.chhisdata.src_ip = src_ip;
	args.parms.chhisdata.dst_ip = dst_ip;
	args.parms.chhisdata.src_port = htons(src_port);
	args.parms.chhisdata.dst_port = htons(dst_port);
	args.parms.chhisdata.proto = proto;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_history_details(&args);
#ifdef __KERNEL__
	local_bh_enable();
#endif

	*length = args.rsltIndex;


	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowChannelAll(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	struct bw_monitor_show_arg args;
	char msg[64];
	int ret;

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	args.type = bw_show_ch;
	args.rsltIndex = rsltIndex;
	args.rsltBuff = rsltBuff;
	args.optlen = optlen;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,1);             //1 for all channels
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = args.rsltIndex;

	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowDev(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	int if_index;
	u8 direction;
	struct bw_monitor_show_arg args;
	char msg[1024];

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	ret = is_valid_dev_name(parms->mStrings[0],&if_index);
	if (ret != 0) {
		sprintf(msg,"Invalided device name\n");
		ret = -eAosRc_InvalidDevice;
		goto fail;
	}

	ret = is_valid_dev_direction(parms->mStrings[1],&direction);
	if (ret != 0) {
		sprintf(msg,"Invalid direction\n");
		ret = -eAosRc_InvalidDirection;
		goto fail;
	}
	args.type = bw_show_eth;
	args.rsltIndex = rsltIndex;
	args.rsltBuff = rsltBuff;
	args.optlen = optlen;
	args.parms.ethdata.if_index = if_index;
	args.parms.ethdata.direction = direction;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,0);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = args.rsltIndex;

	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowDevHistory(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	u8 direction;
	int  if_index;
	u16 start_time;
	u16 end_time;
	u16 interval;
	struct bw_history_show_arg args;
	char msg[1024];

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	ret = is_valid_dev_name(parms->mStrings[0],&if_index);
	if (ret != 0) {
		sprintf(msg,"Invalided device name\n");
		ret = -eAosRc_InvalidDevice;
		goto fail;
	}

	ret = is_valid_dev_direction(parms->mStrings[3],&direction);
	if (ret != 0) {
		sprintf(msg,"Invalid direction\n");
		ret = -eAosRc_InvalidDirection;
		goto fail;
	}

	ret = is_valid_past_time(parms->mStrings[1],&start_time);
	if (ret != 0) {
		sprintf(msg,"Invalid start time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}

	ret = is_valid_past_time(parms->mStrings[2],&end_time);//0 as default
	if (ret !=0) {
		sprintf(msg,"Invalid end time\n");
		ret = -eAosRc_InvalidEndTime;
		goto fail;
	}
	if (start_time < end_time) {
		sprintf(msg, "start time must be equal or more than end time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}
	ret = is_valid_interval(parms->mIntegers[0],&interval);//1 as default
	if (ret !=0) {
		sprintf(msg,"Invalid Interval\n");
		ret = -eAosRc_InvalidInterval;
		goto fail;
	}

	args.type = bw_show_eth;
	args.rsltIndex = rsltIndex;
	args.rsltBuff = rsltBuff;
	args.optlen = optlen;
	args.interval = interval;
	args.start_time = start_time;
	args.end_time = end_time;

	args.parms.ethhisdata.if_index = if_index;
	args.parms.ethhisdata.direction = direction;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_history_details(&args);	
#ifdef __KERNEL__
	local_bh_enable();
#endif

	*length = args.rsltIndex;

	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowDevAll(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	char *rsltBuff = NULL;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	struct bw_monitor_show_arg args;
	char msg[64];
	int ret;

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}
	
	args.type = bw_show_eth;
	args.rsltIndex = rsltIndex;
	args.rsltBuff = rsltBuff;
	args.optlen = optlen;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,1);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = args.rsltIndex;

	return 0;
fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowPriority(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	int if_index;
	u8 priority;
	struct bw_monitor_show_arg args;
	char msg[1024];

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}
	
	ret = is_valid_dev_name(parms->mStrings[0],&if_index);
	if (ret != 0) {
		sprintf(msg,"Invalid device name\n");
		ret = -eAosRc_InvalidDevice;
		goto fail;
	}
	if (parms->mStrings[1] != NULL) {
		ret = is_valid_priority(parms->mStrings[1],&priority);
	} else {
		priority = eROS_QOS_PRIORITY_MAX;
	}

	if (ret != 0) {
		sprintf(msg,"Invalid priority\n");
		ret = -eAosRc_InvalidPriority;
		goto fail;
	}

	args.type = bw_show_pri; 
	args.rsltBuff = rsltBuff;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;

	args.parms.pridata.if_index = if_index;
	args.parms.pridata.priority = priority;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,0);
#ifdef __KERNEL__
	local_bh_enable();
#endif

	*length = args.rsltIndex;

	return 0;
fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowPriorityAll(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	struct bw_monitor_show_arg args;
	char msg[1024];
	int ret;

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	args.type = bw_show_pri; 
	args.rsltBuff = rsltBuff;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_bandwidth_details(&args,1);
#ifdef __KERNEL__
	local_bh_enable();
#endif

	*length = args.rsltIndex;

	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;
}


static int AosQoSCli_BandwidthMonitorShowPriorityHistory(char *data,
											 unsigned int *length, 
											 struct aosKernelApiParms *parms,
											 char *errmsg, 
											 const int errlen)
{
	int ret;
	int optlen = *length;
	unsigned int rsltIndex = 0;
	char *rsltBuff = NULL;
	int if_index;
	u8 priority;
	char msg[1024];
	struct bw_history_show_arg args;

	u16 start_time;
	u16 end_time;
	u16 interval;

	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	ret = 0;

	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
		sprintf(msg,"The facility has not been turned on\n");
		ret = -eAosRc_BandwidthMonitorOff;
		goto fail;
	}

	ret = is_valid_dev_name(parms->mStrings[0],&if_index);
	if (ret != 0) {
		sprintf(msg,"Invalid device name\n");
		ret = -eAosRc_InvalidDevice;
		goto fail;
	}

	
	if (parms->mStrings[3] != NULL) {
		ret = is_valid_priority(parms->mStrings[3],&priority);
	} else {
		priority = eROS_QOS_PRIORITY_MAX;
	}
	if (ret != 0) {
		sprintf(msg,"Invalid priority\n");
		ret = -eAosRc_InvalidPriority;
		goto fail;
	}
	ret = is_valid_past_time(parms->mStrings[1],&start_time);
	if (ret != 0) {
		sprintf(msg,"Invalid start time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}

	ret = is_valid_past_time(parms->mStrings[2],&end_time);//0 as default
	if (ret !=0) {
		sprintf(msg,"Invalid end time\n");
		ret = -eAosRc_InvalidEndTime;
		goto fail;
	}
	if (start_time < end_time) {
		sprintf(msg, "start time must be equal or more than end time\n");
		ret = -eAosRc_InvalidStartTime;
		goto fail;
	}
	ret = is_valid_interval(parms->mIntegers[0],&interval);//1 as default
	if (ret !=0) {
		sprintf(msg,"Invalid end time\n");
		ret = -eAosRc_InvalidInterval;
		goto fail;
	}

	args.type = bw_show_pri;
	args.rsltBuff = rsltBuff;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;
	args.interval = interval;
	args.start_time = start_time;
	args.end_time = end_time;

	args.parms.prihisdata.if_index = if_index;
	args.parms.prihisdata.priority = priority;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	qos_show_history_details(&args);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = args.rsltIndex;

	return 0;

fail:
	strncpy(errmsg, msg, errlen - 1);
	errmsg[errlen - 1] = 0;
	return ret;

}

static int AosQoSCli_BandwidthMonitorSetMaxHistoryInterval(char *data,
														unsigned int *length,
														struct aosKernelApiParms *parms,													                                         char *errmsg, 
														const int errlen)
{
	int interval = parms->mIntegers[0];

	*length = 0;

	if (interval <= 0) {
		strncpy(errmsg, "max history interval must be more than 0\n", errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_InvalidParam;;
	}
	sgMax_history_len = interval * HZ;
	
	ros_qos_config_log(parms->mCmd);
	return 0;	
}

static int AosQos_registerTrafficCli(void)
{
	// 
	// Chen Ding, 01/10/2007
	//
	static int lsRulesAdded = 0;

	int ret = 0;
	
	// 
	// Chen Ding, 01/10/2007
	//
	if (!lsRulesAdded)
	{
		ret |= OmnKernelApi_addCliCmd("QosTrafficStatus", AosQoSTrafficCli_setTrafficStatus);
		ret |= OmnKernelApi_addCliCmd("QosTrafficAddMonitorDirection", AosQoSTrafficCli_addMonitorDirection);
		ret |= OmnKernelApi_addCliCmd("QosTrafficDelMonitorDirection", AosQoSTrafficCli_delMonitorDirection);
		ret |= OmnKernelApi_addCliCmd("QosTrafficShowMonitorDirection", AosQoSTrafficCli_ShowMonitorDirection);

		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowChannel", AosQoSCli_BandwidthMonitorShowChannel);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowChannelHistory", AosQoSCli_BandwidthMonitorShowChannelHistory);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowChannelAll", AosQoSCli_BandwidthMonitorShowChannelAll);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowDev", AosQoSCli_BandwidthMonitorShowDev);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowDevHistory", AosQoSCli_BandwidthMonitorShowDevHistory);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowDevAll", AosQoSCli_BandwidthMonitorShowDevAll);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowPriority", AosQoSCli_BandwidthMonitorShowPriority);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowPriorityHistory", AosQoSCli_BandwidthMonitorShowPriorityHistory);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorShowPriorityAll", AosQoSCli_BandwidthMonitorShowPriorityAll);
		ret |= OmnKernelApi_addCliCmd("QosBandwidthMonitorMaxHistoryInterval", AosQoSCli_BandwidthMonitorSetMaxHistoryInterval);
		lsRulesAdded = 1;
	}

	return ret;
}


void AosQos_unregisterTrafficCli(void)
{
	
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowPriorityAll");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowPriority");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowPriorityHistory");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowDevAll");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowDevHistory");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowDev");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowChannelAll");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowChannelHistory");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorShowChannel");
	OmnKernelApi_delCliCmd("QosTrafficShowMonitorDirection");
	OmnKernelApi_delCliCmd("QosTrafficDelMonitorDirection");
	OmnKernelApi_delCliCmd("QosTrafficAddMonitorDirection");
	OmnKernelApi_delCliCmd("QosTrafficStatus");
	OmnKernelApi_delCliCmd("QosBandwidthMonitorMaxHistoryInterval");
	return;
}


int AosQos_initTrafficModule(void)
{
	int ret;
	ret = AosQos_registerTrafficCli();
	ret = aos_qos_traffic_init_variables();
	return ret;
}


void AosQos_exitTrafficModule(void)
{
	AosQos_unregisterTrafficCli();
	qos_traffic_set_status(AOS_QOS_TRAFFIC_STATUS_OFF);
	return;
}
