
#include "KernelSimu/netfilter.h"
#include "KernelSimu/netfilter_ipv4.h"
#include "KernelSimu/socket.h"
#include "KernelSimu/module.h"
#include "KernelSimu/delay.h"
#include "KernelSimu/thread.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include <linux/errno.h>
#include "QoS/qos_nf.h"
#include "QoS/qos_util.h"
#include "QoS/qos_global.h"
struct _qos_test_cell {
	struct aos_list_head entry;
	struct nf_info *info;
	struct sk_buff *skb;
};
static struct nf_hook_ops test_hook_opin;
struct aos_list_head test_head;
int stop_signal;
int wakeup_signal; 
#ifdef __KERNEL__
wait_queue_head_t test_wait_queue;
struct nf_queue_handler test_handler_struc;
#endif


void test_init_variables(void)
{
	aos_trace("enter %s", __FUNCTION__);

#ifdef __KERNEL__
	init_waitqueue_head(&test_wait_queue);
#endif
	AOS_INIT_LIST_HEAD(&test_head);
	stop_signal = 0;
	wakeup_signal = 0;

	return;
}


void test_reinject_all_pkts(void)
{
	struct aos_list_head *cur,*next;
	struct _qos_test_cell *local_cell;

	aos_trace("enter %s", __FUNCTION__);

#ifdef __KERNEL__
	local_bh_disable();
#endif

	aos_list_for_each_safe(cur,next,&test_head) {
		local_cell = aos_list_entry(cur,struct _qos_test_cell,entry);
		aos_list_del(cur);
		nf_reinject(local_cell->skb, local_cell->info,NF_ACCEPT);
		aos_free(local_cell);
	}

#ifdef __KERNEL__
	local_bh_enable();
#endif

	return;
}


int test_queue_reinject_thread(void *arg)
{
	int ready_to_go;

	aos_trace("enter %s", __FUNCTION__);

	ready_to_go = 0;

	while (1) {
		if (stop_signal != 0) {
			aos_trace("test thread is to remove");
			ready_to_go = 1;
		}

		test_reinject_all_pkts();

		if (ready_to_go != 0) {
			goto out;
		}
#ifdef __KERNEL__
		msleep(1);
#endif
	}

out:
	stop_signal = 0;
	wakeup_signal = 1;
#ifdef __KERNEL__
	wake_up_interruptible(&test_wait_queue);
#endif

	return 0;
}


//1 Means queued
//0 Means droped
int test_queue_handler(struct sk_buff *skb,struct nf_info *info,
		unsigned int queuenum, void *data)
{
	struct _qos_test_cell *local_cell;

	aos_trace("enter %s", __FUNCTION__);

#ifdef __KERNEL__
	local_cell = aos_malloc_atomic(sizeof(struct _qos_test_cell));
#else
	local_cell = (struct _qos_test_cell *)aos_malloc(sizeof(struct _qos_test_cell));
#endif

	if (local_cell == NULL) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Out of Memory");
		return 0;
	}

	AOS_INIT_LIST_HEAD(&local_cell->entry);
	local_cell->skb = skb;
	local_cell->info = info;
	aos_list_add_tail(&local_cell->entry,&test_head);

	return 1;
}


void stop_working_thread(void)
{
	aos_trace("enter %s", __FUNCTION__);

	stop_signal = 1;
#ifdef __KERNEL__
	wait_event_interruptible(test_wait_queue,wakeup_signal);
#endif
	test_init_variables();

	return;
}

#ifdef __KERNEL__
int init_test_handler_struc(struct nf_queue_handler *teststru,
							void *data, 
							char *name)
{
	aos_trace("enter %s", __FUNCTION__);

	aos_assert1(teststru);

	teststru->outfn = test_queue_handler;
	teststru->data = data;
	teststru->name = name;

	return 0;
}
#endif


#ifdef __KERNEL__
static unsigned int test_qos_fnout(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#else
unsigned int test_qos_fnout(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#endif // #ifdef __KERNEL__
{
	aos_trace("enter %s", __FUNCTION__);

	return NF_ACCEPT;
}


#ifdef __KERNEL__
static unsigned int test_qos_fnin(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#else
unsigned int test_qos_fnin(unsigned int hook,
							 struct sk_buff **pskb,
							 const struct net_device *in,
							 const struct net_device *out,
							 int (*okfn)(struct sk_buff*))
#endif // #ifdef __KERNEL__
{
	//aos_trace("Test Success!");
	aos_trace("Test QUEUE!");

	return NF_QUEUE;
}


int test_init_hook_op_parms(struct nf_hook_ops *hook_op,int hook)
{
	aos_trace("enter %s", __FUNCTION__);

	switch (hook) {
		case NF_IP_PRE_ROUTING:
			hook_op->hook = test_qos_fnin;
			break;

		case NF_IP_POST_ROUTING:
			hook_op->hook = test_qos_fnout;
			break;

		default:
			aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Invalid parm");
			return -EINVAL; 
	}

	hook_op->list.prev = NULL;
	hook_op->list.next = NULL;
	hook_op->pf = PF_INET;
	hook_op->owner = THIS_MODULE;
	hook_op->hooknum = hook;
	hook_op->priority = NF_IP_PRI_FILTER;
	return 0;
}

//Begin Thread first
//Register Netfilter second


int AosQos_initTestModule(void)
{
	int ret;

	aos_min_log(eAosMD_QoS,"Test Reinject Init");

	test_init_variables();	
#ifdef __KERNEL__
	ret = aos_thread_create(test_queue_reinject_thread,NULL,CLONE_KERNEL);
#else
	aos_thread_create(test_queue_reinject_thread,NULL,CLONE_KERNEL);
#endif

/*	if (ret != 0) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Creating Thread Error with value %d",ret);
		return ret;
	}*/
#ifdef __KERNEL__
	ret = init_test_handler_struc(&test_handler_struc,NULL,NULL);

	ret = test_init_hook_op_parms(&test_hook_opin,NF_IP_PRE_ROUTING);
/////////////////////////////////////////
	ret = nf_register_hook(&test_hook_opin);
#endif

	if (ret != 0) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Registering Netfilter Error with value %d.",ret);
		goto fail;
	}
#ifdef __KERNEL__
	ret = nf_register_queue_handler(PF_INET, &test_handler_struc);
#endif

	if (ret !=0) {
		aos_alarm(eAosMD_QoS,eAosAlarm_ProgErr,"Registering Queue Handler with value %d",ret);
		goto fail1;
	}

	return ret;

fail1:
	nf_unregister_hook(&test_hook_opin);
fail:
	stop_working_thread();
	return ret;
}

//UnRegister Netfilter First
//Stop Thread Second

void AosQos_exitTestModule(void)
{
	aos_trace("enter %s", __FUNCTION__);
#ifdef __KERNEL__
	nf_unregister_queue_handler(PF_INET);
	nf_unregister_hook(&test_hook_opin);
#endif
	stop_working_thread();

	return;
}


static __init int qostest_init(void)
{
	int ret;

	aos_trace("enter %s", __FUNCTION__);

	//ret = AosQos_initTestModule();

	return ret;

}


static __exit void qostest_exit(void)
{
	aos_trace("enter %s", __FUNCTION__);

	//AosQos_exitTestModule();
	return;
}


#ifdef __KERNEL__
module_init(qostest_init);
module_exit(qostest_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ping Wang");
#endif
