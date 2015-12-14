/*
 * Webwall module API header
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_api.h
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#ifndef WEBWALL_API_H
#define WEBWALL_API_H

#include "aosUtil/Types.h"
/*
 * create category list
 * @list_type
 * @list_name
 * @result
 */
extern int ww_api_create_category_list(char* list_type, char* list_name, char* result);

/*
 * remove category list
 * @list_name
 * @result
 */
extern int ww_api_remove_category_list(char* list_name, char* result);

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 * @result
 */
extern int ww_api_set_time(char* list_name, char* start_time, char* end_time, char* days, char* result);

/*
 * remove time of category list
 * @list_name
 * @result
 */
extern int ww_api_remove_time(char* list_name, char* result);

/*
 * create alias of category list
 * @list_name
 * @alias_name
 * @result
 */
extern int ww_api_create_alias(char* list_name, char* alias_name, char* result);

/*
 * remove alias of category list
 * @list_name
 * @alias_name
 * @result
 */
extern int ww_api_remove_alias(char* list_name, char* alias_name, char* result);

/*
 * create single entry
 * @list_name
 * @entry
 * @result
 */
extern int ww_api_create_single_entry(char* list_name, char* entry, char* result);

/*
 * remove single entry
 * @list_name
 * @entry
 * @result
 */
extern int ww_api_remove_single_entry(char* list_name, char* entry, char* result);

/*
 * create include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 * @result
 */
extern int ww_api_create_include_entry_file(char* list_name, char* entry_type, char* entry_list_name, char* result);

/*
 * remove include entry file
 * @list_name
 * @entry_list_name
 * @result
 */
extern int ww_api_remove_include_entry_file(char* list_name, char* entry_list_name, char* result);

/*
 * show category list
 * @list_name
 * @result
 */
extern int ww_api_show_category_list(char* list_name, char* result);

/*
 * create named user
 * @group_id
 * @user_name
 * @result
 */
extern int ww_api_create_named_user(char* group_id, char* user_name, char* result);

/*
 * remove named user
 * @group_id
 * @user_name
 * @result
 */
extern int ww_api_remove_named_user(char* group_id, char* user_name, char* result);

/*
 * show named user
 * @group_id
 * @user_name
 * @result
 */
extern int ww_api_show_named_user(char* group_id, char* result);

/*
 * remove all named users
 * @group_id
 * @result
 */
extern int ww_api_remove_all_named_users(char* group_id, char* result);

/*
 * create ip based user
 * @group_id
 * @ip
 * @result
 */
extern int ww_api_create_ip_user(char* group_id, char* ip, char* result);

/*
 * remove ip based user
 * @group_id
 * @ip
 * @result
 */
extern int ww_api_remove_ip_user(char* group_id, char* ip, char* result);

/*
 * show ip based user
 * @group_id
 * @result
 */
extern int ww_api_show_ip_user(char* group_id, char* result);

/*
 * remove all ip based user
 * @group_id
 * @result
 */
extern int ww_api_remove_all_ip_users(char* group_id, char* result);

/*
 * set filter group mode
 * @group_id
 * @mode
 * @result
 */
extern int ww_api_set_filter_group_mode(char* group_id, char* mode, char* result);

/*
 * set filter group name
 * @group_id
 * @group_name
 * @result
 */
extern int ww_api_set_filter_group_name(char* group_id, char* group_name, char* result);

/*
 * map category list to filter group
 * @group_id
 * @list_name
 * @result
 */
extern int ww_api_map_category_list_to_filter_group(char* group_id, char* list_name, char* result);

/*
 * unmap category list of filter group
 * @group_id
 * @list_name
 * @result
 */
extern int ww_api_unmap_category_list_of_filter_group(char* group_id, char* list_name, char* result);

/*
 * unmap all category list of filter group
 * @group_id
 * @list_type
 * @result
 */
extern int ww_api_unmap_all_category_list_of_filter_group(char* group_id, char* list_type, char* result);

/*
 * show category list of filter group
 * @group_id
 * @result
 */
extern int ww_api_show_category_list_of_filter_group(char* group_id, char* result);

/*
 * set reporting level of web access denies
 * @reporting_level
 * @result
 */
extern int ww_api_set_reporting_level(int reporting_level, char* result);

/*
 * set language
 * @lang
 * @result
 */
extern int ww_api_set_language(char* lang, char* result);

/*
 * set log level
 * @log_level
 * @result
 */
extern int ww_api_set_log_level(int log_level, char* result);

/*
 * set log exception hits
 * @log_exception_hits
 * @result
 */
extern int ww_api_set_log_exception_hits(char* log_exception_hits, char* result);

/*
 * set log file format
 * @format
 * @result
 */
extern int ww_api_set_log_file_format(int format, char* result);

/*
 * set maximum log item length
 * @len
 * @result
 */
extern int ww_api_set_max_log_item_len(int len, char* result);

/*
 * set anoymous log
 * @anoymous_log
 * @result
 */
extern int ww_api_set_anoymous_log(char* anoymous_log, char* result);

/*
 * set ip and port of webwall
 * @ip
 * @port
 * @result
 */
extern int ww_api_set_ip_and_port_of_webwall(char* ip, u16 port, char* result);

/*
 * set ip and port of proxy
 * @ip
 * @port
 * @result
 */
extern int ww_api_set_ip_and_port_of_proxy(char* ip, u16 port, char* result);

/*
 * set the access denied address
 * @addr
 * @result
 */
extern int ww_api_set_access_denied_addr(char* addr, char* result);

/*
 * set filter group length
 * @len
 * @result
 */
extern int ww_api_set_filter_group_len(int len, char* result);

/*
 * set show weighted found
 * @show_weighted_found
 * @result
 */
extern int ww_api_set_show_weighted_found(char* show_weighted_found, char* result);

/*
 * set weighted phrase mode
 * @mode
 * @result
 */
extern int ww_api_set_weighted_phrase_mode(int mode, char* result);

/*
 * create auth plugins
 * @plugin_name
 * @result
 */
extern int ww_api_create_auth_plugins(char* plugin_name, char* result);

/*
 * remove auth plugin
 * @plugin_name
 * @result
 */
extern int ww_api_remove_auth_plugin(char* plugin_name, char* result);

/*
 * set the reverse address lookups
 * @reverse_addr_lookups
 * @result
 */
extern int ww_api_reverse_addr_lookups(char* reverse_addr_lookups, char* result);

/*
 * set the reverse clientip lookups
 * @reverse_clientip_lookups
 * @result
 */
extern int ww_api_reverse_clientip_lookups(char* reverse_clientip_lookups, char* result);

/*
 * set the reverse client hostname
 * @reverse_client_hostname
 * @result
 */
extern int ww_api_reverse_client_hostname(char* reverse_client_hostname, char* result);

/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 * @ip
 * @port
 * @result
 */
extern int ww_api_set_proxy_addr(char* ip, u16 port, char* result);

/*
 * set proxy action
 * @action
 * @result
 */
extern int ww_api_set_proxy_action(char* action, char* result);

/*
 * show proxy configuration
 * @result
 */
extern int ww_api_show_proxy_config(char* result);

/*
 * set webwall action
 */
extern int ww_api_set_webwall_action(char* action);

/*
 * create group
 */
extern int ww_cli_create_group(int gid, char* result);

/*
 * remove group
 */
extern int ww_cli_remove_group(int gid, char* result);

#endif
