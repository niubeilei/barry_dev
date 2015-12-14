#include "inet_ctrl.h"
#include "inet_ctrl_hash.h"
#include "inet_ctrl_file.h"
#include "inet_ctrl_timer.h"

#include "KernelSimu/string.h"
#include "KernelSimu/timer.h"
#include "aosUtil/List.h"
#include "aosUtil/Mutex.h"
#include "aosUtil/Memory.h"


#ifndef __KERNEL__
#include "aos/aosKernelApi.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/module.h"
#endif
// #include <asm/semaphore.h>
// #include <asm/string.h>
// #include <linux/slab.h>
// #include <linux/ctype.h>
// #include <linux/time.h>

static AOS_LIST_HEAD(app_rule_head);
static AOS_LIST_HEAD(time_rule_head);
static AOS_LIST_HEAD(user_group_head);
static AOS_LIST_HEAD(state_data_head);

static AOS_DECLARE_MUTEX(app_rule_mutex);
static AOS_DECLARE_MUTEX(time_rule_mutex);
static AOS_DECLARE_MUTEX(user_group_mutex);
static AOS_DECLARE_MUTEX(state_data_mutex);

#define aos_lock(x)

static rwlock_t rw_lock = RW_LOCK_UNLOCKED;

int init_inetctrl_vars(void)
{
	AOS_INIT_MUTEX(app_rule_mutex);
	AOS_INIT_MUTEX(time_rule_mutex);
	AOS_INIT_MUTEX(user_group_mutex);
	AOS_INIT_MUTEX(state_data_mutex);
	
	return 0;
}

/* local function */
static int app_name_exist(char* name, app_rule_t** find)
{
	struct aos_list_head *p, *n;
	app_rule_t* app;
	
	if (!name || name[0] == 0)
		return 0;
		
	if (find)
		*find = NULL;
	
	aos_lock(app_rule_mutex);
	aos_list_for_each_safe(p, n, &app_rule_head) {
		app = (app_rule_t*)aos_list_entry(p, app_rule_t, list);
		if (strcasecmp(name, app->app_name) == 0) {
			if (find)
				*find = app;

			aos_unlock(app_rule_mutex);
			return 1;
		}
	}

	aos_unlock(app_rule_mutex);
	return 0;
}

static int app_ipflow_exist(app_rule_t *app, u8 proto, u32 ip, u32 mask, u16 sp, u16 dp)
{
	struct aos_list_head *p, *n;
	app_flow_t* flow;
	
	aos_list_for_each_safe(p, n, &app->app_flow_head) {
		flow = (app_flow_t*)aos_list_entry(p, app_flow_t, list);

		if ((proto == flow->flow_proto) && (ip == flow->flow_ip) && 
			(mask == flow->flow_mask) && (sp == flow->flow_startport) && 
			(dp == flow->flow_endport)) {
			return 1;
		}
	}

	return 0;
}


static int app_urlflow_exist(app_rule_t *app, u8 proto, char *domain, u16 sp, u16 dp)
{
	struct aos_list_head *p, *n;
	app_flow_t* flow;
	
	aos_list_for_each_safe(p, n, &app->app_flow_head) {
		flow = (app_flow_t*)aos_list_entry(p, app_flow_t, list);
		
		if ((proto == flow->flow_proto) && (sp == flow->flow_startport) &&
				(strcasecmp(domain, flow->flow_domain) == 0) &&
				(dp == flow->flow_endport)) {
			return 1;
		}
	}

	return 0;
}


static int time_name_exist(char* name, time_rule_t** find)
{
	struct aos_list_head *p, *n;
	time_rule_t* time;
	
	if (!name || name[0] == 0)
		return 0;
		
	if (find)
		*find = NULL;
	
	aos_lock(time_rule_mutex);
	aos_list_for_each_safe(p, n, &time_rule_head) {
		time = (time_rule_t*)aos_list_entry(p, time_rule_t, list);
		if (strcasecmp(name, time->rule_name) == 0) {
			if (find)
				*find = time;

			aos_unlock(time_rule_mutex);
			return 1;
		}
	}

	aos_unlock(time_rule_mutex);
	return 0;
}

static int time_list_exist(time_rule_t* time, u32 start_sec, u32 end_sec, u32 days_bit)
{	
	time_ctrl_t* timectrl;
	struct aos_list_head *p, *n;
	
	aos_list_for_each_safe(p, n, &time->rule_time_head) {
		timectrl = (time_ctrl_t*)aos_list_entry(p, time_ctrl_t, list);
		if ((timectrl->time_start == start_sec) && (timectrl->time_end == end_sec) &&
			(timectrl->time_days == days_bit)) {
			return 1;
		}
	}

	return 0;
}

static int online_list_exist(time_rule_t* time, u32 type, u32 mins, u32 days_bit)
{
	online_ctrl_t* onlinectrl;
	struct aos_list_head *p, *n;

	aos_list_for_each_safe(p, n, &time->rule_online_head) {
		onlinectrl = (online_ctrl_t*)aos_list_entry(p, online_ctrl_t, list);
		if ((onlinectrl->online_type == type) && (onlinectrl->online_mins == mins) &&
			(onlinectrl->online_days == days_bit)) {
			return 1;
		}
	}

	return 0;
}

static int group_name_exist(char* name, user_group_t** find)
{
	struct aos_list_head *p, *n;
	user_group_t* grp;
	
	if (!name || name[0] == 0)
		return 0;
		
	if (find)
		*find = NULL;
	
	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		grp = (user_group_t*)aos_list_entry(p, user_group_t, list);
		if (strcasecmp(name, grp->group_name) == 0) {
			if (find)
				*find = grp;

			aos_unlock(user_group_mutex);
			return 1;
		}
	}

	aos_unlock(user_group_mutex);
	return 0;
}

#ifdef __KERNEL__
/* according to data_rec initialize pointer */
static int init_data_pointer(state_entry_data_t* data)
{
	file_record_t* fr;
	rule_assoc_t* rule;
	user_account_t* user;
	user_group_t* grp;
	u8 bmac[6];
	char *p;
	
	if (!data)
		return -1;
	
	fr = &data->data_rec;
	
	if (strchr(fr->rec_account, '-')) { /* meaning it's mac */
		sscanf(fr->rec_account, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bmac[0], &bmac[1],
		   &bmac[2], &bmac[3], &bmac[4], &bmac[5]);
		if (match_group_by_mac(bmac, &grp, &user) == 0)
			return -1;
	} else if((p = strchr(fr->rec_account, '/')) != NULL) { /* meaning ip */
		INETCTRL_INFO("IP type: %s\n", fr->rec_account);

		*p = 0;
		if (match_group_by_ip(inet_addr(fr->rec_account), inet_addr(p+1), 
			&grp, &user) == 0) {
			*p = '/';
			return -1;
		}
		*p = '/';
	} else { /* meaning name */
		if (match_group_by_name(fr->rec_account, &grp, &user) == 0)
			return -1;
	}
	
	rule = get_time_asso(grp->group_name, fr->rec_appname, fr->rec_rulename, 0);
	if (!rule)
		return -1;
	
	data->data_account = user;
	atomic_inc(&user->account_refcnt);
	data->data_assoc = rule;
	atomic_inc(&rule->assoc_refcnt);
	return 0;
}
#endif

/* 
 * this walking will not quit in the middle, 
 * because not only day control but week control may exist 
 */
