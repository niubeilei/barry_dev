/*
 * Webwall module CLI implementation
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_cli.cpp
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#include "ParentCtrl/webwall_cli.h"
#include "ParentCtrl/webwall_impl.h"
#include "ParentCtrl/webwall_common.h"
#include "CliUtil/CliUtil.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aosUtil/List.h"
#include "KernelConfMgr/Global.h"
#include "Util/OmnNew.h"
#include "ParentCtrl/webwall_log.h"
#include "ParentCtrl/webwall_util.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef WEBWALL_DEUBG
#include <syslog.h>
#endif

#define AOS_WEBWALL_CLI_NAME_LEN 128
typedef struct ww_cli_command
{
	char name[AOS_WEBWALL_CLI_NAME_LEN];
	int (*invoke)(char*, unsigned int*, struct aosUserLandApiParms*, char*, const int);
} ww_cli_command_t;

static ww_cli_command_t clis[] = {
	{"webwall list create", ww_cli_create_category_list},
	{"webwall list remove", ww_cli_remove_category_list},
	{"webwall list time", ww_cli_set_time},
	{"webwall list remove time", ww_cli_remove_time},
	{"webwall list alias", ww_cli_create_alias},
	{"webwall list remove alias", ww_cli_remove_alias},
	{"webwall list add entry", ww_cli_create_single_entry},
	{"webwall list remove entry", ww_cli_remove_single_entry},
	{"webwall list include entry file", ww_cli_create_include_entry_file},
	{"webwall list remove entry file", ww_cli_remove_include_entry_file},
	{"webwall list show config", ww_cli_show_category_list},
	{"webwall user add name", ww_cli_create_named_user},
	{"webwall user remove name", ww_cli_remove_named_user},
	{"webwall user show name", ww_cli_show_named_user},
	{"webwall user remove all name", ww_cli_remove_all_named_users},
	{"webwall user add ips", ww_cli_create_ip_user},
	{"webwall user remove ips", ww_cli_remove_ip_user},
	{"webwall user show ips", ww_cli_show_ip_user},
	{"webwall user remove all ips", ww_cli_remove_all_ip_users},
	{"webwall group policy mode", ww_cli_set_filter_group_mode},
	{"webwall group policy name", ww_cli_set_filter_group_name},
	{"webwall group policy add list", ww_cli_map_category_list_to_filter_group},
	{"webwall group policy remove list", ww_cli_unmap_category_list_of_filter_group},
	{"webwall group policy remove all list", ww_cli_unmap_all_category_list_of_filter_group},
	{"webwall group policy show config", ww_cli_show_category_list_of_filter_group},
	{"webwall web denied report level", ww_cli_set_reporting_level},
	{"webwall language", ww_cli_set_language},
	{"webwall log level", ww_cli_set_log_level},
	{"webwall log exception hits", ww_cli_set_log_exception_hits},
	{"webwall log format", ww_cli_set_log_file_format},
	{"webwall log max item length", ww_cli_set_max_log_item_len},
	{"webwall log anonymize", ww_cli_set_anoymous_log},
	{"webwall address", ww_cli_set_ip_and_port_of_webwall},
	{"webwall proxy address", ww_cli_set_ip_and_port_of_proxy},
	{"webwall access denied address", ww_cli_set_access_denied_addr},
	{"webwall group counts", ww_cli_set_filter_group_len},
	{"webwall show weighted found", ww_cli_set_show_weighted_found},
	{"webwall weighted phrase mode", ww_cli_set_weighted_phrase_mode},
	{"webwall add auth plugins", ww_cli_create_auth_plugins},
	{"webwall remove auth plugins", ww_cli_remove_auth_plugin},
	{"webwall reverse address lookups", ww_cli_reverse_addr_lookups},
	{"webwall reverse clientip lookups", ww_cli_reverse_clientip_lookups},
	{"webwall reverse client hostname", ww_cli_reverse_client_hostname},
	{"proxy address", ww_cli_set_proxy_addr},
	{"proxy status", ww_cli_set_proxy_action},
	{"proxy show config", ww_cli_show_proxy_config},
	{"webwall status", ww_cli_set_webwall_action},
	{"webwall group create", ww_cli_create_group},
	{"webwall group remove", ww_cli_remove_group},
	{"webwall group show", ww_cli_show_group},
	{"webwall group naughtnesslimit", ww_cli_set_naughtyness_limit},
	{"webwall show log", ww_cli_show_log},
	{"webwall search log", ww_cli_search_log},
	{"webwall clear log", ww_cli_clear_log},
	{"webwall show stat", ww_cli_show_stat},
	{"webwall show config", ww_cli_show_config},
	{"webwall av sockfile", ww_cli_set_av_sock},
	{"webwall av add plugin", ww_cli_create_av_engine},
	{"webwall av remove plugin", ww_cli_remove_av_engine},
	{"webwall list clone", ww_cli_clone_list},
	{"webwall list show all", ww_cli_show_all_list},
	{"webwall list file import", ww_cli_import_file_list}
};
#define CLI_LEN (int)(sizeof(clis)/sizeof(ww_cli_command_t))

int validate_switch(char* name)
{
	if ((strcmp(name, "on") == 0) || (strcmp(name, "off") == 0))
		return 0;
	
	return -eAosRc_WWInvalidSwitch;
}

int validate_ip_and_port(char* ip, int port)
{	
	if (port < 0 || port > 65535) {
		return -eAosRc_WWPortFormat;
	}
	
	if (inet_addr(ip) == INADDR_NONE) {
		return -eAosRc_WWIPFormat;
	}
	
	return 0;
}

static int validate_time(char* time, struct tm* tm_time)
{	
	int year;
	int month;
	int day;
	int hour;
	int minute;

#ifdef WEBWALL_DEUBG
	syslog(LOG_INFO, "validate_time: time is %s\n", time);
#endif

	if (sscanf(time, "%d.%d.%d %d:%d", &year, &month, &day, &hour, &minute) != 5) {
        return -1;
	}
	
	year -= 1900;
	
	if ((year < 0 || year > 60000)
	|| (month < 0 || month > 11)
	|| (day < 1 || day > 31)
	|| (hour < 0 || hour > 23)
	|| (minute < 0 || minute > 59)) {
		return -1;
	}
	tm_time->tm_year = year;
	tm_time->tm_mon = month;
	tm_time->tm_mday = day;
	tm_time->tm_hour = hour;
	tm_time->tm_min = minute;
#ifdef WEBWALL_DEUBG
	syslog(LOG_INFO, "validate_time: pass time is %s\n", time);
#endif
	return 0;
}

/*
 * webwall on by firewall nat rule
 */
static void firewall_action(int type)
{
	OmnString dev;
	OmnString ip;
	OmnString dip;
	OmnString port;
	OmnString dport;
	OmnString rslt;

	dev = "br0";
	port = "80";
	dport << global.filter_port;
	ip << get_ip_address();
	dip << ip;
#ifdef WEBWALL_DEUBG
	syslog(LOG_INFO, "dev is %s, port is %s, dport is %s, ip is %s, dip is %s",
			dev.data(), port.data(), dport.data(), ip.data(), dip.data());
#endif
	switch (type) {
		case T_WEBWALL_START:
			webwallDnat(dev, atoi(port), atoi(dport), inet_addr(ip), inet_addr(dip), rslt);
			break;
		case T_WEBWALL_STOP:
			webwallDelDnat(dev, atoi(port), atoi(dport), inet_addr(ip), inet_addr(dip), rslt);
			break;
	}
}

