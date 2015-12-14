/*
 * Copyright (c) 2006 PacketEngineering Ltd,Co
 * All rights reserved
 *
 * Filename: traffic.h 
 * Fileid:   TBD
 * Abstract: The main work of this program is to monitor the bandwidth 
 * Current Verion: 1.0
 * Author: Ping Wang
 * Complete Date: 11/25/2006
 *
 * Replace Version: N/A
 * Author: N/A
 * Complete Date: N/A
 */
#include "KernelSimu/if.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/ip.h"
#include "KernelSimu/tcp.h"
#include "KernelSimu/udp.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/thread.h"
#include "KernelSimu/jiffies.h"
#include "KernelSimu/wait.h"
#include "KernelSimu/kernel.h" 
#include "KernelSimu/string.h"
#include "KernelSimu/delay.h" 
#include "KernelSimu/errno.h"
#include "aos/aosReturnCode.h"
#include "KernelSimu/if_ether.h"
#include "KernelSimu/netfilter.h"
#include "aosUtil/List.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/BusyLoop.h"
#include "aosUtil/ReturnCode.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_bw_man.h"
#include "QoS/qos_logstat.h"

#define DBG_TRAFFIC 1

#define QOS_MAX_CACHE_SIZE 1000
#define QOS_CACHE_BUCKET_SIZE 100

int sgMax_history_len = 60 * HZ;

int build_pri_ch(int ifid,u8 direction,struct qos_bw_ctrl_data **r,struct prihashkey *k);

int aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_OFF;
static unsigned long aos_qos_traffic_global_temp_jiffy = 0;
static unsigned long aos_qos_traffic_global_remove_jiffy = 0;
static u32 aos_qos_traffic_global_calculate_interval = 1000;
struct _qos_traffic_channel_record pri_recd[3];

struct hashtab *qos_traffic_hash_gen;
struct hashtab *qos_traffic_hash_eth;
struct hashtab *qos_traffic_hash_pri;
//struct list_head qos_traffic_list_gen;
///////////////////////////////////////////////
//Test variables
int monitor_test_direction = monitor_direction_both;
int dbg_print_count = 0;

static int aos_qos_traffic_global_want_to_remove = 0;

static int aos_qos_traffic_global_thread_working_signal = 0;
#ifdef __KERNEL__
static spinlock_t aos_qos_traffic_global_spinlock = SPIN_LOCK_UNLOCKED;
static wait_queue_head_t aos_qos_traffic_global_waiting_queue; 
#else
aos_mutex_t history_mutex;
#endif

/* used by weight order */
static int weight_order;
/* the channel cache is updated by timer */
static struct timer_list updated_cache_timer;

static inline void free_qos_traffic_channel_record(struct _qos_traffic_channel_record *record)
{
	aos_assert0(record != NULL);
	if (record == NULL)
		return;
	
	if (record->array_start != NULL) {
		aos_free(record->array_start);
		record->array_start = NULL;
	}
	clear_all_history_record(record);                      
	aos_free(record);
	return;
}

static inline int clear_channel(void *k,void *d,void *args)
{
	return 1;
}


static inline void destroy_pri_cell(void *k,void *d,void *args)
{
	struct qos_bw_ctrl_data *cell;
	cell = (struct qos_bw_ctrl_data *)d;
	qos_bw_ctrl_data_put(cell);
	aos_free(k);
	return;
}

void simple_destroy_record(void *k,void *d,void *args)
{
	struct _qos_traffic_channel_record *record = (struct _qos_traffic_channel_record *)d;
	
	if (record == NULL)
		return;
	
	aos_assert0(record);
	
	if (record->array_start != NULL) {
		aos_free(record->array_start);
		record->array_start = NULL;
	}
	clear_all_history_record(record);

	return;
}

static inline void destroy_record(void *k,void *d,void *args)
{

	free_qos_traffic_channel_record((struct _qos_traffic_channel_record *)d);
	
	if(k) aos_free(k);
	
	return;
}

void qos_channel_release(struct _qos_traffic_channel *ch)
{
	simple_destroy_record(NULL, (void*)(&ch->record), NULL);
	if (ch->matched_rule)
	{
		if (*((u8*)ch->matched_rule) == AOS_QOS_RULE_GEN)
			qos_general_rule_put((struct _qos_rule_general *) ch->matched_rule);
		else	
			qos_specific_rule_put((struct _qos_rule_specific *) ch->matched_rule);
	}
	
	aos_free(ch);
}

int aos_qos_traffic_init_variables(void)
{
	aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_OFF;
	aos_qos_traffic_global_temp_jiffy = QOS_GET_JIFFIES();
	aos_qos_traffic_global_remove_jiffy = QOS_GET_JIFFIES();
	aos_qos_traffic_global_calculate_interval = 1000;                      //Review comment number to MARCO //Add comment
	aos_qos_traffic_global_want_to_remove = 0;
	aos_qos_traffic_global_thread_working_signal = 0;
	dbg_print_count = 0;
	monitor_test_direction = monitor_direction_both;

#ifdef __KERNEL__
	aos_qos_traffic_global_spinlock = SPIN_LOCK_UNLOCKED;
#else
	history_mutex = AOS_INIT_MUTEX(history_mutex);
#endif
	//INIT_LIST_HEAD(&qos_traffic_list_gen);
	init_waitqueue_head(&aos_qos_traffic_global_waiting_queue); 
	
	return 0;
}


int init_all_resources(void)
{
	int ret = 0;
	//int index;
	qos_traffic_hash_gen = hashtab_create(ch_hash_val_cal,
										  ch_hash_cmp,
										  (2 << QOS_TRAFFIC_CH_HASH_SIZE));
	if (qos_traffic_hash_gen == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate channel hashtable");
		ret = -ENOMEM;
		goto fail;
	}
	qos_traffic_hash_eth = hashtab_create(eth_hash_val_cal,
										  eth_hash_cmp,
										  (2 << QOS_TRAFFIC_ETH_HASH_SIZE));
	if (qos_traffic_hash_gen == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate eth hashtable");
		ret = -ENOMEM;
		goto fail;
	}

	qos_traffic_hash_pri = hashtab_create(pri_hash_val_cal,
										  pri_hash_cmp,
										  (2 << QOS_TRAFFIC_PRI_HASH_SIZE));
/*
#ifdef __KERNEL__
	ret = aos_thread_create(aos_qos_traffic_remove_thread,NULL,CLONE_KERNEL);
	if (ret < 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to start thread");
		goto fail;
	}
#else
	aos_thread_create(aos_qos_traffic_remove_thread,NULL,CLONE_KERNEL);
#endif
*/
	init_timer(&updated_cache_timer);
	updated_cache_timer.data = 0;
	updated_cache_timer.expires = QOS_GET_JIFFIES()  + 1000;
	updated_cache_timer.function = aos_qos_traffic_updated_channel_cache;
	add_timer(&updated_cache_timer);
	
	return 0;
fail:
	return ret;
}


void free_all_resources(void)
{
	//struct _qos_traffic_channel *node;
	//struct _qos_traffic_channel *tmp;
	
	//list_for_each_entry_safe(node, tmp, &qos_traffic_list_gen, list) {
	//	list_del(&(node->list));
	//}
	
	//aos_qos_traffic_global_want_to_remove = 1;
	//QOS_TRAFFIC_PRINTK("Trying to remove thread\n");
	//wait_event_interruptible(aos_qos_traffic_global_waiting_queue,(aos_qos_traffic_global_thread_working_signal != 0));

#ifdef __KERNEL__
	local_bh_disable();
#endif

	hashtab_map_remove_on_error(qos_traffic_hash_gen,clear_channel,destroy_ch_cell,NULL);
	hashtab_destroy(qos_traffic_hash_gen);
	qos_traffic_hash_gen = NULL;
	
	hashtab_map_remove_on_error(qos_traffic_hash_eth,clear_channel,destroy_record,NULL);
	hashtab_destroy(qos_traffic_hash_eth);
	qos_traffic_hash_eth = NULL;
	
	hashtab_map_remove_on_error(qos_traffic_hash_pri,clear_channel,destroy_pri_cell,NULL);
	hashtab_destroy(qos_traffic_hash_pri);
	qos_traffic_hash_pri = NULL;
	
	aos_qos_traffic_init_variables();

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}



//////////////////////////////////////////////////////////////////////////////////////////

// When adding removing modifying a rule
// The chhashtable should be updated
//
// when removing channel the status will be marked
//
// The above means when the rule does not exist
// the channel match the rule will be marked as low priority
// when 3 seconds with out coming 1 pkt
// the channel will be set inactive
// when 1 min without coming 1 pkt
// the channel will be destroyed
/*
int aos_qos_traffic_remove_thread(void *arg)
{
	
	QOS_TRAFFIC_PRINTK("Initing remove thread\n");
	
	while (1) {
		if (aos_qos_traffic_global_want_to_remove == 1) {
			QOS_TRAFFIC_PRINTK("To remove thread\n");
			break;
		}
#ifdef __KERNEL__
		local_bh_disable();
#else
		aos_lock(history_mutex);
#endif
		aos_qos_traffic_global_remove_jiffy = QOS_GET_JIFFIES();
	//	aos_trace("here");
//		aos_trace("Removing............................................");
		hashtab_map_remove_on_error(qos_traffic_hash_gen,
									if_remove_channel,
									destroy_ch_cell,
									NULL);
//		aos_trace("removed");
		hashtab_map(qos_traffic_hash_eth,clear_history_record,NULL);
		hashtab_map(qos_traffic_hash_pri,clear_pri_history_record,NULL);
		
#ifdef __KERNEL__
		local_bh_enable();
#else
		aos_unlock(history_mutex);
#endif
		ssleep(3);

	}
	aos_qos_traffic_global_want_to_remove = 0;
	aos_qos_traffic_global_thread_working_signal = 1;
	wake_up_interruptible(&aos_qos_traffic_global_waiting_queue);
	return 0;
}
*/

