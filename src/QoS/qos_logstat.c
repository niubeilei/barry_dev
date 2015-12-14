//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_log_stat.c
// Description:
//   
// Author: Tim
//
// Modification History: 
//	created 02/26/2006
// 
////////////////////////////////////////////////////////////////////////////
#include "QoS/qos_logstat.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/Modules.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/string.h"
#include "KernelSimu/dev.h"
#include "KernelSimu/netfilter.h"

#include "QoS/qos_traffic.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_global.h"
#include "QoS/qos_bw_man.h"

u8 sgqos_log_rulematch = 0;
u8 sgqos_log_channel = 1;
u8 sgqos_log_config = 0;
u8 sgqos_stat = 0;

char sgqos_channel_action[qos_max_channel_action][16] = {
	"create",
	"active",
	"deactive",
	"remove"};

void ros_qos_config_log(char *cmd)
{ 
	if(sgqos_log_config) 
		aos_min_log(eAosMD_QoS, "QOSLOG-Config: %s", cmd);
}

void ros_qos_channel_log(struct _qos_traffic_channel *ch, int action)
{
	aos_assert0(ch);
	aos_assert0(action < qos_max_channel_action);
	if (!sgqos_log_channel) return;

	aos_min_log(eAosMD_QoS, "QOSLOG-Channel: %s %d.%d.%d.%d:%d to %d.%d.%d.%d:%d", 
			sgqos_channel_action[action],
			NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port));
}

void ros_qos_rule_log(void *r, struct _qos_traffic_channel *ch)
{
	struct _qos_rule_general *gr;
	struct _qos_rule_specific *sr;
	struct net_device *dev;
	
	if(!sgqos_log_rulematch) return;
	
	aos_assert0(r);
	aos_assert0(ch);
	
	if (*((u8*)r) == AOS_QOS_RULE_GEN)
	{
		gr = (struct _qos_rule_general*)r;
		aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- "
				                "Gen(srcip:port %d.%d.%d.%d:%d, dstip:port %d.%d.%d.%d:%d, pri %d, dev %s)", 
								NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
								NIPQUAD(gr->src_ip), gr->src_port, NIPQUAD(gr->dst_ip), gr->dst_port,
								gr->priority, gr->dev_name);
		return;
	}

	sr = (struct _qos_rule_specific*)r;
	switch(sr->type)
	{
		case AOS_QOS_RULE_MAC:
			aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- "
					            "MAC(%02x:%02x:%02x:%02x:%02x:%02x, pri %d)", 
								NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
								sr->parms.macdata.mac[0],
								sr->parms.macdata.mac[1],
								sr->parms.macdata.mac[2],
								sr->parms.macdata.mac[3],
								sr->parms.macdata.mac[4],
								sr->parms.macdata.mac[5],
								sr->priority);
			break;
			
		case AOS_QOS_RULE_VLAN:
			aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- VLAN(%d, pri %d)", 
								NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
								sr->parms.vlandata.tag, sr->priority);
			break;
			
		case AOS_QOS_RULE_IF:
			aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- Dev(%s, pri %d)", 
								NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
								sr->parms.ifdata.dev_name, sr->priority);
			break;

		case AOS_QOS_RULE_PORT:
			dev = dev_get_by_index(sr->parms.portdata.ifid);
			if (dev) {
				aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- "
					"Port(%s %d, pri %d)", 
					NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
					dev->name,
					htons(sr->parms.portdata.port), sr->priority);
				dev_put(dev);
			}
			break;

		case AOS_QOS_RULE_PROTO:
			aos_min_log(eAosMD_QoS, "QOSLOG-Rule: Channel(%d.%d.%d.%d:%d to %d.%d.%d.%d:%d) -- "
					            "Protocol(%s, pri %d)", 
								NIPQUAD(ch->src_ip), htons(ch->src_port), NIPQUAD(ch->dst_ip), htons(ch->dst_port),
								(sr->parms.protodata.proto == AOS_QOS_TCP)? "TCP":"UDP", sr->priority);
			break;
		default:
			break;
	}
}

