/*
 * Webwall module global header
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_common.h
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#ifndef WEBWALL_COMMON_H
#define WEBWALL_COMMON_H

/*#include <linux/types.h>*/
#include "aosUtil/Types.h"
#include "aosUtil/List.h"
#include "aos/aosReturnCode.h"


/* dansguardian configuraton information */
#ifdef WEBWALL_DEUBG

#define WEBWALL_ROOT "/usr/local/etc/dansguardian/"
#define WEBWALL_GLOBAL_CONFIG "/usr/local/etc/dansguardian/dansguardian.conf"
#define WEBWALL_GROUP_PREFIX "dansguardian"
#define WEBWALL_PLUGIN_DIR "/usr/local/etc/dansguardian/authplugins/"
#define WEBWALL_LIST_DIR "/usr/local/etc/dansguardian/lists/"
#define WEBWALL_NAMED_USER_PATH "/usr/local/etc/dansguardian/lists/filtergroupslist"
#define WEBWALL_IP_USER_PATH "/usr/local/etc/dansguardian/lists/authplugins/ipgroups"
#define TINY_PROXY_GLOBAL_CONFIG "/usr/local/etc/tinyproxy/tinyproxy.conf"
#define DANSGUARDIAN_EXE_PATH "/usr/local/sbin/dansguardian"
#define TINY_PROXY_EXE_PATH "/usr/local/sbin/tinyproxy"
#define LOG_LOCATION "/usr/local/var/log/dansguardian/access.log"
#define STAT_LOCATION "/usr/local/var/log/dansguardian/stats"
#define TINY_PROXY_PREFIX "tinyproxy"

#else

#define WEBWALL_ROOT "/usr/local/etc/dansguardian/"
#define WEBWALL_GLOBAL_CONFIG "/usr/local/etc/dansguardian/dansguardian.conf"
#define WEBWALL_GROUP_PREFIX "dansguardian"
#define WEBWALL_PLUGIN_DIR "/usr/local/etc/dansguardian/authplugins/"
#define WEBWALL_LIST_DIR "/usr/local/etc/dansguardian/lists/"
#define WEBWALL_NAMED_USER_PATH "/usr/local/etc/dansguardian/lists/filtergroupslist"
#define WEBWALL_IP_USER_PATH "/usr/local/etc/dansguardian/lists/authplugins/ipgroups"
#define TINY_PROXY_GLOBAL_CONFIG "/usr/local/etc/tinyproxy/tinyproxy.conf"
#define DANSGUARDIAN_EXE_PATH "/usr/local/sbin/dansguardian"
#define TINY_PROXY_EXE_PATH "/usr/local/sbin/tinyproxy"
#define LOG_LOCATION "/usr/local/var/log/dansguardian/access.log"
#define STAT_LOCATION "/usr/local/var/log/dansguardian/stats"

#define TINY_PROXY_PREFIX "tinyproxy"

#endif

#define DEFAULT_LOG_ITEM 10
#define MAX_LOG_ITEM 50
#define MAX_SHOW_LEN 10240

/* category list type */
enum 
{
	T_WW_BANNED_SITE_LIST = 100,
	T_WW_BANNED_URL_LIST,
	T_WW_BANNED_IP_LIST,
	T_WW_EXCEPTION_IP_LIST,
	T_WW_EXCEPTION_SITE_LIST,
	T_WW_EXCEPTION_URL_LIST,
	T_WW_GREY_SITE_LIST,
	T_WW_GREY_URL_LIST,
	T_WW_EXCEPTION_REGEXP_URL_LIST,
	T_WW_BANNED_REGEXP_URL_LIST,
	T_WW_REGEXP_URL_LIST,
	T_WW_BANNED_PHRASE_LIST,
	T_WW_WEIGHTED_PHRASE_LIST,
	T_WW_EXCEPTION_PHRASE_LIST,
	T_WW_PICS_FILE,
	T_WW_CONTENT_REGEXP_LIST,
	T_WW_BANNED_EXTENSION_LIST,
	T_WW_BANNED_MIME_TYPE_LIST,
	T_WW_EXCEPTION_FILE_SITE_LIST,
	T_WW_EXCEPTION_VIRUS_EXTENSION_LIST,
	T_WW_EXCEPTION_VIRUS_MIMETYPE_LIST,
	T_WW_EXCEPTION_VIRUS_SITE_LIST,
	T_WW_EXCEPTION_VIRUS_URL_LIST,
};

/* list category */
enum 
{
	T_WW_ENTRY_TYPE_DOMAIN = 1,
	T_WW_ENTRY_TYPE_URL,
	T_WW_ENTRY_TYPE_PHRASE,
	T_WW_ENTRY_TYPE_NORMAL,
};


