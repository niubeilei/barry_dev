//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_bw_man.c
// Description:
//   
// Author: Tim
//
// Modification History: 
//	created 12/29/2006
// 
////////////////////////////////////////////////////////////////////////////
#include "QoS/qos_bw_man.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Modules.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Memory.h"
#include "aosUtil/BusyLoop.h"
#include "aosUtil/List.h"
#include "KernelSimu/string.h"
#include "KernelSimu/skbuff.h"
#include "KernelSimu/if.h"
#include "KernelSimu/timer.h"
#include "KernelSimu/netdevice.h"
#include "KernelSimu/netfilter.h"
#include "KernelSimu/atomic.h"
#include "KernelSimu/thread.h"
#include "KernelSimu/delay.h"
#include "QoS/hashtab.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_util.h"
#include "QoS/qos_logstat.h"
#include "QoS/qos_global.h"

extern int is_valid_if(char *dev_name,int *devid);

static u8 sgbw_ctrl = 0; 											// disable qos bandwidth control	

static int sgbw_p1_sample_iv = 1000; 							// bandwidth control phrase one interval 3000 milli second
static unsigned long sgbw_p2_sample_iv = 1000;						// phrase sample interval 125 ms 
static unsigned long sgbw_p3_iv = QOS_BW_P3_INTERVAL_DEFAULT;// bandwidth control phrase 3 interval 500 milli second default
static u8 sgqos_single_samples = QOS_BW_SINGLE_STABLE_TIMES; 		// stable sampling times for single state
static u8 sgbw_stable_times = QOS_BW_P2_STABLE_TIMES; 				// stable times for the sampling of phrase 2
static u16 sgbw_detax = QOS_BW_DETA_DEFAULT;// 1k/s the minimum unit for bandwidth tuning, this can be configure through CLI
static u8  sgbw_avr_shift = QOS_BW_AVERAGE_CALCULATE_SHIFT; 		// average throughouts calculate shift
static u8  sgbw_cmp_precision = QOS_BW_COMPARATION_PRECISION_SHIFT; // bandwidth comparation precision shift
static u8  sgbw_total_precision = QOS_TOTAL_BW_CMP_PRECISION_SHIFT; // bandwidth comparation precision shift
static u16 sgbw_eq_throld = QOS_BW_EQUAL_THROLD;					// two bandwidth equal throld
static u32 sgbw_reserved_bw = QOS_BW_RESERVED_BW;					// reserved bandwidth
static u32 sgbw_total_bw_life = QOS_TOTAL_BW_LIFE;					// total bandwidth valid life span

static struct hashtab *qos_bw_ctrl_hash = NULL;

unsigned long gqos_bw_minimum[eROS_QOS_PRIORITY_MAX]; // minimum bandwidth reserved for each priority


static unsigned long sgbw_t20_iv = 1000;
static unsigned long sgbw_t21_iv = 1000;
static unsigned long sgbw_t24_iv = 1000;
static unsigned long sgbw_t30_iv = 1000;
int aos_qos_p20(struct qos_bw_ctrl_data *qos_bw_ctrl_data);
int aos_qos_p21(struct qos_bw_ctrl_data *qos_bw_ctrl_data);
int aos_qos_p24(struct qos_bw_ctrl_data *qos_bw_ctrl_data);
int aos_qos_p30(struct qos_bw_ctrl_data *qos_bw_ctrl_data);

static inline u32 bw_ctrl_hash_val(struct hashtab *h, void *k)
{
	u32 value = 0;
    u8 *pos;
	struct bw_ctrl_key *hashkey;
	int i;  

	hashkey = (struct bw_ctrl_key *)k;
	pos = hashkey->key; 
	value += (*pos << 1);
	++pos;  
	for (i = 1; i < QOS_BW_CTRL_HASH_KEY_SIZE; ++i) 
	{
		value += hashkey->key[i];
    }
    value &= ((2 << QOS_BW_CTRL_HASH_KEY_SIZE) - 1);
    return value;
}

static inline int bw_ctrl_hash_cmp(struct hashtab *h, void *key1, void *key2)
{
	return memcmp(key1,key2,QOS_BW_CTRL_HASH_KEY_SIZE);	
}

static inline int qos_bw_compare(u32 bw1, u32 bw2)
{
	u32 bw;
	if (bw1 >= bw2) 
	{
		bw = bw1 - bw2;
		if (bw <= sgbw_eq_throld) return 0;
		else return 1;
	}
	else
	{
		bw = bw2 - bw1;
		if (bw <= sgbw_eq_throld) return 0;
		else return -1;
	}
}

// bandwidth compare with precision as parameter
static inline int qos_bw_compare_precision(u32 bw1, u32 bw2, int precision)
{
	u32 bw;
	u32 bw_precision;

	bw_precision = bw2 >> precision;
	if (bw1 >= bw2) 
	{
		bw = bw1 - bw2;
		if (bw <= bw_precision) return 0;
		else return 1;
	}
	else
	{
		bw = bw2 - bw1;
		if (bw <= bw_precision) return 0;
		else return -1;
	}
}

static inline int qos_bw_compare2(u32 bw1, u32 bw2, u32 throld, int precision)
{
	if (bw1 <throld && bw2 <= throld) return qos_bw_compare_precision(bw1, bw2, precision);

	return qos_bw_compare(bw1, bw2);
}


static u32 qos_bw_get_avr_throughout(struct _qos_traffic_channel_record *record, int span_shift)
{
	struct _qos_traffic_history_cell *history_cell;
	struct list_head *pos;
	u32 bw;
	int counts;
	int i;

	i = 0;
	counts = 1 << span_shift; //sgbw_avr_shift;
	bw = 0;
	
	list_for_each_prev(pos, &(record->channel_history_queue_head))
	{
		history_cell = list_entry(pos, struct _qos_traffic_history_cell, cell_head);
		bw += history_cell->history_traffic;
		i++;
		if (i >= counts) break;
	}

//aos_eng_log(eAosMD_QoS, "%s enter, bw %u, i %d", __FUNCTION__, bw, i);		
	if (i == counts)
	{
		bw = bw >> span_shift; //sgbw_avr_shift;
	}
	else if (i != 0)
	{
		bw = bw/i;
	}
//aos_eng_log(eAosMD_QoS, "%s exit, bw %u, i %d", __FUNCTION__, bw, i);		

	return bw;
}

static void qos_bw_get_realtime_bw(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int i;
	
	qos_bw_ctrl_data->real_total_throughout = 0;
	qos_bw_ctrl_data->real_high_throughout = 0;

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		// long time since last updateing, update bandwidth for this priroity
		qos_traffic_update(&qos_bw_ctrl_data->record[i], 0);

		qos_bw_ctrl_data->real_bw[i] = qos_bw_get_avr_throughout(&(qos_bw_ctrl_data->record[i]), sgbw_avr_shift);
		
		qos_bw_ctrl_data->real_total_throughout += qos_bw_ctrl_data->real_bw[i];

		if (i > qos_bw_ctrl_data->drop_index)
				qos_bw_ctrl_data->real_high_throughout += qos_bw_ctrl_data->real_bw[i];			
	}
}

static void qos_bw_set_low_bw(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	u32 high_bw;
	u32 low_bw;
	int drop_index, i;
	
	high_bw = 0; low_bw = 0;

	drop_index = qos_bw_ctrl_data->drop_index;

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		if (i > drop_index )
			high_bw += qos_bw_get_avr_throughout(&(qos_bw_ctrl_data->record[i]), sgbw_avr_shift);
		else if (i < drop_index)
			low_bw += qos_bw_get_avr_throughout(&(qos_bw_ctrl_data->record[i]), sgbw_avr_shift);
	}

	if (qos_bw_ctrl_data->total_bw < (high_bw+low_bw+sgbw_reserved_bw))
		qos_bw_ctrl_data->allowed_bw[drop_index] = 0;
	else
		qos_bw_ctrl_data->allowed_bw[drop_index] = 
			qos_bw_ctrl_data->total_bw - high_bw - low_bw - sgbw_reserved_bw;

	if (qos_bw_ctrl_data->allowed_bw[drop_index] < gqos_bw_minimum[drop_index])
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
	}
		
aos_min_log(eAosMD_QoS, "%s drop_index %d, high_bw %u, low_bw %u, total_bw %u, allowed_bw %u",
		__FUNCTION__, drop_index, high_bw, low_bw, qos_bw_ctrl_data->total_bw, qos_bw_ctrl_data->allowed_bw[drop_index]);

}

static void qos_bw_ctrl_reset_data(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int i;
	qos_bw_ctrl_data->drop_index = 0;
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_PASS;
		qos_bw_ctrl_data->last_bw[i] = 0;
		qos_bw_ctrl_data->allowed_bw[i] = 0;
		qos_bw_ctrl_data->limit_bw[i] = 0;
	}
	qos_bw_ctrl_data->last_time = 0;
	qos_bw_ctrl_data->tune_keep_time = 0;
	qos_bw_ctrl_data->tunning_turns = 0;
	qos_bw_ctrl_data->state_stable_times = 0;
}