int validate_group_policy()
{
	int type_list[] = {T_WW_BANNED_PHRASE_LIST, T_WW_WEIGHTED_PHRASE_LIST,
						T_WW_EXCEPTION_PHRASE_LIST, T_WW_BANNED_SITE_LIST,
						T_WW_GREY_SITE_LIST, T_WW_EXCEPTION_SITE_LIST,
						T_WW_BANNED_URL_LIST, T_WW_GREY_URL_LIST,
						T_WW_EXCEPTION_URL_LIST, T_WW_EXCEPTION_REGEXP_URL_LIST,
						T_WW_BANNED_REGEXP_URL_LIST, T_WW_PICS_FILE,
						T_WW_CONTENT_REGEXP_LIST, T_WW_REGEXP_URL_LIST,
						T_WW_BANNED_EXTENSION_LIST, T_WW_BANNED_MIME_TYPE_LIST,
						T_WW_EXCEPTION_FILE_SITE_LIST};
	
	int len; 
	int i;
	int cout;
	ww_filter_group_policy_t* policy;
	ww_filter_group_policy_t* tmp_policy;
	ww_filter_group_list_t* group_list;
	ww_filter_group_list_t* tmp_group_list;
	
	len = (int)(sizeof(type_list) / sizeof(type_list[0]));
	
	aos_list_for_each_entry_safe(policy, tmp_policy, &(policy_head), list) {
		for (i = 0; i < len; i++) {
			cout = 0;
			aos_list_for_each_entry_safe(group_list, tmp_group_list, &(policy->category_list_head), list) {
				if (type_list[i] == group_list->category_list->list_type) {
					cout++;
				}
			}
			if (cout > 1)
				return -eAosRc_WWError;
			if (cout < 1)
				return type_list[i];
		}
	}
	
	return 0;
}
/*
 * create category list
 */
int ww_cli_create_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_type = parms->mStrings[0];
	char* list_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int is_system = 0;
	
	if (parms->mNumIntegers == 1 
		&& parms->mIntegers[0] == 1)
		is_system = 1;
	
	if (is_valid_file_name(list_name) < 0) {
		snprintf(errmsg, errlen - 1, "list name '%s' is invalid\n", list_name);
		errmsg[errlen - 1] = 0;
		return -1;
	}

	ret = ww_create_category_list(list_type, list_name, is_system);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWUnsupportListType:
				sprintf(buff, "The list type '%s' is unsupported\n", list_type);
				break;
			case -eAosRc_WWExistListName:
				sprintf(buff, "The list name '%s' is already existed\n", list_name);
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}

/*
 * remove category list
 */
int ww_cli_remove_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
 	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_remove_category_list(list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWInUse:
				sprintf(buff, "The list name '%s' is in use\n", list_name);
				break;
			case -eAosRc_WWCanNotRemove:
				sprintf(buff, "The List '%s' is system list, so can not remove\n", list_name);
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}

/*
 * set time of category list
 */
int ww_cli_set_time(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* start_time = parms->mStrings[1];
	char* end_time = parms->mStrings[2];
	char* days = parms->mStrings[3];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	ret = ww_set_time(list_name, start_time, end_time, days);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed or time format error\n", list_name);
				break;
			case -eAosRc_WWTimeFormat:
				sprintf(buff, "The time format error\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}

/*
 * remove time of category list
 */
int ww_cli_remove_time(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	ret = ww_remove_time(list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWNoTime:
				sprintf(buff, "time is not existed\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}

/*
 * create alias of category list
 */
int ww_cli_create_alias(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* alias_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_create_alias(list_name, alias_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "The list name '%s' is not existed\n", list_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	return 0;
}


/*
 * remove alias of category list
 */
int ww_cli_remove_alias(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_remove_alias(list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWNoexistAaliasName:
				sprintf(buff, "The alias name is not existed\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}


/*
 * create single entry
 */
int ww_cli_create_single_entry(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* entry = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
		
	ret = ww_create_single_entry(list_name, entry);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWExistEntry:
				sprintf(buff, "The entry name '%s' is already existed\n", entry);
				break;
			case -eAosRc_WWIPFormat:
				sprintf(buff, "IP format error\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}


/*
 * remove single entry
 */
int ww_cli_remove_single_entry(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* entry = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_remove_single_entry(list_name, entry);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWNoexistEntry:
				sprintf(buff, "The entry '%s' is not existed\n", entry);
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}


/*
 * create include entry file
 */
int ww_cli_create_include_entry_file(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* entry_type = parms->mStrings[1];
	char* entry_list_name = parms->mStrings[2];
	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_create_include_entry_file(list_name, entry_type, entry_list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWUnsupportEntryType:
				sprintf(buff, "The entry type '%s' is unsupported\n", entry_type);
				break;
			case -eAosRc_WWExistIncludeEntryFile:
				sprintf(buff, "the include entry file '%s' is already existed\n", entry_list_name);
				break;
			case -eAosRc_WWUnmatchIncludeEntry:
				sprintf(buff, "entry type is not supported by list type\n");
				break;
			case -eAosRc_WWInvalidIncludeEntryFile:
				sprintf(buff, "invalid entry file\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}


/*
 * remove include entry file
 */
int ww_cli_remove_include_entry_file(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	char* entry_list_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];

	ret = ww_remove_include_entry_file(list_name, entry_list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "The list name '%s' is not existed\n", list_name);
				break;
			case -eAosRc_WWNoexistIncludeEntryFile:
				sprintf(buff, "the include entry file '%s' is not existed\n", entry_list_name);
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return 0;
}

/*
 * show category list
 */
int ww_cli_show_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* list_name = parms->mStrings[0];
	ww_category_list_t* node;
	char buff[MAX_BUFF_LEN];
	ww_entry_t* entry;
	ww_entry_t* tmp;
	char* dst;
	unsigned int index = 0;
	
	dst = CliUtil_getBuff(data);
	node = ww_get_category_list(list_name);
	memset(buff, 0, MAX_BUFF_LEN);
	if (node == NULL) {
		sprintf(buff, "list name %s is not existed\n", list_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWNoexistListName;
	}
	
	/* list type name */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "       List Type Name: %s\n", get_list_type_name(node->list_type));
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* time */
	if (node->tc != NULL) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "                 Time: %d:%d %d:%d %s\n", 
				node->tc->start_hour, node->tc->start_minute,
				node->tc->end_hour, node->tc->end_minute,
				node->tc->days);
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	}
	
	/* category alias */
	if (node->category_alias != NULL) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "           Alias Name: %s\n", node->category_alias);
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	}
	/* entry */
	if (!aos_list_empty_careful(&node->entry_list_head)) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "           Entry List:\n");
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		aos_list_for_each_entry_safe(entry, tmp, &node->entry_list_head, list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "                       %s\n", entry->data);
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	/* include */
	if (!aos_list_empty_careful(&node->include_list_head)) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "         Include List:\n");
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		aos_list_for_each_entry_safe(entry, tmp, &node->include_list_head, list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "                       %s\n", entry->data);
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	/* squidguard include */
	if (!aos_list_empty_careful(&node->guard_entry_file_list_head)) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Specific Include List:\n");
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		aos_list_for_each_entry_safe(entry, tmp, &node->guard_entry_file_list_head, list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "                       %s\n", entry->data);
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	
	*length = index;
	
	return 0;
}


/*
 * create named user
 */
int ww_cli_create_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* user_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_create_named_user(group_id, user_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
	 	sprintf(buff, "User Name %s is  existed\n", user_name);	
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	*length = 0;
	
	return ret;
}


/*
 * remove named user
 */
int ww_cli_remove_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* user_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_remove_named_user(group_id, user_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "User Name %s is not existed\n", user_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}

/*
 * show named user
 */
int ww_cli_show_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	ww_filter_name_based_user_t* node;
	ww_filter_name_based_user_t* tmp;
	char buff[MAX_BUFF_LEN];
	int gid;
	char* group_id = parms->mStrings[0];
	char* dst;
	unsigned int index = 0;
	int ret;

	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	dst = CliUtil_getBuff(data);
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Group Name\t\t\tUser Name	\n----------------------------------------\n");
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	aos_list_for_each_entry_safe(node, tmp, &(filter_name_based_user_head), list) {
		if (node->group_id == gid) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "group%d\t\t\t%s\n", node->group_id, node->user_name);
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	
	*length = index;
	
	return 0;
}


/*
 * remove all named users
 */
int ww_cli_remove_all_named_users(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_remove_all_named_users(group_id);
	*length = 0;
	
	return ret;
}


/*
 * create ip based user
 */
int ww_cli_create_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* ip = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	if ((ret = validate_ip_and_ip_range(ip)) < 0) {
	    memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "IP Address %s is invalid\n", ip);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_create_ip_user(group_id, ip);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "IP Address %s is already existed\n", ip);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}


/*
 * remove ip based user
 */
int ww_cli_remove_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* ip = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_remove_ip_user(group_id, ip);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "IP Address %s is not existed\n", ip);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}


/*
 * show ip based user
 */
int ww_cli_show_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	ww_filter_ip_based_user_t* node;
	ww_filter_ip_based_user_t* tmp;
	char buff[MAX_BUFF_LEN];
	int gid;
	char* group_id = parms->mStrings[0];
	char* dst;
	unsigned int index = 0;
	int ret;

	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	dst = CliUtil_getBuff(data);
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Group Name\t\t\tIP	\n----------------------------------------\n");
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	aos_list_for_each_entry_safe(node, tmp, &(filter_ip_based_user_head), list) {
		if (node->group_id == gid) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "group%d\t\t\t%s\n", node->group_id, node->ip);
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	
	*length = index;
	
	return 0;
}


