/*#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ip.h>
#include <linux/tcp.h>

#include <net/ip.h>
#include <net/tcp.h>
#include <net/udp.h>

#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
*/

#include <KernelSimu/module.h>
#include <KernelSimu/kernel.h>

#include <KernelSimu/ip.h>
#include <KernelSimu/tcp.h>
#include <KernelSimu/udp.h>

#include <KernelSimu/netfilter.h>
#include <KernelSimu/netfilter_ipv4.h>

#include "inet_ctrl_cli.h"
#include "inet_ctrl_hash.h"
#include "inet_ctrl_timer.h"

#define	HASHTAB_SIZE		64

#define	SWAP_TUPLE(tuple) do { \
	(tuple)->src_ip = ((tuple)->src_ip) ^ ((tuple)->dst_ip); \
	(tuple)->dst_ip = ((tuple)->src_ip) ^ ((tuple)->dst_ip); \
	(tuple)->src_ip = ((tuple)->src_ip) ^ ((tuple)->dst_ip); \
	(tuple)->src_port = ((tuple)->src_port) ^ ((tuple)->dst_port); \
	(tuple)->dst_port = ((tuple)->src_port) ^ ((tuple)->dst_port); \
	(tuple)->src_port = ((tuple)->src_port) ^ ((tuple)->dst_port); \
																	\
	INETCTRL_INFO("Swapped Packet: %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", \
			NIPQUAD((tuple)->src_ip), (tuple)->src_port,		\
			NIPQUAD((tuple)->dst_ip), (tuple)->dst_port);		\
																\
} while(0)

#define IS_DNS_TRAFFIC(tuple) (((tuple)->dst_port == 53 || (tuple)->src_port == 53) \
	&& ((tuple)->proto == FLOW_TYPE_UDP))

/* for checking if time in the range */
typedef struct time_arg {
	u32 secofday;
	u32 dayofweek;
} time_arg_t;

/*
 * walk throuth the list of timectrl, 0 means continuing, 
 * -1 means quitting due to error, 1 means matched 
 */
static int walk_timectrl_list(void* p, void* arg)
{
	time_ctrl_t* timectrl = (time_ctrl_t*)aos_list_entry((struct aos_list_head*)p, 
		time_ctrl_t, list);
	time_arg_t* targ = (time_arg_t*)arg;
	
	if (timectrl == NULL)
		return -1; /* error quit */
	
	if (FLAG_ISSET(timectrl->time_days, DayMask(targ->dayofweek))) {
		if ((targ->secofday >= timectrl->time_start) &&
			(targ->secofday < timectrl->time_end))
			return 1; /* matched quit */
		else
			return 0; /* not in time range, so denied */
	}

	return 0; /* continue */
}

/* 0 means permitted, < 0 means denied */
static int check_is_intime(rule_assoc_t* passo)
{
	int ret;
	time_arg_t arg;
	time_rule_t *timerule;
	
	if (passo == NULL)
		return -1;
	
	/* Has been deleted */
	if (FLAG_ISSET(passo->assoc_flag, FLAG_PRE_DELETE))
		return -1;
		
	memset(&arg, 0, sizeof(arg));
	arg.secofday = get_secoftoday();
	arg.dayofweek = get_weekoftoday();
	
	INETCTRL_INFO("secs %u, day %u\n", arg.secofday, arg.dayofweek);
	
	timerule = passo->assoc_time;
	ret = walk_sub_list(&timerule->rule_time_head, walk_timectrl_list, &arg);
	switch (ret) {
		case 1: /* in time range, let it go */
			INETCTRL_INFO("In time range, permit it\n");
			break;

		case 0: /* not any rule matched, apply default policy, also denied */
		case -1: /* error happened, or not in time range, denied */
		default: /* other cases, denied */
			INETCTRL_ERR("Not in time range, deny it\n");
			return -1;
	}
	
	return 0;
}

/* 0 means permitted, < 0 means denied */
static int check_is_inonline(state_entry_t* entry)
{
	u32 secs;
	state_entry_data_t* entry_data = entry->st_data;
	file_record_t* fr = &entry_data->data_rec;
	
	secs = get_secoftoday();
	if (FLAG_ISSET(fr->rec_flag, TYPE_TIME_WEEK)) {

		INETCTRL_INFO("Enter week online control, %u\n", fr->rec_total_timeleft);
		
		if (fr->rec_total_timeleft < (secs - entry_data->data_enter))
			return -1;
	}
	
	if (FLAG_ISSET(fr->rec_flag, TYPE_TIME_DAY)) {

		INETCTRL_INFO("Enter day online control, %u\n", fr->rec_timeleft);

		if (secs > entry->st_update) {
			if (fr->rec_timeleft < (secs - entry_data->data_enter))
				return -1;
		}
	}
	
	INETCTRL_INFO("In online control, permit it\n");
	return 0;
}

/* 
 * for now, only support user identified by IP 
 */