static inline void qos_bw_decrease_low(struct qos_bw_ctrl_data *qos_bw_ctrl_data, int drop_index)
{
aos_eng_log(eAosMD_QoS, "%s enter drop_index %d, allowd_bw %u, tunning_turns %d",	__FUNCTION__, 
		drop_index, qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->tunning_turns);

	// decrease low level bw, reserved bw for high level flow
	if (qos_bw_ctrl_data->allowed_bw[drop_index] == 0)
		qos_bw_ctrl_data->allowed_bw[drop_index] = qos_bw_ctrl_data->last_bw[drop_index];

	if (qos_bw_ctrl_data->tunning_turns == 0)
	{
		if (qos_bw_ctrl_data->allowed_bw[drop_index] < sgbw_detax)
		{
			qos_bw_ctrl_data->allowed_bw[drop_index] = 0;
			goto low_min;
		}
		qos_bw_ctrl_data->allowed_bw[drop_index] -= sgbw_detax;
	}
	else
	{
		//if (qos_bw_ctrl_data->allowed_bw[drop_index] < (u32)((sgbw_detax<< (qos_bw_ctrl_data->tunning_turns-1))))
		if (qos_bw_ctrl_data->allowed_bw[drop_index] < (u32)sgbw_detax)
		{
			qos_bw_ctrl_data->allowed_bw[drop_index] = 0;
			goto low_min;
		}
		// qos_bw_ctrl_data->allowed_bw[drop_index] -= (sgbw_detax << (qos_bw_ctrl_data->tunning_turns-1));
		qos_bw_ctrl_data->allowed_bw[drop_index] -= sgbw_detax;
	}
	qos_bw_ctrl_data->tunning_turns++;

low_min:
	if (qos_bw_ctrl_data->allowed_bw[drop_index] < gqos_bw_minimum[drop_index])
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
	}
	
aos_eng_log(eAosMD_QoS, "%s drop_index %d, allowd_bw %u, tunning_turns %d",	__FUNCTION__, 
		drop_index, qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->tunning_turns);

}

static inline void qos_bw_increase_low(struct qos_bw_ctrl_data *qos_bw_ctrl_data, int drop_index)
{
	// decrease low level bw, reserved bw for high level flow
	if (qos_bw_ctrl_data->allowed_bw[drop_index] == 0)
		qos_bw_ctrl_data->allowed_bw[drop_index] = qos_bw_ctrl_data->last_bw[drop_index];

	if (qos_bw_ctrl_data->tunning_turns == 0)
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] += sgbw_detax;
	}
	else
	{
		// qos_bw_ctrl_data->allowed_bw[drop_index] += (sgbw_detax << (qos_bw_ctrl_data->tunning_turns-1));
		qos_bw_ctrl_data->allowed_bw[drop_index] += sgbw_detax;
	}
	qos_bw_ctrl_data->tunning_turns++;
	
	if (qos_bw_ctrl_data->allowed_bw[drop_index] >QOS_BW_MAXIMUM_DEFAULT)
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = QOS_BW_MAXIMUM_DEFAULT;
	}
}


// return 0, there still enough space 
// return 1, there is no enough space
static inline int qos_bw_dec_low2(
	struct qos_bw_ctrl_data *qos_bw_ctrl_data, 
	int drop_index,
	u32 bw_dec)
{
	if (qos_bw_ctrl_data->allowed_bw[drop_index] == gqos_bw_minimum[drop_index] ||
	     qos_bw_ctrl_data->ctrl[drop_index] == eROS_QOS_FLOW_CTRL_MINIMUM)
	{
aos_eng_log(eAosMD_QoS, "%s no enough space, drop_index %d, allowd_bw %u, ctrl %d", 
		__FUNCTION__, drop_index, qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->ctrl[drop_index]);
		return 1;
	}
	
	if (qos_bw_ctrl_data->allowed_bw[drop_index] <= bw_dec)
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
		if (bw_dec > qos_bw_ctrl_data->allowed_bw[drop_index] ) 
			return 0;
		else {
aos_eng_log(eAosMD_QoS, "%s no enough space, drop_index %d, allowd_bw %u", 
		__FUNCTION__, drop_index, qos_bw_ctrl_data->allowed_bw[drop_index]);
			return 1;
		}
	}

	qos_bw_ctrl_data->allowed_bw[drop_index] -= bw_dec;
	if (qos_bw_ctrl_data->allowed_bw[drop_index] < gqos_bw_minimum[drop_index])
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
	}

aos_eng_log(eAosMD_QoS, "%s, drop_index %d, allowd_bw %u", 
		__FUNCTION__, drop_index, qos_bw_ctrl_data->allowed_bw[drop_index]);
	return 0;
}

static inline void qos_bw_inc_low2(
	struct qos_bw_ctrl_data *qos_bw_ctrl_data, 
	int drop_index,
	u32 bw_inc)
{
	qos_bw_ctrl_data->allowed_bw[drop_index] += bw_inc;

	if (qos_bw_ctrl_data->allowed_bw[drop_index] > gqos_bw_minimum[drop_index])
	{
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_DROP;
	}
	if (qos_bw_ctrl_data->allowed_bw[drop_index] >QOS_BW_MAXIMUM_DEFAULT)
	{
		qos_bw_ctrl_data->allowed_bw[drop_index] = QOS_BW_MAXIMUM_DEFAULT;
	}
aos_eng_log(eAosMD_QoS, "%s increase low band, drop_index %d, allowed_bw %u",
		__FUNCTION__, drop_index, qos_bw_ctrl_data->allowed_bw[drop_index]);
}

static inline int qos_bw_is_high_glow(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	long real_high_bw;
	long last_high_bw;
	int i;
	
	// reenter this state
	// Sample Start
	// check to see whether high level bandwith is increased
	real_high_bw = 0;
	last_high_bw = 0;
	for (i = qos_bw_ctrl_data->drop_index+1; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		real_high_bw += qos_bw_ctrl_data->real_bw[i]; 
		last_high_bw += qos_bw_ctrl_data->last_bw[i];
		
	}
aos_eng_log(eAosMD_QoS, "liqin %s, drop_index %d, real_high_bw %d, last_high_bw %d, real_high_bw-last_high_bw %d",
		__FUNCTION__, qos_bw_ctrl_data->drop_index, real_high_bw, last_high_bw, real_high_bw-last_high_bw);
	return qos_bw_compare2(real_high_bw, last_high_bw, sgbw_eq_throld, sgbw_cmp_precision);
}

static inline void qos_bw_set_all_last_bw(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int i;
	
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		qos_bw_ctrl_data->last_bw[i] = qos_bw_ctrl_data->real_bw[i];
	}

	qos_bw_ctrl_data->last_total_throughout = qos_bw_ctrl_data->real_total_throughout;
	qos_bw_ctrl_data->last_high_throughout = qos_bw_ctrl_data->real_high_throughout;
}

// return 0 means single level flow, nonzero means multiple levels flows
static inline int is_multi_level_exist(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int nonzero_bw_counts = 0;
	int i;

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		// if (qos_bw_ctrl_data->real_bw[i] != 0) nonzero_bw_counts++;
		//if (qos_bw_ctrl_data->record[i].total_band != 0) nonzero_bw_counts++;
		if (qos_bw_get_avr_throughout(&(qos_bw_ctrl_data->record[i]), 2) != 0) nonzero_bw_counts++;
		//else
		//	aos_eng_log(eAosMD_QoS, "%s, %d level throughout is zero", __FUNCTION__, i);
	}

	// whether flows are all of the same priority, or there is no flow. 
	// there is no need for bandwidith control. change to state P2_SINGLE
	if (nonzero_bw_counts <= 1)  
	{
		return 0;
	}

	return 1;
}

static inline int is_total_bw_life_expired(struct qos_bw_ctrl_data *qos_bw_ctrl_data, u32 cur_time)
{
	u32 diff;
	
	if (qos_bw_ctrl_data->total_bw_jiffies <= cur_time)
	{
		diff = cur_time - qos_bw_ctrl_data->total_bw_jiffies;
		if (diff >= sgbw_total_bw_life) return 1;
	}
	else
	{	
		diff = qos_bw_ctrl_data->total_bw_jiffies + sgbw_total_bw_life;
		if (diff <= cur_time) return 1;
	}

	return 0;
}

static void qos_bw_set_total_bw(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	if (qos_bw_compare(qos_bw_ctrl_data->real_total_throughout, qos_bw_ctrl_data->total_bw) > 0)
	{
		aos_eng_log(eAosMD_QoS, "liqin %s before, jiffies %u, real_total_throughout %u, total_bw %u", 
			__FUNCTION__, QOS_GET_JIFFIES(), 
			qos_bw_ctrl_data->real_total_throughout, qos_bw_ctrl_data->total_bw);
		// set new total bandwidth
		qos_bw_ctrl_data->total_bw = qos_bw_ctrl_data->real_total_throughout;
		qos_bw_ctrl_data->total_bw_jiffies = QOS_GET_JIFFIES();
		aos_eng_log(eAosMD_QoS, "liqin %s after, jiffies %u, real_total_throughout %u, total_bw %u", 
			__FUNCTION__, QOS_GET_JIFFIES(), 
			qos_bw_ctrl_data->real_total_throughout, qos_bw_ctrl_data->total_bw);
		
	}
}

static int qos_bw_state_init(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	aos_assert1(qos_bw_ctrl_data);
aos_eng_log(eAosMD_QoS, "%s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	
	init_timer(&qos_bw_ctrl_data->timer);
	qos_bw_ctrl_data->timer.data = (long)qos_bw_ctrl_data;
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
	qos_bw_ctrl_data->timer.function = ros_qos_bw_ctrl_timer;

	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);
	
	//QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P20);
	
	return eAosRc_Success;
}

