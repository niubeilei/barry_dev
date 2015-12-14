#include "KernelSimu/netfilter.h"
#include "KernelSimu/netfilter_ipv4.h"
#include "KernelSimu/socket.h"
#include "KernelSimu/module.h"
#include <linux/errno.h>
#include "QoS/qos_nf.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
#include "QoS/qos_bw_man.h"
#include "QoS/qos_logstat.h"

//struct nf_queue_handler qos_queue_handler_struc;


static inline int is_lo(int ifindex)
{
	struct net_device *dev;
	
	dev = dev_get_by_name("lo");
	if (dev) {
		if (dev->ifindex == ifindex)
		{
			aos_trace("lo index %d", ifindex);
			dev_put(dev);
			return 1;
		}
		dev_put(dev);
		return 0;
	}
	return 1;
}
static int qos_bandwidth_update(int nf_dir, int direction, struct sk_buff *skb, 
								struct ros_qos_skb_info *skb_info,u8 *priority)
{
	//struct ros_qos_skb_info skb_info;
	//int ret;
	aos_assert1(skb);
	//ret = qos_retrieve_info(skb, &skb_info, direction);
	//if (ret)
	//{
	//	return ret;
	//}
	qos_bandwidth_monitor(nf_dir, direction, skb_info, priority);
	aos_qos_channel_put(skb_info->ch);

	return 0;
}