static int walk_online(void* p, void* arg)
{
	online_ctrl_t* onlinectrl = (online_ctrl_t*)aos_list_entry((struct aos_list_head*)p,
		online_ctrl_t, list);
	file_record_t* fr = (file_record_t*)arg;
	int date;
	
	if (!fr || !onlinectrl)
		return -1; /* error quit */
	
	if (onlinectrl->online_type == TYPE_TIME_DAY) {
		date = get_weekoftoday();
		if (FLAG_ISSET(onlinectrl->online_days, DayMask(date))) {
			FLAG_SET(fr->rec_flag, TYPE_TIME_DAY);
			fr->rec_timeleft = onlinectrl->online_mins*60;

			INETCTRL_INFO("Got a time left: %u\n", fr->rec_timeleft);
			return 0; /* matched, but continue */
		}
	} else if (onlinectrl->online_type == TYPE_TIME_WEEK) {
		FLAG_SET(fr->rec_flag, TYPE_TIME_WEEK);
		fr->rec_total_timeleft = onlinectrl->online_mins*60;

		INETCTRL_INFO("Got a total time left: %u\n", fr->rec_total_timeleft);
		return 0; /* matched, but continue */
	} else
		return -1; /* error quit */

	return 0; /* continue */
}

/* according to the pointer of rules initialize the data record field */
static int init_data_record(state_entry_data_t* data)
{
	file_record_t* fr;
	char uname[MAX_NAME_LEN+1], tmp1[MAX_NAME_LEN+1], tmp2[MAX_NAME_LEN+1];
	
	if (!data->data_account || !data->data_assoc)
		return -1;
	
	fr = &data->data_rec;
	
	if (data->data_account->account_type == TYPE_USER_NAME)
		strcpy(uname, data->data_account->account_name);
	else if (data->data_account->account_type == TYPE_USER_IP) {
		strcpy(tmp1, inet_ntoa(data->data_account->account_ip));
		strcpy(tmp2, inet_ntoa(data->data_account->account_mask));
		sprintf(uname, "%s/%s", tmp1, tmp2);
	} else if (data->data_account->account_type == TYPE_USER_MAC)
#ifndef __KERNEL__
		sprintf(uname, "%hhu:%hhu:%hhu:%hhu:%hhu:%hhu", data->data_account->account_mac[0],
				data->data_account->account_mac[1],data->data_account->account_mac[2],
				data->data_account->account_mac[3],data->data_account->account_mac[4],
				data->data_account->account_mac[5]);
#else
		sprintf(uname, "%u:%u:%u:%u:%u:%u", data->data_account->account_mac[0],
				data->data_account->account_mac[1],data->data_account->account_mac[2],
				data->data_account->account_mac[3],data->data_account->account_mac[4],
				data->data_account->account_mac[5]);
#endif
	else
		return -1;

	strncpy(fr->rec_account, uname, MAX_NAME_LEN);
	fr->rec_account[MAX_NAME_LEN] = 0;
	strncpy(fr->rec_appname, data->data_app->app_name, MAX_NAME_LEN);
	fr->rec_appname[MAX_NAME_LEN] = 0;
	strncpy(fr->rec_rulename, data->data_time->rule_name, MAX_NAME_LEN);
	fr->rec_rulename[MAX_NAME_LEN] = 0;
	
	/* must before walking */
	fr->rec_timeleft = UINT_MAX;
	fr->rec_total_timeleft = UINT_MAX;
	walk_sub_list(&data->data_time->rule_online_head, walk_online, fr);
	
	INETCTRL_INFO("From rule to get the configuration: timeleft %u, totaltimeleft %u\n", 
				  fr->rec_timeleft, fr->rec_total_timeleft);

	fr->rec_daystart = get_startoftoday();
	fr->rec_weekstart = get_sunoftoday();
	
	return 0;
}

/* 
 * exported function section 
 */
app_rule_t* get_app_rule(char* name, int nnew)
{
	app_rule_t* app;
	
	if (strlen(name) > MAX_NAME_LEN)
		return NULL;

	if (name) {
		if (app_name_exist(name, &app) == 1) 
			return app;
	}

	if (nnew == 0)
		return NULL;

	app = (app_rule_t*)aos_malloc(sizeof(*app));
	if (app == NULL)
		return NULL;

	memset(app, 0, sizeof(*app));
	AOS_INIT_LIST_HEAD(&app->app_flow_head);
	atomic_set(&app->app_refcnt, 1);
	
	if (name) {
		strncpy(app->app_name, name, MAX_NAME_LEN);
		app->app_name[MAX_NAME_LEN] = 0;
	}
	
	aos_lock(app_rule_mutex);
	aos_list_add_tail(&app->list, &app_rule_head);
	aos_unlock(app_rule_mutex);
	
	return app;
}

int remove_app_rule(char* name)
{
	app_rule_t* app;
	struct aos_list_head *p, *n;
	
	if (name) {
		if (app_name_exist(name, &app) == 0)
			return ERR_APP_NOT_EXIST;
		
		if (atomic_read(&app->app_refcnt) > 1)
			return ERR_APP_USEING;
		
		/* remove it */		
		aos_lock(app_rule_mutex);
		aos_list_del(&app->list);
		aos_unlock(app_rule_mutex);
		
		aos_list_for_each_safe(p, n, &app->app_flow_head) {
			app_flow_t* flow = (app_flow_t*)aos_list_entry(p, app_flow_t, list);
			aos_list_del(&flow->list);
			aos_free(flow);
		}
		
		aos_free(app);
		return ERR_OK;
	}
	
	return ERR_FAILED;
}

//add by xyb
int clear_app_rule(void)
{
	struct aos_list_head *p, *n, *p1, *n1;
	app_rule_t* app;
	app_flow_t* flow;

	aos_lock(app_rule_mutex);
	aos_list_for_each_safe(p, n, &app_rule_head) {
		app = (app_rule_t*)aos_list_entry(p, app_rule_t, list);
		if (atomic_read(&app->app_refcnt) > 1) {
			aos_unlock(app_rule_mutex);
			return ERR_APP_USEING;
		}
	}

	aos_list_for_each_safe(p, n, &app_rule_head) {
		app = (app_rule_t*)aos_list_entry(p, app_rule_t, list);
		if (atomic_read(&app->app_refcnt) > 1)
			continue;

		aos_list_for_each_safe(p1, n1, &app->app_flow_head) {
			flow = (app_flow_t*)aos_list_entry(p1, app_flow_t, list);
			aos_list_del(&flow->list);
			aos_free(flow);
		}

		aos_list_del(&app->list);
		aos_free(app);
	}
	aos_unlock(app_rule_mutex);

	return ERR_OK;
}

int clear_time_rule(void)
{
	struct aos_list_head *p, *n, *p1, *n1;
	time_rule_t *time;
	time_ctrl_t *timectrl;
	online_ctrl_t *onlinectrl;
	
	aos_lock(time_rule_mutex);
	aos_list_for_each_safe(p, n, &time_rule_head) {
		time = (time_rule_t*)aos_list_entry(p, time_rule_t, list);
		if (atomic_read(&time->rule_refcnt) > 1) {
			aos_unlock(time_rule_mutex);
			return ERR_TIME_USEING;
		}
	}

	aos_list_for_each_safe(p, n, &time_rule_head) {
		time = (time_rule_t*)aos_list_entry(p, time_rule_t, list);
		if (atomic_read(&time->rule_refcnt) > 1)
			continue;

		aos_list_for_each_safe(p1, n1, &time->rule_time_head) {
			timectrl = (time_ctrl_t*)aos_list_entry(p1, time_ctrl_t, list);
			aos_list_del(&timectrl->list);
			aos_free(timectrl);
		}
	
		aos_list_for_each_safe(p1, n1, &time->rule_online_head) {
			onlinectrl = (online_ctrl_t*)aos_list_entry(p1, online_ctrl_t, list);
			aos_list_del(&onlinectrl->list);
			aos_free(onlinectrl);
		}	

		aos_list_del(&time->list);
		aos_free(time);
	}
	aos_unlock(time_rule_mutex);

	return ERR_OK;
}


