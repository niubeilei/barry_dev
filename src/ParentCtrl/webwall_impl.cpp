/*
 * Webwall module implemenation
 * description: 
 * author	  :	<a href=mailto:xw_cn@163.com>bill xia</a>
 * directory  :	ParentCtrl/webwall_impl.cpp
 * date		  :	01/05/2007
 * history	  : created by bill xia, 01/05/2006
 */
#include "ParentCtrl/webwall_common.h"
#include "ParentCtrl/webwall_impl.h"
#include "aosUtil/List.h"
#include "aosUtil/Mutex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef WEBWALL_DEUBG
#include <syslog.h>
#endif

#define GROUP_PREFIX "group"

/* utilities */
typedef struct ww_map
{
	char name[MAX_NAME_LEN];
	int value;
} ww_map_t;

/* list type */
static ww_map_t list_types[] = {
	{"bannedsitelist", T_WW_BANNED_SITE_LIST},
	{"bannedurllist", T_WW_BANNED_URL_LIST},
	{"exceptionsitelist", T_WW_EXCEPTION_SITE_LIST},
	{"exceptionurllist", T_WW_EXCEPTION_URL_LIST},
	{"greysitelist", T_WW_GREY_SITE_LIST},
	{"greyurllist", T_WW_GREY_URL_LIST},
	{"exceptionregexpurllist", T_WW_EXCEPTION_REGEXP_URL_LIST},
	{"bannedregexpurllist", T_WW_BANNED_REGEXP_URL_LIST},
	{"urlregexplist", T_WW_REGEXP_URL_LIST},
	{"bannedphraselist", T_WW_BANNED_PHRASE_LIST}, 
	{"weightedphraselist", T_WW_WEIGHTED_PHRASE_LIST}, 
	{"exceptionphraselist", T_WW_EXCEPTION_PHRASE_LIST}, 
	{"picsfile", T_WW_PICS_FILE}, 
	{"contentregexplist", T_WW_CONTENT_REGEXP_LIST},  
	{"bannedextensionlist", T_WW_BANNED_EXTENSION_LIST},  
	{"bannedmimetypelist", T_WW_BANNED_MIME_TYPE_LIST},  
	{"exceptionfilesitelist", T_WW_EXCEPTION_FILE_SITE_LIST},
	{"bannediplist", T_WW_BANNED_IP_LIST},
	{"exceptioniplist", T_WW_EXCEPTION_IP_LIST},
	{"exceptionvirusextensionlist", T_WW_EXCEPTION_VIRUS_EXTENSION_LIST},
	{"exceptionvirusmimetypelist", T_WW_EXCEPTION_VIRUS_MIMETYPE_LIST},
	{"exceptionvirussitelist", T_WW_EXCEPTION_VIRUS_SITE_LIST},
	{"exceptionvirusurllist", T_WW_EXCEPTION_VIRUS_URL_LIST}
};
#define LIST_TYPES_LEN (int)(sizeof(list_types)/sizeof(ww_map_t))

/* entry type */
static ww_map_t entry_types[] = {
	{"blacklist_domains", T_WW_ENTRY_TYPE_DOMAIN},
	{"blacklist_urls", T_WW_ENTRY_TYPE_URL},
	{"phraselists", T_WW_ENTRY_TYPE_PHRASE},
	{"normal", T_WW_ENTRY_TYPE_NORMAL}
};
#define ENTRY_TYPES_LEN (int)(sizeof(entry_types)/sizeof(ww_map_t))

/* group mode */
static ww_map_t group_modes[] = {
	{"banned", T_BANNED},
	{"filtered", T_FILTERED},
	{"unfiltered", T_UNFILTERED}
};
#define GROUP_MODES_LEN (int)(sizeof(group_modes)/sizeof(ww_map_t))

/* actions */
static ww_map_t actions[] = {
	{"start", T_WEBWALL_START},
	{"stop", T_WEBWALL_STOP},
	{"reload", T_WEBWALL_RELOAD}
};
#define ACTIONS_LEN (int)(sizeof(actions)/sizeof(ww_map_t))

/* phrase mode */

/* language */
static ww_map_t languages[] = {
	{"bulgarian", 1},
	{"chinesebig5", 2},
	{"chinesegb2312", 3},
	{"czech", 4},
	{"danish", 5},
	{"dutch", 6},
	{"french", 7},
	{"german", 8},
	{"hebrew", 9},
	{"hungarian", 10},
	{"indonesian", 11},
	{"italian", 12},
	{"lithuanian", 13},
	{"polish", 14},
	{"portuguese", 15},
	{"ptbrazilian", 16},
	{"russian-1251", 17},
	{"russian-koi8-r", 18},
	{"slovak", 19},
	{"spanish", 20},
	{"swedish", 21},
	{"turkish", 22},
	{"ukenglish", 23}
};
#define LANGUAGES_LEN (int)(sizeof(languages)/sizeof(ww_map_t))

/* av engine */
static ww_map_t engines[] = {
	{"clamdscan", T_VIRUS_CLAMAV},
	{"kavdscan", T_VIRUS_KAV}
};
#define ENGINES_LEN (int)(sizeof(engines)/sizeof(ww_map_t))

/* transport method */
static ww_map_t methods[] = {
	{"local", T_WEBWALL_IMPORT_TYPE_LOCAL},
	{"ftp", T_WEBWALL_IMPORT_TYPE_FTP},
	{"http", T_WEBWALL_IMPORT_TYPE_HTTP}
};
#define METHODS_LEN (int)(sizeof(methods)/sizeof(ww_map_t))

/* global variable */
AOS_LIST_HEAD(category_list_head);
static AOS_DECLARE_MUTEX(category_list_head_sem);

AOS_LIST_HEAD(filter_name_based_user_head);
static AOS_DECLARE_MUTEX(filter_name_based_user_head_sem);

AOS_LIST_HEAD(filter_ip_based_user_head);
static AOS_DECLARE_MUTEX(filter_ip_based_user_head_sem);

AOS_LIST_HEAD(policy_head);
static AOS_DECLARE_MUTEX(policy_head_sem);

ww_global_t global;
static AOS_DECLARE_MUTEX(auth_plugin_head_sem);

ww_tiny_proxy_t tiny_proxy;

AOS_LIST_HEAD(av_engine_head);

void category_list_head_lock(void)
{
	 aos_lock(category_list_head_sem);
}

void category_list_head_unlock(void)
{
	aos_unlock(category_list_head_sem);
}

void filter_name_based_user_head_lock(void)
{
	aos_lock(filter_name_based_user_head_sem);
}

void filter_name_based_user_head_unlock(void)
{
	aos_unlock(filter_name_based_user_head_sem);
}

void filter_ip_based_user_head_lock(void)
{
	aos_lock(filter_ip_based_user_head_sem);
}

void filter_ip_based_user_head_unlock(void)
{
	aos_unlock(filter_ip_based_user_head_sem);
}

void policy_head_lock(void)
{
	aos_lock(policy_head_sem);
}

void policy_head_unlock(void)
{
	aos_unlock(policy_head_sem);
}

void auth_plugin_lock(void)
{
	aos_lock(auth_plugin_head_sem);
}

void auth_plugin_unlock(void)
{
	aos_unlock(auth_plugin_head_sem);		
}

int validate_list_type(char* list_type)
{
	int i;
	
	for (i = 0; i < LIST_TYPES_LEN; i++) {
		if (strcmp(list_type, list_types[i].name) == 0) {
			return list_types[i].value;
		}
	}
	
	return -eAosRc_WWUnsupportListType;
}

char* get_list_type_name(int type)
{
	int i;
	
	for (i = 0; i < LIST_TYPES_LEN; i++) {
		if (type == list_types[i].value) {
			return list_types[i].name;
		}
	}
	
	return NULL;
}

char* get_entry_type_name(int type)
{
	int i;
	
	for (i = 0; i < ENTRY_TYPES_LEN; i++) {
		if (type == entry_types[i].value) {
			return entry_types[i].name;
		}
	}
	
	return NULL;
}

int validate_entry_type(char* entry_type)
{
	int i;
	
	for (i = 0; i < ENTRY_TYPES_LEN; i++) {
		if (strcmp(entry_type, entry_types[i].name) == 0) {
			return entry_types[i].value;
		}
	}
	
	return -eAosRc_WWUnsupportEntryType;
}

int validate_group_mode(char* group_mode)
{
	int i;
	
	for (i = 0; i < GROUP_MODES_LEN; i++) {
		if (strcmp(group_mode, group_modes[i].name) == 0) {
			return group_modes[i].value;
		}
	}
	
	return -eAosRc_WWUnsupportGroupModeType;
}

char* get_group_mode_name(int type)
{
	int i;
	
	for (i = 0; i < GROUP_MODES_LEN; i++) {
		if (type == group_modes[i].value) {
			return group_modes[i].name;
		}
	}
	
	return NULL;
}

int validate_action(char* name)
{
	int i;
	
	for (i = 0; i < ACTIONS_LEN; i++) {
		if (strcmp(name, actions[i].name) == 0) {
			return actions[i].value;
		}
	}
	
	return -eAosRc_WWUnsupportActionType;
}

char* get_action(int type)
{
	int i;
	
	for (i = 0; i < ACTIONS_LEN; i++) {
		if (actions[i].value == type) {
			return actions[i].name;
		}
	}
	
	return NULL;
}

int validate_language(char* name)
{
	int i;
	
	for (i = 0; i < LANGUAGES_LEN; i++) {
		if (strcmp(name, languages[i].name) == 0) {
			return 0;
		}
	}
		
	return -eAosRc_WWUnsupportLanguageType;
}

int validate_engine(char* name)
{
	int i;
	
	for (i = 0; i < ENGINES_LEN; i++) {
		if (strcmp(name, engines[i].name) == 0) {
			return engines[i].value;
		}
	}
		
	return -eAosRc_WWUnsupportEngineType;
}
int validate_method(char* name)
{
	int i;
	
	for (i = 0; i < METHODS_LEN; i++) {
		if (strcmp(name, methods[i].name) == 0) {
			return methods[i].value;
		}
	}
		
	return -eAosRc_WWUnsupportMethodType;
}

char* get_engine_name(int type)
{
	int i;
	
	for (i = 0; i < ENGINES_LEN; i++) {
		if (engines[i].value == type) {
			return engines[i].name;
		}
	}
		
	return NULL;
}

int validate_ip_and_ip_range(char* ip)
{
	char* p;
	char* q;
	
	if (inet_addr(ip) != INADDR_NONE) 
		return 0;
	
	p = ip;
	q = strstr(p, "-");
	if (q == NULL)
		return -eAosRc_WWIPFormat;
	*q = '\0';
	if (inet_addr(p) == INADDR_NONE) 
		return -eAosRc_WWIPFormat;
	q++;
	if (inet_addr(q) == INADDR_NONE) 
		return -eAosRc_WWIPFormat;
	
	return 0;
}
/*
 * search for the category list by list name
 */
ww_category_list_t* ww_get_category_list(char* list_name)
{
	ww_category_list_t* node;
	ww_category_list_t* tmp;
	
	category_list_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &category_list_head, list) {
		if (strcmp(node->path, list_name) == 0) {
			category_list_head_unlock();
			return node;
		}
	}
	category_list_head_unlock();
	
	return NULL;
}

/*
 * create category list
 * @list_type
 * @list_name
 * @is_system
 */
