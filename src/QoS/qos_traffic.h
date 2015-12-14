/*
 * Copyright (c) 2006 PacketEngineering Beijing, PRC
 * All right reserved
 * 
 * Filename: qos_traffic.h 
 * FileID:   TBD
 * Abstract: This file is header for qos traffic facility 
 *
 * Current Version: 1.0
 * Author: Ping Wang
 * Complete Date: 11/25/2006
 *
 * Replace Version: N/A
 * Author: N/A
 * Complete Date: N/A
 */

#ifndef QOS_TRAFFIC_H
#define QOS_TRAFFIC_H

#include "KernelSimu/skbuff.h"
#include "KernelSimu/if.h"
#include "KernelSimu/if_ether.h"
#include "KernelSimu/atomic.h"
#include "QoS/hashtab.h"
#include <linux/list.h>

#include "aosUtil/List.h"

#define AOS_QOS_TRAFFIC_STATUS_OFF  0
#define AOS_QOS_TRAFFIC_STATUS_ON   1
#define AOS_QOS_TRAFFIC_STATUS_STOP 2
#define AOS_QOS_TRAFFIC_STATUS_READY 3
#define QOS_TRAFFIC_CH_HASH_KEY_LEN 16 
#define QOS_TRAFFIC_ETH_HASH_KEY_LEN 8
#define QOS_TRAFFIC_PRI_HASH_KEY_LEN 8 
#define QOS_TRAFFIC_CH_HASH_SIZE  10
#define QOS_TRAFFIC_ETH_HASH_SIZE 6 
#define QOS_TRAFFIC_PRI_HASH_SIZE 6 
#define QOS_CHANNEL_ACTIVE 1
#define QOS_CHANNEL_INACTIVE 0
#define QOS_DIRECTION_IN 0				// in direction macro, in means receive skb

#define qos_traffic_get_vlan_tag(skb)  (skb)->vid

//Reviewed added folloing emum errnumer
//emum _qos_channel_err {
//	eqos_ch_inval = EINVAL,
//	eqos_ch_nomem,
//	-EINVAL	  ,
//}
//Reviewed mayby 1.8M occupied in one hour 30 channel

enum qos_channel_action{
	qos_create_channel= 0,
	qos_active_channel,
	qos_deactive_channel,
	qos_remove_channel,
	qos_max_channel_action,
};

enum qos_monitor_direction {
	monitor_direction_none = 0,
	monitor_direction_in   = 1,
	monitor_direction_out  = 2,
	monitor_direction_both = 3
};

enum qos_direction {
	direction_in = 0,
	direction_out = 1,
};

enum qos_nf_direction {
	local_in = 0,
	pre_routing,
	post_routing,
};

enum bw_show_type{
	bw_show_ch = 0,
	bw_show_pri = 1,
	bw_show_eth = 2,

	bw_show_max
};

struct qos_bw_ctrl_data;

struct ch_show_data{
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  proto;
	u8  src_maskbit;
	u8  dst_maskbit;
};


struct pri_show_data{
	int if_index;
	u8 priority;
};


struct eth_show_data{
	int if_index;
	u8 direction;
};


struct bw_monitor_show_arg{
	int type;   //CHANNEL,PRIORITY,PROTO
	char *rsltBuff;
	unsigned int rsltIndex;
	int optlen;
	union{
		struct ch_show_data chdata;
		struct pri_show_data pridata;
		struct eth_show_data ethdata;
	}parms;
};


struct ch_history_show_data{
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  proto;
};


struct pri_history_show_data{
	int if_index;
	u8 priority;
};

struct eth_history_show_data{
	int if_index;
	u8 direction;
};


