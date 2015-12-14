//#include <linux/module.h>
//#include <linux/netdevice.h>
#include <linux/errno.h>
#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/module.h"
#include "KernelSimu/list.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
#include "QoS/qos_rule.h"
#include "QoS/qos_cli_check_parms.h"
#include "QoS/qos_bw_man.h"
#include "QoS/qos_logstat.h"

void add_output(int ret, char *errmsg)
{
	switch (ret) {
		case -EINVAL:
			sprintf(errmsg,"Failed!Invalid argument\n");
			break;
		case -EEXIST:
			sprintf(errmsg,"Failed!The rule is set already\n");
			break;
		case -ENOMEM:
			sprintf(errmsg,"Failed!Allocating memory failed\n");
			break;
		case -eAosRc_InvalidDevice:
			sprintf(errmsg, "invalid device\n");
			break;
		case -eAosRc_InvalidPriority:
			sprintf(errmsg, "invalid priority\n");
			break;
		case 0:
			//sprintf(errmsg,"Success!Add rule success\n");
 			break;
		default:
			sprintf(errmsg,"wrong hashreturn value");
	}
	return;
}


void del_output(int ret, char *errmsg)
{
	switch (ret) {
		case -EINVAL:
			sprintf(errmsg,"Failed!Invalid argument\n");
			break;
		case -ENOENT:
			sprintf(errmsg,"Failed!There is no such rule\n");
			break;
		case -eAosRc_InvalidDevice:
			sprintf(errmsg, "invalid device\n");
			break;
		case -eAosRc_InvalidPriority:
			sprintf(errmsg, "invalid priority\n");
			break;
		case 0:
			//sprintf(errmsg,"Success!Remove rule success\n");
			break;
		default:
			sprintf(errmsg,"wrong hashreturn value");
	}
	return;
}


void mod_output(int ret, char *errmsg)
{
	switch (ret) {
		case -EINVAL:
			sprintf(errmsg,"Failed!Invalid argument\n");
			break;
		case -ENOMEM:
			sprintf(errmsg,"Failed!Allocating memory failed\n");
			break;
		case 0:
			sprintf(errmsg,"Success!Mod rule success\n");
			break;
		default:
			sprintf(errmsg,"wrong hashreturn value\n");
	}
	return;
}




