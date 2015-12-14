#include "webwall.h"
#include "common.h"

#define MAX_BUFF_LEN 1024

/*
 * set proxy ip address and port
 * @ip
 * @port
 */
int aos_ww_api_set_proxy_addr(char* ip, int port)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "proxy address %s %d", ip, port);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * start/stop proxy
 * @action
 */
int aos_ww_api_set_proxy_action(char *action)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "proxy status %s", action);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
} 

/*
 * show configuration of proxy
 * @result
 */
int aos_ww_api_proxy_retrieve_config(char *result, int *resultlen)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;
	
	sprintf(cmd, "proxy show config");
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
        
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	strcpy(result, rslt);

	return *((int*)tmp);
}

/*
 * set the access denied address
 * @url
 */
int aos_ww_api_set_access_denied_addr(char *url)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall access denied address %s", url);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the language
 * @lang
 */
int aos_ww_cli_set_language(char *lang)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall language %s", lang);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the log level
 * @level
 */
int aos_ww_api_set_log_level(int level)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall log level %d", level);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the log exception hits
 * @onoff
 */
int aos_ww_api_set_log_exception_hits(char *onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall log exception hits %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the format of log file
 * @format
 */
int aos_ww_api_set_log_file_format(int format)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall log format %d", format);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
} 

/*
 * set the maximum length of log item
 * @len
 */
int aos_ww_api_set_max_log_item_len(int len)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall log max item length %d", len);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the anonymous log
 * @onoff
 */
int aos_ww_api_set_anonymous_log(char *onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall log anonymize %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the ip address and port of webwall
 * @ip
 * @port
 */
int aos_ww_api_set_ip_and_port_of_webwall(char *ip, int port)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall address %s %d", ip, port);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
} 

/*
 * set the reporting level
 * @level
 */
int aos_ww_api_set_reporting_level(int level)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall web denied report level %d", level);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the length of filter groups
 * @len
 */
int aos_ww_api_set_filter_group_len(int len)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group counts %d", len);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the show weighted found
 * @onoff
 */
