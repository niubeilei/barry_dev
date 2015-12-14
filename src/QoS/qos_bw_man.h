//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_bw_man.h
// Description:
//   
//
// Modification History:
//		created 12/31/2006
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __QOS_BW_MAN_H__
#define __QOS_BW_MAN_H__

#include "aosUtil/Types.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/string.h"
#include "KernelSimu/atomic.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_util.h"
#include "QoS/qos_logstat.h"

#define QOS_BW_FLOW_CTRL(priority) gqos_bw_flow_ctrl[(priority)]

//#define QOS_BW_SET_ALL_FLOW_CTRL(ctrl, ctrl_value)  memset(ctrl, sizeof(ctrl), (ctrl_value))

#define QOS_BW_DETA_DEFAULT 16384			// 1kbytes/s
#define QOS_BW_MINIMUM_DEFAULT 2*1024 		// 2kbytes/s
#define QOS_BW_MAXIMUM_DEFAULT 1000000000 	// 1Gbytes/s
#define QOS_BW_P2_STABLE_TIMES 2			// default stable sampling times fo phrase 2 state
#define QOS_BW_SINGLE_STABLE_TIMES 3		// default stable sampling times fo single state
#define QOS_BW_P3_INTERVAL_DEFAULT 5000		// default stable time fo phrase 3 state
#define QOS_BW_AVERAGE_CALCULATE_SHIFT 5 	// average throughouts calculate shift, default 4 seconds
#define QOS_BW_EQUAL_THROLD 256				// two bandwidth equal throld
#define QOS_BW_RESERVED_BW 4096				// default reserved bandwidth
#define QOS_TOTAL_BW_LIFE 600000			// 10 minutes
#define QOS_BW_COMPARATION_PRECISION_SHIFT 5// bandwidth comparation precision shift, 1/(2^5) = 1/32 = 3.125%
#define QOS_TOTAL_BW_CMP_PRECISION_SHIFT   3// total bandwidth comparation precision shift, 1/(2^3) = 1/8 = 12.5%

#define IS_QOS_BW_CTRL_ON (sgbw_ctrl != 0)

typedef enum ros_qos_state{
	eROS_QOS_BW_STATE_INIT = 0,		// init state
	eROS_QOS_BW_STATE_P1,			// phrase 1 state, no control in this state
	eROS_QOS_BW_STATE_SINGLE,		// single state, only one kind of priority flow exist
	eROS_QOS_BW_STATE_P2,			// phrase 2 state, start to control in this state
	eROS_QOS_BW_STATE_P2_DECREASE_LOW,// phrase 2 state, multiple kinds of priority flows exist
	eROS_QOS_BW_STATE_P2_TUNE_LOW,	// phrase 2 state, the high level flow is stable, recover the low level flow
	eROS_QOS_BW_STATE_P2_INCREASE_LOW,// phrase 2 state, multiple kinds of priority flows exist, increased low level flow
	eROS_QOS_BW_STATE_P2_TUNE_LOW2,	// phrase 2 state, the high level flow is stable, recover the low level flow
	eROS_QOS_BW_STATE_P3,			// phrase 3 state, stable state aftter BW control


	eROS_QOS_BW_STATE_P20 = 20,		
	eROS_QOS_BW_STATE_P21 = 21,		
	eROS_QOS_BW_STATE_P24 = 24,		
	eROS_QOS_BW_STATE_P30 = 30,		

	eROS_QOS_BW_STATE_MAX,
}ros_qos_state_t;

typedef enum ros_qos_priority{
	eROS_QOS_PRIORITY_LOW = 0,	// priority low 0
	eROS_QOS_PRIORITY_MED,		// priority medium 1
	eROS_QOS_PRIORITY_HIH ,		// priority high 2
	eROS_QOS_PRIORITY_MAX,
}ros_qos_priority_t;

typedef enum ros_qos_flow_ctrl{
	eROS_QOS_FLOW_CTRL_PASS = 0,	// forward the packet
	eROS_QOS_FLOW_CTRL_DROP,		// drop the packet
	eROS_QOS_FLOW_CTRL_MINIMUM,		// the bandwidth is dropped to minimum now
	eROS_QOS_FLOW_CTRL_BUFFERED,	// buffer the packet
}ros_qos_flow_ctrl_t;

typedef struct qos_queue_cell {
	struct aos_list_head list;
	struct nf_info *info;
	struct sk_buff *skb;
}qos_queue_cell_t;