static int
create_state_byrule(five_tuple_t* tuple)
{
	user_group_t* grp;
	rule_assoc_t* asso;
	user_account_t* account;
	state_entry_t* entry;
	state_entry_data_t* entry_data;
	int ret;

	if (match_group_by_ip(tuple->src_ip, 0xffffffff, &grp, &account) == 0) {
		inetctrl_log(default_policy, tuple, "User no find, go default policy\n");
		return default_policy; /* not this rule */
	}
	
	INETCTRL_INFO("Find the user\n");

	/* match rule */
	ret = match_rule_ingroup(tuple, grp, &asso);
	switch (ret) {
		case ERR_OK:
			break;

		case ERR_UNSUPPORT_TYPE:
		default:
			INETCTRL_ERR("Not found the rule association\n");
			inetctrl_log(default_policy, tuple, "Not found the rule association, go default policy\n");

			return default_policy;	/* not matched */
	}

	if (check_is_intime(asso) < 0) { /* not in */
   	    inetctrl_log(BLOCK, tuple, "Checking time range failed\n");
		return -1;
	}

	/* go here, meaning we can create the state entry */
	entry_data = get_state_data(asso, account, 1);
	if (entry_data == NULL)
		return -1; /* error happened, deny */

	INETCTRL_INFO("Here, got the state data: timeleft %u, totaltimeleft %u\n", entry_data->data_timeleft,
				  entry_data->data_total_timeleft);
	
	if (!ENTRY_DATA_GOOD(entry_data)) {
		inetctrl_log(default_policy, tuple, "Data be removed, go default policy\n");
		return default_policy;
	}

	if ((entry_data->data_timeleft == 0) || (entry_data->data_total_timeleft == 0)) {
		inetctrl_log(BLOCK, tuple, "Checking online failed\n");
		return -1;
	}

	entry = inet_ctrl_hash_search(tuple, 1);
	if (entry == NULL)
		return -1;
		
	/* here, MUST not set memory to 0 */
	entry->st_data = entry_data;
	atomic_inc(&entry_data->data_refcnt);

	/* very important, Must */
	FLAG_UNSET(entry_data->data_flag, DATA_PRE_DELETE);
	entry->st_access = entry->st_update = entry->st_start = get_secoftoday();

	INETCTRL_INFO("Create a new state\n");
	return 0;
}

/* 0 means permitted, < 0 means denied. */
static int
check_state(state_entry_t* state)
{
	rule_assoc_t* passo;
	state_entry_data_t* data = state->st_data;
	
	if (data == NULL) {/* should never happen */
		inetctrl_log(BLOCK, &(state->st_tuple), "state date empty\n");
		return -1;
	}
	
	if (!ENTRY_DATA_GOOD(data)) {
		FLAG_SET(state->st_flag, ENTRY_PRE_DELETE);
		/* inet_ctrl_hash_remove(&state->st_tuple); */
		inetctrl_log(default_policy, &(state->st_tuple), "Data be removed, go default policy\n");
		return ((default_policy==ALLOW)?0:-1);
	}

	state->st_access = get_secoftoday();
	passo = state->st_data->data_assoc;
	if (passo == NULL || passo->assoc_time == NULL) {
		inetctrl_log(ALLOW, &(state->st_tuple), "No rule, of cource go\n");
		return 0; /* no rule, of cource go */
	}

	/* check if now is in time range */
	if (check_is_intime(passo) < 0) { /* not in */
		INETCTRL_ERR("Checking time range failed\n");
		inetctrl_log(BLOCK, &(state->st_tuple), "Checking time range failed\n");

		FLAG_SET(state->st_flag, ENTRY_PRE_DELETE);
		/* inet_ctrl_hash_remove(&state->st_tuple); */
		return -1;
	}

	if (check_is_inonline(state) < 0) {/* left time expired */
		INETCTRL_ERR("Checking online failed\n");
		inetctrl_log(BLOCK, &(state->st_tuple), "Checking online failed\n");

		FLAG_SET(state->st_flag, ENTRY_PRE_DELETE);
		/* inet_ctrl_hash_remove(&state->st_tuple); */
		return -1;
	}
	
	INETCTRL_INFO("Checking state passed\n");
	inetctrl_log(ALLOW, &(state->st_tuple), "Checking state passed\n");
	return 0;
}