int ww_create_category_list(char* list_type, char* list_name, int is_system)
{
	int type;
	ww_category_list_t *node;
	
	if ((type = validate_list_type(list_type)) == -eAosRc_WWUnsupportListType) {
		return -eAosRc_WWUnsupportListType;
	}
	
	if ((node = ww_get_category_list(list_name)) != NULL) {
		return -eAosRc_WWExistListName;
	}
	
	node = (ww_category_list_t*)malloc(sizeof(ww_category_list_t));
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	
	node->list_type = type;
	strcpy(node->path, list_name);
	node->tc = NULL;
	node->category_alias = NULL;
	node->ref_cout = 0;
	node->is_system = is_system;
	AOS_INIT_LIST_HEAD(&(node->include_list_head));
	AOS_INIT_LIST_HEAD(&(node->entry_list_head));
	AOS_INIT_LIST_HEAD(&(node->guard_entry_file_list_head));
	
	category_list_head_lock();
	aos_list_add_tail(&(node->list), &category_list_head);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove category list
 * @list_name
 */
int ww_remove_category_list(char* list_name)
{
	ww_category_list_t* node;
	ww_entry_t* entry;
	ww_entry_t* tmp_entry;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if ((node->ref_cout) > 0)
		return -eAosRc_WWInUse;
	if (node->is_system == 1)
		return -eAosRc_WWCanNotRemove;
		
	category_list_head_lock();
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

	aos_list_del(&(node->list));
	free(node);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * set time of category list
 * @list_name
 * @start_time
 * @end_time
 * @days
 */
int ww_set_time(char* list_name, char* start_time, char* end_time, char* days)
{
	ww_category_list_t* node;
	char buff[MAX_BUFF_LEN];
	char *p;
	int start_hour;
	int end_hour;
	int start_minute;
	int end_minute;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	
		
	/* start time */
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, start_time);
	p = strstr(buff, ":");
	if (p == NULL) {
		return -eAosRc_WWTimeFormat;
	}
		
	*p = '\0';
	p++;
	start_hour = atoi(buff);
	if (start_hour < 0 || start_hour > 23) {
		return -eAosRc_WWTimeFormat;
	}
	start_minute = atoi(p);
	if (start_minute < 0 || start_minute > 59) {
		return -eAosRc_WWTimeFormat;
	}
	/* end time */
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, end_time);
	p = strstr(buff, ":");
	if (p == NULL) {
		return -eAosRc_WWTimeFormat;
	}
	
	*p = '\0';
	p++;
	end_hour = atoi(buff);
	if (end_hour < 0 || end_hour > 23) {
		return -eAosRc_WWTimeFormat;
	}
	end_minute = atoi(p);
	if (end_minute < 0 || end_minute > 59) {
		return -eAosRc_WWTimeFormat;
	}
	/* validate the time */
	if ((start_hour > end_hour)
	 	|| ((start_minute > end_minute) && (start_hour == end_hour))) 
	 	return -eAosRc_WWTimeFormat;
	 	
	/* days */
	if (strlen(days) > 7)
		return -eAosRc_WWTimeFormat;
		
	p = days;
	while (*p) {
		if (*p < '0' || *p > '6') {
			return -eAosRc_WWTimeFormat;
		}
		p++;
	}
	
	if (node->tc == NULL) {
		node->tc = (ww_time_control_t*)malloc(sizeof(ww_time_control_t));
		if (node->tc == NULL) {
			return -eAosRc_WWError;
		}
	} 

	node->tc->start_hour = start_hour;
	node->tc->end_hour = end_hour;
	node->tc->start_minute = start_minute;
	node->tc->end_minute = end_minute;
	strcpy(node->tc->days, days);
	
	return -eAosRc_WWSuccess;
}

/*
 * remove time of category list
 * @list_name
 */
int ww_remove_time(char* list_name)
{
	ww_category_list_t* node;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if (node->tc == NULL) {
		return -eAosRc_WWNoTime;
	}
	
	free(node->tc);
	node->tc = NULL;
	
	return -eAosRc_WWSuccess;
}

/*
 * create alias of category list
 * @list_name
 * @alias_name
 */
int ww_create_alias(char* list_name, char* alias_name)
{
	ww_category_list_t* node;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if (node->category_alias == NULL) {
		node->category_alias = (char*)malloc(MAX_NAME_LEN);
		if (node->category_alias == NULL) {
			return -eAosRc_WWError;
		}
	}
	
	strcpy(node->category_alias, alias_name);
	
	return -eAosRc_WWSuccess;
}

/*
 * remove alias of category list
 * @list_name
 */
int ww_remove_alias(char* list_name)
{
	ww_category_list_t* node;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if (node->category_alias == NULL) {
		return -eAosRc_WWNoexistAaliasName;
	}
	
	free(node->category_alias);
	node->category_alias = NULL;
	
	return -eAosRc_WWSuccess;
}

static ww_entry_t* ww_get_single_entry(struct aos_list_head* head, char* data)
{
	ww_entry_t* node;
	ww_entry_t* tmp;
	
	category_list_head_lock();
	aos_list_for_each_entry_safe(node, tmp, head, list) {
		if (strcmp(node->data,data) == 0) {
			category_list_head_unlock();
			return node;
		}
	}
	category_list_head_unlock();
	
	return NULL;
}
/*
 * create single entry
 * @list_name
 * @entry
 */