void aos_qos_traffic_updated_channel_cache(unsigned long data)
{
	if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF)
		return;
	
	hashtab_map_remove_on_error(qos_traffic_hash_gen,
		if_remove_channel,
		destroy_ch_cell,
		NULL);
	hashtab_map(qos_traffic_hash_eth,
		clear_history_record,
		NULL);
	hashtab_map(qos_traffic_hash_pri,
		clear_pri_history_record,
		NULL);
	
	updated_cache_timer.expires = QOS_GET_JIFFIES() + 1000;
	add_timer(&updated_cache_timer);
}

int qos_traffic_monitor(struct sk_buff *skb, struct ros_qos_skb_info *skb_info, int direction, u8 *priority)
{
	//struct ros_qos_skb_info skb_info;
	unsigned long temp_jiffy;
	u32 band_req;
	int ret = 0;
	
	// aos_trace("enter %s", __FUNCTION__);

	*priority = 0;
	// aos_qos_traffic_global_temp_jiffy = jiffies;
	temp_jiffy = aos_qos_traffic_global_temp_jiffy;
	aos_qos_traffic_global_temp_jiffy = QOS_GET_JIFFIES();
//	aos_trace("last:%u,now:%u",temp_jiffy,aos_qos_traffic_global_temp_jiffy);

/*	if (aos_qos_traffic_global_temp_jiffy < temp_jiffy) {
		aos_trace("last:%u",temp_jiffy);
		aos_trace("now:%u",aos_qos_traffic_global_temp_jiffy);
		ssleep(1000000);
	}*/
	
	if (aos_qos_traffic_facility_status != AOS_QOS_TRAFFIC_STATUS_ON) {
		return 0;
	}
	//ret = qos_traffic_parse_parm(skb, &skb_info, direction);     //Need to be finished
	//if (ret != 0) {
	//	return 0;
	//}											//It will return when the packet is not IP packet

#ifndef __KERNEL__
	aos_lock(history_mutex);
#endif
	ret = match_build_channel_hashtable(skb, skb_info->src_ip, skb_info->dst_ip, skb_info->src_port, 
			                            skb_info->dst_port, skb_info->protocol, skb_info->ifid,
										skb_info->src_mac, skb_info->dst_mac, skb_info->vlan, 
										skb_info->len, priority, &band_req, direction);
	if (ret) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Error Happend when updating channel hashtable with return value %d",ret);
		goto fail1;
	}

	ret = match_build_priority_hash(skb_info->ifid, direction, *priority, skb_info->len, band_req);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Error Happend when updating priority hashtable with return value %d",ret);
		goto fail2;
	}
	
	ret = match_build_ifhash(skb_info->ifid, skb_info->len, direction);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Error Happend when updating eth hashtable with return value %d",ret);
		goto fail3;
	}

#ifndef __KERNEL__
	aos_unlock(history_mutex);
#endif

	return ret;
fail3:
fail2:
fail1:
#ifndef __KERNEL__
	aos_unlock(history_mutex);
#endif
	return ret;
}

int qos_bandwidth_monitor(int nf_dir, int direction, struct ros_qos_skb_info *skb_info, u8 *priority)
{
	int ret = 0;
	int band_req;
	//int in_ifid;

	//aos_trace("enter %s", __FUNCTION__);
	  
	if (!skb_info) return 0;
	
	aos_qos_traffic_global_temp_jiffy = QOS_GET_JIFFIES();
	
	if (aos_qos_traffic_facility_status != AOS_QOS_TRAFFIC_STATUS_ON) {
		return 0;
	}
	
	if (skb_info->mac_proto != 0x0008) { // it is not ip packet
/*#ifndef __KERNEL__
		aos_lock(history_mutex);
#endif
		match_build_ifhash(skb_info->ifid, skb_info->len, direction);
#ifndef __KERNEL__
		aos_unlock(history_mutex);
#endif*/
		return 0;
	}											//It will return when the packet is not IP packet

#ifndef __KERNEL__
	aos_lock(history_mutex);
#endif
	if (nf_dir == local_in) {
		ret = match_update_channel_hashtable(nf_dir, skb_info, priority);
		goto out;
	}
	
	if (nf_dir == post_routing) {
		if (skb_info->ch == NULL)
			ret = build_channel_hashtable(skb_info->src_ip, skb_info->dst_ip, skb_info->src_port, 
			                            skb_info->dst_port, skb_info->protocol, skb_info->ifid,
										skb_info->src_mac, skb_info->dst_mac, skb_info->vlan, 
										skb_info->len, priority, &band_req, direction);
		match_update_channel_hashtable(direction, skb_info, priority);
	} //else {
	//	match_update_channel_hashtable(direction, skb_info, priority);
	//}

	// update priority based bandwidth
	if (nf_dir == post_routing) 
		ret = match_build_priority_hash(skb_info->ifid, direction, *priority, skb_info->len, band_req);
	
	ret = match_update_priority_hash(skb_info->ifid, direction, *priority, skb_info->len);
	ros_qos_priority_stat(skb_info->ifid, direction, NF_ACCEPT, *priority, 
				(skb_info->len>=ETH_HLEN) ? (skb_info->len-ETH_HLEN): skb_info->len);
	
	// update interface based bandwidth
	aos_trace("%s ifid %d, direction %d", __FUNCTION__, skb_info->ifid, direction);
	if (nf_dir == post_routing)
		ret = match_build_ifhash(skb_info->ifid, skb_info->len, direction);
	ret = match_update_ifhash(skb_info->ifid, skb_info->len, direction);

	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Error Happend when updating eth hashtable with return value %d",ret);
	}

	//aos_trace("Traffic Monitor updated with priority %u", *priority);
out:
#ifndef __KERNEL__
	aos_unlock(history_mutex);
#endif
	return ret;
}

struct _qos_traffic_channel *build_peer_channel(struct _qos_traffic_channel *ch)
{
	struct _qos_traffic_channel *new_ch;
	struct chhashkey *ch_key; 
	int ret;

	if (!ch) return NULL;
	
#ifdef __KERNEL__
	new_ch = (struct _qos_traffic_channel *)aos_malloc_atomic(sizeof(struct _qos_traffic_channel));
#else
	new_ch = (struct _qos_traffic_channel *)aos_malloc(sizeof(struct _qos_traffic_channel));
#endif

	if (new_ch == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for new channel");
		return NULL;
	}
	
#ifdef __KERNEL__
	ch_key = (struct chhashkey *)aos_malloc_atomic(sizeof(struct chhashkey));
#else
	ch_key = (struct chhashkey *)aos_malloc(sizeof(struct chhashkey));
#endif

	if (ch_key == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for channel hash key");
		ret = -ENOMEM;
		goto fail;
	}

	memset(new_ch, 0, sizeof(*new_ch));
	
	atomic_set(&new_ch->refcnt, 1);
	
	// switch src_ip src_port with dst_ip dst_port
	new_ch->src_ip = ch->dst_ip;
	new_ch->dst_ip = ch->src_ip;
	new_ch->src_port = ch->dst_port;
	new_ch->dst_port = ch->src_port;
	new_ch->protocol = ch->protocol;
	new_ch->priority = ch->priority;
	new_ch->status = ch->status;
	new_ch->ifid = ch->ifid;
	new_ch->vlan = ch->vlan;
	memcpy(new_ch->src_mac, ch->dst_mac, ETH_ALEN);
	memcpy(new_ch->dst_mac, ch->src_mac, ETH_ALEN);
	new_ch->matched_rule = ch->matched_rule;
	if (new_ch->matched_rule)
	{
		if (*((u8*)new_ch->matched_rule) == AOS_QOS_RULE_GEN)
			qos_general_rule_hold((struct _qos_rule_general *) new_ch->matched_rule);
		else	
			qos_specific_rule_hold((struct _qos_rule_specific *) new_ch->matched_rule);
	}
	
	aos_debug_log(eAosMD_QoS, "orginal channel src_ip %u, src_port %d, dst_ip %u, dst_port %d, proto %d"
			                  "ifid %d, vlan %d, rule %x", ch->src_ip, ch->src_port, ch->dst_ip, 
							  ch->dst_port, ch->protocol, ch->ifid, ch->vlan, ch->matched_rule);

	aos_debug_log(eAosMD_QoS, "peer channel src_ip %u, src_port %d, dst_ip %u, dst_port %d, proto %d"
			                  "ifid %d, vlan %d, rule %x", new_ch->src_ip, new_ch->src_port, new_ch->dst_ip, 
							  new_ch->dst_port, new_ch->protocol, new_ch->ifid, new_ch->vlan, new_ch->matched_rule);
			
	ret = init_channel_record(&new_ch->record);
	if (ret) 
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed when initing channel record");
		goto fail;
	}	

	ret = ch_create_hashkey(new_ch->src_ip, new_ch->dst_ip, new_ch->src_port, new_ch->dst_port, new_ch->protocol, ch_key);
	if (ret) 
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to create channel hash key");
		goto fail;
	}

	ret = hashtab_insert(qos_traffic_hash_gen, ch_key, new_ch);
	if (ret) 
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed when inserting channeli into hashtable");
		goto fail;
	}
	
	return new_ch;
fail:
	if (new_ch) 
	{
		qos_channel_release(new_ch);
	}
	if (ch_key) 
		aos_free(ch_key);	
	
	return NULL;
}

/*
 * delete channel from hashtab
 */
/*static void qos_remove_cache_channel(struct _qos_traffic_channel *c)
{
	struct chhashkey *ch_key;
	
	ch_key = (struct chhashkey*)aos_malloc_atomic(sizeof(struct chhashkey));
	if (ch_key == NULL)
		return;
	
	ch_create_hashkey(c->src_ip,
		c->dst_ip,
		c->src_port,
		c->dst_port,
		c->protocol,
		ch_key);
	
	hashtab_remove(qos_traffic_hash_gen, 
		ch_key, 
		destroy_ch_cell,
		c);
}
*/
/*
static void qos_cache_gc(void)
{
	int i = 0;
	struct _qos_traffic_channel *node;
	struct _qos_traffic_channel *tmp;
	
	printk(KERN_ERR "starting to garabage collection\n");
	local_bh_disable();
	list_for_each_entry_safe(node, tmp, &qos_traffic_list_gen, list) {
		if (i < QOS_CACHE_BUCKET_SIZE) {
			list_del(&(node->list));
			qos_remove_cache_channel(node);
			printk(KERN_ERR "index is %d\n", i); 
			i++;
		} else
			break;
	}
	local_bh_enable();
	printk(KERN_ERR "ending to garabage collection\n");
}
*/
int build_new_gen_ch(u32 src_ip,
					 u32 dst_ip,
					 u16 src_port,
					 u16 dst_port,
					 u8  proto,
					 u8  *src_mac,
					 u8  *dst_mac,
					 int   ifid,
					 u16 vlan,
					 u8  *priority,
					 int direction,
					 void  *r,
					 struct _qos_traffic_channel **cell)
{
	struct _qos_traffic_channel *new_ch; // *peer_ch;
	struct chhashkey *ch_key; 
	int ret = 0;

#ifdef __KERNEL__
	new_ch = (struct _qos_traffic_channel *)aos_malloc_atomic(sizeof(struct _qos_traffic_channel));
#else
	new_ch = (struct _qos_traffic_channel *)aos_malloc(sizeof(struct _qos_traffic_channel));
#endif