/*
 * remove all ip based user
 */
int ww_cli_remove_all_ip_users(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_remove_all_ip_users(group_id);
	*length = 0;
	
	return ret;
}


/*
 * set filter group mode
 */
int ww_cli_set_filter_group_mode(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* mode = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_set_filter_group_mode(group_id, mode);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWUnsupportGroupModeType:
				sprintf(buff, "the group mode name %s is unsupported\n", mode);	
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}


/*
 * set filter group name
 */
int ww_cli_set_filter_group_name(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* group_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_set_filter_group_name(group_id, group_name);
	*length = 0;
	
	return ret;
}


/*
 * map category list to filter group
 */
int ww_cli_map_category_list_to_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	char* list_name = parms->mStrings[1];
	int ret;
	char buff[MAX_BUFF_LEN];
	int gid;
	
	gid = ww_get_group_id(group_id);
	if (validate_group_id(gid) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}

	ret = ww_map_category_list_to_filter_group(group_id, list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistListName:
				sprintf(buff, "List Name %s is not existed\n", list_name);
				break;
			case -eAosRc_WWExistListName:
				sprintf(buff, "Same List Type is already existed, please remove and try again\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	*length = 0;
	
	return ret;
}


/*
 * unmap category list of filter group
 */
int ww_cli_unmap_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	//char* group_id = parms->mStrings[0];
	//char* list_name = parms->mStrings[1];
	//int ret = 0;
	char buff[MAX_BUFF_LEN];
	//int gid;
	
	/* the mechansim is modified, the interface is obsolete */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "the CLI is obsolete\n");
	strncpy(errmsg, buff, errlen - 1);
	errmsg[errlen - 1] = 0;
	
	return -1;
	/*	
	gid = ww_get_group_id(group_id);
	if (validate_group_id(gid) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}

	ret = ww_unmap_category_list_of_filter_group(group_id, list_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "List Name %s is not existed\n", list_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	*length = 0;
	
	return ret;
	*/
}


/*
 * unmap all category list of filter group
 */
int ww_cli_unmap_all_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	//char* group_id = parms->mStrings[0];
	//char* list_type = parms->mStrings[1];
	//int ret;
	char buff[MAX_BUFF_LEN];
	//int gid;
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "The CLI is obsolete\n");
	strncpy(errmsg, buff, errlen - 1);
	errmsg[errlen - 1] = 0;
	return -1;

	/*	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}

	ret = ww_unmap_all_category_list_of_filter_group(group_id, list_type);
	*length = 0;
	
	return ret;
	*/
}


/*
 * show category list of filter group
 */
int ww_cli_show_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int gid;
	unsigned int index = 0;
	char* dst;
	char buff[MAX_BUFF_LEN];
	ww_filter_group_policy_t* policy;
	ww_filter_group_list_t* node;
	ww_filter_group_list_t* tmp;
	int ret;

	dst = CliUtil_getBuff(data);
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the group %s is not valid.\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	policy = ww_get_filter_group_policy(gid);
	if (policy == NULL) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the group %s is not exist.\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	
	/* group mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "       Group Mode: %s\n", get_group_mode_name(policy->group_mode));
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* group name */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "       Group Name: %s\n", policy->group_name);
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* naughtyness limit */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Naughtyness Limit: %d\n", policy->naughtyness_limit);
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* list */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "             List:\n");
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	aos_list_for_each_entry_safe(node, tmp, &(policy->category_list_head), list) {
		if (node->category_list != NULL) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "                  %s(List Type: %s)\n", node->category_list->path, get_list_type_name(node->category_list->list_type));
			CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
		}
	}
	
	*length = index;
	
	return 0;
}


/*
 * set reporting level of web access denies
 */
int ww_cli_set_reporting_level(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int reporting_level = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
#ifdef WEBWALL_DEUBG
	printf("cli::set reporting level: %d\n", reporting_level);
#endif
	if (reporting_level < -1 || reporting_level > 3) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, " report level %d is invalid\n", reporting_level);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidReportingLevel;
	}
	
	ret = ww_set_reporting_level(reporting_level);
	*length = 0;
	
	return ret;
}


/*
 * set language
 */
int ww_cli_set_language(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* language = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_language(language)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "language '%s' is invalid\n", language);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_language(language);
	*length = 0;
	
	return ret;
}


/*
 * set log level
 */
int ww_cli_set_log_level(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int log_level = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (log_level < 0 || log_level > 3) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "log level %d is invalid\n", log_level);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidLogLevel;
	}
	
	ret = ww_set_log_level(log_level);
	*length = 0;
	
	return ret;
}


/*
 * set log exception hits
 */
int ww_cli_set_log_exception_hits(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* log_exception_hits = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(log_exception_hits)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, " log exception hits '%s' is invalid\n", log_exception_hits);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_log_exception_hits(log_exception_hits);
	*length = 0;
	
	return ret;
}


/*
 * set log file format
 */
int ww_cli_set_log_file_format(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int log_file_format = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (log_file_format < 1 || log_file_format > 4) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "log file format %d is invalid\n", log_file_format);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidLogFileFormat;
	}
	
	ret = ww_set_log_file_format(log_file_format);
	*length = 0;
	
	return ret;
}


/*
 * set maximum log item length
 */
int ww_cli_set_max_log_item_len(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int max_log_item_len = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (max_log_item_len < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the max log item length is more than zero\n");
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidLogItemLength;
	}
	
	ret = ww_set_max_log_item_len(max_log_item_len);
	*length = 0;
	
	return ret;
}


/*
 * set anoymous log
 */
int ww_cli_set_anoymous_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* anoymous_log = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(anoymous_log)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, " anoymous log '%s' is invalid\n", anoymous_log);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_anoymous_log(anoymous_log);
	*length = 0;
	
	return ret;
}


/*
 * set ip and port of webwall
 */
int ww_cli_set_ip_and_port_of_webwall(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* ip = parms->mStrings[0];
	int port = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_ip_and_port(ip, port)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "ip address '%s' or port %d is invalid\n", ip, port);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_ip_and_port_of_webwall(ip, port);
	*length = 0;
	
	return ret;
}


/*
 * set ip and port of proxy
 */
int ww_cli_set_ip_and_port_of_proxy(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* ip = parms->mStrings[0];
	int port = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (validate_ip_and_port(ip, port) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "ip address '%s' or port %d is invalid\n", ip, port);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	
	ret = ww_set_ip_and_port_of_proxy(ip, port);
	*length = 0;
	
	return ret;
}


/*
 * set the access denied address
 */
int ww_cli_set_access_denied_addr(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* addr = parms->mStrings[0];
	int ret;
	
	ret = ww_set_access_denied_addr(addr);
	*length = 0;
	
	return ret;
}


