//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_shaper.c
// Description:
//   
// Author: Tim
//
// Modification History: 
//	created 01/17/2007
// 
////////////////////////////////////////////////////////////////////////////
#include "QoS/qos_shaper.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/List.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/BusyLoop.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/netfilter.h"
#include "KernelSimu/skbuff.h"

#include "QoS/hashtab.h" 
#include "QoS/qos_traffic.h" 
#include "QoS/qos_util.h" 
#include "QoS/qos_logstat.h"

static struct hashtab *qos_ch_shaper_hash = NULL;
static struct timer_list sg_shaper_timer;
static u16 sg_time_slot_iv = QOS_SHAPER_TIME_SLOT_DEF_INTERVAL; 		// time slot interval 100 ms
static u16 sg_time_slot_counts = HZ/QOS_SHAPER_TIME_SLOT_DEF_INTERVAL;	// time slot counts in one second
static u16 sg_qos_avr_bw_cal_iv = QOS_SHAPER_AVR_BW_CAL_DEF_INTERVAL; 	// average bw calculate interval 
static u8 sg_qos_shaper_ctrl = 0;  	// default disable qos traffic shaper


static inline int _qos_shaper_data_init(struct qos_shaper_data *shaper_data)
{
	skb_queue_head_init(&(shaper_data->skb_queue));
	return eAosRc_Success;
}

static inline void _qos_shaper_data_clear(struct qos_shaper_data *shaper_data)
{
	struct sk_buff *skb;
	while(NULL != (skb = skb_dequeue(&shaper_data->skb_queue)))
	{
		kfree_skb(skb);
	}
}

static struct qos_ch_shaper* qos_ch_shaper_data_create(void)
{
	struct qos_ch_shaper* ch_shaper;

#ifdef __KERNEL__
	ch_shaper = (struct qos_ch_shaper *)aos_malloc_atomic(sizeof(struct qos_ch_shaper));
#else
	ch_shaper = (struct qos_ch_shaper *)aos_malloc(sizeof(struct qos_ch_shaper));
#endif
	if (!ch_shaper) return NULL;
	
	memset(ch_shaper, 0, sizeof(struct qos_ch_shaper));
	_qos_shaper_data_init(&(ch_shaper->shaper_data));

	return ch_shaper;
}

static inline void qos_ch_shaper_data_release(struct qos_ch_shaper *ch_shaper)
{
	_qos_shaper_data_clear(&(ch_shaper->shaper_data));
	aos_free(ch_shaper);
}

static int ch_shaper_create_key(
	u32 src_ip, 
	u32 dst_ip, 
	u16 src_port, 
	u16 dst_port, 
	u8  proto,
	struct ch_shaper_hkey *k)
{
	u8 *pos;

	memset(k->k,0,QOS_SHAPER_CH_HASH_KEY_SIZE);
	pos = k->k;
	memcpy(pos,&src_ip,sizeof(u32));
	pos += sizeof(u32);
	memcpy(pos,&dst_ip,sizeof(u32));
	pos += sizeof(u32);
	memcpy(pos,&src_port,sizeof(u16));
	pos += sizeof(u16);
	memcpy(pos,&dst_port,sizeof(u16));
	pos += sizeof(u16);
	memcpy(pos,&proto,sizeof(u8));
	pos += sizeof(u8);
	return 0;
}

static struct qos_ch_shaper* build_ch_shaper_data(
	u32 src_ip,
	u32 dst_ip,
	u16 src_port,
	u16 dst_port,
	u8  proto,
	u8  priority)
{
	struct qos_ch_shaper* ch_shaper;
	struct ch_shaper_hkey hkey;

	ch_shaper = qos_ch_shaper_data_create();
	if (!ch_shaper) 
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to create ch shaper data");
		return NULL;
	}
	
	ch_shaper_create_key(src_ip, dst_ip, src_port, dst_port, proto, &hkey);

#ifdef __KERNEL__
		local_bh_disable();
#endif
	if (hashtab_insert(qos_ch_shaper_hash, &hkey, ch_shaper))
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to insert channel shaper date into hashtable");
		#ifdef __KERNEL__
		local_bh_enable();
		#endif

		goto build_failed;
	}
#ifdef __KERNEL__
		local_bh_enable();
#endif

	ch_shaper->ch.src_ip = src_ip;
	ch_shaper->ch.dst_ip = dst_ip;
	ch_shaper->ch.src_port = src_port;
	ch_shaper->ch.dst_port = dst_port;
	ch_shaper->ch.protocol = proto;
	ch_shaper->ch.priority = priority;

	return ch_shaper;
	
build_failed:
	if (ch_shaper != NULL) 
	{
		aos_free(ch_shaper);
	}
	
	return NULL;
}


static void ros_qos_clear_ch_shaper_data(void *k, void *d, void *arg)
{
	struct qos_ch_shaper *ch_shaper; 
	struct sk_buff *skb;

	ch_shaper = (struct qos_ch_shaper*) d;
	// reinject buffered skb
	while(NULL != (skb = skb_dequeue(&(ch_shaper->shaper_data.skb_queue))) )
	{
		// reinject TBD.
	}
	qos_ch_shaper_data_release(ch_shaper);
}