int clear_group_rule(void)
{
	struct aos_list_head *p, *n, *p1, *n1;
	user_group_t *group;
	rule_assoc_t *rule;
	user_account_t *account;

	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		group = (user_group_t*)aos_list_entry(p, user_group_t, list);
		
		aos_list_for_each_safe(p1, n1, &group->group_rule_head) {
			rule = (rule_assoc_t*)aos_list_entry(p1, rule_assoc_t, list);
			aos_list_del(&rule->list);
			atomic_dec(&(rule->assoc_app->app_refcnt));
			atomic_dec(&(rule->assoc_time->rule_refcnt));
			FLAG_SET(rule->assoc_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&rule->assoc_refcnt))
				aos_free(rule);
		}
	
		aos_list_for_each_safe(p1, n1, &group->group_user_head) {
			account = (user_account_t*)aos_list_entry(p1, user_account_t, list);
			aos_list_del(&account->list);
			FLAG_SET(account->account_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&account->account_refcnt))
				aos_free(account);
		}

		aos_list_del(&group->list);
		aos_free(group);
	}
	aos_unlock(user_group_mutex);

	return ERR_OK;
}

time_rule_t* get_time_rule(char* name, int nnew)
{
	time_rule_t* time;
	
	if (strlen(name) > MAX_NAME_LEN)
		return NULL;

	if (name) {
		if (time_name_exist(name, &time) == 1) 
			return time;
	}
	
	if (nnew == 0)
		return NULL;

	time = (time_rule_t*)aos_malloc(sizeof(*time));
	if (time == NULL)
		return NULL;

	memset(time, 0, sizeof(*time));
	AOS_INIT_LIST_HEAD(&time->rule_time_head);
	AOS_INIT_LIST_HEAD(&time->rule_online_head);
	atomic_set(&time->rule_refcnt, 1);
	
	if (name) {
		strncpy(time->rule_name, name, MAX_NAME_LEN);
		time->rule_name[MAX_NAME_LEN] = 0;
	}

	aos_lock(time_rule_mutex);
	aos_list_add_tail(&time->list, &time_rule_head);
	aos_unlock(time_rule_mutex);

	return time;
}

int remove_time_rule(char* name)
{
	time_rule_t* time;
	struct aos_list_head *p, *n;
	
	if (name) {
		if (time_name_exist(name, &time) == 0)
			return ERR_TIME_NOT_EXIST;
		
		if (atomic_read(&time->rule_refcnt) > 1) 
			return ERR_TIME_USEING;
		
		/* remove it */		
		aos_lock(time_rule_mutex);
		aos_list_del(&time->list);
		aos_unlock(time_rule_mutex);
		
		aos_list_for_each_safe(p, n, &time->rule_time_head) {
			time_ctrl_t* timectrl = (time_ctrl_t*)aos_list_entry(p, time_ctrl_t, list);
			aos_free(timectrl);
		}
		
		aos_list_for_each_safe(p, n, &time->rule_online_head) {
			online_ctrl_t* onlinectrl = (online_ctrl_t*)aos_list_entry(p, online_ctrl_t, list);
			aos_free(onlinectrl);
		}
		
		aos_free(time);
		return ERR_OK;
	}
	
	return ERR_FAILED;
}

int count_group_num(void)
{
	int num = 0;
	struct aos_list_head *p, *n;
	
	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		num++;
	}
	aos_unlock(user_group_mutex);
	
	return num;
}

user_group_t* get_user_group(char* name, int nnew)
{
	user_group_t* grp;
	
	if (strlen(name) > MAX_NAME_LEN)
		return NULL;

	if (name) {
		if (group_name_exist(name, &grp) == 1) 
			return grp;
	}
	
	if (nnew == 0)
		return NULL;

	grp = (user_group_t*)aos_malloc(sizeof(*grp));
	if (grp == NULL)
		return NULL;

	memset(grp, 0, sizeof(*grp));
	AOS_INIT_LIST_HEAD(&grp->group_user_head);
	AOS_INIT_LIST_HEAD(&grp->group_rule_head);
	
	if (name) {
		strncpy(grp->group_name, name, MAX_NAME_LEN);
		grp->group_name[MAX_NAME_LEN] = 0;
	}
	
	aos_lock(user_group_mutex);
	aos_list_add_tail(&grp->list, &user_group_head);
	aos_unlock(user_group_mutex);
	
	return grp;
}

int remove_group(char* name)
{
	user_group_t* grp;
	struct aos_list_head *p, *n;
	
	grp = get_user_group(name, 0);
	if (grp == NULL)
		return ERR_GROUP_NOT_EXIST;

	aos_lock(user_group_mutex);
	aos_list_del(&grp->list);
	aos_unlock(user_group_mutex);
	
	/* release the reference */
	aos_list_for_each_safe(p, n, &grp->group_rule_head) {
		rule_assoc_t* rule = (rule_assoc_t*)aos_list_entry(p, rule_assoc_t, list);
		aos_list_del(&rule->list);
		atomic_dec(&(rule->assoc_app->app_refcnt));
		atomic_dec(&(rule->assoc_time->rule_refcnt));
		FLAG_SET(rule->assoc_flag, FLAG_PRE_DELETE);
		if (atomic_dec_and_test(&rule->assoc_refcnt))
			aos_free(rule);
	}
	
	/* free the user */
	aos_list_for_each_safe(p, n, &grp->group_user_head) {
		user_account_t* account = (user_account_t*)aos_list_entry(p, user_account_t, list);
		FLAG_SET(account->account_flag, FLAG_PRE_DELETE);
		aos_list_del(&account->list);
		if (atomic_dec_and_test(&account->account_refcnt))
			aos_free(account);
	}
	
	aos_free(grp);
	return ERR_OK;
}

app_flow_t* get_app_ip_flow(char* name, u8 protocol, u32 ip, u32 mask, u16 sp, u16 dp)
{
	app_rule_t* app;
	app_flow_t* flow;
	
	if (app_name_exist(name, &app) == 0) 
		return NULL;
	    
	if (app_ipflow_exist(app, protocol, ip, mask, sp, dp) > 0)
		return NULL;

	flow = (app_flow_t*)aos_malloc(sizeof(*flow));
	if (flow == NULL)
		return NULL;
	
	memset(flow, 0, sizeof(*flow));
	flow->flow_type = TYPE_FLOW_URL;
	aos_list_add_tail(&flow->list, &app->app_flow_head);
	
	return flow;
}

app_flow_t* get_app_url_flow(char* name, u8 protocol, char *domain, u16 sp, u16 dp)
{
	app_rule_t* app;
	app_flow_t* flow;

	if (app_name_exist(name, &app) == 0) 
		return NULL;

	if (app_urlflow_exist(app, protocol, domain, sp, dp) > 0)
		return NULL;

	flow = (app_flow_t*)aos_malloc(sizeof(*flow));
	if (flow == NULL)
		return NULL;
	
	memset(flow, 0, sizeof(*flow));
	flow->flow_type = TYPE_FLOW_URL;
	aos_list_add_tail(&flow->list, &app->app_flow_head);
	
	return flow;
}