struct bw_history_show_arg{
	int type;
	char *rsltBuff;
	unsigned int rsltIndex;
	int optlen;
	u16 interval;
	u16 start_time;
	u16 end_time;
	union{
		struct ch_history_show_data chhisdata;
		struct pri_history_show_data prihisdata;
		struct eth_history_show_data ethhisdata;
	}parms;

};
struct arg_match_gen_rule {
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  proto;
	int   ifid;
	u8  priority;
	void  *r;
};	
struct chhashkey {
	u8 k[QOS_TRAFFIC_CH_HASH_KEY_LEN];
};
struct ethhashkey {
	u8 k[QOS_TRAFFIC_ETH_HASH_KEY_LEN];
};
struct prihashkey {
	u8 k[QOS_TRAFFIC_PRI_HASH_KEY_LEN];
};
struct _qos_traffic_history_cell{
	struct list_head cell_head;
	unsigned int  history_traffic;
	unsigned long start_jiffy;
};

struct _qos_traffic_channel_record{
	unsigned int total_band;
	u8  channel_type;
	u16 array_size;
	u16 *array_start;
	int   current_index;
	unsigned long start_jiffy;
	unsigned long last_recorded_jiffy;
	
	u32	packet_dropped;
	u32	bytes_dropped;
	u32	packet_sent;
	u32 bytes_sent;
	u32 packet_rcvd;
	u32 bytes_rcvd;
	u32 packet_buffered;
	u32 bytes_buffered;

	u8	status;
	u32  maximum_history_interval;
	struct list_head channel_history_queue_head;
};

struct _qos_traffic_channel{
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  protocol;
	u8  status;
	u8  priority;
	int   ifid;
	u16 vlan;
	u8  src_mac[ETH_ALEN];
	u8  dst_mac[ETH_ALEN];
	void  *matched_rule;
	u32 band_required;			// required maximum band for this channel
	int direction;
	struct _qos_traffic_channel_record record;
	//struct list_head list;
	
	atomic_t refcnt;
};
struct _qos_traffic_interface{
	u8 status;
	int dev_id;
	char dev_name[IFNAMSIZ];
	struct _qos_traffic_channel_record interface_record;
};

typedef struct ros_qos_skb_info{
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  protocol;
	int ifid;
	u16 vlan;
	u8  src_mac[ETH_ALEN];
	u8  dst_mac[ETH_ALEN];
	u8  dev_name[IFNAMSIZ];
	u16 len;
	u16 mac_proto;
	struct _qos_traffic_channel *ch;
}ros_qos_skb_info_t; 

extern void qos_channel_release(struct _qos_traffic_channel *ch); 

static inline void aos_qos_channel_hold(struct _qos_traffic_channel *ch)
{
	if(ch) atomic_inc(&ch->refcnt);	
}

static inline int aos_qos_channel_put(struct _qos_traffic_channel *ch)
{	
	if (ch && atomic_dec_and_test(&ch->refcnt))
	{	
		qos_channel_release(ch);
		return 0;
	}

	return 1;
}

extern struct hashtab *qos_traffic_hash_gen;
extern struct hashtab *qos_traffic_hash_pri;
extern struct hashtab *qos_traffic_hash_eth;
//extern struct list_head qos_traffic_list_gen;

int aos_qos_traffic_init_variables(void);
///////////////////////////////////////////////
//
//Traffic functions
void qos_traffic_update(struct _qos_traffic_channel_record *record,
						int len);
int insert_history_record(struct _qos_traffic_channel_record *record);
void clear_expired_history_record(struct _qos_traffic_channel_record *record);
int init_channel_record(struct _qos_traffic_channel_record *record);
int rebuild_channel_record(struct _qos_traffic_channel_record *record);
void clear_all_history_record(struct _qos_traffic_channel_record *record);

u32 ch_hash_val_cal(struct hashtab *h,void *k);
int ch_hash_cmp(struct hashtab *h,void *key1,void *key2);