int ww_create_single_entry(char* list_name, char* data)
{
	ww_category_list_t* node;
	ww_entry_t* entry;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if ((entry = ww_get_single_entry(&node->entry_list_head, data)) != NULL) {
		return -eAosRc_WWExistEntry;
	}
	
	/* validate format if ip address and range*/
	switch (node->list_type) {
		case T_WW_EXCEPTION_IP_LIST:
			if (validate_ip_and_ip_range(data) < 0)
				return -eAosRc_WWIPFormat;
			break;
		case T_WW_BANNED_IP_LIST:
			if (validate_ip_and_ip_range(data) < 0)
				return -eAosRc_WWIPFormat;
			break;
	}
	
	entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (entry == NULL) {
		return -eAosRc_WWError;
	}
	strcpy(entry->data, data);
	
	category_list_head_lock();
	aos_list_add_tail(&entry->list, &node->entry_list_head);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove single entry
 * @list_name
 * @entry
 */
int ww_remove_single_entry(char* list_name, char* data)
{
	ww_category_list_t* node;
	ww_entry_t* entry;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	if ((entry = ww_get_single_entry(&node->entry_list_head, data)) == NULL) {
		return -eAosRc_WWNoexistEntry;
	}
	
	category_list_head_lock();
	aos_list_del(&entry->list);
	free(entry);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

static ww_entry_t* ww_get_include_entry_file(struct aos_list_head* head, char* entry_list_name)
{
	ww_entry_t* node;
	ww_entry_t* tmp;
	
	aos_list_for_each_entry_safe(node, tmp, head, list) {
		if (strcmp(node->data, entry_list_name) == 0) {
			return node;
		}
	}
	
	return NULL;
}
/*
 * create include entry file
 * @list_name
 * @entry_type
 * @entry_list_name
 */
int ww_create_include_entry_file(char* list_name, char* entry_type, char* entry_list_name)
{
	ww_category_list_t* node;
	ww_entry_t* entry;
	int type;
	char path[MAX_BUFF_LEN];
	struct stat s;

	memset(path, 0, MAX_BUFF_LEN);
	if ((type = validate_entry_type(entry_type)) == -eAosRc_WWUnsupportEntryType) {
		return -eAosRc_WWUnsupportEntryType;
	}	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	/*
	 * list type checking 
	 */ 
	if (ww_get_include_entry_file(&(node->guard_entry_file_list_head), 
									entry_list_name) != NULL) {
		return -eAosRc_WWExistIncludeEntryFile;
	}
	if (ww_get_include_entry_file(&(node->include_list_head), 
									entry_list_name) != NULL) {
		return -eAosRc_WWExistIncludeEntryFile;
	}
	
	
	/* 
	 * validate the relation between entry type and list type
	 */
	switch (type) {
		case T_WW_ENTRY_TYPE_DOMAIN:
			if ((node->list_type != T_WW_BANNED_SITE_LIST) 
				&& (node->list_type != T_WW_EXCEPTION_SITE_LIST)
				&& (node->list_type != T_WW_GREY_SITE_LIST))
				return -eAosRc_WWUnmatchIncludeEntry;  
			sprintf(path, "%sblacklists/%s/domains", WEBWALL_LIST_DIR, entry_list_name);
			if ((stat(path, &s) < 0) || (!S_ISREG(s.st_mode))) 
				return -eAosRc_WWInvalidIncludeEntryFile;
			break;
		case T_WW_ENTRY_TYPE_URL:
			if ((node->list_type != T_WW_BANNED_URL_LIST) 
				&& (node->list_type != T_WW_EXCEPTION_URL_LIST)
				&& (node->list_type != T_WW_GREY_URL_LIST))
				return -eAosRc_WWUnmatchIncludeEntry;
			sprintf(path, "%sblacklists/%s/urls", WEBWALL_LIST_DIR, entry_list_name);
			if ((stat(path, &s) < 0) || (!S_ISREG(s.st_mode)))
				return -eAosRc_WWInvalidIncludeEntryFile;
			break;
		case T_WW_ENTRY_TYPE_PHRASE:
			if ((node->list_type != T_WW_BANNED_PHRASE_LIST) 
				&& (node->list_type != T_WW_EXCEPTION_PHRASE_LIST)
				&& (node->list_type != T_WW_WEIGHTED_PHRASE_LIST))
				return -eAosRc_WWUnmatchIncludeEntry;
			sprintf(path, "%sphraselists/%s", WEBWALL_LIST_DIR, entry_list_name);
			if ((stat(path, &s) < 0) || (!S_ISREG(s.st_mode)))
				return -eAosRc_WWInvalidIncludeEntryFile;
			break;
		case T_WW_ENTRY_TYPE_NORMAL:
			if (ww_get_category_list(entry_list_name) == NULL) {
				sprintf(path, "%s%s", WEBWALL_LIST_DIR, entry_list_name);
				if ((stat(path, &s) < 0) || (!S_ISREG(s.st_mode)))
					return -eAosRc_WWInvalidIncludeEntryFile;
			}
	}
	
	entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (entry == NULL) {
		return -eAosRc_WWError;
	}
	
	entry->type = type;
	strcpy(entry->data, entry_list_name);
	
	category_list_head_lock();
	if (type == T_WW_ENTRY_TYPE_NORMAL)
		aos_list_add_tail(&entry->list, &node->include_list_head);
	else
		aos_list_add_tail(&entry->list, &node->guard_entry_file_list_head);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove include entry file
 * @list_name
 * @entry_list_name
 */
int ww_remove_include_entry_file(char* list_name, char* entry_list_name)
{
	ww_category_list_t* node;
	ww_entry_t* entry;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	
	if ((entry = ww_get_include_entry_file(&(node->guard_entry_file_list_head), entry_list_name)) == NULL) {
		if ((entry = ww_get_include_entry_file(&(node->include_list_head), entry_list_name)) == NULL)
			return -eAosRc_WWNoexistIncludeEntryFile;
	}
	
	category_list_head_lock();
	aos_list_del(&entry->list);
	free(entry);
	category_list_head_unlock();
	
	return -eAosRc_WWSuccess;
}

static ww_filter_name_based_user_t* ww_get_named_user(int group_id, char* user_name)
{
	ww_filter_name_based_user_t* node;
	ww_filter_name_based_user_t* tmp;
	
	filter_name_based_user_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &filter_name_based_user_head, list) {
		if (node->group_id == group_id && strcmp(node->user_name, user_name) == 0) {
			filter_name_based_user_head_unlock();
			return node;
		}
	}
	filter_name_based_user_head_unlock();
	
	return NULL;
}
int ww_get_group_id(char* group_id)
{
	char* p;
	char* q;
	
	p = group_id;
	q = strstr(group_id, GROUP_PREFIX);	
	if (q == NULL)
		return 0;

	p += strlen(GROUP_PREFIX);
	
	return atoi(p);
}
int validate_group_id(int gid)
{
	ww_filter_group_policy_t* node;
	ww_filter_group_policy_t* tmp;
	
	if (gid < 0 || gid > WEBWALL_MAX_GROUP) {
		return -eAosRc_WWInvalidGroupID;
	}
	
	policy_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &policy_head, list) {
		if (node->group_id == gid) {
			policy_head_unlock();
			return 0;
		}
	}
	policy_head_unlock();
	
	return -eAosRc_WWNoexistGroup;
}
/*
 * create named user
 * @group_id
 * @user_name
 */
int ww_create_named_user(char* group_id, char* user_name)
{
	ww_filter_name_based_user_t* node;
	int gid;
	
	gid = ww_get_group_id(group_id);
	if (ww_get_named_user(gid, user_name) != NULL) {
		return -eAosRc_WWExistNamedUser;
	}
	
	node = (ww_filter_name_based_user_t*)malloc(sizeof(ww_filter_name_based_user_t));
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	
	node->group_id = gid;
	strcpy(node->user_name, user_name);
	
	filter_name_based_user_head_lock();
	aos_list_add_tail(&node->list, &filter_name_based_user_head);
	filter_name_based_user_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove named user
 * @group_id
 * @user_name
 */
int ww_remove_named_user(char* group_id, char* user_name)
{
	ww_filter_name_based_user_t* node;
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((node = ww_get_named_user(gid, user_name)) == NULL) {
		return -eAosRc_WWNoexistNamedUser;
	}
	
	filter_name_based_user_head_lock();
	aos_list_del(&node->list);
	free(node);
	filter_name_based_user_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove all named users
 * @group_id
 */
int ww_remove_all_named_users(char* group_id)
{
	ww_filter_name_based_user_t* node;
	ww_filter_name_based_user_t* tmp;
	int gid;
	
	gid = ww_get_group_id(group_id);
	filter_name_based_user_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &filter_name_based_user_head, list) {
		if (node->group_id == gid) {
			aos_list_del(&node->list);
			free(node);
		}
	}
	filter_name_based_user_head_unlock();
	
	return -eAosRc_WWSuccess;
}

static ww_filter_ip_based_user_t* ww_get_ip_user(int group_id, char* ip)
{
	ww_filter_ip_based_user_t* node;
	ww_filter_ip_based_user_t* tmp;
	
	filter_ip_based_user_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &filter_ip_based_user_head, list) {
		if (node->group_id == group_id && strcmp(node->ip, ip) == 0) {
			filter_ip_based_user_head_unlock();
			return node;
		}
	}
	filter_ip_based_user_head_unlock();
	
	return NULL;
}
/*
 * create ip based user
 * @group_id
 * @ip
 */
int ww_create_ip_user(char* group_id, char* ip)
{
	ww_filter_ip_based_user_t* node;
	int gid;
	
	gid = ww_get_group_id(group_id);
	if (ww_get_ip_user(gid, ip) != NULL) {
		return -eAosRc_WWExistIPUser;
	}
	
	node = (ww_filter_ip_based_user_t*)malloc(sizeof(ww_filter_ip_based_user_t));
	if (node == NULL) {
		return -eAosRc_WWSuccess;
	}
	
#ifdef WEBWALL_DEUBG
	printf("create ip user %s\n", ip);
#endif
	node->group_id = gid;
	strcpy(node->ip, ip);
#ifdef WEBWALL_DEUBG
	printf("after, ip user %s\n", node->ip);
#endif
	filter_ip_based_user_head_lock();
	aos_list_add_tail(&node->list, &filter_ip_based_user_head);
	filter_ip_based_user_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove ip based user
 * @group_id
 * @ip
 */
int ww_remove_ip_user(char* group_id, char* ip)
{
	ww_filter_ip_based_user_t* node;
	int gid;
	
	gid = ww_get_group_id(group_id);
	if ((node = ww_get_ip_user(gid, ip)) == NULL) {
		return -eAosRc_WWNoexistIPUser;
	}
	
	filter_ip_based_user_head_lock();
	aos_list_del(&node->list);
	free(node);
	filter_ip_based_user_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove all ip based user
 * @group_id
 */
int ww_remove_all_ip_users(char* group_id)
{
	ww_filter_ip_based_user_t* node;
	ww_filter_ip_based_user_t* tmp;
	int gid;
	
	gid = ww_get_group_id(group_id);
	
	filter_ip_based_user_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &filter_ip_based_user_head, list) {
		if (node->group_id == gid) {
			aos_list_del(&node->list);
			free(node);
		}
	}
	filter_ip_based_user_head_unlock();
	
	return 0;
}

ww_filter_group_policy_t* ww_get_filter_group_policy(int group_id)
{
	ww_filter_group_policy_t* node;
	ww_filter_group_policy_t* tmp;
	
	policy_head_lock();
	aos_list_for_each_entry_safe(node, tmp, &policy_head, list) {
		if (node->group_id == group_id) {
			policy_head_unlock();
			return node;
		}
	}
	policy_head_unlock();
	
	return NULL;
}
/*
 * set filter group mode
 * @group_id
 * @mode
 */
int ww_set_filter_group_mode(char* group_id, char* mode)
{
	int gid;
	int m;
	ww_filter_group_policy_t* node;
	
	if ((m = validate_group_mode(mode)) == -eAosRc_WWUnsupportGroupModeType) {
		return -eAosRc_WWUnsupportGroupModeType;
	}
	gid = ww_get_group_id(group_id);
	if (validate_group_id(gid) < 0) {
		return -eAosRc_WWError;
	}
	
	node = ww_get_filter_group_policy(gid);
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	
	node->group_mode = m;
	
	return -eAosRc_WWSuccess;
}

/*
 * set filter group name
 * @group_id
 * @group_name
 */
int ww_set_filter_group_name(char* group_id, char* group_name)
{
	int gid;
	ww_filter_group_policy_t* node;
	
	gid = ww_get_group_id(group_id);
	if (validate_group_id(gid) < 0) {
		return -eAosRc_WWError;
	}
	
	node = ww_get_filter_group_policy(gid);
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	
	strcpy(node->group_name, group_name); 
	
	return -eAosRc_WWSuccess;
}

/*
 * map category list to filter group
 * @group_id
 * @list_name
 */
int ww_map_category_list_to_filter_group(char* group_id, char* list_name)
{
	ww_category_list_t* node;
	ww_filter_group_policy_t* policy;
	ww_filter_group_list_t* group_list;
	ww_filter_group_list_t* tmp_group_list;
	int gid;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	gid = ww_get_group_id(group_id);
	policy = ww_get_filter_group_policy(gid);
	if (policy == NULL) {
		return -eAosRc_WWError;
	}
	
	aos_list_for_each_entry_safe(group_list, tmp_group_list, &(policy->category_list_head), list) {
		if (group_list->category_list->list_type == node->list_type) {
			return -eAosRc_WWExistListName;
		}
	}
	
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL) {
		return -eAosRc_WWError;
	}
	group_list->category_list = node;
	group_list->category_list->ref_cout++;
	
	policy_head_lock();
	aos_list_add_tail(&group_list->list, &policy->category_list_head);
	policy_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * unmap category list of filter group
 * @group_id
 * @list_name
 */
int ww_unmap_category_list_of_filter_group(char* group_id, char* list_name)
{
	ww_category_list_t* node;
	ww_filter_group_policy_t* policy;
	ww_filter_group_list_t* group_list;
	ww_filter_group_list_t* tmp;
	int gid;
	
	if ((node = ww_get_category_list(list_name)) == NULL) {
		return -eAosRc_WWNoexistListName;
	}
	gid = ww_get_group_id(group_id);
	policy = ww_get_filter_group_policy(gid);
	if (policy == NULL) {
		return -eAosRc_WWError;
	}
	
	policy_head_lock();
	aos_list_for_each_entry_safe(group_list, tmp, &(policy->category_list_head), list) {
		if (group_list->category_list == node) {
			node->ref_cout--;
			aos_list_del(&group_list->list);
			free(group_list);
			policy_head_unlock();
			return -eAosRc_WWSuccess;
		}
	}
	policy_head_unlock();
	
	return -eAosRc_WWError;
}

/*
 * unmap all category list of filter group
 * @group_id
 * @list_type
 */
int ww_unmap_all_category_list_of_filter_group(char* group_id, char* list_type)
{
	int type;
	int gid;
	ww_filter_group_policy_t* policy;
	ww_filter_group_list_t* group_list;
	ww_filter_group_list_t* tmp;
	ww_category_list_t* node;
	
	if ((type = validate_list_type(list_type)) == -eAosRc_WWUnsupportListType) {
		return -eAosRc_WWUnsupportListType;
	}
	gid = ww_get_group_id(group_id);
	policy = ww_get_filter_group_policy(gid);
	if (policy == NULL) {
		return -eAosRc_WWError;
	}
	
	policy_head_lock();
	aos_list_for_each_entry_safe(group_list, tmp, &policy->category_list_head, list) {
		node = group_list->category_list;
		if (node != NULL) {
			if (node->list_type == type) {
				node->ref_cout--;
				aos_list_del(&group_list->list);
				free(group_list);
			}
		}
	}
	policy_head_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * set reporting level of web access denies
 * @reporting_level
 */
int ww_set_reporting_level(int reporting_level)
{
	global.report_level = reporting_level;
	
	return -eAosRc_WWSuccess;
}

/*
 * set language
 * @lang
 */
int ww_set_language(char* lang)
{
	strcpy(global.language, lang);
	
	return -eAosRc_WWSuccess;
}

/*
 * set log level
 * @log_level
 */
int ww_set_log_level(int log_level)
{
	global.log_level = log_level;
	
	return -eAosRc_WWSuccess;
}

/*
 * set log exception hits
 * @log_exception_hits
 */
int ww_set_log_exception_hits(char* log_exception_hits)
{
	strcpy(global.log_exception_hits, log_exception_hits);
	
	return -eAosRc_WWSuccess;
}

/*
 * set log file format
 * @format
 */
int ww_set_log_file_format(int format)
{
	global.log_file_format = format;
	
	return -eAosRc_WWSuccess;
}

/*
 * set maximum log item length
 * @len
 */
int ww_set_max_log_item_len(int len)
{
	global.max_log_item_len = len;
	
	return -eAosRc_WWSuccess;
}

/*
 * set anoymous log
 * @anoymous_log
 */
int ww_set_anoymous_log(char* anoymous_log)
{
	strcpy(global.anoymous_log_switch, anoymous_log);
	
	return -eAosRc_WWSuccess;
}

/*
 * set ip and port of webwall
 * @ip
 * @port
 */
int ww_set_ip_and_port_of_webwall(char* ip, int port)
{
	strcpy(global.filter_ip_address, ip);
	global.filter_port = port;
	
	return -eAosRc_WWSuccess;
}

/*
 * set ip and port of proxy
 * @ip
 * @port
 */
int ww_set_ip_and_port_of_proxy(char* ip, int port)
{
	strcpy(global.proxy_ip_address, ip);
	global.proxy_port = port;
	/* 
	 * in order to user's mistake, meanwhile modify tinyproxy ip and port
	 */
	strcpy(tiny_proxy.ip, ip);
	tiny_proxy.port = port;
	
	return -eAosRc_WWSuccess;
}

/*
 * set the access denied address
 * @addr
 */
int ww_set_access_denied_addr(char* addr)
{
	strcpy(global.access_deny_address, addr);
	
	return -eAosRc_WWSuccess;
}

/*
 * set filter group length
 * @len
 */
int ww_set_filter_group_len(int len)
{
	global.filter_groups = len;
	
	return -eAosRc_WWSuccess;
}

/*
 * set show weighted found
 * @show_weighted_found
 */
int ww_set_show_weighted_found(char* show_weighted_found)
{
	strcpy(global.show_weighted_found, show_weighted_found);
	
	return -eAosRc_WWSuccess;
}

/*
 * set weighted phrase mode
 * @mode
 */
int ww_set_weighted_phrase_mode(int mode)
{
	global.weighted_phrase_mode = mode;
	
	return -eAosRc_WWSuccess;
}

static ww_entry_t* ww_get_auth_plugin(char* plugin_name)
{
	ww_entry_t* node;
	ww_entry_t* tmp;
	
	auth_plugin_lock();
	aos_list_for_each_entry_safe(node, tmp, &global.auth_plugin_head, list) {
		if (strcmp(node->data, plugin_name) == 0) {
			auth_plugin_unlock();
			return node;
		}
	}
	auth_plugin_unlock();

	return NULL;
}
/*
 * create auth plugins
 * @plugin_name
 */
int ww_create_auth_plugins(char* plugin_name)
{
	ww_entry_t* node;

	if (ww_get_auth_plugin(plugin_name) != NULL) {
		return -eAosRc_WWExistPlugin;
	}

	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return -eAosRc_WWError;
	}

	strcpy(node->data, plugin_name);

	auth_plugin_lock();
	aos_list_add_tail(&node->list, &global.auth_plugin_head);
	auth_plugin_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * remove auth plugin
 * @plugin_name
 */
int ww_remove_auth_plugin(char* plugin_name)
{
	ww_entry_t* node;
	
	if ((node = ww_get_auth_plugin(plugin_name)) == NULL) {
		return -eAosRc_WWNoexistPlugin;
	}

	auth_plugin_lock();
	aos_list_del(&node->list);
	free(node);
	auth_plugin_unlock();
	
	return -eAosRc_WWSuccess;
}

/*
 * set the reverse address lookups
 * @reverse_addr_lookups
 */
int ww_reverse_addr_lookups(char* reverse_addr_lookups)
{
	strcpy(global.reverse_address_lookups, reverse_addr_lookups);
	
	return -eAosRc_WWSuccess;
}

/*
 * set the reverse clientip lookups
 * @reverse_clientip_lookups
 */
int ww_reverse_clientip_lookups(char* reverse_clientip_lookups)
{
	strcpy(global.reverse_clientip_lookups, reverse_clientip_lookups);
	
	return -eAosRc_WWSuccess;
}

/*
 * set the reverse client hostname
 * @reverse_client_hostname
 */
int ww_reverse_client_hostname(char* reverse_client_hostname)
{
	strcpy(global.log_client_hostname, reverse_client_hostname);
	
	return -eAosRc_WWSuccess;
}

/* the following is the configuration section of tinyproxy or squid*/
/*
 * set proxy address
 * @ip
 * @port
 */
int ww_set_proxy_addr(char* ip, int port)
{
	ww_set_ip_and_port_of_proxy(ip, port);
	return 0;
}

/* following initalize function */
/*
static char* ww_trim(char* str)
{
	int len = strlen(str);
	char* p;
	char* q;
	
	q = str;
	while (*q == ' ' || *q == '\t' || *q == '\'') {
		q++;
	}
	p = str + len - 1;
	while (p != str) {
		if (*p == '\n' || *p == ' ' || *p == '\t' || *p == '\'') {
			*p = '\0';
		} else
			break;
		p--;
	}
	
	return q;
}
*/

/*
static int ww_read_config_file(char* path, char* tag, char* value)
{
	FILE* fp;
	char buff[MAX_BUFF_LEN];
	char key[MAX_BUFF_LEN];
	char* p;
	char* q;
	int len;
	
	if ((fp = fopen(path, "r")) == NULL) {
		return -eAosRc_WWError;
	}
	
	memset(buff, 0, MAX_BUFF_LEN);
	memset(key, 0, MAX_BUFF_LEN);
	
	sprintf(key, "%s =", tag);
	len = strlen(key);
	
	while (!feof(fp)) {
		memset(buff, 0, MAX_BUFF_LEN);
		fgets(buff, MAX_BUFF_LEN, fp);
		q = ww_trim(buff);
		if (strncmp(q, key, len) == 0) {
			p = q + len;
			strcpy(value, p);
			fclose(fp);
			return -eAosRc_WWSuccess;
		}
	}

	fclose(fp);

	return -eAosRc_WWError;
}
*/

static int ww_clear_file(char* path)
{
	char cmd[MAX_BUFF_LEN];
	int ret;
	
	memset(cmd, 0, MAX_BUFF_LEN);
	sprintf(cmd, "cp -vf %s %s.backup", path, path);
	ret = system(cmd);
	
	if (ret < 0) {
		memset(cmd, 0, MAX_BUFF_LEN);
		sprintf(cmd, "touch %s ", path);
		ret = system(cmd);
	}
	
	memset(cmd, 0, MAX_BUFF_LEN);
	sprintf(cmd, "echo \"\" >%s", path);
	ret |= system(cmd);
	
	return ret;
}

static int ww_write_config_file_for_named_user(char* path, char* tag, char* value)
{
	FILE* fp;
	char key[MAX_BUFF_LEN];
	int len;
	
	if ((fp = fopen(path, "a+")) == NULL) {
		return -eAosRc_WWError;
	}
	
	memset(key, 0, MAX_BUFF_LEN);
	
	sprintf(key, "%s=%s\n", tag, value);
	len = strlen(key);
	fseek(fp, 0, SEEK_END);
	fwrite(key, 1, len, fp);
	fclose(fp);

	return -eAosRc_WWError;
}


static int ww_write_config_file(char* path, char* tag, char* value)
{
	FILE* fp;
	char key[MAX_BUFF_LEN];
	int len;
	
	if ((fp = fopen(path, "a+")) == NULL) {
		return -eAosRc_WWError;
	}
	
	memset(key, 0, MAX_BUFF_LEN);
	
	sprintf(key, "%s = %s\n", tag, value);
	len = strlen(key);
	fseek(fp, 0, SEEK_END);
	fwrite(key, 1, len, fp);
	fclose(fp);

	return -eAosRc_WWError;
}

/*
static int ww_read_config_fileM(char* path, char* tag, struct aos_list_head *head)
{
	FILE* fp;
	char buff[MAX_BUFF_LEN];
	char key[MAX_BUFF_LEN];
	char value[MAX_BUFF_LEN];
	char* p;
	char* q;
	int len;
	ww_entry_t* entry;
	
	if ((fp = fopen(path, "r")) == NULL) {
		return -eAosRc_WWError;
	}
	
	memset(buff, 0, MAX_BUFF_LEN);
	memset(key, 0, MAX_BUFF_LEN);
	
	sprintf(key, "%s =", tag);
	len = strlen(key);
	
	while (!feof(fp)) {
		memset(buff, 0, MAX_BUFF_LEN);
		fgets(buff, MAX_BUFF_LEN, fp);
		q = ww_trim(buff);
		if (strncmp(q, key, len) == 0) {
			p = q + len;
			strcpy(value, p);
			entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
			strcpy(entry->data, value);
			aos_list_add_tail(&entry->list, head);
		}
	}

	fclose(fp);

	return -eAosRc_WWError;
}
*/

/* set the default value*/
static int ww_init_global(void)
{
	ww_entry_t* entry;
	
	/* reporting level */
	global.report_level = 3;
	/* language path */
#ifdef WEBWALL_DEUBG
	strcpy(global.language_path, "/usr/local/share/dansguardian/languages");
#else
	strcpy(global.language_path, "/usr/local/share/dansguardian/languages");
#endif
	/* language */
	strcpy(global.language, "ukenglish");
	/* log level */
	global.log_level = 2;
	/* log exception hits */
	strcpy(global.log_exception_hits, "on");
	/* log file format */
	global.log_file_format = 1;
	/* max log item length */
	global.max_log_item_len = 400;
	/* anoymous logs */
	strcpy(global.anoymous_log_switch, "on");
	/* log file path */
#ifdef WEBWALL_DEUBG
	strcpy(global.log_file_path, "/usr/local/var/log/dansguardian/access.log");
#else
	strcpy(global.log_file_path, "/usr/local/var/log/dansguardian/access.log");
#endif
	/* stat log file path */
#ifdef WEBWALL_DEUBG
	strcpy(global.stat_log_file_path, "/usr/local/var/log/dansguardian/stats");
#else
	strcpy(global.stat_log_file_path, "/usr/local/var/log/dansguardian/stats");
#endif
	/* filter ip address */
	strcpy(global.filter_ip_address, "0.0.0.0");
	/* filter port*/
	global.filter_port = 8080;
	/* proxy ip address */
	strcpy(global.proxy_ip_address, "127.0.0.1");
	strcpy(tiny_proxy.ip, "127.0.0.1");
	/* proxy port */
	global.proxy_port = 3128;
	tiny_proxy.port = 3128;
	/* access denied address */
	strcpy(global.access_deny_address, "http://YOURSERVER.YOURDOMAIN/cgi-bin/dansguardian.pl");
	/* no standard delimter */
	strcpy(global.no_standard_delimiter, "on");
	/* use custom banned image */
	global.use_custom_banned_image = 1;
	/* custom banned image path */
#ifdef WEBWALL_DEUBG
	strcpy(global.custom_banned_image_path, "/usr/local/share/dansguardian/transparent1x1.gif");
#else
	strcpy(global.custom_banned_image_path, "/usr/local/share/dansguardian/transparent1x1.gif");
#endif
	/* filter groups */
	global.filter_groups = 1;
	/* filter group list */
#ifdef WEBWALL_DEUBG
	strcpy(global.filter_groups_list_path, "/usr/local/etc/dansguardian/lists/filtergroupslist");
#else
	strcpy(global.filter_groups_list_path, "/usr/local/etc/dansguardian/lists/filtergroupslist");
#endif
	/* banned ip list path */
#ifdef WEBWALL_DEUBG
	strcpy(global.banned_ip_list_path, "/usr/local/etc/dansguardian/lists/bannediplist");
#else
	strcpy(global.banned_ip_list_path, "/usr/local/etc/dansguardian/lists/bannediplist");
#endif
	/* exception ip list */
#ifdef WEBWALL_DEUBG
	strcpy(global.exception_ip_list_path, "/usr/local/etc/dansguardian/lists/exceptioniplist");
#else
	strcpy(global.exception_ip_list_path, "/usr/local/etc/dansguardian/lists/exceptioniplist");
#endif
	/* show weighted found*/
	strcpy(global.show_weighted_found, "on");
	/* weighted phrase mode */
	global.weighted_phrase_mode = 2;
	/* url cache number */
	global.url_cache_number = 1000;
	/* url cache age */
	global.url_cache_age = 900;
	/* scan clean cache */
	strcpy(global.scan_clean_cache, "on");
	/* phrase filter mode */
	global.phrase_filter_mode = 2;
	/* preserver case */
	global.preserve_case = 0;
	/* hex decode content */
	global.hex_decode_content = 0;
	/* force quick search */
	global.force_quick_search = 0;
	/* reverver address lookups */
	strcpy(global.reverse_address_lookups, "off");
	/* reverse clientip lookups */
	strcpy(global.reverse_clientip_lookups, "off");
	/* log client hostnames */
	strcpy(global.log_client_hostname, "off");
	/* create list cache files */
	strcpy(global.create_list_cache_files, "on");
	/* max upload size */
	global.max_upload_size = -1;
	/* max content filter size */
	global.max_content_filter_size = 256;
	/* max content ram cache scan size */
	global.max_content_ram_cache_scan_size = 2000;
	/* max content file cache scan size */
	global.max_content_file_cache_scan_size = 20000;
	/* file cache dir */
	strcpy(global.file_cache_dir, "/tmp");
	/* delete downloaded temp files */
	strcpy(global.delete_downloaded_temp_files, "on");
	/* initial trickle delay */
	global.initial_trickle_delay = 20;
	/* trickle delay*/
	global.trickle_delay = 10;
	/* download manager */
	AOS_INIT_LIST_HEAD(&global.download_manager_head);
	entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (entry == NULL) {
		return -eAosRc_WWError;
	}
#ifdef WEBWALL_DEUBG
	strcpy(entry->data, "/usr/local/etc/dansguardian/downloadmanagers/fancy.conf");
#else
	strcpy(entry->data, "/usr/local/etc/dansguardian/downloadmanagers/fancy.conf");
#endif
	aos_list_add_tail(&entry->list, &global.download_manager_head);
	
	entry = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (entry == NULL) {
		return -eAosRc_WWError;
	}
#ifdef WEBWALL_DEUBG
	strcpy(entry->data, "/usr/local/etc/dansguardian/downloadmanagers/default.conf");
#else
	strcpy(entry->data, "/usr/local/etc/dansguardian/downloadmanagers/default.conf");
#endif	
	aos_list_add_tail(&entry->list, &global.download_manager_head);
	/* content scanner timeout */
	global.content_scanner_timeout = 60;
	/* content scan exceptions*/
	strcpy(global.content_scan_exceptions, "off");
	/* recheck replace urls*/
	strcpy(global.recheck_replace_urls, "off");
	/* forwarded for */
	strcpy(global.forwarded_for, "off");
	/* use xforwarded for */
	strcpy(global.use_xforwarded_for, "off");
	/* log connection handling errors */
	strcpy(global.log_connection_handling_errors, "on");
	/* log child process handling */
	strcpy(global.log_child_process_handling, "off");
	/* max children */
	global.max_children = 120;
	/* min children */
	global.min_children = 8;
	/* min spare children */
	global.min_spare_children = 4;
	/* prefork children */
	global.prefork_children = 6;
	/* max spare children */
	global.max_spare_children = 32;
	/* max age children */
	global.max_age_children = 500;
	/* max ips*/
	global.max_ips = 0;
	/* no daemon */
	strcpy(global.no_daemon, "off");
	/* no logger */
	strcpy(global.no_logger, "off");
	/* log ad blocks */
	strcpy(global.log_ad_blocks, "off");
	/* soft restart*/
	strcpy(global.soft_restart, "off");
	/* auth plugin*/
	AOS_INIT_LIST_HEAD(&(global.auth_plugin_head));
	/* webwall status 
	 * modified booting mechansim for saving config
	 * 01/26/2007
	 */
	global.webwall_status = T_WEBWALL_STOP;
	
	return 0;
}

static int ww_init_default_policy(ww_filter_group_policy_t* policy, int gid)
{
	ww_category_list_t* category;
	ww_filter_group_list_t* group_list;
	char buff[MAX_BUFF_LEN];
	
	policy->group_mode = 1;
	strcpy(policy->group_name, "");
	policy->reporting_level = 3;
	policy->group_id = 1;
	policy->naughtyness_limit = 50;
	policy->category_display_threshold = 0;
	policy->embedded_url_weight = 0;
	strcpy(policy->enable_pics, "off");
	policy->by_pass = 0;
	strcpy(policy->by_pass_key, "");
	policy->infection_by_pass = 0;
	strcpy(policy->infection_by_pass_key, "");
	strcpy(policy->infection_by_pass_errors_only, "on");
	strcpy(policy->disable_content_scan, "off");
	strcpy(policy->deep_url_analysis, "off");
	AOS_INIT_LIST_HEAD(&(policy->category_list_head));
	
	/* bannedphraselist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedphraselist%d", gid);
	else
		sprintf(buff, "bannedphraselist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedphraselist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* weightedphraselist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "weightedphraselist%d", gid);
	else
		sprintf(buff, "weightedphraselist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("weightedphraselist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* exceptionphraselist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "exceptionphraselist%d", gid);
	else
		sprintf(buff, "exceptionphraselist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("exceptionphraselist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* bannedsitelist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedsitelist%d", gid);
	else
		sprintf(buff, "bannedsitelist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedsitelist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* greysitelist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "greysitelist%d", gid);
	else
		sprintf(buff, "greysitelist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("greysitelist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* exceptionsitelist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "exceptionsitelist%d", gid);
	else
		sprintf(buff, "exceptionsitelist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("exceptionsitelist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* bannedurllist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedurllist%d", gid);
	else
		sprintf(buff, "bannedurllist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedurllist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* greyurllist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "greyurllist%d", gid);
	else
		sprintf(buff, "greyurllist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("greyurllist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* exceptionurllist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "exceptionurllist%d", gid);
	else
		sprintf(buff, "exceptionurllist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("exceptionurllist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* exceptionregexpurllist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "exceptionregexpurllist%d", gid);
	else
		sprintf(buff, "exceptionregexpurllist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("exceptionregexpurllist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* bannedregexpurllist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedregexpurllist%d", gid);
	else
		sprintf(buff, "bannedregexpurllist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedregexpurllist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* picsfile */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "pics%d", gid);
	else
		sprintf(buff, "pics");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("picsfile", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* contentregexplist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "contentregexplist%d", gid);
	else
		sprintf(buff, "contentregexplist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("contentregexplist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* urlregexplist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "urlregexplist%d", gid);
	else
		sprintf(buff, "urlregexplist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("urlregexplist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* bannedextensionlist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedextensionlist%d", gid);
	else
		sprintf(buff, "bannedextensionlist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedextensionlist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* bannedmimetypelist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "bannedmimetypelist%d", gid);
	else
		sprintf(buff, "bannedmimetypelist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("bannedmimetypelist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	/* exceptionfilesitelist */
	memset(buff, 0, MAX_BUFF_LEN);
	if (gid != 1)
		sprintf(buff, "exceptionfilesitelist%d", gid);
	else
		sprintf(buff, "exceptionfilesitelist");
	category = ww_get_category_list(buff);
	if (category == NULL) {
		ww_create_category_list("exceptionfilesitelist", buff, 1);
		if ((category = ww_get_category_list(buff)) == NULL) {
			return -1;
		}
	}
	category->is_system = 1;
	group_list = (ww_filter_group_list_t*)malloc(sizeof(ww_filter_group_list_t));
	if (group_list == NULL)
		return -1;
	category->ref_cout++;
	group_list->category_list = category;
	aos_list_add_tail(&(group_list->list), &(policy->category_list_head));
	
	#ifdef WEBWALL_DEUBG
	printf("success to init default policy\n");
	#endif
	
	return 0;
}
/*
 * initialize the default group
 */
static int ww_init_policy(void)
{
	ww_filter_group_policy_t* policy;
	
	AOS_INIT_LIST_HEAD(&policy_head);
	policy = (ww_filter_group_policy_t*)malloc(sizeof(ww_filter_group_policy_t));
	if (policy == NULL) {
		return -1;
	}
	
	if (ww_init_default_policy(policy, 1) < 0) {
		#ifdef WEBWALL_DEUBG
		printf("fail to initialize policy\n");
		#endif
		return -1;
	}
	
	policy_head_lock();
	aos_list_add_tail(&(policy->list), &policy_head);
	policy_head_unlock();
	
	return 0;
}

static int ww_set_sys_of_list(char* list_name)
{
	ww_category_list_t* category;
	
	category = ww_get_category_list(list_name);
	if (category == NULL) {
		return -1;
	}
	
	category->is_system = 1;
	
	return 0;
}

static int ww_init_category(void)
{
	AOS_INIT_LIST_HEAD(&category_list_head);
	
	/* bannedphraselist */
	ww_create_category_list("bannedphraselist", "bannedphraselist", 1);
	ww_create_include_entry_file("bannedphraselist", "phraselists", "pornography/banned");
		
	/* weightedphraselist */
	ww_create_category_list("weightedphraselist", "weightedphraselist", 1);
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_general");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_news");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_general_danish");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_general_dutch");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_general_malay");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "goodphrases/weighted_general_portuguese");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_chinese");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_danish");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_dutch");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_french");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_german");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_italian");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_japanese");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_malay");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_portuguese");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_spanish");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "pornography/weighted_russian");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "nudism/weighted");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "badwords/weighted_dutch");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "badwords/weighted_french");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "badwords/weighted_german");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "badwords/weighted_portuguese");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "badwords/weighted_spanish");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "malware/weighted");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "proxies/weighted");
	ww_create_include_entry_file("weightedphraselist", "phraselists", "warezhacking/weighted");
	
	/* exceptionphraselist */
	ww_create_category_list("exceptionphraselist", "exceptionphraselist", 1);
	
	/* bannedsitelist */
	ww_create_category_list("bannedsitelist", "bannedsitelist", 1);
	ww_create_include_entry_file("bannedsitelist", "blacklist_domains", "ads");
	
	/* greysitelist */
	ww_create_category_list("greysitelist", "greysitelist", 1);
	
	/* exceptionsitelist */
	ww_create_category_list("exceptionsitelist", "exceptionsitelist", 1);
	
	/* bannedurllist */
	ww_create_category_list("bannedurllist", "bannedurllist", 1);
	ww_create_include_entry_file("bannedsitelist", "blacklist_urls", "ads");
	
	/* greyurllist */
	ww_create_category_list("greyurllist", "greyurllist", 1);
	
	/* exceptionurllist */
	ww_create_category_list("exceptionurllist", "exceptionurllist", 1);
	
	/* exceptionregexpurllist */
	ww_create_category_list("exceptionregexpurllist", "exceptionregexpurllist", 1);
	
	/* bannedregexpurllist */
	ww_create_category_list("bannedregexpurllist", "bannedregexpurllist", 1);
	
	/* picsfile */
	ww_create_category_list("picsfile", "pics", 1);
	
	/* contentregexplist */
	ww_create_category_list("contentregexplist", "contentregexplist", 1);
	
	/* urlregexplist */
	ww_create_category_list("urlregexplist", "urlregexplist", 1);
	
	/* bannedextensionlist */
	ww_create_category_list("bannedextensionlist", "bannedextensionlist", 1);
	
	/* bannedmimetypelist */
	ww_create_category_list("bannedmimetypelist", "bannedmimetypelist", 1);
	
	/* exceptionfilesitelist */
	ww_create_category_list("exceptionfilesitelist", "exceptionfilesitelist", 1);
	
	ww_create_single_entry("exceptionfilesitelist", "windowsupdate.microsoft.com");
	ww_create_single_entry("exceptionfilesitelist", "update.microsoft.com");
	ww_create_single_entry("exceptionfilesitelist", "download.windowsupdate.com");
	
	/* 
	 * below anti-virus list 
	 */
	
	/* exceptionvirusextensionlist */
	ww_create_category_list("exceptionvirusextensionlist", "exceptionvirusextensionlist", 1);
	ww_set_sys_of_list("exceptionvirusextensionlist");
	
	/* exceptionvirusmimetypelist */
	ww_create_category_list("exceptionvirusmimetypelist", "exceptionvirusmimetypelist", 1);
	ww_set_sys_of_list("exceptionvirusmimetypelist");
	
	/* exceptionvirussitelist */
	ww_create_category_list("exceptionvirussitelist", "exceptionvirussitelist", 1);
	ww_set_sys_of_list("exceptionvirussitelist");
	
	/* exceptionvirusurllist */
	ww_create_category_list("exceptionvirusurllist", "exceptionvirusurllist", 1);
	ww_set_sys_of_list("exceptionvirusurllist");
	
	#ifdef WEBWALL_DEUBG
	printf("new init the default category\n");
	#endif
	
	return 0;
}

