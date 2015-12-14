#include "inet_ctrl_cli.h"
#include "inet_ctrl.h"
#include "inet_ctrl_printf.h"
#include "inet_ctrl_timer.h"
#include "inet_ctrl_hash.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/List.h"
#include "KernelSimu/string.h"

// #include <linux/string.h>
/*
 * Below are the CLIs:
 	1. app
 	
		inetctrl app name <name>
		inetctrl app addflow ip <name> <dst ip> <dst mask> <start port> <end port>
		inetctrl app addflow url <name> <url> <start port> <end port>
		inetctrl app revflow ip <name> <dst ip> <dst mask> <start port> <end port>
		inetctrl app revflow url <name> <url> <start port> <end port>
		
		inetctrl app remove <name>
		
		inetctrl app show config
		
	2. Time Rule
		
		inetctrl timerule name <name>
		inetctrl timerule addrule time <name> <start time> <end time> <time days>
		inetctrl timerule addrule online <name> <online type> <online minutes> <online days>
		inetctrl timerule revrule time <name> <start time> <end time> <time days>
		inetctrl timerule revrule online <name> <online type> <online minutes> <online days>
		
		inetctrl timerule remove <name>
		
		inetctrl timerule show config
		
	3. Group
		
		inetctrl group name <name>
		inetctrl group adduser byname <name> <username>
		inetctrl group adduser byip <name> <ip> <mask>
		inetctrl group adduser bymac <name> <mac>
		inetctrl group revuser byname <name> <username>
		inetctrl group revuser byip <name> <ip> <mask>
		inetctrl group revuser bymac <name> <mac>
		
		inetctrl group addrule <name> <appname> <timename>
		inetctrl group revrule <name> <appname> <timename>
		
		inetctrl group defautpolicy <name> <access/deny>
		
		inetctrl group remove <name>
		
		inetctrl group show config
	
	4. Global
		inetctrl on/off
		inetctrl show config
 *
 */

#define PARENT_LIST_NEW		0x1
#define CHILD_LIST_NEW		0x2

int inetctrl_switch = 0; /* off */
int log_level = 0;
int default_policy = 2;

typedef struct show_struct {
	u32 flag;
	int maxlen;
	int* pindex;
	char* pdata;
} show_struct_t;

typedef struct save_elm {
	show_struct_t* content;
	char *name;
} save_elm_t;
/* switch for inetctrl */
static int inetctrl_onoff(int onoff)
{
	if (inetctrl_switch == onoff)
		return (onoff == 1)?-2:-3;
	
	inetctrl_switch = onoff;

#ifdef __KERNEL__
	if (onoff == 1) { /* off -> on */
		inetctrl_init_timer();
	} else {
		inetctrl_remove_timer();

		/* Clear tab must be ahead */
		inet_ctrl_cleartab();
		clear_data_list();
	}
#endif

	return 0;
}