/*
 * set filter group length
 */
int ww_cli_set_filter_group_len(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int len = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (len < 1 || len > WEBWALL_MAX_GROUP) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "filter group length is more than 0 or less than 10\n");
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidGroupLength;
	}
	
	ret = ww_set_filter_group_len(len);
	*length = 0;
	
	return ret;
}


/*
 * set show weighted found
 */
int ww_cli_set_show_weighted_found(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* show_weighted_found = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(show_weighted_found)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, " show weighted found '%s' is invalid\n", show_weighted_found);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_show_weighted_found(show_weighted_found);
	*length = 0;
	
	return ret;
}


/*
 * set weighted phrase mode
 */
int ww_cli_set_weighted_phrase_mode(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int weighted_phrase_mode = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if (weighted_phrase_mode < 0 || weighted_phrase_mode > 2) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "weighted phrase mode is more than 0 but less than 2\n");
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidWeightedPhraseMode;
	}
	
	ret = ww_set_weighted_phrase_mode(weighted_phrase_mode);
	*length = 0;
	
	return ret;
}


/*
 * create auth plugins
 */
int ww_cli_create_auth_plugins(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* plugin_name = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ( (strcmp(plugin_name, "proxy-basic") == 0)
			|| (strcmp(plugin_name, "proxy-ntlm") == 0)
			|| (strcmp(plugin_name, "ident") == 0) 
			|| (strcmp(plugin_name, "ip") == 0)) {
	} else {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the plugin name %s is invalid.\n", plugin_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidPlugin;
	
	}	
	ret = ww_create_auth_plugins(plugin_name);
	
	if (ret < 0) {
		
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the plugin %s is existed.\n", plugin_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWExistPlugin;
	}

	*length = 0;
	
	return 0;
}


/*
 * remove auth plugin
 */
int ww_cli_remove_auth_plugin(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* plugin_name = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ( (strcmp(plugin_name, "proxy-basic") == 0)
			|| (strcmp(plugin_name, "proxy-ntlm") == 0)
			|| (strcmp(plugin_name, "ident") == 0) 
			|| (strcmp(plugin_name, "ip") == 0)) {
	} else {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the plugin name %s is invalid.\n", plugin_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidPlugin;
	
	}

	ret = ww_remove_auth_plugin(plugin_name);
	if (ret < 0) {
		sprintf(buff, "the plugin %s is not existed.\n", plugin_name);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWNoexistPlugin;
	}
			
	*length = 0;
	
	return ret;
}


/*
 * set the reverse address lookups
 */
int ww_cli_reverse_addr_lookups(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* reverse_addr_lookups = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(reverse_addr_lookups)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "reverse address lookups '%s' is invalid\n", reverse_addr_lookups);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_reverse_addr_lookups(reverse_addr_lookups);
	*length = 0;
	
	return ret;
}


/*
 * set the reverse clientip lookups
 */
int ww_cli_reverse_clientip_lookups(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* reverse_clientip_lookups = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(reverse_clientip_lookups)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "reverse clientip lookups '%s' is invalid\n", reverse_clientip_lookups);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_reverse_clientip_lookups(reverse_clientip_lookups);
	*length = 0;
	
	return ret;
}


/*
 * set the reverse client hostname
 */
int ww_cli_reverse_client_hostname(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* reverse_client_hostname = parms->mStrings[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_switch(reverse_client_hostname)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "reverse client hostname '%s' is invalid\n", reverse_client_hostname);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_reverse_client_hostname(reverse_client_hostname);
	*length = 0;
	
	return ret;
}


/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 */
int ww_cli_set_proxy_addr(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* ip = parms->mStrings[0];
	int port = parms->mIntegers[0];
	int ret;
	char buff[MAX_BUFF_LEN];
	
	if ((ret = validate_ip_and_port(ip, port)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "ip address '%s' or port %d is invalid\n", ip, port);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_set_proxy_addr(ip, port);
	*length = 0;
	
	return ret;
}


/*
 * set proxy action
 */
int ww_cli_set_proxy_action(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* action = parms->mStrings[0];
	int ret;
	int type;
	char buff[MAX_BUFF_LEN];
	char cmd[MAX_BUFF_LEN];
	int previous_status;
	int is_running;
	
	type = validate_action(action);
	if (type == -eAosRc_WWUnsupportActionType) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the action %s is unsupported.\n", action);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return type;
	}
	/* save memory to file before starting */
	previous_status = tiny_proxy.status;
	memset(cmd, 0, MAX_BUFF_LEN);
	switch (type) {
		case T_WEBWALL_START:
			if (tiny_proxy.status == T_WEBWALL_START) {
				strcpy(errmsg, "tinyproxy is already running\n");
				errmsg[errlen - 1] = 0;
				return -eAosRc_WWAlreadyRun;
			}
			ww_save_proxy_data_to_file();
			sprintf(cmd, "killall -9 -q %s", TINY_PROXY_PREFIX);
			system(cmd);
			
			memset(cmd, 0, MAX_BUFF_LEN);
			sprintf(cmd, "%s", TINY_PROXY_EXE_PATH);
			tiny_proxy.status = T_WEBWALL_START;	
			break;
		case T_WEBWALL_STOP:
			if (tiny_proxy.status == T_WEBWALL_STOP) {
				strcpy(errmsg, "tinyproxy is already stopping\n");
				errmsg[errlen - 1] = 0;
				return -eAosRc_WWAlreadyStop;
			}
			sprintf(cmd, "killall -9 -q %s", TINY_PROXY_PREFIX);
			tiny_proxy.status = T_WEBWALL_STOP;
			break;
		case T_WEBWALL_RELOAD:/* because of noexist of reload in tinyproxy, first kill, and then start*/
			if (tiny_proxy.status != T_WEBWALL_START) {
				strcpy(errmsg, "tinyproxy is already stopping\n");
				errmsg[errlen - 1] = 0;
				return -eAosRc_WWAlreadyStop;
			}
			ww_save_proxy_data_to_file();
			sprintf(cmd, "killall -9 -q %s", TINY_PROXY_PREFIX);
			system(cmd);
			
			memset(cmd, 0, MAX_BUFF_LEN);
			sprintf(cmd, "%s", TINY_PROXY_EXE_PATH);
			tiny_proxy.status = T_WEBWALL_START;	
			break;
	}
	
	ret = system(cmd);
	sleep(2);	
	/* check whether the exist in process treee*/
	if (ret >= 0) {
		is_running = is_running_by_name(TINY_PROXY_PREFIX);
		switch (type) {
			case T_WEBWALL_START:
				if (is_running < 0)
					ret = -1;
				break;
			case T_WEBWALL_STOP:
				if (is_running >= 0)
					ret = -1;
				break;
			case T_WEBWALL_RELOAD:
				if (is_running < 0)
					ret = -1;
				break;
		}
	}
		
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "failed to %s tinyproxy.\n", action);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		tiny_proxy.status = previous_status;
		return -eAosRc_WWFailAction;
	}
	
	*length = 0;
	
	return 0;
}


/*
 * show proxy configuration
 */
int ww_cli_show_proxy_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* dst;
	char buff[MAX_BUFF_LEN];
	unsigned int index = 0;
	
	dst = CliUtil_getBuff(data);
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "  Proxy IP: %s\n", tiny_proxy.ip);
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Proxy Port: %d\n", tiny_proxy.port);
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "    Status: %s\n", get_action(tiny_proxy.status));	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));	
	
	*length = index;
	
	return 0;
}

/*
 * save config (output cli command)
 */