static int ww_init_user(void)
{
	AOS_INIT_LIST_HEAD(&filter_name_based_user_head);
	AOS_INIT_LIST_HEAD(&filter_ip_based_user_head);
	
	return 0;
}

static int ww_write_global_config_file()
{
	char buff[MAX_BUFF_LEN];
	ww_entry_t* entry;
	ww_entry_t* tmp;
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp_engine;
	
	/* clear file content */
	if (ww_clear_file(WEBWALL_GLOBAL_CONFIG) < 0) {
		return -eAosRc_WWError;
	}
	
	/* reporting level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.report_level);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "reportinglevel", buff);
	/* language path */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.language_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "languagedir", buff);
	/* language */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.language);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "language", buff);
	/* log level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.log_level);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "loglevel", buff);
	/* log exception hits */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.log_exception_hits);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logexceptionhits", buff);
	/* log file format */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.log_file_format);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logfileformat", buff);
	/* max log item length */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_log_item_len);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxlogitemlength", buff);
	/* anoymous logs */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.anoymous_log_switch);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "anonymizelogs", buff);
	/* log file path */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.log_file_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "loglocation", buff);
	/* stat log file path */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.stat_log_file_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "statlocation", buff);
	/* filter ip address */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.filter_ip_address);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "filterip", buff);
	/* filter port*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.filter_port);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "filterport", buff);
	/* proxy ip address */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.proxy_ip_address);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "proxyip", buff);
	/* proxy port */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.proxy_port);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "proxyport", buff);
	/* access denied address */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.access_deny_address);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "accessdeniedaddress", buff);
	/* no standard delimter */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.no_standard_delimiter);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "nonstandarddelimiter", buff);
	/* use custom banned image */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.use_custom_banned_image);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "usecustombannedimage", buff);
	/* custom banned image path */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.custom_banned_image_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "custombannedimagefile", buff);
	/* filter groups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.filter_groups);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "filtergroups", buff);
	/* filter group list */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.filter_groups_list_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "filtergroupslist", buff);
	/* banned ip list path */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.banned_ip_list_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "bannediplist", buff);
	/* exception ip list */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.exception_ip_list_path);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "exceptioniplist", buff);
	/* show weighted found*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.show_weighted_found);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "showweightedfound", buff);
	/* weighted phrase mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.weighted_phrase_mode);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "weightedphrasemode", buff);
	/* url cache number */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.url_cache_number);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "urlcachenumber", buff);
	/* url cache age */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.url_cache_age);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "urlcacheage", buff);
	/* scan clean cache */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.scan_clean_cache);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "scancleancache", buff);
	/* phrase filter mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.phrase_filter_mode);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "phrasefiltermode", buff);
	/* preserver case */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.preserve_case);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "preservecase", buff);
	/* hex decode content */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.hex_decode_content);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "hexdecodecontent", buff);
	/* force quick search */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.force_quick_search);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "forcequicksearch", buff);
	/* reverver address lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.reverse_address_lookups);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "reverseaddresslookups", buff);
	/* reverse clientip lookups */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.reverse_clientip_lookups);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "reverseclientiplookups", buff);
	/* log client hostnames */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.log_client_hostname);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logclienthostnames", buff);
	/* create list cache files */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.create_list_cache_files);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "createlistcachefiles", buff);
	/* max upload size */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_upload_size);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxuploadsize", buff);
	/* max content filter size */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_content_filter_size);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxcontentfiltersize", buff);
	/* max content ram cache scan size */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_content_ram_cache_scan_size);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxcontentramcachescansize", buff);
	/* max content file cache scan size */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_content_file_cache_scan_size);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxcontentfilecachescansize", buff);
	/* file cache dir */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "'%s'", global.file_cache_dir);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "filecachedir", buff);
	/* delete downloaded temp files */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.delete_downloaded_temp_files);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "deletedownloadedtempfiles", buff);
	/* initial trickle delay */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.initial_trickle_delay);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "initialtrickledelay", buff);
	/* trickle delay*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.trickle_delay);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "trickledelay", buff);
	/* download manager */
	aos_list_for_each_entry_safe(entry, tmp, &global.download_manager_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "'%s'", entry->data);
		ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "downloadmanager", buff);
	}
	
	/* AV engine */
	aos_list_for_each_entry_safe(engine, tmp_engine, &(av_engine_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s%s/%s", WEBWALL_ROOT, "contentscanners", engine->file_name);
		ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "contentscanner", buff);
	}
	
	/* content scanner timeout */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.content_scanner_timeout);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "contentscannertimeout", buff);
	/* content scan exceptions*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.content_scan_exceptions);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "contentscanexceptions", buff);
	/* recheck replace urls*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.recheck_replace_urls);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "recheckreplacedurls", buff);
	/* forwarded for */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.forwarded_for);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "forwardedfor", buff);
	/* use xforwarded for */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.use_xforwarded_for);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "usexforwardedfor", buff);
	/* log connection handling errors */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.log_connection_handling_errors);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logconnectionhandlingerrors", buff);
	/* log child process handling */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.log_child_process_handling);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logchildprocesshandling", buff);
	/* max children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxchildren", buff);
	/* min children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.min_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "minchildren", buff);
	/* min spare children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.min_spare_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "minsparechildren", buff);
	/* prefork children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.prefork_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "preforkchildren", buff);
	/* max spare children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_spare_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxsparechildren", buff);
	/* max age children */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_age_children);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxagechildren", buff);
	/* max ips*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%d", global.max_ips);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "maxips", buff);
	/* no daemon */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.no_daemon);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "nodaemon", buff);
	/* no logger */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.no_logger);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "nologger", buff);
	/* log ad blocks */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.log_ad_blocks);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "logadblocks", buff);
	/* soft restart*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s", global.soft_restart);
	ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "softrestart", buff);
	/* auth plugin*/
	aos_list_for_each_entry_safe(entry, tmp, &global.auth_plugin_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "'%s%s.conf'", WEBWALL_PLUGIN_DIR, entry->data);
		ww_write_config_file(WEBWALL_GLOBAL_CONFIG, "authplugin", buff);
	}
	
	return -eAosRc_WWSuccess;
}

