/*
 * Webwall module implementation header
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_impl.h
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#ifndef WEBWALL_IMPL_H
#define WEBWALL_IMPL_H

#include "ParentCtrl/webwall_common.h" 

/*
 * create category list 
 * @list_type
 * @list_name
 * @is_system
 */
extern int ww_create_category_list(char* list_type, char* list_name, int is_system);

/*
 * remove category list
 * @list_name
 */
extern int ww_remove_category_list(char* list_name);

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 */
extern int ww_set_time(char* list_name, char* start_time, char* end_time, char* days);

/*
 * remove time of category list
 * @list_name
 */
extern int ww_remove_time(char* list_name);

/*
 * create alias of category list
 * @list_name
 * @alias_name
 */
extern int ww_create_alias(char* list_name, char* alias_name);

/*
 * remove alias of category list
 * @list_name
 */
extern int ww_remove_alias(char* list_name);

/*
 * create single entry
 * @list_name
 * @entry
 */
extern int ww_create_single_entry(char* list_name, char* entry);

/*
 * remove single entry
 * @list_name
 * @entry
 */
extern int ww_remove_single_entry(char* list_name, char* entry);

/*
 * create include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 */
extern int ww_create_include_entry_file(char* list_name, char* entry_type, char* entry_list_name);

/*
 * remove include entry file
 * @list_name
 * @entry_list_name
 */
extern int ww_remove_include_entry_file(char* list_name, char* entry_list_name);

/*
 * create named user
 * @group_id
 * @user_name
 */
extern int ww_create_named_user(char* group_id, char* user_name);

/*
 * remove named user
 * @group_id
 * @user_name
 */
extern int ww_remove_named_user(char* group_id, char* user_name);

/*
 * remove all named users
 * @group_id
 */
extern int ww_remove_all_named_users(char* group_id);

/*
 * create ip based user
 * @group_id
 * @ip
 */
extern int ww_create_ip_user(char* group_id, char* ip);

/*
 * remove ip based user
 * @group_id
 * @ip
 */
extern int ww_remove_ip_user(char* group_id, char* ip);

/*
 * remove all ip based user
 * @group_id
 */
extern int ww_remove_all_ip_users(char* group_id);

/*
 * set filter group mode
 * @group_id
 * @mode
 */
extern int ww_set_filter_group_mode(char* group_id, char* mode);

/*
 * set filter group name
 * @group_id
 * @group_name
 */
extern int ww_set_filter_group_name(char* group_id, char* group_name);

/*
 * map category list to filter group
 * @group_id
 * @list_name
 */
extern int ww_map_category_list_to_filter_group(char* group_id, char* list_name);

/*
 * unmap category list of filter group
 * @group_id
 * @list_name
 */
extern int ww_unmap_category_list_of_filter_group(char* group_id, char* list_name);

/*
 * unmap all category list of filter group
 * @group_id
 * @list_type
 */
extern int ww_unmap_all_category_list_of_filter_group(char* group_id, char* list_type);

/*
 * set reporting level of web access denies
 * @reporting_level
 */
extern int ww_set_reporting_level(int reporting_level);

/*
 * set language
 * @lang
 */
extern int ww_set_language(char* lang);

/*
 * set log level
 * @log_level
 */
extern int ww_set_log_level(int log_level);

/*
 * set log exception hits
 * @log_exception_hits
 */
extern int ww_set_log_exception_hits(char* log_exception_hits);

/*
 * set log file format
 * @format
 */
extern int ww_set_log_file_format(int format);

/*
 * set maximum log item length
 * @len
 */
extern int ww_set_max_log_item_len(int len);

/*
 * set anoymous log
 * @anoymous_log
 */
extern int ww_set_anoymous_log(char* anoymous_log);

/*
 * set ip and port of webwall
 * @ip
 * @port
 */
extern int ww_set_ip_and_port_of_webwall(char* ip, int port);

/*
 * set ip and port of proxy
 * @ip
 * @port
 */
extern int ww_set_ip_and_port_of_proxy(char* ip, int port);

/*
 * set the access denied address
 * @addr
 */
extern int ww_set_access_denied_addr(char* addr);

/*
 * set filter group length
 * @len
 */
extern int ww_set_filter_group_len(int len);

/*
 * set show weighted found
 * @show_weighted_found
 */
extern int ww_set_show_weighted_found(char* show_weighted_found);

/*
 * set weighted phrase mode
 * @mode
 */
extern int ww_set_weighted_phrase_mode(int  mode);

/*
 * create auth plugins
 * @plugin_name
 */
extern int ww_create_auth_plugins(char* plugin_name);

/*
 * remove auth plugin
 * @plugin_name
 */
extern int ww_remove_auth_plugin(char* plugin_name);

/*
 * set the reverse address lookups
 * @reverse_addr_lookups
 */
extern int ww_reverse_addr_lookups(char* reverse_addr_lookups);

/*
 * set the reverse clientip lookups
 * @reverse_clientip_lookups
 */
extern int ww_reverse_clientip_lookups(char* reverse_clientip_lookups);

/*
 * set the reverse client hostname
 * @reverse_client_hostname
 */
extern int ww_reverse_client_hostname(char* reverse_client_hostname);

/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 * @ip
 * @port
 */
extern int ww_set_proxy_addr(char* ip, int port);

/*
 * search for the category list by list name
 */
extern ww_category_list_t* ww_get_category_list(char* list_name);

extern ww_filter_group_policy_t* ww_get_filter_group_policy(int gid);
extern int validate_group_id(int gid);
extern int ww_get_group_id(char* group_id);
extern  char* get_list_type_name(int type);
extern  char* get_entry_type_name(int type);
extern  int validate_entry_type(char* entry_type);
extern  int validate_group_mode(char* group_mode);
extern  int validate_list_type(char* list_type);
extern  char* get_group_mode_name(int type);
extern  int validate_action(char* name);
extern  char* get_action(int type);
extern  int validate_language(char* name);
extern int validate_ip_and_ip_range(char* ip);
extern int validate_engine(char* name);
extern char* get_engine_name(int type);
extern int validate_method(char* name);
/*
 * save webwall config to file
 */
extern int ww_save_webwall_data_to_file();

/* 
 *save proxy config to file 
 */
extern int ww_save_proxy_data_to_file();
/* 
 * create group 
 */
extern int ww_create_group(int gid);
/* 
 * remove group 
 */
extern int ww_remove_group(int gid);

/* 
 *clear config
 */
extern int ww_clear_config(void);

/*
 * set sock file of av engine
 * @type
 * @path
 */
extern int ww_set_av_sock(int type, char* path);

/*
 * set naughtyness limit
 * @gid
 * @naughtyness
 */
extern int ww_set_naughtyness_limit(int gid, int naughtyness);

/*
 * add AV engine
 * @plugin_name
 */
extern int ww_create_av_plugin(char* plugin_name);

/*
 * remove AV engine
 * @plugin_name
 */
extern int ww_remove_av_plugin(char* plugin_name);

/*
 * list clone
 * @src_list
 * @dst_list
 */
extern int ww_list_clone(char* src_list, char* dst_list);


#endif

