/*
 * Webwall module CLI header
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_cli.h
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#ifndef WEBWALL_CLI_H
#define WEBWALL_CLI_H

#include "CliUtil/CliUtil.h"

/* 
 * create category list
 */
extern int ww_cli_create_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove category list
 */
extern int ww_cli_remove_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set time of category list
 */
extern int ww_cli_set_time(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove time of category list
 */
extern int ww_cli_remove_time(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create alias of category list
 */
extern int ww_cli_create_alias(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove alias of category list
 */
extern int ww_cli_remove_alias(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create single entry
 */
extern int ww_cli_create_single_entry(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove single entry
 */
extern int ww_cli_remove_single_entry(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create include entry file
 */
extern int ww_cli_create_include_entry_file(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove include entry file
 */
extern int ww_cli_remove_include_entry_file(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show category list
 */
extern int ww_cli_show_category_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create named user
 */
extern int ww_cli_create_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove named user
 */
extern int ww_cli_remove_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show named user
 */
extern int ww_cli_show_named_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove all named users
 */
extern int ww_cli_remove_all_named_users(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create ip based user
 */
extern int ww_cli_create_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove ip based user
 */
extern int ww_cli_remove_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show ip based user
 */
extern int ww_cli_show_ip_user(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove all ip based user
 */
extern int ww_cli_remove_all_ip_users(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set filter group mode
 */
extern int ww_cli_set_filter_group_mode(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set filter group name
 */
extern int ww_cli_set_filter_group_name(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * map category list to filter group
 */
extern int ww_cli_map_category_list_to_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * unmap category list of filter group
 */
extern int ww_cli_unmap_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * unmap all category list of filter group
 */
extern int ww_cli_unmap_all_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show category list of filter group
 */
extern int ww_cli_show_category_list_of_filter_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set reporting level of web access denies
 */
extern int ww_cli_set_reporting_level(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set language
 */
extern int ww_cli_set_language(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set log level
 */
extern int ww_cli_set_log_level(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set log exception hits
 */
extern int ww_cli_set_log_exception_hits(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set log file format
 */
extern int ww_cli_set_log_file_format(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set maximum log item length
 */
extern int ww_cli_set_max_log_item_len(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set anoymous log
 */
extern int ww_cli_set_anoymous_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set ip and port of webwall
 */
extern int ww_cli_set_ip_and_port_of_webwall(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set ip and port of proxy
 */
extern int ww_cli_set_ip_and_port_of_proxy(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set the access denied address
 */
extern int ww_cli_set_access_denied_addr(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set filter group length
 */
extern int ww_cli_set_filter_group_len(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set show weighted found
 */
extern int ww_cli_set_show_weighted_found(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set weighted phrase mode
 */
extern int ww_cli_set_weighted_phrase_mode(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create auth plugins
 */
extern int ww_cli_create_auth_plugins(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove auth plugin
 */
extern int ww_cli_remove_auth_plugin(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set the reverse address lookups
 */
extern int ww_cli_reverse_addr_lookups(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set the reverse clientip lookups
 */
extern int ww_cli_reverse_clientip_lookups(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set the reverse client hostname
 */
extern int ww_cli_reverse_client_hostname(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 */
extern int ww_cli_set_proxy_addr(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set proxy action
 */
extern int ww_cli_set_proxy_action(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show proxy configuration
 */
extern int ww_cli_show_proxy_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * create group
 */
extern int ww_cli_create_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove group
 */
extern int ww_cli_remove_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove group
 */
extern int ww_cli_show_group(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);


/*
 * save config
 */
extern char* ww_cli_save_config(void);

/*
 * load config
 */
extern int ww_cli_load_config();

/*
 * clear config
 */
extern int ww_cli_clear_config(void);

/*
 * set webwall action
 */
extern int ww_cli_set_webwall_action(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set naughtyness limit
 */
extern int ww_cli_set_naughtyness_limit(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * set sock file of av engine
 */
extern int ww_cli_set_av_sock(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show log
 */
extern int ww_cli_show_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * search log
 */
extern int ww_cli_search_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * clear log
 */
extern int ww_cli_clear_log(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show stat
 */
extern int ww_cli_show_stat(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show global config in webwall
 */
extern int ww_cli_show_config(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * add av engine plugin
 */
extern int ww_cli_create_av_engine(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * remove av engine plugin
 */
extern int ww_cli_remove_av_engine(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * list clone
 */
extern int ww_cli_clone_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show all list
 */
extern int ww_cli_show_all_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * show all list
 */
extern int ww_cli_import_file_list(char *data, unsigned int *length, struct aosUserLandApiParms *parms, char *errmsg, const int errlen);

/*
 * register webwall cli
 */
extern int ww_cli_register(void);

/*
 * unregister webwall cli
 */
extern int ww_cli_unregister(void);
#endif