char* ww_cli_save_config(void)
{
	int len = 0;
	int total_len = 0;
	char buff[MAX_BUFF_LEN];
	char *dst;
	struct aos_list_head head;
	ww_entry_t* node;
	ww_entry_t* tmp;
	ww_entry_t* entry;
	ww_entry_t* tmp_entry;
	ww_filter_name_based_user_t* named_user;
	ww_filter_name_based_user_t* tmp_named_user;
	ww_filter_ip_based_user_t* ip_user;
	ww_filter_ip_based_user_t* tmp_ip_user;
	ww_category_list_t* category;
	ww_category_list_t* tmp_category;
	ww_filter_group_policy_t* policy;
	ww_filter_group_policy_t* tmp_policy;
	ww_filter_group_list_t* filter_group_list;
	ww_filter_group_list_t* tmp_filter_group_list;
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp_engine;
	
	AOS_INIT_LIST_HEAD(&head);
	
	/* global cli */
#ifdef WEBWALL_DEUBG
	printf("start to save global cli\n");
#endif 
	/* web access denied report level */
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL; 
	}

	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall web denied report level %d</Cmd>\n", global.report_level);
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("report level: %d\n", global.report_level);
#endif 
	/* language */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall language %s</Cmd>\n", global.language);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("language: %s\n", global.language);
#endif
	/* log level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall log level %d</Cmd>\n", global.log_level);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("log level: %d\n", global.log_level);
#endif
	/* log exception hits */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall log exception hits %s</Cmd>\n", global.log_exception_hits);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("log exception hits: %s\n", global.log_exception_hits);
#endif
	/* log file format */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall log format %d</Cmd>\n", global.log_file_format);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("log file format: %d\n", global.log_file_format);
#endif
	/* max log item length */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall log max item length %d</Cmd>\n", global.max_log_item_len);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("max log item length: %d\n", global.max_log_item_len);
#endif
	/* anoymous log */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall log anonymize %s</Cmd>\n", global.anoymous_log_switch);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("anoymous log: %s\n", global.anoymous_log_switch);
#endif
	/* webwall ip and port */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall address %s %d</Cmd>\n", global.filter_ip_address, global.filter_port);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("webwall ip and port: %s\n", global.filter_ip_address);
#endif
	/* proxy address and port */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall proxy address %s %d</Cmd>\n", global.proxy_ip_address, global.proxy_port);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("proxy address and port: %s\n", global.proxy_ip_address);
#endif
	/* access denied address */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall access denied address %s</Cmd>\n", global.access_deny_address);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("access denied address: %s\n", global.access_deny_address);
#endif
	/* number of filter groups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall group counts %d</Cmd>\n", global.filter_groups);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("number of filter groups: %d\n", global.filter_groups);
#endif
	/* show weighted found */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall show weighted found %s</Cmd>\n", global.show_weighted_found);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("show weighted found: %s\n", global.show_weighted_found);
#endif
	/* weighted phrase mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall weighted phrase mode %d</Cmd>\n", global.weighted_phrase_mode);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
	/* plugins */
	aos_list_for_each_entry_safe(node, tmp, &(global.auth_plugin_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (entry == NULL) {
			return NULL;
		}
		sprintf(buff, "<Cmd>webwall add auth plugins %s</Cmd>\n", node->data);
		strcpy(entry->data, buff);
		aos_list_add_tail(&(entry->list), &head);
	}
	
	/* reverse address lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall reverse address lookups %s</Cmd>\n", global.reverse_address_lookups);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
	/* reverse clientip lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall reverse clientip lookups %s</Cmd>\n", global.reverse_clientip_lookups);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
	/* reverse client hostnames */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>webwall reverse client hostname %s</Cmd>\n", global.log_client_hostname);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
	
	/* category cli */
	aos_list_for_each_entry_safe(category, tmp_category, &category_list_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall list create %s %s %d</Cmd>\n", get_list_type_name(category->list_type), category->path, category->is_system);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
		
		/* time */
		if (category->tc != NULL) {
			#ifdef WEBWALL_DEUBG
			printf("in time\n");
			#endif
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall list time %s %d:%d %d:%d %s</Cmd>\n",
					category->path, 
					category->tc->start_hour, 
					category->tc->start_minute,
					category->tc->end_hour, 
					category->tc->end_minute, 
					category->tc->days);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}

		/* alias name */
		if (category->category_alias != NULL) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall list alias %s %s</Cmd>\n", 
					category->path, 
					category->category_alias);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}

		/* entry list */
		aos_list_for_each_entry_safe(entry, tmp_entry, &(category->entry_list_head), list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall list add entry %s \"%s\"</Cmd>\n", 
					category->path,
				 	entry->data);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}

		/* squidgard include file */
		aos_list_for_each_entry_safe(entry, tmp_entry, &(category->guard_entry_file_list_head), list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall list include entry file %s %s %s</Cmd>\n", 
					category->path,
					get_entry_type_name(entry->type),
				 	entry->data);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}
		
		/* normal include file */
		aos_list_for_each_entry_safe(entry, tmp_entry, &(category->include_list_head), list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall list include entry file %s %s %s</Cmd>\n", 
					category->path,
					get_entry_type_name(entry->type),
				 	entry->data);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}
	}
#ifdef WEBWALL_DEUBG
	printf("finish saving category cli\n");
#endif
	/* filter group policy cli */
	aos_list_for_each_entry_safe(policy, tmp_policy, &policy_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall group create group%d</Cmd>\n", policy->group_id);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);	
	}
	aos_list_for_each_entry_safe(policy, tmp_policy, &policy_head, list) {
		/* group mode */
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall group policy mode group%d %s</Cmd>\n", 
				policy->group_id,
				get_group_mode_name(policy->group_mode));
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
		
		/* group name */
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall group policy name group%d %s</Cmd>\n",
				policy->group_id,
				policy->group_name);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
		
		/* naughtyness limit */
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall group naughtnesslimit group%d %d</Cmd>\n",
				policy->group_id,
				policy->naughtyness_limit);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
		
		/* list */
		aos_list_for_each_entry_safe(filter_group_list, tmp_filter_group_list, &(policy->category_list_head), list) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall group policy add list group%d %s</Cmd>\n",
					policy->group_id,
					filter_group_list->category_list->path);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}
	}
#ifdef WEBWALL_DEUBG
	printf("finish saving policy cli\n");
#endif

	/* user cli */
	aos_list_for_each_entry_safe(named_user, tmp_named_user, &filter_name_based_user_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall user add name group%d %s</Cmd>\n", named_user->group_id, named_user->user_name);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
	}
	
	aos_list_for_each_entry_safe(ip_user, tmp_ip_user, &filter_ip_based_user_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall user add ips group%d %s</Cmd>\n", ip_user->group_id, ip_user->ip);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
	}
#ifdef WEBWALL_DEUBG
	printf("finish saving user cli\n");
#endif
	
	/* anti-virus */
	aos_list_for_each_entry_safe (engine, tmp_engine, &(av_engine_head), list) {
		/* plugin */
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "<Cmd>webwall av add plugin %s</Cmd>\n", engine->plugname);
		node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
		if (node == NULL) {
			return NULL;
		}
		strcpy(node->data, buff);
		aos_list_add_tail(&(node->list), &head);
		
		/* sockfile location*/
		if (strlen(engine->sockfile_location) > 0) {
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "<Cmd>webwall av sockfile %s %s</Cmd>\n", get_engine_name(engine->type), engine->sockfile_location);
			node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			if (node == NULL) {
				return NULL;
			}
			strcpy(node->data, buff);
			aos_list_add_tail(&(node->list), &head);
		}
	}
	
	/* proxy cli */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "<Cmd>proxy address %s %d</Cmd>\n", tiny_proxy.ip, tiny_proxy.port);
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("finish saving proxy cli\n");
#endif
	/* proxy status */
	memset(buff, 0, MAX_BUFF_LEN);
	switch (tiny_proxy.status) {
		case T_WEBWALL_START:
			sprintf(buff, "<Cmd>proxy status start</Cmd>\n");
			break;
		case T_WEBWALL_STOP:
			sprintf(buff, "<Cmd>proxy status stop</Cmd>\n");
			break;
	}
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);

	/* webwall status */
	memset(buff, 0, MAX_BUFF_LEN);
	switch (global.webwall_status) {
		case T_WEBWALL_START:
			sprintf(buff, "<Cmd>webwall status start</Cmd>\n");
			break;
		case T_WEBWALL_STOP:
			sprintf(buff, "<Cmd>webwall status stop</Cmd>\n");
			break;
	}
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return NULL;
	}
	strcpy(node->data, buff);
	aos_list_add_tail(&(node->list), &head);