static int if_remove_ch_shaper_data(void *k, void *d, void *arg)
{
	return 1;
}

static void ros_qos_release_ch_shaper_hashtable(void)
{
	aos_eng_log(eAosMD_QoS, "%s", __FUNCTION__);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(qos_ch_shaper_hash,
									if_remove_ch_shaper_data,
									ros_qos_clear_ch_shaper_data,
									NULL);
#ifdef __KERNEL__
	local_bh_enable();
#endif
		
}

static int ros_qos_proc_ch_shaper_data(void *k, void *d, void *arg)
{
	struct qos_ch_shaper *ch_shaper; 
	struct sk_buff *skb;

	aos_eng_log(eAosMD_QoS, "%s", __FUNCTION__);

	ch_shaper = (struct qos_ch_shaper*) d;

	// reinject buffered skb
	while(NULL != (skb = skb_dequeue(&(ch_shaper->shaper_data.skb_queue))) )
	{
		// check skb len
		ch_shaper->shaper_data.bytes_sent += skb->len;
		if ( (ch_shaper->shaper_data.bytes_sent  <= ch_shaper->shaper_data.bytes_allowed) )
		{
			// reinject TBD.

			continue;
		}
		else
		{
			// delay to send 
			skb_queue_head(&(ch_shaper->shaper_data.skb_queue), skb);
		}
		
	}

	// clear sent bytes
	ch_shaper->shaper_data.bytes_sent = 0;

		// if skb queue is empty, remove this channel
	if (skb_queue_empty(&(ch_shaper->shaper_data.skb_queue)))
	{
		return 1; // remove ch_shaper in hashtable
	}
		
	return 0;
}

int ros_qos_update_ch_avr_bw(
	struct _qos_traffic_channel *ch,
	struct qos_ch_shaper *ch_shaper)
{
	struct _qos_traffic_history_cell *history_cell;
	struct list_head *cur;
	u32 total_bw;
	u32 cur_time;
	u32 time_counts;
	
	aos_assert1(ch);
	aos_assert1(ch_shaper);
	
	aos_eng_log(eAosMD_QoS, "%s", __FUNCTION__);

	cur_time = QOS_GET_JIFFIES();
	
	if (!ch_shaper->shaper_data.last_time) 
	{
		ch_shaper->shaper_data.last_time = cur_time;
		ch_shaper->shaper_data.avr_bw = ch->record.total_band;
		ch_shaper->shaper_data.bytes_allowed = ch->record.total_band/sg_time_slot_counts;

		return eAosRc_Success;
	}

	total_bw = 0;
	if ( (cur_time-ch_shaper->shaper_data.last_time) >= sg_qos_avr_bw_cal_iv )
	{
		// calculate history bw
		time_counts = 0;
		list_for_each(cur, &(ch->record.channel_history_queue_head))
		{
			history_cell = list_entry(cur, struct _qos_traffic_history_cell, cell_head);
			if (time_counts >= sg_qos_avr_bw_cal_iv) break;
			time_counts += HZ; // one second each history data
			total_bw = history_cell->history_traffic;
		}
		if (!time_counts)
		{
			ch_shaper->shaper_data.last_time = cur_time;
			ch_shaper->shaper_data.avr_bw = total_bw/time_counts;
			ch_shaper->shaper_data.bytes_allowed = ch_shaper->shaper_data.avr_bw/sg_time_slot_counts;
		}
	}

	return eAosRc_Success;
}

int ros_qos_ch_shaper(
	struct sk_buff *skb,
	u32 src_ip, 
	u32 dst_ip, 
	u16 src_port, 
	u16 dst_port, 
	u8  protocol, 
	u8  priority)
{
	struct qos_ch_shaper *ch_shaper;
	struct ch_shaper_hkey hkey;

	aos_assert1(skb);

	aos_eng_log(eAosMD_QoS, "%s", __FUNCTION__);

	if (!sg_qos_shaper_ctrl) return NF_ACCEPT;
	
	ch_shaper_create_key(src_ip, dst_ip, src_port, dst_port, protocol, &hkey);

#ifdef __KERNEL__
	local_bh_disable();
#endif	
 	ch_shaper = (struct qos_ch_shaper *)hashtab_search(qos_ch_shaper_hash, &hkey);
#ifdef __KERNEL__
	local_bh_disable();
#endif	

	if (!ch_shaper)
	{
		ch_shaper = build_ch_shaper_data(src_ip, dst_ip, src_port, dst_port, protocol, priority);
		if (!ch_shaper)
		{
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to build ch shaper data");
			goto failed;
		}
		return NF_ACCEPT;
	}

	// check whether buffered queue is already exist, then queue skb 
	if  (!skb_queue_empty(&(ch_shaper->shaper_data.skb_queue)) || 
	      ch_shaper->shaper_data.bytes_sent  >= ch_shaper->shaper_data.bytes_allowed) 
	{
		skb_queue_tail(&(ch_shaper->shaper_data.skb_queue), skb);
		return NF_QUEUE;
	}

	ch_shaper->shaper_data.bytes_sent += skb->len;

	return NF_ACCEPT;
	
failed:
	if (ch_shaper) qos_ch_shaper_data_release(ch_shaper);
	return NF_DROP;

}