int remove_app_flow_byhost(char* name, char *protocol, char* host, u16 sp, u16 dp)
{
	app_flow_t* flow;
	app_rule_t* app;
	u8 proto;
	struct aos_list_head *p, *n;

	if (strcasecmp(protocol, "tcp") == 0)
		proto = FLOW_TYPE_TCP;
	else if (strcasecmp(protocol, "udp") == 0)
		proto = FLOW_TYPE_UDP;
	else
		return ERR_PROTO_NOT_EXIST;

	if (app_name_exist(name, &app) == 0) 
		return ERR_APP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &app->app_flow_head) {
		flow = (app_flow_t*)aos_list_entry(p, app_flow_t, list);
		if (flow->flow_type != TYPE_FLOW_URL)
			continue;

		if ((sp == flow->flow_startport) && (dp == flow->flow_endport) &&
			(proto == flow->flow_proto) && (strcasecmp(flow->flow_domain, host) == 0)) {
			aos_list_del(&flow->list);
			aos_free(flow);
			return ERR_OK;
		}
	}
	
	return ERR_FAILED;
}

int remove_app_flow_bynet(char* name, char *protocol, u32 ip, u32 mask, u16 sp, u16 dp)
{
	app_flow_t* flow;
	app_rule_t* app;
	u8 proto;
	struct aos_list_head *p, *n;
	
	if (strcasecmp(protocol, "tcp") == 0)
		proto = FLOW_TYPE_TCP;
	else if (strcasecmp(protocol, "udp") == 0)
		proto = FLOW_TYPE_UDP;
	else
		return ERR_PROTO_NOT_EXIST;

	if (app_name_exist(name, &app) == 0) 
		return ERR_APP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &app->app_flow_head) {
		flow = (app_flow_t*)aos_list_entry(p, app_flow_t, list);
		if (flow->flow_type != TYPE_FLOW_IP)
			continue;

		if ((sp == flow->flow_startport) && (dp == flow->flow_endport) &&
			(ip == flow->flow_ip) && (mask == flow->flow_mask) &&
			(proto == flow->flow_proto)) {
			aos_list_del(&flow->list);
			aos_free(flow);
			return ERR_OK;
		}
	}
	
	return ERR_FAILED;
}
 
time_ctrl_t* get_time_ctrl(char* name, int start_sec, int end_sec, int days_bit)
{
	time_rule_t* time;
	time_ctrl_t* time_ctrl;
	
	if (time_name_exist(name, &time) == 0)
		return NULL;

	if (time_list_exist(time, start_sec, end_sec, days_bit) > 0)
		return NULL;
	
	time_ctrl = (time_ctrl_t*)aos_malloc(sizeof(*time_ctrl));
	if (time_ctrl == NULL)
		return NULL;
	
	memset(time_ctrl, 0, sizeof(*time_ctrl));
	aos_list_add_tail(&time_ctrl->list, &time->rule_time_head);
	
	return time_ctrl;
}

int remove_timerule_timectrl(char* name, u32 start, u32 end, u32 days)
{
	time_rule_t* timerule;
	time_ctrl_t* timectrl;
	struct aos_list_head *p, *n;
	
	if (time_name_exist(name, &timerule) == 0) 
		return ERR_TIME_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &timerule->rule_time_head) {
		timectrl = (time_ctrl_t*)aos_list_entry(p, time_ctrl_t, list);
		if ((timectrl->time_start == start) && (timectrl->time_end == end) &&
			(timectrl->time_days == days)) {
			aos_list_del(&timectrl->list);
			aos_free(timectrl);
			return ERR_OK;
		}
	}
	
	return ERR_FAILED;
}

/* set flag to every data entry with this time rule */
void setflag_data_modified(char* name, online_ctrl_t* online)
{
	struct aos_list_head *p, *n;
	state_entry_data_t* data;
	time_rule_t* time;

	if (time_name_exist(name, &time) == 0)
		return;

	aos_lock(state_data_mutex);
	aos_list_for_each_safe(p, n, &state_data_head) {
		data = (state_entry_data_t*)aos_list_entry(p, state_entry_data_t, list);
		if (data->data_time == time) {
			if ((online->online_type == TYPE_TIME_DAY) && 
				FLAG_ISSET(online->online_days, DayMask(get_weekoftoday())))
				FLAG_SET(data->data_flag, ONLINE_DAY_CHANGE);
			else if (online->online_type == TYPE_TIME_WEEK) 
				FLAG_SET(data->data_flag, ONLINE_WEEK_CHANGE);
		}
	}
	aos_unlock(state_data_mutex);

	/* at once execute the slow timeout */
	execute_slow_timeout();
}

online_ctrl_t* get_online_ctrl(char* name, u32 type, u32 mins, int days_bit)
{
	time_rule_t* time;
	online_ctrl_t* online_ctrl;
	
	if (time_name_exist(name, &time) == 0)
		return NULL;

	if (online_list_exist(time, type, mins, days_bit) > 0)
		return NULL;
	
	online_ctrl = (online_ctrl_t*)aos_malloc(sizeof(*online_ctrl));
	if (online_ctrl == NULL)
		return NULL;

	memset(online_ctrl, 0, sizeof(*online_ctrl));
	online_ctrl->online_type = TYPE_TIME_WEEK;
	aos_list_add_tail(&online_ctrl->list, &time->rule_online_head);
	
	return online_ctrl;
}

int remove_timerule_onlinectrl(char* name, u32 type, u32 mins, u32 days)
{
	time_rule_t* timerule;
	online_ctrl_t* onlinectrl;
	struct aos_list_head *p, *n;
	
	if (time_name_exist(name, &timerule) == 0) 
		return ERR_TIME_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &timerule->rule_online_head) {
		onlinectrl = (online_ctrl_t*)aos_list_entry(p, online_ctrl_t, list);
		if ((onlinectrl->online_type == type) && (onlinectrl->online_mins == mins) &&
			(onlinectrl->online_days == days)) {
			aos_list_del(&onlinectrl->list);
			/* must be after removed from list */
			setflag_data_modified(name, onlinectrl);

			aos_free(onlinectrl);

			return ERR_OK;
		}
	}
	
	return ERR_FAILED;
}

int match_group_by_name(char* name, user_group_t** match_grp, user_account_t** match_acc)
{
	struct aos_list_head *p, *n, *p1, *n1;
	user_group_t* grp;
	user_account_t* account;
	
	if (!name || name[0] == 0)
		return 0;
		
	if (match_grp)
		*match_grp = NULL;
	if (match_acc)
		*match_acc = NULL;
	
	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		grp = (user_group_t*)aos_list_entry(p, user_group_t, list);
		aos_list_for_each_safe(p1, n1, &grp->group_user_head) {
			account = (user_account_t*)aos_list_entry(p, user_account_t, list);
			if (account->account_type != TYPE_USER_NAME)
				continue;
				
			if (strcasecmp(name, account->account_name) == 0) {
				if (match_grp)
					*match_grp = grp;
				if (match_acc)
					*match_acc = account;
					
				aos_unlock(user_group_mutex);
				return 1;
			}
		}
	}

	aos_unlock(user_group_mutex);
	return 0;
}

