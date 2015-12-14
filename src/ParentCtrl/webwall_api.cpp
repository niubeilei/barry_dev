/*
 * Webwall module API implementation
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_api.cpp
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#include "ParentCtrl/webwall_api.h"
#include "aosUtil/Types.h"
/*
 * create category list
 * @list_type
 * @list_name
 * @result
 */
int ww_api_create_category_list(char* list_type, char* list_name, char* result)
{
	return 0;
}

/*
 * remove category list
 * @list_name
 * @result
 */
int ww_api_remove_category_list(char* list_name, char* result)
{
	return 0;
}

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 * @result
 */
int ww_api_set_time(char* list_name, char* start_time, char* end_time, char* days, char* result)
{
	return 0;
}

/*
 * remove time of category list
 * @list_name
 * @result
 */
int ww_api_remove_time(char* list_name, char* result)
{
	return 0;
}

/*
 * create alias of category list
 * @list_name
 * @alias_name
 * @result
 */
int ww_api_create_alias(char* list_name, char* alias_name, char* result)
{
	return 0;
}

/*
 * remove alias of category list
 * @list_name
 * @alias_name
 * @result
 */
int ww_api_remove_alias(char* list_name, char* alias_name, char* result)
{
	return 0;
}

/*
 * create single entry
 * @list_name
 * @entry
 * @result
 */
int ww_api_create_single_entry(char* list_name, char* entry, char* result)
{
	return 0;
}

/*
 * remove single entry
 * @list_name
 * @entry
 * @result
 */
int ww_api_remove_single_entry(char* list_name, char* entry, char* result)
{
	return 0;
}

/*
 * create include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 * @result
 */
int ww_api_create_include_entry_file(char* list_name, char* entry_type, char* entry_list_name, char* result)
{
	return 0;
}

/*
 * remove include entry file
 * @list_name
 * @entry_list_name
 * @result
 */
int ww_api_remove_include_entry_file(char* list_name, char* entry_list_name, char* result)
{
	return 0;
}

/*
 * show category list
 * @list_name
 * @result
 */
int ww_api_show_category_list(char* list_name, char* result)
{
	return 0;
}

/*
 * create named user
 * @group_id
 * @user_name
 * @result
 */
int ww_api_create_named_user(char* group_id, char* user_name, char* result)
{
	return 0;
}

/*
 * remove named user
 * @group_id
 * @user_name
 * @result
 */
int ww_api_remove_named_user(char* group_id, char* user_name, char* result)
{
	return 0;
}

/*
 * show named user
 * @group_id
 * @user_name
 * @result
 */
int ww_api_show_named_user(char* group_id, char* result)
{
	return 0;
}

/*
 * remove all named users
 * @group_id
 * @result
 */
int ww_api_remove_all_named_users(char* group_id, char* result)
{
	return 0;
}

/*
 * create ip based user
 * @group_id
 * @ip
 * @result
 */
int ww_api_create_ip_user(char* group_id, char* ip, char* result)
{
	return 0;
}

/*
 * remove ip based user
 * @group_id
 * @ip
 * @result
 */
int ww_api_remove_ip_user(char* group_id, char* ip, char* result)
{
	return 0;
}

/*
 * show ip based user
 * @group_id
 * @result
 */
int ww_api_show_ip_user(char* group_id, char* result)
{
	return 0;
}

/*
 * remove all ip based user
 * @group_id
 * @result
 */
int ww_api_remove_all_ip_users(char* group_id, char* result)
{
	return 0;
}

/*
 * set filter group mode
 * @group_id
 * @mode
 * @result
 */
int ww_api_set_filter_group_mode(char* group_id, char* mode, char* result)
{
	return 0;
}

/*
 * set filter group name
 * @group_id
 * @group_name
 * @result
 */
int ww_api_set_filter_group_name(char* group_id, char* group_name, char* result)
{
	return 0;
}

/*
 * map category list to filter group
 * @group_id
 * @list_name
 * @result
 */
int ww_api_map_category_list_to_filter_group(char* group_id, char* list_name, char* result)
{
	return 0;
}

/*
 * unmap category list of filter group
 * @group_id
 * @list_name
 * @result
 */
int ww_api_unmap_category_list_of_filter_group(char* group_id, char* list_name, char* result)
{
	return 0;
}

