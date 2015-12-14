#ifndef AOS_RHC_WEBWALL_API_H
#define AOS_RHC_WEBWALL_API_H

//#define eAosRc_WebwallStart 112000

/* status code */
enum 
{
	eAosRc_WWSuccess = 0,
	eAosRc_WWUnsupportListType = 112000 + 1,
	eAosRc_WWUnsupportEntryType,
	eAosRc_WWUnsupportGroupModeType,
	eAosRc_WWUnsupportActionType,
	eAosRc_WWUnsupportLanguageType,
	eAosRc_WWUnsupportEngineType,
	eAosRc_WWUnmatchIncludeEntry,
	eAosRc_WWNoexistListName,
	eAosRc_WWExistListName,
	eAosRc_WWExistNamedUser,
	eAosRc_WWExistPlugin,
	eAosRc_WWNoexistPlugin,
	eAosRc_WWNoexistNamedUser,
	eAosRc_WWExistIPUser,
	eAosRc_WWNoexistIPUser,
	eAosRc_WWExistIncludeEntryFile,
	eAosRc_WWNoexistIncludeEntryFile,
	eAosRc_WWNoexistAaliasName,
	eAosRc_WWExistEntry,
	eAosRc_WWNoexistEntry,
	eAosRc_WWInUse,
	eAosRc_WWNoTime,
	eAosRc_WWCanNotRemove,
	eAosRc_WWTimeFormat,
	eAosRc_WWIPFormat,
	eAosRc_WWPortFormat,
	eAosRc_WWInvalidOperation,
	eAosRc_WWNotInRange,
	eAosRc_WWExistGroup,
	eAosRc_WWNoexistGroup,
	eAosRc_WWAlreadyRun,	
	eAosRc_WWAlreadyStop,
	eAosRc_WWFailAction,
	eAosRc_WWInvalidLogLevel,
	eAosRc_WWInvalidLogFileFormat,
	eAosRc_WWInvalidSwitch,
	eAosRc_WWInvalidLogItemLength,
	eAosRc_WWInvalidReportingLevel,
	eAosRc_WWInvalidGroupLength,
	eAosRc_WWInvalidWeightedPhraseMode,
	eAosRc_WWInvalidPlugin,
	eAosRc_WWInvalidGroupID,
	eAosRc_WWInvalidParameter,
	eAosRc_WWInvalidPath,
	eAosRc_WWError,
	eAosRc_WWUnsupportMethodType,
	eAosRc_WWImportFile,
	eAosRc_WWInvalidIncludeEntryFile,
};

/*
 * set proxy ip address and port
 * @ip
 * @port
 */
extern int aos_ww_api_set_proxy_addr(char* ip, int port);

/*
 * start/stop proxy
 * @action
 */
extern int aos_ww_api_set_proxy_action(char *action);

/*
 * show configuration of proxy
 * @result
 */
extern int aos_ww_api_proxy_retrieve_config(char *result, int *resultlen);

/*
 * set the access denied address
 * @url
 */
extern int aos_ww_api_set_access_denied_addr(char *url);

/*
 * set the language
 * @lang
 */
extern int aos_ww_cli_set_language(char *lang);

/*
 * set the log level
 * @level
 */
extern int aos_ww_api_set_log_level(int level);

/*
 * set the log exception hits
 * @switch
 */
extern int aos_ww_api_set_log_exception_hits(char* onoff);

/*
 * set the format of log file
 * @format
 */
extern int aos_ww_api_set_log_file_format(int format); 

/*
 * set the maximum length of log item
 * @len
 */
extern int aos_ww_api_set_max_log_item_len(int len);
/*
 * set the anonymous log
 * @onoff
 */
extern int aos_ww_api_set_anonymous_log(char *onoff);

/*
 * set the ip address and port of webwall
 * @ip
 * @port
 */
extern int aos_ww_api_set_ip_and_port_of_webwall(char *ip, int port); 

/*
 * set the reporting level
 * @level
 */
extern int aos_ww_api_set_reporting_level(int level);

/*
 * set the length of filter groups
 * @len
 */
extern int aos_ww_api_set_filter_group_len(int len);

/*
 * set the show weighted found
 * @onoff
 */
extern int aos_ww_api_set_show_weighted_found(char *onoff);

/*
 * set the weighted phrase mode
 * @mode
 */
extern int aos_ww_api_set_weighted_phrase_mode(int mode);

/*
 * create the authority plugin
 * @pluginname
 */
extern int aos_ww_api_create_auth_plugins(char *pluginname);

/*
 * remove authority plugin
 * @pluginname
 */
extern int aos_ww_api_remove_auth_plugin(char *pluginname);

/*
 * set the reverse address lookups
 * @onoff
 */
extern int aos_ww_api_reverse_addr_lookups(char *onoff);

/*
 * set the reverse client hostname
 * @onoff
 */
extern int aos_ww_api_reverse_client_hostname(char *onoff);

/*
 * set the reverse clientip lookups
 * @onoff
 */
extern int aos_ww_api_reverse_clientip_lookups(char *onoff);