int aos_ww_api_set_show_weighted_found(char *onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall show weighted found %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the weighted phrase mode
 * @mode
 */
int aos_ww_api_set_weighted_phrase_mode(int mode)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall weighted phrase mode %d", mode);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create the authority plugin
 * @pluginname
 */
int aos_ww_api_create_auth_plugins(char *pluginname)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall add auth plugins %s", pluginname);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove authority plugin
 * @pluginname
 */
int aos_ww_api_remove_auth_plugin(char *pluginname)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall remove auth plugins %s", pluginname);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the reverse address lookups
 * @onoff
 */
int aos_ww_api_reverse_addr_lookups(char* onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall reverse address lookups %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the reverse client hostname
 * @onoff
 */
int aos_ww_api_reverse_client_hostname(char *onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall reverse client hostname %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the reverse clientip lookups
 * @onoff
 */
int aos_ww_api_reverse_clientip_lookups(char *onoff)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall reverse clientip lookups %s", onoff);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create a named user
 * @group_id
 * @user_name
 */
int aos_ww_api_create_named_user(char *group_id, char* user_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user add name %s %s", group_id, user_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a named user
 * @group_id
 * @user_name
 */
int aos_ww_api_remove_named_user(char *group_id, char *user_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user remove name %s %s", group_id, user_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show information of named user
 * @group_id
 * @result
 */
int aos_ww_api_retrieve_named_user(char *group_id, char *result, int *resultlen)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall user show name %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	strcpy(result, rslt);

	return *((int*)tmp);
}

/*
 * remove all named users of a group
 * @group_id
 */
int aos_ww_api_remove_all_named_users(char *group_id)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user remove all name %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create ip-based user
 * @group_id
 * @ip
 */
int aos_ww_api_create_ip_user(char *group_id, char *ip)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user add ips %s %s", group_id, ip);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a ip-based user
 * @group_id
 * @ip
 */
int aos_ww_api_remove_ip_user(char *group_id, char *ip)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user remove ips %s %s", group_id, ip);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show all ip-based user of a group
 * @group_id
 * @result
 */
int aos_ww_api_retrieve_ip_user(char *group_id, char *result, int *resultlen)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall user show ips %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	strcpy(result, rslt);

	return *((int*)tmp);
}

/*
 * remove all ip-based user of a group
 * @group_id
 */
int aos_ww_api_remove_all_ip_users(char *group_id)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall user remove all ips %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the mode of filter group
 * @group_id
 * @mode_name
 */
int aos_ww_api_set_filter_group_mode(char *group_id, char *mode_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group policy mode %s %s", group_id, mode_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the name of filter group
 * @group_id
 * @group_name
 */
int aos_ww_api_set_filter_group_name(char *group_id, char *group_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group policy name %s %s", group_id, group_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show information of group policy
 * @group_id
 * @result
 */
int aos_ww_api_retrieve_category_list_of_filter_group(char *group_id, char *result, int *resultlen)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall group policy show config %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	strcpy(result, rslt);

	return *((int*)tmp);
}

/*
 * create a filter group
 * @group_id
 */
int aos_ww_api_create_group(char *group_id)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group create %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a filter group
 * @group_id
 */
int aos_ww_api_remove_group(char *group_id)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group remove %s", group_id);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create a category list
 * @list_type_name
 * @list_name
 */
int aos_ww_api_create_category_list(char *list_type_name, char *list_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list create %s %s", list_type_name, list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a category list
 * @list_name
 */
int aos_ww_api_remove_category_list(char *list_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list remove %s", list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 * */
int aos_ww_api_set_time(char *list_name, char *start_time, char *end_time, char *days)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list time %s %s %s %s", list_name, start_time, end_time, days);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove time of a category list
 * @list_name
 */
int aos_ww_api_remove_time(char *list_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list remove time %s", list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create alias of a category list
 * @list_name
 * @alias_name
 */
int aos_ww_api_create_alias(char *list_name, char *alias_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list alias %s %s", list_name, alias_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove alias of a category list
 * @list_name
 */
int aos_ww_api_remove_alias(char *list_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list remove alias %s", list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create a single entry
 * @list_name
 * @entry
 */
int aos_ww_api_create_single_entry(char *list_name, char *entry)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list add entry %s %s", list_name, entry);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove a single entry
 * @list_name
 * @entry
 */
int aos_ww_api_remove_single_entry(char *list_name, char *entry)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list remove entry %s %s", list_name, entry);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create an include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 */
int aos_ww_api_create_include_entry_file(char *list_name, char *entry_type, char* entry_list_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list include entry file %s %s %s", 
			list_name,
			entry_type, entry_list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove an include entry file
 * @list_name
 * @entry_file_name
 */
int aos_ww_api_remove_include_entry_file(char *list_name, char *entry_file_name)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list remove entry file %s %s", list_name, entry_file_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
} 

/*
 * show information of a category list
 * @list_name
 * @result
 */
int aos_ww_api_retrieve_category_list(char *list_name, char *result, int *resultlen)
{
    OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall list show config %s", list_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}  

/*
 * set the limit of naughtyness
 * @group_id
 * @limit
 */
int aos_ww_api_set_naughtyness_limit(char *group_id, int limit)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall group naughtnesslimit %s %d", group_id, limit);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * set the sock file of av engine
 * @engine_name
 * @path
 */
int aos_ww_api_set_av_sock(char *engine_name, char *path)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall av sockfile %s %s", engine_name, path);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show log
 * @len
 * @result
 * @resultlen
 */
int aos_ww_api_retrieve_log(int items, char *result, int *resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall show log %d", items);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * search log by keywords
 * @start_time
 * @end_time
 * @keywords
 */
int aos_ww_api_search_log(char *start_time, char *end_time, char *keywords, char *result, int *resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall search log \"%s\" \"%s\" \"%s\"", start_time, end_time, keywords);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * clear log
 */
int aos_ww_api_clear_log()
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall clear log");
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show statistics
 * @result
 * @resultlen
 */
int aos_ww_api_retrieve_stat(char *result, int *resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall show stat");
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show global configuration
 * @result
 * @resultlen
 */
int aos_ww_api_retrieve_config(char *result, int *resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall show config");
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * create anti-virus engine
 * @plugin_name
 */
int aos_ww_api_create_av_engine(char *plugin_name)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall av add plugin %s", plugin_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * remove anti-virus engine
 * @plugin_name
 */
int aos_ww_api_remove_av_engine(char *plugin_name)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall av remove plugin %s", plugin_name);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/* 
 * clone list
 * @src
 * @dst
 */
int aos_ww_api_clone_list(char *src, char *dst)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];

	sprintf(cmd, "webwall list clone %s %s", src, dst);
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}

/*
 * show list configuration
 * @result
 * @resultlen
 */
int aos_ww_api_retrieve_all_list(char *result, int *resultlen)
{
	OmnString rslt = "";
	char* tmp;
	char cmd[MAX_BUFF_LEN];
	int len;

	sprintf(cmd, "webwall list show all");
	
	OmnCliProc::getSelf()->runCliAsClient(cmd, rslt);
	
	len = strlen(rslt);
    if (len > *resultlen) {
    	strncpy(result, rslt, *resultlen);
        return eAosRc_LenTooShort;
    } else {
        strcpy(result, rslt);
        *resultlen = len;
    }
    
	tmp = rslt.getBuffer();
	tmp += rslt.find(0, false) + 1;

	return *((int*)tmp);
}


   
