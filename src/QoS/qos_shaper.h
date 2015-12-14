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
#ifndef __QOS_SHAPER_H__
#define __QOS_SHAPER_H__

#include "aosUtil/Types.h"
#include "KernelSimu/skbuff.h"

#define QOS_SHAPER_TIME_SLOT_DEF_INTERVAL 100 		// default time slot interval ms 
#define QOS_SHAPER_CH_HASH_KEY_SIZE 16 
#define QOS_SHAPER_AVR_BW_CAL_DEF_INTERVAL 20*HZ	// default average bandwidth calculation interval 20 seconds


typedef struct ch_shaper_hkey {
	u8 k[QOS_SHAPER_CH_HASH_KEY_SIZE];
}ch_shaper_hkey_t;

typedef struct qos_channel{
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8  protocol;
	u8  priority;
}qos_channel_t;

typedef struct qos_shaper_data{
	struct sk_buff_head skb_queue;	// buffered skb queue	
	u32 bytes_allowed;				// bytes allowed to sent/received in one time slot
	u32 bytes_sent;					// bytes sent/received in this time slot
	u32 avr_bw;						// average bandwidth for this channel
	u32 last_time;					// last time of detecting average bandwidth
}qos_shaper_data_t;

typedef struct qos_ch_shaper{
	struct qos_channel ch;
	struct qos_shaper_data shaper_data;
}qos_ch_shaper_t;

extern int ros_qos_shaper_init(void);

#endif // #ifdef __QOS_SHAPER_H
