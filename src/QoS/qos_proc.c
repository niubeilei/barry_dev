////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: qos_proc.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__

#include "QoS/qos_proc.h"

//#include <linux/byteorder/generic.h>

#include "aosUtil/Alarm.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/kernel.h"

#include "QoS/qos_traffic.h"
#include "QoS/qos_bw_man.h"

static struct proc_dir_entry *proc_qos;

static int qos_show_channel_info(void *k,void *d,void *args)
{
	struct _qos_traffic_channel *channel;
	struct proc_show_buf *show_buf;
	char line[256];
	int len;

	channel = (struct _qos_traffic_channel*)d;
	show_buf = (struct proc_show_buf*)args;

	if (channel == NULL || show_buf == NULL || show_buf->buf == NULL)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "proc show channel info error, channel is null or show buff null");
		return 1;
	}

	memset(line, 0, 256);
	
	// status, vlantag ifid priority total_bands, proto src ip:port \t dst ip:port 
	len = sprintf(line, "state %u, vlan %02u, ifid %02d, pri %u, total_band %04u Bps, proto %u, src %d.%d.%d.%d:%d, dst%d.%d.%d.%d:%d, band_req %u\n",
			channel->status,
			channel->vlan,
			channel->ifid,
			channel->priority,
			channel->record.total_band,
			channel->protocol,
			NIPQUAD(channel->src_ip),
			ntohs(channel->src_port),
			NIPQUAD(channel->dst_ip),
			ntohs(channel->dst_port),
			channel->band_required);

	if ((show_buf->offset+len) > show_buf->size) return 1;
	memcpy(show_buf->buf+show_buf->offset, line, len);
	show_buf->offset += len;
	show_buf->buf[show_buf->offset] = 0;
	
	return 0;	
}

static int
ros_qos_proc_read_channel(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	struct proc_show_buf show_buf;

	aos_trace("%s offset = %d, count = %d", __FUNCTION__, offset, count);

	if(offset > 0)
		return 0;

	show_buf.buf = buf;
	show_buf.offset = 0;
	show_buf.size = count;

	// status, vlantag ifid priority total_bands, proto src ip:port \t dst ip:port 
	if (!qos_traffic_hash_gen)
	{
		*eof = 1;
		return 0;
	}

	local_bh_disable();
	sprintf(buf, "status, vlan ifid pri bw, proto srcip:port\t dstip:port, entry counts %d\n", 
			hashtab_elements(qos_traffic_hash_gen));

	show_buf.offset = strlen(show_buf.buf);
			
	hashtab_map(qos_traffic_hash_gen, qos_show_channel_info, (void*)(&show_buf));

	local_bh_enable();

	*eof = 1;
	return strlen(buf);
}

static int qos_show_pri_info(void *k,void *d,void *args)
{
	struct qos_bw_ctrl_data *qos_bw_ctrl_data;
	struct proc_show_buf *show_buf;
	char line[256];
	int len, i;

	qos_bw_ctrl_data = (struct qos_bw_ctrl_data*)d;
	show_buf = (struct proc_show_buf*)args;

	if (qos_bw_ctrl_data == NULL || show_buf == NULL || show_buf->buf == NULL)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "proc show bwctrl info error, bw ctrl data is null or show buff null");
		return 1;
	}

	memset(line, 0, 256);
	
	// ifid, direction, bwctrl 
	len = sprintf(line, "ifid %d, dir %u, bw_ctrl %u, state %u, drop_index %u, turns %d, total_bw %u, real_total %u, last_total %u\n",
			qos_bw_ctrl_data->ifid,
			qos_bw_ctrl_data->direction,
			qos_bw_ctrl_data->bw_ctrl,
			qos_bw_ctrl_data->state,
			qos_bw_ctrl_data->drop_index,
			qos_bw_ctrl_data->tunning_turns,
			qos_bw_ctrl_data->total_bw,
			qos_bw_ctrl_data->real_total_throughout,
			qos_bw_ctrl_data->last_total_throughout);
	
	if ((show_buf->offset+len) > show_buf->size) return 1;
	
	memcpy(show_buf->buf+show_buf->offset, line, len);
	show_buf->offset += len;

	for (i = 0; i < eROS_QOS_PRIORITY_MAX; i++)
	{
		memset(line, 0, 256);
		len = sprintf(line, "\tpri %u, ctrl %u, real_bw %u, last_bw %u, allowed_bw %u, total_bw %u, req_bw %u, limit_bw %u\n",
						i,
						qos_bw_ctrl_data->ctrl[i],
						qos_bw_ctrl_data->real_bw[i],
						qos_bw_ctrl_data->last_bw[i],
						qos_bw_ctrl_data->allowed_bw[i],
						qos_bw_ctrl_data->record[i].total_band,
						qos_bw_ctrl_data->req_bw[i],
						qos_bw_ctrl_data->limit_bw[i]);
		if ((show_buf->offset+len) > show_buf->size) return 1;
		memcpy(show_buf->buf+show_buf->offset, line, len);
		show_buf->offset += len;
	}
	
	show_buf->buf[show_buf->offset] = 0;
	return 0;	
}

static int
ros_qos_proc_read_bwctrl(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	struct proc_show_buf show_buf;

	aos_trace("%s offset = %d, count = %d", __FUNCTION__, offset, count);

	if(offset > 0)
		return 0;

	show_buf.buf = buf;
	show_buf.offset = 0;
	show_buf.size = count;
	
	if (!qos_traffic_hash_pri)
	{
		*eof = 1;
		return 0;
	}
			
	local_bh_disable();

	hashtab_map(qos_traffic_hash_pri, qos_show_pri_info, (void*)(&show_buf));

	local_bh_enable();
	
	*eof = 1;
	return strlen(buf);
}


int ros_qos_register_proc(void)
{
	aos_trace("%s", __FUNCTION__);
	
	proc_qos = proc_mkdir("qos",&proc_root);
	if(!proc_qos)
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "QoS proc_mkdir failed!");
		goto failed;
	}

	if ( 0 == create_proc_read_entry("channels", 0444, proc_qos, ros_qos_proc_read_channel, 0) )
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "create QoS proc entry channels failed!");
		goto release_qos;
	}

	if ( 0 == create_proc_read_entry("bwctrl", 0444, proc_qos, ros_qos_proc_read_bwctrl, 0) )
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "create QoS proc entry bwctrl failed!");
		goto release_channels;
	}
	aos_min_log(eAosMD_QoS, "qos create proc success");
	
	return 0;
release_channels:
	remove_proc_entry("channels", proc_qos);
release_qos:
	remove_proc_entry("qos",&proc_root);
failed:
	return -1;
}



void ros_qos_unregister_proc(void)
{
	remove_proc_entry("channels", proc_qos);
	remove_proc_entry("qos",&proc_root);
}

#endif // #ifdef __KERNEL__
