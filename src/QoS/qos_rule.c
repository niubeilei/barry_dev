//#include <linux/kernel.h>
#include "KernelSimu/errno.h"
#include "KernelSimu/atomic.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "QoS/qos_rule.h"
#include "QoS/hashtab.h"
#include "QoS/qos_global.h"
#include "QoS/qos_util.h"
#include "QoS/qos_traffic.h"
#include "QoS/qos_logstat.h"

int qos_rule_show_count = 0;
struct hashtab *aos_qos_rule_gen_hash;
struct hashtab *aos_qos_rule_spc_hash;

int gen_rul_size = 0;
int mac_rul_size = 0;
int vlan_rul_size = 0;
int if_rul_size = 0;
int port_rul_size = 0;
int proto_rul_size = 0;
extern int dbg_print_count;
extern void * match_in_all_rule(struct _qos_traffic_channel* cell);

char* get_pri_str(u8 pri)
{
	switch(pri) {
		case AOS_QOS_RULE_PRIORITY_HIGH:
			return "High";
		case AOS_QOS_RULE_PRIORITY_MEDIUM:
			return "Medium";
		case AOS_QOS_RULE_PRIORITY_LOW:
			return "Low";
	}
	return "Invalid Priority";
}

/*
static char* get_port_direction_str(u8 direction)
{
	switch (direction) {
		case AOS_QOS_RULE_DIRECTION_SENDING:
			return "Sending";
		case AOS_QOS_RULE_DIRECTION_RECEIVING:
			return "Receiving";
	}
	return "Invalid Direction";
}
*/

static char* get_proto_str(u8 proto)
{
	switch (proto) {
		case AOS_QOS_TCP:
			return "TCP";
		case AOS_QOS_UDP:
			return "UDP";
	}
	return "Not Support";
}


void set_zero(struct _qos_rule_key *k)
{
	memset(k,0,sizeof(struct _qos_rule_key));
	return;
}

u32 cal_hash_value(struct hashtab *h,void *key)
{
	struct _qos_rule_key *k = NULL;
	u32 value = 0;
	int i;
	u8 *pos;

//	QOS_RULE_PRINTK("Entering to cal hash value\n");
	k = (struct _qos_rule_key *)key;
	pos = &(k->elem[0]);
	value += (~(*pos)) << 1;
	++pos;
	pos = &(k->elem[1]);
	value += (~(*pos)) << 1;
	++pos;
	for (i = 2; i < AOS_QOS_KEY_LENGTH;) {
		value += *pos;
		++pos;
		++i;
	}
	value &= ((1 << AOS_QOS_RULE_HASH_SIZE) - 1);
//	QOS_RULE_PRINTK("Hash value %u\n",value);
	return value;
}


int cmp_hash_key(struct hashtab *h,void *k1,void *k2)
{
	return memcmp(k1,k2,sizeof(struct _qos_rule_key));
}