int match_group_by_ip(u32 ip, u32 mask, user_group_t** match_grp, user_account_t** match_acc)
{
	struct aos_list_head *p, *n, *p1, *n1;
	user_group_t* grp;
	user_account_t* account;
		
	if (match_grp)
		*match_grp = NULL;
	if (match_acc)
		*match_acc = NULL;

	INETCTRL_INFO("packet source ip = %s\n", inet_ntoa(ip));

	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		grp = (user_group_t*)aos_list_entry(p, user_group_t, list);
		aos_list_for_each_safe(p1, n1, &grp->group_user_head) {
			account = (user_account_t*)aos_list_entry(p1, user_account_t, list);
			if (account->account_type != TYPE_USER_IP)
				continue;

			INETCTRL_INFO("config subnet: %s\n", inet_ntoa(account->account_ip));
			INETCTRL_INFO("config mask: %s\n", inet_ntoa(account->account_mask));

			if (mask == 0xffffffff) {
				if ((ip & account->account_mask) == (account->account_ip & 
					account->account_mask)) {
					if (match_grp)
						*match_grp = grp;
					if (match_acc)
						*match_acc = account;
						
					aos_unlock(user_group_mutex);
					return 1;
				}
			} else {
				if ((ip == account->account_ip) && 
					(mask == account->account_mask)) {
					if (match_grp)
						*match_grp = grp;
					if (match_acc)
						*match_acc = account;
	
					aos_unlock(user_group_mutex);
					return 1;
				}
			}
		}
	}

	aos_unlock(user_group_mutex);
	return 0;
}

int match_group_by_mac(u8* mac, user_group_t** match_grp, user_account_t** match_acc)
{
	struct aos_list_head *p, *n, *p1, *n1;
	user_group_t* grp;
	user_account_t* account;
		
	if (match_grp)
		*match_grp = NULL;
	if (match_acc)
		*match_acc = NULL;
	
	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		grp = (user_group_t*)aos_list_entry(p, user_group_t, list);
		aos_list_for_each_safe(p1, n1, &grp->group_user_head) {
			account = (user_account_t*)aos_list_entry(p1, user_account_t, list);
			if (account->account_type != TYPE_USER_MAC)
				continue;
			
			if (memcmp(mac, account->account_mac, 6) == 0) {
				if (match_grp)
					*match_grp = grp;
				if (match_acc)
					*match_acc = account;
					
				aos_unlock(user_group_mutex);
				return 1;
			}
		}
	}

	aos_unlock(user_group_mutex);
	return 0;
}

int get_account_byname(char* acc_name, char* group_name, user_account_t** account)
{	
	user_group_t* group;
	
	if (match_group_by_name(acc_name, NULL, NULL) == 1)
		return ERR_ACCOUNT_EXIST;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;

	if (account == NULL)
		return ERR_MEMORY_ALLOC;
	
	*account = (user_account_t*)aos_malloc(sizeof(**account));
	if (*account == NULL)
		return ERR_MEMORY_ALLOC;
	
	memset(*account, 0, sizeof(**account));
	atomic_set(&(*account)->account_refcnt, 1);
	(*account)->account_type = TYPE_USER_NAME;
	strncpy((*account)->account_name, acc_name, MAX_NAME_LEN);
	((*account)->account_name)[MAX_NAME_LEN] = 0;
	
	aos_list_add_tail(&((*account)->list), &group->group_user_head);
	return ERR_OK;
}

int remove_account_byname(char* acc_name, char* group_name)
{
	user_group_t* group;
	user_account_t* account;
	struct aos_list_head *p, *n;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &group->group_user_head) {
		account = (user_account_t*)aos_list_entry(p, user_account_t, list);
		if (account->account_type != TYPE_USER_NAME)
			continue;
		
		if (strcasecmp(acc_name, account->account_name) == 0) {
			aos_list_del(&account->list);
			FLAG_SET(account->account_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&account->account_refcnt))
				aos_free(account);

			return ERR_OK;
		}
	}
	
	return ERR_ACCOUNT_NOT_EXIST;
}

int get_account_byip(u32 ip, u32 mask, char* group_name, user_account_t** account)
{
	user_group_t* group;
	
	if (match_group_by_ip(ip, mask, NULL, NULL) == 1)
		return ERR_ACCOUNT_EXIST;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;

	if (account == NULL)
		return ERR_MEMORY_ALLOC;
	
	*account = (user_account_t*)aos_malloc(sizeof(**account));
	if (*account == NULL)
		return ERR_MEMORY_ALLOC;
	
	memset(*account, 0, sizeof(**account));
	atomic_set(&(*account)->account_refcnt, 1);
	(*account)->account_type = TYPE_USER_IP;
	(*account)->account_ip = ip;
	(*account)->account_mask = mask;
	
	aos_list_add_tail(&((*account)->list), &group->group_user_head);
	return ERR_OK;
}

int remove_account_byip(u32 ip, u32 mask, char* group_name)
{
	user_group_t* group;
	user_account_t* account;
	struct aos_list_head *p, *n;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &group->group_user_head) {
		account = (user_account_t*)aos_list_entry(p, user_account_t, list);
		if (account->account_type != TYPE_USER_IP)
			continue;
		
		if (ip == account->account_ip && mask == account->account_mask) {
			aos_list_del(&account->list);
			FLAG_SET(account->account_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&account->account_refcnt))
				aos_free(account);

			return ERR_OK;
		}
	}
	
	return ERR_ACCOUNT_NOT_EXIST;
}

int get_account_bymac(u8* mac, char* group_name, user_account_t** account)
{	
	user_group_t* group;
	
	if (match_group_by_mac(mac, NULL, NULL) == 1)
		return ERR_ACCOUNT_EXIST;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;
		
	if (account == NULL)
		return ERR_MEMORY_ALLOC;
	
	*account = (user_account_t*)aos_malloc(sizeof(**account));
	if (*account == NULL)
		return ERR_MEMORY_ALLOC;
	
	memset(*account, 0, sizeof(**account));
	atomic_set(&(*account)->account_refcnt, 1);
	(*account)->account_type = TYPE_USER_MAC;
	memcpy((*account)->account_mac, mac, 6);
	
	aos_list_add_tail(&((*account)->list), &group->group_user_head);
	return ERR_OK;
}

int remove_account_bymac(u8* mac, char* group_name)
{	
	user_group_t* group;
	user_account_t* account;
	struct aos_list_head *p, *n;
	
	if (group_name_exist(group_name, &group) == 0)
		return ERR_GROUP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &group->group_user_head) {
		account = (user_account_t*)aos_list_entry(p, user_account_t, list);
		if (account->account_type != TYPE_USER_MAC)
			continue;
		
		if (memcmp(mac, account->account_mac, 6) == 0) {
			aos_list_del(&account->list);
			FLAG_SET(account->account_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&account->account_refcnt))
				aos_free(account);

			return ERR_OK;
		}
	}
	
	return ERR_ACCOUNT_NOT_EXIST;
}