static int ww_write_named_user_config_file()
{
	int ret;
	char buff[MAX_BUFF_LEN];
	ww_filter_name_based_user_t* node;
	ww_filter_name_based_user_t* tmp;
	
	ret = ww_clear_file(WEBWALL_NAMED_USER_PATH);
	if (ret < 0) {
		return -eAosRc_WWError;
	}

	aos_list_for_each_entry_safe(node, 
								 tmp, 
					             &filter_name_based_user_head, 
								 list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "filter%d", node->group_id);
		ww_write_config_file_for_named_user(WEBWALL_NAMED_USER_PATH, node->user_name, buff);
	}
	
	return -eAosRc_WWSuccess;
}

static int ww_write_ip_user_config_file()
{
	int ret;
	char buff[MAX_BUFF_LEN];
	ww_filter_ip_based_user_t* node;
	ww_filter_ip_based_user_t* tmp;
	
	ret = ww_clear_file(WEBWALL_IP_USER_PATH);
	if (ret < 0) {
		return -eAosRc_WWError;
	}

	aos_list_for_each_entry_safe(node, 
								 tmp, 
					             &filter_ip_based_user_head, 
								 list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "filter%d", node->group_id);
		ww_write_config_file(WEBWALL_IP_USER_PATH, node->ip, buff);
	}
	
	return -eAosRc_WWSuccess;
}