/* status code */
enum 
{
	eAosRc_WWSuccess = 0,
	eAosRc_WWUnsupportListType = eAosRc_WebwallStart + 1,
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

/* import type */
enum 
{
	T_WEBWALL_IMPORT_TYPE_LOCAL,
	T_WEBWALL_IMPORT_TYPE_FTP,
	T_WEBWALL_IMPORT_TYPE_HTTP,
};

/* time control */
#define MAX_DAYS_OF_WEEK 7
typedef struct ww_time_control 
{
	int start_hour;
	int start_minute;
	int end_hour;
	int end_minute;
	char days[MAX_DAYS_OF_WEEK];
} ww_time_control_t;

#define MAX_ENTRY_LEN 512
typedef struct ww_entry 
{
	struct aos_list_head list;
	char data[MAX_ENTRY_LEN];
	int type;
} ww_entry_t;

/* category list */
#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 256
#define MAX_IP_LEN 56
#define MAX_BUFF_LEN 256
#define MAX_SWITCH_LEN 8
typedef struct ww_category_list {
	struct aos_list_head list;
	struct aos_list_head include_list_head;/* don't use */
	ww_time_control_t *tc;
	char *category_alias;
	int list_type;
	struct aos_list_head entry_list_head; 
	struct aos_list_head guard_entry_file_list_head;
	char path[MAX_PATH_LEN];
	int ref_cout;
	int is_system;
} ww_category_list_t;
extern struct aos_list_head category_list_head;
extern void category_list_head_lock(void);
extern void category_list_head_unlock(void);

/* filter user */
typedef struct	ww_filter_name_based_user 
{
	struct aos_list_head list;
	char user_name[MAX_NAME_LEN];
	int group_id;
} ww_filter_name_based_user_t;

typedef struct ww_filter_ip_based_user 
{
	struct aos_list_head list;
	int group_id;
	char ip[MAX_IP_LEN];
} ww_filter_ip_based_user_t;

extern struct aos_list_head filter_name_based_user_head;
extern void filter_name_based_user_head_lock(void);
extern void filter_name_based_user_head_unlock(void);

extern struct aos_list_head filter_ip_based_user_head;
extern void filter_ip_based_user_head_lock(void);
extern void filter_ip_based_user_head_unlock(void);

/* filter group policy */
#define WEBWALL_MAX_GROUP 9
enum 
{
	T_BANNED = 0,
	T_FILTERED = 1,
	T_UNFILTERED = 2,
};

enum 
{
	T_LOG_AND_DONT_BLOCK = -1,
	T_ACCESS_DENY,
	T_REPORT_BUT_DONT_DENY,
	T_REPORT_ALL,
	T_HTML_TEMPLATE,
};

typedef struct ww_filter_group_list 
{
	struct aos_list_head list;
	ww_category_list_t *category_list;
} ww_filter_group_list_t;

typedef struct ww_filter_group_policy 
{
	struct aos_list_head list;
	int group_mode;
	char group_name[MAX_NAME_LEN];
	int reporting_level;
	int group_id;
	int naughtyness_limit;
	int category_display_threshold;
	int embedded_url_weight;
	char enable_pics[MAX_SWITCH_LEN];
	int by_pass;
	char by_pass_key[MAX_ENTRY_LEN];
	int infection_by_pass;
	char infection_by_pass_key[MAX_ENTRY_LEN];
	char infection_by_pass_errors_only[MAX_SWITCH_LEN];
	char disable_content_scan[MAX_SWITCH_LEN];
	char deep_url_analysis[MAX_SWITCH_LEN];
	struct aos_list_head category_list_head;
} ww_filter_group_policy_t;

extern struct aos_list_head policy_head;
extern void policy_head_lock(void);
extern void policy_head_unlock(void);
/* global configuration */
enum 
{
	T_WEBWALL_LOG_FILE_DANSGUARDIAN = 1,
	T_WEBWALL_LOG_FILE_CSV,
	T_WEBWALL_LOG_FILE_SQUID,
	T_WEBWALL_LOG_FILE_TAB,
};
enum
{
	T_WEBWALL_PHRASE_MODE_OFF = 1,
	T_WEBWALL_PHRASE_MODE_ON_NORMAL,
	T_WEBWALL_PHRASE_MODE_ON_SINGULAR
};
enum 
{
	T_WEBWALL_LOG_NONE,
	T_WEBWALL_LOG_DENY,
	T_WEBWALL_LOG_ALL_TEXT,
	T_WEBWALL_LOG_ALL_REQUEST,
};
#define MAX_SWITCH_LEN 8
typedef struct ww_global 
{
	int report_level;
	char language_path[MAX_PATH_LEN];
	char language[MAX_NAME_LEN];
	int log_level;
	char log_exception_hits[MAX_SWITCH_LEN];
	int max_log_item_len;
	char anoymous_log_switch[MAX_SWITCH_LEN];
	char log_file_path[MAX_PATH_LEN];
	char stat_log_file_path[MAX_PATH_LEN];
	int log_file_format;
	char stats_file_path[MAX_PATH_LEN];
	char filter_ip_address[MAX_IP_LEN];
	int filter_port;
	char proxy_ip_address[MAX_IP_LEN];
	int proxy_port;
	char access_deny_address[MAX_PATH_LEN];
	char no_standard_delimiter[MAX_SWITCH_LEN];
	int use_custom_banned_image;
	char custom_banned_image_path[MAX_PATH_LEN];
	int filter_groups;
	char show_weighted_found[MAX_SWITCH_LEN];
	int weighted_phrase_mode;
	int url_cache_number;
	int url_cache_age;
	char scan_clean_cache[MAX_SWITCH_LEN];
	int phrase_filter_mode;
	int preserve_case;
	int hex_decode_content;
	int force_quick_search;
	struct aos_list_head auth_plugin_head;
	char filter_groups_list_path[MAX_PATH_LEN];
	char banned_ip_list_path[MAX_PATH_LEN];
	char exception_ip_list_path[MAX_PATH_LEN];
	char reverse_address_lookups[MAX_SWITCH_LEN];
	char reverse_clientip_lookups[MAX_SWITCH_LEN];
	char log_client_hostname[MAX_SWITCH_LEN];
	char create_list_cache_files[MAX_SWITCH_LEN];
	int max_upload_size;
	int max_content_filter_size;
	int max_content_ram_cache_scan_size;
	int max_content_file_cache_scan_size;
	char file_cache_dir[MAX_PATH_LEN];
	char delete_downloaded_temp_files[MAX_SWITCH_LEN];
	int initial_trickle_delay;
	int trickle_delay;
	struct aos_list_head download_manager_head;
	int content_scanner_timeout;
	char content_scan_exceptions[MAX_SWITCH_LEN];
	char recheck_replace_urls[MAX_SWITCH_LEN];
	char forwarded_for[MAX_SWITCH_LEN];
	char use_xforwarded_for[MAX_SWITCH_LEN];
	char log_connection_handling_errors[MAX_SWITCH_LEN];
	char log_child_process_handling[MAX_SWITCH_LEN];
	int max_children;
	int min_children;
	int min_spare_children;
	int prefork_children;
	int max_spare_children;
	int max_age_children;
	int max_ips;
	char no_daemon[MAX_SWITCH_LEN];
	char no_logger[MAX_SWITCH_LEN];
	char log_ad_blocks[MAX_SWITCH_LEN];
	char soft_restart[MAX_SWITCH_LEN];
	int webwall_status;
} ww_global_t;

extern ww_global_t global;

/* tiny proxy */
typedef struct ww_tiny_proxy
{
	char ip[MAX_IP_LEN];
	int port;
	char user_name[MAX_NAME_LEN];
	char group[MAX_NAME_LEN];
	int timeout;
	char default_error_file[MAX_PATH_LEN];
	char stat_file[MAX_PATH_LEN];
	char log_file[MAX_PATH_LEN];
	char log_level[MAX_NAME_LEN];
	char pid_file[MAX_PATH_LEN];
	int max_clients;
	int max_spare_servers;
	int min_spare_servers;
	int start_servers;
	int max_request_per_child;
	struct aos_list_head allow_head;
	char via_proxy_name[MAX_NAME_LEN];
	struct aos_list_head connect_port_head;
	int status;
} ww_tiny_proxy_t;

extern ww_tiny_proxy_t tiny_proxy;

enum
{
	T_WEBWALL_START,
	T_WEBWALL_STOP,
	T_WEBWALL_RELOAD,
};
/*
 * initalize webwall defalut setting
 */
extern int ww_init(void);

/* anti-virus engine */
typedef struct ww_av_engine
{
	struct aos_list_head list;
	char plugname[MAX_NAME_LEN];
	char file_name[MAX_PATH_LEN];
	char sockfile_location[MAX_PATH_LEN];
	ww_category_list_t* extension;
	ww_category_list_t* mimetype;
	ww_category_list_t* site;
	ww_category_list_t* url;
	int type;
} ww_av_engine_t;

extern struct aos_list_head av_engine_head;

enum
{
	T_VIRUS_CLAMAV = 1,
	T_VIRUS_KAV,
};

#endif
 
