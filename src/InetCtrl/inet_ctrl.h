#ifndef INET_CTRL_H
#define INET_CTRL_H

#include "aosUtil/List.h"
#include "aosUtil/Types.h"
#include "KernelSimu/atomic.h"
#include "KernelSimu/in.h"

// #include <linux/list.h>
// #include <asm/atomic.h>
// #include <asm/types.h>
// #include <linux/in.h>

/* definition section */
#define ONLINE_FILE						"/var/online_time"
#define MAX_DOMAIN_LEN					127
#define	MAX_NAME_LEN					63

/* don't change this flag */
#define	TYPE_TIME_DAY					0x01
#define TYPE_TIME_WEEK					0x02
#define TYPE_TIME_UNKNOWN				0x00

#define	TYPE_FLOW_URL					100
#define TYPE_FLOW_IP					101

#define ENTRY_PRE_DELETE				0x10
#define ENTRY_UDP_TIMEOUT				0x20

#define DATA_PRE_DELETE					0x40

/* must be different with TYPE_TIME_DAY and TYPE_TIME_WEEK */
#define ONLINE_DAY_CHANGE				0x100
#define ONLINE_WEEK_CHANGE				0x200

#define	DayMask(x)     					((u32)(1 << x))

/* common delete flag */
#define FLAG_PRE_DELETE					0x1000

#define MAX_GROUP_NUM					16

#define FLAG_ISSET(flag, mask)			(((flag) & (mask)) == (mask))
#define FLAG_SET(flag, mask)			((flag) = (flag) | (mask))
#define FLAG_UNSET(flag, mask)			((flag) = (flag) & ~(mask))

/* for printk */
#if 0 
#define INETCTRL_ERR(msg...) 			printk(KERN_ERR "INETCTRL: " msg)
#define INETCTRL_INFO(msg...) 			printk(KERN_INFO "INETCTRL: " msg)
#define INETCTRL_WARNING(msg...) 		printk(KERN_WARNING "INETCTRL: " msg)
#else
#define INETCTRL_ERR(msg...)
#define INETCTRL_INFO(msg...)
#define INETCTRL_WARNING(msg...)
#endif

#ifndef inetctrl_log 
#define inetctrl_log(access, key, mesg)	inetctrl_log_f(__FILE__, __LINE__, access, key, mesg)
#endif


#define	ENTRY_DATA_GOOD(d)		(!FLAG_ISSET(d->data_account->account_flag, FLAG_PRE_DELETE) \
	&& !FLAG_ISSET(d->data_assoc->assoc_flag, FLAG_PRE_DELETE))

#define	ENTRY_DATA_HASONLINE(d)	(!aos_list_empty(&d->data_time->rule_online_head))

#define	FLOW_TYPE_TCP					IPPROTO_TCP
#define	FLOW_TYPE_UDP					IPPROTO_UDP
#define	FLOW_TYPE_OTHER					IPPROTO_MAX

#define	BLOCK							1
#define	ALLOW							2

/* one flow of application */
typedef struct app_flow	{
	struct aos_list_head list;
	
	u8 flow_proto;
	u8 flow_type; 
    union {
		char domain[MAX_DOMAIN_LEN+1];
     	struct {
     		u32 ip;
     		u32 mask;
     	} ipaddr;
	} flow_server; /* domain/IP of destination server */
	u16 flow_startport;
	u16 flow_endport;

#define flow_domain		flow_server.domain
#define flow_ip			flow_server.ipaddr.ip
#define flow_mask		flow_server.ipaddr.mask

} app_flow_t;

typedef struct app_rule {
	struct aos_list_head list;
	
	char app_name[MAX_NAME_LEN+1];
	u32 app_flag;
	atomic_t app_refcnt;
	struct aos_list_head app_flow_head;
} app_rule_t;

enum {
	Monday = 0, /* must start at 0 */
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
	Sunday,
	
	Holiday,
	Maxday
};

typedef struct time_ctrl {
	struct aos_list_head list;
	
	u32 time_start;
	u32 time_end;
	u32 time_days; /* bit 0 for sun, bit 1 for mon, ... */
} time_ctrl_t;

