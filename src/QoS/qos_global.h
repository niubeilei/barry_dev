#ifndef _AOS_QOS_GLOBAL_H
#define _AOS_QOS_GLOBAL_H
#include "QoS/qos_traffic.h"
#include "QoS/qos_rule.h"
#define AOS_QOS_TCP 6
#define AOS_QOS_UDP 17
#define AOS_QOS_NONE    0
//struct list_head aos_qos_traffic_global_rule_list_head;
//int aos_qos_traffic_facility_status;
//struct _qos_traffic_interface global_interface[2 * MAX_SUPPORTED_INTERFACE];

enum ros_qos_api_error_code{
	eAosRc_QosErrorCodeStart = 2300,
	eAosRc_NullParam,
	eAosRc_RsltBuffTooShort,
	eAosRc_InvalidProtocol,
	eAosRc_InvalidDevice,
	eAosRc_Failed,
	eAosRc_FailedToEnable,
	eAosRc_FailedToDisable,
	eAosRc_InvalidMACAddress,
	eAosRc_InvalidParam,
//	eAosRc_InvalidPriority,
	eAosRc_InvalidVLANTag,
	eAosRc_InvalidApp,
	eAosRc_InvalidDirection,
	eAosRc_BandwidthMonitorOff,
	eAosRc_InvalidIPAddress,
	eAosRc_InvalidPort,
	eAosRc_InvalidStartTime,
	eAosRc_InvalidEndTime,
	eAosRc_InvalidInterval,
	eAosRc_DevNotFound,
	eAosRc_InvalidMiniBand,
};


#ifndef __KERNEL__

extern int aosqos_init(void);

extern unsigned int qos_fnin(
	unsigned int hook,
	struct sk_buff **pskb,
	const struct net_device *in,
	const struct net_device *out,
	int (*okfn)(struct sk_buff*));

#endif // #ifndef __KERNEL__

extern void qos_traffic_update(struct _qos_traffic_channel_record *record,	int len);

extern int qos_traffic_monitor(struct sk_buff *skb, struct ros_qos_skb_info* skb_info, int direction, u8 *priority);

extern int qos_traffic_control(struct sk_buff *skb, int skb_len, int direction, u8 priority);

extern __u8 qos_get_priority_of_channel(struct ros_qos_skb_info *skb_info, int direction, struct _qos_traffic_channel **ch);

extern int qos_get_channel_bw(u32 sip, u16 sport, u32 dip, u16 dport, u8 proto, u32 *band);
#endif