	if (new_ch == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for new channel list cell");
		return -ENOMEM;
	}
	
#ifdef __KERNEL__
	ch_key = (struct chhashkey *)aos_malloc_atomic(sizeof(struct chhashkey));
#else
	ch_key = (struct chhashkey *)aos_malloc(sizeof(struct chhashkey));
#endif

	if (ch_key == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for new hash key");
		ret = -ENOMEM;
		aos_free(new_ch);
		return ret;
	//	goto fail;
	}

	memset(new_ch,0,sizeof (struct _qos_traffic_channel));

	ret = init_channel_record(&new_ch->record);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed when initing channel record");
		goto fail;
	}	
	ret = ch_create_hashkey(src_ip,dst_ip,src_port,dst_port,proto,ch_key);
	
	/* check if garable collection */
	//if (qos_traffic_hash_gen != NULL) {
	//	if (qos_traffic_hash_gen->nel > QOS_MAX_CACHE_SIZE)
	//		printk(KERN_ERR "exceed over maximum\n");/*qos_cache_gc();*/
	//}
	
	ret = hashtab_insert(qos_traffic_hash_gen,ch_key,new_ch);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed when inserting channel list cell to hashtable");
		goto fail2;
	}

	atomic_set(&new_ch->refcnt, 1);

	new_ch->src_ip = src_ip;
	new_ch->dst_ip = dst_ip;
	new_ch->src_port = src_port;
	new_ch->dst_port = dst_port;
	new_ch->protocol = proto;
	new_ch->priority = *priority;
	new_ch->status = QOS_CHANNEL_ACTIVE;
	new_ch->ifid = ifid;
	new_ch->vlan =vlan;
	new_ch->direction = direction;
	memcpy(new_ch->src_mac, src_mac, ETH_ALEN);
	memcpy(new_ch->dst_mac, dst_mac, ETH_ALEN);
	*cell = new_ch;
	new_ch->matched_rule = r;
	//INIT_LIST_HEAD(&(new_ch->list));
	//list_add_tail(&(new_ch->list), &qos_traffic_list_gen);
	
	if (r)
	{
		if (*((u8*)r) == AOS_QOS_RULE_GEN)
			qos_general_rule_hold((struct _qos_rule_general *) r);
		else	
			qos_specific_rule_hold((struct _qos_rule_specific *) r);
	}

	/*peer_ch = build_peer_channel(new_ch);

	if (!peer_ch)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to create peer channel");
		ret = -1;
		goto fail;
	}*/

	//ros_qos_channel_log(new_ch, qos_create_channel);
	return ret;
	
fail2:
fail:
	if (new_ch != NULL) {
		qos_channel_release(new_ch);
	}
	if (ch_key) {
		aos_free(ch_key);
	}
	
	return ret;
}

