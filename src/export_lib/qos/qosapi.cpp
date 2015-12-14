#include "common.h"
#include "qosapi.h"

static char* get_pri_str(u8 pri)
{
	switch(pri) {
		case eROS_QOS_PRIORITY_HIH:
			return "high";
		case eROS_QOS_PRIORITY_MED:
			return "medium";
		case eROS_QOS_PRIORITY_LOW:
			return "low";
	}
	return "Invalid Priority";
}

static int _retrieve_info_by_cli(char *cli, char *rslt_buff, u32 *buf_len)
{
	OmnString rslt = "";
	char* tmp;

	if (!rslt_buff || !buf_len || !cli) return -eAosRc_NullParam;
	if (*buf_len <= 0) return -eAosRc_NullParam;

	OmnCliProc::getSelf()->runCliAsClient(cli, rslt);

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	if (*buf_len < (strlen(rslt)+1))
	{
		*buf_len = strlen(rslt)+1;
		return eAosRc_RsltBuffTooShort;
	}
	
	strcpy(rslt_buff, rslt);	
	
	*buf_len = strlen(rslt);
	
	return *((int*)tmp);
}

static int _run_api_by_cli(char *cli)
{
	OmnString rslt = "";
	char* tmp;

	if (!cli) return -eAosRc_NullParam;

	OmnCliProc::getSelf()->runCliAsClient(cli, rslt);

	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

int aos_qos_enable(u8 enable)
{
	char local[128];
	
	sprintf(local, "qos traffic status %s", enable?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_add_generalrule(
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *protocol, 
	u8 priority)
{
	char   local[128];

	if (!src_ip || !dst_ip  || !protocol) return -eAosRc_NullParam;

	sprintf(local, "qos generalrule add %s %d %s %d %s eth0 %s", 
		src_ip, src_port, dst_ip, dst_port, protocol, get_pri_str(priority));

	return _run_api_by_cli(local);
}

int aos_qos_remove_generalrule(
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *protocol)
{
	char   local[128];

	if (!src_ip || !dst_ip  || !protocol) return -eAosRc_NullParam;

	sprintf(local, "qos generalrule remove %s %d %s %d %s eth0", 
		src_ip, src_port, dst_ip, dst_port, protocol);

	return _run_api_by_cli(local);
}

int aos_qos_retrieve_generalrule(char *rslt_buff, u32 *buf_len)
{
	return _retrieve_info_by_cli("qos generalrule show", rslt_buff, buf_len);
}

int aos_qos_clear_generalrule(void)
{
	return _run_api_by_cli("qos generalrule clear");
}

int aos_qos_add_macrule(char *mac, u8 priority)
{
	char   local[128];

	sprintf(local, "qos macrule add %s %s", mac, get_pri_str(priority));

	return _run_api_by_cli(local);
}

int aos_qos_remove_macrule(char *mac)
{
	char   local[128];

	if (!mac) return -eAosRc_NullParam;

	sprintf(local, "qos macrule remove %s", mac);

	return _run_api_by_cli(local);
}

int aos_qos_retrieve_macrule(char *rslt_buff, u32 *buf_len)
{
	return _retrieve_info_by_cli("qos macrule show", rslt_buff, buf_len);
}

int aos_qos_clear_macrule(void)
{
	return _run_api_by_cli("qos macrule clear");
}

int aos_qos_add_vlanrule(u16 vlan_tag, u8 priority)
{
	char   local[128];

	sprintf(local, "qos vlanrule add %u %s", vlan_tag, get_pri_str(priority));

	return _run_api_by_cli(local);
}

int aos_qos_remove_vlanrule(u16 vlan_tag)
{
	char   local[128];

	sprintf(local, "qos vlanrule remove %d", vlan_tag);

	return _run_api_by_cli(local);

}

int aos_qos_retrieve_vlanrule(char *rslt_buff, u32 *buf_len)
{
	return _retrieve_info_by_cli("qos vlanrule show", rslt_buff, buf_len);
}

int aos_qos_clear_vlanrule(void)
{
	return _run_api_by_cli("qos vlanrule clear");
}


int aos_qos_add_portrule(char *app, char* ifname, u16 port, u8 priority)

{
	char   local[128];

	if (!app) return -eAosRc_NullParam;

	sprintf(local, "qos portrule add %s %s %u %s", 
		app,  ifname, port, get_pri_str(priority));

	return _run_api_by_cli(local);
}

int aos_qos_remove_portrule(char* ifname, u16 port)
{
	char   local[128];

	sprintf(local, "qos port remove %s %u", ifname, port);

	return _run_api_by_cli(local);
}

int aos_qos_retrieve_portrule(char *rslt_buff, u32 *buf_len)
{
	return _retrieve_info_by_cli("qos portrule show", rslt_buff, buf_len);
}

int aos_qos_clear_portrule(void)
{
	return _run_api_by_cli("qos portrule clear");
}

int aos_qos_add_protocolrule(char *protocol, u8 priority)

{
	char   local[128];

	if (!protocol) return -eAosRc_NullParam;

	sprintf(local, "qos protocolrule add %s %s", protocol, get_pri_str(priority));

	return _run_api_by_cli(local);
}

int aos_qos_remove_protocolrule(char *protocol)
{
	char   local[128];

	sprintf(local, "qos port remove %s", protocol);

	return _run_api_by_cli(local);
}

int aos_qos_retrieve_protocolrule(char *rslt_buff, u32 *buf_len)
{
	return _retrieve_info_by_cli("qos protocolrule show", rslt_buff, buf_len);
}

int aos_qos_clear_protocolrule(void)
{
	return _run_api_by_cli("qos protocolrule clear");
}

int aos_qos_retrieve_channel(
	char *protocol, 
	char *src_ip, 
	u16 src_port, 
	char *dst_ip, 
	u16 dst_port, 
	char *rslt_buff,
	u32 *buf_len)
{
	char   local[128];

	if (!src_ip || !dst_ip  || !protocol) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show channel %s %s %d %s %d", 
			  	protocol, src_ip, src_port, dst_ip, dst_port);

	return _retrieve_info_by_cli(local, rslt_buff, buf_len);
}

int aos_qos_retrieve_all_channels(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos bandwidth monitor show channel all", rslt_buff, buff_len);
}

int aos_qos_retrieve_channel_history(
		char *protocol, 
		char *src_ip, 
		u16 src_port, 
		char *dst_ip, 
		u16 dst_port, 
		int start_time, 
		int end_time, 
		int interval, 
		char *rslt_buff,
		u32 *buff_len)
{
	char   local[128];

	if (!src_ip || !dst_ip  || !protocol) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show channel history %s %s %d %s %d %d %d %d", 
			  	protocol, src_ip, src_port, dst_ip, dst_port,
			  	start_time, end_time, interval);

	return _retrieve_info_by_cli(local, rslt_buff, buff_len);

}

int aos_qos_retrieve_interface(
	char *interface_name, 
	u8 direction, 
	char *rslt_buff,
	u32 *buf_len)
{
	char   local[128];

	if (!interface_name) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show dev %s %s", 
			  	interface_name, direction?"out":"in");

	return _retrieve_info_by_cli(local, rslt_buff, buf_len);
}