/*
 * create a named user
 * @group_id
 * @user_name
 */
extern int aos_ww_api_create_named_user(char *group_id, char* user_name);

/*
 * remove a named user
 * @group_id
 * @user_name
 */
extern int aos_ww_api_remove_named_user(char *group_id, char *user_name);

/*
 * show information of named user
 * @group_id
 * @result
 */
extern int aos_ww_api_retrieve_named_user(char *group_id, char *result, int *resultlen);

/*
 * remove all named users of a group
 * @group_id
 */
extern int aos_ww_api_remove_all_named_users(char *group_id);

/*
 * create ip-based user
 * @group_id
 * @ip
 */
extern int aos_ww_api_create_ip_user(char *group_id, char *ip);

/*
 * remove a ip-based user
 * @group_id
 * @ip
 */
extern int aos_ww_api_remove_ip_user(char *group_id, char *ip);

/*
 * show all ip-based user of a group
 * @group_id
 * @result
 */
extern int aos_ww_api_retrieve_ip_user(char *group_id, char *result, int *resultlen);

/*
 * remove all ip-based user of a group
 * @group_id
 */
extern int aos_ww_api_remove_all_ip_users(char *group_id);

/*
 * set the mode of filter group
 * @group_id
 * @mode_name
 */
extern int aos_ww_api_set_filter_group_mode(char *group_id, char *mode_name);

/*
 * set the name of filter group
 * @group_id
 * @group_name
 */
extern int aos_ww_api_set_filter_group_name(char *group_id, char *group_name);

/*
 * show information of group policy
 * @group_id
 * @result
 */
extern int aos_ww_api_retrieve_category_list_of_filter_group(char *group_id, char *result, int *resultlen);

/*
 * create a filter group
 * @group_id
 */
extern int aos_ww_api_create_group(char *group_id);

/*
 * remove a filter group
 * @group_id
 */
extern int aos_ww_api_remove_group(char *group_id);

/*
 * create a category list
 * @list_type_name
 * @list_name
 */
extern int aos_ww_api_create_category_list(char *list_type_name, char *list_name);

/*
 * remove a category list
 * @list_name
 */
extern int aos_ww_api_remove_category_list(char *list_name);

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 * */
extern int aos_ww_api_set_time(char *list_name, char *start_time, char *end_time, char *days);

/*
 * remove time of a category list
 * @list_name
 */
extern int aos_ww_api_remove_time(char *list_name);

/*
 * create alias of a category list
 * @list_name
 * @alias_name
 */
extern int aos_ww_api_create_alias(char *list_name, char *alias_name);

/*
 * remove alias of a category list
 * @list_name
 */
extern int aos_ww_api_remove_alias(char *list_name);

/*
 * create a single entry
 * @list_name
 * @entry
 */
extern int aos_ww_api_create_single_entry(char *list_name, char *entry);

/*
 * remove a single entry
 * @list_name
 * @entry
 */
extern int aos_ww_api_remove_single_entry(char *list_name, char *entry);

/*
 * create an include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 */
extern int aos_ww_api_create_include_entry_file(char *list_name, char *entry_type, char* entry_list_name);

/*
 * remove an include entry file
 * @list_name
 * @entry_file_name
 */
extern int aos_ww_api_remove_include_entry_file(char *list_name, char *entry_file_name); 

/*
 * show information of a category list
 * @list_name
 * @result
 */
extern int aos_ww_api_retrieve_category_list(char *list_name, char *result, int *resultlen);

/*
 * set the limit of naughtyness
 * @group_id
 * @limit
 */
extern int aos_ww_api_set_naughtyness_limit(char *group_id, int limit);  

/*
 * set the sock file of av engine
 * @engine_name
 * @path
 */
extern int aos_ww_api_set_av_sock(char *engine_name, char *path);

/*
 * show log
 * @len
 * @result
 * @resultlen
 */
extern int aos_ww_api_retrieve_log(int len, char *result, int *resultlen);

/*
 * search log by keywords
 * @start_time
 * @end_time
 * @keywords
 */
extern int aos_ww_api_search_log(char *start_time, char *end_time, char *keywords, char *result, int *resultlen);

/*
 * clear log
 */
extern int aos_ww_api_clear_log();

/*
 * show statistics
 * @result
 * @resultlen
 */
extern int aos_ww_api_retrieve_stat(char *result, int *resultlen);

/*
 * show global configuration
 * @result
 * @resultlen
 */
extern int aos_ww_api_retrieve_config(char *result, int *resultlen);

/*
 * create anti-virus engine
 * @plugin_name
 */
extern int aos_ww_api_create_av_engine(char *plugin_name);

/*
 * remove anti-virus engine
 * @plugin_name
 */
extern int aos_ww_api_remove_av_engine(char *plugin_name);

/* 
 * clone list
 * @src
 * @dst
 */
extern int aos_ww_api_clone_list(char *src, char *dst);

/*
 * show list configuration
 * @result
 * @resultlen
 */
extern int aos_ww_api_retrieve_all_list(char *result, int *resultlen);

#endif
