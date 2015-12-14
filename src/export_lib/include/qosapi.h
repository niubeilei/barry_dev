#ifndef __QOS_API_H__
#define __QOS_API_H__

#include <asm/types.h>

typedef enum ros_qos_priority{
	eROS_QOS_PRIORITY_LOW = 0,	// priority low 0
	eROS_QOS_PRIORITY_MED,		// priority medium 1
	eROS_QOS_PRIORITY_HIH ,		// priority high 2
	eROS_QOS_PRIORITY_MAX,
}ros_qos_priority_t;

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

extern int aos_qos_enable(u8 enable);

extern int aos_qos_add_generalrule(
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *protocol, 
	u8 priority);

extern int aos_qos_remove_generalrule(
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *protocol);

extern int aos_qos_retrieve_generalrule(char *rslt_buff, u32 *buf_len);

extern int aos_qos_clear_generalrule(void);

extern int aos_qos_add_macrule(char *mac, u8 priority);

extern int aos_qos_remove_macrule(char *mac);

extern int aos_qos_retrieve_macrule(char *rslt_buff, u32 *buf_len);

extern int aos_qos_clear_macrule(void);

extern int aos_qos_add_vlanrule(u16 vlan_tag, u8 priority);

extern int aos_qos_remove_vlanrule(u16 vlan_tag);

extern int aos_qos_retrieve_vlanrule(char *rslt_buff, u32 *buf_len);

extern int aos_qos_clear_vlanrule(void);

extern int aos_qos_add_portrule(char *app, char* ifname, u16 port, u8 priority);

extern int aos_qos_remove_portrule(char* ifname, u16 port);

extern int aos_qos_retrieve_portrule(char *rslt_buff, u32 *buf_len);

extern int aos_qos_clear_portrule(void);

extern int aos_qos_add_protocolrule(char *protocol, u8 priority);

extern int aos_qos_remove_protocolrule(char *protocol);

extern int aos_qos_retrieve_protocolrule(char *rslt_buff, u32 *buf_len);

extern int aos_qos_clear_protocolrule(void);

extern int aos_qos_retrieve_channel(
	char *protocol, 
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *rslt_buff,
	u32 *buf_len);

extern int aos_qos_retrieve_all_channels(char *rslt_buff, u32 *buff_len);

extern int aos_qos_retrieve_channel_history(
		char *protocol, 
		char *src_ip, 
		u16 src_port, 
		char *dst_ip, 
		u16 dst_port, 
		int start_time, 
		int end_time, 
		int interval, 
		char *rslt_buff,
		u32 *buff_len);

extern int aos_qos_retrieve_interface(
	char *interface_name, 
	u8 direction, 
	char *rslt_buff,
	u32 *buf_len);

extern int aos_qos_retrieve_all_interface(char *rslt_buff, u32 *buff_len);

extern int aos_qos_retrieve_interface_history(
	char *interface_name, 
	u8 direction, 
	int start_time, 
	int end_time, 
	int interval,
	char *rslt_buff,
	u32 *buff_len);

extern int aos_qos_retrieve_priority(
	char *interface_name, 
	u8 priority, 
	char *rslt_buff,
	u32 *buf_len);

extern int aos_qos_retrieve_all_priority(char *rslt_buff, u32 *buff_len);

extern int aos_qos_retrieve_priority_history(
	char *dev_name,
	u8 priority, 
	int start_time, 
	int end_time, 
	int interval, 
	char *rslt_buff,
	u32 *buff_len);

extern int aos_qos_set_max_history_interval(u16 interval);

extern int aos_qos_set_bandwidth_control_status(u8 status);

extern int aos_qos_set_bandwidth_control_dev(char *dev_name, u8 direction, u8 status);

extern int aos_qos_set_minimum_bandwidth(u32 miniband);

extern int aos_qos_set_rule_match_log(u8 status);

extern int aos_qos_set_channel_log(u8 status);

extern int aos_qos_set_config_log(u8 status);

extern int aos_qos_set_statistics_status(u8 status);

extern int aos_qos_retrieve_statistics(char *rslt_buff, u32 *buff_len);

extern int aos_qos_save_config(char *rslt_buff, u32 *buff_len);

extern int aos_qos_clear_config(void);

extern int aos_qos_show_config(char *rslt_buff, u32 *buff_len);

#endif