void ros_qos_priority_stat(int ifid, int direction, int action, int priority, int skb_len)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct prihashkey k;

	if (!sgqos_stat) return;
	
	qos_bw_ctrl_data = match_priority_hash(ifid, 0, priority, &k);
	if (!qos_bw_ctrl_data) return;
	
	if (direction == direction_in)
	{
		if (action == NF_ACCEPT)
		{
			qos_bw_ctrl_data->stat[priority].packet_rcvd++;
			qos_bw_ctrl_data->stat[priority].bytes_rcvd += skb_len;
		}
		else
		{
			qos_bw_ctrl_data->stat[priority].packet_drop++;
			qos_bw_ctrl_data->stat[priority].bytes_drop += skb_len;
		}
	}
	else
	{
		if (action == NF_ACCEPT)
		{
			qos_bw_ctrl_data->stat[priority].packet_sent++;
			qos_bw_ctrl_data->stat[priority].bytes_sent += skb_len;
		}
		else
		{
			qos_bw_ctrl_data->stat[priority].packet_drop++;
			qos_bw_ctrl_data->stat[priority].bytes_drop += skb_len;
		}
	}
}

void ros_qos_channel_stat(struct _qos_traffic_channel *ch, int direction, int action, int skb_len)
{

	if (!sgqos_stat) return;
	
	// aos_assert0(ch);
	
	if (direction == direction_in)
	{
		if (action == NF_ACCEPT)
		{
			ch->record.packet_rcvd++;
			ch->record.bytes_rcvd += skb_len;
		}
		else
		{
			ch->record.packet_dropped++;
			ch->record.bytes_dropped += skb_len;
		}
	}
	else
	{
		if (action == NF_ACCEPT)
		{
			ch->record.packet_sent++;
			ch->record.bytes_sent += skb_len;
		}
		else
		{
			ch->record.packet_dropped++;
			ch->record.bytes_dropped += skb_len;
		}
	}
}

static int ros_qos_log_rulematch_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// qos log rule match <on/off>
	char *status = parms->mStrings[0];

	if (strcmp(status, "on") == 0)
		sgqos_log_rulematch = 1;
	else
		sgqos_log_rulematch = 0;
	
	*length = 0;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

static int ros_qos_log_channel_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// qos log channel <on/off>
	char *status = parms->mStrings[0];

	if (strcmp(status, "on") == 0)
		sgqos_log_channel = 1;
	else
		sgqos_log_channel = 0;
	
	*length = 0;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

static int ros_qos_log_config_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// qos log config <on/off>
	char *status = parms->mStrings[0];

	if (strcmp(status, "on") == 0)
		sgqos_log_config = 1;
	else
		sgqos_log_config = 0;
	
	*length = 0;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

int qos_logstat_show_config(char *rsltBuff, unsigned int *rsltIndex, int optlen)
{
	char local[200];

	sprintf(local, "Log Rule Matching: %s\n", sgqos_log_rulematch?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "Log Channel: %s\n", sgqos_log_channel?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Log Config: %s\n", sgqos_log_config?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "QoS Statistics: %s\n", sgqos_stat?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	return eAosRc_Success;
}

int ros_qos_log_show_config_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);

	qos_logstat_show_config(rsltBuff, &rsltIndex, optlen);

	*length = rsltIndex;
	return eAosRc_Success;
}

static int qos_stat_show_pri(void *k, void *d, void *args)
{

	struct net_device *dev;
	struct bw_monitor_show_arg *show_args;
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	char msg[256];
	int if_index, i;

	show_args = (struct bw_monitor_show_arg*) args;
	if (!show_args) return 0;

	qos_bw_ctrl_data = (struct qos_bw_ctrl_data*)d;
	if (!qos_bw_ctrl_data) return 0;
	
	if_index = *((int *)k);
	dev = dev_get_by_index(if_index);

	if (!dev) return 0;

	sprintf(msg,"== statictics for dev %s ==\n", dev->name);
	aosCheckAndCopy(show_args->rsltBuff, &(show_args->rsltIndex), show_args->optlen, msg, strlen(msg));

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; ++i) 
	{
		sprintf(msg,"pri %d  packet_rcvd %-10lu packet_drop %-10lu packet_sent %-10lu\n"
				    "        bytes_rcvd %-10lu  bytes_drop %-10lu  bytes_sent %-10lu\n",  
				                i, 
								qos_bw_ctrl_data->stat[i].packet_rcvd, 
								qos_bw_ctrl_data->stat[i].packet_drop, 
								qos_bw_ctrl_data->stat[i].packet_sent,
				                qos_bw_ctrl_data->stat[i].bytes_rcvd, 
								qos_bw_ctrl_data->stat[i].bytes_drop, 
								qos_bw_ctrl_data->stat[i].bytes_sent);
		aosCheckAndCopy(show_args->rsltBuff, &(show_args->rsltIndex), show_args->optlen, msg, strlen(msg));
	}

	sprintf(msg,"\n");
	aosCheckAndCopy(show_args->rsltBuff, &(show_args->rsltIndex), show_args->optlen, msg, strlen(msg));
	
	if (dev)
		dev_put(dev);
		
	return 0;
}