static unsigned int qos_fn_post_routing(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
{
	int ret = NF_ACCEPT;
	struct ros_qos_skb_info skb_info;
	struct _qos_traffic_channel *ch = NULL;
	u8 priority;
	
	//printk(KERN_ERR "enter function %s", __FUNCTION__);
	qos_traffic_parse_parm(*pskb, &skb_info, direction_out); 
		
	priority = qos_get_priority_of_channel(&skb_info, direction_out, &ch);
		
	if (priority < eROS_QOS_PRIORITY_MAX)
	{
		ret = qos_traffic_control(*pskb, (*pskb)->len, direction_out, priority);
	}
	
	return ret;
}


static struct nf_hook_ops hook_pre_routing;
static struct nf_hook_ops hook_local_in;
//static struct nf_hook_ops hook_local_out;
//static struct nf_hook_ops hook_forward;
static struct nf_hook_ops hook_post_routing;


static unsigned int qos_fn_local_in(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
{
	int ret;
	u8 priority;
	struct ros_qos_skb_info skb_info;
	
	ret = NF_ACCEPT;

	if (aos_qos_traffic_facility_status != AOS_QOS_TRAFFIC_STATUS_ON) {
		return ret;
	}
	
	if (*pskb && is_lo((*pskb)->dev->ifindex))
		return NF_ACCEPT;
	
	qos_traffic_parse_parm(*pskb, &skb_info, direction_in); 
	
	
	qos_bandwidth_update(local_in, direction_in, *pskb, &skb_info, &priority);
	
	return ret;

}


#ifdef __KERNEL__
static unsigned int qos_fn_pre_routing(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#else
unsigned int qos_fn_pre_routing(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#endif // #ifdef __KERNEL__
{
	struct _qos_traffic_channel *ch = NULL;
	int ret;
	u8 priority;
	struct ros_qos_skb_info skb_info;

	ret = NF_ACCEPT;

	if (aos_qos_traffic_facility_status != AOS_QOS_TRAFFIC_STATUS_ON) {
		return ret;
	}
	
	if (*pskb && is_lo((*pskb)->dev->ifindex))
		return NF_ACCEPT;

	if (monitor_test_direction & monitor_direction_in){
		aos_trace("enter %s", __FUNCTION__);
		qos_traffic_parse_parm(*pskb, &skb_info, direction_in); 
		
		priority = qos_get_priority_of_channel(&skb_info, direction_in, &ch);
		
		if (priority < eROS_QOS_PRIORITY_MAX)
		{
			ret = qos_traffic_control(*pskb, (*pskb)->len, direction_in, priority);
		}
		
		if (ret)
		{
			qos_traffic_monitor(*pskb, &skb_info, direction_in, &priority);
		}

		// statistics for interface
		ros_qos_priority_stat((*pskb)->dev->ifindex, direction_in, ret, priority, (*pskb)->len);

		// statistics for channel
		if (ch)
		{
			ros_qos_channel_stat(ch, direction_in, ret, (*pskb)->len);
			aos_qos_channel_put(ch);
		}
		//
		// update bandwidth of interface
		// update bandwidth of priority
		aos_trace("enter %s ifid %d", __FUNCTION__, (*pskb)->dev->ifindex);
		qos_bandwidth_update(pre_routing, direction_in, *pskb, &skb_info, &priority);

	}
	return ret;
}

int init_hook_op_parms(struct nf_hook_ops *hook_op,int hook, int priority)
{
	switch (hook) {
		case NF_IP_FORWARD:
			//hook_op->hook = qos_fnin;
			break;
			
		case NF_IP_PRE_ROUTING:
			hook_op->hook = qos_fn_pre_routing;
			break;
			
		case NF_IP_POST_ROUTING:
			hook_op->hook = qos_fn_post_routing;
			break;
			
		case NF_IP_LOCAL_OUT:
			//hook_op->hook = qos_fnout;
			break;
		case NF_IP_LOCAL_IN:
			hook_op->hook = qos_fn_local_in;
			break;
		default:
			QOS_MODULE_PRINTK("ERROR:Invalid parm\n");
			return -EINVAL; 
	}
	hook_op->list.prev = NULL;
	hook_op->list.next = NULL;
	hook_op->pf = PF_INET;
	hook_op->owner = THIS_MODULE;
	hook_op->hooknum = hook;
	//hook_op->priority = NF_IP_PRI_BRIDGE_SABOTAGE_FORWARD - 1; 
	hook_op->priority = priority;
	return 0;
}


int AosQos_initNFModule(void)
{
	int ret = 0;

	QOS_MODULE_PRINTK("Test\n");
	
	init_hook_op_parms(&hook_pre_routing, NF_IP_PRE_ROUTING, NF_IP_PRI_FILTER);
	init_hook_op_parms(&hook_local_in, NF_IP_LOCAL_IN, NF_IP_PRI_FILTER);
	//init_hook_op_parms(&hook_local_out,NF_IP_LOCAL_OUT);
	//init_hook_op_parms(&hook_forward, NF_IP_FORWARD);
	init_hook_op_parms(&hook_post_routing, NF_IP_POST_ROUTING, NF_IP_PRI_FILTER);
#ifdef __KERNEL__
	ret |= nf_register_hook(&hook_pre_routing);
	ret |= nf_register_hook(&hook_local_in);
	//ret |= nf_register_hook(&hook_local_out);
	//ret = nf_register_hook(&hook_forward);
	ret |= nf_register_hook(&hook_post_routing);
#endif
	if (ret != 0) {
		QOS_MODULE_PRINTK("Registering Netfilter Error\n");
	}

	/*qos_queue_handler_struc.outfn = qos_nf_queue_handler;
	qos_queue_handler_struc.data = NULL;
	qos_queue_handler_struc.name = NULL;

	ret = nf_register_queue_handler(PF_INET, &qos_queue_handler_struc);
	if (ret != 0) 
	{
		aos_alarm(eAosMD_QoS, eAosAlarm_ProgErr, "Registering Queue Handler with value %d",ret);
	}*/
	
	return ret;
}


void AosQos_exitNFModule(void)
{
	nf_unregister_hook(&hook_pre_routing);
	
	nf_unregister_hook(&hook_local_in);
	//nf_unregister_hook(&hook_local_out);
	//nf_unregister_hook(&hook_forward);
	nf_unregister_hook(&hook_post_routing);
	return;
}