static int qos_bw_state_p1(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	u32 cur_time;
	aos_assert1(qos_bw_ctrl_data);

	// add reference count, TBD
	// qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p1_sample_iv; // milli second
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p1_sample_iv; // milli second		

	// set default control "passthrough" for all levels
	if (!qos_bw_ctrl_data->state_start_time)
	{
aos_eng_log(eAosMD_QoS, "%s first, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	
		qos_bw_ctrl_data->state_start_time = QOS_GET_JIFFIES();
		qos_bw_ctrl_reset_data(qos_bw_ctrl_data);
		return eAosRc_Success;
	}

	// if single level flows exist then go to single state
	if (!is_multi_level_exist(qos_bw_ctrl_data))
	{
aos_eng_log(eAosMD_QoS, "%s single level, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_SINGLE);
		return eAosRc_Success;
	}

	// total flow is not changed and total flow is larger or equal than total bw
	// then change state to P2
	if ((0 == qos_bw_compare_precision(qos_bw_ctrl_data->real_total_throughout, 
					                   qos_bw_ctrl_data->last_total_throughout,
									   sgbw_cmp_precision)) && 
	    (0 <= qos_bw_compare_precision(qos_bw_ctrl_data->real_total_throughout, 
									   qos_bw_ctrl_data->total_bw,
									   sgbw_cmp_precision)) )//&& 
	/*    (0 == qos_bw_compare_precision(qos_bw_ctrl_data->real_high_throughout, 
							  		   qos_bw_ctrl_data->last_high_throughout,
							  		   sgbw_cmp_precision))) */
	{
aos_eng_log(eAosMD_QoS, "%s change to P2, jiffies %u, rel_tot_thro %u, lst_tot_throu %u, tot_bw %u"
		                ", rel_hig_thro %u, lst_hig_thro %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->real_total_throughout, 
						qos_bw_ctrl_data->last_total_throughout, qos_bw_ctrl_data->total_bw,	
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout);	

		qos_bw_ctrl_data->state_high_throughout = qos_bw_ctrl_data->real_high_throughout;
		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv; // milli second		
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2);
		return eAosRc_Continue;
	}

	cur_time = QOS_GET_JIFFIES();
	if (is_total_bw_life_expired(qos_bw_ctrl_data, cur_time))
	{
		// total bandwidth life expired
aos_eng_log(eAosMD_QoS, "%s Total bandwidth life expired, last total_bw %u, new total_bw %u, last time %u, cur time %u", 
		__FUNCTION__, qos_bw_ctrl_data->total_bw, qos_bw_ctrl_data->real_total_throughout, 
		qos_bw_ctrl_data->total_bw_jiffies, cur_time);

		qos_bw_ctrl_data->total_bw = qos_bw_ctrl_data->real_total_throughout;
		qos_bw_ctrl_data->total_bw_jiffies = cur_time;
	}
	
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_tot_thro %u, lst_tot_throu %u, tot_bw %u"
		                ", rel_hig_thro %u, lst_hig_thro %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->real_total_throughout, 
						qos_bw_ctrl_data->last_total_throughout, qos_bw_ctrl_data->total_bw,	
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout);	

	//qos_bw_ctrl_data->last_total_throughout = qos_bw_ctrl_data->real_total_throughout;
	return eAosRc_Success;
}

static int qos_bw_state_p2(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	//int drop_index;
	//int i;

// aos_eng_log(eAosMD_QoS, "%s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	

	// This is the first time to enter this state
	// do no change state, next time will still enter the same state P2
	if (!qos_bw_ctrl_data->state_start_time)
	{

aos_eng_log(eAosMD_QoS, "liqin first enter %s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	

		qos_bw_ctrl_data->state_start_time = QOS_GET_JIFFIES();

		// set total bandwidth
		qos_bw_set_total_bw(qos_bw_ctrl_data);
		
		// retrieve the realtime bw for all levels
		// decrease the lowest flow by detaX
		/*
		drop_index = -1;
		for (i = 0; i < eROS_QOS_PRIORITY_MAX-1; i++)
		{
			qos_bw_ctrl_data->last_bw[i] = qos_bw_ctrl_data->real_bw[i];
			qos_bw_ctrl_data->allowed_bw[i] = QOS_BW_MAXIMUM_DEFAULT;
			if (drop_index < 0 && qos_bw_ctrl_data->real_bw[i] != 0 && i != eROS_QOS_PRIORITY_MAX) 
			{
				if (qos_bw_ctrl_data->real_bw[i] < gqos_bw_minimum[i]) 
				{
					qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_MINIMUM;
					qos_bw_ctrl_data->allowed_bw[i] = gqos_bw_minimum[i];
					qos_bw_ctrl_data->limit_bw[i] = 0;
					aos_eng_log(eAosMD_QoS, "%s, jiffies %u, index %d, limit_bw %u", 
								__FUNCTION__, QOS_GET_JIFFIES(), i, qos_bw_ctrl_data->limit_bw[i]);	
				}
				else
				{
					if (qos_bw_ctrl_data->real_bw[i] < sgbw_detax)
					{
						qos_bw_ctrl_data->allowed_bw[i] = gqos_bw_minimum[i];
						drop_index = i;
					}
					else
						qos_bw_ctrl_data->allowed_bw[i] = qos_bw_ctrl_data->real_bw[i] - sgbw_detax;

					if (qos_bw_ctrl_data->allowed_bw[i] < gqos_bw_minimum[i]) 
					{
						qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_MINIMUM;
						qos_bw_ctrl_data->allowed_bw[i] =  gqos_bw_minimum[i];
					}
					else
					{
						qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_DROP;
						drop_index = i;
					}
				}
			}
		}

		if (drop_index < 0)
		{
aos_eng_log(eAosMD_QoS, "liqin %s, can not find drop_index", __FUNCTION__);	
			// QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_SINGLE);
			qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
			return eAosRc_Success;
		}

aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, real_bw %u, allow_bw %u", __FUNCTION__, QOS_GET_JIFFIES(),
							drop_index, qos_bw_ctrl_data->real_bw[drop_index], qos_bw_ctrl_data->allowed_bw[drop_index]);

		qos_bw_ctrl_data->tunning_turns++;	// tunning is done
		qos_bw_ctrl_data->drop_index = drop_index;*/

		qos_do_limit(qos_bw_ctrl_data);

		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
		return eAosRc_Success;
	}

aos_eng_log(eAosMD_QoS, "liqin second enter %s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	

	// reenter this state
	// Sample Start
	// check to see whether high level bandwith is glowing
	if (qos_bw_is_high_glow(qos_bw_ctrl_data) > 0)
	{
		// decrease low level bw, reserved bw for high level flow
	/*	if ((qos_bw_ctrl_data->real_high_throughout-qos_bw_ctrl_data->last_high_throughout) >
				qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index])
		{
			qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index] = 0;
			qos_bw_limit(qos_bw_ctrl_data, sgbw_detax);
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, real_tot_high %u, last_tot_high %u, limit_bw %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->drop_index, 
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout,
						qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index]);
		}
		else
		{
			qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index]
				-= (qos_bw_ctrl_data->real_high_throughout - qos_bw_ctrl_data->last_high_throughout);
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, real_tot_high %u, last_tot_high %u, limit_bw %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->drop_index, 
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout,
						qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index]);
		}*/
		
		qos_bw_limit(qos_bw_ctrl_data, sgbw_detax);
		
		//qos_bw_decrease_low(qos_bw_ctrl_data, qos_bw_ctrl_data->drop_index);
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2_DECREASE_LOW);
		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
		return eAosRc_Success;
		
	}

	// no high level flow bandwidth is increased
	// if low level bandwidith is sacrified before, then recover it step by step
	// qos_bw_ctrl_data->tunning_turns--;
	// QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2_TUNE_LOW);

	// do nothing at this time, keep throughout control for low level
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2_DECREASE_LOW);
	return eAosRc_Success;
}

int qos_bw_state_single(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	// aos_eng_log(eAosMD_QoS, "%s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());	
	
	if (!qos_bw_ctrl_data->state_start_time)
	{
		qos_bw_ctrl_data->state_start_time = QOS_GET_JIFFIES();
	}	

	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;

		// if single level flows exist then go to single state
	if (!is_multi_level_exist(qos_bw_ctrl_data))
	{
		if ((QOS_GET_JIFFIES() - qos_bw_ctrl_data->state_start_time) >= 
			(QOS_BW_SINGLE_STABLE_TIMES*sgbw_p2_sample_iv) )
		{
			// QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P3);
			return eAosRc_Success;
		}
	}
	else
	{
		aos_eng_log(eAosMD_QoS, "%s, jiffies %u, change to P1", __FUNCTION__, QOS_GET_JIFFIES());	
		// change state to p2
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);
	}
	return eAosRc_Success;
}