static int
inetctrl_access_check(struct sk_buff *skb)
{
	struct iphdr *iph;
	struct tcphdr* tcph;
	five_tuple_t tuple;
	state_entry_t* state;
	
	iph = skb->nh.iph;
	tcph = (struct tcphdr*)(iph + 1);
	
	tuple.proto = iph->protocol;
	tuple.src_ip = iph->saddr;
	tuple.dst_ip = iph->daddr;
	tuple.src_port = ntohs(tcph->source);
	tuple.dst_port = ntohs(tcph->dest);
	
	INETCTRL_INFO("Packet:proto %u, %u.%u.%u.%u:%u->%u.%u.%u.%u:%u\n", tuple.proto,
				  NIPQUAD(tuple.src_ip), tuple.src_port, NIPQUAD(tuple.dst_ip), 
				  tuple.dst_port);

	/* dns packet, allow it */
	if (IS_DNS_TRAFFIC(&tuple)) {

		INETCTRL_INFO("DNS packet, let go\n");
		inetctrl_log(ALLOW, &tuple, "DNS packet, let go\n");

		return 0;
	}

try_again:
	state = inet_ctrl_hash_search(&tuple, 0);
	/* we should check the incoming packet from server if not searched */
	if (state == NULL) {
		SWAP_TUPLE(&tuple);
		state = inet_ctrl_hash_search(&tuple, 0);
		/* don't forget to swap back */
		SWAP_TUPLE(&tuple);
	}

	/* if close or reset, remove the state */
	if (iph->protocol == FLOW_TYPE_TCP && (tcph->fin == 1 || tcph->rst == 1)) {
		if (state) {
			state_entry_data_t* data = state->st_data;
			
			inet_ctrl_hash_remove(&state->st_tuple);
			if (FLAG_ISSET(data->data_flag, DATA_PRE_DELETE)) {
				int cur_time = get_secoftoday();
				
				data->data_timeleft -= cur_time - data->data_enter;
				data->data_total_timeleft -= cur_time - data->data_enter;
			}
		}

		INETCTRL_INFO("tcp close or reset, without a state\n");
		inetctrl_log(ALLOW, &tuple, "tcp close or reset, without a state\n");
		return 0; /* accept it */
	}
	
	/* no state entry found, then matching rule */
	if (state == NULL) { 

		INETCTRL_INFO("checking twice(c->s and s->c), not found entry\n");
		
		int ret = create_state_byrule(&tuple); 

		if (ret > 0) 
			return ((default_policy==ALLOW)?0:-1);
		else if (ret < 0) 
			return -1;
		else 	
			goto try_again;

	} else if (FLAG_ISSET(state->st_flag, ENTRY_PRE_DELETE)) {

		INETCTRL_INFO("Entry is set to DELETE\n");
		inetctrl_log(BLOCK, &tuple, "Entry is set to DELETE\n");
		
		return -1;
	}
	
	return check_state(state);
}

static unsigned int
inetctrl_forward(unsigned int hooknum, struct sk_buff **pskb,
	 const struct net_device *in, const struct net_device *out,
	 int (*okfn)(struct sk_buff *))
{
	struct sk_buff *skb = *pskb;
	
	if (inetctrl_switch == 0)
		return NF_ACCEPT;
	
	if (skb->nh.iph->protocol != FLOW_TYPE_TCP && 
		skb->nh.iph->protocol != FLOW_TYPE_UDP)
		return NF_ACCEPT;
		
	if (inetctrl_access_check(skb) < 0)
		return NF_DROP;

	return NF_ACCEPT;
}

#ifdef	__KERNEL__
static struct nf_hook_ops inetctrl_forward_ops = {
	.hook		= inetctrl_forward,
	.owner		= THIS_MODULE,
	.pf			= PF_INET,
	.hooknum    = NF_IP_FORWARD,
	.priority   = NF_IP_PRI_FILTER,
};
#else
static struct nf_hook_ops inetctrl_forward_ops;

void inetctrl_init_hook()
{
	inetctrl_forward_ops.hook = inetctrl_forward;
	inetctrl_forward_ops.owner = THIS_MODULE;
	inetctrl_forward_ops.pf	= PF_INET;
	inetctrl_forward_ops.hooknum = NF_IP_FORWARD;
	inetctrl_forward_ops.priority = NF_IP_PRI_FILTER;
}
#endif

static int __init inetctrl_init(void)
{
	int ret;

	ret = reg_inetctrl_cli();
	if (ret < 0) {
		INETCTRL_ERR("can't setup application helper.\n");
		goto cleanup_nothing;
	}
	
	ret = inet_ctrl_tabinit(HASHTAB_SIZE);
	if (ret < 0) {
		INETCTRL_ERR("can't setup control.\n");
		goto cleanup_nothing;
	}
	
#ifndef __KERNEL__
	inetctrl_init_hook();
#else
	ret = nf_register_hook(&inetctrl_forward_ops);
	if (ret < 0) {
		INETCTRL_ERR("can't register in hook.\n");
		goto cleanup_hashtab;
	}
	
	init_inetctrl_vars();
#endif
	
	start_writefile_thread();
	
	INETCTRL_INFO("inetctrl loaded.\n");
	return ret;

cleanup_hashtab:
	inet_ctrl_tabexit();

cleanup_nothing:
	return ret;
}

static void __exit inetctrl_cleanup(void)
{
	nf_unregister_hook(&inetctrl_forward_ops);
	inet_ctrl_tabexit();
	stop_writefile_thread();
	
	INETCTRL_INFO("inetctrl unloaded.\n");
}

module_init(inetctrl_init);
module_exit(inetctrl_cleanup);
MODULE_LICENSE("GPL");