rule_assoc_t* get_time_asso(char* gname, char* aname, char* tname, int nnew)
{
	user_group_t* grp;
	app_rule_t* app;
	time_rule_t* time;
	rule_assoc_t* assoc;
	struct aos_list_head *p, *n;
	
	if (group_name_exist(gname, &grp) == 0)
		return NULL;

	aos_list_for_each_safe(p, n, &grp->group_rule_head) {
		assoc = (rule_assoc_t*)aos_list_entry(p, rule_assoc_t, list);
		if (strcasecmp(assoc->assoc_app->app_name, aname) == 0 &&
			strcasecmp(assoc->assoc_time->rule_name, tname) == 0)
			return assoc;
	}
	
	if (!nnew)
		return NULL;
	
	if (app_name_exist(aname, &app) == 0)
		return NULL;
	
	if (time_name_exist(tname, &time) == 0)
		return NULL;

	assoc = (rule_assoc_t*)aos_malloc(sizeof(*assoc));
	if (assoc == NULL)
		return NULL;
	
	memset(assoc, 0, sizeof(*assoc));
	atomic_set(&assoc->assoc_refcnt, 1);
	assoc->assoc_app = app;
	atomic_inc(&app->app_refcnt);
	assoc->assoc_time = time;
	atomic_inc(&time->rule_refcnt);
	
	aos_list_add_tail(&assoc->list, &grp->group_rule_head);
	return assoc;
}

/* only used to remove time association from configuration */
int remove_time_asso(char* gname, char* aname, char* tname)
{
	user_group_t* grp;
	rule_assoc_t* assoc;
	struct aos_list_head *p, *n;
	
	if (group_name_exist(gname, &grp) == 0)
		return ERR_GROUP_NOT_EXIST;
	
	aos_list_for_each_safe(p, n, &grp->group_rule_head) {
		assoc = (rule_assoc_t*)aos_list_entry(p, rule_assoc_t, list);
		if (strcasecmp(assoc->assoc_app->app_name, aname) == 0 &&
			strcasecmp(assoc->assoc_time->rule_name, tname) == 0) {
			aos_list_del(&assoc->list);
	
			atomic_dec(&assoc->assoc_app->app_refcnt);
			atomic_dec(&assoc->assoc_time->rule_refcnt);
			FLAG_SET(assoc->assoc_flag, FLAG_PRE_DELETE);
			if (atomic_dec_and_test(&assoc->assoc_refcnt))
				aos_free(assoc);
	
			return ERR_OK;
		}
	}
	
	return ERR_NOT_MATCHED;
}

/* 
 * While a packet comes, firstly check the state table, and secondly, match the rules
 * and create a state for it.
 */
int match_rule_ingroup(five_tuple_t* key, user_group_t* grp, rule_assoc_t** match)
{
	struct aos_list_head *p, *n, *p1, *n1;
	rule_assoc_t* rule;
	app_flow_t* flow;
	
	if (match)
		*match = NULL;
	
	INETCTRL_INFO("tuple: src %u.%u.%u.%u/%u dst %u.%u.%u.%u/%u\n", NIPQUAD(key->src_ip), key->src_port,
				   NIPQUAD(key->dst_ip), key->dst_port);

	aos_list_for_each_safe(p, n, &grp->group_rule_head) {
		rule = (rule_assoc_t*)aos_list_entry(p, rule_assoc_t, list);
		aos_list_for_each_safe(p1, n1, &(rule->assoc_app->app_flow_head)) {
			flow = (app_flow_t*)aos_list_entry(p1, app_flow_t, list);
			switch (flow->flow_type) {
				case TYPE_FLOW_IP:
					INETCTRL_INFO("IP flow: %u.%u.%u.%u/%u.%u.%u.%u %u-%u\n", NIPQUAD(flow->flow_ip),
								   NIPQUAD(flow->flow_mask), flow->flow_startport, flow->flow_endport);
					
					if ((key->dst_ip & flow->flow_mask) != (flow->flow_ip & flow->flow_mask)) 
						continue; 
					
					INETCTRL_INFO("In ip range\n");
					
					if (flow->flow_endport > 0) {
						if ((key->dst_port < flow->flow_startport) || (key->dst_port > flow->flow_endport))
							continue;
					} else {
						if (key->dst_port != flow->flow_startport)
							continue;
					}
					/* lucky, got it */
					*match = rule;
					
					INETCTRL_INFO("In port range\n");
					return ERR_OK;
					
				case TYPE_FLOW_URL:
					INETCTRL_INFO("URL flow\n");

					break;

				default:
					return ERR_UNSUPPORT_TYPE;
					
			}
		}
	}
	
	return ERR_NOT_MATCHED;
}

state_entry_data_t* get_state_data(rule_assoc_t* rule, user_account_t* user, int nnew)
{
	state_entry_data_t* data;
	struct aos_list_head *p, *n;
	
	aos_lock(state_data_mutex);
	aos_list_for_each_safe(p, n, &state_data_head) {
		data = (state_entry_data_t*)aos_list_entry(p, state_entry_data_t, list);
		if ((data->data_account == user) && (data->data_assoc == rule)) {
			file_record_t* fr = &data->data_rec;
			if (FLAG_ISSET(fr->rec_flag, TYPE_TIME_DAY)) {
				if (is_newweek(fr->rec_weekstart) || is_newday(fr->rec_daystart)) {
					reset_online_ctrl(&data->data_time->rule_online_head, fr, 0);
					fr->rec_daystart = get_startoftoday();
				}
			}

			if (FLAG_ISSET(fr->rec_flag, TYPE_TIME_WEEK)) {
				if (is_newweek(fr->rec_weekstart)) {
					reset_online_ctrl(&data->data_time->rule_online_head, fr, 1);
					fr->rec_weekstart = get_sunoftoday();
				}
			}
			
			INETCTRL_INFO("Find a state data: timeleft %u, total_tomeleft %u\n",
					  	  data->data_timeleft, data->data_total_timeleft);
			
			aos_unlock(state_data_mutex);
			
			if (atomic_read(&data->data_refcnt) == 0)
				data->data_enter = get_secoftoday();
			return data;
		}
	}
	aos_unlock(state_data_mutex);
	
	if (nnew) {
		data = (state_entry_data_t*)aos_malloc(sizeof(*data));
		if (data == NULL)
			return NULL;
		
		memset(data, 0, sizeof(*data));
		data->data_account = user;
		data->data_assoc = rule;

		/* according to rule, initialize the data_rec */
		if (init_data_record(data) < 0) {
			aos_free(data);
			return NULL;
		}
		
		/* must hold the refernence */
		atomic_inc(&user->account_refcnt);
		atomic_inc(&rule->assoc_refcnt);

		aos_lock(state_data_mutex);
		aos_list_add_tail(&data->list, &state_data_head);
		aos_unlock(state_data_mutex);
		
		INETCTRL_INFO("New a state data: timeleft %u, total_tomeleft %u\n",
					  data->data_timeleft, data->data_total_timeleft);
		
		data->data_enter = get_secoftoday();
		return data;
	}
	
	INETCTRL_INFO("Not get a state data\n");
	return NULL;
}

int remove_state_data(state_entry_data_t* data)
{
	if (FLAG_ISSET(data->data_flag, DATA_PRE_DELETE)) {
		/* don't raise lock, because it will be used in lock */
		aos_list_del(&data->list);
		
		if (atomic_dec_and_test(&data->data_account->account_refcnt))
			aos_free(data->data_account);
		if (atomic_dec_and_test(&data->data_assoc->assoc_refcnt))
			aos_free(data->data_assoc);

		aos_free(data);
	}
	
	return 0;
}