int qos_bw_state_p2_decrease_low(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int is_high_increased;
	
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_tot_thro %u, tot_bw %u", __FUNCTION__, QOS_GET_JIFFIES(),
						qos_bw_ctrl_data->last_total_throughout, qos_bw_ctrl_data->total_bw);
	// check if total through is dropped a lot than total bw
	// then change state to P1
	if (0 > qos_bw_compare_precision(qos_bw_ctrl_data->real_total_throughout, 
					                  qos_bw_ctrl_data->total_bw, 
									  sgbw_total_precision))  
	{
aos_eng_log(eAosMD_QoS, "%s real total dropped too much, jiffies %u, rel_tot_thro %u, lst_tot_throu %u, tot_bw %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->real_total_throughout, 
						qos_bw_ctrl_data->last_total_throughout, qos_bw_ctrl_data->total_bw);

		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv; // milli second		
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);
		return eAosRc_Success;
	}

	// reenter this state
	// Sample Start
	// check to see whether high level bandwith is glowing

	is_high_increased = qos_bw_is_high_glow(qos_bw_ctrl_data); 
	
	if (is_high_increased > 0)
	{
		// decrease low level bw, reserved bw for high level flow
/*		if (((qos_bw_ctrl_data->real_high_throughout-qos_bw_ctrl_data->last_high_throughout) >
				qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index]) ||
			 qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index] < (sgbw_detax>>1))
		{
			qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index] = 0;
			qos_bw_limit(qos_bw_ctrl_data, sgbw_detax);
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, real_tot_high %u, last_tot_high %u, limit_bw %u, allow_bw %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->drop_index, 
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout,
						qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index],
						qos_bw_ctrl_data->allowed_bw[qos_bw_ctrl_data->drop_index]);
		}
		else
		{
			qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index]
				-= (qos_bw_ctrl_data->real_high_throughout - qos_bw_ctrl_data->last_high_throughout);
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, real_tot_high %u, last_tot_high %u, limit_bw %u, allow_bw %u",
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->drop_index, 
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->last_high_throughout,
						qos_bw_ctrl_data->limit_bw[qos_bw_ctrl_data->drop_index],
						qos_bw_ctrl_data->allowed_bw[qos_bw_ctrl_data->drop_index]);
		}
*/

		qos_bw_limit(qos_bw_ctrl_data, sgbw_detax);

		// qos_bw_decrease_low(qos_bw_ctrl_data, qos_bw_ctrl_data->drop_index);
		qos_bw_ctrl_data->state_stable_times = 0;
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2_DECREASE_LOW);
		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;
		return eAosRc_Success;
		
	}

	// high level throughout is not changed
	if (is_high_increased == 0)
	{
		qos_bw_ctrl_data->state_stable_times ++;

	aos_eng_log(eAosMD_QoS, "%s, jiffies %u, state_stable_times %d", 
		__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->state_stable_times);	
			
		if (qos_bw_ctrl_data->state_stable_times >= sgbw_stable_times)
		{
			// set total bandwidth
			qos_bw_set_total_bw(qos_bw_ctrl_data);
			
			// at this time total bandwidth is already detected, 
			// so we can get total bandwdith and decided the low bandwidth
			// and change state to p3
			
			qos_bw_set_low_bw(qos_bw_ctrl_data);
			qos_bw_ctrl_data->state_high_throughout = qos_bw_ctrl_data->real_high_throughout;
			QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P3);
			
			qos_bw_ctrl_data->state_stable_times = 0;
		}
			
	}
	
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;

	return eAosRc_Success;
}

static int qos_bw_state_p3(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	u32 bw;
	int i, ret;
	
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, real_high_throughout %u, state_high_throughout %u",
			__FUNCTION__, QOS_GET_JIFFIES(),
			qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);

	// check if total through is dropped a lot than total bw
	// then change state to P1
	if (0 > qos_bw_compare_precision(qos_bw_ctrl_data->real_total_throughout, 
					                  qos_bw_ctrl_data->total_bw, 
									  sgbw_total_precision))  
	{
aos_eng_log(eAosMD_QoS, "%s real total dropped too much, jiffies %u, rel_tot_thro %u, lst_tot_throu %u, tot_bw %u", 
						__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->real_total_throughout, 
						qos_bw_ctrl_data->last_total_throughout, qos_bw_ctrl_data->total_bw);

		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv; // milli second		
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);
		return eAosRc_Success;
	}
	
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_p2_sample_iv;

/*
	// single level exist , change status
	if (!is_multi_level_exist(qos_bw_ctrl_data))
	{ 
		qos_bw_ctrl_data->state_stable_times++;

		aos_eng_log(eAosMD_QoS, "%s, jiffies %u, only one level flows exist, stable times %d",
				__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->state_stable_times);
		
		if( qos_bw_ctrl_data->state_stable_times > 2*sgbw_stable_times)
		{

			aos_eng_log(eAosMD_QoS, "%s, jiffies %u, change state to P1", __FUNCTION__, QOS_GET_JIFFIES());
		
			QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);
			return eAosRc_Success;
		}
	}
	else
		qos_bw_ctrl_data->state_stable_times = 0;
*/		
		
	ret = qos_bw_compare(qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);

	// high is glowing
	if (qos_bw_ctrl_data->real_high_throughout > qos_bw_ctrl_data->state_high_throughout)
	{
		bw = qos_bw_ctrl_data->real_high_throughout - qos_bw_ctrl_data->state_high_throughout;
		if (bw >= sgbw_reserved_bw)
		{
aos_eng_log(eAosMD_QoS, "%s high glowing excced reserved bw, jiffies %u, real_high_throughout %u, state_high_throughout %u"
						"glowed bw %u",	__FUNCTION__, QOS_GET_JIFFIES(),qos_bw_ctrl_data->real_high_throughout, 
						qos_bw_ctrl_data->state_high_throughout, bw);

			// decrease the low level flow TBD
			ret = qos_bw_dec_low2(qos_bw_ctrl_data, qos_bw_ctrl_data->drop_index, bw);
			// reset the p3 total high
			qos_bw_ctrl_data->state_high_throughout  = qos_bw_ctrl_data->real_high_throughout;

			if (ret) // no enough space to reserved for high
			for (i = 0; i < eROS_QOS_PRIORITY_MAX-1; i ++)
			{
				if ( !(i != qos_bw_ctrl_data->drop_index && qos_bw_ctrl_data->allowed_bw[i] > gqos_bw_minimum[i]))
					continue;
				// we found this level which we can decrease it for high level
aos_eng_log(eAosMD_QoS, "%s change decreased level, jiffies %u, level %d, allowed_bw %u" ,
			__FUNCTION__, QOS_GET_JIFFIES(), i, qos_bw_ctrl_data->allowed_bw[i]);

				QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P2);
				return eAosRc_Success;
				
			}	
				
		}
		return eAosRc_Success;
	}
	else {
		bw = qos_bw_ctrl_data->state_high_throughout - qos_bw_ctrl_data->real_high_throughout;
		if (bw >= sgbw_reserved_bw)
		{
aos_eng_log(eAosMD_QoS, "%s high decreased excceed reserved bw, jiffies %u, rel_hig_throu %u, state_hig_throu %u"
						"glowed bw %u",	__FUNCTION__, QOS_GET_JIFFIES(),qos_bw_ctrl_data->real_high_throughout, 
						qos_bw_ctrl_data->state_high_throughout, bw);
			// recover the low level
			qos_bw_inc_low2(qos_bw_ctrl_data, qos_bw_ctrl_data->drop_index, sgbw_reserved_bw);

			// reset the state total high
			qos_bw_ctrl_data->state_high_throughout  = qos_bw_ctrl_data->real_high_throughout;
			return eAosRc_Success;
		}
	}

aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_tot_throu %u, total_bw %u" ,
			__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->real_total_throughout, qos_bw_ctrl_data->total_bw);

	// if total bandwidth lowed a lot, or total bandwidth life expired
	//  then change state to P1
	if (((qos_bw_ctrl_data->real_total_throughout + 4*sgbw_reserved_bw) < qos_bw_ctrl_data->total_bw) ||
	     is_total_bw_life_expired(qos_bw_ctrl_data, QOS_GET_JIFFIES()))
	{
aos_eng_log(eAosMD_QoS, "%s change to P1 state, jiffies %u, bw_jiffies %u, life %u" ,
			__FUNCTION__, QOS_GET_JIFFIES(), qos_bw_ctrl_data->total_bw_jiffies, sgbw_total_bw_life);
	
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P1);		
	}

	return eAosRc_Success;
}
// 
// In phrase one: there is no any bandwidth control for all the flows
// In phrase two:
// 		retrieve the realtime bandwiths for priority based flows
//		sacrify bandwidth for low priority flows, reserved to high priority flows.
//		a. If there is no 
void ros_qos_bw_ctrl_timer(unsigned long data)
{
	int ret;
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	
	qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)data;
	if (!qos_bw_ctrl_data)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "QoS bandwidth Manager timer error, flow control is null");
		return;
	}
	
	//aos_trace("%s, ifid %d, direction %d, qos_bw_ctrl_data 0x%02x", 
	//		__FUNCTION__, qos_bw_ctrl_data->ifid, qos_bw_ctrl_data->direction, qos_bw_ctrl_data);	
	
	if (qos_bw_ctrl_data_put(qos_bw_ctrl_data) == 1)
	{
		aos_eng_log(eAosMD_QoS, "%s, qos bw ctrl data is free", __FUNCTION__);
		return;
	}
	
	// whether the ctrl is on for this interface and direction
	if (!qos_bw_ctrl_data->bw_ctrl)
	{
		aos_min_log(eAosMD_QoS, "bw ctrl off on ifid %d direction %d", qos_bw_ctrl_data->ifid, qos_bw_ctrl_data->direction);
		return;
	}
	
	ret = eAosRc_Continue;
	qos_bw_get_realtime_bw(qos_bw_ctrl_data);
	
	while( ret == eAosRc_Continue)
	{
		switch (qos_bw_ctrl_data->state)
		{
			case eROS_QOS_BW_STATE_INIT:
				ret = qos_bw_state_init(qos_bw_ctrl_data);
				break;

			case eROS_QOS_BW_STATE_P1:
				ret = qos_bw_state_p1(qos_bw_ctrl_data);	
				break;
			
			case eROS_QOS_BW_STATE_P2:
				ret = qos_bw_state_p2(qos_bw_ctrl_data);	
				break;
	
			case eROS_QOS_BW_STATE_SINGLE:
				ret = qos_bw_state_single(qos_bw_ctrl_data);
				break;

			case eROS_QOS_BW_STATE_P2_DECREASE_LOW:
				ret = qos_bw_state_p2_decrease_low(qos_bw_ctrl_data);
				break;

			case eROS_QOS_BW_STATE_P3:
				ret = qos_bw_state_p3(qos_bw_ctrl_data);	
				break;

			case eROS_QOS_BW_STATE_P20:
				ret = aos_qos_p20(qos_bw_ctrl_data);	
				break;
			case eROS_QOS_BW_STATE_P21:
				ret = aos_qos_p21(qos_bw_ctrl_data);	
				break;
			case eROS_QOS_BW_STATE_P24:
				ret = aos_qos_p24(qos_bw_ctrl_data);	
				break;
			case eROS_QOS_BW_STATE_P30:
				ret = aos_qos_p30(qos_bw_ctrl_data);	
				break;

			default:
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "qos bw control, no such state");
				ret = eAosRc_Success;
				break;
			
	 	}		
	}
	
	qos_bw_set_all_last_bw(qos_bw_ctrl_data);
	qos_bw_ctrl_data_hold(qos_bw_ctrl_data);
	add_timer(&(qos_bw_ctrl_data->timer));
	 	
}