int aos_qos_retrieve_all_interface(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos bandwidth monitor show dev all", rslt_buff, buff_len);
}

int aos_qos_retrieve_interface_history(
	char *interface_name, 
	u8 direction, 
	int start_time, 
	int end_time, 
	int interval,
	char *rslt_buff,
	u32 *buff_len)
{
	char   local[128];

	if (!interface_name) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show dev history %s %d %d %d %s", 
			  	interface_name,
			  	start_time, end_time, interval,
			  	direction?"out":"in");

	return _retrieve_info_by_cli(local, rslt_buff, buff_len);

}

int aos_qos_retrieve_priority(
	char *interface_name, 
	u8 priority, 
	char *rslt_buff,
	u32 *buf_len)
{
	char   local[128];

	if (!interface_name) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show priority %s %d", 
			  	interface_name, priority);

	return _retrieve_info_by_cli(local, rslt_buff, buf_len);
}

int aos_qos_retrieve_all_priority(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos bandwidth monitor show priority all", rslt_buff, buff_len);
}

int aos_qos_retrieve_priority_history(
	char *dev_name,
	u8 priority, 
	int start_time, 
	int end_time, 
	int interval, 
	char *rslt_buff,
	u32 *buff_len)

{
	char   local[128];

	if (!dev_name) return -eAosRc_NullParam;

	sprintf(local, "qos bandwidth monitor show priority history %s %d %d %d %s", 
			  	dev_name,
			  	start_time, end_time, interval,
			  	get_pri_str(priority));

	return _retrieve_info_by_cli(local, rslt_buff, buff_len);

}

int aos_qos_set_max_history_interval(u16 interval)
{
	char   local[128];

	sprintf(local, "qos bandwidth monitor max history interval %u", interval);

	return _run_api_by_cli(local);
}

int aos_qos_set_bandwidth_control_status(u8 status)
{
	char local[128];
	
	sprintf(local, "qos bandwidth manager %s", status?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_set_bandwidth_control_dev(char *dev_name, u8 direction, u8 status)
{
	char local[128];
	
	sprintf(local, "qos bandwidth manager ctrl %s %s %s", 
		dev_name, direction?"out":"in", status?"on": "off");
	
	return _run_api_by_cli(local);
}	

int aos_qos_set_minimum_bandwidth(u32 miniband)
{
	char   local[128];

	sprintf(local, "qos bandwidth manager set miniband %lu", miniband);

	return _run_api_by_cli(local);
}

int aos_qos_set_rule_match_log(u8 status)
{
	char local[128];
	
	sprintf(local, "qos log rule match %s", status?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_set_channel_log(u8 status)
{
	char local[128];
	
	sprintf(local, "qos log channel %s", status?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_set_config_log(u8 status)
{
	char local[128];
	
	sprintf(local, "qos log config %s", status?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_set_statistics_status(u8 status)
{
	char local[128];
	
	sprintf(local, "qos stat %s", status?"on": "off");
	
	return _run_api_by_cli(local);
}

int aos_qos_retrieve_statistics(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos stat show", rslt_buff, buff_len);
}

int aos_qos_save_config(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos save config", rslt_buff, buff_len);
}

int aos_qos_clear_config(void)
{
	return _run_api_by_cli("qos clear config");
}

int aos_qos_show_config(char *rslt_buff, u32 *buff_len)
{
	return _retrieve_info_by_cli("qos show config", rslt_buff, buff_len);
}