int create_hash_key(void *rule,struct _qos_rule_key *k)
{
	int ret = 0;
	struct _qos_rule_general *gen_rule = NULL;
	struct _qos_rule_specific *spc_rule = NULL;
	int type = 0;
	u8 *pos = NULL;
	pos = (u8 *) rule;
	type = *pos;
	gen_rule = (struct _qos_rule_general *)(rule);
	spc_rule = (struct _qos_rule_specific *)(rule);
	set_zero(k);
	switch (type) {
		case AOS_QOS_RULE_GEN:
			pos = k->elem;
			*pos = type;
			++pos;
			memcpy(pos,&gen_rule->src_ip,sizeof(u32));
			pos += sizeof(u32);
			memcpy(pos,&gen_rule->dst_ip,sizeof(u32));
			pos += sizeof(u32);
			memcpy(pos,&gen_rule->src_port,sizeof(u16));
			pos += sizeof(u16);
			memcpy(pos,&gen_rule->dst_port,sizeof(u16));
			pos += sizeof(u16);
			memcpy(pos,&gen_rule->devid,sizeof(int));
			pos += sizeof(int);
			*pos = gen_rule->proto;
			++pos;
			break;
		case AOS_QOS_RULE_MAC:
			pos = k->elem;
			*pos = type;
			++pos;
			memcpy(pos,&spc_rule->parms.macdata.mac,ETH_ALEN);
			break;
		case AOS_QOS_RULE_VLAN:
			pos = k->elem;
			*pos = type;
			++pos;
			memcpy(pos,&spc_rule->parms.vlandata.tag,sizeof(u16));
			break;
		case AOS_QOS_RULE_IF:
			pos = k->elem;
			*pos = type;
			++pos;
			memcpy(pos,&spc_rule->parms.ifdata.ifid,sizeof(int));
			break;
		case AOS_QOS_RULE_PORT:
			pos = k->elem;
			*pos = type;
			++pos;
			*pos = spc_rule->parms.portdata.ifid; 
			++pos;
			memcpy(pos,&spc_rule->parms.portdata.port,sizeof(u16));
			break;
		case AOS_QOS_RULE_PROTO:
			pos = k->elem;
			*pos = type;
			++pos;
			*pos = spc_rule->parms.protodata.proto; 
			break;
		default:
			QOS_RULE_PRINTK("Invalid rule type\n");
			return -EINVAL;
	}// end swith

	return ret;
}
int rematch_single_ch(void *k, void *d, void *args)
{
	struct _qos_traffic_channel *cell;
	void *r;
	cell = (struct _qos_traffic_channel *)d;

	if (cell->matched_rule != args) return 0;
	
	r = match_in_all_rule(cell);
	if (args)
	{
		if (*((u8*)args) == AOS_QOS_RULE_GEN)
			qos_general_rule_put((struct _qos_rule_general *) args);
		else	
			qos_specific_rule_put((struct _qos_rule_specific *) args);
	}
	if (r)
	{
		if (*((u8*)r) == AOS_QOS_RULE_GEN)
			qos_general_rule_hold((struct _qos_rule_general *) r);
		else	
			qos_specific_rule_hold((struct _qos_rule_specific *) r);
		
		ros_qos_rule_log(r, cell);
	}

	cell->matched_rule = r;
	
	return 0;
}
void rematch_channel_priority(void *r)
{
	if(qos_traffic_hash_gen)
		hashtab_map(qos_traffic_hash_gen,rematch_single_ch,r);
	return;
}
int only_update_priority(void *k,void *d,void *args)
{
	struct _qos_traffic_channel *cell;
	struct _qos_rule_general *gen;
	struct _qos_rule_specific *spc;
	u8 *pos;
	cell = (struct _qos_traffic_channel *)d;
	if (cell->matched_rule == args) {
		pos = (u8 *)args;
		switch (*pos) {
			case AOS_QOS_RULE_GEN:
				gen = (struct _qos_rule_general *)args;
				cell->priority = gen->priority;
				dbg_print_count = 0;
				return 1;
			case AOS_QOS_RULE_MAC:
			case AOS_QOS_RULE_VLAN:
			case AOS_QOS_RULE_IF:
			case AOS_QOS_RULE_PORT:
			case AOS_QOS_RULE_PROTO:
				spc = (struct _qos_rule_specific *)args;
				cell->priority = spc->priority;
				dbg_print_count = 0;
				return 1;
			default:
				aos_assert1(0);
		}
	}
	return 0;

}
void hashch_only_mod_priority(void *d)
{
	hashtab_map(qos_traffic_hash_gen,only_update_priority,d);

}
int hashtab_insert_on_modch(struct hashtab *h,
							void *k,
							void *r)
{
	int ret = 0;
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_insert(h,k,r);
	if (ret == 0) {
		rematch_channel_priority(NULL);
	}
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
	//change hash file name ,function name
void destroy_rule(void *k,void *d,void *args)
{
	u8 *type;
	
	aos_assert0(args);
	type = (u8*)args;
	
	aos_debug_log(eAosMD_QoS, "Rule %p, Key %p , type %u destroying\n", k, d, *type);
	
	rematch_channel_priority(d);

	if (*type == AOS_QOS_RULE_GEN)
		qos_general_rule_put((struct _qos_rule_general*)d);
	else
		qos_specific_rule_put((struct _qos_rule_specific*)d);
	aos_free(k);
	return;
}

void show_gen_rule(struct _qos_rule_show_arg *lp,
				   char *local,
				   struct _qos_rule_general *gen_rul)
{
	sprintf(local, "GenRule %d: pri %s, proto %s, src %d.%d.%d.%d:%d, dst %d.%d.%d.%d:%d, dev_name %s, ifid %d\n", 
					++qos_rule_show_count,
					get_pri_str(gen_rul->priority),
					get_proto_str(gen_rul->proto),
					NIPQUAD(gen_rul->src_ip), 
					ntohs(gen_rul->src_port), 
					NIPQUAD(gen_rul->dst_ip), 
					ntohs(gen_rul->dst_port),
					gen_rul->dev_name,
					gen_rul->devid);
	
	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}
void show_mac_rule(struct _qos_rule_show_arg *lp,
				   char *local,
				   struct _qos_rule_specific *spc_rul)
{
	sprintf(local, "MACRule %d: pri %-6s, MAC %02X:%02X:%02X:%02X:%02X:%02X\n", 
					++qos_rule_show_count,
					get_pri_str(spc_rul->priority),
					spc_rul->parms.macdata.mac[0],
					spc_rul->parms.macdata.mac[1],
					spc_rul->parms.macdata.mac[2],
					spc_rul->parms.macdata.mac[3],
					spc_rul->parms.macdata.mac[4],
					spc_rul->parms.macdata.mac[5]);
	
	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}
void show_vlan_rule(struct _qos_rule_show_arg *lp,
					char *local,
					struct _qos_rule_specific *spc_rul)
{
	sprintf(local, "VLANRule %d: pri %-6s, VLAN tag %u\n", 
					++qos_rule_show_count,
					get_pri_str(spc_rul->priority),
					spc_rul->parms.vlandata.tag);
	
	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}

void show_if_rule(struct _qos_rule_show_arg *lp,
				  char *local,
				  struct _qos_rule_specific *spc_rul)
{
	sprintf(local, "IFRule %d: pri %s, dev_name %s, if_id %d\n", 
					++qos_rule_show_count,
					get_pri_str(spc_rul->priority),
					spc_rul->parms.ifdata.dev_name,
					spc_rul->parms.ifdata.ifid);
	
	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}

void show_port_rule(struct _qos_rule_show_arg *lp,
					char *local,
					struct _qos_rule_specific *spc_rul)
{
	struct net_device *dev;
	
	dev = dev_get_by_index(spc_rul->parms.portdata.ifid);
	if (dev) {
		sprintf(local, "PortRule %d: pri %s, app_name %s, ifid %s, port %u\n", 
			++qos_rule_show_count,
			get_pri_str(spc_rul->priority),
			spc_rul->parms.portdata.app,
			dev->name,
			ntohs(spc_rul->parms.portdata.port));
		dev_put(dev);
	}

	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}

void show_proto_rule(struct _qos_rule_show_arg *lp,
					 char *local,
					 struct _qos_rule_specific *spc_rul)
{
	sprintf(local, "ProtoRule %d: pri %s, proto %s\n", 
					++qos_rule_show_count,
					get_pri_str(spc_rul->priority),
					get_proto_str(spc_rul->parms.protodata.proto));
	
	aosCheckAndCopy(lp->rsltBuff, &(lp->rsltIndex), lp->optlen, local, strlen(local));
	
	return;
}

int show_single_rule(void *k,void *d,void *args)  //Reviewed divide function
{
	struct _qos_rule_show_arg *lp = NULL;
	struct _qos_rule_general *gen_rul = NULL;
	struct _qos_rule_specific *spc_rul = NULL;
	char local[256];
	u8 type;

	lp = (struct _qos_rule_show_arg *)args;
	type = lp->type;
	gen_rul = (struct _qos_rule_general *)(d);
	spc_rul = (struct _qos_rule_specific *)(d);
	QOS_RULE_PRINTK("Entering switch:\n");
	switch (type) {
		case AOS_QOS_RULE_GEN:
			if (gen_rul->type == type) {
				show_gen_rule(lp,local,gen_rul);
			}
			break;
		case AOS_QOS_RULE_MAC:
			if (spc_rul->type == type) {
				show_mac_rule(lp,local,spc_rul);
			}
			break;
		case AOS_QOS_RULE_VLAN:
			if (spc_rul->type == type) {
				show_vlan_rule(lp,local,spc_rul);
			}
			break;
		case AOS_QOS_RULE_IF:
			if (spc_rul->type == type) {
				show_if_rule(lp,local,spc_rul);
			}
			break;
		case AOS_QOS_RULE_PORT:
			if (spc_rul->type == type) {
				show_port_rule(lp,local,spc_rul);
			}
			break;
		case AOS_QOS_RULE_PROTO:
			if (spc_rul->type == type) {
				show_proto_rule(lp,local,spc_rul);
			}
			break;
		default:
			sprintf(local,"Invalid type\n");
			aosCheckAndCopy(lp->rsltBuff,
							&(lp->rsltIndex),
							lp->optlen,
							local,
							strlen(local));
	}//end switch
	return 0;
}
int if_clear_single_rule(void *k, void *d, void *args)
{
	u8 type;
	int ret = 0;
	struct _qos_rule_clear_arg *lp = NULL;
	struct _qos_rule_general *gen_rul = NULL;
	struct _qos_rule_specific *spc_rul = NULL;
	lp = (struct _qos_rule_clear_arg *)args;
	type = lp->type;
	gen_rul = (struct _qos_rule_general *)(d);
	spc_rul = (struct _qos_rule_specific *)(d);
	QOS_RULE_PRINTK("Checking rule to clear,type %u will be cleared\n",type);
	switch (type) {
		case AOS_QOS_RULE_GEN:
			if (gen_rul->type == type) {
				ret = 1;
			}
			break;
		case AOS_QOS_RULE_MAC:
		case AOS_QOS_RULE_VLAN:
		case AOS_QOS_RULE_IF:
		case AOS_QOS_RULE_PORT:
		case AOS_QOS_RULE_PROTO:
			if (spc_rul->type == type) {
				ret = 1;
			}
			break;
		default:
			QOS_RULE_PRINTK("Invalid type\n");
			ret = -EINVAL;
	}
	QOS_RULE_PRINTK("Clear Behavier with return value %d\n",ret);
	return ret;
}


int aos_qos_rule_init_variables(void)
{
	int ret = 0;
	struct hashtab *tmp = NULL;

	aos_eng_log(eAosMD_QoS, "%s enter", __FUNCTION__);

	tmp = hashtab_create(cal_hash_value,
						 cmp_hash_key,
						 1 << AOS_QOS_RULE_HASH_SIZE);
	if (tmp == NULL) {
		QOS_RULE_PRINTK("Allocating Memory failed for hashtable gen\n");
		return -ENOMEM;
	}
	aos_qos_rule_gen_hash = tmp;
	tmp = NULL;
	tmp = hashtab_create(cal_hash_value,
						 cmp_hash_key,
						 1 << AOS_QOS_RULE_HASH_SIZE);
	if (tmp == NULL) {
		QOS_RULE_PRINTK("Allocating Memory failed for hashtable gen\n");
		hashtab_destroy(aos_qos_rule_gen_hash);
		return -ENOMEM;
	}
	aos_qos_rule_spc_hash = tmp;
	return ret;
}
void aos_qos_rule_destroy(void)
{
	struct _qos_rule_clear_arg arg;

	arg.type = AOS_QOS_RULE_GEN; 
#ifdef __KERNEL__
	local_bh_disable();
#endif
	hashtab_map_remove_on_error(aos_qos_rule_gen_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
	arg.type = AOS_QOS_RULE_MAC; 
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
	arg.type = AOS_QOS_RULE_VLAN; 
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
	arg.type = AOS_QOS_RULE_IF; 
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
	arg.type = AOS_QOS_RULE_PORT; 
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
	arg.type = AOS_QOS_RULE_PROTO; 
	hashtab_map_remove_on_error(aos_qos_rule_spc_hash,
								if_clear_single_rule,
								destroy_rule,
								&arg);
#ifdef __KERNEL__
	local_bh_enable();
#endif
	hashtab_destroy(aos_qos_rule_gen_hash);
	hashtab_destroy(aos_qos_rule_spc_hash);
	return;
}

int qos_update_rule_weight(void *k, void *d, void *args) 
{
	struct _qos_rule_general *rule;
	struct _qos_rule_general *tmp_rule;
	int rule_order = 0;
	int tmp_rule_order = 0;
	
	rule = (struct _qos_rule_general*)args;
	tmp_rule = (struct _qos_rule_general*)d;
	
	if (rule->src_ip != tmp_rule->src_ip){ 
		if (rule->src_ip == WILDCARD_IP)
			rule_order++;
		else if (tmp_rule->src_ip == WILDCARD_IP)
			tmp_rule_order++;
	}
	
	if (rule->dst_ip != tmp_rule->dst_ip){ 
		if (rule->dst_ip == WILDCARD_IP) 
			rule_order++;
		else if (tmp_rule->dst_ip == WILDCARD_IP)
			tmp_rule_order++;	
	}
	
	if (rule->src_port != tmp_rule->src_port){
		if (rule->src_port == WILDCARD_PORT)
			rule_order++;
		else if (tmp_rule->src_port == WILDCARD_PORT)
			tmp_rule_order++;
	}
	
	if (rule->dst_port != tmp_rule->dst_port){ 
		if (rule->dst_port == WILDCARD_PORT) 
			rule_order++;
		else if (tmp_rule->dst_port == WILDCARD_PORT)
			tmp_rule_order++;
	}
	
	if (rule_order == tmp_rule_order)
		return 0;
	
	if (rule_order > tmp_rule_order) 
	 	rule->weight = tmp_rule->weight + 1;
	else 
		tmp_rule->weight = tmp_rule->weight + 1;
	
	return 0;
}

/*
 * apply weight to rule
 */
int qos_evaluate_rule(struct _qos_rule_general *rule) 
{	
	hashtab_map(aos_qos_rule_gen_hash, qos_update_rule_weight, rule);
	
	return 0;
}

int qos_rule_add_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_id,
						 u8 priority,
						 char *dev_name, 
						 u32 band_req)
{
	int ret = 0;
	struct _qos_rule_general *rule = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_general *)aos_malloc_atomic(sizeof(struct _qos_rule_general));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_GEN;
	rule->src_ip = src_ip;
	rule->dst_ip = dst_ip;
	rule->src_port = src_port;
	rule->dst_port = dst_port;
	rule->proto = proto;
	rule->devid = dev_id;
	rule->proto = proto;
	rule->priority = priority;
	rule->requester = 0;
	memset(rule->dev_name,0,IFNAMSIZ);
	strcpy(rule->dev_name,dev_name);
	rule->band_required = band_req;
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);//Review Add mutex
	
	/* initalize weight */
	rule->weight = 1;
	qos_evaluate_rule(rule);
	
	ret = hashtab_insert_on_modch(aos_qos_rule_gen_hash,key,rule);  //change hash file name ,function name
	if (ret != 0) {
		goto fail;
	}
	
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_general_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_id)
{
	int ret = 0;
	struct _qos_rule_general rule;
	struct _qos_rule_key key;  //Reviewd condidering the limited stack size
	rule.type = AOS_QOS_RULE_GEN;
	rule.src_ip = src_ip;
	rule.dst_ip = dst_ip;
	rule.src_port = src_port;
	rule.dst_port = dst_port;
	rule.proto = proto;
	rule.devid = dev_id;
	rule.proto = proto;
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_gen_hash,&key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
int qos_rule_mod_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_id,
						 u8 priority,
						 char *dev_name)
{
	int ret = 0;
	int i = 0;
	struct _qos_rule_general *rule = NULL;
	struct _qos_rule_general *data= NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_general *)aos_malloc_atomic(sizeof(struct _qos_rule_general));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_GEN;
	rule->src_ip = src_ip;
	rule->dst_ip = dst_ip;
	rule->src_port = src_port;
	rule->dst_port = dst_port;
	rule->proto = proto;
	rule->devid = dev_id;
	rule->proto = proto;
	rule->priority = priority;
	rule->requester = 0;
	for (i = 0; i < IFNAMSIZ; ++i) {
		rule->dev_name[i] = 0;
	}
	strcpy(rule->dev_name,dev_name);
	
	atomic_set(&rule->refcnt, 1);

	create_hash_key(rule,key);
	data = (struct _qos_rule_general *)hashtab_search(aos_qos_rule_gen_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_general_rule_put(rule);
		rule = NULL;
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_gen_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_general_rule_put(rule);
	}
	return ret;
}
int qos_rule_add_mac(u8 *mac,
					 int len,
					 u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_MAC;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.macdata.mac[0]=mac[0];
	rule->parms.macdata.mac[1]=mac[1];
	rule->parms.macdata.mac[2]=mac[2];
	rule->parms.macdata.mac[3]=mac[3];
	rule->parms.macdata.mac[4]=mac[4];
	rule->parms.macdata.mac[5]=mac[5];
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_mac(u8 *mac,
					 int len)
{
	int ret = 0;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_MAC;
	rule.parms.macdata.mac[0]=mac[0];
	rule.parms.macdata.mac[1]=mac[1];
	rule.parms.macdata.mac[2]=mac[2];
	rule.parms.macdata.mac[3]=mac[3];
	rule.parms.macdata.mac[4]=mac[4];
	rule.parms.macdata.mac[5]=mac[5];
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_spc_hash, &key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
int qos_rule_mod_mac(u8 *mac,
					 int len,
					 u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_specific *data = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_MAC;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.macdata.mac[0]=mac[0];
	rule->parms.macdata.mac[1]=mac[1];
	rule->parms.macdata.mac[2]=mac[2];
	rule->parms.macdata.mac[3]=mac[3];
	rule->parms.macdata.mac[4]=mac[4];
	rule->parms.macdata.mac[5]=mac[5];
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	data = (struct _qos_rule_specific *)hashtab_search(aos_qos_rule_spc_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_specific_rule_put(rule);
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_add_vlan(u16 tag,
					  u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_VLAN;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.vlandata.tag = tag;
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_vlan(u16 tag)
{
	int ret = 0;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_VLAN;
	rule.parms.vlandata.tag = tag;
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_spc_hash, &key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
int qos_rule_mod_vlan(u16 tag,
					  u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_specific *data= NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_VLAN;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.vlandata.tag = tag;
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	data = (struct _qos_rule_specific *)hashtab_search(aos_qos_rule_spc_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_specific_rule_put(rule);
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_add_interface(int dev_id,
						   u16 priority,
						   char *dev_name)
{
	int ret = 0;
	int i = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_IF;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.ifdata.ifid = dev_id;
	atomic_set(&rule->refcnt, 1);
	for (i = 0; i < IFNAMSIZ; ++i) {
		rule->parms.ifdata.dev_name[i] = 0;
	}
	strcpy(rule->parms.ifdata.dev_name,dev_name);
	create_hash_key(rule,key);
	ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_interface(int dev_id)
{
	int ret = 0;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_IF;
	rule.parms.ifdata.ifid = dev_id;
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_spc_hash, &key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
int qos_rule_mod_interface(int dev_id,
						   u16 priority,
						   char *dev_name)
{
	int ret = 0;
	int i = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_specific *data= NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_IF;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.ifdata.ifid = dev_id;
	atomic_set(&rule->refcnt, 1);
	for (i = 0; i < IFNAMSIZ; ++i) {
		rule->parms.ifdata.dev_name[i] = 0;
	}
	strcpy(rule->parms.ifdata.dev_name,dev_name);
	create_hash_key(rule,key);
	data = (struct _qos_rule_specific *)hashtab_search(aos_qos_rule_spc_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_specific_rule_put(rule);
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;

}
int qos_rule_add_port(int ifid,
					  u16 port,
					  u8 priority,
					  char *app)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_key *key = NULL;
	int i = 0;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_PORT;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.portdata.port = port;
	rule->parms.portdata.ifid = ifid;
	atomic_set(&rule->refcnt, 1);
	for (i = 0; i < AOS_QOS_APP_MAX_SIZE; ++i) {
		rule->parms.portdata.app[i] = 0;
	}
	strcpy(rule->parms.portdata.app,app);
	create_hash_key(rule,key);
	ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_port(u16 port,
					  int ifid)
{
	int ret = 0;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_PORT;
	rule.parms.portdata.port = port;
	rule.parms.portdata.ifid = ifid;
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_spc_hash, &key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}

int qos_rule_mod_port(u8 direction,
					  u16 port,
					  u8 priority,
					  char *app)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_specific *data = NULL;
	struct _qos_rule_key *key = NULL;
	int i = 0;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_PORT;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.portdata.port = port;
	//rule->parms.portdata.sending = direction;
	atomic_set(&rule->refcnt, 1);
	for (i = 0; i < AOS_QOS_APP_MAX_SIZE; ++i) {
		rule->parms.portdata.app[i] = 0;
	}
	strcpy(rule->parms.portdata.app,app);
	create_hash_key(rule,key);
	data = (struct _qos_rule_specific *)hashtab_search(aos_qos_rule_spc_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_specific_rule_put(rule);
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_add_protocol(u8 proto,
						  u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_PROTO;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.protodata.proto = proto;
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}
int qos_rule_del_protocol(u8 proto)
{
	int ret = 0;
	struct _qos_rule_specific rule;
	struct _qos_rule_key key;
	rule.type = AOS_QOS_RULE_PROTO;
	rule.parms.protodata.proto = proto;
	create_hash_key(&rule,&key);
#ifdef __KERNEL__
	local_bh_disable();
#endif
	ret = hashtab_remove(aos_qos_rule_spc_hash, &key, destroy_rule, (void*)(&rule.type));
#ifdef __KERNEL__
	local_bh_enable();
#endif
	return ret;
}
int qos_rule_mod_protocol(u8 proto,
						  u8 priority)
{
	int ret = 0;
	struct _qos_rule_specific *rule = NULL;
	struct _qos_rule_specific *data = NULL;
	struct _qos_rule_key *key = NULL;
	rule = (struct _qos_rule_specific *)aos_malloc_atomic(sizeof(struct _qos_rule_specific));
	if (rule == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for rule\n");
		ret = -ENOMEM;
		goto fail;
	}
	key = (struct _qos_rule_key *)aos_malloc_atomic(sizeof(struct _qos_rule_key));
	if (key == NULL) {
		QOS_RULE_PRINTK("Cannot allocate memory for key\n");
		ret = -ENOMEM;
		goto fail;
	}
	rule->type = AOS_QOS_RULE_PROTO;
	rule->priority = priority;
	rule->requester = 0;
	rule->parms.protodata.proto = proto;
	atomic_set(&rule->refcnt, 1);
	create_hash_key(rule,key);
	data = (struct _qos_rule_specific *)hashtab_search(aos_qos_rule_spc_hash,key);
	if (data != NULL) {
		data->priority = priority;
		hashch_only_mod_priority(data);
		aos_free(key);
		qos_specific_rule_put(rule);
	} else {
		ret = hashtab_insert_on_modch(aos_qos_rule_spc_hash,key,rule);
	}
	if (ret != 0) {
		goto fail;
	}
	return ret;
fail:
	if (key != NULL) {
		aos_free(key);
	}
	if (rule != NULL) {
		qos_specific_rule_put(rule);
	}
	return ret;
}

