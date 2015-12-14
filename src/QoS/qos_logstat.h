//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_logstat.h
// Description:
//   
//
// Modification History:
//		created 02/26/2006
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __QOS_LOGSTAT_H__
#define __QOS_LOGSTAT_H__

#include "aosUtil/Types.h"

typedef struct qos_stat{
	unsigned long packet_rcvd;
	unsigned long packet_drop;
	unsigned long packet_sent;

	unsigned long bytes_rcvd;
	unsigned long bytes_drop;
	unsigned long bytes_sent;
}qos_stat_t;
extern u8 sgqos_stat;
struct _qos_traffic_channel;
extern int qos_logstat_register_cli(void);
extern void qos_logstat_unregister_cli(void);
extern void ros_qos_channel_log(struct _qos_traffic_channel *ch, int action);
extern void ros_qos_config_log(char *cmd);
extern void ros_qos_rule_log(void *r, struct _qos_traffic_channel *ch);
extern void ros_qos_priority_stat(int ifid, int direction, int action, int priority, int skb_len);
extern int qos_logstat_show_config(char *rsltBuff, unsigned int *rsltIndex, int optlen);
extern int qos_logstat_save_config(char *rsltBuff, unsigned int *rsltIndex, int optlen);
extern int qos_logstat_clear_config(void);
extern void ros_qos_channel_stat(struct _qos_traffic_channel *ch, int direction, int action, int skb_len);

#endif // #ifdef __QOS_LOGSTAT_H