// run each time slot
void ros_qos_shaper_timer(unsigned long data)
{
	if (!sg_qos_shaper_ctrl) 
	{
		aos_min_log(eAosMD_QoS, "qos traffic shaper disabled");
		ros_qos_release_ch_shaper_hashtable();
		return;
	}

#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(qos_ch_shaper_hash,
									ros_qos_proc_ch_shaper_data,
									ros_qos_clear_ch_shaper_data,
									NULL);
#ifdef __KERNEL__
	local_bh_enable();
#endif

	// if ch queue is not empty, add timer for next time slot
	if (hashtab_elements(qos_ch_shaper_hash))
	{
		add_timer(&sg_shaper_timer);
	}
	
}

//
// CLI for QoS Traffic Shaper
//

// set time slot interval
int ros_qos_shaper_set_status_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos shaper set status <on/off>
	//
	char *ctrl= parms->mStrings[0];

	*length = 0;

	if (strcmp(ctrl, "on") == 0)
		sg_qos_shaper_ctrl = 1;
	else
		sg_qos_shaper_ctrl = 0;
	
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

// set time slot interval
int ros_qos_shaper_set_time_slot_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos shaper set time slot <interval>
	//
	int interval = parms->mIntegers[0];

	*length = 0;

	if (interval <= 0) 
	{
		sprintf(errmsg, "time slot interval should be integer larger than zero");
		return -1;
	}

	sg_time_slot_iv = interval;

	sg_time_slot_counts =  HZ/sg_time_slot_iv;

	if (sg_time_slot_counts <= 0)
	{
		sg_time_slot_counts = HZ/QOS_SHAPER_TIME_SLOT_DEF_INTERVAL;
	}
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

// set average bandwidth interval
int ros_qos_shaper_set_avr_bw_iv_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos shaper set average bandwidth <interval>
	//
	int interval = parms->mIntegers[0];

	*length = 0;

	if (interval <= 0) 
	{
		sprintf(errmsg, "Interval should be integer larger than zero");
		return -1;
	}

	sg_qos_avr_bw_cal_iv = interval;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

int ros_qos_shaper_show_param_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);
	char local[200];

	*length = 0;

	sprintf(local, "QoS Traffic Shaper: %s\n", sg_qos_shaper_ctrl?"on": "off");
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "\t Time Slot Interval: %d\n", sg_time_slot_iv);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "\t Time Slot Counts Per Second: %d\n", sg_time_slot_counts);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));


	sprintf(local, "\t Average Bandwidth Calculation Interval: %d\n", sg_qos_avr_bw_cal_iv);
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return eAosRc_Success;
}

static int qos_shaper_register_cli(void)
{
	static int is_cli_reg = 0;

	int ret = 0;
	
	if (is_cli_reg == 0)
	{
		ret |= OmnKernelApi_addCliCmd("QosShaperStatus", ros_qos_shaper_set_status_cli);
		ret |= OmnKernelApi_addCliCmd("QosShaperSetTimeSlot", ros_qos_shaper_set_time_slot_cli);
		ret |= OmnKernelApi_addCliCmd("QosShaperSetAvrBwInterval", ros_qos_shaper_set_avr_bw_iv_cli);
		ret |= OmnKernelApi_addCliCmd("QosShaperShow", ros_qos_shaper_show_param_cli);
		is_cli_reg = 1;
	}
	
	return ret;
}

void qos_shaper_unregister_cli(void)
{
	OmnKernelApi_delCliCmd("QosShaperStatus");
	OmnKernelApi_delCliCmd("QosShaperSetTimeSlot");
	OmnKernelApi_delCliCmd("QosShaperSetAvrBwInterval");
	OmnKernelApi_delCliCmd("QosShaperShow");
}

int ros_qos_shaper_init(void)
{
	static int qos_shaper_init = 0;

	aos_min_log(eAosMD_QoS, "QoS shaper module init");
	
	if (qos_shaper_init)
	{
		aos_eng_log(eAosMD_QoS, "QoS shaper module already inited");
		return eAosRc_Success;
	}

	qos_shaper_init = 1;

	sg_shaper_timer.function = ros_qos_shaper_timer;
	sg_shaper_timer.data = 0;
	sg_shaper_timer.expires = QOS_GET_JIFFIES() + sg_time_slot_iv;

	init_timer(&sg_shaper_timer);

	sg_time_slot_iv = QOS_SHAPER_TIME_SLOT_DEF_INTERVAL; 	// time slot interval 100 ms

	qos_shaper_register_cli();

	return eAosRc_Success;
}