static int qos_stat_reset_pri(void *k, void *d, void *args)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	int i;

	qos_bw_ctrl_data = (struct qos_bw_ctrl_data*)d;
	if (!qos_bw_ctrl_data) return 0;

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		memset(&(qos_bw_ctrl_data->stat[i]), 0, sizeof(struct qos_stat));
	}
	return 0;
}

static int ros_qos_stat_status_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	// qos stat <on/off>
	char *status = parms->mStrings[0];

	if (strcmp(status, "on") == 0)
		sgqos_stat = 1;
	else
	{
		sgqos_stat = 0;
		hashtab_map(qos_traffic_hash_pri, qos_stat_reset_pri, NULL);
	}
	
	*length = 0;

	ros_qos_config_log(parms->mCmd);
	return eAosRc_Success;
}

static int ros_qos_stat_reset_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	*length = 0;

	hashtab_map(qos_traffic_hash_pri, qos_stat_reset_pri, NULL);

	return eAosRc_Success;
}

int ros_qos_stat_show_cli(
	char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	struct bw_monitor_show_arg args;
	char *rsltBuff = aosKernelApi_getBuff(data);
	char local[200];

	*length = 0;

	if (!sgqos_stat)
	{
		sprintf(local, "QoS Statistics: %s\n", sgqos_stat?"on": "off");
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		*length = rsltIndex;
		return eAosRc_Success;
	}
	
	memset(&args, 0, sizeof(struct bw_monitor_show_arg));

	args.rsltBuff = rsltBuff;
	args.rsltIndex = rsltIndex;
	args.optlen = optlen;

	hashtab_map(qos_traffic_hash_pri, qos_stat_show_pri, (void*)(&args));

	*length = args.rsltIndex;	
	return eAosRc_Success;
}

int qos_logstat_save_config(char *rsltBuff, unsigned int *rsltIndex, int optlen)
{
	char local[200];

	sprintf(local, "<Cmd>qos log rule match %s</Cmd>\n", sgqos_log_rulematch?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	sprintf(local, "<Cmd>qos log channel %s</Cmd>\n", sgqos_log_channel?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "<Cmd>qos log config %s</Cmd>\n", sgqos_log_config?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	sprintf(local, "<Cmd>qos stat %s</Cmd>\n", sgqos_stat?"on": "off");
	aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));
	
	return eAosRc_Success;
}

int qos_logstat_clear_config(void)
{
	sgqos_log_rulematch = 0;
	sgqos_log_channel = 1;
	sgqos_log_config = 0;
	sgqos_stat = 0;
	
	hashtab_map(qos_traffic_hash_pri, qos_stat_reset_pri, NULL);

	return eAosRc_Success;
}


int qos_logstat_register_cli(void)
{
	static int isreg = 0;

	int ret = 0;
	
	if (!isreg)
	{
		ret |= OmnKernelApi_addCliCmd("QosLogRuleMatch", ros_qos_log_rulematch_cli);
		ret |= OmnKernelApi_addCliCmd("QosLogChannel", ros_qos_log_channel_cli);
		ret |= OmnKernelApi_addCliCmd("QosLogConfig", ros_qos_log_config_cli);
		ret |= OmnKernelApi_addCliCmd("QosLogShowConfig", ros_qos_log_show_config_cli);
		ret |= OmnKernelApi_addCliCmd("QosStatShow", ros_qos_stat_show_cli);
		ret |= OmnKernelApi_addCliCmd("QosStatStatus", ros_qos_stat_status_cli);
		ret |= OmnKernelApi_addCliCmd("QosStatReset", ros_qos_stat_reset_cli);
		isreg = 1;
	}
	return ret;
}

void qos_logstat_unregister_cli(void)
{
	OmnKernelApi_delCliCmd("QosLogRuleMatch");
	OmnKernelApi_delCliCmd("QosLogChannel");
	OmnKernelApi_delCliCmd("QosLogConfig");
	OmnKernelApi_delCliCmd("QosLogShowConfig");
	OmnKernelApi_delCliCmd("QosStatShow");
	OmnKernelApi_delCliCmd("QosStatStatus");
	OmnKernelApi_delCliCmd("QosStatReset");
}