static int AosQoSRuleCli_AddGeneral(char *data,
									unsigned int *length,
									struct aosKernelApiParms *parms,
									char *errmsg,
									const int errlen)
{
	int ret = 0;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8 proto;
	char *dev_name = NULL;
	char *proto_type = NULL;
	char *priority = NULL;
	//char *sband_req;
	int dev_id = 0;
	u8 level = 0;
	char app[AOS_QOS_APP_MAX_SIZE] = "na";
	long band_req;

	QOS_RULE_PRINTK("General Rule Adding\n");
	src_ip = parms->mIntegers[0];
	src_port = parms->mIntegers[1];
	dst_ip = parms->mIntegers[2];
	dst_port = parms->mIntegers[3];
	proto_type = parms->mStrings[0];
	dev_name = parms->mStrings[1];
	priority = parms->mStrings[2];
	//sband_req = parms->mStrings[3];
	
	*length = 0;
	src_port = htons(src_port);
	dst_port = htons(dst_port);
	
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		ret = -eAosRc_InvalidProtocol;
		goto out;
	}

	ret = is_valid_priority(priority,&level);

	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	ret = is_valid_if(dev_name,&dev_id);
	if (ret != 0) {
		ret = -eAosRc_InvalidParam;
		goto out;
	}
	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT 
		&& dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		sprintf(errmsg, "Ivalid general rule\n");
		// return -EINVAL;
		return -eAosRc_InvalidIPAddress;
	}
	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT) {
		if (dst_ip == WILDCARD_IP) {
			//ret = qos_rule_add_port(AOS_QOS_RULE_DIRECTION_RECEIVING,dst_port,level,app);
			ret = qos_rule_add_port(dev_id, dst_port, level, app);
			QOS_RULE_PRINTK("Rule Added with Return value %d\n",ret);
			goto out;
		}
	}
	if (dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		if (src_ip == WILDCARD_IP) {
			//ret = qos_rule_add_port(AOS_QOS_RULE_DIRECTION_SENDING,src_port,level,app);
			ret = qos_rule_add_port(dev_id, src_port, level, app);
			QOS_RULE_PRINTK("Rule Added with Return value %d\n",ret);
			goto out;
		}
	}

	band_req = 0;
	/*
	while(*sband_req != 0)
	{
		if (*sband_req < '0' || *sband_req > '9')
		{
			sprintf(errmsg, "bandwidth required should be interger larger than zero\n");
			ret = -1;
			goto out;
		}
		band_req = band_req*10 + (*sband_req-'0');
		sband_req++;
	}

	band_req *= 1024;
	if (band_req <=0 || band_req > QOS_BW_MAXIMUM_DEFAULT)
	{
		ret = -1;
		sprintf(errmsg, "bandwidth should be larger than 0 and little than 1G bits/s");
		goto out;
	}
	*/
	QOS_TRAFFIC_PRINTK("src_ip:%u.%u.%u.%u\n",NIPQUAD(src_ip));
	QOS_TRAFFIC_PRINTK("dst_ip:%u.%u.%u.%u\n",NIPQUAD(dst_ip));
	QOS_TRAFFIC_PRINTK("src_port:%u\n",src_port);
	QOS_TRAFFIC_PRINTK("dst_port:%u\n",dst_port);
	ret = qos_rule_add_general(src_ip,src_port,dst_ip,dst_port,proto,dev_id,level,dev_name, band_req);
	if (ret) 
	{
		ret = -eAosRc_Failed;
		goto out;
	}
	//ret = qos_rule_add_general(dst_ip, dst_port, src_ip, src_port, proto, dev_id, level, dev_name, band_req);
	//if (ret) goto out;

	ros_qos_config_log(parms->mCmd);
	return 0;