typedef struct online_ctrl {
	struct aos_list_head list;
	
	u32 online_flag;
	
	u32 online_type;
	u32 online_mins;
	u32 online_days; /* bit 0 for sun, bit 1 for mon, ... */
} online_ctrl_t;

typedef struct time_rule {
	struct aos_list_head list;
	
	char rule_name[MAX_NAME_LEN+1];
	u32 rule_flag;
	atomic_t rule_refcnt;
	
	struct aos_list_head rule_time_head; 
	struct aos_list_head rule_online_head;
} time_rule_t;

typedef struct rule_assoc {
    struct aos_list_head list;
	
	u32 assoc_flag;
	atomic_t assoc_refcnt;
	struct app_rule *assoc_app;
	struct time_rule *assoc_time;
} rule_assoc_t;

enum {
	ACCESS_DENY = 10000,
	ACCESS_ALLOW,
	ACCESS_NEXT,
};

enum {
	TYPE_USER_NAME = 1000,
	TYPE_USER_IP,
	TYPE_USER_MAC,
};

typedef struct user_account {
	struct aos_list_head list;
	
	u32 account_flag;
	u32 account_type;
	union {
		char name[MAX_NAME_LEN+1];
		struct {
			u32 ip;
			u32 mask;
		} ip_type;
		u8 mac[6];	
	} account_info;
	
	atomic_t account_refcnt;
	
#define account_name 		account_info.name
#define account_ip 			account_info.ip_type.ip
#define account_mask 		account_info.ip_type.mask
#define account_mac 		account_info.mac

} user_account_t;

typedef struct user_group {
	struct aos_list_head list;
	
	char group_name[MAX_NAME_LEN+1];
	u32	group_default_access;
	u32 group_flag;
	
	struct aos_list_head group_user_head;
	struct aos_list_head group_rule_head;
} user_group_t;

typedef struct file_record { /* for writing to file */
	u32 rec_flag;
	
	u32 rec_daystart; /* 00:00:00 of today */
	u32 rec_weekstart; /* sunday of this week */
	
	char rec_account[MAX_NAME_LEN+1];
	char rec_appname[MAX_NAME_LEN+1];
	char rec_rulename[MAX_NAME_LEN+1];
	
	u32 rec_timeleft;
	u32 rec_total_timeleft;
} file_record_t;

typedef struct state_entry_data {
	struct aos_list_head list;
	
	u32 data_flag;
	atomic_t data_refcnt;
	
	file_record_t data_rec;
	u32 data_enter;
	
	user_account_t* data_account;
	rule_assoc_t* data_assoc;

#define	data_timeleft			data_rec.rec_timeleft
#define	data_total_timeleft		data_rec.rec_total_timeleft
#define	data_app				data_assoc->assoc_app
#define	data_time				data_assoc->assoc_time

} state_entry_data_t;

/* 5 tuple */
typedef struct five_tuple {
	u8 proto;
	u32 src_ip;
	u16 src_port;
	u32 dst_ip;
	u16 dst_port;
} five_tuple_t;

typedef struct state_entry {
	/* Five tuple */
	five_tuple_t st_tuple;
	
	u32 st_flag;
	u32 st_start;
	u32 st_update;
	u32 st_access;
	state_entry_data_t* st_data;
} state_entry_t;

/* return code */
enum {
	ERR_APP_USEING = -1000,
	ERR_TIME_USEING,
	ERR_APP_NOT_EXIST,
	ERR_PROTO_NOT_EXIST,
	ERR_TIME_NOT_EXIST,
	ERR_GROUP_NOT_EXIST,
	ERR_ACCOUNT_EXIST,
	ERR_ACCOUNT_NOT_EXIST,
	ERR_MEMORY_ALLOC,
	ERR_UNSUPPORT_TYPE,
	ERR_NOT_MATCHED,
	
	ERR_FAILED = -1,
	ERR_OK = 0,
};

/* global variant */

extern int inetctrl_switch;
extern int default_policy;
extern int log_level;