static int ww_write_single_category_to_file(ww_category_list_t* node)
{
	char buff[MAX_BUFF_LEN];
	int ret;
	FILE* fp;
	int len;
	ww_entry_t* entry;
	ww_entry_t* tmp;
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s%s", WEBWALL_LIST_DIR, node->path);
	ret = ww_clear_file(buff);
	if (ret < 0) {
		return -eAosRc_WWError;
	}
	
	if ((fp = fopen(buff, "w+")) == NULL) {
		return -eAosRc_WWError;
	}
	
	/* time section */
	if (node->tc != NULL) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "#time: %d %d %d %d %s\n", 
				node->tc->start_hour, node->tc->start_minute, 
				node->tc->end_hour, node->tc->end_minute,
				node->tc->days);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	/* list category */
	if (node->category_alias != NULL) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "#listcategory:\"%s\"\n", node->category_alias);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	/* entry */
	aos_list_for_each_entry_safe(entry, tmp, &node->entry_list_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s\n", entry->data);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	/* include file */
	aos_list_for_each_entry_safe(entry, tmp, &node->include_list_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, ".Include<%s%s>\n", WEBWALL_LIST_DIR, entry->data);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	/* squidguard file */
	aos_list_for_each_entry_safe(entry, tmp, &node->guard_entry_file_list_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		switch(entry->type) {
			case T_WW_ENTRY_TYPE_DOMAIN:
				sprintf(buff, ".Include<%sblacklists/%s/domains>\n", WEBWALL_LIST_DIR, entry->data);
				break;
			case T_WW_ENTRY_TYPE_URL:
				sprintf(buff, ".Include<%sblacklists/%s/urls>\n", WEBWALL_LIST_DIR, entry->data);
				break;
			case T_WW_ENTRY_TYPE_PHRASE:
				sprintf(buff, ".Include<%sphraselists/%s>\n", WEBWALL_LIST_DIR, entry->data);
				break;
		}
		len = strlen(buff);
		fwrite(buff, 1, len, fp);	
	}
	
	#ifdef WEBWALL_DEUBG
	printf("save category %s\n", node->path);
	#endif
	
	fclose(fp);
	
	return 0;
}

/*
 * save system category list to file
 */