out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_DelGeneral(char *data,
									unsigned int *length,
									struct aosKernelApiParms *parms,
									char *errmsg,
									const int errlen)
{
	int ret = 0;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8 proto;
	char *dev_name = NULL;
	char *proto_type = NULL;
	int dev_id = 0;
	*length = 0;
	src_ip = parms->mIntegers[0];
	src_port = parms->mIntegers[1];
	dst_ip = parms->mIntegers[2];
	dst_port = parms->mIntegers[3];
	proto_type = parms->mStrings[0];
	dev_name = parms->mStrings[1];
//	src_ip = htonl(src_ip);
//	dst_ip = htonl(dst_ip);
	src_port = htons(src_port);
	dst_port = htons(dst_port);
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		ret = -eAosRc_InvalidProtocol;
		goto out;
	}
	ret = is_valid_if(dev_name,&dev_id);
	if (ret != 0) {
		ret = -eAosRc_InvalidDevice;
		goto out;
	}
	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT
		&& dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		sprintf(errmsg, "Ivalid general rule\n");
		// return -EINVAL;
		return -eAosRc_InvalidIPAddress;
	}
	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT) {
		if (dst_ip == WILDCARD_IP) {
			//ret = qos_rule_del_port(dst_port,AOS_QOS_RULE_DIRECTION_RECEIVING);
			ret = qos_rule_del_port(dst_port, dev_id);
			QOS_RULE_PRINTK("Rule Removed with Return value %d\n",ret);
			ret = -eAosRc_Failed;
			goto out;
		}
	}
	if (dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		if (src_ip == WILDCARD_IP) {
			//ret = qos_rule_del_port(src_port,AOS_QOS_RULE_DIRECTION_SENDING);
			ret = qos_rule_del_port(src_port, dev_id);
			QOS_RULE_PRINTK("Rule Removed with Return value %d\n",ret);
			ret = -eAosRc_Failed;
			goto out;
		}
	}

	ret = qos_rule_del_general(src_ip,src_port,dst_ip,dst_port,proto,dev_id);
	QOS_RULE_PRINTK("Rule Removed with Return value %d\n",ret);
	if (ret) 
	{
		ret = -eAosRc_Failed;
		goto out; 
	}

	// ret = qos_rule_del_general(dst_ip, dst_port, src_ip, src_port, proto, dev_id);
	// if (ret) goto out;

	ros_qos_config_log(parms->mCmd);
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModGeneral(char *data,
									unsigned int *length,
									struct aosKernelApiParms *parms,
									char *errmsg,
									const int errlen)
{
	int ret = 0;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	u8 proto;
	char *dev_name = NULL;
	char *proto_type = NULL;
	char *priority = NULL;
	int dev_id = 0;
	u8 level = 0;
	char app[AOS_QOS_APP_MAX_SIZE] = "na";
	*length = 0;
	src_ip = parms->mIntegers[0];
	src_port = parms->mIntegers[1];
	dst_ip = parms->mIntegers[2];
	dst_port = parms->mIntegers[3];
	proto_type = parms->mStrings[0];
	dev_name = parms->mStrings[1];
	priority = parms->mStrings[2];
//	src_ip = htonl(src_ip);
//	dst_ip = htonl(dst_ip);
	src_port = htons(src_port);
	dst_port = htons(dst_port);
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_if(dev_name, &dev_id);
	if (ret != 0) {
		goto out;
	}

	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT
		&& dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		sprintf(errmsg, "Ivalid general rule\n");
		return -EINVAL;
	}
	if (src_ip == WILDCARD_IP && src_port == WILDCARD_PORT) {
		if (dst_ip == WILDCARD_IP) {
			ret = qos_rule_mod_port(AOS_QOS_RULE_DIRECTION_RECEIVING,dst_port,level,app);
			QOS_RULE_PRINTK("Rule Modified with Return value %d\n",ret);
			goto out;
		}
	}
	if (dst_ip == WILDCARD_IP && dst_port == WILDCARD_PORT) {
		if (src_ip == WILDCARD_IP) {
			ret = qos_rule_mod_port(AOS_QOS_RULE_DIRECTION_SENDING,src_port,level,app);
			QOS_RULE_PRINTK("Rule Modified with Return value %d\n",ret);
			goto out;
		}
	}
	ret = qos_rule_mod_general(src_ip,src_port,dst_ip,dst_port,proto,dev_id,level,dev_name);
	QOS_RULE_PRINTK("Rule Modified with Return value %d\n",ret);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ShowGeneral(char *data,
									unsigned int *length,
									struct aosKernelApiParms *parms,
									char *errmsg,
									const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_GEN;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_gen_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"No rules found\n");
	}
	return 0;
}
static int AosQoSRuleCli_ClearGeneral(char *data,
									unsigned int *length,
									struct aosKernelApiParms *parms,
									char *errmsg,
									const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_GEN;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_gen_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}