/* 
 * interfaces section
 */
/* Parameter 'new' means whether to create it if not exist. */
app_rule_t* get_app_rule(char* name, int nnew);
time_rule_t* get_time_rule(char* name, int nnew);
user_group_t* get_user_group(char* name, int nnew);

/* create a flow and insert it into an appliaction */
app_flow_t* get_app_ip_flow(char* name, u8 proto, u32 ip, u32 mask, u16 sp, u16 dp);
app_flow_t* get_app_url_flow(char* name, u8 proto, char *domain, u16 sp, u16 dp);
time_ctrl_t* get_time_ctrl(char* name, int start_sec, int end_sec, int days_bit);
online_ctrl_t* get_online_ctrl(char* name, u32 type, u32 mins, int days_bit);
int remove_app_rule(char* name);
int remove_app_flow_byhost(char* name, char *proto, char* host, u16 sp, u16 dp);
int remove_app_flow_bynet(char* name, char *proto, u32 ip, u32 mask, u16 sp, u16 dp);

/* while setting online control, it should be called */
void setflag_data_modified(char* name, online_ctrl_t* online);

int remove_time_rule(char* name);
int remove_timerule_onlinectrl(char* name, u32 type, u32 mins, u32 days);
int remove_timerule_timectrl(char* name, u32 start, u32 end, u32 days);

int match_group_by_name(char* name, user_group_t** match_grp, user_account_t** match_acc);
int match_group_by_ip(u32 ip, u32 mask, user_group_t** match_grp, user_account_t** match_acc);
int match_group_by_mac(u8* mac, user_group_t** match_grp, user_account_t** match_acc);

int get_account_byname(char* acc_name, char* group_name, user_account_t** account);
int remove_account_byname(char* acc_name, char* group_name);
int get_account_byip(u32 ip, u32 mask, char* group_name, user_account_t** account);
int remove_account_byip(u32 ip, u32 mask, char* group_name);
int get_account_bymac(u8* mac, char* group_name, user_account_t** account);
int remove_account_bymac(u8* mac, char* group_name);
int remove_group(char* name);

rule_assoc_t* get_time_asso(char* gname, char* aname, char* tname, int nnew);
int remove_time_asso(char* gname, char* aname, char* tname);

/* for walking through the list */
int walk_apps_list(int (*walk_func)(void* elm, void* arg), void* arg);
int walk_timerules_list(int (*walk_func)(void* elm, void* arg), void* arg);
int walk_groups_list(int (*walk_func)(void* elm, void* arg), void* arg);
int walk_statedata_list(int (*walk_func)(void* elm, void* arg), void* arg);

int walk_sub_list(struct aos_list_head* head, int (*walk_func)(void* elm, void* arg), void* arg);

/* write to online file */
int writeto_online_file(void);
int readfrom_online_file(void);

/* for packet handle */
int match_rule_ingroup(five_tuple_t* key, user_group_t* grp, rule_assoc_t** match);
state_entry_data_t* get_state_data(rule_assoc_t* rule, user_account_t* user, int nnew);
int remove_state_data(state_entry_data_t* data);

/* which = 0, means resetting day type, 1 means resetting week type, 2 means both */
void reset_online_ctrl(struct aos_list_head* h, file_record_t* fr, int which);

void clear_data_list(void);

/* initialize all variant */
int init_inetctrl_vars(void);

/* common interface */
int strcasecmp(const char *s1, const char *s2);
char* inet_ntoa(u32 ip);
u32 inet_addr(char* ip_str); 

/* time handle */
int str2secs(char *time_str);
char* secs2str(u32 sec);
int get_secoftoday(void);
int get_weekoftoday(void);
int str2days(char *day_str);
char* days2str(u32 days);
int get_startoftoday(void);
int is_newday(int old);
int get_sunoftoday(void);
int is_newweek(int old);

int clear_app_rule(void);
int clear_time_rule(void);
int clear_group_rule(void);

void inetctrl_log_f(char* file, int line, int access, five_tuple_t* key, char *mesg);

#endif

