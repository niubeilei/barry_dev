#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/module.h"
#include "QoS/hashtab.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_config.h"
#include "QoS/qos_logstat.h"
#include "QoS/qos_bw_man.h"

/////////////////////////////////////////////////////////////////////
//Facilities

void change_priority_from_num_to_char(char *priority,__u8 priority_num)
{

	switch (priority_num) {
		case AOS_QOS_RULE_PRIORITY_HIGH:
			strcpy(priority,"high");
			break;

		case AOS_QOS_RULE_PRIORITY_MEDIUM:
			strcpy(priority,"medium");
			break;

		case AOS_QOS_RULE_PRIORITY_LOW:
			strcpy(priority,"low");
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid priority number");
	}

	return;
}

void change_mac_from_num_to_char(char *mac,__u8 *mac_num)
{
	int count;
	int number;
	int i;

	count = 0;
	number = 0;

	for (i = 0; i < 17; ++i){
		switch (count) {
			case 0:
				mac[i] = mac_num[number] >> 4;
				aos_assert0(mac[i] >= 0 && mac[i] <= 15);
				if (mac[i] < 10) {
					mac[i] += '0';
				} else {
					mac[i] += ('A' - 10);
				}
				break;

			case 1:
				mac[i] = mac_num[number] & 0xf;
				aos_assert0(mac[i] >= 0 && mac[i] <=15);
				if (mac[i] < 10) {
					mac[i] += '0';
				} else {
					mac[i] += ('A' - 10);
				}
				break;

			case 2:
				mac[i] = ':';
				break;

			default:
				aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid count number");
		}
			++count;
			if (count == 3) {
				++number;
				count = 0;
			}
	}

	return;
}

void change_proto_from_num_to_char(char *proto,__u8 proto_num)
{

	switch (proto_num) {
		case AOS_QOS_TCP:
			strcpy(proto,"tcp");
			break;

		case AOS_QOS_UDP:
			strcpy(proto,"udp");
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid proto number");
	}

	return;
}

void change_direction_from_num_to_char(char *direction, __u8 direction_num)
{

	switch(direction_num) {
		case AOS_QOS_RULE_DIRECTION_SENDING:
			strcpy(direction,"sending");
			break;

		case AOS_QOS_RULE_DIRECTION_RECEIVING:
			strcpy(direction,"receiving");
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid direction number");
	}

	return;
}
//Facilities end

//Clear Functions
int qos_clear_rule_manager_config(void)
{
	int ret;

	aos_qos_rule_destroy();
	ret = aos_qos_rule_init_variables();

	return ret;
	
}

int qos_clear_traffic_monitor_config(void)
{
	int ret;

	ret = qos_traffic_set_status(AOS_QOS_TRAFFIC_STATUS_OFF);
	sgMax_history_len = 60 * HZ;

	return ret;
}

//Show Functions
void gen_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_general *rule)
{
	char local_proto[QOS_TRAFFIC_PROTO_BUFF_SIZE];
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];

	change_proto_from_num_to_char(local_proto,rule->proto);

	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"GeneralRule: from %u.%u.%u.%u:%u to %u.%u.%u.%u:%u with protocol %s, devname %s; priority:%s \n",NIPQUAD(rule->src_ip),ntohs(rule->src_port),NIPQUAD(rule->dst_ip),ntohs(rule->dst_port),local_proto,rule->dev_name,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void mac_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char mac[18];
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	memset(mac,0,18);
	change_mac_from_num_to_char(mac,rule->parms.macdata.mac);

	sprintf(local,"MacRule with MAC Address %s; priority: %s\n",mac,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void vlan_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"VlanRule with Vlantag %u; priority: %s.\n",rule->parms.vlandata.tag,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void if_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"InterfaceRule with interface name %s;priority %s\n",rule->parms.ifdata.dev_name,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}


void port_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	//char local_direction[QOS_TRAFFIC_DIRECTION_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);
	//change_direction_from_num_to_char(local_direction,rule->parms.portdata.sending);
	
	//sprintf(local,"PortRule with %s direction, port number:%u,application protocol name:%s; priority:%s.\n",local_direction,ntohs(rule->parms.portdata.port),rule->parms.portdata.app,local_priority);
	sprintf(local,"PortRule with port number:%u,application protocol name:%s; priority:%s.\n",ntohs(rule->parms.portdata.port),rule->parms.portdata.app,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));
	return;
}