void *match_in_all_rule(struct _qos_traffic_channel* cell)
{
	void *r;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  proto;
	u8  *src_mac;
	u8  *dst_mac;
	int   ifid;
	u16 vlan;
	u8  priority;
	int direction;
   	
	src_ip = cell->src_ip;
	dst_ip = cell->dst_ip;
	src_port = cell->src_port;
	dst_port = cell->dst_port;
	proto = cell->protocol;
	ifid = cell->ifid;
	src_mac = cell->src_mac;
	dst_mac = cell->dst_mac;
	vlan = cell->vlan;
	direction = cell->direction;

//	QOS_TRAFFIC_PRINTK("Rematching %p\n",cell);

	r = match_hash_gen(src_ip,dst_ip,src_port,dst_port,proto,ifid,&priority);
	if (r != NULL) {
		cell->priority = priority;
//		QOS_TRAFFIC_PRINTK("Gen rule rematched ,priority %u\n",priority);
		return r;
	}

	r = match_hash_mac(src_mac,&priority);
	if (r != NULL) {
//		QOS_TRAFFIC_PRINTK("MAC rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}
	
	r = match_hash_mac(dst_mac,&priority);
	if (r != NULL) {
//		QOS_TRAFFIC_PRINTK("MAC rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}

	r = match_hash_vlan(vlan,&priority);
	if (r != NULL) {
//		QOS_TRAFFIC_PRINTK("Vlan rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}
	r = match_hash_if(ifid,&priority);
	if (r != NULL) {
	//	QOS_TRAFFIC_PRINTK("Interface rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}
	r = match_hash_port(src_port,dst_port,&priority, ifid, direction);
	if (r != NULL) {
	//	QOS_TRAFFIC_PRINTK("Port rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}
	r = match_hash_proto(proto,&priority);
	if (r != NULL) {
	//	QOS_TRAFFIC_PRINTK("Proto rule rematched ,priority %u\n",priority);
		cell->priority = priority;
		return r;
	}
 	cell->priority = 0;
//	QOS_TRAFFIC_PRINTK("Rematch is called with no high priority\n");
//	QOS_TRAFFIC_PRINTK("This channel will be set to priority LOW\n");
	return NULL;	

}
struct _qos_traffic_channel * match_channel_hashtable(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u8 proto,struct chhashkey *ch_key)
{
	struct _qos_traffic_channel *new_ch;

	ch_create_hashkey(src_ip,dst_ip,src_port,dst_port,proto,ch_key);
	new_ch = (struct _qos_traffic_channel *)hashtab_search(qos_traffic_hash_gen,ch_key);

	return new_ch;
}

int match_update_channel_hashtable(int nf_dir, struct ros_qos_skb_info *skb_info, u8 *priority)
{
	struct _qos_traffic_channel *ch;
	struct chhashkey ch_key;
	void  *r = NULL;
	int rematch_type;

	ch = match_channel_hashtable(skb_info->src_ip, skb_info->dst_ip, skb_info->src_port, 
			                         skb_info->dst_port, skb_info->protocol, &ch_key);
	if (!ch) return -1;
	
	if (ch->status == QOS_CHANNEL_ACTIVE) {
		aos_trace("%p is been marked as inactive", ch->record.array_start);
		aos_trace("global_jiffy:%u", aos_qos_traffic_global_temp_jiffy);
		aos_trace("last_rx:%u", ch->record.last_recorded_jiffy);
	}
	aos_assert1(ch->status == QOS_CHANNEL_ACTIVE);
	qos_traffic_update(&ch->record, skb_info->len);
	*priority = ch->priority;
	//*in_ifid = ch->ifid;

	if (local_in == nf_dir)
		ros_qos_channel_stat(ch, direction_in, NF_ACCEPT, (skb_info->len>=ETH_HLEN) ? (skb_info->len-ETH_HLEN): skb_info->len);
	else
		ros_qos_channel_stat(ch, direction_out, NF_ACCEPT, (skb_info->len>=ETH_HLEN) ? (skb_info->len-ETH_HLEN): skb_info->len);
	
	if (nf_dir != post_routing) return 0;
	
	if (*priority != eROS_QOS_PRIORITY_LOW) return 0;
	
	rematch_type = 0;
	
	if (memcmp(skb_info->dst_mac, ch->dst_mac, ETH_ALEN) != 0)
	{
		rematch_type = 1; // dst mac need rematching
		r = match_hash_mac(skb_info->dst_mac, priority);
	}
	
	if (!r && skb_info->vlan != ch->vlan)
	{
		rematch_type = 2; // vlan tag need rematching
		r = match_hash_vlan(skb_info->vlan, priority);
	}

	if (r && r != ch->matched_rule)
	{
		if (*((u8*)r) == AOS_QOS_RULE_GEN)
			qos_general_rule_hold((struct _qos_rule_general *) r);
		else	
			qos_specific_rule_hold((struct _qos_rule_specific *) r);
			
		if (ch->matched_rule && (*((u8*)ch->matched_rule) == AOS_QOS_RULE_GEN))
			qos_general_rule_put((struct _qos_rule_general *) ch->matched_rule);
		else if(ch->matched_rule)	
			qos_specific_rule_put((struct _qos_rule_specific *) ch->matched_rule);

		ch->matched_rule = r;
		ch->priority = *priority;
		if (rematch_type == 1)
			memcpy(ch->dst_mac, skb_info->dst_mac, ETH_ALEN);
		else if (rematch_type == 2)
			ch->vlan = skb_info->vlan;
	}
	else
		*priority = ch->priority;
				
	return 0;
}

int match_build_channel_hashtable(
	struct sk_buff *skb,
	u32 src_ip,
	u32 dst_ip,
	u16 src_port,
	u16 dst_port,
	u8  proto,
	int   ifid,
	u8  *src_mac,
	u8  *dst_mac,
	u16 vlantag,
	u16 len,
	u8  *priority,
	u32 *band_req,
	int direction)
{
	int ret;
	void *r;
	struct _qos_traffic_channel *new_ch;
	struct chhashkey ch_key;

	ret = 0;
	*band_req = 0;

	new_ch = match_channel_hashtable(src_ip,dst_ip,src_port,dst_port,proto,&ch_key);

	if (new_ch != NULL) {
			*priority = new_ch->priority;
		if (new_ch->status != QOS_CHANNEL_ACTIVE) {
			ret = rebuild_channel_record(&new_ch->record);
			if (ret != 0) {
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to re build channel record");
				return ret;
			}
			new_ch->status = QOS_CHANNEL_ACTIVE;
			*band_req = new_ch->band_required;
			//ros_qos_channel_log(new_ch, qos_active_channel);
		}
	} else {

		r = NULL;
		
		// if (gen_rul_size > 0) TBD
		r = match_hash_gen(src_ip,dst_ip,src_port,dst_port,proto,ifid,priority);
		if (r) {
			struct _qos_rule_general *genrule;
			genrule = (struct _qos_rule_general*)r;
			*band_req = genrule->band_required;
			goto channel_build;
		}
		
		// if (mac_rul_size > 0) TBD
		r = match_hash_mac(src_mac,priority);
		if (r) goto channel_build;

		// if (vlan_rul_size > 0) TBD
		r = match_hash_vlan(vlantag,priority);
		if (r) goto channel_build;

		// if (if_rul_size > 0) TBD
		r = match_hash_if(ifid,priority);
		if (r) goto channel_build;
	
		// if (port_rul_size > 0) TBD
		r = match_hash_port(src_port,dst_port,priority, ifid, direction);
		if (r) goto channel_build;

		// if (proto_rul_size > 0) TBD
		r = match_hash_proto(proto,priority);
		if (r) goto channel_build;

		*priority = 0;
channel_build:
		ret = build_new_gen_ch(src_ip, dst_ip, src_port, dst_port, proto,
							   src_mac, dst_mac, ifid, vlantag, priority, direction, r,  &new_ch);
		if (ret) {
			*band_req = 0;
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to re build channel record");
			return ret;
		}
		new_ch->band_required = *band_req;
	
		if (r) ros_qos_rule_log(r, new_ch);
	
		ros_qos_channel_stat(new_ch, direction, NF_ACCEPT, len);	
	}
	
	aos_qos_channel_put(skb->aos_qos_ch);
	aos_qos_channel_hold(new_ch);
	skb->aos_qos_ch = new_ch;
	
	
	aos_assert1(new_ch->status == QOS_CHANNEL_ACTIVE);
	return ret; 
}

int build_channel_hashtable(
	u32 src_ip,
	u32 dst_ip,
	u16 src_port,
	u16 dst_port,
	u8  proto,
	int   ifid,
	u8  *src_mac,
	u8  *dst_mac,
	u16 vlantag,
	u16 len,
	u8  *priority,
	u32 *band_req,
	int direction)
{
	int ret;
	void *r;
	struct _qos_traffic_channel *new_ch;
	struct chhashkey ch_key;

	ret = 0;
	*band_req = 0;

	new_ch = match_channel_hashtable(src_ip,dst_ip,src_port,dst_port,proto,&ch_key);

	if (new_ch != NULL) {
			*priority = new_ch->priority;
		if (new_ch->status != QOS_CHANNEL_ACTIVE) {
			ret = rebuild_channel_record(&new_ch->record);
			if (ret != 0) {
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to re build channel record");
				printk(KERN_ERR "bill:fail to build channel record");
				return ret;
			}
			new_ch->status = QOS_CHANNEL_ACTIVE;
			*band_req = new_ch->band_required;
			//ros_qos_channel_log(new_ch, qos_active_channel);
		}
	} else {

		r = NULL;
		
		// if (gen_rul_size > 0) TBD
		r = match_hash_gen(src_ip,dst_ip,src_port,dst_port,proto,ifid,priority);
		if (r) {
			struct _qos_rule_general *genrule;
			genrule = (struct _qos_rule_general*)r;
			*band_req = genrule->band_required;
			goto channel_build;
		}
		
		// if (mac_rul_size > 0) TBD
		r = match_hash_mac(src_mac,priority);
		if (r) goto channel_build;

		// if (vlan_rul_size > 0) TBD
		r = match_hash_vlan(vlantag,priority);
		if (r) goto channel_build;

		// if (if_rul_size > 0) TBD
		r = match_hash_if(ifid,priority);
		if (r) goto channel_build;
	
		// if (port_rul_size > 0) TBD
		r = match_hash_port(src_port,dst_port,priority, ifid, direction);
		if (r) goto channel_build;

		// if (proto_rul_size > 0) TBD
		r = match_hash_proto(proto,priority);
		if (r) goto channel_build;

		*priority = 0;
channel_build:
		ret = build_new_gen_ch(src_ip, dst_ip, src_port, dst_port, proto,
							   src_mac, dst_mac, ifid, vlantag, priority, direction, r,  &new_ch);
		if (ret) {
			*band_req = 0;
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to re build channel record");
			printk(KERN_ERR "bill:fail to build channel record");
			return ret;
		}
		new_ch->band_required = *band_req;
	
		if (r) ros_qos_rule_log(r, new_ch);
	
		ros_qos_channel_stat(new_ch, direction_in, NF_ACCEPT, len);	
	}
	
	aos_qos_channel_hold(new_ch);
	
	
	aos_assert1(new_ch->status == QOS_CHANNEL_ACTIVE);
	return ret; 
}

void *match_hash_gen(u32 src_ip,
					 u32 dst_ip,
					 u16 src_port,
					 u16 dst_port,
					 u8  proto,
					 int ifid,
					 u8 *priority)
{
	struct _qos_rule_general rule;
//	struct _qos_rule_key key;
	void *ret;
	struct arg_match_gen_rule args;
	int match_result;
//	int i;

	args.src_ip = src_ip;
	args.dst_ip = dst_ip;
	args.src_port = src_port;
	args.dst_port = dst_port;
	args.proto = proto;
	args.ifid = ifid;
//	rule.type = AOS_QOS_RULE_GEN; 
	//1111
//	rule.src_ip = src_ip;
//	rule.dst_ip = dst_ip;
//	rule.src_port = src_port;
//	rule.dst_port = dst_port;
//	rule.proto = proto;
//	rule.devid = ifid;

	weight_order = 0;
	
	match_result = hashtab_map(aos_qos_rule_gen_hash,map_match_gen_rule,(void *)&args);
	if (match_result != 0) {
		ret = args.r;
		*priority = args.priority;

		memset(&rule,0,sizeof(struct _qos_rule_general));
		memcpy(&rule,args.r,sizeof(struct _qos_rule_general));
/*		QOS_TRAFFIC_PRINTK("Rule Matched!\n");
		QOS_TRAFFIC_PRINTK("=======Rule parameter=====================\n");
		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(rule.src_ip));
		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(rule.dst_ip));
		QOS_TRAFFIC_PRINTK("src_port:%u\n",rule.src_port);
		QOS_TRAFFIC_PRINTK("dst_port:%u\n",rule.dst_port);
		QOS_TRAFFIC_PRINTK("proto:%u\n",rule.proto);
		QOS_TRAFFIC_PRINTK("devid:%u\n",rule.devid);
		QOS_TRAFFIC_PRINTK("priority:%u\n",rule.priority);
		QOS_TRAFFIC_PRINTK("=======Stream parameter=====================\n");
		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(src_ip));
		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(dst_ip));
		QOS_TRAFFIC_PRINTK("src_port:%u\n",src_port);
		QOS_TRAFFIC_PRINTK("dst_port:%u\n",dst_port);
		QOS_TRAFFIC_PRINTK("proto:%u\n",proto);
		QOS_TRAFFIC_PRINTK("devid:%u\n",ifid);
		QOS_TRAFFIC_PRINTK("priority:%u\n",*priority);*/
	} else {
		ret = NULL;
		*priority = AOS_QOS_RULE_PRIORITY_LOW;
		if (dbg_print_count < 1) {
//		QOS_TRAFFIC_PRINTK("Rule DisMatched!\n");
//		QOS_TRAFFIC_PRINTK("=======Rule parameter=====================\n");
//		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(rule.src_ip));
//		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(rule.dst_ip));
//		QOS_TRAFFIC_PRINTK("src_port:%u\n",rule.src_port);
//		QOS_TRAFFIC_PRINTK("dst_port:%u\n",rule.dst_port);
//		QOS_TRAFFIC_PRINTK("proto:%u\n",rule.proto);
//		QOS_TRAFFIC_PRINTK("devid:%u\n",rule.devid);
//		QOS_TRAFFIC_PRINTK("priority:%u\n",rule.priority);
/*		QOS_TRAFFIC_PRINTK("=======Stream parameter=====================\n");
		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(src_ip));
		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(dst_ip));
		QOS_TRAFFIC_PRINTK("src_port:%u\n",src_port);
		QOS_TRAFFIC_PRINTK("dst_port:%u\n",dst_port);
		QOS_TRAFFIC_PRINTK("proto:%u\n",proto);
		QOS_TRAFFIC_PRINTK("devid:%u\n",ifid);
		QOS_TRAFFIC_PRINTK("priority:%u\n",*priority);*/
		}
	}
	if (ret != NULL) {
		*priority = ((struct _qos_rule_general *)ret)->priority;	
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}
	return ret;
}
int map_match_gen_rule(void *k, void *d,void *args)
{
	struct arg_match_gen_rule *local_arg;
	struct _qos_rule_general *rule;

	local_arg = (struct arg_match_gen_rule *)args;
	rule = (struct _qos_rule_general *)d;	

	aos_assert1(d != NULL);
	if ((rule->src_ip != local_arg->src_ip) && (rule->src_ip != WILDCARD_IP)) {
		return 0;
	}
	if ((rule->dst_ip != local_arg->dst_ip) && (rule->dst_ip != WILDCARD_IP)) {
		return 0;
	}
	if ((rule->src_port != local_arg->src_port) && (rule->src_port != WILDCARD_PORT)) {
		return 0;
	}
	if ((rule->dst_port != local_arg->dst_port) && (rule->dst_port != WILDCARD_PORT)) {
		return 0;
	}
	if ((rule->proto != local_arg->proto)) {
		return 0;
	}
	
	// if ((rule->devid != local_arg->ifid)) {
	//	return 0;
	// }
	if (rule->weight == 1) {
    	local_arg->r = d;
    	local_arg->priority = rule->priority;
    	return 1;
	}
	
	if (weight_order == 0) {
		weight_order = rule->weight;
		local_arg->r = d;
    	local_arg->priority = rule->priority;
	}
		
	if (weight_order > rule->weight) {
		weight_order = rule->weight;
		local_arg->r = d;
    	local_arg->priority = rule->priority;
	}
	
    return 0;
}

void *match_hash_mac(u8 *mac,
					 u8 *priority)
{
	void *ret;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_MAC;
	rule.parms.macdata.mac[0]=mac[0];
	rule.parms.macdata.mac[1]=mac[1];
	rule.parms.macdata.mac[2]=mac[2];
	rule.parms.macdata.mac[3]=mac[3];
	rule.parms.macdata.mac[4]=mac[4];
	rule.parms.macdata.mac[5]=mac[5];
	create_hash_key(&rule,&key);
	ret = hashtab_search(aos_qos_rule_spc_hash,&key);
	if (ret != NULL) {
		*priority = ((struct _qos_rule_specific *)ret)->priority;	
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}
	return ret;
}
void *match_hash_vlan(u16 vlantag,
					  u8 *priority)
{
	void *ret;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_VLAN;
	rule.parms.vlandata.tag = vlantag;
	create_hash_key(&rule,&key);
	ret = hashtab_search(aos_qos_rule_spc_hash,&key);
	if (ret != NULL) {
		*priority = ((struct _qos_rule_specific *)ret)->priority;	
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}
	return ret;
}

void *match_hash_if(u8 ifid,
					u8 *priority)
{
	void *ret;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_IF;
	rule.parms.ifdata.ifid = ifid;
	create_hash_key(&rule,&key);
	ret = hashtab_search(aos_qos_rule_spc_hash,&key);
	if (ret != NULL) {
		*priority = ((struct _qos_rule_specific *)ret)->priority;	
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}
	return ret;
}
void *match_hash_port(u16 src_port,
					  u16 dst_port,
					  u8 *priority,
					  int ifid,
					  int direction)
{
	void *ret;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;

	rule.type = AOS_QOS_RULE_PORT;
	if (direction == direction_in)
		rule.parms.portdata.port = dst_port;
	else
		rule.parms.portdata.port = src_port;
	
	//rule.parms.portdata.sending = AOS_QOS_RULE_DIRECTION_SENDING;
	rule.parms.portdata.ifid = ifid;
	create_hash_key(&rule,&key);
	ret = hashtab_search(aos_qos_rule_spc_hash,&key);
	if (ret != NULL) {
		*priority = ((struct _qos_rule_specific *)ret)->priority;	
		goto out;
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}

out:
	return ret;
}

void *match_hash_proto(u8 proto,
					   u8 *priority)
{
	void *ret;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;

	rule.type = AOS_QOS_RULE_PROTO;
	rule.parms.protodata.proto = proto;
	create_hash_key(&rule,&key);
	ret = hashtab_search(aos_qos_rule_spc_hash,&key);
	if (ret != NULL) {
		*priority = ((struct _qos_rule_specific *)ret)->priority;	
	} else {
		*priority = AOS_QOS_RULE_PRIORITY_LOW; 
	}
	return ret;

}


struct qos_bw_ctrl_data *match_priority_hash(int ifid,u8 direction,u8 priority,struct prihashkey *k)
{
	struct qos_bw_ctrl_data *cell;
	
	pri_create_hashkey(ifid,direction,priority,k);
	cell = (struct qos_bw_ctrl_data *)hashtab_search(qos_traffic_hash_pri,k);

	return cell;

}

int match_update_priority_hash(int ifid,
						 u8 direction,
						 u8 priority,
						 u16 len)
{
	struct prihashkey k;
	int ret;
	struct qos_bw_ctrl_data *cell;
	int i;

	aos_trace("enter %s", __FUNCTION__);
	ret = 0;
	cell = match_priority_hash(ifid,direction,priority,&k);

	if (cell != NULL) {
		for (i = 0; i< eROS_QOS_PRIORITY_MAX; ++i) {
			if (i == priority) {
				qos_traffic_update(&cell->record[i],len);
			} else {
				qos_traffic_update(&cell->record[i],0);
			}
			aos_trace("PINFO:traffic bandwidth %u priority %d.",cell->record[i].total_band,i);
		}
	}else {
		aos_trace("Not found priority channel");
	}
	return 0;
}

int match_build_priority_hash(int ifid, u8 direction, u8 priority, u16 len, u32 band_req)
{
	struct qos_bw_ctrl_data *cell;
	struct prihashkey k;
	int ret;

	ret = 0;
	// aos_trace("enter %s", __FUNCTION__);
	cell = match_priority_hash(ifid, direction, priority, &k);

	if (cell== NULL) {
		ret = build_pri_ch(ifid, direction, &cell, &k);
		if (ret != 0) {
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to build priority cell");
			return ret;
		}
	}

	cell->req_bw[priority] += band_req;
	return ret;
}

int build_pri_ch(int ifid,u8 direction,struct qos_bw_ctrl_data **r,struct prihashkey *k)
{
	struct qos_bw_ctrl_data *cell;
	struct prihashkey *nw_key; 
	int ret = 0;
	int i;
	
#ifdef __KERNEL__
	nw_key = (struct prihashkey *)aos_malloc_atomic(sizeof(struct prihashkey));
#else
	nw_key = (struct prihashkey *)aos_malloc(sizeof(struct prihashkey));
#endif

	if (nw_key == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to rebuild priority hash key");
		return -ENOMEM;
	}

	memcpy(nw_key,k,sizeof (struct prihashkey));

	cell = qos_bw_ctrl_data_create(ifid, direction);
	
	if (cell == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to rebuild priority cell");
		ret = -ENOMEM;
		goto fail;
	}

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; ++i) {
		ret = init_channel_record(&cell->record[i]);
		if (ret) goto fail;
	}

	ret = hashtab_insert(qos_traffic_hash_pri,nw_key,cell);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to insert priority hash table");
		goto fail2;
	}

	*r = cell;
	return ret;

fail2:
fail:
	if (nw_key) {
		aos_free(nw_key);
	}
	
	qos_bw_ctrl_data_put(cell);
	return ret;
}

int build_eth_ch(int ifid,u8 direction,struct _qos_traffic_channel_record **r,struct ethhashkey *k)
{
	struct _qos_traffic_channel_record *record;
	struct ethhashkey *nw_key; 
	int ret = 0;

#ifdef __KERNEL__
	nw_key = (struct ethhashkey *)aos_malloc_atomic(sizeof(struct ethhashkey));
#else
	nw_key = (struct ethhashkey *)aos_malloc(sizeof(struct ethhashkey));
#endif

	if (nw_key == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for eth hash key");
		return -ENOMEM;
	}
	memcpy(nw_key,k,sizeof (struct ethhashkey));
	
#ifdef __KERNEL__
	record = (struct _qos_traffic_channel_record *)aos_malloc_atomic(sizeof(struct _qos_traffic_channel_record));
#else
	record = (struct _qos_traffic_channel_record *)aos_malloc(sizeof(struct _qos_traffic_channel_record));
#endif
	
	if (record == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for eth channel record");
		ret = -ENOMEM;
		goto fail;
	}
	ret = init_channel_record(record);
	
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to init channel record");
		goto fail;
	}

	ret = hashtab_insert(qos_traffic_hash_eth,nw_key,record);
	if (ret != 0) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to insert eth hashtable");
		goto fail2;
	}
	*r = record;
	return ret;
	
fail2:
fail:
	if (nw_key) {
		aos_free(nw_key);
	}

	if (record != NULL) {
		destroy_record(NULL, record, NULL);
	}

	return ret;
}


int match_build_ifhash(int ifid,
				  u16 len,
				  u8 direction)
{
	int ret = 0;
	struct _qos_traffic_channel_record *record;
	struct ethhashkey k;

	record = match_ifhash(ifid,direction,&k);
	if (record == NULL) {
		ret = build_eth_ch(ifid,direction,&record,&k);
		if (ret != 0) {
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate eth channel ");
			goto fail;
		}
	}
	//qos_traffic_update(record,len);
	++dbg_print_count;
	if ((dbg_print_count % 64) == 0) {
		QOS_TRAFFIC_PRINTK("EINFO:Updated eth%d, direction %u with vaule %u\n",ifid,direction,record->total_band);
	}
	return ret;
	
fail:
	return ret;
}

int match_update_ifhash(int ifid, u16 len, u8 direction)
{
	int ret = 0;
	struct _qos_traffic_channel_record *record;
	struct ethhashkey k;

	record = match_ifhash(ifid,direction,&k);
	if (record == NULL) {
		ret = build_eth_ch(ifid,direction,&record,&k);
		if (ret != 0) {
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate eth channel ");
			goto fail;
		}
	}
	qos_traffic_update(record,len);
	++dbg_print_count;
	if ((dbg_print_count % 64) == 0) {
		QOS_TRAFFIC_PRINTK("EINFO:Updated eth%d, direction %u with vaule %u\n",ifid,direction,record->total_band);
	}
	return ret;
	
fail:
	return ret;
}

struct _qos_traffic_channel_record *match_ifhash(int ifid,u8 direction,struct ethhashkey *k)
{
	struct _qos_traffic_channel_record *record;

	eth_create_hashkey(ifid,direction,k);

	record = (struct _qos_traffic_channel_record *)hashtab_search(qos_traffic_hash_eth,k);

	return record;
}

int qos_retrieve_info(struct sk_buff *skb, struct ros_qos_skb_info *skb_info, int direction)
{
	struct _qos_traffic_channel *ch;
	struct ethhdr *mac_head;
	struct iphdr  *ip_head;
	struct udphdr *udp_head;

	memset(skb_info, 0, sizeof(struct ros_qos_skb_info));

	if (skb->aos_qos_ch == NULL)  
	{
		ip_head = skb->nh.iph;
		udp_head = (struct udphdr *)((char *)ip_head + (ip_head->ihl << 2));
	
		if (ip_head->protocol != IPPROTO_TCP && ip_head->protocol != IPPROTO_UDP) {
			return -1;
		}
		
		
        skb_info->mac_proto = skb->protocol;	
        skb_info->src_ip = ip_head->saddr;
        skb_info->dst_ip = ip_head->daddr;
        skb_info->src_port = udp_head->source;
        skb_info->dst_port = udp_head->dest;
        skb_info->protocol = ip_head->protocol;
        skb_info->len = skb->len;
        memcpy(skb_info->dev_name, skb->dev->name, IFNAMSIZ);
        skb_info->ifid = skb->dev->ifindex;
        skb_info->ch = NULL;
        
        return 0;
	}

	ch = skb->aos_qos_ch;
	
	skb_info->mac_proto = skb->protocol;	
	skb_info->src_ip = ch->src_ip;
	skb_info->dst_ip = ch->dst_ip;
	skb_info->src_port = ch->src_port;
	skb_info->dst_port = ch->dst_port;
	skb_info->protocol = ch->protocol;
	memcpy(skb_info->src_mac, ch->src_mac, ETH_ALEN); 
		
	skb_info->len = skb->len;
	memcpy(skb_info->dev_name, skb->dev->name, IFNAMSIZ);
	skb_info->ifid = skb->dev->ifindex;
	
	if (direction == 0) {

		mac_head = eth_hdr(skb);
		if (mac_head == NULL) {
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Some thing happend");
			return -1;
		}
		if (mac_head->h_proto != 0x0008) {
			aos_trace("Non IP packet found with proto %u",mac_head->h_proto);
			// aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Non IP packet found with proto %u",mac_head->h_proto);
			return -1;
		}

		memcpy(skb_info->dst_mac, ch->dst_mac, ETH_ALEN);
		skb_info->vlan = ch->vlan;
	
	} else {
		mac_head = eth_hdr(skb);
		if (mac_head == NULL) {
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Some thing happend");
			return -1;
		}

		//mac_head = (struct ethhdr *)(skb->data);
		if (mac_head->h_proto != 0x0008) {
			aos_trace("Non IP packet found with proto %u",mac_head->h_proto);
			// aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Non IP packet found with proto %u",mac_head->h_proto);
			return -1;
		}
		memcpy(skb_info->dst_mac, mac_head->h_dest, ETH_ALEN);
		skb_info->vlan = skb->vid;
	}
	

	skb_info->ch = ch;
	aos_qos_channel_hold(ch);

	//if (aos_qos_channel_put(skb->aos_qos_ch) == 0) skb->aos_qos_ch = NULL;

	return 0;
}
	
int qos_traffic_parse_parm(struct sk_buff *skb, struct ros_qos_skb_info *skb_info, int direction)
{
	int ret = 0;
	struct ethhdr *mac_head;
	struct iphdr  *ip_head;
	struct udphdr *udp_head;

	memset(skb_info, 0, sizeof(struct ros_qos_skb_info));

	skb_info->protocol =  skb->protocol;
	if (skb->protocol != 0x0008)
	{
		aos_trace("%s Non IP packet found with proto %u", __FUNCTION__, skb->protocol);
		return -1;
	}

	if(direction != direction_in) 
	{
		aos_trace("%s direction is not in", __FUNCTION__);
		return -1;
	}
	
	if (direction == 0) {

		mac_head = eth_hdr(skb);
		if (mac_head == NULL) {
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Some thing happend");
			return -1;
		}
		if (mac_head->h_proto != 0x0008) {
			aos_trace("Non IP packet found with proto %u",mac_head->h_proto);
			// aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Non IP packet found with proto %u",mac_head->h_proto);
			return -1;
		}

		memcpy(skb_info->src_mac, mac_head->h_source, ETH_ALEN);
		memcpy(skb_info->dst_mac, mac_head->h_dest, ETH_ALEN);
		ip_head = (struct iphdr *)skb->data;
	
	} else {
		/*mac_head = eth_hdr(skb);
		if (mac_head == NULL) {
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Some thing happend");
			return -1;
		}

		//mac_head = (struct ethhdr *)(skb->data);
		if (mac_head->h_proto != 0x0008) {
			aos_trace("Non IP packet found with proto %u",mac_head->h_proto);
			// aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Non IP packet found with proto %u",mac_head->h_proto);
			return -1;
		}
		memcpy(skb_info->src_mac, mac_head->h_source, ETH_ALEN);
		memcpy(skb_info->dst_mac, mac_head->h_dest, ETH_ALEN);
		ip_head = (struct iphdr *)((char*)mac_head + ETH_HLEN);*/
		ip_head = (struct iphdr *)(skb->data);
	}

	skb_info->src_ip = ip_head->saddr;
	skb_info->dst_ip = ip_head->daddr;
	skb_info->protocol =  ip_head->protocol;
	skb_info->mac_proto = skb->protocol;
		
	udp_head = (struct udphdr *)((char *)ip_head + (ip_head->ihl << 2));
	skb_info->src_port = udp_head->source;
	skb_info->dst_port = udp_head->dest;
	skb_info->len = skb->len;

	memcpy(skb_info->dev_name, skb->dev->name,IFNAMSIZ);
	skb_info->ifid = skb->dev->ifindex;
	skb_info->vlan = qos_traffic_get_vlan_tag(skb);
	
/*	if (dbg_print_count % 60 == 0) {
		QOS_TRAFFIC_PRINTK("========Input parameter:=======\n");
		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",*(u8 *)(skb->data+12),*(u8 *)(skb->data+13),*(u8 *)(skb->data+14),*(u8 *)(skb->data+15));
		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",*(u8 *)(skb->data+16),*(u8 *)(skb->data+17),*(u8 *)(skb->data+18),*(u8 *)(skb->data+19));
		QOS_TRAFFIC_PRINTK("src_port:%u.%u\n",*(u8 *)(skb->data+20),*(u8 *)(skb->data+21));
		QOS_TRAFFIC_PRINTK("src_port:%u\n",*(u16 *)(skb->data+20));
		QOS_TRAFFIC_PRINTK("dst_port:%u.%u\n",*(u8 *)(skb->data+22),*(u8 *)(skb->data+23));
		QOS_TRAFFIC_PRINTK("dst_port:%u\n",*(u16 *)(skb->data+22));
		QOS_TRAFFIC_PRINTK("proto:%u\n",*(u8 *)(skb->data+9));
		QOS_TRAFFIC_PRINTK("dst_mac:%X:%X:%X:%X:%X:%X\n",mac_head->h_dest[0],mac_head->h_dest[1],mac_head->h_dest[2],mac_head->h_dest[3],mac_head->h_dest[4],mac_head->h_dest[5]);
		QOS_TRAFFIC_PRINTK("src_mac:%X:%X:%X:%X:%X:%X\n",mac_head->h_source[0],mac_head->h_source[1],mac_head->h_source[2],mac_head->h_source[3],mac_head->h_source[4],mac_head->h_source[5]);
		QOS_TRAFFIC_PRINTK("MAC TYPE:%u\n",mac_head->h_proto);

		QOS_TRAFFIC_PRINTK("========Parsed parameter:=======\n");
		QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(*src_ip));
		QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(*dst_ip));
		QOS_TRAFFIC_PRINTK("src_port:%u\n",*src_port);
		QOS_TRAFFIC_PRINTK("dst_port:%u\n",*dst_port);
		QOS_TRAFFIC_PRINTK("proto:%u\n",*proto);
		QOS_TRAFFIC_PRINTK("mac:%X:%X:%X:%X:%X:%X\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
		QOS_TRAFFIC_PRINTK("vlantag:%d\n",*vlantag);
		++dbg_print_count;
	}*/
	return ret;
}


////////////////////////////////////////////////////////////////////////////////
//Description:  Cli function to turn on or turn off the facility
//Input Value:  The status to be set.
//Return value: 0  success
//				-EINVAL Invalid variables
//				-ENOMEM Allocating memory failed when initiazing the resources 
//Comment:      local_bh_disable() will not be considerated.
//
// cli api
int qos_traffic_set_status(int s)
{
	int ret = 0;

	switch (s) {
		case AOS_QOS_TRAFFIC_STATUS_OFF:
			if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_ON) {
				aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_OFF;
#ifdef __KERNEL__
			//	local_bh_disable();
			//	spin_lock(&aos_qos_traffic_global_spinlock);
				free_all_resources();
			//	local_bh_enable();
			//	spin_unlock(&aos_qos_traffic_global_spinlock);
#endif
			}else if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_STOP) {
#ifdef __KERNEL__
			//	local_bh_disable();
			//	spin_lock(&aos_qos_traffic_global_spinlock);
				free_all_resources();
			//	local_bh_enable();
			//	spin_unlock(&aos_qos_traffic_global_spinlock);
#endif
			}
			break;
		case AOS_QOS_TRAFFIC_STATUS_STOP:
			if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_ON) {
				aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_STOP;

			} else if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Facility is OFF for the moment");
			//	QOS_TRAFFIC_PRINTK("WARNING:The facility is off for the moment\n");
				ret = -EINVAL;
				goto fail;
			}
			break;
		case AOS_QOS_TRAFFIC_STATUS_ON:
			if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_OFF) {
				ret = init_all_resources();
				if (ret == 0) {
					aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_ON;
				} else {
					goto fail;
				} 
			}else if (aos_qos_traffic_facility_status == AOS_QOS_TRAFFIC_STATUS_STOP) {
				aos_qos_traffic_facility_status = AOS_QOS_TRAFFIC_STATUS_ON;
				}
			break;
		case AOS_QOS_TRAFFIC_STATUS_READY:
			ret = aos_qos_traffic_init_variables();
			aos_assert1((ret == 0));
			break;
		default:
			ret = -EINVAL;
			aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Invalid parm");
			goto fail;
			break;
	}//end switch case
	return ret;