#ifdef __KERNEL__
/* init from file */
int readfrom_online_file(void)
{
	KFILE* fp;
	state_entry_data_t* data;
	file_record_t* fr;
	char line[512];
	int num;
	
	fp = inetctrl_fopen(ONLINE_FILE, "r");
	if (fp == NULL)
		return 0; /* it is ok if file does not exist */
	
	while (inetctrl_fgets(line, 512, fp)) {
		data = aos_malloc(sizeof(*data));
		if (data == NULL) {
			inetctrl_fclose(fp);
			return -1;
		}
		
		fr = &data->data_rec;
		num = sscanf(line, "%u\t%u\t%u\t%s\t%s\t%s\t%u\t%u\n", &fr->rec_flag,
					&fr->rec_daystart, &fr->rec_weekstart, fr->rec_account, fr->rec_appname,
					fr->rec_rulename, &fr->rec_timeleft, &fr->rec_total_timeleft);
		
		if (num != 8) {
			INETCTRL_ERR("read from file error, line %s\n", line);

			aos_free(data);
			continue;
		}
		
		if (init_data_pointer(data) < 0) {
			INETCTRL_ERR("Initialize the data pointer from record error\n");
			/* 
			 * don't release the reference, because <0 returned means 
			 * we have not added the reference 
			 */
			aos_free(data);
			continue;
		}
		
		aos_lock(state_data_mutex);
		read_lock_bh(&rw_lock);
		aos_list_add_tail(&data->list, &state_data_head);
		read_unlock_bh(&rw_lock);
		aos_unlock(state_data_mutex);
	}
	
	inetctrl_fclose(fp);
	return 0;
}

/* write structure to file */
int writeto_online_file(void)
{
	KFILE* fp;
	struct aos_list_head *p, *n;
	file_record_t* fr;
	state_entry_data_t* data;
	
	fp = inetctrl_fopen(ONLINE_FILE, "w");
	if (fp == NULL)
		return -1;
	
	aos_lock(state_data_mutex);
	write_lock_bh(&rw_lock);
	aos_list_for_each_safe(p, n, &state_data_head) {
		data = (state_entry_data_t*)aos_list_entry(p, state_entry_data_t, list);
		fr = &data->data_rec;
		inetctrl_fprintf(fp, "%u\t%u\t%u\t%s\t%s\t%s\t%u\t%u\n", fr->rec_flag,
					fr->rec_daystart, fr->rec_weekstart, fr->rec_account, 
					fr->rec_appname, fr->rec_rulename, fr->rec_timeleft, 
					fr->rec_total_timeleft);
	}
	write_unlock_bh(&rw_lock);
	aos_unlock(state_data_mutex);
	
	inetctrl_fclose(fp);
	return 0;
}
#endif

/* which = 0, means resetting day type, 1 means resetting week type, 2 means both */
void reset_online_ctrl(struct aos_list_head* h, file_record_t* fr, int which)
{
	int old_day = -1, old_week = -1;
	switch (which) {
		case 0:
			old_week = fr->rec_total_timeleft;
			
			break;
		case 1:
			old_day = fr->rec_timeleft;
			
			break;
		case 2:
			break;
		default:
			return;
	}
	
	/* must before walking */
	fr->rec_timeleft = UINT_MAX;
	fr->rec_total_timeleft = UINT_MAX;
	walk_sub_list(h, walk_online, fr);
	
	if (old_day != -1)
		fr->rec_timeleft = old_day;
	
	if (old_week != -1)
		fr->rec_total_timeleft = old_week;
}

void clear_data_list(void)
{
	struct aos_list_head *p, *n;
	state_entry_data_t* data;
	
	aos_lock(state_data_mutex);
	aos_list_for_each_safe(p, n, &state_data_head) {
		data = (state_entry_data_t*)aos_list_entry(p, state_entry_data_t, list);
		if (atomic_dec_and_test(&data->data_account->account_refcnt))
			aos_free(data->data_account);
		if (atomic_dec_and_test(&data->data_assoc->assoc_refcnt))
			aos_free(data->data_assoc);

		if (!FLAG_ISSET(data->data_flag, DATA_PRE_DELETE))	
			INETCTRL_ERR("Somewhere wrong, its reference must be 0\n");

		aos_free(data);
	}
	
	AOS_INIT_LIST_HEAD(&state_data_head);
	aos_unlock(state_data_mutex);
}

/* 
 * below are the walk through function, 
 * by which, you can walk through every element of list
 */
int walk_apps_list(int (*walk_func)(void* elm, void* arg), void* arg)
{
	struct aos_list_head *p, *n;
	app_rule_t* app;
	int ret;
	
	aos_lock(app_rule_mutex);
	aos_list_for_each_safe(p, n, &app_rule_head) {
		app = (app_rule_t*)aos_list_entry(p, app_rule_t, list);
		ret = walk_func(app, arg);
		if (ret) {
			aos_unlock(app_rule_mutex);
			return ret;
		}
	}

	aos_unlock(app_rule_mutex);
	return 0;
}

int walk_timerules_list(int (*walk_func)(void* elm, void* arg), void* arg)
{
	struct aos_list_head *p, *n;
	time_rule_t* time;
	int ret;
	
	aos_lock(time_rule_mutex);
	aos_list_for_each_safe(p, n, &time_rule_head) {
		time = (time_rule_t*)aos_list_entry(p, time_rule_t, list);
		ret = walk_func(time, arg);
		if (ret) {
			aos_unlock(time_rule_mutex);
			return ret;
		}
	}

	aos_unlock(time_rule_mutex);
	return 0;
}

int walk_groups_list(int (*walk_func)(void* elm, void* arg), void* arg)
{
	struct aos_list_head *p, *n;
	user_group_t* grp;
	int ret;
	
	aos_lock(user_group_mutex);
	aos_list_for_each_safe(p, n, &user_group_head) {
		grp = (user_group_t*)aos_list_entry(p, user_group_t, list);
		ret = walk_func(grp, arg);
		if (ret) {
			aos_unlock(user_group_mutex);
			return ret;
		}
	}

	aos_unlock(user_group_mutex);
	return 0;
}

int walk_statedata_list(int (*walk_func)(void* elm, void* arg), void* arg)
{
	struct aos_list_head *p, *n;
	state_entry_data_t* data;
	int ret;
	
	aos_lock(state_data_mutex);
	aos_list_for_each_safe(p, n, &state_data_head) {
		data = (state_entry_data_t*)aos_list_entry(p, state_entry_data_t, list);
		ret = walk_func(data, arg);
		if (ret) {
			aos_unlock(state_data_mutex);
			return ret;
		}
	}

	aos_unlock(state_data_mutex);
	return 0;
}

int walk_sub_list(struct aos_list_head* head, int (*walk_func)(void* elm, void* arg), void* arg)
{
	struct aos_list_head *p, *n;
	int ret;
	
	aos_list_for_each_safe(p, n, head) {
		ret = walk_func(p, arg);
		if (ret)
			return ret;
	}

	return 0;
} 

/* time handle */
int str2secs(char *time_str)
{
	u8 hh, mm, ss;
	int num;
	
	num = sscanf(time_str, "%hhd:%hhd:%hhd", &hh, &mm, &ss);
	if (num != 3)
		return -1;

	if (hh >= 24 || mm >= 60 || ss >= 60)
		return -1;

	return 	(hh*3600 + mm*60 + ss);
}

char* secs2str(u32 sec)
{
	static char _time_str[sizeof "xx:xx:xx"];
	int hh, mm, ss;
	
	memset(_time_str, 0, sizeof(_time_str));
	
	hh = sec / 3600;
	hh %= 24;
	
	sec %= 3600;
	mm = sec / 60;
	
	ss = sec % 60;

#ifndef __KERNEL__
	snprintf(_time_str, sizeof(_time_str), "%hhu:%hhu:%hhu", hh, mm, ss);
#else
	snprintf(_time_str, sizeof(_time_str), "%u:%u:%u", hh, mm, ss);
#endif
	return _time_str;
}

