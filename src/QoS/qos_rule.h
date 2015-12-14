#ifndef _QOS_RULE_H
#define _QOS_RULE_H

#include "KernelSimu/netdevice.h"
#include "KernelSimu/atomic.h"
#include "aosUtil/Memory.h"
#include "QoS/hashtab.h"
#include "QoS/qos_util.h"

#define AOS_QOS_RULE_PRIORITY_HIGH 2
#define AOS_QOS_RULE_PRIORITY_LOW 0
#define AOS_QOS_RULE_PRIORITY_MEDIUM 1
#define AOS_QOS_RULE_DIRECTION_SENDING 0
#define AOS_QOS_RULE_DIRECTION_RECEIVING 1
#define AOS_QOS_RULE_GEN 0
#define AOS_QOS_RULE_MAC 1
#define AOS_QOS_RULE_VLAN 2
#define AOS_QOS_RULE_IF 3
#define AOS_QOS_RULE_PORT 4
#define AOS_QOS_RULE_PROTO 5
#define AOS_QOS_RULE_SPC   6

#define AOS_QOS_RULE_HASHKEY_LENGTH 16
#define AOS_QOS_RULE_KEY_MAX_U8	10 
#define AOS_QOS_RULE_KEY_MAX_U16 2
#define AOS_QOS_RULE_KEY_MAX_U32 2
#define AOS_QOS_RULE_KEY_MAX_INT 2

#define AOS_QOS_RULE_HASH_SIZE 10

#define AOS_QOS_SHOW_BUFFER_SIZE 1024
#define AOS_QOS_APP_MAX_SIZE 12
#define AOS_QOS_KEY_LENGTH  20
#define WILDCARD_IP 0xFFFFFFFF
#define WILDCARD_PORT 0 

extern struct hashtab *aos_qos_rule_gen_hash;
extern struct hashtab *aos_qos_rule_spc_hash;
extern int qos_rule_show_count;
struct _qos_rule_show_arg {
	u8 type;
	char *rsltBuff;
	unsigned int rsltIndex;
	int optlen;
};
struct _qos_rule_clear_arg {
	u8 type;
	void *d;
};
struct _qos_rule_destroy_arg {
	void *d;
};
struct _qos_rule_key{
//	u8 valu8[AOS_QOS_RULE_KEY_MAX_U8];
//	u16 valu16[AOS_QOS_RULE_KEY_MAX_U16];
//	u32 valu32[AOS_QOS_RULE_KEY_MAX_U32];
//	int val[AOS_QOS_RULE_KEY_MAX_INT];
	u8 elem[AOS_QOS_KEY_LENGTH];
};
struct _qos_rule_mac_data {
	u8 mac[ETH_ALEN];
};
struct _qos_rule_vlan_data {
	u16 tag;
};
struct _qos_rule_if_data {
	int ifid;
	char dev_name[IFNAMSIZ];
};
struct _qos_rule_port_data {
	u16 port;
	char app[AOS_QOS_APP_MAX_SIZE];
	int ifid;
};
struct _qos_rule_proto_data {
	u8 proto;
};
struct _qos_rule_specific{
	u8 type;
	u8 priority;
	unsigned int requester;
	union {
		struct _qos_rule_mac_data macdata;
		struct _qos_rule_vlan_data vlandata;
		struct _qos_rule_if_data  ifdata;
		struct _qos_rule_port_data portdata;
		struct _qos_rule_proto_data protodata;
	}parms;
	atomic_t refcnt;
};
struct _qos_rule_general{
	u8 type;
	u8  priority;
	unsigned int requester;
	u32 src_ip;
	u32 dst_ip;
	u16 src_port;
	u16 dst_port;
	int   devid;
	u8  proto;
	unsigned int band_required;
	char dev_name[IFNAMSIZ];
	atomic_t refcnt;
	int weight;
};

static inline void qos_general_rule_release(struct _qos_rule_general *rule)
{
	aos_free(rule);
}

static inline void qos_general_rule_hold(struct _qos_rule_general *rule)
{
	atomic_inc(&rule->refcnt);	
}

static inline int qos_general_rule_put(struct _qos_rule_general *rule)
{
	if (atomic_dec_and_test(&rule->refcnt))
	{
		qos_general_rule_release(rule);
		return 0;	// the ponitor has been released
	}
	return 1;
}

static inline void qos_specific_rule_release(struct _qos_rule_specific *rule)
{
	aos_free(rule);
}

static inline void qos_specific_rule_hold(struct _qos_rule_specific *rule)
{
	atomic_inc(&rule->refcnt);	
}

static inline int qos_specific_rule_put(struct _qos_rule_specific *rule)
{
	if (atomic_dec_and_test(&rule->refcnt))
	{
		qos_specific_rule_release(rule);
		return 0;	// the ponitor has been released
	}
	return 1;
}

extern int gen_rul_size;
extern int mac_rul_size;
extern int vlan_rul_size;
extern int if_rul_size;
extern int port_rul_size;
extern int proto_rul_size;
int qos_rule_add_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_id,
						 u8 priority,
						 char *dev_name,
						 u32 band_req);

int qos_rule_del_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_index);
int qos_rule_mod_general(u32 src_ip,
						 u16 src_port,
						 u32 dst_ip,
						 u16 dst_port,
						 u8 proto,
						 int dev_id,
						 u8 priority,
						 char *dev_name);
int qos_rule_add_mac(u8 *mac,
					 int len,
					 u8 priority);
int qos_rule_del_mac(u8 *mac,
					 int len);
int qos_rule_mod_mac(u8 *mac,
					 int len,
					 u8 priority);
int qos_rule_add_vlan(u16 tag,
					  u8 priority);
int qos_rule_del_vlan(u16 tag);
int qos_rule_mod_vlan(u16 tag,
					  u8 priority);
int qos_rule_add_interface(int dev_id,
						   u16 priority,
						   char *dev_name);
int qos_rule_del_interface(int dev_id);
int qos_rule_mod_interface(int dev_id,
						   u16 priority,
						   char *dev_name);
int qos_rule_add_port(int ifid,
					  u16 port,
					  u8 priority,
					  char *app);
int qos_rule_del_port(u16 port,
					  int ifid);
int qos_rule_mod_port(u8 direction,
					  u16 port,
					  u8 priority,
					  char *app);
int qos_rule_add_protocol(u8 proto,
						  u8 priority);
int qos_rule_del_protocol(u8 proto);
int qos_rule_mod_protocol(u8 proto,
						  u8 priority);
int aos_qos_rule_init_variables(void);
int if_clear_single_rule(void *k, void *d, void *args);

void aos_qos_rule_destroy(void);
void destroy_rule(void *k,void *d,void *args);
int show_single_rule(void *k,void *d,void *args);
int create_hash_key(void *rule,struct _qos_rule_key *k);
extern int AosQos_initRuleModule(void);
void AosQos_exitRuleModule(void);
extern struct hashtab *qos_traffic_hash_gen;
extern char* get_pri_str(u8 pri);
extern int qos_evaluate_rule(struct _qos_rule_general *rule);
extern int qos_update_rule_weight(void *k, void *d, void *args);
#endif