/* CLI functions */
static int 
inetctrl_app_name(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	app_rule_t* app;
	char* name;
	
	*length = 0;
	name = parms->mStrings[0];
	
	/* new it if not exist */
	app = get_app_rule(name, 0);
	if (app) {
		snprintf(errmsg, errlen-1, "App %s exists\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_NameExist;
	}
	
	app = get_app_rule(name, 1);
	if (app == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add app %s: kmalloc error" 
				"or the name is too long\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_MallocFailed;
	}
	
	return ret;
}

static int
inetctrl_app_addflow_ip(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *proto;
	u32 ip, mask;
	u16 sp, dp;
	app_flow_t* flow;
	u8 protocol;

	*length = 0;
	name = parms->mStrings[0];
	proto = parms->mStrings[1];
	ip = parms->mIntegers[0];
	mask = parms->mIntegers[1];
	sp = parms->mIntegers[2];
	dp = parms->mIntegers[3];
	
	if (sp > dp && dp != 0) {
		snprintf(errmsg, errlen-1, "Failed to add this flow:"
				 "the endport must be big than the startport");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	if (strcasecmp(proto, "tcp") == 0)
		protocol = FLOW_TYPE_TCP;
	else if (strcasecmp(proto, "udp") == 0)
		protocol = FLOW_TYPE_UDP;
	else {
		snprintf(errmsg, errlen-1, "Failed to add this flow:"
				 "the protocol must be tcp/udp\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	flow = get_app_ip_flow(name, protocol, ip, mask, sp, dp);
	if (flow == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add this flow:"
				 " app %s doesn't exist or kmalloc error"
				 " or this flow rule already exist\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_FlowExist;
	}

	flow->flow_type = TYPE_FLOW_IP;
	
	/* set protocol */
	if (protocol == FLOW_TYPE_TCP)
		flow->flow_proto = FLOW_TYPE_TCP;
	else 
		flow->flow_proto = FLOW_TYPE_UDP;

	
	/* set ip and mask */
	flow->flow_ip = ip;
	flow->flow_mask = mask;
	flow->flow_startport = sp;
	flow->flow_endport = dp;
	
	return ret;
}

static int 
inetctrl_app_addflow_url(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *proto, *domain;
	app_flow_t* flow;
	u16 sp, dp;
	u8 protocol;

	*length = 0;
	name = parms->mStrings[0];
	proto = parms->mStrings[1];
	domain = parms->mStrings[2];
	sp = parms->mIntegers[0];
	dp = parms->mIntegers[1];

	if (strcasecmp(proto, "tcp") == 0)
		protocol = FLOW_TYPE_TCP;
	else if (strcasecmp(proto, "udp") == 0)
		protocol = FLOW_TYPE_UDP;
	else {
		snprintf(errmsg, errlen-1, "Failed to add this flow:"
				 "the protoc must be tcp/udp\n");
		errmsg[errlen-1] = 0;
		return -1;
	}
	
	flow = get_app_url_flow(name, protocol, domain, sp, dp);
	if (flow == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add this flow:"
			     " app %s doesn't exist or kmalloc error"
				 "or this flow already exist\n", name);
		errmsg[errlen-1] = 0;
		return -1;
	}

	flow->flow_type = TYPE_FLOW_URL;
	
	/* set protocol */
	if (protocol == FLOW_TYPE_TCP)
		flow->flow_proto = FLOW_TYPE_TCP;
	else 
		flow->flow_proto = FLOW_TYPE_UDP;
	
	/* set ip and mask */
	strncpy(flow->flow_domain, domain, MAX_DOMAIN_LEN);
	flow->flow_domain[MAX_DOMAIN_LEN] = 0;
	flow->flow_startport = sp;
	flow->flow_endport = dp;
	
	return ret;
}

static int 
inetctrl_app_revflow_ip(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char* name, *proto;
	u32 ip, mask;
	u16 sp, dp;
	
	*length = 0;
	name = parms->mStrings[0];
	proto = parms->mStrings[1];
	ip = parms->mIntegers[0];
	mask = parms->mIntegers[1];
	sp = parms->mIntegers[2];
	dp = parms->mIntegers[3];
	
	ret = remove_app_flow_bynet(name, proto, ip, mask, sp, dp);
	switch (ret) {
		case ERR_APP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove this flow:"
			         " app %s doesn't exist\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;
			break;
		
		case ERR_OK:
			ret = 0;
			break;
		
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove this flow"
			         " app %s contains no this flow\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_FlowNotExist;
			break;
	}
	
	return ret;
}

static int 
inetctrl_app_revflow_url(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *url, *name, *proto;
	u16 sp, dp;
	
	*length = 0;
	name = parms->mStrings[0];
	proto = parms->mStrings[1];
	url = parms->mStrings[2];
	sp = parms->mIntegers[0];
	dp = parms->mIntegers[1];
	
	ret = remove_app_flow_byhost(name, proto, url, sp, dp);
	switch (ret) {
		case ERR_APP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove this flow:"
			         " app %s doesn't exist\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
		
		case ERR_PROTO_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove this flow:"
			         " proto error\n");
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;

		case ERR_OK:
			ret = 0;
			break;
		
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove this flow"
			         " app %s contains no this flow\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
	}
	
	return ret;
}

static int 
inetctrl_app_remove(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char* name = parms->mStrings[0];
	*length = 0;
	
	switch (remove_app_rule(name)) {
		case ERR_APP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove app %s(not exist)\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;			
			break;
			
		case ERR_APP_USEING:			
			snprintf(errmsg, errlen-1, "Failed to remove app %s(in use)\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_InUsed;
			break;
		
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove app %s\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int show_flows_ofapp(void* p, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	app_flow_t* pflow = (app_flow_t*)aos_list_entry((struct aos_list_head*)p, app_flow_t, list);
	char tmp[256], tmp1[256], ip[64], mask[64];
	static int flow_index = 0;
	
	if (show == NULL)
		return -1;
	if (pflow == NULL)
		return 0;
	
	/* start show */
	if (FLAG_ISSET(show->flag, CHILD_LIST_NEW)) {
		FLAG_UNSET(show->flag, CHILD_LIST_NEW);
		flow_index = 0;
	}
	
	flow_index++;

	/* index, type and flow*/
	if (pflow->flow_type == TYPE_FLOW_URL) {
		my_sprintf(YELLOW, tmp, "  %-*dURL    %-*s%-*s%d-%d\n", 
				   8, flow_index, 10, (pflow->flow_proto==FLOW_TYPE_TCP)?"TCP":"UDP",
				   32, pflow->flow_domain, pflow->flow_startport, pflow->flow_endport);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (pflow->flow_type == TYPE_FLOW_IP) {
		strcpy(ip, inet_ntoa(pflow->flow_ip));
		strcpy(mask, inet_ntoa(pflow->flow_mask));
		sprintf(tmp1, "%s/%s", ip, mask);
		
		my_sprintf(YELLOW, tmp, "  %-*dIP     %-*s%-*s%d-%d\n", 
				   8, flow_index, 10, (pflow->flow_proto==FLOW_TYPE_TCP)?"TCP":"UDP",
				   32, tmp1, pflow->flow_startport, pflow->flow_endport);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else {
		my_sprintf(YELLOW, tmp, "  %-*dUNKNOWN\n", 8, flow_index);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	}
	
	return 0;
}


static int save_flows_ofapp(void* p, void* arg)
{
	save_elm_t* save = (save_elm_t*)arg;
	show_struct_t* show = (show_struct_t*)save->content;
	char* name = save->name;

	app_flow_t* pflow = (app_flow_t*)aos_list_entry((struct aos_list_head*)p, app_flow_t, list);
	char tmp[256], ip[64], mask[64];
	
	if (show == NULL)
		return -1;
	if (pflow == NULL)
		return 0;

	/* index, type and flow*/
	if (pflow->flow_type == TYPE_FLOW_URL) {
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl app addflow url %s %s %s %d %d</Cmd>\n", 
				   name, (pflow->flow_proto==FLOW_TYPE_TCP)?"tcp":"udp",
				   pflow->flow_domain, pflow->flow_startport, pflow->flow_endport);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (pflow->flow_type == TYPE_FLOW_IP) {
		strcpy(ip, inet_ntoa(pflow->flow_ip));
		strcpy(mask, inet_ntoa(pflow->flow_mask));
		
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl app addflow ip %s %s %s %s %d %d</Cmd>\n", 
				   name, (pflow->flow_proto==FLOW_TYPE_TCP)?"tcp":"udp", 
				   ip, mask, pflow->flow_startport, pflow->flow_endport);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else 
		return -1;
	
	return 0;
}


static int show_one_app(void* app, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	app_rule_t* papp = (app_rule_t*)app;
	char tmp[256];
	static int app_index = 0;
	
	if (show == NULL)
		return -1;
	
	if (app == NULL) /* next */
		return 0;
	
	if (FLAG_ISSET(show->flag, PARENT_LIST_NEW)) {
		FLAG_UNSET(show->flag, PARENT_LIST_NEW);
		app_index = 0;
	}
	app_index++;
	
	/* start show */
	my_sprintf(YELLOW, tmp, "Application Index: %d\n", app_index);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* name */
	my_sprintf(YELLOW, tmp, "Application Name: %s\n", papp->app_name);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* flows */
	my_sprintf(YELLOW, tmp, "Flows List:\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	my_sprintf(YELLOW, tmp, "  Index   Type   Protocol  Host/IP%-*sPort\n", 25, " ");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	FLAG_SET(show->flag, CHILD_LIST_NEW);
	walk_sub_list(&papp->app_flow_head, show_flows_ofapp, show);
	
	my_sprintf(YELLOW, tmp, "\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int save_one_app(void* app, void* arg)
{
	app_rule_t* papp = (app_rule_t*)app;
	save_elm_t save;

	char tmp[256];
	int ret;

	save.content = (show_struct_t*)arg;
	save.name = ((app_rule_t*)papp)->app_name;
	if (save.content == NULL)
		return -1;
	
	if (app == NULL) /* next */
		return 0;
	
	/* name */
	my_sprintf(YELLOW, tmp, "<Cmd>inetctrl app name %s</Cmd>\n", save.name);
	aosCheckAndCopy(save.content->pdata, (unsigned int*)save.content->pindex, save.content->maxlen, tmp, strlen(tmp));
	
	ret = walk_sub_list(&papp->app_flow_head, save_flows_ofapp, &save);
	if (ret)
		return ret;

	return 0;
}


static int 
inetctrl_app_show_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0, index = 0;
	char* buff, tmp[256];
	show_struct_t show;
	
	memset(&show, 0, sizeof(show));
	
	buff = aosKernelApi_getBuff(data);
	show.pindex = &index;
	show.pdata = buff;
	show.maxlen = *length;
	
	/* print the header */
	my_sprintf(BLUE, tmp, "------------Begin Application List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	FLAG_SET(show.flag, PARENT_LIST_NEW);
	ret = walk_apps_list(show_one_app, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to show app(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}
	
	/* print the end */
	my_sprintf(BLUE, tmp, "------------End Application List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	*length = index;	
	return 0;
}

//add by xyb
static int 
inetctrl_app_clear_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	*length = 0;

	switch (clear_app_rule()) {
		case ERR_APP_USEING:
			snprintf(errmsg, errlen-1, "Failed to clear app config, some app(in use)\n");
			errmsg[errlen-1] = 0;
			return -eAosRc_InUsed;

		case ERR_OK:
			break;

		default:
			snprintf(errmsg, errlen-1, "Failed to clear all apps\n");
			errmsg[errlen-1] = 0;
			return -eAosRc_LackFailed;
	}

	return 0;
}

static int
inetctrl_timerule_name(char *data,
	unsigned int *length,
	struct aosKernelApiParms *parms,
	char *errmsg,
	const int errlen)
{
	int ret = 0;
	time_rule_t* time;
	char* name;
	
	*length = 0;
	name = parms->mStrings[0];
	
	/* new it if not exist */
	time = get_time_rule(name, 0);
	if (time) {
		snprintf(errmsg, errlen-1, "Timectrl %s exists\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_NameExist;
	}
	
	time = get_time_rule(name, 1);
	if (time == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add timectrl %s: kmalloc error \
				or the name is too long\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_MallocFailed;
	}
	
	return ret;
}

static int 
inetctrl_timerule_addrule_time(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *start, *end, *days;
	time_ctrl_t* timectrl;
	int start_sec, end_sec, days_bit;
	
	*length = 0;
	name = parms->mStrings[0];
	start = parms->mStrings[1];
	end = parms->mStrings[2];
	days = parms->mStrings[3];	
	
	start_sec = str2secs(start);
	if (start_sec == -1) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " format of start time %s is not correct\n", start);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	end_sec = str2secs(end);
	if (end_sec == -1) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " format of end time %s is not correct\n", end);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	if (start_sec > end_sec) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " end start time must be earlier than the end time\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	days_bit = str2days(days);
	if (days_bit == -1) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " format of days %s is not correct\n", days);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	timectrl = get_time_ctrl(name, start_sec, end_sec, days_bit);
	if (timectrl == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " Timectrl %s doesn't exist or kmalloc error"
				 " or this rule is already exist\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_MallocFailedTimeCtrNoExistRuleExist;
	}
	
	timectrl->time_start = start_sec;
	timectrl->time_end = end_sec;
	timectrl->time_days = days_bit;
	
	return ret;
}

static int 
inetctrl_timerule_addrule_online(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *type, *days;
	online_ctrl_t* onlinectrl;
	u32 mins, timetype;
	int days_bit;
	
	*length = 0;
	name = parms->mStrings[0];
	type = parms->mStrings[1];
	days = parms->mStrings[2];
	mins = parms->mIntegers[0];	
	
	days_bit = str2days(days);
	if (days_bit == -1) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " format of days %s is not correct\n", days);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	if (strcasecmp(type, "day") == 0)
		timetype = TYPE_TIME_DAY;
	else if (strcasecmp(type, "week") == 0)
		timetype= TYPE_TIME_WEEK;
	else {
		snprintf(errmsg, errlen-1, "Failed to add this online control:"
				 " the online type only support day/week \n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	if (timetype == TYPE_TIME_DAY && mins > 60*24) {
		snprintf(errmsg, errlen-1, "Failed to add this online control:"
				 " Duration minutes is lager than a day's minutes\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	} else if (mins > 60*24*7) {
		snprintf(errmsg, errlen-1, "Failed to add this online control:"
				 " Duration minutes is lager than a week's minutes\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	onlinectrl = get_online_ctrl(name, timetype, mins, days_bit);
	if (onlinectrl == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add this online control:"
				 " Timectrl %s doesn't exist or kmalloc error\n"
				 " or this online rule already exist\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_MallocFailedTimeCtrNoExistOnlineExist;
	}
	
	if (strcasecmp(type, "day") == 0)
		onlinectrl->online_type = TYPE_TIME_DAY;
	else if (strcasecmp(type, "week") == 0)
		onlinectrl->online_type = TYPE_TIME_WEEK;

	onlinectrl->online_mins = mins;
	onlinectrl->online_days = days_bit;
	
	setflag_data_modified(name, onlinectrl);

	return ret;
}

static int
inetctrl_timerule_revrule_time(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *start, *end, *days;
	int start_sec, end_sec, days_bit;
	
	*length = 0;
	name = parms->mStrings[0];
	start = parms->mStrings[1];
	end = parms->mStrings[2];
	days = parms->mStrings[3];	
	
	start_sec = str2secs(start);
	if (start_sec == -1) {
		snprintf(errmsg, errlen-1, "Failed to remove this time control:"
				 " format of start time %s is not correct\n", start);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	end_sec = str2secs(end);
	if (end_sec == -1) {
		snprintf(errmsg, errlen-1, "Failed to remove this time control:"
				 " format of end time %s is not correct\n", end);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	days_bit = str2days(days);
	if (days_bit == -1) {
		snprintf(errmsg, errlen-1, "Failed to remove this time control:"
				 " format of days %s is not correct\n", days);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	ret = remove_timerule_timectrl(name, start_sec, end_sec, days_bit);
	switch (ret) {
		case ERR_TIME_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove this timectrl:"
			         " Timerule %s doesn't exist\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;
			break;
		
		case ERR_OK:
			ret = 0;
			break;
		
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove this timectrl"
			         " Timerule %s contains no this timectrl\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_TimeCtrlNotExist;
			break;
	}
	
	return ret;
}

static int
inetctrl_timerule_revrule_online(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *name, *type_str, *days;
	u32 mins, type;
	int days_bit;
	
	*length = 0;
	name = parms->mStrings[0];
	type_str = parms->mStrings[1];
	days = parms->mStrings[2];
	mins = parms->mIntegers[0];	
	
	days_bit = str2days(days);
	if (days_bit == -1) {
		snprintf(errmsg, errlen-1, "Failed to add this time control:"
				 " format of days %s is not correct\n", days);
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}
	
	if (strcasecmp(type_str, "day") == 0)
		type = TYPE_TIME_DAY;
	else if (strcasecmp(type_str, "week") == 0)
		type = TYPE_TIME_WEEK;
	else {
		snprintf(errmsg, errlen-1, "Failed to remove this online control:"
				 " the online type only supports day/week\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_ArgumentErr;
	}

	ret = remove_timerule_onlinectrl(name, type, mins, days_bit);
	switch (ret) {
		case ERR_TIME_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove this onlinectrl:"
			         " Timerule %s doesn't exist\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;
			break;
		
		case ERR_OK:
			ret = 0;
			break;
		
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove this onlinectrl"
			         " Timerule %s contains no this onlinectrl\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_OnlineNotExist;
			break;
	}
	
	return ret;
}

static int 
inetctrl_timerule_remove(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char* name = parms->mStrings[0];
	*length = 0;
	
	switch (remove_time_rule(name)) {
		case ERR_TIME_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove timectrl %s(not exist)\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;			
			break;
			
		case ERR_TIME_USEING:			
			snprintf(errmsg, errlen-1, "Failed to remove timectrl %s(in use)\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_InUsed;
			break;
		
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove timectrl %s\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int show_timectrl_oftime(void* p, void* arg)
{
	
#define	TIME_MAX_LEN		sizeof("xx:xx:xx")

	show_struct_t* show = (show_struct_t*)arg;
	time_ctrl_t* ptime = (time_ctrl_t*)aos_list_entry((struct aos_list_head*)p, time_ctrl_t, list);
	char tmp[256], str_start[TIME_MAX_LEN], str_end[TIME_MAX_LEN];
	static int timectrl_index = 0;
	
	if (show == NULL)
		return -1;
	if (ptime == NULL)
		return 0;
	
	/* start show */
	if (FLAG_ISSET(show->flag, CHILD_LIST_NEW)) {
		FLAG_UNSET(show->flag, CHILD_LIST_NEW);
		timectrl_index = 0;
	}
	timectrl_index++;
	
	strcpy(str_start, secs2str(ptime->time_start));
	strcpy(str_end, secs2str(ptime->time_end));
	my_sprintf(YELLOW, tmp, "  %-*d%-*s%-*s%s\n", 8, timectrl_index, 12, str_start,
			   12, str_end, days2str(ptime->time_days));
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int save_timectrl_oftime(void* p, void* arg)
{
	
#define	TIME_MAX_LEN		sizeof("xx:xx:xx")

	save_elm_t* save = (save_elm_t*)arg;
	show_struct_t* show = (show_struct_t*)save->content;
	char *name = save->name;

	time_ctrl_t* ptime = (time_ctrl_t*)aos_list_entry((struct aos_list_head*)p, time_ctrl_t, list);
	char tmp[256], str_start[TIME_MAX_LEN], str_end[TIME_MAX_LEN];
	
	if (show == NULL)
		return -1;
	if (ptime == NULL)
		return 0;
	
	/* start save */
	strcpy(str_start, secs2str(ptime->time_start));
	strcpy(str_end, secs2str(ptime->time_end));
	my_sprintf(YELLOW, tmp, "<Cmd>inetctrl timerule addrule time %s %s %s %s</Cmd>\n", name, str_start,
			   str_end, days2str(ptime->time_days));
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int show_onlinectrl_oftime(void* p, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	online_ctrl_t* ponline = (online_ctrl_t*)aos_list_entry((struct aos_list_head*)p, online_ctrl_t, list);
	char tmp[256], mm[16];
	static int onlinectrl_index = 0;
	
	if (show == NULL)
		return -1;
	if (ponline == NULL)
		return 0;
	
	/* start show */
	if (FLAG_ISSET(show->flag, CHILD_LIST_NEW)) {
		FLAG_UNSET(show->flag, CHILD_LIST_NEW);
		onlinectrl_index = 0;
	}
	onlinectrl_index++;
	
	/* index, type and mins*/
	sprintf(mm, "%uM", ponline->online_mins);
	if (ponline->online_type == TYPE_TIME_DAY) {
		my_sprintf(YELLOW, tmp, "  %-*dDAY    %-*s%s\n", 8, onlinectrl_index,
			   7, mm, days2str(ponline->online_days));
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (ponline->online_type == TYPE_TIME_WEEK) {
		my_sprintf(YELLOW, tmp, "  %-*dWEEK   %-*s%s\n", 8, onlinectrl_index,
			   7, mm, days2str(ponline->online_days));
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else {
		my_sprintf(YELLOW, tmp, "  %-*dUNKOWN  0M     NONE\n", 8, onlinectrl_index);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	}
	
	return 0;
}

static int save_onlinectrl_oftime(void* p, void* arg)
{
	save_elm_t* save = (save_elm_t*)arg;
	show_struct_t* show = (show_struct_t*)save->content;
	char* name = save->name;

	online_ctrl_t* ponline = (online_ctrl_t*)aos_list_entry((struct aos_list_head*)p, online_ctrl_t, list);
	char tmp[256];
	
	if (show == NULL)
		return -1;
	if (ponline == NULL)
		return 0;
	
	/* start save */
	if (ponline->online_type == TYPE_TIME_DAY) {
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl timerule addrule online %s day %u %s</Cmd>\n", name, 
			   ponline->online_mins, days2str(ponline->online_days));
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (ponline->online_type == TYPE_TIME_WEEK) {
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl timerule addrule online %s week %u %s</Cmd>\n", name,
			   ponline->online_mins, days2str(ponline->online_days));
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else
		return -1;
	
	return 0;
}


static int show_one_timerule(void* elm, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	time_rule_t* p = (time_rule_t*)elm;
	char tmp[256];
	static int time_index = 0;
	
	if (show == NULL)
		return -1;
	
	if (p == NULL) /* next */
		return 0;
	
	if (FLAG_ISSET(show->flag, PARENT_LIST_NEW)) {
		FLAG_UNSET(show->flag, PARENT_LIST_NEW);
		time_index = 0;
	}
	
	time_index++;
	
	/* start show */
	my_sprintf(YELLOW, tmp, "Time Rule Index: %d\n", time_index);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* name */
	my_sprintf(YELLOW, tmp, "Time Rule Name: %s\n", p->rule_name);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* time ctrl */
	my_sprintf(YELLOW, tmp, "Time Ctrl List:\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	my_sprintf(YELLOW, tmp, "  Index   Start       End         Days\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	FLAG_SET(show->flag, CHILD_LIST_NEW);
	walk_sub_list(&p->rule_time_head, show_timectrl_oftime, show);
	
	/* online ctrl */
	my_sprintf(YELLOW, tmp, "Online Ctrl List:\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	my_sprintf(YELLOW, tmp, "  Index   Type   Mins   Days\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	FLAG_SET(show->flag, CHILD_LIST_NEW);
	walk_sub_list(&p->rule_online_head, show_onlinectrl_oftime, show);
	
	my_sprintf(YELLOW, tmp, "\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int save_one_timerule(void* elm, void* arg)
{
	time_rule_t* p = (time_rule_t*)elm;
	save_elm_t save;
	char tmp[256];
	int ret;
	
	save.content = (show_struct_t*)arg;
	save.name = p->rule_name;
	if (save.content == NULL)
		return -1;
	
	if (p == NULL) /* next */
		return 0;
	
	/* name */
	my_sprintf(YELLOW, tmp, "<Cmd>inetctrl timerule name %s</Cmd>\n", save.name);
	aosCheckAndCopy(save.content->pdata, (unsigned int*)save.content->pindex, save.content->maxlen, tmp, strlen(tmp));

	ret = walk_sub_list(&p->rule_time_head, save_timectrl_oftime, &save);
	if (ret)
		return ret;

	/* online ctrl */
	ret = walk_sub_list(&p->rule_online_head, save_onlinectrl_oftime, &save);
	if (ret)
		return ret;
	
	return 0;
}


static int 
inetctrl_timerule_show_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0, index = 0;
	char* buff, tmp[256];
	show_struct_t show;
	
	memset(&show, 0, sizeof(show));
	
	buff = aosKernelApi_getBuff(data);
	show.pindex = &index;
	show.pdata = buff;
	show.maxlen = *length;
	
	/* print the header */
	my_sprintf(BLUE, tmp, "------------Begin Time Rule List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	FLAG_SET(show.flag, PARENT_LIST_NEW);
	ret = walk_timerules_list(show_one_timerule, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to show timerule(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}
	
	/* print the end */
	my_sprintf(BLUE, tmp, "------------End Time Rule List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	*length = index;	
	return 0;
}

//add by xyb
static int 
inetctrl_timerule_clear_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	*length = 0;

	switch (clear_time_rule()) {
		case ERR_TIME_USEING:
			snprintf(errmsg, errlen-1, "Failed to clear timectrl config, some timectrl(in use)\n");
			errmsg[errlen-1] = 0;
			return -eAosRc_InUsed;

		case ERR_OK:
			break;

		default:
			snprintf(errmsg, errlen-1, "Failed to clear all timerule\n");
			errmsg[errlen-1] = 0;
			return -eAosRc_LackFailed;
	}

	return 0;
}

static int 
inetctrl_group_name(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	user_group_t* grp;
	char* name;
	
	*length = 0;
	name = parms->mStrings[0];
	
	/* new it if not exist */
	grp = get_user_group(name, 0);
	if (grp) {
		snprintf(errmsg, errlen-1, "Group %s exists\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_NameExist;
	}
	
	grp = get_user_group(name, 1);
	if (grp == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add group %s: kmalloc error \
				or the name is too long\n", name);
		errmsg[errlen-1] = 0;
		return -eAosRc_MallocFailed;
	}
	
	grp->group_default_access = ACCESS_DENY;
	return ret;
}

static int 
inetctrl_group_adduser_byname(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	user_account_t* account;
	char *gname, *uname;
	
	*length = 0;
	gname = parms->mStrings[0];
	uname = parms->mStrings[1];
	
	ret = get_account_byname(uname, gname, &account);
	switch (ret) {
		case ERR_ACCOUNT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account %s(exist)\n", uname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_MEMORY_ALLOC:
			snprintf(errmsg, errlen-1, "Failed to add account %s(Kmalloc error)\n", uname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account %s(group %s not exist)\n",
					 uname, gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to add account %s to group %s\n",
					 uname, gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_adduser_byip(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	user_account_t* account;
	char *gname;
	u32 ip, mask;
	
	*length = 0;
	gname = parms->mStrings[0];
	ip = parms->mIntegers[0];
	mask = parms->mIntegers[1];
	
	ret = get_account_byip(ip, mask, gname, &account);
	switch (ret) {
		case ERR_ACCOUNT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s(user already exist)\n", gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_UserExist;
			break;
			
		case ERR_MEMORY_ALLOC:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s(Kmalloc error)\n", gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_MallocFailed;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account(group %s not exist)\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameExist;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_adduser_bymac(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	int num;
	user_account_t* account;
	char *gname, *mac;
	u8 bmac[6];
	
	*length = 0;
	gname = parms->mStrings[0];
	mac = parms->mStrings[1];

	num = sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bmac[0], &bmac[1],
		   &bmac[2], &bmac[3], &bmac[4], &bmac[5]);

	if (num != 6) {
		snprintf(errmsg, errlen-1, "Failed to add account into group %s(Format of MAC error)\n", 
				gname);
		errmsg[errlen-1] = 0;
		return -1;
	}

	ret = get_account_bymac(bmac, gname, &account);
	switch (ret) {
		case ERR_ACCOUNT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s(exist)\n", gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_MEMORY_ALLOC:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s(Kmalloc error)\n", gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to add account(group %s not exist)\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to add account to group %s\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_revuser_byname(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *grp_name, *acc_name;

	*length = 0;
	grp_name = parms->mStrings[0];
	acc_name = parms->mStrings[1];
	
	ret = remove_account_byname(acc_name, grp_name);
	switch (ret) {
		case ERR_ACCOUNT_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account %s from group %s(account not exist)\n", 
					 acc_name, grp_name);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account %s (group %s not exist)\n",
					 acc_name, grp_name);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove account %s from group %s\n",
					 acc_name, grp_name);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_revuser_byip(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *gname;
	u32 ip, mask;
	
	*length = 0;
	gname = parms->mStrings[0];
	ip = parms->mIntegers[0];
	mask = parms->mIntegers[1];
	
	ret = remove_account_byip(ip, mask, gname);
	switch (ret) {
		case ERR_ACCOUNT_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account from group %s(account not exist)\n", 
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_UserNotExist;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account(group %s not exist)\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove account from group %s\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_revuser_bymac(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *gname, *mac;
	u8 bmac[6];
	int num;
	
	*length = 0;
	gname = parms->mStrings[0];
	mac = parms->mStrings[1];

	num = sscanf(mac, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &bmac[0], &bmac[1],
		   &bmac[2], &bmac[3], &bmac[4], &bmac[5]);

	if (num != 6) {
		snprintf(errmsg, errlen-1, "Failed to remove account from group %s(Format of MAC error)\n", 
				gname);
		errmsg[errlen-1] = 0;
		return -1;
	}

	ret = remove_account_bymac(bmac, gname);
	switch (ret) {
		case ERR_ACCOUNT_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account from group %s(account not exist)\n", 
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove account(group %s not exist)\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove account from group %s\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -1;
			break;
	}
	
	return ret;
}

static int 
inetctrl_group_addrule(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *gname, *aname, *tname;
	rule_assoc_t* timeassoc;
	
	*length = 0;
	gname = parms->mStrings[0];
	aname = parms->mStrings[1];
	tname = parms->mStrings[2];
	
	timeassoc = get_time_asso(gname, aname, tname, 0);
	if (timeassoc) {
		snprintf(errmsg, errlen-1, "Failed to add rule to group %s(exist)\n", gname);
		errmsg[errlen-1] = 0;
		return -eAosRc_NameNotExist;
	}
	
	timeassoc = get_time_asso(gname, aname, tname, 1);
	if (timeassoc == NULL) {
		snprintf(errmsg, errlen-1, "Failed to add rule to group %s(app %s or time %s not exist, or malloc error)\n", 
				 gname, aname, tname);
		errmsg[errlen-1] = 0;
		return -eAosRc_RuleExistAppNoExistTimeCtrNoExist;
	}
	
	return ret;
}

static int 
inetctrl_group_revrule(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char *gname, *aname, *tname;
	
	*length = 0;
	gname = parms->mStrings[0];
	aname = parms->mStrings[1];
	tname = parms->mStrings[2];
	
	ret = remove_time_asso(gname, aname, tname);
	switch (ret) {
		case ERR_NOT_MATCHED:
			snprintf(errmsg, errlen-1, "Failed to remove time from group %s(not matched)\n", 
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_RuleNotExist;
			break;
			
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove time(group %s not exist)\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;
			break;
			
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove time from group %s\n",
					 gname);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int 
inetctrl_default_policy(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;

	char* policy = parms->mStrings[0];
	*length = 0;

	if (strcmp(policy, "allow") == 0) {
		default_policy = ALLOW;
	} else if (strcmp(policy, "deny") == 0) {
		default_policy = BLOCK; 
	} else {
		snprintf(errmsg, errlen-1, "The parameter error%s\n", policy);
		errmsg[errlen-1] = 0;
		ret = -eAosRc_ArgumentErr;
	}

	return ret;
}

static int 
inetctrl_group_remove(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;
	char* name = parms->mStrings[0];
	*length = 0;
	
	switch (remove_group(name)) {
		case ERR_GROUP_NOT_EXIST:
			snprintf(errmsg, errlen-1, "Failed to remove group %s(not exist)\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_NameNotExist;			
			break;
		
		case ERR_OK:
			
			ret = 0;
			break;
			
		case ERR_FAILED:
		default:
			snprintf(errmsg, errlen-1, "Failed to remove group %s\n", name);
			errmsg[errlen-1] = 0;
			
			ret = -eAosRc_LackFailed;
			break;
	}
	
	return ret;
}

static int show_account_ofgroup(void* p, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	user_account_t* paccount = (user_account_t*)aos_list_entry((struct aos_list_head*)p, user_account_t, list);
	char tmp[256], ip[16], mask[16];
	static int account_index = 0;
	
	if (show == NULL)
		return -1; /* error happened */
	if (paccount == NULL)
		return 0; /* next */
	
	/* start show */
	if (FLAG_ISSET(show->flag, CHILD_LIST_NEW)) {
		FLAG_UNSET(show->flag, CHILD_LIST_NEW);
		account_index = 0;
	}
	account_index++;
	
	/* index, type and user*/
	if (paccount->account_type == TYPE_USER_NAME) {
		my_sprintf(YELLOW, tmp, "  %-*dNAME   %s\n", 8, account_index, paccount->account_name);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (paccount->account_type == TYPE_USER_IP){
		strcpy(ip, inet_ntoa(paccount->account_ip));
		strcpy(mask, inet_ntoa(paccount->account_mask));
		my_sprintf(YELLOW, tmp, "  %-*dIP     %s/%s\n", 
				   8, account_index, ip, mask);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (paccount->account_type == TYPE_USER_MAC){
		char tmp1[256];
#ifndef __KERNEL__
		sprintf(tmp1, "%.2hhX:%.2hhX:%.2hhX:%.2hhX:%.2hhX:%.2hhX",
				paccount->account_mac[0], paccount->account_mac[1], paccount->account_mac[2],
				paccount->account_mac[3], paccount->account_mac[4], paccount->account_mac[5]);
#else
		sprintf(tmp1, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
				paccount->account_mac[0], paccount->account_mac[1], paccount->account_mac[2],
				paccount->account_mac[3], paccount->account_mac[4], paccount->account_mac[5]);
#endif
		my_sprintf(YELLOW, tmp, "  %-*dMAC    %s\n", 8, account_index, tmp1);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else {
		my_sprintf(YELLOW, tmp, "%-*dUNKNOWN\n", 8, account_index);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	}
	
	return 0;
}


static int save_account_ofgroup(void* p, void* arg)
{
	save_elm_t* save = (save_elm_t*)arg;
	show_struct_t* show = (show_struct_t*)save->content;
	char* name = save->name;

	user_account_t* paccount = (user_account_t*)aos_list_entry((struct aos_list_head*)p, user_account_t, list);
	char tmp[256], ip[16], mask[16];
	
	if (show == NULL)
		return -1; /* error happened */
	if (paccount == NULL)
		return 0; /* next */
	
	/* index, type and user*/
	if (paccount->account_type == TYPE_USER_NAME) {
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl group adduser byname %s %s</Cmd>\n", name,
				paccount->account_name);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (paccount->account_type == TYPE_USER_IP){
		strcpy(ip, inet_ntoa(paccount->account_ip));
		strcpy(mask, inet_ntoa(paccount->account_mask));
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl group adduser byip %s %s %s</Cmd>\n", 
				name, ip, mask);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else if (paccount->account_type == TYPE_USER_MAC){
		char tmp1[256];
#ifndef __KERNEL__
		sprintf(tmp1, "%.2hhX-%.2hhX-%.2hhX-%.2hhX-%.2hhX-%.2hhX",
				paccount->account_mac[0], paccount->account_mac[1], paccount->account_mac[2],
				paccount->account_mac[3], paccount->account_mac[4], paccount->account_mac[5]);
#else
		sprintf(tmp1, "%.2X-%.2X-%.2X-%.2X-%.2X-%.2X",
				paccount->account_mac[0], paccount->account_mac[1], paccount->account_mac[2],
				paccount->account_mac[3], paccount->account_mac[4], paccount->account_mac[5]);
#endif
		my_sprintf(YELLOW, tmp, "<Cmd>inetctrl group adduser bymac %s %s", name, tmp1);
		aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	} else 
		return -1;
	
	return 0;
}


static int show_rule_ofgroup(void* p, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	rule_assoc_t* passo = (rule_assoc_t*)aos_list_entry((struct aos_list_head*)p, 
			rule_assoc_t, list);
	char tmp[256];
	static int asso_index = 0;
	
	if (show == NULL)
		return -1;
	if (passo == NULL)
		return 0;
	
	/* start show */
	if (FLAG_ISSET(show->flag, CHILD_LIST_NEW)) {
		FLAG_UNSET(show->flag, CHILD_LIST_NEW);
		asso_index = 0;
	}
	asso_index++;
	
	my_sprintf(YELLOW, tmp, "  %-*d%-*s%s\n", 8, asso_index, 10, 
			   (passo->assoc_app)?passo->assoc_app->app_name:"N/A",
			   (passo->assoc_time)?passo->assoc_time->rule_name:"N/A");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int save_rule_ofgroup(void* p, void* arg)
{
	save_elm_t* save = (save_elm_t*)arg;
	show_struct_t* show = (show_struct_t*)save->content;
	char* name= save->name;

	rule_assoc_t* passo = (rule_assoc_t*)aos_list_entry((struct aos_list_head*)p, 
			rule_assoc_t, list);
	char tmp[256];
	
	if (show == NULL)
		return -1;
	if (passo == NULL)
		return 0;
	
	my_sprintf(YELLOW, tmp, "<Cmd>inetctrl group addrule %s %s %s</Cmd>\n", name, 
			   (passo->assoc_app)?passo->assoc_app->app_name:"N/A",
			   (passo->assoc_time)?passo->assoc_time->rule_name:"N/A");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int show_one_group(void* grp, void* arg)
{
	show_struct_t* show = (show_struct_t*)arg;
	user_group_t* pgrp = (user_group_t*)grp;
	char tmp[256];
	static int grp_index = 0;
	
	if (show == NULL)
		return -1;
	
	if (pgrp == NULL) /* next */
		return 0;
	
	if (FLAG_ISSET(show->flag, PARENT_LIST_NEW)) {
		FLAG_UNSET(show->flag, PARENT_LIST_NEW);
		grp_index = 0;
	}
	grp_index++;
	
	/* start show */
	my_sprintf(YELLOW, tmp, "Group Index: %d\n", grp_index);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* name */
	my_sprintf(YELLOW, tmp, "Group Name: %s\n", pgrp->group_name);
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	/* Walk account */
	my_sprintf(YELLOW, tmp, "Account List:\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	my_sprintf(YELLOW, tmp, "  Index   Type   User\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	FLAG_SET(show->flag, CHILD_LIST_NEW);
	walk_sub_list(&pgrp->group_user_head, show_account_ofgroup, show);
	
	/* Walk rule */
	my_sprintf(YELLOW, tmp, "Rule List:\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	my_sprintf(YELLOW, tmp, "  Index   AppName   TimeName\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	FLAG_SET(show->flag, CHILD_LIST_NEW);
	walk_sub_list(&pgrp->group_rule_head, show_rule_ofgroup, show);
	
	/* tailor */
	my_sprintf(YELLOW, tmp, "\n");
	aosCheckAndCopy(show->pdata, (unsigned int*)show->pindex, show->maxlen, tmp, strlen(tmp));
	
	return 0;
}


static int save_one_group(void* grp, void* arg)
{
	user_group_t* pgrp = (user_group_t*)grp;
	save_elm_t save;
	char tmp[256];
	int ret;
	
	save.content = (show_struct_t*)arg;
	save.name = pgrp->group_name;
	if (save.content == NULL)
		return -1;
	
	if (pgrp == NULL) /* next */
		return 0;
	
	
	/* name */
	my_sprintf(YELLOW, tmp, "<Cmd>inetctrl group name %s</Cmd>\n", save.name);
	aosCheckAndCopy(save.content->pdata, (unsigned int*)save.content->pindex, save.content->maxlen, tmp, strlen(tmp));
	
	/* Walk account */
	ret = walk_sub_list(&pgrp->group_user_head, save_account_ofgroup, &save);
	if (ret)
		return ret;
	
	/* Walk rule */
	ret = walk_sub_list(&pgrp->group_rule_head, save_rule_ofgroup, &save);
	if (ret)
		return ret;
	
	return 0;
}


static int 
inetctrl_group_show_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0, index = 0;
	char* buff, tmp[256];
	show_struct_t show;
	
	memset(&show, 0, sizeof(show));
	
	buff = aosKernelApi_getBuff(data);
	show.pindex = &index;
	show.pdata = buff;
	show.maxlen = *length;
	
	/* print the header */
	my_sprintf(BLUE, tmp, "------------Begin Group List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	FLAG_SET(show.flag, PARENT_LIST_NEW);
	ret = walk_groups_list(show_one_group, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to show group(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}
	
	/* print the end */
	my_sprintf(BLUE, tmp, "------------End Group List---------------\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	*length = index;	
	return 0;
}


//add by xyb
static int 
inetctrl_group_clear_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret;
	*length = 0;

	ret = clear_group_rule();
	if (ret) {
		snprintf(errmsg, errlen-1, "Failed to clear all group\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_LackFailed;
	}

	return ret;
}


static int 
inetctrl_save_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{	
	int ret = 0, index = 0;
	char* buff, tmp[256];
	show_struct_t show;

	memset(&show, 0, sizeof(show));
	
	buff = aosKernelApi_getBuff(data);
	show.pindex= &index;
	show.pdata = buff;
	show.maxlen = *length;
	
	/* save app config */
	ret = walk_apps_list(save_one_app, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to save app(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}

	/* save timerule config */
	ret = walk_timerules_list(save_one_timerule, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to save timerule(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}
	
	/* save group config */
	ret = walk_groups_list(save_one_group, &show);
	if (ret < 0) {
		snprintf(errmsg, errlen-1, "Failed to save group(small buffer size)\n");
		errmsg[errlen-1] = 0;
		*length = 0;
		return -eAosRc_RetriveBufLenTooShort;
	}

	/* save log level */
	if (log_level == 0)
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl log level no</Cmd>\n");
	else if (log_level == 1)
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl log level block</Cmd>\n");
	else 
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl log level all</Cmd>\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));

	/* save default policy*/
	if (default_policy == ALLOW)
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl default policy allow</Cmd>\n");
	else
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl default policy deny</Cmd>\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));

	/* save onoff */
	if (inetctrl_switch == 1)
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl on</Cmd>\n");
	else
		my_sprintf(BLUE, tmp, "<Cmd>inetctrl off</Cmd>\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	*length = index;	

	return ret;
}


static int 
inetctrl_clear_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{	
	int ret;
	*length = 0;

	/* clear log level, no log*/
	log_level = 0;

	/* clear default policy, allow*/
	default_policy = ALLOW;

	/* clear inetctrl status, off*/
	ret = inetctrl_onoff(0);
	switch (ret) {
	case -2:
	case -3:
	case 0:
		break;
	case -1:
	default:
		snprintf(errmsg, errlen-1, "Failed to turn off IAC\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_FailedToChangeStatus;
	}

	/* clear group config */
	ret = clear_group_rule();
	if (ret) {
		snprintf(errmsg, errlen-1, "Failed to clear all apps\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_LackFailed;
	}

	/* clear app config */
	ret = clear_app_rule();
	if (ret) {
		snprintf(errmsg, errlen-1, "Failed to clear all apps\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_LackFailed;
	}

	/* clear time rule config */
	ret = clear_time_rule();
	if (ret) {
		snprintf(errmsg, errlen-1, "Failed to clear all apps\n");
		errmsg[errlen-1] = 0;
		return -eAosRc_LackFailed;
	}

	return ret;
}
	
static int 
inetctrl_log_level(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{	
	int ret = 0;

	char* level;
	*length = 0;
	level = parms->mStrings[0];
	
	if (strcmp(level, "no") == 0)
		log_level = 0;
	else if (strcmp(level, "block") == 0)
		log_level = 1;
	else if (strcmp(level, "all") == 0)
		log_level = 2;
	else {
			snprintf(errmsg, errlen-1, "Failed to input log level\n");
			errmsg[errlen-1] = 0;
			return -eAosRc_ArgumentErr;
	}

	return ret;
}


static int 
inetctrl_on(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;

	*length = 0;
	
	ret = inetctrl_onoff(1);
	switch (ret) {
	case -2:
	case -3:
		snprintf(errmsg, errlen-1, "IAC is on\n");
		errmsg[errlen-1] = 0;

		return -eAosRc_AlreadyOn;
	case 0:
		break;
	case -1:
	default:
		snprintf(errmsg, errlen-1, "Failed to turn on IAC\n");
		errmsg[errlen-1] = 0;

		return -eAosRc_FailedToChangeStatus;
	}
	
	return ret;
}

static int 
inetctrl_off(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0;

	*length = 0;
	
	ret = inetctrl_onoff(0);
	switch (ret) {
	case -2:
	case -3:
		snprintf(errmsg, errlen-1, "IAC is off\n");
		errmsg[errlen-1] = 0;

		return -eAosRc_AlreadyOff;
	case 0:
		break;
	case -1:
	default:
		snprintf(errmsg, errlen-1, "Failed to turn off IAC\n");
		errmsg[errlen-1] = 0;

		return -eAosRc_FailedToChangeStatus;
	}

	return ret;
}

static int 
inetctrl_show_config(char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int ret = 0, index = 0;
	char* buff, tmp[256];
	
	buff = aosKernelApi_getBuff(data);

	/*show status */
	if (inetctrl_switch == 1)
		strcpy(tmp, "inetctrl on\n");
	else
		strcpy(tmp, "inetctrl off\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	/* show log level */
	if (log_level == 0)
		strcpy(tmp, "inetctrl log level no\n");
	else if (log_level == 1)
		strcpy(tmp, "inetctrl log level block\n");
	else 
		strcpy(tmp, "inetctrl log level all\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	/* show default policy */
	if (default_policy == ALLOW)
		strcpy(tmp, "inetctrl default policy allow\n");
	else 
		strcpy(tmp, "inetctrl default policy deny\n");
	aosCheckAndCopy(buff, (unsigned int*)&index, *length, tmp, strlen(tmp));
	
	*length = index;
	return ret;
}

int reg_inetctrl_cli(void)
{
	int ret = 0;
	
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_addflow_ip", inetctrl_app_addflow_ip);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_addflow_url", inetctrl_app_addflow_url);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_name", inetctrl_app_name);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_remove", inetctrl_app_remove);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_revflow_ip", inetctrl_app_revflow_ip);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_revflow_url", inetctrl_app_revflow_url);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_show_config", inetctrl_app_show_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_app_clear_config", inetctrl_app_clear_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_addrule", inetctrl_group_addrule);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_adduser_byip", inetctrl_group_adduser_byip);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_adduser_bymac", inetctrl_group_adduser_bymac);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_adduser_byname", inetctrl_group_adduser_byname);
	ret |= OmnKernelApi_addCliCmd("inetctrl_default_policy", inetctrl_default_policy);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_name", inetctrl_group_name);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_remove", inetctrl_group_remove);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_revrule", inetctrl_group_revrule);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_revuser_byip", inetctrl_group_revuser_byip);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_revuser_byname", inetctrl_group_revuser_byname);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_revuser_bymac", inetctrl_group_revuser_bymac);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_show_config", inetctrl_group_show_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_group_clear_config", inetctrl_group_clear_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_addrule_online", inetctrl_timerule_addrule_online);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_addrule_time", inetctrl_timerule_addrule_time);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_name", inetctrl_timerule_name);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_remove", inetctrl_timerule_remove);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_revrule_online", inetctrl_timerule_revrule_online);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_revrule_time", inetctrl_timerule_revrule_time);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_show_config", inetctrl_timerule_show_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_timerule_clear_config", inetctrl_timerule_clear_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_save_config", inetctrl_save_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_clear_config", inetctrl_clear_config);
	ret |= OmnKernelApi_addCliCmd("inetctrl_log_level", inetctrl_log_level);
	ret |= OmnKernelApi_addCliCmd("inetctrl_off", inetctrl_off);
	ret |= OmnKernelApi_addCliCmd("inetctrl_on", inetctrl_on);
	ret |= OmnKernelApi_addCliCmd("inetctrl_show_config", inetctrl_show_config);

	return ret;
}