/*
 * unmap all category list of filter group
 * @group_id
 * @list_type
 * @result
 */
int ww_api_unmap_all_category_list_of_filter_group(char* group_id, char* list_type, char* result)
{
	return 0;
}

/*
 * show category list of filter group
 * @group_id
 * @result
 */
int ww_api_show_category_list_of_filter_group(char* group_id, char* result)
{
	return 0;
}

/*
 * set reporting level of web access denies
 * @reporting_level
 * @result
 */
int ww_api_set_reporting_level(int reporting_level, char* result)
{
	return 0;
}

/*
 * set language
 * @lang
 * @result
 */
int ww_api_set_language(char* lang, char* result)
{
	return 0;
}

/*
 * set log level
 * @log_level
 * @result
 */
int ww_api_set_log_level(int log_level, char* result)
{
	return 0;
}

/*
 * set log exception hits
 * @log_exception_hits
 * @result
 */
int ww_api_set_log_exception_hits(char* log_exception_hits, char* result)
{
	return 0;
}

/*
 * set log file format
 * @format
 * @result
 */
int ww_api_set_log_file_format(int format, char* result)
{
	return 0;
}

/*
 * set maximum log item length
 * @len
 * @result
 */
int ww_api_set_max_log_item_len(int len, char* result)
{
	return 0;
}

/*
 * set anoymous log
 * @anoymous_log
 * @result
 */
int ww_api_set_anoymous_log(char* anoymous_log, char* result)
{
	return 0;
}

/*
 * set ip and port of webwall
 * @ip
 * @port
 * @result
 */
int ww_api_set_ip_and_port_of_webwall(char* ip, u16 port, char* result)
{
	return 0;
}

/*
 * set ip and port of proxy
 * @ip
 * @port
 * @result
 */
int ww_api_set_ip_and_port_of_proxy(char* ip, u16 port, char* result)
{
	return 0;
}

/*
 * set the access denied address
 * @addr
 * @result
 */
int ww_api_set_access_denied_addr(char* addr, char* result)
{
	return 0;
}

/*
 * set filter group length
 * @len
 * @result
 */
int ww_api_set_filter_group_len(int len, char* result)
{
	return 0;
}

/*
 * set show weighted found
 * @show_weighted_found
 * @result
 */
int ww_api_set_show_weighted_found(char* show_weighted_found, char* result)
{
	return 0;
}

/*
 * set weighted phrase mode
 * @mode
 * @result
 */
int ww_api_set_weighted_phrase_mode(int mode, char* result)
{
	return 0;
}

/*
 * create auth plugins
 * @plugin_name
 * @result
 */
int ww_api_create_auth_plugins(char* plugin_name, char* result)
{
	return 0;
}

/*
 * remove auth plugin
 * @plugin_name
 * @result
 */
int ww_api_remove_auth_plugin(char* plugin_name, char* result)
{
	return 0;
}

/*
 * set the reverse address lookups
 * @reverse_addr_lookups
 * @result
 */
int ww_api_reverse_addr_lookups(char* reverse_addr_lookups, char* result)
{
	return 0;
}

/*
 * set the reverse clientip lookups
 * @reverse_clientip_lookups
 * @result
 */
int ww_api_reverse_clientip_lookups(char* reverse_clientip_lookups, char* result)
{
	return 0;
}

/*
 * set the reverse client hostname
 * @reverse_client_hostname
 * @result
 */
int ww_api_reverse_client_hostname(char* reverse_client_hostname, char* result)
{
	return 0;
}

/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 * @ip
 * @port
 * @result
 */
int ww_api_set_proxy_addr(char* ip, u16 port, char* result)
{
	return 0;
}

/*
 * set proxy action
 * @action
 * @result
 */
int ww_api_set_proxy_action(char* action, char* result)
{
	return 0;
}

/*
 * show proxy configuration
 * @result
 */
int ww_api_show_proxy_config(char* result)
{
	return 0;
}

/*
 * set webwall action
 */
int ww_api_set_webwall_action(char* action)
{
	return 0;
}

/*
 * create group
 */
int ww_cli_create_group(int gid, char* result)
{
	return 0;
}

/*
 * remove group
 */
int ww_cli_remove_group(int gid, char* result)
{
	return 0;
}