fail:
	switch (ret) {
		case -EINVAL: 
			QOS_TRAFFIC_PRINTK("WARNING:unfit parameter is set\n");
			break;
		case -ENOMEM:
			QOS_TRAFFIC_PRINTK("Not enough memory to be allocated\n");
			break;
		default:
			QOS_TRAFFIC_PRINTK("ALARMING:unexpected return value when running\n");
	}
	return ret;
}


///////////////////////////////////////////////////////////////////////////////
//
//Description:
//		Callback function for Traffic monitor
//Function:
//		To Clear the expired history record
//
//
int clear_list_history_record(void *k, void *d,void *args)
{

	struct _qos_traffic_channel *cell;
	
	// aos_trace("enter %s", __FUNCTION__);
	
	cell = (struct _qos_traffic_channel *)d;
	clear_expired_history_record(&(cell->record));

	return 0;

}
int clear_pri_history_record (void *k, void *d,void *args)
{
	int i;
	struct qos_bw_ctrl_data * cell;
	
	 //aos_trace("enter %s", __FUNCTION__);
	
	cell = (struct qos_bw_ctrl_data *)d;
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; ++i) {
		clear_history_record(k,&cell->record[i],args);
	}
	return 0;

}
int clear_history_record(void *k, void *d,void *args)
{
	//aos_trace("enter %s", __FUNCTION__);
	clear_expired_history_record((struct _qos_traffic_channel_record *)d);
	//aos_trace("cleared");

	return 0;

}