#ifdef WEBWALL_DEUBG
	printf("finish saving webwall status\n");
#endif
	
	/* output results */
	len = 0;
	aos_list_for_each_entry_safe(node, tmp, &head, list) {
		len++;
	}
	total_len = len * MAX_ENTRY_LEN;
	dst = (char*)malloc(total_len);
	if (dst == NULL) {
		return NULL;
	}
	
	memset(dst, 0, total_len);
	aos_list_for_each_entry_safe(node, tmp, &head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		strcpy(buff, node->data);
		strcat(dst, buff);
	}
	
	/* free resources */
	aos_list_for_each_entry_safe(node, tmp, &head, list) {
		aos_list_del(&node->list);
		free(node);
	}
	
#ifdef WEBWALL_DEUBG
	printf("save cli: %s\n", dst);
#endif 

	return dst;
}

/*
 * clear config
 */
int ww_cli_clear_config(void)
{
	return ww_clear_config();
}
/*
 * set webwall action
 */
int ww_cli_set_webwall_action(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* action = parms->mStrings[0];
	int ret;
	int type;
	int list_type;
	char buff[MAX_BUFF_LEN];
	char cmd[MAX_BUFF_LEN];
	int previous_status;
	int is_running;
	
	type = validate_action(action);
	if (type == -eAosRc_WWUnsupportActionType) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "the action %s is unsupported.\n", action);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	
	if ((type == global.webwall_status) && (type == T_WEBWALL_START)) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "webwall is already running");
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}
	
	if ((type == global.webwall_status) && (type == T_WEBWALL_STOP)) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "webwall is already stopping");
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -1;
	}

	if ((type == T_WEBWALL_RELOAD) && (global.webwall_status != T_WEBWALL_START)) {
		strcpy(errmsg, "webwall is already stopping");
		errmsg[errlen - 1] = 0;
		return -1;
	}
	
	/* save memory to file before starting */
	previous_status = global.webwall_status;
	memset(cmd, 0, MAX_BUFF_LEN);
	switch (type) {
		case T_WEBWALL_START:
			list_type = validate_group_policy();
			if (list_type > 0) {
				memset(buff, 0, MAX_BUFF_LEN);
				sprintf(buff, 
						"the list type %s must be in group policy\n", 
						get_list_type_name(list_type));
				strncpy(errmsg, buff, errlen - 1);
				errmsg[errlen - 1] = 0;
				return -1;
			} else if (list_type < 0) {
				strncpy(errmsg, "program exception\n", errlen - 1);
				errmsg[errlen - 1] = 0;
				return -1;
			}
			
			ww_save_webwall_data_to_file();
			sprintf(cmd, "killall -9 -q %s", WEBWALL_GROUP_PREFIX);
			ret = system(cmd);

			memset(cmd, 0, MAX_BUFF_LEN);
			sprintf(cmd, "%s", DANSGUARDIAN_EXE_PATH);
			firewall_action(T_WEBWALL_START);
			global.webwall_status = T_WEBWALL_START;
			break;
		case T_WEBWALL_STOP:
			sprintf(cmd, "killall -9 -q %s", WEBWALL_GROUP_PREFIX);	
			firewall_action(T_WEBWALL_STOP);
			global.webwall_status = T_WEBWALL_STOP;
			break;
		case T_WEBWALL_RELOAD:
			list_type = validate_group_policy();
			if (list_type > 0) {
				memset(buff, 0, MAX_BUFF_LEN);
				sprintf(buff, 
						"the list type %s must be in group policy\n", 
						get_list_type_name(list_type));
				strncpy(errmsg, buff, errlen - 1);
				errmsg[errlen - 1] = 0;
				return -1;
			} else if (list_type < 0) {
				strncpy(errmsg, "program exception\n", errlen - 1);
				errmsg[errlen - 1] = 0;
				return -1;
			}

			ww_save_webwall_data_to_file();
			sprintf(cmd, "killall -1 -q %s", WEBWALL_GROUP_PREFIX);
			break;
	}
	
	ret = system(cmd);
	sleep(2);
	/* check whether the exist in process treee*/
	if (ret >= 0) {
		is_running = is_running_by_name(WEBWALL_GROUP_PREFIX);
		switch (type) {
			case T_WEBWALL_START:
				if (is_running < 0) {
					ret = -1;
				}
				break;
			case T_WEBWALL_STOP:
				if (is_running >= 0)
					ret = -1;
				break;
			case T_WEBWALL_RELOAD:
				if (is_running < 0)
					ret = -1;
				break;
		}
	}
	
	if (ret < 0) {
		switch (type) {
			case T_WEBWALL_START:
				firewall_action(T_WEBWALL_STOP);
				break;
			case T_WEBWALL_STOP:
				firewall_action(T_WEBWALL_START);
				break;
		}
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "failed to %s dansguardian.\n", action);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		global.webwall_status = previous_status;
		return -1;
	}
	
	*length = 0;
	return 0;
}	

int ww_cli_register(void)
{
	int ret;
	int i;

	for (i = 0; i < CLI_LEN; i++) {
		ret |= CliUtil_regCliCmd(clis[i].name, clis[i].invoke);
	}
	ww_init();
	
	return ret;
}

int ww_cli_unregister(void)
{
	return 0;
}

/*
 * create group
 */
int ww_cli_create_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int gid;
	int ret;
	char buff[MAX_BUFF_LEN];
	
	gid = ww_get_group_id(group_id);
	if (gid < 1 || gid > WEBWALL_MAX_GROUP) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s is not valid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return -eAosRc_WWInvalidGroupID;
	}
	
	ret = ww_create_group(gid);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Group Name %s is already existed\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	*length = 0;
	
	return ret;
}

/*
 * remove group
 */
int ww_cli_remove_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int gid;
	int ret;
	char buff[MAX_BUFF_LEN];
	
	gid = ww_get_group_id(group_id);
	if ((ret = validate_group_id(gid)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s is not valid\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	ret = ww_remove_group(gid);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s is existed\n", group_id);
		strncpy(errmsg, buff, errlen - 1);
		errmsg[errlen - 1] = 0;
		return ret;
	}
	
	*length = 0;
	
	return 0;
}

/*
 * remove group
 */
int ww_cli_show_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* dst;
	ww_filter_group_policy_t* node;
	ww_filter_group_policy_t* tmp;
	unsigned int index = 0;
	char buff[MAX_BUFF_LEN];
	
	dst = CliUtil_getBuff(data);
	
	aos_list_for_each_entry_safe(node, tmp, &(policy_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "group%d\n", node->group_id);
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	}
	
	*length = index;
	
	return 0;
}

/*
 * set naughtyness limit
 */
int ww_cli_set_naughtyness_limit(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* group_id = parms->mStrings[0];
	int naughtyness = parms->mIntegers[0];
	int gid;
	char buff[MAX_BUFF_LEN];
	int ret;
	
	gid = ww_get_group_id(group_id);
    if ((ret = validate_group_id(gid)) < 0) {
        memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "Group ID %s is not existed or invalid\n", group_id);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
    }
	
	if (naughtyness < 1 || naughtyness > 65530) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "%d is overflow\n", naughtyness);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWNotInRange;
	}
	
	ww_set_naughtyness_limit(gid, naughtyness);
	*length = 0;
	
	return 0;
}

/*
 * set sock file of av engine
 */