void proto_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	char local_proto[QOS_TRAFFIC_PROTO_BUFF_SIZE];

	change_proto_from_num_to_char(local_proto,rule->parms.protodata.proto);
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"ProtocolRule with %s;priority: %s.\n",local_proto,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}


void spc_rule_show_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	switch(rule->type) {
		case AOS_QOS_RULE_MAC:
			mac_rule_show_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_VLAN:
			vlan_rule_show_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_IF:
			if_rule_show_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_PORT:
			port_rule_show_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_PROTO:
			proto_rule_show_single_config(lp,local,rule);
			break;
		default:
			break;
	}

	return;
}

int config_show_single_rule(void *k,void *d, void *args)
{
	__u8 type;
	struct _qos_rule_show_arg *lp;
	struct _qos_rule_general *gen_rul;
	struct _qos_rule_specific *spc_rul;
	char local[QOS_CONFIG_RET_SIZE];

	lp = (struct _qos_rule_show_arg *)args;
	type = lp->type;
	gen_rul = (struct _qos_rule_general *)(d);
	spc_rul = (struct _qos_rule_specific *)(d);

	switch (type) {
		case AOS_QOS_RULE_GEN:
				gen_rule_show_single_config(lp,local,gen_rul);
			break;
		case AOS_QOS_RULE_SPC:
				spc_rule_show_single_config(lp,local,spc_rul);
			break;
		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid rule type");
	}//end switch

	return 0;
}


void gen_rule_show_config(struct _qos_rule_show_arg *arg)
{
	arg->type = AOS_QOS_RULE_GEN;

#ifdef __KERNEL__
	local_bh_disable();
#endif

	hashtab_map(aos_qos_rule_gen_hash,config_show_single_rule,arg);

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}

void spc_rule_show_config(struct _qos_rule_show_arg *arg)
{
	arg->type = AOS_QOS_RULE_SPC;

#ifdef __KERNEL__
	local_bh_disable();
#endif

	hashtab_map(aos_qos_rule_spc_hash,config_show_single_rule,arg);

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}


void qos_rule_show_config (struct _qos_rule_show_arg *arg)
{

	gen_rule_show_config(arg);
	spc_rule_show_config(arg);

	return;
}


void qos_traffic_show_config(struct _qos_rule_show_arg *arg)
{
	char local[QOS_CONFIG_RET_SIZE];
	char status[QOS_TRAFFIC_STATUS_BUFF_SIZE];

	memset(local,0,QOS_CONFIG_RET_SIZE);
	memset(status,0,QOS_TRAFFIC_STATUS_BUFF_SIZE);
	
	sprintf(local,"Maximum History Interval: %d seconds\n", sgMax_history_len/HZ);
	aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
	
	switch (aos_qos_traffic_facility_status) {

		case AOS_QOS_TRAFFIC_STATUS_OFF:
			sprintf(local,"Monitor Status : off\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		case AOS_QOS_TRAFFIC_STATUS_ON:
			sprintf(local,"Monitor Status : on\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		case AOS_QOS_TRAFFIC_STATUS_STOP:
			sprintf(local,"Monitor Status : stop\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid facility status occured");

	}

	return;
}
//End Show

//Begin Save
void gen_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_general *rule)
{
	char local_proto[QOS_TRAFFIC_PROTO_BUFF_SIZE];
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];

	change_proto_from_num_to_char(local_proto,rule->proto);

	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"<Cmd>qos generalrule add %u.%u.%u.%u %u %u.%u.%u.%u %u %s %s %s </Cmd>\n",NIPQUAD(rule->src_ip),ntohs(rule->src_port),NIPQUAD(rule->dst_ip),ntohs(rule->dst_port),local_proto,rule->dev_name,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}