int qos_bw_limit(struct qos_bw_ctrl_data *qos_bw_ctrl_data, u32 bw_change)
{
	int drop_index;
	int limit_too_much;

	drop_index = qos_bw_ctrl_data->drop_index;
aos_eng_log(eAosMD_QoS, "%s enter, jiffies %u, drop_index %d, bw_change %u, allowed_bw %u, limit_bw %u",
		__FUNCTION__, QOS_GET_JIFFIES(), drop_index, bw_change,
		qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index]);

	limit_too_much = 0;

	// allowed_bw < gqos_bw_minimum
	if (qos_bw_ctrl_data->allowed_bw[drop_index] < gqos_bw_minimum[drop_index])
	{
		limit_too_much = 1;
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
			
aos_eng_log(eAosMD_QoS, "%s exit, limit too much, allowed_bw %u, limit_bw %u", __FUNCTION__, 
		qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index]);

		return limit_too_much;
	}

	// allowed_bw < bw_change
	if (qos_bw_ctrl_data->allowed_bw[drop_index] < bw_change)
	{
		limit_too_much = 1;
		qos_bw_ctrl_data->limit_bw[drop_index] += qos_bw_ctrl_data->allowed_bw[drop_index]-gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
			
aos_eng_log(eAosMD_QoS, "%s exit, limit too much, allowed_bw %u, limit_bw %u", __FUNCTION__, 
		qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index]);

		return limit_too_much;
	}

	// allowed_bw >= bw_change
	qos_bw_ctrl_data->allowed_bw[drop_index] -= bw_change;
	if (qos_bw_ctrl_data->allowed_bw[drop_index] < gqos_bw_minimum[drop_index])
	{
		limit_too_much = 1;
		qos_bw_ctrl_data->limit_bw[drop_index] += 
			(qos_bw_ctrl_data->allowed_bw[drop_index]+bw_change) - gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->allowed_bw[drop_index] = gqos_bw_minimum[drop_index];
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_MINIMUM;
			
aos_eng_log(eAosMD_QoS, "%s exit, limit too much, allowed_bw %u, limit_bw %u", __FUNCTION__, 
		qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index]);

		return limit_too_much;
	}
	else
	{
		qos_bw_ctrl_data->limit_bw[drop_index] += bw_change;
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_DROP;
	}

aos_eng_log(eAosMD_QoS, "%s exit, allowed_bw %u, limit_bw %u", __FUNCTION__, 
		qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index]);

	return limit_too_much;
}


int qos_do_limit(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int drop_found;
	int drop_index, i;

	drop_found = 0;
	drop_index = qos_bw_ctrl_data->drop_index;

aos_eng_log(eAosMD_QoS, "%s enter", __FUNCTION__);

	for (i = 0; i < eROS_QOS_PRIORITY_MAX-1; i++)
	{
		if (qos_bw_ctrl_data->limit_bw[i] != 0 && drop_index == i) {
			drop_found = 1;
			break;
		}
	}

	if (!drop_found)
	{
		
aos_eng_log(eAosMD_QoS, "%s find drop index", __FUNCTION__);

		// retrieve the realtime bw for all levels
		// decrease the lowest flow by detaX
		drop_index = -1;
		for (i = 0; i < eROS_QOS_PRIORITY_MAX-1; i++)
		{
			qos_bw_ctrl_data->allowed_bw[i] = QOS_BW_MAXIMUM_DEFAULT;
			if (drop_index < 0 && qos_bw_ctrl_data->real_bw[i] != 0) 
			{
				if (qos_bw_ctrl_data->real_bw[i] < gqos_bw_minimum[i]) 
				{
					qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_MINIMUM;
					qos_bw_ctrl_data->allowed_bw[i] = gqos_bw_minimum[i];
				}
				else
				{
					qos_bw_ctrl_data->allowed_bw[i] = qos_bw_ctrl_data->real_bw[i];
					qos_bw_ctrl_data->ctrl[i] = eROS_QOS_FLOW_CTRL_DROP;
					drop_index = i;
				}
			}
		}
	}

	if (drop_index < 0)
	{
		aos_eng_log(eAosMD_QoS, "%s can not find drop index", __FUNCTION__);
		return drop_index;
	}

	return qos_bw_limit(qos_bw_ctrl_data, sgbw_detax);
}	

int qos_reduce_limit(struct qos_bw_ctrl_data *qos_bw_ctrl_data, u32 bw_change)
{
	int drop_index;

	drop_index = qos_bw_ctrl_data->drop_index;

aos_eng_log(eAosMD_QoS, "%s enter, drop_index %d, allowed_bw %u, limit_bw %u, bw_change %u", __FUNCTION__, drop_index,
						qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index], bw_change);

	if (qos_bw_ctrl_data->allowed_bw[drop_index] > (QOS_BW_MAXIMUM_DEFAULT - bw_change))
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "allowed_bw %u will be large than default max %u if plus bw_change %u",
					qos_bw_ctrl_data->allowed_bw[drop_index], QOS_BW_MAXIMUM_DEFAULT, bw_change);
		return 0;
	}

	qos_bw_ctrl_data->allowed_bw[drop_index] += bw_change;
	
	if (qos_bw_ctrl_data->allowed_bw[drop_index] > gqos_bw_minimum[drop_index])
		qos_bw_ctrl_data->ctrl[drop_index] = eROS_QOS_FLOW_CTRL_DROP;
	
	if (qos_bw_ctrl_data->limit_bw[drop_index] > bw_change)
		qos_bw_ctrl_data->limit_bw[drop_index] -= bw_change;
	else
		qos_bw_ctrl_data->limit_bw[drop_index] = 0;
		
aos_eng_log(eAosMD_QoS, "%s exit, drop_index %d, allowed_bw %u, limit_bw %u, bw_change %u", __FUNCTION__, drop_index,
						qos_bw_ctrl_data->allowed_bw[drop_index], qos_bw_ctrl_data->limit_bw[drop_index], bw_change);

	return 0;
}

int aos_qos_p20(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int limit_too_much;

aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_hig_throu %u, state_hig_throu %u",
						__FUNCTION__, QOS_GET_JIFFIES(),
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);

	// 
	// When this function is called:
	// 1. If there is only one type of traffic, do nothing. 
	// 2. We will reduce the low bandwidth usage to see whether
	//    the high priority traffic grows up. 
	// 3. Wait for a while
	// 4. Check it again
	//
	// We assume when this function is called:
	//     aos_p1_allowed = aos_p1_band;
	//
	
	if (!is_multi_level_exist(qos_bw_ctrl_data))
	{
		// go to P3;
aos_eng_log(eAosMD_QoS, "%s single level, jiffies %u, change from P20 to P30", __FUNCTION__, QOS_GET_JIFFIES());

		qos_bw_ctrl_reset_data(qos_bw_ctrl_data);
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P30);
		return eAosRc_Continue;
	}

	// 
	// Reduce Priority 1 usage
	//
		
	limit_too_much = qos_do_limit(qos_bw_ctrl_data);
	if (limit_too_much == 1)
	{
		// 
		// Can't limit anymore. That's all we can do
		//
		// go to P3;
aos_eng_log(eAosMD_QoS, "%s limit too much, jiffies %u, change from P20 to P30", __FUNCTION__, QOS_GET_JIFFIES());
		QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P30);
		return eAosRc_Continue;
	}

	// start timer: T20; state = p21;
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_t20_iv;
	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P21);
		
	return 0;
}