int ww_cli_set_av_sock(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* engine_name = parms->mStrings[0];
	char* path = parms->mStrings[1];
	int type;
	char buff[MAX_BUFF_LEN];
	struct stat s;
	int ret;
	
	if ((type = validate_engine(engine_name)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "%s is invalid\n", engine_name);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return type;
	}
	if (stat(path, &s) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "the path %s is invalid\nNote that the AV engine must be running\n", path);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWInvalidPath;
	}
	if (!S_ISSOCK(s.st_mode)) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "the file %s can't be a socket file\nNote that the AV engine must be running\n", path);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWInvalidPath;
	}
	
	ret = ww_set_av_sock(type, path);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistPlugin:
				sprintf(buff, "AV engine name '%s' is not existed\n", engine_name);
				break;
			case -eAosRc_WWError:
				sprintf(buff, "exception occured\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
	}
	
	*length = 0;
	
	return 0;
}

/*
 * show log
 */
int ww_cli_show_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	int log_item;
	char buff[MAX_BUFF_LEN];
	char* p;
	char* dst;
	unsigned int index = 0;
	
	if (parms->mNumIntegers != 1) {
		log_item = DEFAULT_LOG_ITEM;
	} else {
		log_item = parms->mIntegers[0];
	}
	
	if ((log_item <= 0)
		|| (log_item > MAX_LOG_ITEM)) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "the length of log item must be more than 0 and lower than %d", MAX_LOG_ITEM);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWInvalidParameter;
	}
	p = ww_get_log(log_item);
	if (p != NULL){
		dst = CliUtil_getBuff(data);
		CliUtil_checkAndCopy(dst, &index, *length, p, strlen(p));
#ifdef WEBWALL_DEUBG
		syslog(LOG_INFO, "webwall: show log information \n%s\n", p);
#endif
		*length = index;
		free(p);
		return 0;
	} else {
#ifdef WEBWALL_DEUBG
		syslog(LOG_INFO, "webwall: show log error\n");
#endif
	}
	
	*length = 0;
	
	return 0;
}

/*
 * search log
 */
int ww_cli_search_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* start_time = parms->mStrings[0];
	char* end_time = parms->mStrings[1];
	char* keywords = parms->mStrings[2];
	char buff[MAX_BUFF_LEN];
	struct tm s_time;
	struct tm e_time;
	char* p;
	char* dst;
	unsigned int index = 0;
	
	if ((validate_time(start_time, &s_time) < 0)
		|| (validate_time(end_time, &e_time) < 0)
		|| (mktime(&s_time) == -1)
		|| (mktime(&e_time) == -1)) {
		memset(buff, 0, MAX_BUFF_LEN);
        strcpy(buff, "the format of time is invalid\n");
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWTimeFormat;
	}
	if (mktime(&s_time) > mktime(&e_time)) {
		memset(buff, 0, MAX_BUFF_LEN);
        strcpy(buff, "the end time must be larger than the start time\n");
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWTimeFormat;
	}
	p = ww_search_log(&s_time, &e_time, keywords);
	if (p != NULL) {
		dst = CliUtil_getBuff(data);
		CliUtil_checkAndCopy(dst, &index, *length, p, strlen(p));
		*length = index;
		free(p);
		return 0;
	}
	
	*length = 0;
	
	return 0;
}

/*
 * clear log
 */
int ww_cli_clear_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char cmd[MAX_BUFF_LEN];
	char buff[MAX_BUFF_LEN];
	
	memset(cmd, 0, MAX_BUFF_LEN);
	sprintf(cmd, "echo \"\">%s", LOG_LOCATION);
	if (system(cmd) == -1) {
		memset(buff, 0, MAX_BUFF_LEN);
        strcpy(buff, "failed to clear log\n");
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return -1;
	}
	
	*length = 0;
	
	return 0;
}


/*
 * show stat
 */
int ww_cli_show_stat(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* p;
	char* dst;
	unsigned int index = 0;
	
	p = ww_get_stat();
	if (p != NULL){
		dst = CliUtil_getBuff(data);
		CliUtil_checkAndCopy(dst, &index, *length, p, strlen(p));
		*length = index;
		free(p);
		return 0;
	}
	
	*length = 0;
	
	return 0;
}              

/*
 * show global config in webwall
 */
int ww_cli_show_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char buff[MAX_BUFF_LEN];
	char* dst;
	unsigned int index = 0;
	ww_entry_t* node;
	ww_entry_t* tmp;
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp_engine;
	
	dst = CliUtil_getBuff(data);
	/* status */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "                   Status: %s\n", get_action(global.webwall_status));	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* reporting level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "          Reporting Level: %d\n", global.report_level);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* language */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "                 Language: %s\n", global.language);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* log level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "                Log Level: %d\n", global.log_level);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* log exception hits */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "       Log Exception Hits: %s\n", global.log_exception_hits);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* log file format */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "          Log File Format: %d\n", global.log_file_format);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* max log item */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "         Maximum Log Item: %d\n", global.max_log_item_len);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* anonymize log */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "            Anonymize Log: %s\n", global.anoymous_log_switch);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* ip and port of webwall */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "               IP Address: %s\n", global.filter_ip_address);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "                     Port: %d\n", global.filter_port);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* accesss denied address */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "    Access Denied Address: %s\n", global.access_deny_address);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* group length */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "              Group Count: %d\n", global.filter_groups);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* show weighted found */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "      Show Weighted Found: %s\n", global.show_weighted_found);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* weighted phrase mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "     Weighted Phrase Mode: %d\n", global.weighted_phrase_mode);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* auth plugins */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "             Auth Plugins:\n");	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	aos_list_for_each_entry_safe (node, tmp, &(global.auth_plugin_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "                           %s\n", node->data);	
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	}
	
	/* anti-virus plugin */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "               AV Plugins:\n");	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	aos_list_for_each_entry_safe (engine, tmp_engine, &(av_engine_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "                           %s\n", engine->plugname);	
		CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	}
	
	/* reverse address lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "  Reverse Address Lookups: %s\n", global.reverse_address_lookups);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* reverse clientip lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Reverse Client IP Lookups: %s\n", global.reverse_clientip_lookups);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	/* reverse client hostname */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "  Reverse Client Hostname: %s\n", global.log_client_hostname);	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	
	*length = index;
	
	return 0;
}

/*
 * add av engine plugin
 */
int ww_cli_create_av_engine(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char buff[MAX_BUFF_LEN];
	char* plugin_name = parms->mStrings[0];
	int ret;
	
	if ((ret = validate_engine(plugin_name)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "AV engine name '%s' is not invalid\n", plugin_name);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
	}
	
	ret = ww_create_av_plugin(plugin_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWExistPlugin:
				sprintf(buff, "AV engine name '%s' is already existed\n", plugin_name);
				break;
			case -eAosRc_WWError:
				sprintf(buff, "exception occured\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
	}
	
	*length = 0;
	
	return 0;
}

/*
 * remove av engine plugin
 */
int ww_cli_remove_av_engine(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char buff[MAX_BUFF_LEN];
	char* plugin_name = parms->mStrings[0];
	int ret;
	
	if ((ret = validate_engine(plugin_name)) < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
        sprintf(buff, "AV engine name '%s' is not invalid\n", plugin_name);
        strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
	}
	
	ret = ww_remove_av_plugin(plugin_name);
	if (ret < 0) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch (ret) {
			case -eAosRc_WWNoexistPlugin:
				sprintf(buff, "AV engine name '%s' is not existed\n", plugin_name);
				break;
			case -eAosRc_WWError:
				sprintf(buff, "exception occured\n");
				break;
		}
		strncpy(errmsg, buff, errlen - 1);
        errmsg[errlen - 1] = 0;
        return ret;
	}
	
	*length = 0;
	
	return 0;
}

/*
 * list clone
 */
int ww_cli_clone_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* src_list = parms->mStrings[0];
	char* dst_list = parms->mStrings[1];
	int ret;
	
	if (strcmp(src_list, dst_list) == 0) {
		strncpy(errmsg, "source list name must be different from destination list name\n", errlen - 1);
        errmsg[errlen - 1] = 0;
        return -eAosRc_WWInvalidParameter;
	}
	
	ret = ww_list_clone(src_list, dst_list);
	if (ret < 0) {
		switch (ret) {
			case -eAosRc_WWNoexistListName: 
				sprintf(errmsg, "the source list name '%s' is not existed\n", src_list);
				break;
			case -eAosRc_WWError:
				sprintf(errmsg, "exception occured\n");			
				break;	
		}
		errmsg[errlen - 1] = 0;
        return ret;
	}
	
	*length = 0;

	return 0;
}