int get_secoftoday()
{
	int secs, ss, mm, hh;
	/* struct timeval tv; */
#ifdef __KERNEL__
	/* do_gettimeofday(&tv);*/
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	secs = tv.tv_sec;
#endif
	
	ss = secs % 60;
	
	secs /= 60;
	mm = secs % 60;
	
	secs /= 60;
	hh = secs % 24;
	
	return (hh*3600 + mm*60 + ss);
/*	
	return tv.tv_sec;*/
}

/* return 0-7 for sun, mon, ... */
int get_weekoftoday(void)
{
	u32 curr_secs;
	
#ifdef __KERNEL__
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	curr_secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	curr_secs = time(NULL);
#endif
	
	curr_secs /= 3600; /* hours since 1970 */
	curr_secs /= 24; /* days since 1970*/

	/*
	curr_secs %= 7;*/ /* increasing value for week */
	
	/* 1/1/1970 is thursday */
	return ((Thursday + curr_secs) % 7);
}

/* return seconds of the start point(00:00:00) of today since 1970/1/1 */
int get_startoftoday(void)
{
	u32 curr_secs;
	
#ifdef __KERNEL__
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	curr_secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	curr_secs = time(NULL);
#endif
	
	curr_secs /= 3600; /* hours since 1970 */
	curr_secs /= 24; /* days since 1970*/
	
	return (curr_secs * 86400); /* 3600*24 */
}

/* note: old must the starting time of today */
int is_newday(int old)
{
	u32 curr_secs;
	
#ifdef __KERNEL__
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	curr_secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	curr_secs = time(NULL);
#endif

	return ((curr_secs - old) >= 86400); /* 3600*24 */
}

/* return the monday today is in */
int get_sunoftoday(void)
{
	u32 curr_secs;
	int days, weekday;
	
#ifdef __KERNEL__
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	curr_secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	curr_secs = time(NULL);
#endif
	
	days = curr_secs/86400; /* 3600*24 */
	weekday = (Thursday + days) % 7;
	days = days * 86400;
	
	return (days - ((weekday - Monday) * 86400));
}

/* note: old must the starting time of monday */
int is_newweek(int old)
{
	u32 curr_secs;
	
#ifdef __KERNEL__
	/* get all seconds from 1970 */
	extern struct timezone sys_tz;
	curr_secs = get_seconds() - sys_tz.tz_minuteswest * 60;
#else
	curr_secs = time(NULL);
#endif
	
	return ((curr_secs-old) >= 604800); /* 86400*7 */
}

int str2days(char *days_str)
{
	int days = 0, num = 0, quit = 0;
	char day_str[7][8], *token, *start;
	
	start = days_str;
	while (!quit) {
		token = strchr(start, '|');
		if (token)
			*token = 0;
		else
			quit = 1;

		strcpy(day_str[num], start);
		num++;
		
		if (token) {
			*token = '|';
			start = token + 1;
		}
	}
	
	while (num--) { /* don't change, must use num-- */
		if (strcasecmp("sun", day_str[num]) == 0)
			days |= DayMask(Sunday);
		else if (strcasecmp("mon", day_str[num]) == 0)
			days |= DayMask(Monday);
		else if (strcasecmp("tue", day_str[num]) == 0)
			days |= DayMask(Tuesday);
		else if (strcasecmp("wed", day_str[num]) == 0)
			days |= DayMask(Wednesday);
		else if (strcasecmp("thu", day_str[num]) == 0)
			days |= DayMask(Thursday);
		else if (strcasecmp("fri", day_str[num]) == 0)
			days |= DayMask(Friday);
		else if (strcasecmp("sat", day_str[num]) == 0)
			days |= DayMask(Saturday);
		else
			return -1;
	}
	
	return days;
}

char* days2str(u32 days)
{
	static char _days_str[64];
	
	memset(_days_str, 0, sizeof(_days_str));
	
	if (FLAG_ISSET(days, DayMask(Sunday)))
		strcat(_days_str, "|SUN");
	
	if (FLAG_ISSET(days, DayMask(Monday)))
		strcat(_days_str, "|MON");
		
	if (FLAG_ISSET(days, DayMask(Tuesday)))
		strcat(_days_str, "|TUE");
	
	if (FLAG_ISSET(days, DayMask(Wednesday)))
		strcat(_days_str, "|WED");
	
	if (FLAG_ISSET(days, DayMask(Thursday)))
		strcat(_days_str, "|THU");
		
	if (FLAG_ISSET(days, DayMask(Friday)))
		strcat(_days_str, "|FRI");
	
	if (FLAG_ISSET(days, DayMask(Saturday)))
		strcat(_days_str, "|SAT");
	
	if (_days_str[0] == 0)
		strcpy(_days_str, "NONE");
	
	if (_days_str[0] == '|')
		return (_days_str+1);
	
	return _days_str;
}

char* inet_ntoa(u32 ip)
{
    static char ip_str[sizeof "255.255.255.255"];
    u8 *src = (u8 *)&ip;

#ifndef __KERNEL__
    sprintf(ip_str, "%hhu.%hhu.%hhu.%hhu", src[0], src[1], src[2], src[3]);
#else
	sprintf(ip_str, "%u.%u.%u.%u", src[0], src[1], src[2], src[3]);
#endif
    return ip_str;
}

u32 inet_addr(char* ip_str) 
{
    u32 ip; 
   	u8* p;

	p = (u8*)&ip;
    if (sscanf(ip_str, "%hhd.%hhd.%hhd.%hhd", p, p+1, p+2, p+3) != 4) 
		return 0;

    return ip;
}

int strcasecmp(const char *s1, const char *s2)
{
	u8 c1, c2;

	do {
		 c1 = tolower(*s1++);
		 c2 = tolower(*s2++);
	} while (c1 == c2 && c1 != 0 && c2 != 0);
	return c1 - c2;
}

void inetctrl_log_f(char*file, int line, int access, five_tuple_t* key, char *mesg)
{
	int secs;													

	if (access > log_level)
		return;

#ifdef __KERNEL__
	extern struct timezone sys_tz;								
	secs = get_seconds() - sys_tz.tz_minuteswest * 60;			
#else
	struct timeval tv;											
	gettimeofday(&tv, 0);										
	secs = tv.tv_sec;											
#endif

	switch (access) {
		case BLOCK:
			printk(KERN_INFO"<%s:%d>%s Block LOGLEVEL:%d tuple: src %u.%u.%u.%u/%u dst %u.%u.%u.%u/%u proto: %s Result: %s",
					file, line, 
					secs2str(secs),								 
					log_level, 
					NIPQUAD((key)->src_ip), 
					key->src_port, 
					NIPQUAD((key)->dst_ip),
					key->dst_port, 
					((key->proto == FLOW_TYPE_TCP)?"tcp":"udp"),
					mesg);
			break;

		case ALLOW:
			printk(KERN_INFO"<%s:%d>%s Allow LOGLEVEL:%d tuple: src %u.%u.%u.%u/%u dst %u.%u.%u.%u/%u proto: %s Result: %s",
					file, line,
					secs2str(secs),								 
					log_level,
					NIPQUAD((key)->src_ip),
					key->src_port, 
					NIPQUAD((key)->dst_ip),
					key->dst_port, 
					((key->proto == FLOW_TYPE_TCP)?"tcp":"udp"),
					mesg);
			break;

		default:
			break;
	}
}