static int ww_write_sys_category_config_file(ww_category_list_t* node)
{
	char buff[MAX_BUFF_LEN];
    int ret;
    FILE* fp;
    int len;
    ww_entry_t* entry;
    ww_entry_t* tmp;
    	
	/* anti-virus */
	if (node->list_type == T_WW_EXCEPTION_VIRUS_EXTENSION_LIST
		|| node->list_type == T_WW_EXCEPTION_VIRUS_MIMETYPE_LIST
		|| node->list_type == T_WW_EXCEPTION_VIRUS_SITE_LIST
		|| node->list_type == T_WW_EXCEPTION_VIRUS_URL_LIST)
	{    	
    	memset(buff, 0, MAX_BUFF_LEN);
    	sprintf(buff, "%s%s/%s", WEBWALL_LIST_DIR, "contentscanners", node->path);
    	ret = ww_clear_file(buff);
    	if (ret < 0) {
    		return -eAosRc_WWError;
    	}
    	
    	if ((fp = fopen(buff, "w+")) == NULL) {
    		return -eAosRc_WWError;
    	}
    	/* here, only consider single entry, later others */
    	aos_list_for_each_entry_safe(entry, tmp, &(node->entry_list_head), list) {
    		memset(buff, 0, MAX_BUFF_LEN);
    		sprintf(buff, "%s\n", entry->data);
    		len = strlen(buff);
    		fwrite(buff, 1, len, fp);
    	}
    	
    	fclose(fp);
	} else
		ww_write_single_category_to_file(node);
	
	return 0;
}


static int ww_write_single_antivirus_config_file(ww_av_engine_t* engine)
{
	char buff[MAX_BUFF_LEN];
    int ret;
    FILE* fp;
    int len;
    
    memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "%s%s/%s", WEBWALL_ROOT, "contentscanners", engine->file_name);
	ret = ww_clear_file(buff);
	if (ret < 0) {
		return -eAosRc_WWError;
	}
	
	if ((fp = fopen(buff, "w+")) == NULL) {
		return -eAosRc_WWError;
	}
	
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "plugname = '%s'\n", engine->plugname);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	if (strlen(engine->sockfile_location) > 2) {
    	memset(buff, 0, MAX_BUFF_LEN);
    	switch (engine->type){
    		case T_VIRUS_CLAMAV:
    			sprintf(buff, "clamdudsfile = '%s'\n", engine->sockfile_location);
    			break;
    		case T_VIRUS_KAV:
    			sprintf(buff, "kavdudsfile = '%s'\n", engine->sockfile_location);
    			break;
    		default:
    			fclose(fp);
    			return -1;
    	}
    	len = strlen(buff);
    	fwrite(buff, 1, len, fp);
	}
	
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, "exceptionvirusmimetypelist = '/usr/local/etc/dansguardian/lists/contentscanners/exceptionvirusmimetypelist'\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, "exceptionvirusextensionlist = '/usr/local/etc/dansguardian/lists/contentscanners/exceptionvirusextensionlist'\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, "exceptionvirussitelist = '/usr/local/etc/dansguardian/lists/contentscanners/exceptionvirussitelist'\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, "exceptionvirusurllist = '/usr/local/etc/dansguardian/lists/contentscanners/exceptionvirusurllist'\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	fclose(fp);
	
	return 0;
}

/*
 * write anti-virus config to file
 */
static int ww_write_antivirus_config_file()
{
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp;
	int ret = 0;
	
	aos_list_for_each_entry_safe(engine, tmp, &(av_engine_head), list) {
		ret |= ww_write_single_antivirus_config_file(engine);		
	}
	
	return ret;
}

static int ww_write_category_config_file()
{
	ww_category_list_t* node;
	ww_category_list_t* tmp;
	int ret;
	
	aos_list_for_each_entry_safe(node, tmp, &category_list_head, list) {
		if (node->is_system != 1)
			ret |= ww_write_single_category_to_file(node);
		else
			ret |= ww_write_sys_category_config_file(node);
	}
	
	return -eAosRc_WWSuccess;
}

static int ww_write_single_policy_to_file(ww_filter_group_policy_t* node)
{
	FILE* fp;
	char buff[MAX_BUFF_LEN];
	int ret;
	int len;
	ww_filter_group_list_t* entry;
	ww_filter_group_list_t* tmp;
	
	sprintf(buff, "%s%sf%d.conf", WEBWALL_ROOT, WEBWALL_GROUP_PREFIX, node->group_id);
	ret = ww_clear_file(buff);
	if (ret < 0) {
		return -eAosRc_WWError;
	}
	
	if ((fp = fopen(buff, "w+")) == NULL) {
		return -eAosRc_WWError;
	}
	
	/* group mode */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "groupmode = %d\n", node->group_mode);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* group name */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "groupname = '%s'\n", node->group_name);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* list*/
	aos_list_for_each_entry_safe(entry, tmp, &node->category_list_head, list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "%s = '%s%s'\n", get_list_type_name(entry->category_list->list_type), WEBWALL_LIST_DIR, 
				entry->category_list->path);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	
	/*naughtyness limit*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "naughtynesslimit = %d\n", node->naughtyness_limit);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* category displayt hreshold*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "categorydisplaythreshold = 0\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* embedded url weight*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "embeddedurlweight = 0\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* enable pics */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "enablepics = off\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* by pass */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "bypass = 0\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* by pass key*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "bypasskey = ''\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* infection by pass*/
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "infectionbypass = 0\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* infection by pass key */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "infectionbypasskey = ''\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* infectionbypasserrorsonly */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "infectionbypasserrorsonly = on\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* disablecontentscan */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "disablecontentscan = off\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* deepurlanalysis */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "deepurlanalysis = off\n");
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	
	
	
	fclose(fp);
	
	return -eAosRc_WWSuccess;
}

static int ww_write_policy_config_file()
{
	ww_filter_group_policy_t* node;
	ww_filter_group_policy_t* tmp;
	int ret;
	
	aos_list_for_each_entry_safe(node, tmp, &policy_head, list) {
		ret |= ww_write_single_policy_to_file(node);
	}
	
	return -eAosRc_WWSuccess;
}

static int ww_write_tiny_proxy_config_file(void)
{
	int ret;
	FILE* fp = NULL;
	char buff[MAX_BUFF_LEN];
	int len = 0;
	ww_entry_t* node;
	ww_entry_t* tmp;
	
	ret = ww_clear_file(TINY_PROXY_GLOBAL_CONFIG);
	if (ret < 0) {
		return -1;
	}
	
	if ((fp = fopen(TINY_PROXY_GLOBAL_CONFIG, "w+")) == NULL) {
		return -1;
	}
#ifdef WEBWALL_DEUBG
	printf("save tiny proxy configuration informaton to file\n");
#endif 
	/* user */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "User %s\n", tiny_proxy.user_name);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* group */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Group %s\n", tiny_proxy.group);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* ip */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Listen %s\n", tiny_proxy.ip);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* port */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Port %d\n", tiny_proxy.port);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* timeout */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Timeout %d\n", tiny_proxy.timeout);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* default error file */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "DefaultErrorFile \"%s\"\n", tiny_proxy.default_error_file);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* stat file */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "StatFile \"%s\"\n", tiny_proxy.stat_file);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* log file */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "Logfile \"%s\"\n", tiny_proxy.log_file);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* log level */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "LogLevel %s\n", tiny_proxy.log_level);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* pid file */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "PidFile \"%s\"\n", tiny_proxy.pid_file);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* max client */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "MaxClients %d\n", tiny_proxy.max_clients);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* max sparse servers */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "MaxSpareServers %d\n", tiny_proxy.max_spare_servers);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* min sparse servers */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "MinSpareServers %d\n", tiny_proxy.min_spare_servers);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* start servers */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "StartServers %d\n", tiny_proxy.start_servers);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* max request per child */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "MaxRequestsPerChild %d\n", tiny_proxy.max_request_per_child);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* via proxy name */
	memset(buff, 0, MAX_BUFF_LEN);
	sprintf(buff, "ViaProxyName \"%s\"\n", tiny_proxy.via_proxy_name);
	len = strlen(buff);
	fwrite(buff, 1, len, fp);
	/* allow */
	aos_list_for_each_entry_safe(node, tmp, &(tiny_proxy.allow_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "Allow %s\n", node->data);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	/* connect port */
	aos_list_for_each_entry_safe(node, tmp, &(tiny_proxy.connect_port_head), list) {
		memset(buff, 0, MAX_BUFF_LEN);
		sprintf(buff, "ConnectPort %s\n", node->data);
		len = strlen(buff);
		fwrite(buff, 1, len, fp);
	}
	
	fclose(fp);
	
	return 0;
}
/* end*/

/*
 * save the memory data to configuration file
 */
int ww_save_webwall_data_to_file()
{
	int ret;
#ifdef WEBWALL_DEUBG
	printf("saving memory to file\n");
#endif
	ret = ww_write_global_config_file();
#ifdef WEBWALL_DEUBG
	printf("save global config successfully\n");
#endif
	ret |= ww_write_named_user_config_file();
#ifdef WEBWALL_DEUBG
	printf("save named user config successfully\n");
#endif
	ret |= ww_write_ip_user_config_file();
#ifdef WEBWALL_DEUBG
	printf("save ip user config successfully\n");
#endif
	ret |= ww_write_category_config_file();
#ifdef WEBWALL_DEUBG
	printf("save category config successfully\n");
#endif
	ret |= ww_write_policy_config_file();
#ifdef WEBWALL_DEUBG
	printf("save policy config successfully\n");
#endif
	//remove to when proxy start
	//ret |= ww_write_tiny_proxy_config_file();
	
	ret |= ww_write_antivirus_config_file();

	return ret;
}

/* 
 *save proxy config to file 
 */
int ww_save_proxy_data_to_file()
{
	return ww_write_tiny_proxy_config_file();
}

static int ww_init_tiny_proxy(void)
{
	ww_entry_t* node;
	
	strcpy(tiny_proxy.user_name, "nobody");
	strcpy(tiny_proxy.group, "nogroup");
	strcpy(tiny_proxy.ip, "127.0.0.1");
	tiny_proxy.port = 3128;
	tiny_proxy.timeout = 600;
	strcpy(tiny_proxy.default_error_file, "/usr/share/tinyproxy/default.html");
	strcpy(tiny_proxy.stat_file, "/usr/share/tinyproxy/stats.html");
	strcpy(tiny_proxy.log_file, "/var/log/tinyproxy.log");
	strcpy(tiny_proxy.log_level, "Info");
	strcpy(tiny_proxy.pid_file, "/var/run/tinyproxy.pid");
	tiny_proxy.max_clients = 100;
	tiny_proxy.max_spare_servers = 20;
	tiny_proxy.min_spare_servers = 5;
	tiny_proxy.start_servers = 10;
	tiny_proxy.max_request_per_child = 0;
	strcpy(tiny_proxy.via_proxy_name, "tinyproxy");
	/* allow */
	AOS_INIT_LIST_HEAD(&(tiny_proxy.allow_head));
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return -1;
	}
	strcpy(node->data, "127.0.0.1");
	aos_list_add_tail(&(node->list), &(tiny_proxy.allow_head));
	/* connect */
	AOS_INIT_LIST_HEAD(&(tiny_proxy.connect_port_head));
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return -1;
	}
	strcpy(node->data, "443");
	aos_list_add_tail(&(node->list), &(tiny_proxy.connect_port_head));
	AOS_INIT_LIST_HEAD(&(tiny_proxy.connect_port_head));
	node = (ww_entry_t*)malloc(sizeof(ww_entry_t));
	if (node == NULL) {
		return -1;
	}
	strcpy(node->data, "563");
	aos_list_add_tail(&(node->list), &(tiny_proxy.connect_port_head));
	
	tiny_proxy.status = T_WEBWALL_STOP;
	
	return 0;
}
/* 
 * create group 
 */