/*
 * show all list
 */
int ww_cli_show_all_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* dst;
	unsigned int index = 0;
	char buff[MAX_BUFF_LEN * 10];
	ww_category_list_t* node;
	ww_category_list_t* tmp;
	
	memset(buff, 0, MAX_BUFF_LEN);
	dst = CliUtil_getBuff(data);
	aos_list_for_each_entry_safe (node, tmp, &(category_list_head), list) {
		if (node->is_system == 1)
			sprintf(buff, "%s%s(System List, %d)\n", buff, node->path, node->ref_cout);
		else
			sprintf(buff, "%s%s(User List, %d)\n", buff, node->path, node->ref_cout);
	}
	
	CliUtil_checkAndCopy(dst, &index, *length, buff, strlen(buff));
	*length = index;
	
	return 0;
}

static void free_category_list(ww_category_list_t* node)
{
    ww_entry_t* entry;
    ww_entry_t* tmp_entry;
    
	if (node->tc != NULL) {
        free(node->tc);
    }
    if (node->category_alias != NULL) {
        free(node->category_alias);
    }

    aos_list_for_each_entry_safe (entry, tmp_entry, &(node->include_list_head), list) {
        aos_list_del(&(entry->list));
        free(entry);
    }

    aos_list_for_each_entry_safe (entry, tmp_entry, &(node->entry_list_head), list) {
        aos_list_del(&(entry->list));
        free(entry);
    }

    aos_list_for_each_entry_safe (entry, tmp_entry, &(node->guard_entry_file_list_head), list) {
        aos_list_del(&(entry->list));
        free(entry);
    }

}

ww_category_list_t* parse_file(const char* path, const char* file_name, int type)
{
    ww_category_list_t* node;
	char buff[MAX_BUFF_LEN];
	FILE* fp = NULL;
	int len;
	ww_entry_t* entry;
	int start_hour;
	int end_hour;
	int start_minute;
	int end_minute;
	char days[MAX_BUFF_LEN];
	char* p;
	
	if ((fp = fopen(path, "r")) == NULL) {
	    printf("can not open file '%s'\n", path);
		return NULL;
	}
	
	if ((node = (ww_category_list_t*)malloc(sizeof(ww_category_list_t))) == NULL) {
	    return NULL;
	}
	
	node->list_type = type;
	strcpy(node->path, file_name);
	node->tc = NULL;
	node->category_alias = NULL;
	node->ref_cout = 0;
	node->is_system = 0;
	AOS_INIT_LIST_HEAD(&(node->include_list_head));
	AOS_INIT_LIST_HEAD(&(node->entry_list_head));
	AOS_INIT_LIST_HEAD(&(node->guard_entry_file_list_head));

	while (!feof(fp)) {
	    fgets(buff, MAX_BUFF_LEN, fp);
		len = strlen(buff);
		if (len > 2) {
		    if (buff[0] == '#') {
			    if (strncmp(buff, "#time: ", strlen("#time: ")) == 0) {
				    if (sscanf(buff, "#time: %d %d %d %d %s", 
							   &start_hour, &start_minute, &end_hour, &end_minute,
							   days) == 5) {
					    if (start_hour < 0 || start_hour > 23
							|| end_hour < 0 || end_hour > 23
							|| start_minute < 0 || start_minute > 59
							|| end_minute < 0 || end_minute > 59) {
						    free_category_list(node);
						    continue;
						}
						if ((start_hour > end_hour) 
							|| (start_hour == end_hour && start_minute > end_minute)) {
						    continue;
						}
					    if (strlen(days) > 7) {
						    continue;
						} else {
						    p = days;
							while (*p) {
							    if (*p < '0' || *p > '6') {
								    continue;
							    }
							}
						}
						/* create time section */
						node->tc = (ww_time_control_t*)malloc(sizeof(ww_time_control_t));
						if (node->tc == NULL) {
						    free_category_list(node);
							return NULL;
						}
						node->tc->start_hour = start_hour;
						node->tc->start_minute = start_minute;
						node->tc->end_hour = end_hour;
						node->tc->end_minute = end_minute;
						strcpy(node->tc->days, days);
					}
				} else if (strncmp(buff, "#listcategory:", strlen("#listcategory:")) == 0) {
				    if ((node->category_alias = (char* )malloc(sizeof(char) * MAX_BUFF_LEN)) == NULL) {
						free_category_list(node);
						return NULL;
				}
					strcpy(node->category_alias, buff + strlen("#listcategory:"));
				}
				continue;
			} else if (buff[0] == '.') {
			    if (strncmp(buff, ".Include<", strlen(".Include<")) == 0
					&& strcmp(buff + len - 1, ">") == 0) {
				    continue;
				}
			}
			if ((entry = (ww_entry_t* )malloc(sizeof(ww_entry_t))) == NULL) {
			    free_category_list(node);
				return NULL;
			}
			aos_list_add_tail(&(entry->list), &(node->entry_list_head));
		}
	}
	fclose(fp);

	return node;
}


/*
 * show all list
 */
int ww_cli_import_file_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen)
{
	char* method_name = parms->mStrings[0];
	char* list_type_name = parms->mStrings[1];
	char* url = parms->mStrings[2];
	struct stat s;
	char buff[MAX_BUFF_LEN];
	ww_category_list_t* category;
	int list_type;
	int method_type;
	char* p;
	
	if ((list_type = validate_list_type(list_type_name)) < 0) {
		snprintf(errmsg, errlen - 1, "list type '%s' is unsupported\n", list_type_name);
        errmsg[errlen - 1] = 0;
        return list_type;
	}
	
	if ((method_type = validate_method(method_name)) < 0) {
		snprintf(errmsg, errlen - 1, "import method '%s' is unsupported\n", method_name);
        errmsg[errlen - 1] = 0;
        return method_type;
	} 
	
	memset(buff, 0, MAX_BUFF_LEN);
	
	switch (method_type) {
		case T_WEBWALL_IMPORT_TYPE_LOCAL:
			if ((stat(url, &s) < 0) || (!S_ISREG(s.st_mode))) {
        		snprintf(errmsg, errlen - 1, "local file '%s' is not existed or invalid\n", url);
        		errmsg[errlen - 1] = 0;
        		return -eAosRc_WWInvalidPath;
			}
			p = get_file_name(url);
			if (p == NULL) {
				snprintf(errmsg, errlen - 1, "failed to import file\n");
			    return -eAosRc_WWImportFile;
			}
			category = parse_file(url, p, list_type);
			
			if (category == NULL) {
			    snprintf(errmsg, errlen - 1, "failed to import file\n");
			    return -eAosRc_WWImportFile;
			}
			aos_list_add_tail(&(category->list), &category_list_head);
			break;
		default:
		    sprintf(buff, "wget %s -o /tmp", url);
			if (system(buff) != 0) {
			    snprintf(errmsg, errlen - 1, "failed to import file\n");
			    return -eAosRc_WWImportFile;
			}
			p = get_file_name(url);
			if (p == NULL) {
				snprintf(errmsg, errlen - 1, "failed to import file\n");
			    return -eAosRc_WWImportFile;
			}
			memset(buff, 0, MAX_BUFF_LEN);
			sprintf(buff, "/tmp/%s", p);
			category = parse_file(buff, p, list_type);
			
			if (category == NULL) {
			    snprintf(errmsg, errlen - 1, "failed to import file\n");
			    return -eAosRc_WWImportFile;
			}
			aos_list_add_tail(&(category->list), &category_list_head);
			break;
	}
	
	*length = 0;
	
	return 0;
}