//////////////////////////////////////////////////////
//
//Channel Hash key functions
//Create Hash Key
//


int ch_create_hashkey(u32 src_ip,
					  u32 dst_ip,
					  u16 src_port,
					  u16 dst_port,
					  u8  proto,
					  struct chhashkey *k)
{
	u8 *pos;

	memset(k->k,0,QOS_TRAFFIC_CH_HASH_KEY_LEN);
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


//////////////////////////////////////////////////////
//
//Description:
//		Product hash index from hashkey
//		It is used for channel hashtable

u32 ch_hash_val_cal(struct hashtab *h,void *k)
{
	u32 value = 0;
	u8 *pos;
	struct chhashkey *key;
	int i;

	key = (struct chhashkey *)k;
	pos = key->k;
	value += (*pos << 1);
	++pos;
	for (i = 1; i < QOS_TRAFFIC_CH_HASH_KEY_LEN; ++i) {
		value += key->k[i];
	}
	value &= ((2 << QOS_TRAFFIC_CH_HASH_SIZE) - 1);
	return value;
}


///////////////////////////////////////////////////////
//
//Description:
//		Callback function for comarision two hash keys
//		It is used only for channel hashtable


int ch_hash_cmp(struct hashtab *h,void *key1,void *key2)
{
	return memcmp(key1,key2,QOS_TRAFFIC_CH_HASH_KEY_LEN);

}

////////////////////////////////////////////////////
//
//Description:
//		To Determine if the channel_record is to be
//		removed
//		If need return 1. Hashtab will remove it
//		If need not, return 0

int if_remove_channel (void *k, void *d, void *args)
{
	struct _qos_traffic_channel *cell;
	int ret;
	ret = 0;

	// aos_trace("enter %s", __FUNCTION__);
	if (d == NULL)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "%s channel should not be null here", __FUNCTION__);
		return 0;
	}
	
	cell = (struct _qos_traffic_channel *)d;
	
	clear_expired_history_record(&cell->record);                             


	if (cell->status != QOS_CHANNEL_INACTIVE) {
		// if (jiffies - cell->record.last_recorded_jiffy > 3 * HZ) {
//		if (QOS_GET_JIFFIES() - cell->record.last_recorded_jiffy > 3 * HZ) {
		if (aos_qos_traffic_global_remove_jiffy - cell->record.last_recorded_jiffy > 3 * HZ){
			aos_trace("%p is to be marked as inactive",cell->record.array_start);
			aos_trace("remove_jiffy:%u",aos_qos_traffic_global_remove_jiffy);
			aos_trace("last_rx:%u",cell->record.last_recorded_jiffy);
//			goto test;                            ///TEST
			if (cell->record.array_start)			
			{
				aos_free(cell->record.array_start);
				cell->record.array_start = NULL;
			}
			cell->status = QOS_CHANNEL_INACTIVE; 
			//ros_qos_channel_log(cell, qos_deactive_channel);
		}
	}
	if (list_empty(&(cell->record.channel_history_queue_head))) {
		if (cell->status == QOS_CHANNEL_INACTIVE) {
			aos_assert1(cell->record.array_start == NULL);
			QOS_TRAFFIC_PRINTK("Channel_list_cell %p will be destroyed",cell);
			ret = 1;
			//ros_qos_channel_log(cell, qos_remove_channel);
		}
	}
	//aos_trace("out the function %s",__FUNCTION__);
	return ret;
}