int ww_create_group(int gid)
{
	ww_filter_group_policy_t* node;

#ifdef WEBWALL_DEUBG
	printf("group id is %d", gid);
#endif 
	
	node = ww_get_filter_group_policy(gid);
	if (node != NULL) {
		return -eAosRc_WWExistGroup;
	}
	
	node = (ww_filter_group_policy_t*)malloc(sizeof(ww_filter_group_policy_t));
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	
	if (ww_init_default_policy(node, gid) < 0) 
		return -1;
	
	node->group_id = gid;
	
	policy_head_lock();
	aos_list_add_tail(&(node->list), &(policy_head));
#ifdef WEBWALL_DEUBG
	printf("success add group%d\n", node->group_id);	
#endif
	policy_head_unlock();
	
	return 0;
}

/* 
 * remove group 
 */
int ww_remove_group(int gid)
{
	ww_filter_group_policy_t* node;
	ww_category_list_t* category;
	ww_category_list_t* tmp_category;
	
	node = ww_get_filter_group_policy(gid);
	if (node == NULL) {
		return -eAosRc_WWNoexistGroup;
	}
	/* 
	 * remove the counter of category list
	 */
	aos_list_for_each_entry_safe (category, tmp_category, &(node->category_list_head), list) {
		category->ref_cout--;
	}
	
	policy_head_lock();
	aos_list_del(&(node->list));
	free(node);
	policy_head_unlock();
	
	return 0;
}

/* 
 * initalize anti-virus engine
 */
static int ww_init_av_engine()
{
	AOS_INIT_LIST_HEAD(&av_engine_head);
	
	return 0;
}


/*
 * set sock file of av engine
 * @type_name
 * @path
 */
int ww_set_av_sock(int type, char* path)
{
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp;
	
	aos_list_for_each_entry_safe(engine, tmp, &(av_engine_head), list) {
		if (engine->type == type) {
			strcpy(engine->sockfile_location, path);
#ifdef WEBWALL_DEUBG
			syslog(LOG_INFO, "AV engine path %s\n", engine->sockfile_location);
#endif
			return 0;
		}
	}
	
	return -eAosRc_WWNoexistPlugin;
}

/*
 * set naughtyness limit
 */
int ww_set_naughtyness_limit(int gid, int naughtyness)
{
	ww_filter_group_policy_t* policy;
	
	policy = ww_get_filter_group_policy(gid);
	if (policy == NULL) 
		return -1;
	
	policy->naughtyness_limit = naughtyness;
	
	return 0;
}

/*
 * add AV engine
 * @plugin_name
 */
int ww_create_av_plugin(char* plugin_name)
{
	ww_av_engine_t* node;
	ww_av_engine_t* tmp;
	int type;
	
	type = validate_engine(plugin_name);
	if (type < 0) {
		return -eAosRc_WWError;
	}
	
	aos_list_for_each_entry_safe (node, tmp, &(av_engine_head), list) {
		if (strcmp(plugin_name, node->plugname) == 0)
			return -eAosRc_WWExistPlugin;
	}
	
	node = (ww_av_engine_t* )malloc(sizeof(ww_av_engine_t));
	if (node == NULL) {
		return -eAosRc_WWError;
	}
	switch (type) {
		case T_VIRUS_CLAMAV:
			strcpy(node->plugname, "clamdscan");
			strcpy(node->file_name, "clamdscan.conf");
			break;
		case T_VIRUS_KAV:
			strcpy(node->plugname, "kavdscan");
			strcpy(node->file_name, "kavdscan.conf");
			break;
	}
	strcpy(node->sockfile_location, "");
	node->extension = ww_get_category_list("exceptionvirusextensionlist");
	node->mimetype = ww_get_category_list("exceptionvirusmimetypelist");
	node->site = ww_get_category_list("exceptionvirussitelist");
	node->url = ww_get_category_list("exceptionvirusurllist");
	node->type = type;
	
	aos_list_add_tail(&(node->list), &av_engine_head);
	
	return 0;
}

/*
 * remove AV engine
 * @plugin_name
 */
int ww_remove_av_plugin(char* plugin_name)
{
	ww_av_engine_t* node;
	ww_av_engine_t* tmp;
	
	aos_list_for_each_entry_safe (node, tmp, &(av_engine_head), list) {
		if (strcmp(plugin_name, node->plugname) == 0) {
			aos_list_del(&(node->list));
			free(node);
			return 0;
		}
	}
	
	return -eAosRc_WWNoexistPlugin;
}

/*
 * list clone
 * @src_list
 * @dst_list
 */
int ww_list_clone(char* src_list, char* dst_list)
{
	ww_category_list_t* node;
	ww_category_list_t* new_node;
	ww_entry_t* entry_node;
	ww_entry_t* tmp;
	ww_entry_t* new_entry;
	
	if ((node = ww_get_category_list(src_list)) == NULL)
		return -eAosRc_WWNoexistListName;
	
	new_node = (ww_category_list_t* )malloc(sizeof(ww_category_list_t));
	if (new_node == NULL)
		return -eAosRc_WWError;
	
	new_node->list_type = node->list_type;
	strcpy(new_node->path, dst_list);
	new_node->tc = NULL;
	new_node->category_alias = NULL;
	new_node->ref_cout = 0;
	new_node->is_system = 0;
	AOS_INIT_LIST_HEAD(&(new_node->include_list_head));
	AOS_INIT_LIST_HEAD(&(new_node->entry_list_head));
	AOS_INIT_LIST_HEAD(&(new_node->guard_entry_file_list_head));
	
	/* start to clone */
	/* time control */
	if (node->tc != NULL) {
		new_node->tc = (ww_time_control_t* )malloc(sizeof(ww_time_control_t));
		if (new_node->tc == NULL) {
			free(new_node);
			return -eAosRc_WWError;
		}
		new_node->tc->start_hour = node->tc->start_hour;
		new_node->tc->start_minute = node->tc->start_minute;
		new_node->tc->end_hour = node->tc->end_hour;
		new_node->tc->end_minute = node->tc->end_minute;
		strcpy(new_node->tc->days, node->tc->days);
	}
 	/* alias */
	/* include list */
	aos_list_for_each_entry_safe (entry_node, tmp, &(node->include_list_head), list) {
		new_entry = (ww_entry_t* )malloc(sizeof(ww_entry_t));
		if (new_entry == NULL) {
			return -eAosRc_WWError;
		}
		new_entry->type = entry_node->type;
		strcpy(new_entry->data, entry_node->data);
		aos_list_add_tail(&(new_entry->list), &(new_node->include_list_head));	
	}	
	/* entry list */
	aos_list_for_each_entry_safe (entry_node, tmp, &(node->entry_list_head), list) {
		new_entry = (ww_entry_t* )malloc(sizeof(ww_entry_t));
		if (new_entry == NULL) {
			return -eAosRc_WWError;
		}
		new_entry->type = entry_node->type;
		strcpy(new_entry->data, entry_node->data);
		aos_list_add_tail(&(new_entry->list), &(new_node->entry_list_head));	
	}
	/* guard entry list */
	aos_list_for_each_entry_safe (entry_node, tmp, &(node->guard_entry_file_list_head), list) {
		new_entry = (ww_entry_t* )malloc(sizeof(ww_entry_t));
		if (new_entry == NULL) {
			return -eAosRc_WWError;
		}
		new_entry->type = entry_node->type;
		strcpy(new_entry->data, entry_node->data);
		aos_list_add_tail(&(new_entry->list), &(new_node->guard_entry_file_list_head));	
	}	

	category_list_head_lock();
	aos_list_add_tail(&(new_node->list), &category_list_head);
	category_list_head_unlock();

	return 0;
}



/*
 * initalize webwall defalut setting
 */
int ww_init(void)
{
	int ret;

	ret = ww_init_global();
	ret |= ww_init_category();
	ret |= ww_init_policy();
	ret |= ww_init_user();
	ret |= ww_init_tiny_proxy();
	ret |= ww_init_av_engine();
#ifdef WEBWALL_DEUBG
	if (ret < 0) {
		printf("failed to initialize webwall\n");
	} else {
		printf("initialize webwall successfully\n");
	}
#endif

	return ret;
}

/* 
 *clear config
 */
int ww_clear_config(void)
{
	ww_category_list_t* category;
	ww_category_list_t* tmp_category;
	ww_entry_t* entry;
	ww_entry_t* tmp_entry;
	ww_filter_group_policy_t* policy;
	ww_filter_group_policy_t* tmp_policy;
	ww_filter_group_list_t* filter_group_list;
	ww_filter_group_list_t* tmp_filter_group_list;
	ww_filter_name_based_user_t* name_user;
	ww_filter_name_based_user_t* tmp_name_user;
	ww_filter_ip_based_user_t* ip_user;
	ww_filter_ip_based_user_t* tmp_ip_user;
	ww_av_engine_t* engine;
	ww_av_engine_t* tmp_engine;
	
	/* global */
	aos_list_for_each_entry_safe(entry, tmp_entry, &(global.auth_plugin_head), list) {
		aos_list_del(&(entry->list));
		free(entry);
	}
	aos_list_for_each_entry_safe(entry, tmp_entry, &(global.download_manager_head), list) {
		aos_list_del(&(entry->list));
		free(entry);
	}
	ww_init_global();
	
	/* named user*/
	filter_name_based_user_head_lock();
	aos_list_for_each_entry_safe(name_user, tmp_name_user, &(filter_name_based_user_head), list) {
		aos_list_del(&(name_user->list));
		free(name_user);
	}
	filter_name_based_user_head_unlock();
	
	/* ip user */
	filter_ip_based_user_head_lock();
	aos_list_for_each_entry_safe(ip_user, tmp_ip_user, &(filter_ip_based_user_head), list) {
		aos_list_del(&(ip_user->list));
		free(ip_user);
	}
	filter_ip_based_user_head_unlock();
	//ww_init_user();
	
	/* category */
	category_list_head_lock();
	aos_list_for_each_entry_safe(category, tmp_category, &(category_list_head), list) {
		aos_list_del(&(category->list));
		free(category);
	}
	category_list_head_unlock();
	//ww_init_category();
	
	/* filter group policy */	
	policy_head_lock();
	aos_list_for_each_entry_safe(policy, tmp_policy, &(policy_head), list) {
		aos_list_for_each_entry_safe(filter_group_list, tmp_filter_group_list, &(policy->category_list_head), list) {
			aos_list_del(&(filter_group_list->list));
			free(filter_group_list);
		}
		aos_list_del(&(policy->list));
	}
	policy_head_unlock();
	//ww_init_policy();
	
	/* tiny proxy*/
	aos_list_for_each_entry_safe(entry, tmp_entry, &(tiny_proxy.allow_head), list) {
		aos_list_del(&(entry->list));
		free(entry);
	}
	aos_list_for_each_entry_safe(entry, tmp_entry, &(tiny_proxy.connect_port_head), list) {
		aos_list_del(&(entry->list));
		free(entry);
	}
	//ww_init_tiny_proxy();
	
	/* anti-virus */
	aos_list_for_each_entry_safe (engine, tmp_engine, &(av_engine_head), list) {
		aos_list_del(&(engine->list));
		free(engine);
	}
	//ww_init_av_engine();
	
	return 0;
}