void destroy_ch_cell(void *k,void *d,void *args);
u32 eth_hash_val_cal(struct hashtab *h,void *k);
int eth_hash_cmp(struct hashtab *h,void *key1,void *key2);
int aos_qos_traffic_remove_thread(void *arg);
int clear_history_record(void *k, void *d,void *args);
int if_remove_channel (void *k, void *d, void *args);
void *match_hash_gen(u32 src_ip,
					 u32 dst_ip,
					 u16 src_port,
					 u16 dst_port,
					 u8  proto,
					 int ifid,
					 u8 *priority);
void *match_hash_mac(u8 *mac,
					 u8 *priority);
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
	int direction);

int update_priority_array(u8 priority,
						  u16 len);
struct _qos_traffic_channel_record *match_ifhash(int ifid,u8 direction,struct ethhashkey *k);

int match_build_ifhash(int ifid,
				  u16 len,
				  u8 direction);
int match_update_ifhash(int ifid,
				  u16 len,
				  u8 direction);

int ch_create_hashkey(u32 src_ip,
					  u32 dst_ip,
					  u16 src_port,
					  u16 dst_port,
					  u8  proto,
					  struct chhashkey *k);
void *match_hash_vlan(u16 vlantag,
					  u8 *priority);
void *match_hash_if(u8 ifid,
					u8 *priority);
void *match_hash_port(u16 src_port,
					  u16 dst_port,
					  u8 *priority,
					  int ifid,
					  int direction);
int qos_traffic_parse_parm(struct sk_buff *skb, struct ros_qos_skb_info *skb_info, int direction);
void *match_hash_proto(u8 proto,
					   u8 *priority);
int map_match_gen_rule(void *k, void *d,void *args);
int eth_create_hashkey(int ifid,
					   u8 direction,
					   struct ethhashkey *k);
void simple_destroy_record(void *k,void *d,void *args);
int clear_list_history_record(void *k, void *d,void *args);
u32 pri_hash_val_cal(struct hashtab *h,void *k);
int pri_hash_cmp(struct hashtab *h,void *key1,void *key2);
int match_build_priority_hash(int ifid, u8 direction, u8 priority, u16 len, u32 band_required);
int match_update_priority_hash(int ifid,
						 u8 direction,
						 u8 priority,
						 u16 len);
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
	int direction);
int match_update_channel_hashtable(int nf_dir, struct ros_qos_skb_info *skb_info, u8 *priority);
int pri_create_hashkey(int ifid,
					   u8 direction,
					   u8 priority,
					   struct prihashkey *k);

extern int build_pri_ch(int ifid,u8 direction,struct qos_bw_ctrl_data **r,struct prihashkey *k);


int clear_pri_history_record (void *k, void *d,void *args);
extern int qos_get_channel_bw(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u32 *band);
///////////////////////////////////////////
//init functions
extern int qos_traffic_init_varialbes(void);
extern int qos_traffic_set_status(int s);
extern int AosQos_initTrafficModule(void);
extern void AosQos_exitTrafficModule(void);
extern int monitor_test_direction;
extern int aos_qos_traffic_facility_status;
extern int qos_traffic_monitor(struct sk_buff *skb, struct ros_qos_skb_info* skb_info, int direction, u8 *priority);
extern int qos_get_channel_bw(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u32 *band);
extern int qos_bandwidth_monitor(int nf_dir, int direction, struct ros_qos_skb_info *skb_info, u8 *priority);
extern struct qos_bw_ctrl_data *match_priority_hash(int ifid,u8 direction,u8 priority,struct prihashkey *k);
extern struct _qos_traffic_channel_record *match_ifhash(int ifid,u8 direction,struct ethhashkey *k);
struct _qos_traffic_channel * match_channel_hashtable(u32 src_ip,u32 dst_ip,u16 src_port,u16 dst_port,u8 proto,struct chhashkey *ch_key);
extern int qos_retrieve_info(struct sk_buff *skb, struct ros_qos_skb_info *skb_info, int direction);
extern void aos_qos_traffic_updated_channel_cache(unsigned long data);
extern int sgMax_history_len;
#endif