///////////////////////////////////////////////////
//
//Description:
//		Hash call back function
//		It is used only for channel hash table
//		

void destroy_ch_cell(void *k,void *d,void *args)
{
	struct _qos_traffic_channel *cell = (struct _qos_traffic_channel *)d;

	aos_assert0(cell);
	
	if (k) aos_free(k);
	
	aos_qos_channel_put(cell);

	return;
}

int pri_create_hashkey(int ifid,
					   u8 direction,
					   u8 priority,
					   struct prihashkey *k)
{
	u8 *pos;
	memset(k->k,0,QOS_TRAFFIC_PRI_HASH_KEY_LEN);
	pos = k->k;
	memcpy(pos,&ifid,sizeof(int));
	pos += sizeof(int);
	memcpy(pos,&direction,sizeof(u8));
	pos += sizeof(u8);
	return 0;

}
u32 pri_hash_val_cal(struct hashtab *h,void *k)
{
	u32 value = 0;
	u8 *pos;
	struct prihashkey *key;
	int i;

	key = (struct prihashkey*)k;
	pos = key->k;
	value += (*pos << 1);
	++pos;
	for (i = 1; i < QOS_TRAFFIC_PRI_HASH_KEY_LEN; ++i) {
		value += key->k[i];
	}
	value &= ((2 << QOS_TRAFFIC_PRI_HASH_SIZE) - 1);
	return value;

}

int pri_hash_cmp(struct hashtab *h,void *key1,void *key2)
{
	return memcmp(key1,key2,QOS_TRAFFIC_PRI_HASH_KEY_LEN);
}
//////////////////////////////////////////////////////////////////////
//Eth Hash Functions
//

int eth_create_hashkey(int ifid,
					   u8 direction,
					   struct ethhashkey *k)
{
	u8 *pos;
	memset(k->k,0,QOS_TRAFFIC_ETH_HASH_KEY_LEN);
	pos = k->k;
	memcpy(pos,&ifid,sizeof(int));
	pos += sizeof(int);
	memcpy(pos,&direction,sizeof(u8));
	pos += sizeof(u8);
	return 0;
}
u32 eth_hash_val_cal(struct hashtab *h,void *k)
{
	u32 value = 0;
	u8 *pos;
	struct ethhashkey *key;
	int i;

	key = (struct ethhashkey*)k;
	pos = key->k;
	value += (*pos << 1);
	++pos;
	for (i = 1; i < QOS_TRAFFIC_ETH_HASH_KEY_LEN; ++i) {
		value += key->k[i];
	}
	value &= ((2 << QOS_TRAFFIC_ETH_HASH_SIZE) - 1);
	return value;

}
int eth_hash_cmp(struct hashtab *h,void *key1,void *key2)
{
	return memcmp(key1,key2,QOS_TRAFFIC_ETH_HASH_KEY_LEN);
}

////////////////////////////////////////////////////////
//The following is the functions of channel_history_cell
//
//

int init_qos_traffic_history_cell(struct _qos_traffic_history_cell *history_cell)
{
	int ret = 0;
	
	aos_assert1(history_cell != NULL);
	
	INIT_LIST_HEAD(&(history_cell->cell_head));

	return ret;
}

void free_qos_traffic_history_cell(struct _qos_traffic_history_cell *history_cell)
{
	aos_assert0(history_cell != NULL);
	
	aos_free(history_cell);

	return;
}	

void clear_all_history_record(struct _qos_traffic_channel_record *record) 
{
	//struct _qos_traffic_history_cell *local_cell;
	struct _qos_traffic_history_cell *node;
	struct _qos_traffic_history_cell *tmp;
	//struct aos_list_head *local_head;
	if (record == NULL)
		return;
	aos_assert0(record != NULL);
	/*
	local_head = record->channel_history_queue_head.next;
	while (local_head != &(record->channel_history_queue_head)) {
	//	aos_trace("here");
		local_cell = container_of(local_head,struct _qos_traffic_history_cell, cell_head);
		local_cell->cell_head.next->prev = local_cell->cell_head.prev;
		local_cell->cell_head.prev->next = local_cell->cell_head.next;
		free_qos_traffic_history_cell(local_cell);
		local_head = local_head->next;
	}
	*/
	if (list_empty(&(record->channel_history_queue_head))) {
		return;
	}
	if (record->channel_history_queue_head.next == NULL) {
		
		return;
	}
	list_for_each_entry_safe(node, tmp, &(record->channel_history_queue_head), cell_head) {
		list_del(&(node->cell_head)); 
		aos_free(node); 
	}
	
	return;

}
int insert_history_record(struct _qos_traffic_channel_record *record)
{
	int ret = 0;
	struct _qos_traffic_history_cell *local_history_cell = NULL; 

	aos_assert1(record != NULL);

#ifdef __KERNEL__
	local_history_cell = (struct _qos_traffic_history_cell *)aos_malloc_atomic(sizeof(struct _qos_traffic_history_cell) );
#else
	local_history_cell = (struct _qos_traffic_history_cell *)aos_malloc(sizeof(struct _qos_traffic_history_cell) );
#endif

	if (local_history_cell == NULL) {
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for history_cell");
		ret = -ENOMEM;

		return ret;
	}

	init_qos_traffic_history_cell(local_history_cell);
	local_history_cell->history_traffic = record->total_band;
	local_history_cell->start_jiffy = record->start_jiffy;
	
	list_add_tail(&(local_history_cell->cell_head),&(record->channel_history_queue_head));
	// record->start_jiffy = jiffies;
	//record->start_jiffy = QOS_GET_JIFFIES();

	return ret;
}