int aos_qos_p21(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{

aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_hig_throu %u, state_hig_throu %u",
						__FUNCTION__, QOS_GET_JIFFIES(),
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);
	// 
	// We reduced low priority bandwidth and waited for a while
	// Now, we need to check it.
	//
	// if (high[i] increased)
	if (qos_bw_is_high_glow(qos_bw_ctrl_data) > 0)
	{	
		// 
		// This means that we need to limit more. 
		//
		
		// limit more;
		if (qos_bw_limit(qos_bw_ctrl_data, sgbw_detax) == 1)
		{
			// limit too much
			// go to P30
aos_eng_log(eAosMD_QoS, "%s, change from P21 to P3", __FUNCTION__);
			QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P30);
			return eAosRc_Continue;
		}
			
		// start timer: T21; state = p21;
		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_t21_iv;
		return eAosRc_Success;
	}

	// 
	// This means all high priority stopped climing. 
	// We will wait for another time to determine
	// whether they really stopped climing. 
	//
	
	// start timer T24; state = P24;
	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_t24_iv;
	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P24);

	return 0;
}


int aos_qos_p24(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{
	int drop_index;
	
	drop_index = qos_bw_ctrl_data->drop_index;
aos_eng_log(eAosMD_QoS, "%s, jiffies %u, drop_index %d, rel_hig_throu %u, state_hig_throu %u",
						__FUNCTION__, QOS_GET_JIFFIES(), drop_index,
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);
	// 
	// We reduced the limit, and waited T3, now we need
	// to check whether any high priority starts dropping. 
	// If yes, we found the point. 
	//
	
	// if (no high priority dropping)
	if (qos_bw_is_high_glow(qos_bw_ctrl_data) >= 0)
	{
		// if (limit becomes 0)
		if (qos_bw_ctrl_data->limit_bw[drop_index] == 0)
		{
			// 
			// This means the total bandwidth is not over used. 
			// All traffic can run.
			//
			// state = P3;	goto P3;
aos_eng_log(eAosMD_QoS, "%s, change from P24 to P3", __FUNCTION__);
			QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P30);
			return eAosRc_Continue;
		}

		// continue reduce the limit;
		// start timer T24;
		qos_reduce_limit(qos_bw_ctrl_data, sgbw_detax);
		qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_t24_iv;
		return 0;
	}

	// 
	// This means some high priority starts dropping. 
	// We need to go back to its previous limitation.
	//
	// restore the previous limit + some reserved;
	// state = P3;	goto P3;
	qos_bw_limit(qos_bw_ctrl_data, sgbw_detax+sgbw_reserved_bw);
	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P30);
	return eAosRc_Continue;

}

int aos_qos_p30(struct qos_bw_ctrl_data *qos_bw_ctrl_data)
{

aos_eng_log(eAosMD_QoS, "%s, jiffies %u, rel_hig_throu %u, state_hig_throu %u",	__FUNCTION__, QOS_GET_JIFFIES(),
						qos_bw_ctrl_data->real_high_throughout, qos_bw_ctrl_data->state_high_throughout);

	qos_bw_ctrl_data->timer.expires = QOS_GET_JIFFIES() + sgbw_t30_iv;
	QOS_BW_NEXT_STATE(qos_bw_ctrl_data, eROS_QOS_BW_STATE_P20);
	return eAosRc_Success;
}

// CLI for QoS Bandwidth Management

// set parameter for phrase 1
int ros_qos_bw_set_p1_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos bw set p1 param <interval>
	//
	int interval = parms->mIntegers[0];

	*length = 0;

	sgbw_p1_sample_iv = interval;
	
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

// set parameter for phrase 2
int ros_qos_bw_set_p2_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos bw set p2 param <sampling_interval>  <single_sampling_times> <stable_sampling_times> 
	//
	int sampling_interval = parms->mIntegers[0];
	int single_sampling_times = parms->mIntegers[1];
	int stable_sampling_times = parms->mIntegers[2];

	*length = 0;
	
	if (single_sampling_times <= 0 || single_sampling_times > 255)
	{
		sprintf(errmsg, "single sampling times should be between 1 and 255");
		return -1;
	}
	
	if (stable_sampling_times <= 0 || stable_sampling_times > 255)
	{
		sprintf(errmsg, "single sampling times should be between 1 and 255");
		return -1;
	}

	sgbw_p2_sample_iv = sampling_interval;
	sgqos_single_samples = (u8)single_sampling_times;
	sgbw_stable_times = (u8) stable_sampling_times;
	
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

// set parameter for phrase 3
int ros_qos_bw_set_p3_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos bw set p1 param <interval>
	//
	int interval = parms->mIntegers[0];

	*length = 0;

	sgbw_p3_iv = interval;
	
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

// set parameter for phrase 1
int ros_qos_bw_set_detx_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// 
	// qos bw set p1 param <interval>
	//
	int detax = parms->mIntegers[0];

	*length = 0;

	sgbw_detax = detax;
	
	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

int ros_qos_bw_manager_status_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	char *status = parms->mStrings[0];

	*length = 0;

	if (strcmp(status, "on") == 0)
		sgbw_ctrl = 1;
	else
		sgbw_ctrl = 0;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

int qos_change_bw_ctrl(void *k, void *d, void *arg)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	u8 *ctrl;

	ctrl = (u8*)arg;

	if (!ctrl) return 1;

	qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)d;
	
	if(qos_bw_ctrl_data->bw_ctrl != *ctrl)
	{
		qos_bw_ctrl_data->bw_ctrl = *ctrl;
#ifdef __KERNEL__
		if (ctrl) del_timer(&(qos_bw_ctrl_data->timer));
		qos_bw_ctrl_data->state = eROS_QOS_BW_STATE_INIT;
#endif
	}

	return 0;
}

int ros_qos_bw_set_ctrl_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	struct net_device *dev;
	char *devname = parms->mStrings[0];
	char *direction = parms->mStrings[1];
	char *status = parms->mStrings[2];
	int ifid, direct;
	u8 priority, ctrl;
	int ret = eAosRc_Success;

	*length = 0;

	if (strcmp(devname, "all") != 0) // all means all the interface dev
	{
		if(is_valid_if(devname, &ifid))
		{
			sprintf(errmsg, "dev %s not found\n", devname);			
			return -eAosRc_DevNotFound;
		}
	}

	priority = 0;
	
	if (strcmp(direction, "in") == 0) 
		direct = 0;
	else if (strcmp(direction, "out") == 0)
		direct = 1;
 	else {
		sprintf(errmsg, "direction error\n");
		return -eAosRc_InvalidDirection;	
	}
	
	if (strcmp(status, "on") == 0)
		ctrl = 1;
	else
		ctrl = 0;

#ifdef __KERNEL__
	local_bh_disable();
#endif
	
	for (dev = dev_base; dev; dev = dev->next)		
	{
		aos_trace("dev info, name: %s, ifindex: %d, type: %d", dev->name, dev->ifindex, dev->type);

		if (dev->type != 1) continue;
		if ((strcmp(devname, "all") != 0) && dev->ifindex != ifid) continue;
		
		pri_create_hashkey(dev->ifindex, direct, priority, &k);
		qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)hashtab_search(qos_traffic_hash_pri, &k);
		if (qos_bw_ctrl_data == NULL)
		{
			ret = build_pri_ch(dev->ifindex, direct, &qos_bw_ctrl_data, &k);
			if (ret != 0) 
			{
				sprintf(errmsg, "failed to create priority based hash entry");
				aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Failed to build priority cell, ret %d", ret);
				ret = -eAosRc_Failed;
				goto out;
			}
		}
		
		qos_change_bw_ctrl(NULL, (void*)qos_bw_ctrl_data, (void*)(&ctrl));

	}

out:
	
#ifdef __KERNEL__
	local_bh_enable();
#endif

	ros_qos_config_log(parms->mCmd);
	return ret;
}

int ros_qos_bw_set_param_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	int avr_shift = parms->mIntegers[0];
	int eq_throld = parms->mIntegers[1];
	int reserved_bw = parms->mIntegers[2];
	int life = parms->mIntegers[3];
	int bw_precision = parms->mIntegers[4];
	int total_bw_precision = parms->mIntegers[5];

	*length = 0;

	if (avr_shift <= 0)
	{
		sprintf(errmsg, "average shift should be larger than zero\n");
		return -1;
	}
	
	if (eq_throld <= 0)
	{
		sprintf(errmsg, "bandwidth equal throld should be larger than zero\n");
		return -1;
	}
	
	if (reserved_bw <= 0)
	{
		sprintf(errmsg, "reserved bandwidth should be larger than zero\n");
		return -1;
	}
	
	if (life <= 0)
	{
		sprintf(errmsg, "Total bandwidth life should be larger than zero\n");
		return -1;
	}

	if (bw_precision <= 0)
	{
		sprintf(errmsg, "Bandwidth comparation precision shift should be larger than zero\n");
		return -1;
	}
	
	if (total_bw_precision <= 0)
	{
		sprintf(errmsg, "Total Bandwidth comparation precision shift should be larger than zero\n");
		return -1;
	}

	sgbw_avr_shift = avr_shift; 	// average throughouts calculate shift
	sgbw_eq_throld = eq_throld;		// two bandwidth equal throld
	sgbw_reserved_bw = reserved_bw;	// reserved bandwidth
	sgbw_total_bw_life = life;		// total bandwidth valid life span
	sgbw_cmp_precision = bw_precision;// bandwidth comparation precsion shift;
	sgbw_total_precision = total_bw_precision;// bandwidth comparation precsion shift;
	
	ros_qos_config_log(parms->mCmd);
	return 0;
}