static int AosQoSRuleCli_AddMAC(char *data,
								unsigned int *length,
								struct aosKernelApiParms *parms,
								char *errmsg,
								const int errlen)
{
	int ret = 0;
	u8 MAC[ETH_ALEN];
	char *pos = NULL;
	char *priority = NULL;
	u8 level = 0;
	*length = 0;
	pos = parms->mStrings[0];
	priority = parms->mStrings[1];
	*length = 0;
	ret = is_valid_mac_address(pos,MAC);
	if (ret != 0) {
		ret = -eAosRc_InvalidMACAddress;
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	ret = qos_rule_add_mac(MAC,ETH_ALEN,level);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}

static int AosQoSRuleCli_DelMAC(char *data,
								unsigned int *length,
								struct aosKernelApiParms *parms,
								char *errmsg,
								const int errlen)
{
	int ret = 0;
	u8 MAC[ETH_ALEN];
	char *pos = NULL;
	*length = 0;
	pos = parms->mStrings[0];
	ret = is_valid_mac_address(pos,MAC);
	if (ret != 0) {
		ret = -eAosRc_InvalidMACAddress;
		goto out;
	}
	ret = qos_rule_del_mac(MAC,ETH_ALEN);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModMAC(char *data,
								unsigned int *length,
								struct aosKernelApiParms *parms,
								char *errmsg,
								const int errlen)
{
	int ret = 0;
	u8 MAC[ETH_ALEN];
	char *pos = NULL;
	char *priority = NULL;
	u8 level = 0;
	*length = 0;
	pos = parms->mStrings[0];
	priority = parms->mStrings[1];
	ret = is_valid_mac_address(pos,MAC);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_priority(priority,&level);

	if (ret != 0) {
		goto out;
	}
	ret = qos_rule_mod_mac(MAC,ETH_ALEN,level);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ShowMAC(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_MAC;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_spc_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"There is not any rule.\n");
	}
	return 0;
}
static int AosQoSRuleCli_ClearMAC(char *data,
								  unsigned int *length,
								  struct aosKernelApiParms *parms,
								  char *errmsg,
								  const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_MAC;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}

static int AosQoSRuleCli_AddVlan(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	char *priority = NULL;
	u8 level = 0;
	u16 tag;
	*length = 0;
	priority = parms->mStrings[0];
	tag = parms->mIntegers[0];
	ret = is_valid_vlantag(tag);
	if (ret != 0) {
		ret = -eAosRc_InvalidVLANTag;
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	ret = qos_rule_add_vlan(tag,level);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}

static int AosQoSRuleCli_DelVlan(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	u16 tag;
	*length = 0;
	tag = parms->mIntegers[0];
	ret = is_valid_vlantag(tag);
	if (ret != 0) {
		ret = -eAosRc_InvalidVLANTag;
		goto out;
	}
	ret = qos_rule_del_vlan(tag);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModVlan(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	char *priority = NULL;
	u8 level = 0;
	u16 tag;
	*length = 0;
	priority = parms->mStrings[0];
	tag = parms->mIntegers[0];
	ret = is_valid_vlantag(tag);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		goto out;
	}
	ret = qos_rule_mod_vlan(tag,level);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ShowVlan(char *data,
								  unsigned int *length,
								  struct aosKernelApiParms *parms,
								  char *errmsg,
								  const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_VLAN;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_spc_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"There is not any rule.\n");
	}
	return 0;
}
static int AosQoSRuleCli_ClearVlan(char *data,
								   unsigned int *length,
								   struct aosKernelApiParms *parms,
								   char *errmsg,
								   const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_VLAN;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}

static int AosQoSRuleCli_AddInterface(char *data,
									  unsigned int *length,
									  struct aosKernelApiParms *parms,
									  char *errmsg,
									  const int errlen)
{
	int ret = 0;
	char *dev_name = NULL;
	char *priority = NULL;
	int dev_id = 0;
	u8 level = 0;
	*length = 0;
	dev_name = parms->mStrings[0];
	priority = parms->mStrings[1];

	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	ret = is_valid_if(dev_name,&dev_id);
	if (ret != 0) {
		ret = -eAosRc_InvalidDevice;
		goto out;
	}
	ret = qos_rule_add_interface(dev_id,level,dev_name);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_DelInterface(char *data,
									  unsigned int *length,
									  struct aosKernelApiParms *parms,
									  char *errmsg,
									  const int errlen)
{
	int ret = 0;
	char *dev_name = NULL;
	int dev_id = 0;
	*length = 0;
	dev_name = parms->mStrings[0];
	ret = is_valid_if(dev_name,&dev_id);
	if (ret != 0) {
		ret = -eAosRc_InvalidDevice;
		goto out;
	}
	ret = qos_rule_del_interface(dev_id);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModInterface(char *data,
									  unsigned int *length,
									  struct aosKernelApiParms *parms,
									  char *errmsg,
									  const int errlen)
{
	int ret = 0;
	char *dev_name = NULL;
	char *priority = NULL;
	int dev_id = 0;
	u8 level = 0;
	*length = 0;
	dev_name = parms->mStrings[0];
	priority = parms->mStrings[1];
	
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		goto out;
	}

	ret = is_valid_if(dev_name,&dev_id);
	if (ret != 0) {
		goto out;
	}
	ret = qos_rule_mod_interface(dev_id,level,dev_name);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ShowInterface(char *data,
									   unsigned int *length,
									   struct aosKernelApiParms *parms,
									   char *errmsg,
									   const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_IF;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_spc_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"There is not any rule.\n");
	}
	return 0;
}

static int AosQoSRuleCli_ClearInterface(char *data,
										unsigned int *length,
										struct aosKernelApiParms *parms,
										char *errmsg,
										const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_IF;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}

static int AosQoSRuleCli_AddPort(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	char *priority = NULL;
	u8 level;
	char *app = NULL;
	char *ifname = NULL;
	int ifid;
	u16 port;
	struct net_device *dev = NULL;
	
	*length = 0;
	app = parms->mStrings[0];
	ifname = parms->mStrings[1];
	priority = parms->mStrings[2];
	port = parms->mIntegers[0];
	ret = is_valid_app(app);
	
	if (ret != 0) {
		ret = -eAosRc_InvalidApp;
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	
	dev = dev_get_by_name(ifname);
	if (dev) {
		ifid = dev->ifindex;
		dev_put(dev);
	} else {
		ret = -eAosRc_InvalidDevice;
		goto out;
	}

	port = htons(port);
	ret = qos_rule_add_port(ifid,port,level,app);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_DelPort(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	u16 port;
	struct net_device* dev = NULL;
	char *ifname = parms->mStrings[0];
	int ifid;
	*length = 0;
	
	dev = dev_get_by_name(ifname);
	if (dev) {
		ifid = dev->ifindex;
		dev_put(dev);
	} else {
		ret = -eAosRc_InvalidDevice;
		goto out;
	}
	
	port = parms->mIntegers[0];
	port = htons(port);

	ret = qos_rule_del_port(port,ifid);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModPort(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	int ret = 0;
	char *priority = NULL;
	u8 level;
	char *app = NULL;
	char *ifsending = NULL;
	u8 direction;
	u16 port;
	*length = 0;
	ifsending = parms->mStrings[0];
	app = parms->mStrings[1];
	priority = parms->mStrings[2];
	port = parms->mIntegers[0];
	ret = is_valid_app(app);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_direction(ifsending,&direction);
	if (ret != 0) {
		goto out;
	}

	port = htons(port);
	ret = qos_rule_mod_port(direction,port,level,app);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ClearPort(char *data,
								 unsigned int *length,
								 struct aosKernelApiParms *parms,
								 char *errmsg,
								 const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_PORT;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}
static int AosQoSRuleCli_ShowPort(char *data,
								  unsigned int *length,
								  struct aosKernelApiParms *parms,
								  char *errmsg,
								  const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_PORT;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_spc_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"There is not any rule.\n");
	}
	return 0;
}

static int AosQoSRuleCli_AddProtocol(char *data,
									 unsigned int *length,
									 struct aosKernelApiParms *parms,
									 char *errmsg,
									 const int errlen)
{
	int ret = 0;
	u8 proto;
	char *priority = NULL;
	u8 level = 0;
	*length = 0;
	priority = parms->mStrings[1];
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		ret = -eAosRc_InvalidProtocol;
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		ret = -eAosRc_InvalidPriority;
		goto out;
	}
	ret = qos_rule_add_protocol(proto,level);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	add_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_DelProtocol(char *data,
									 unsigned int *length,
									 struct aosKernelApiParms *parms,
									 char *errmsg,
									 const int errlen)
{
	int ret = 0;
	u8 proto;
	*length = 0;
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		ret = -eAosRc_InvalidProtocol;
		goto out;
	}
	ret = qos_rule_del_protocol(proto);
	if (ret == 0) {
		*length = 0;
		ros_qos_config_log(parms->mCmd);
		return 0;
	}
	ret = -eAosRc_Failed;
out:
	del_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return ret;
}
static int AosQoSRuleCli_ModProtocol(char *data,
									 unsigned int *length,
									 struct aosKernelApiParms *parms,
									 char *errmsg,
									 const int errlen)
{
	int ret = 0;
	u8 proto;
	char *priority = NULL;
	u8 level = 0;
	*length = 0;
	priority = parms->mStrings[1];
	ret = is_valid_proto(parms->mStrings[0],&proto);
	if (ret != 0) {
		goto out;
	}
	ret = is_valid_priority(priority,&level);
	if (ret != 0) {
		goto out;
	}
	ret = qos_rule_mod_protocol(proto,level);
	if (ret == 0) {
		*length = 0;
		return 0;
	}
out:
	mod_output(ret,errmsg);
	errmsg[errlen - 1] = 0;
	
	return -1;
}
static int AosQoSRuleCli_ShowProtocol(char *data,
									  unsigned int *length,
									  struct aosKernelApiParms *parms,
									  char *errmsg,
									  const int errlen)
{
	int optlen = *length;
	unsigned int rsltIndex =  0;
	char *rsltBuff = NULL;
	struct _qos_rule_show_arg arg;
	*length = 0;
	rsltBuff = aosKernelApi_getBuff(data);
	arg.type = AOS_QOS_RULE_PROTO;
	arg.rsltBuff = rsltBuff;
	arg.rsltIndex = rsltIndex;
	arg.optlen = optlen;
	qos_rule_show_count = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map(aos_qos_rule_spc_hash,
				show_single_rule,
				&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	*length = arg.rsltIndex;
	if (qos_rule_show_count == 0) {
		sprintf(errmsg,"There is not any rule.\n");
	}
	return 0;
}

static int AosQoSRuleCli_ClearProtocol(char *data,
									   unsigned int *length,
									   struct aosKernelApiParms *parms,
									   char *errmsg,
									   const int errlen)
{
	struct _qos_rule_clear_arg arg;
	*length = 0;
	arg.type = AOS_QOS_RULE_PROTO;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	ros_qos_config_log(parms->mCmd);
	return 0;
}

static int AosQos_registerRuleCli(void)
{
	int ret = 0;

	ret |= OmnKernelApi_addCliCmd("QosRuleAddGeneral",
		   						  AosQoSRuleCli_AddGeneral);
	if (ret != 0) {
		goto addgen_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleDelGeneral", 
								  AosQoSRuleCli_DelGeneral);
	if (ret != 0) {
		goto delgen_error;
	}

	ret |= OmnKernelApi_addCliCmd("QosRuleModGeneral", 
								  AosQoSRuleCli_ModGeneral);

	if (ret != 0) {
		goto modgen_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowGeneral", 
								  AosQoSRuleCli_ShowGeneral);
	if (ret != 0) {
		goto shwgen_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearGeneral", 
								  AosQoSRuleCli_ClearGeneral);
	if (ret != 0) {
		goto clrgen_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleAddMAC", 
								  AosQoSRuleCli_AddMAC);
	if (ret != 0) {
		goto addmac_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleDelMAC", 
								  AosQoSRuleCli_DelMAC);
	if (ret != 0) {
		goto delmac_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleModMAC", 
								  AosQoSRuleCli_ModMAC);
	if (ret != 0) {
		goto modmac_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowMAC", 
								  AosQoSRuleCli_ShowMAC);
	if (ret != 0) {
		goto shwmac_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearMAC", 
								  AosQoSRuleCli_ClearMAC);
	if (ret != 0) {
		goto clrmac_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleAddVlan", 
								  AosQoSRuleCli_AddVlan);
	if (ret != 0) {
		goto addvlan_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleDelVlan", 
								  AosQoSRuleCli_DelVlan);
	if (ret != 0) {
		goto delvlan_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleModVlan", 
								  AosQoSRuleCli_ModVlan);
	if (ret != 0) {
		goto modvlan_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowVlan", 
								  AosQoSRuleCli_ShowVlan);
	if (ret != 0) {
		goto shwvlan_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearVlan", 
								  AosQoSRuleCli_ClearVlan);
	if (ret != 0) {
		goto clrvlan_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleAddInterface", 
								  AosQoSRuleCli_AddInterface);
	if (ret != 0) {
		goto addif_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleDelInterface", 
								  AosQoSRuleCli_DelInterface);
	if (ret != 0) {
		goto delif_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleModInterface", 
								  AosQoSRuleCli_ModInterface);
	if (ret != 0) {
		goto modif_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowInterface", 
								  AosQoSRuleCli_ShowInterface);
	if (ret != 0) {
		goto shwif_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearInterface", 
								  AosQoSRuleCli_ClearInterface);
	if (ret != 0) {
		goto clrif_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleAddPort", 
								  AosQoSRuleCli_AddPort);
	if (ret != 0) {
		goto addport_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleDelPort", 
								  AosQoSRuleCli_DelPort);
	if (ret != 0) {
		goto delport_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleModPort", 
								  AosQoSRuleCli_ModPort);
	if (ret != 0) {
		goto modport_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowPort", 
								  AosQoSRuleCli_ShowPort);
	if (ret != 0) {
		goto shwport_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearPort", 
								  AosQoSRuleCli_ClearPort);
	if (ret != 0) {
		goto clrport_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleAddProtocol", 
								  AosQoSRuleCli_AddProtocol);
	if (ret != 0) {
		goto addpro_error;
	}

	ret |= OmnKernelApi_addCliCmd("QosRuleDelProtocol", 
								  AosQoSRuleCli_DelProtocol);
	if (ret != 0) {
		goto delpro_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleModProtocol", 
								  AosQoSRuleCli_ModProtocol);
	if (ret != 0) {
		goto modpro_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleShowProtocol", 
								  AosQoSRuleCli_ShowProtocol);
	if (ret != 0) {
		goto shwpro_error;
	}
	ret |= OmnKernelApi_addCliCmd("QosRuleClearProtocol", 
								  AosQoSRuleCli_ClearProtocol);
	if (ret != 0) {
		goto clrpro_error;
	}

	return ret;

clrpro_error:
	OmnKernelApi_delCliCmd("QosRuleShowProtocol");
shwpro_error:
	OmnKernelApi_delCliCmd("QosRuleModProtocol");
modpro_error:
	OmnKernelApi_delCliCmd("QosRuleDelProtocol");
delpro_error:
	OmnKernelApi_delCliCmd("QosRuleAddProtocol");
addpro_error:
	OmnKernelApi_delCliCmd("QosRuleClearPort");
clrport_error:
	OmnKernelApi_delCliCmd("QosRuleShowPort");
shwport_error:
	OmnKernelApi_delCliCmd("QosRuleModPort");
modport_error:
	OmnKernelApi_delCliCmd("QosRuleDelPort");
delport_error:
	OmnKernelApi_delCliCmd("QosRuleAddPort");
addport_error:
	OmnKernelApi_delCliCmd("QosRuleClearInterface");
clrif_error:
	OmnKernelApi_delCliCmd("QosRuleShowInterface");
shwif_error:
	OmnKernelApi_delCliCmd("QosRuleModInterface");
modif_error:
	OmnKernelApi_delCliCmd("QosRuleDelInterface");
delif_error:
	OmnKernelApi_delCliCmd("QosRuleAddInterface");
addif_error:
	OmnKernelApi_delCliCmd("QosRuleClearVlan");
clrvlan_error:
	OmnKernelApi_delCliCmd("QosRuleShowVlan");
shwvlan_error:
	OmnKernelApi_delCliCmd("QosRuleModVlan");
modvlan_error:
	OmnKernelApi_delCliCmd("QosRuleDelVlan");
delvlan_error:
	OmnKernelApi_delCliCmd("QosRuleAddVlan");
addvlan_error:
	OmnKernelApi_delCliCmd("QosRuleClearMAC");
clrmac_error:
	OmnKernelApi_delCliCmd("QosRuleShowMAC");
shwmac_error:
	OmnKernelApi_delCliCmd("QosRuleModMAC");
modmac_error:
	OmnKernelApi_delCliCmd("QosRuleDelMAC");
delmac_error:
	OmnKernelApi_delCliCmd("QosRuleAddMAC");
addmac_error:
	OmnKernelApi_delCliCmd("QosRuleClearGeneral");
clrgen_error:
	OmnKernelApi_delCliCmd("QosRuleShowGeneral");
shwgen_error:
	OmnKernelApi_delCliCmd("QosRuleModGeneral");
modgen_error:
	OmnKernelApi_delCliCmd("QosRuleDelGeneral");
delgen_error:
	OmnKernelApi_delCliCmd("QosRuleAddGeneral");
addgen_error:
	return ret;
}

static void AosQos_unregisterRuleCli(void)
{
	OmnKernelApi_delCliCmd("QosRuleAddGeneral");
	OmnKernelApi_delCliCmd("QosRuleDelGeneral");
	OmnKernelApi_delCliCmd("QosRuleModGeneral");
	OmnKernelApi_delCliCmd("QosRuleShowGeneral");
	OmnKernelApi_delCliCmd("QosRuleClearGeneral");
	OmnKernelApi_delCliCmd("QosRuleAddMAC");
	OmnKernelApi_delCliCmd("QosRuleDelMAC");
	OmnKernelApi_delCliCmd("QosRuleModMAC");
	OmnKernelApi_delCliCmd("QosRuleShowMAC");
	OmnKernelApi_delCliCmd("QosRuleClearMAC");
	OmnKernelApi_delCliCmd("QosRuleAddVlan");
	OmnKernelApi_delCliCmd("QosRuleDelVlan");
	OmnKernelApi_delCliCmd("QosRuleModVlan");
	OmnKernelApi_delCliCmd("QosRuleShowVlan");
	OmnKernelApi_delCliCmd("QosRuleClearVlan");
	OmnKernelApi_delCliCmd("QosRuleAddInterface");
	OmnKernelApi_delCliCmd("QosRuleDelInterface");
	OmnKernelApi_delCliCmd("QosRuleModInterface");
	OmnKernelApi_delCliCmd("QosRuleShowInterface");
	OmnKernelApi_delCliCmd("QosRuleClearInterface");
	OmnKernelApi_delCliCmd("QosRuleAddPort");
	OmnKernelApi_delCliCmd("QosRuleDelPort");
	OmnKernelApi_delCliCmd("QosRuleModPort");
	OmnKernelApi_delCliCmd("QosRuleShowPort");
	OmnKernelApi_delCliCmd("QosRuleClearPort");
	OmnKernelApi_delCliCmd("QosRuleAddProtocol");
	OmnKernelApi_delCliCmd("QosRuleDelProtocol");
	OmnKernelApi_delCliCmd("QosRuleModProtocol");
	OmnKernelApi_delCliCmd("QosRuleShowProtocol");
	OmnKernelApi_delCliCmd("QosRuleClearProtocol");
	
	return;
}
int AosQos_initRuleModule(void)
{
	// 
	// Chen Ding, 01/10/2007
	//
	static int lsRulesRegistered = 0;

	int ret = 0;

	// 
	// Chen Ding, 01/10/2007
	//
	if (!lsRulesRegistered)
	{
		ret = AosQos_registerRuleCli();
		lsRulesRegistered = 1;
	}

	if (ret != 0) {
		QOS_RULE_PRINTK("Init failed\n");
		return ret;
	}
	ret = aos_qos_rule_init_variables();
	if (ret != 0) {
		goto fail;
	}
	return ret;	
fail:
	AosQos_unregisterRuleCli();
	return ret;
}


void AosQos_exitRuleModule(void)
{
	aos_qos_rule_destroy();
	AosQos_unregisterRuleCli();
	return;
}



#ifdef __KERNEL__
module_init(AosQos_initRuleModule);
module_exit(AosQos_exitRuleModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ping Wang");
#endif 