void clear_expired_history_record(struct _qos_traffic_channel_record *record)
{
	//struct _qos_traffic_history_cell *local_history_cell = NULL;
	//struct aos_list_head *cur, *next;
	struct _qos_traffic_history_cell *node;
	struct _qos_traffic_history_cell *tmp;
	//int i;
	//i = 0;

	aos_assert0(record != NULL);
	
	// comment by liqin 2007/01/06 for segment fault
	/*
	aos_list_for_each_safe(cur, next, &(record->channel_history_queue_head))
	{
		local_history_cell = aos_list_entry(cur, struct _qos_traffic_history_cell, cell_head);
		if (local_history_cell == NULL) {
			aos_trace("bill:find bugs, NULL pointer\n");
			continue;
		}
		if (aos_qos_traffic_global_remove_jiffy - local_history_cell->start_jiffy > record->maximum_history_interval)	
		{
			aos_list_del(cur);
			++i;
			if (i % 100 == 0)
			{
				aos_trace("start_jiffy:%u",local_history_cell->start_jiffy);
				aos_trace("global_remove_jiffy",aos_qos_traffic_global_remove_jiffy);
			}
			free_qos_traffic_history_cell(local_history_cell);
		}
		else
			break;
	}
	*/
	list_for_each_entry_safe(node, tmp, &(record->channel_history_queue_head), cell_head) {
		aos_qos_traffic_global_remove_jiffy = QOS_GET_JIFFIES();
		if (aos_qos_traffic_global_remove_jiffy - node->start_jiffy > record->maximum_history_interval)	
		{
			list_del(&node->cell_head);
			free_qos_traffic_history_cell(node);
		}
	}
	
	return;
}
//////////////////////////////////////////////////////////////////
//
//The following is the functions of channel_record
/*
int init_qos_traffic_channel_record(struct _qos_traffic_channel_record *record, int channel_type,int array_size)
{
	int ret = 0;
	u16 *local_array_start = NULL;
	if (record == NULL) {
		QOS_TRAFFIC_PRINTK("Traffic channel_record does not exist, it should not happen in normal case\n");
		ret =  -EINVAL;
	} else {
		QOS_TRAFFIC_PRINTK("Initing channel_record\n");
		local_array_start = (u16*)aos_malloc(array_size * sizeof(u16));
		if (local_array_start == NULL) {
			QOS_TRAFFIC_PRINTK("Allocated memory failed\n");
			ret = -ENOMEM;
			goto fail;
		}
		memset(local_array_start,0,array_size * sizeof(u16));
		record->total_band = 0;
		record->channel_type = channel_type;
		record->array_size = array_size;
		record->array_start = local_array_start;
		record->start_jiffy = 0;
		record->last_recorded_jiffy = 0;
		record->current_index = 0;
		record->maximum_history_interval = 0;
		AOS_INIT_LIST_HEAD(&(record->channel_history_queue_head));
	}
	return ret;
fail:
	return ret;
}*/


/*
int modify_qos_calculating_interval(struct _qos_traffic_channel_record *record,int new_value)
{
	int ret = 0;
	u16 *local_start = NULL;
	if (record == NULL) {
		QOS_TRAFFIC_PRINTK("The record does not exist\n");
		ret = -EINVAL;
		goto fail;
	} else {
		local_start = (u16*)aos_malloc(new_value * sizeof(u16));
		if(local_start != NULL) {
			QOS_TRAFFIC_PRINTK("Modification Error\n");
			ret = -ENOMEM;
			goto fail;
		}
		insert_history_record(record);
		aos_free(record->array_start);
		record->array_start = local_start;
		return ret;
	}
fail:
	return ret;
}
*/
int rebuild_channel_record(struct _qos_traffic_channel_record *record)
{
	int ret;

	aos_assert1(record->array_size > 0);

#ifdef __KERNEL__
	record->array_start = (u16 *)aos_malloc_atomic(record->array_size * sizeof(u16));
#else
	record->array_start = (u16 *)aos_malloc(record->array_size * sizeof(u16));
#endif
	if (record->array_start == NULL) {
		ret = -ENOMEM;
		QOS_TRAFFIC_PRINTK("ERROR:No memory available\n");
		return ret;
	}
	memset(record->array_start,0,record->array_size * sizeof(u16));
	record->start_jiffy = aos_qos_traffic_global_temp_jiffy;
	//record->start_jiffy = QOS_GET_JIFFIES();
	record->last_recorded_jiffy = record->start_jiffy;
	record->current_index = 0;
	record->total_band = 0;
	return 0;
}
int init_channel_record(struct _qos_traffic_channel_record *record)
{
	int ret;

	ret = 0;
	memset(record,0,sizeof(struct _qos_traffic_channel_record));
//	record->channel_type = AOS_QOS_RULE_GEN;
	record->array_size = aos_qos_traffic_global_calculate_interval; 
#ifdef __KERNEL__
	record->array_start = (u16 *)aos_malloc_atomic(record->array_size * sizeof(u16));
#else
	record->array_start = (u16 *)aos_malloc(record->array_size * sizeof(u16));
#endif
	if (record->array_start == NULL) {
		ret = -ENOMEM;
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for channel record");
		return ret;
	}
	memset(record->array_start,0,record->array_size * sizeof(u16));
	record->start_jiffy = aos_qos_traffic_global_temp_jiffy;
	record->last_recorded_jiffy = record->start_jiffy;
	record->maximum_history_interval = sgMax_history_len;
	INIT_LIST_HEAD(&(record->channel_history_queue_head));
	return ret;

}


void show_history(struct _qos_traffic_channel_record *record)
{
//	struct list_head *local_head;
//	struct _qos_traffic_history_cell *local_cell;
//	int num;
//	num = 0;
/*
	local_head = record->channel_history_queue_head.next;
	while (local_head != &record->channel_history_queue_head) {
		local_cell = aos_list_entry(local_head, struct _qos_traffic_history_cell, cell_head);
		aos_trace("history:%u",local_cell->history_traffic);
		aos_trace("start_jiffy:%u",local_cell->start_jiffy);
		local_head = local_head->next;
//		++num;
	}
*/
//	aos_trace("num:%d\n",num);

	return;
}

void qos_traffic_update(struct _qos_traffic_channel_record *record,
						int len)
{
	int local_index = 0;
	int ret = 0;
	unsigned long local_jiffy;
	unsigned long temp_jiffy;
	unsigned long diff;

	aos_assert0(record);
	aos_assert0(record->array_start);

	local_index = record->current_index;
	temp_jiffy = aos_qos_traffic_global_temp_jiffy;
	local_jiffy = record->last_recorded_jiffy;

	if (temp_jiffy < local_jiffy) {
		//if (local_jiffy - temp_jiffy > 0x80000000) {
		//	local_jiffy -= ++temp_jiffy;
		//	temp_jiffy = 0xFFFFFFFF;
		//}
		diff = (u64)(temp_jiffy)+0xffffffff- local_jiffy;
		aos_trace("%s, temp_jiffy %u less than local_jiffy %u, diff %u", 
				__FUNCTION__, temp_jiffy,local_jiffy, diff);

		//aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"temp_jiffy %u less than local_jiffy %u, diff %u",
		//		temp_jiffy,local_jiffy, diff);
	}                                                //to avoid the special case
	else
	{
		diff = temp_jiffy - local_jiffy;
	}

//	while (local_jiffy < temp_jiffy) {
	while (diff > 0) {
		diff--;
	//	++i;
		++local_index;
		++local_jiffy;
		if (local_index == record->array_size) {
			ret = insert_history_record(record);                 
			if (ret != 0) {
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to allocate memory for history record");
			}
			local_index = 0;
			record->start_jiffy = local_jiffy;
		}
		record->total_band -= record->array_start[local_index];
		record->array_start[local_index] = 0;
	}
	record->total_band += len;
	record->array_start[local_index] += len;
	record->last_recorded_jiffy = aos_qos_traffic_global_temp_jiffy;
	record->current_index = local_index;

//	show_history(record);
	
	return;
}

u8 qos_get_priority_of_channel(struct ros_qos_skb_info *skb_info/*struct sk_buff *skb*/, int direction, struct _qos_traffic_channel **ch)
{
	struct _qos_traffic_channel *new_ch;
	struct chhashkey ch_key;
	//struct ros_qos_skb_info skb_info;
	u8 priority;
	int ret = 0;
	
	// aos_trace("enter %s", __FUNCTION__);
	if (ch) *ch = NULL;
	priority = eROS_QOS_PRIORITY_MAX;
	
	//ret = qos_traffic_parse_parm(skb, &skb_info, direction); 
	
	ret = ch_create_hashkey(skb_info->src_ip, 
			                skb_info->dst_ip, 
							skb_info->src_port, 
							skb_info->dst_port, 
							skb_info->protocol, 
							&ch_key);
	aos_trace("src_ip:%d\ndst_ip:%d\nsrc_port:%d\ndst_port:%d\n", skb_info->src_ip, 
			                skb_info->dst_ip, 
							skb_info->src_port, 
							skb_info->dst_port);
	new_ch = (struct _qos_traffic_channel *)hashtab_search(qos_traffic_hash_gen,&ch_key);
	
	if (new_ch)
	{
		priority = new_ch->priority;
		if (ch) 
		{
			*ch = new_ch;
			aos_qos_channel_hold(new_ch);
		}
	}	
	
	return priority; 
}


int qos_get_channel_bw(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u32 *band)
{
	struct _qos_traffic_channel *new_ch;
	struct chhashkey ch_key;
	
	*band = 0;
	
	ch_create_hashkey(sip,dip,sport,dport,proto,&ch_key);
	new_ch = (struct _qos_traffic_channel *)hashtab_search(qos_traffic_hash_gen,&ch_key);
	if (!new_ch) return -1;
	
	*band = new_ch->record.total_band;
	return eAosRc_Success;
}						


int qos_get_channel_info(u32 sip,u16 sport,u32 dip,u16 dport,u8 proto,u32 *src_ip,u16 *src_port, u32 *dst_ip, u16 *dst_port,u8 *outproto,void **p)
{
	struct _qos_traffic_channel *new_ch;
	struct chhashkey ch_key;

	ch_create_hashkey(sip,dip,sport,dport,proto,&ch_key);
	new_ch = (struct _qos_traffic_channel *)hashtab_search(qos_traffic_hash_gen,&ch_key);
	if (!new_ch) return -1;
	aos_assert1(new_ch);
	*src_ip = new_ch->src_ip;
	*dst_ip = new_ch->dst_ip;
	*src_port = new_ch->src_port;
	*dst_port = new_ch->dst_port;
	*outproto = new_ch->protocol;
	*p = new_ch;
	
	return 0;

}