int ros_qos_bw_set_interval_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	int t20 = parms->mIntegers[0];
	int t21 = parms->mIntegers[1];
	int t24 = parms->mIntegers[2];
	int t30 = parms->mIntegers[3];

	*length = 0;

	if (t20 <= 0)
	{
		sprintf(errmsg, "P20 interval should be large than zero\n");
		return -1;
	}
	
	if (t21 <= 0)
	{
		sprintf(errmsg, "P21 interval should be large than zero\n");
		return -1;
	}
	
	if (t24 <= 0)
	{
		sprintf(errmsg, "P24 interval should be large than zero\n");
		return -1;
	}
	
	if (t30 <= 0)
	{
		sprintf(errmsg, "P30 interval should be large than zero\n");
		return -1;
	}
	
	sgbw_t20_iv = t20;
	sgbw_t21_iv = t21;
	sgbw_t24_iv = t24;
	sgbw_t30_iv = t30;
	
	ros_qos_config_log(parms->mCmd);
	return 0;
}

int ros_qos_bw_set_mini_band_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	int miniband = parms->mIntegers[0];
	int i;

	*length = 0;

	if (miniband <= 0)
	{
		sprintf(errmsg, "Minimum bandwidth should be large than zero\n");
		return -eAosRc_InvalidMiniBand;
	}

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
		gqos_bw_minimum[i] = miniband;

	return eAosRc_Success;
}	

int qos_bw_man_show_config(char *rsltBuff, unsigned int *rsltIndex, int optlen)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	struct net_device *dev;
	char local[200];
	int i;
	
	sprintf(local, "QoS Bandwidth Manager: %s\n", sgbw_ctrl?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	for (dev = dev_base; dev; dev = dev->next)		
	{
		if (dev->type != 1) continue;
		
		for (i = 0; i < 2; i++)
		{
			pri_create_hashkey(dev->ifindex, i, 0, &k);
		
			qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)hashtab_search(qos_traffic_hash_pri, &k);
			if (qos_bw_ctrl_data != NULL && qos_bw_ctrl_data->bw_ctrl)
			{
				sprintf(local,"QoS Bandwidth Manager control: %s %s on\n", dev->name, i? "out":"in");
				aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
			}
		}
		
	}

	sprintf(local, "Detax: %d\n", sgbw_detax);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Phrase One sampling interval: %d ms\n", sgbw_p1_sample_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Phrase One single state sampling times: %d\n", sgqos_single_samples);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Phrase Two sampling interval: %d ms\n", (int)sgbw_p2_sample_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Phrase Three interval: %lu ms\n", sgbw_p3_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Phrase stable sampling times: %d\n", sgbw_stable_times);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Average bandwidth calculation time shift: %d\n", sgbw_avr_shift);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Bandwidth comparation equal throld: %u bytes\n", sgbw_eq_throld);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Bandwidth comparation precison shift: %u\n", sgbw_cmp_precision);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Total Bandwidth comparation precison shift: %u\n", sgbw_total_precision);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Reserved bandwidth: %u bytes\n", (unsigned int)sgbw_reserved_bw);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Total bandwidth life: %u ms\n", (unsigned int)sgbw_total_bw_life);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "P20 interval: %lu ms\n", sgbw_t20_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	sprintf(local, "P21 interval: %lu ms\n", sgbw_t21_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	sprintf(local, "P24 interval: %lu ms\n", sgbw_t24_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	sprintf(local, "P30 interval: %lu ms\n", sgbw_t30_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Minimum bandwidth: %lu bytes\n", gqos_bw_minimum[0]);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	return eAosRc_Success;
}

int ros_qos_bw_show_param_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);

	qos_bw_man_show_config(rsltBuff, &rsltIndex, optlen);
	
	*length = rsltIndex;
	return eAosRc_Success;
}

static int qos_bw_ctrl_register_cli(void)
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
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetP1Param", ros_qos_bw_set_p1_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetP2Param", ros_qos_bw_set_p2_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetP3Param", ros_qos_bw_set_p3_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetDetxBw", ros_qos_bw_set_detx_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerStatus", ros_qos_bw_manager_status_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetCtrl", ros_qos_bw_set_ctrl_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerShow", ros_qos_bw_show_param_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetParam", ros_qos_bw_set_param_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetInterval", ros_qos_bw_set_interval_cli);
		ret |= OmnKernelApi_addCliCmd("QosBWManagerSetMiniBand", ros_qos_bw_set_mini_band_cli);
		lsRulesAdded = 1;
	}
	return ret;
}

int qos_bw_man_save_config(char *rsltBuff, unsigned int *rsltIndex, int optlen)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	struct net_device *dev;
	char local[128];
	int i;

	aos_min_log(eAosMD_QoS, "%s", __FUNCTION__);

	sprintf(local,"<Cmd>qos bandwidth manager set p1 %d</Cmd>\n", sgbw_p1_sample_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local,"<Cmd>qos bandwidth manager set p2 %d %d %d</Cmd>\n", 
			(int)sgbw_p2_sample_iv, sgqos_single_samples, sgbw_stable_times);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local,"<Cmd>qos bandwidth manager set p3 %d</Cmd>\n", (int)sgbw_p3_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local,"<Cmd>qos bandwidth manager set detx %d</Cmd>\n", sgbw_detax);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local,"<Cmd>qos bandwidth manager set param %d %d %d %d %d %d</Cmd>\n", 
			sgbw_avr_shift, sgbw_eq_throld, sgbw_reserved_bw, sgbw_total_bw_life, sgbw_cmp_precision, sgbw_total_precision);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local,"<Cmd>qos bandwidth manager set interval %d %d %d %d</Cmd>\n", 
			(int)sgbw_t20_iv, (int)sgbw_t21_iv, (int)sgbw_t24_iv, (int)sgbw_t30_iv);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local,"<Cmd>qos bandwidth manager set miniband %lu</Cmd>\n", gqos_bw_minimum[0]);
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	for (dev = dev_base; dev; dev = dev->next)		
	{
		if (dev->type != 1) continue;
		
		for (i = 0; i < 2; i++)
		{
			pri_create_hashkey(dev->ifindex, i, 0, &k);
		
			qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)hashtab_search(qos_traffic_hash_pri, &k);
			if (qos_bw_ctrl_data != NULL && qos_bw_ctrl_data->bw_ctrl)
			{
				sprintf(local,"<Cmd>qos bandwidth manager ctrl %s %s on</Cmd>\n", dev->name, i? "out":"in");
				aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
			}
		}
		
	}
	
	sprintf(local,"<Cmd>qos bandwidth manager %s</Cmd>\n", sgbw_ctrl? "on" : "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	return 0;
}

int qos_bw_man_clear_config(void)
{
	struct net_device *dev;
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	int i;

	aos_min_log(eAosMD_QoS, "%s", __FUNCTION__);

	sgbw_ctrl = 0; 											// disable qos bandwidth control	

	sgbw_p1_sample_iv = 1000; 								// bandwidth control phrase one interval 3000 milli second
	sgbw_p2_sample_iv = 1000;								// phrase sample interval 125 ms 
	sgbw_p3_iv = QOS_BW_P3_INTERVAL_DEFAULT;				// bandwidth control phrase 3 interval 500 milli second default
	sgqos_single_samples = QOS_BW_SINGLE_STABLE_TIMES; 		// stable sampling times for single state
	sgbw_stable_times = QOS_BW_P2_STABLE_TIMES; 			// stable times for the sampling of phrase 2
	sgbw_detax = QOS_BW_DETA_DEFAULT;// 1k/s the minimum unit for bandwidth tuning, this can be configure through CLI
	sgbw_avr_shift = QOS_BW_AVERAGE_CALCULATE_SHIFT; 		// average throughouts calculate shift
	sgbw_cmp_precision = QOS_BW_COMPARATION_PRECISION_SHIFT;// bandwidth comparation precision shift
	sgbw_total_precision = QOS_TOTAL_BW_CMP_PRECISION_SHIFT;// bandwidth comparation precision shift
	sgbw_eq_throld = QOS_BW_EQUAL_THROLD;					// two bandwidth equal throld
	sgbw_reserved_bw = QOS_BW_RESERVED_BW;					// reserved bandwidth
	sgbw_total_bw_life = QOS_TOTAL_BW_LIFE;					// total bandwidth valid life span

	// minimum bandwidth reserved for each priority
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	 gqos_bw_minimum[i] = QOS_BW_MINIMUM_DEFAULT;

	sgbw_t20_iv = 1000;
	sgbw_t21_iv = 1000;
	sgbw_t24_iv = 1000;
	sgbw_t30_iv = 1000;

	for (dev = dev_base; dev; dev = dev->next)		
	{
		if (dev->type != 1) continue;
		
		for (i = 0; i < 2; i++)
		{
			pri_create_hashkey(dev->ifindex, i, 0, &k);
			qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)hashtab_search(qos_traffic_hash_pri, &k);
			if (qos_bw_ctrl_data) 
			{
				qos_bw_ctrl_data->bw_ctrl = 0;
				qos_bw_ctrl_reset_data(qos_bw_ctrl_data);
			}
		}
		
	}
	
	return eAosRc_Success;
}