void mac_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char mac[18];
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	memset(mac,0,18);
	change_mac_from_num_to_char(mac,rule->parms.macdata.mac);

	sprintf(local,"<Cmd>qos macrule add %s %s </Cmd>\n",mac,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void vlan_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"<Cmd>qos vlanrule add %u %s </Cmd>\n",rule->parms.vlandata.tag,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void if_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"<Cmd>qos ifrule add %s %s </Cmd>\n",rule->parms.ifdata.dev_name,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void port_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	char local_direction[QOS_TRAFFIC_DIRECTION_BUFF_SIZE];
	struct net_device *dev;
	
	change_priority_from_num_to_char(local_priority,rule->priority);
	//change_direction_from_num_to_char(local_direction,rule->parms.portdata.sending);

	//sprintf(local,"<Cmd>qos portrule add %s %u %s %s </Cmd>\n",local_direction,ntohs(rule->parms.portdata.port),rule->parms.portdata.app,local_priority);
	dev = dev_get_by_index(rule->parms.portdata.ifid);
	if (dev)
		sprintf(local,"<Cmd>qos portrule add %s %s %u %s </Cmd>\n", 
			rule->parms.portdata.app, 
			dev->name, 
			ntohs(rule->parms.portdata.port),
			local_priority);
			
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));
	if (dev)
		dev_put(dev);
		
	return;
}

void proto_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	char local_priority[QOS_TRAFFIC_PRI_BUFF_SIZE];
	char local_proto[QOS_TRAFFIC_PROTO_BUFF_SIZE];

	change_proto_from_num_to_char(local_proto,rule->parms.protodata.proto);
	change_priority_from_num_to_char(local_priority,rule->priority);

	sprintf(local,"<Cmd>qos protorule add %s %s </Cmd>\n",local_proto,local_priority);
	aosCheckAndCopy(lp->rsltBuff,
					&(lp->rsltIndex),
					lp->optlen,
					local,
					strlen(local));

	return;
}

void spc_rule_save_single_config(struct _qos_rule_show_arg *lp,char *local,struct _qos_rule_specific *rule)
{
	switch(rule->type) {
		case AOS_QOS_RULE_MAC:
			mac_rule_save_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_VLAN:
			vlan_rule_save_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_IF:
			if_rule_save_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_PORT:
			port_rule_save_single_config(lp,local,rule);
			break;
		case AOS_QOS_RULE_PROTO:
			proto_rule_save_single_config(lp,local,rule);
			break;
		default:
			break;
	}

	return;
}

int save_single_rule(void *k,void *d, void *args)
{
	__u8 type;
	struct _qos_rule_show_arg *lp;
	struct _qos_rule_general *gen_rul;
	struct _qos_rule_specific *spc_rul;
	char local[QOS_CONFIG_RET_SIZE];

	lp = (struct _qos_rule_show_arg *)args;
	type = lp->type;
	gen_rul = (struct _qos_rule_general *)(d);
	spc_rul = (struct _qos_rule_specific *)(d);

	switch (type) {
		case AOS_QOS_RULE_GEN:
				gen_rule_save_single_config(lp,local,gen_rul);
			break;
		case AOS_QOS_RULE_SPC:
				spc_rule_save_single_config(lp,local,spc_rul);
			break;
		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid rule type");
	}//end switch

	return 0;
}

void gen_rule_save_config(struct _qos_rule_show_arg *arg)
{
	arg->type = AOS_QOS_RULE_GEN;

#ifdef __KERNEL__
	local_bh_disable();
#endif

	hashtab_map(aos_qos_rule_gen_hash,save_single_rule,arg);

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}

void spc_rule_save_config(struct _qos_rule_show_arg *arg)
{
	arg->type = AOS_QOS_RULE_SPC;

#ifdef __KERNEL__
	local_bh_disable();
#endif

	hashtab_map(aos_qos_rule_spc_hash,save_single_rule,arg);

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}

void qos_rule_save_config (struct _qos_rule_show_arg *arg)
{

	gen_rule_save_config(arg);
	spc_rule_save_config(arg);

	return;
}