typedef struct qos_bw_ctrl_data{
	u16 ifid;									// interface id
	u8 direction;								//  0 = 'in', 1 = 'out'
	u8 bw_ctrl;									// bandwidth control is on or off
	u8 drop_index;								// define at this state which priority flow is to be dropped
	ros_qos_state_t state;						// bandwidth control state
	u8 ctrl[eROS_QOS_PRIORITY_MAX];				// drop, pass or buffered,

	u32 total_bw;								// total bandwidth for this interface in in/out direction
	u32 total_bw_jiffies;						// total bandwidth detection time
	
	u32 real_total_throughout;					// real time total throughout
	u32 last_total_throughout;					// last time total throughout
	u32 real_high_throughout;					// real time high level throughout
	u32 last_high_throughout;					// last time high level throughout
	u32 state_high_throughout;					// stateful high level throughout


	u32 real_bw[eROS_QOS_PRIORITY_MAX];			// real time bandwidth detected for each priority
	u32 last_bw[eROS_QOS_PRIORITY_MAX];			// last time bandwidth detected for each priority
	u32 last_time;								// control time
	u32 allowed_bw[eROS_QOS_PRIORITY_MAX];		// bandwidth allowed for each priority 
	u32 allowed_bytes[eROS_QOS_PRIORITY_MAX];	// bytes allowed in one sampleing time for each priority 
	u32 sent_bytes[eROS_QOS_PRIORITY_MAX];		// bytes sent in one sampleing time for each priority 
	u32 last_sent_time[eROS_QOS_PRIORITY_MAX];	// last packet sending time
	u32 req_bw[eROS_QOS_PRIORITY_MAX];			// bandwidth required by each priority flows
	
	u32 limit_bw[eROS_QOS_PRIORITY_MAX];		// bandwidth limited for each priority 
	
	u32 tune_keep_time;							// how long tunning of this turn is keeping.
	int tunning_turns;							// This tell what times have you tunned the bandwidth

	u8 state_stable_times;						// stable times for phrase 2
	u32 state_start_time;						// start time of the new state

	struct timer_list timer;
	struct _qos_traffic_channel_record record[eROS_QOS_PRIORITY_MAX];
	
	struct qos_stat stat[eROS_QOS_PRIORITY_MAX];

	struct aos_list_head skb_rcv_queue[eROS_QOS_PRIORITY_MAX];	// buffered skb queue	
		
	atomic_t refcnt;
}qos_bw_ctrl_data_t;

#define QOS_BW_CTRL_HASH_KEY_SIZE 8
struct bw_ctrl_key{
	    u8 key[QOS_BW_CTRL_HASH_KEY_SIZE];
};

static inline void bw_ctrl_hashkey(int ifindex, u8 direction, struct bw_ctrl_key *hashkey)
{
    int ifid;

    ifid = ifindex;
    memset(hashkey, 0, sizeof(struct bw_ctrl_key));
    memcpy(hashkey->key, &ifid, sizeof(int));
    hashkey->key[sizeof(int)] = direction;
}

static inline void QOS_BW_NEXT_STATE(struct qos_bw_ctrl_data *qos_bw_ctrl_data, ros_qos_state_t state)
{
	qos_bw_ctrl_data->state_start_time = 0;
	qos_bw_ctrl_data->state = state;
	qos_bw_ctrl_data->state_stable_times = 0;
}

static inline struct qos_bw_ctrl_data* qos_bw_ctrl_data_create(int ifid, int direction)
{
	struct qos_bw_ctrl_data* qos_bw_ctrl_data;
	int i;

#ifdef __KERNEL__
	qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)aos_malloc_atomic(sizeof(struct qos_bw_ctrl_data));
#else
    qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)aos_malloc(sizeof(struct qos_bw_ctrl_data));
#endif
	if (!qos_bw_ctrl_data)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed to malloc qos bw ctrl data");
		return NULL;
	}
	memset(qos_bw_ctrl_data, 0, sizeof(struct qos_bw_ctrl_data));
	atomic_set(&(qos_bw_ctrl_data->refcnt), 1);
	qos_bw_ctrl_data->ifid = ifid;
	qos_bw_ctrl_data->direction = direction;
	
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		AOS_INIT_LIST_HEAD(&(qos_bw_ctrl_data->skb_rcv_queue[i]));
	}

	return qos_bw_ctrl_data;
}

static inline void qos_bw_ctrl_data_release(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int i;
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; ++i) 
	{
		simple_destroy_record(NULL, &(qos_bw_ctrl_data->record[i]), NULL);
	}
	aos_free(qos_bw_ctrl_data);
}

static inline int qos_bw_ctrl_data_hold(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	atomic_inc(&qos_bw_ctrl_data->refcnt);
	return 0;
}

static inline int qos_bw_ctrl_data_put(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	if (atomic_dec_and_test(&qos_bw_ctrl_data->refcnt))
	{
		qos_bw_ctrl_data_release(qos_bw_ctrl_data);
		return 1;	// the ponitor has been released
	}
	return 0;
}

extern void ros_qos_bw_ctrl_timer(unsigned long data);

extern int ros_qos_bw_ctrl_init(void);

extern int qos_traffic_enqueue(struct sk_buff *skb, int skb_len, int direction, u8 priority);

extern int qos_nf_queue_handler(struct sk_buff *skb, struct nf_info *info, unsigned int queuenum, void *data);

extern int priority_reinject_skb_thread(void *arg);

extern unsigned int qos_nf_queue_in(unsigned int hook,
					 struct sk_buff **pskb,
					 const struct net_device *in,
					 const struct net_device *out,
					 int (*okfn)(struct sk_buff*));

extern int qos_bw_man_save_config(char *rsltBuff, unsigned int *rsltIndex, int optlen);

extern int qos_bw_man_clear_config(void);

extern int qos_bw_man_show_config(char *rsltBuff, unsigned int *rsltIndex, int optlen);

extern int qos_bw_limit(struct qos_bw_ctrl_data *qos_bw_ctrl_data, u32 bw_change);

extern int qos_do_limit(struct qos_bw_ctrl_data *qos_bw_ctrl_data);
#endif // #ifdef __QOS_BW_MAN_H