void qos_bw_ctrl_unregister_cli(void)
{
	OmnKernelApi_delCliCmd("QosBWManagerSetP1Param");
	OmnKernelApi_delCliCmd("QosBWManagerSetP2Param");
	OmnKernelApi_delCliCmd("QosBWManagerSetP3Param");
	OmnKernelApi_delCliCmd("QosBWManagerSetDetxBw");
	OmnKernelApi_delCliCmd("QosBWManagerStatus");
	OmnKernelApi_delCliCmd("QosBWManagerSetCtrl");
	OmnKernelApi_delCliCmd("QosBWManagerShow");
	OmnKernelApi_delCliCmd("QosBWManagerSetParam");
	OmnKernelApi_delCliCmd("QosBWManagerSetInterval");
	OmnKernelApi_delCliCmd("QosBWManagerSetMiniBand");
}

int ros_qos_bw_ctrl_init(void)
{
	int i;

	// set all level of flow ctrl.
	aos_min_log(eAosMD_QoS, "QoS Bandwidth Manager Init");
	
	sgbw_ctrl = 0; // disable qos bandwidth control

	// default minimum bandwidth reserved for each level
	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
		gqos_bw_minimum[i] = QOS_BW_MINIMUM_DEFAULT; 
	
	qos_bw_ctrl_hash = hashtab_create(bw_ctrl_hash_val, bw_ctrl_hash_cmp, (2<<QOS_BW_CTRL_HASH_KEY_SIZE)-1);

	if (!qos_bw_ctrl_hash)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "failed it init bandwidth management hash table");
		return -1;
	}
	
	qos_bw_ctrl_register_cli();


	// aos_thread_create(priority_reinject_skb_thread, NULL, CLONE_KERNEL);
	
	return eAosRc_Success;
}

int qos_traffic_control(struct sk_buff *skb, int skb_len, int direction, u8 priority)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	unsigned long interval;
	int ifid;
	
	aos_assert1(priority < eROS_QOS_PRIORITY_MAX);
		
	aos_eng_log(eAosMD_QoS, "enter1 %s", __FUNCTION__);
	if (priority == eROS_QOS_PRIORITY_HIH) return NF_ACCEPT;

	aos_eng_log(eAosMD_QoS, "enter2 %s", __FUNCTION__);
	if (!IS_QOS_BW_CTRL_ON) return NF_ACCEPT;

	aos_eng_log(eAosMD_QoS, "enter3 %s", __FUNCTION__);

	ifid = skb->dev->ifindex;
	pri_create_hashkey(ifid, direction, priority, &k);
	qos_bw_ctrl_data = (struct qos_bw_ctrl_data*)hashtab_search(qos_traffic_hash_pri, &k);

	if (!qos_bw_ctrl_data)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Interface %d priority %d is not in priority hashtab",
				ifid, priority);
		return NF_ACCEPT;
	}
	else if (qos_bw_ctrl_data->state == eROS_QOS_BW_STATE_INIT && qos_bw_ctrl_data->bw_ctrl)
	{
aos_eng_log(eAosMD_QoS, "liqn %s, if %d, direction %d, priority %d", __FUNCTION__, ifid, direction, priority);
	    qos_bw_ctrl_data_hold(qos_bw_ctrl_data);
		ros_qos_bw_ctrl_timer((unsigned long)qos_bw_ctrl_data);
	}

	// do control based on bw control data
/*
	interval = QOS_GET_JIFFIES() - qos_bw_ctrl_data->last_sent_time[priority];
    if (qos_bw_ctrl_data->last_sent_time[priority] == 0)
	{
		qos_bw_ctrl_data->last_sent_time[priority] = QOS_GET_JIFFIES();
	}
	else if ((qos_bw_ctrl_data->ctrl[priority] != eROS_QOS_FLOW_CTRL_PASS) && 
			 (skb_len > (interval*qos_bw_ctrl_data->allowed_bw[priority])) )
	{
		return NF_DROP;
	}
*/

	if (qos_bw_ctrl_data->ctrl[priority] == eROS_QOS_FLOW_CTRL_PASS) return NF_ACCEPT;

	interval = QOS_GET_JIFFIES() - qos_bw_ctrl_data->last_sent_time[priority];

	if (interval > HZ)
	{
		qos_bw_ctrl_data->allowed_bytes[priority] = qos_bw_ctrl_data->allowed_bw[priority];
		qos_bw_ctrl_data->last_sent_time[priority] = QOS_GET_JIFFIES();
		qos_bw_ctrl_data->sent_bytes[priority] = 0;
	}

aos_trace("liqin %s, priority %d, skb_len %d, allowed_bw %u, "
		                 "interval %d, allowed_bytes %u, sent_bytes %u",
				__FUNCTION__, priority, skb_len, qos_bw_ctrl_data->allowed_bw[priority], interval,
				qos_bw_ctrl_data->allowed_bytes[priority], qos_bw_ctrl_data->sent_bytes[priority]);

	if (qos_bw_ctrl_data->sent_bytes[priority] > qos_bw_ctrl_data->allowed_bytes[priority] )
	{

aos_trace("liqin %s, NF_DROP, priority %d, skb_len %d, allowed_bw %u, "
		                 "interval %d, allowd_bytes %d, sent_bytes %u",
				__FUNCTION__, priority, skb_len, qos_bw_ctrl_data->allowed_bw[priority], interval,
				qos_bw_ctrl_data->allowed_bytes[priority], qos_bw_ctrl_data->sent_bytes[priority]);
		return NF_DROP;
	}

	qos_bw_ctrl_data->sent_bytes[priority] += skb_len;
		
	return NF_ACCEPT;
}

unsigned int qos_nf_queue_in(unsigned int hook,
					 struct sk_buff **pskb,
					 const struct net_device *in,
					 const struct net_device *out,
					 int (*okfn)(struct sk_buff*))
{
	u8 priority;
	int ret;
	struct ros_qos_skb_info skb_info;
	
	if (aos_qos_traffic_facility_status != AOS_QOS_TRAFFIC_STATUS_ON) return NF_ACCEPT;

	if (!IS_QOS_BW_CTRL_ON) return NF_ACCEPT;
	
	if (monitor_test_direction & monitor_direction_in){
		ret = qos_traffic_parse_parm(*pskb, &skb_info, direction_in); 
		if (ret < 0) {
			aos_trace("fail to get skb_info\n");
			return NF_QUEUE;
		}
		qos_traffic_monitor(*pskb, &skb_info, 0, &priority);

		return NF_QUEUE;
	}

	return NF_ACCEPT;
}

// return 0 means dropping
// return 1 means queue
int qos_nf_queue_handler(struct sk_buff *skb, struct nf_info *info, unsigned int queuenum, void *data)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;
	struct qos_queue_cell *queue_cell;
	u8 priority;
	struct ros_qos_skb_info skb_info;
	
	if (!skb || !info)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "skb %x or info %x is null error");
		return 0;
	}
	
	// aos_trace("enter %s", __FUNCTION__);
	qos_traffic_parse_parm(skb, &skb_info, direction_in); 
	
	priority = qos_get_priority_of_channel(&skb_info, 0, NULL);
	
	if (priority >= eROS_QOS_PRIORITY_MAX)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "priority error");
		return 0;
	}
	
	// if (priority == eROS_QOS_PRIORITY_HIH) return 1;


	pri_create_hashkey(skb->dev->ifindex, 0, priority, &k);
	qos_bw_ctrl_data = (struct qos_bw_ctrl_data*)hashtab_search(qos_traffic_hash_pri, &k);
	if (!qos_bw_ctrl_data)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Interface %d priority %d is not in priority hashtab",
				skb->dev->ifindex, priority);
		return 0;
	}

	queue_cell = aos_malloc_atomic(sizeof(struct qos_queue_cell));
	if (!queue_cell)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "malloc qos queue cell failed");
		return 0;
	}

	AOS_INIT_LIST_HEAD(&queue_cell->list);
	queue_cell->skb = skb;
	queue_cell->info = info;
//aos_min_log(eAosMD_QoS, "%s pir %d", __FUNCTION__, priority);	
	aos_list_add_tail(&queue_cell->list, &(qos_bw_ctrl_data->skb_rcv_queue[priority]));
	
	return 1;
}

int priority_reinject_skb(void *k, void *d, void *arg)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct aos_list_head *cur,*next;
	struct qos_queue_cell *queue_cell;
	int i, count;

	qos_bw_ctrl_data = (struct qos_bw_ctrl_data *)d;

	aos_assert1(qos_bw_ctrl_data);

	if (qos_bw_ctrl_data->direction != 0 ) return 0;

	for (i = eROS_QOS_PRIORITY_MAX-1; i >= 0; i--)
	{
		// reinject buffered skb
		count = 0;
		cur = NULL; next = NULL;
		aos_list_for_each_safe(cur, next, &(qos_bw_ctrl_data->skb_rcv_queue[i])) 
		{
			queue_cell = aos_list_entry(cur, struct qos_queue_cell, list);
			aos_list_del(cur);
			nf_reinject(queue_cell->skb, queue_cell->info, NF_ACCEPT);
			aos_free(queue_cell);
			count++;
		}
aos_min_log(eAosMD_QoS, "%s, ifid %d, dir %d, pri %d, counts %d", __FUNCTION__, qos_bw_ctrl_data->ifid, 
		qos_bw_ctrl_data->direction, i, count);
	}

	return 0;
}

int priority_reinject_skb_thread(void *arg)
{
	aos_min_log(eAosMD_QoS, "enter %s, jiffies %u", __FUNCTION__, QOS_GET_JIFFIES());

	while(1)
	{

		local_bh_disable();
		hashtab_map(qos_traffic_hash_pri, priority_reinject_skb, NULL);
		local_bh_enable();

		msleep(sgbw_avr_shift);
	}

	return 0;
}