void qos_traffic_save_config(struct _qos_rule_show_arg *arg)
{
	char local[QOS_CONFIG_RET_SIZE];
	char status[QOS_TRAFFIC_STATUS_BUFF_SIZE];

	memset(local,0,QOS_CONFIG_RET_SIZE);
	memset(status,0,QOS_TRAFFIC_STATUS_BUFF_SIZE);
	
	sprintf(local,"<Cmd>qos bandwidth monitor max history interval %d</Cmd>\n", sgMax_history_len/HZ);
	aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));

	switch (aos_qos_traffic_facility_status) {

		case AOS_QOS_TRAFFIC_STATUS_OFF:
			sprintf(local,"<Cmd>qos traffic status off </Cmd>\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		case AOS_QOS_TRAFFIC_STATUS_ON:
			sprintf(local,"<Cmd>qos traffic status on </Cmd>\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		case AOS_QOS_TRAFFIC_STATUS_STOP:
			sprintf(local,"<Cmd>qos traffic status on </Cmd>\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			sprintf(local,"<Cmd>qos traffic status stop </Cmd>\n");
			aosCheckAndCopy(arg->rsltBuff,&arg->rsltIndex,arg->optlen,local,strlen(local));
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid facility status occured");

	}

	return;
}
//End Save Functions

//CLI Interface
int AosQos_showConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	unsigned int rsltIndex;
	unsigned int optlen;
	char local[128];
	struct _qos_rule_show_arg arg;
	char *rsltBuff;
	
	rsltBuff = aosKernelApi_getBuff(data);
	rsltIndex = 0;
	optlen = *length;
	*length = 0;

	arg.type = AOS_QOS_RULE_GEN;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;

	sprintf(local,"------Qos Rule Config------\n");
	aosCheckAndCopy(arg.rsltBuff,&arg.rsltIndex,arg.optlen,local,strlen(local));

	qos_rule_show_config(&arg);
	
	sprintf(local,"\n------Qos Traffic Monitor Config------\n");
	aosCheckAndCopy(arg.rsltBuff,&arg.rsltIndex,arg.optlen,local,strlen(local));
	
	qos_traffic_show_config(&arg);
	
	sprintf(local,"\n------Qos Bandwidth Manager Config------\n");
	aosCheckAndCopy(arg.rsltBuff,&arg.rsltIndex,arg.optlen,local,strlen(local));

	qos_bw_man_show_config(arg.rsltBuff, &arg.rsltIndex, arg.optlen);

	sprintf(local,"\n------Qos Log and Statistics Config------\n");
	aosCheckAndCopy(arg.rsltBuff,&arg.rsltIndex,arg.optlen,local,strlen(local));

	qos_logstat_show_config(arg.rsltBuff, &arg.rsltIndex, arg.optlen);
	
	*length =arg.rsltIndex;

	return 0;
}

int AosQos_saveConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	unsigned int rsltIndex;
	unsigned int optlen;
	struct _qos_rule_show_arg arg;
	char *rsltBuff;
	
	rsltBuff = aosKernelApi_getBuff(data);
	rsltIndex = 0;
	optlen = *length;
	*length = 0;

	arg.type = AOS_QOS_RULE_GEN;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;

	qos_rule_save_config(&arg);
	
	qos_traffic_save_config(&arg);

	qos_bw_man_save_config(arg.rsltBuff, &arg.rsltIndex, arg.optlen);
	
	qos_logstat_save_config(arg.rsltBuff, &arg.rsltIndex, arg.optlen);

	*length =arg.rsltIndex;

	ros_qos_config_log(parms->mCmd);
	return 0;
}

int AosQos_clearConfigCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	int ret = 0;
	
	*length = 0;

	ret |= qos_clear_rule_manager_config();
	ret |= qos_clear_traffic_monitor_config();
	ret |= qos_bw_man_clear_config();
	ret != qos_logstat_clear_config();
	
	ros_qos_config_log(parms->mCmd);
	return ret;
}

int AosQos_registerConfCli(void)
{
	int ret;

	ret = 0;

	ret |= OmnKernelApi_addCliCmd("qos_show_config",
		   						  AosQos_showConfigCli);
	ret |= OmnKernelApi_addCliCmd("qos_save_config",
		   						  AosQos_saveConfigCli);
	ret |= OmnKernelApi_addCliCmd("qos_clear_config",
		   						  AosQos_clearConfigCli);
	return ret;
}

void AosQos_unregisterConfCli(void)
{

	OmnKernelApi_delCliCmd("qos_clear_config");
	OmnKernelApi_delCliCmd("qos_save_config");
	OmnKernelApi_delCliCmd("qos_show_config");

	return;
}

int AosQos_initConfModule(void)
{
	int ret;

	ret = AosQos_registerConfCli();

	return ret;
}

void AosQos_exitConfModule(void)
{
	AosQos_unregisterConfCli();
	
	return;
}


