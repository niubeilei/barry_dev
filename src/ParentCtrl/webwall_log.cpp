#include "ParentCtrl/webwall_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef WEBWALL_DEUBG
#include <syslog.h>
#endif

char* ww_get_log(int log_item)
{
	char buff[MAX_BUFF_LEN];
	FILE* fp;
	char* result;
	int total_item = 0;
	int current_item = 0;
	int cache_len = 0;
	int total_len = 0;
	int current_len = 0;
	
	if ((log_item > MAX_LOG_ITEM) 
		|| (log_item <= 0))
		return NULL;
	
	if ((fp = fopen(LOG_LOCATION, "r")) == NULL) {
		return NULL;
	}
	
	if ((result = (char* )malloc(sizeof(char) * MAX_BUFF_LEN)) == NULL) { /* initialize by minimum size */
		fclose(fp);
		return NULL;
	}
	cache_len = MAX_BUFF_LEN;
	result[0] = '\0';
	
	while (!feof(fp)) {
		fgets(buff, MAX_BUFF_LEN, fp);
		total_item++;
	}
	
#ifdef WEBWALL_DEUBG
	syslog(LOG_INFO, "ww_get_log: total item is %d, log item is %d\n", total_item, log_item);
#endif

	if (total_item <= 0) {
		fclose(fp);
		free(result);
		return NULL;
	}
	
	rewind(fp);
	while (!feof(fp)) {
		memset(buff, 0, MAX_BUFF_LEN);
		current_len = 0;
		fgets(buff, MAX_BUFF_LEN, fp);
		current_len = strlen(buff);
		current_item++;
		if (current_len <= 0) {
			break;
		} 
		
		if ((total_item <= log_item)
			|| ((total_item - current_item) <= log_item)) {
			
			if ((total_len + current_len) > MAX_SHOW_LEN)
				break;
			total_len += current_len;
			if (total_len > cache_len) {
alloc_cache:	if ((result = (char* )realloc(result, cache_len + MAX_BUFF_LEN)) == NULL) {
					fclose(fp);
					free(result);
#ifdef WEBWALL_DEUBG
					syslog(LOG_INFO, "ww_get_log: failed to realloc memory\n");
#endif
					return NULL;
				}
#ifdef WEBWALL_DEUBG
				syslog(LOG_INFO, "ww_get_log: realloc memory\n");
#endif
				cache_len += MAX_BUFF_LEN;
				if (cache_len < total_len) {
					goto alloc_cache;
				}
			}
			strcat(result, buff);		
		}
	}
	
	fclose(fp);
	
	return result;
}

char* ww_get_stat()
{
	char* result;
	FILE* fp;
	int len;
	char buff[MAX_BUFF_LEN];
	
	if ((fp = fopen(STAT_LOCATION, "r")) == NULL) {
		return NULL;
	}
	
	memset(buff, 0, MAX_BUFF_LEN);
	result = (char* )malloc(sizeof(char) * MAX_BUFF_LEN);
	if (result == NULL) {
		return NULL;
	}
	len = fread(result, 1, MAX_BUFF_LEN - 1, fp);
	if (len <= 0)
		return NULL; 
	result[len] = 0;
	
	return result;
}

int ww_parse_line(const char* line, struct tm* start_time, struct tm* end_time, char* keywords)
{
	char buff[MAX_BUFF_LEN];
	char* p;
	char* q;
	char* r;
	struct tm t;
	
	memset(buff, 0, MAX_BUFF_LEN);
	strcpy(buff, line);
	
	if (sscanf(buff, "%d.%d.%d %d:%d", 
		&(t.tm_year),
		&(t.tm_mon),
		&(t.tm_mday),
		&(t.tm_hour),
		&(t.tm_min)) == 4) {
		return -1;
	}
	t.tm_year -= 1900;
	if(mktime(&t) == -1) {
		return -1;
	}
	else if(mktime(&t) < mktime(start_time)) {
		return -1;
	}
	else if(mktime(&t) > mktime(end_time)) {
		return -1;
	} 
	
	r = buff + strlen("YYYY.MM.DD HH:MM:SS"); 
	p = strtok(keywords, "|");
	if (p == NULL) {
		q = strstr(r, keywords);
		if (q != NULL)
			return 0;
	}
	while (p != NULL) {
		q = strstr(r, p);
		if (q != NULL)
			return 0;
		p = strtok(NULL, "|");
	}
	
	return -1;
}

char* ww_search_log(struct tm* start_time, struct tm* end_time, char* keywords)
{
	char buff[MAX_BUFF_LEN];
	FILE* fp;
	char* result;
	int cache_len = 0;
	int total_len = 0;
	int current_len = 0;
	
	if ((fp = fopen(LOG_LOCATION, "r")) == NULL) {
		return NULL;
	}
	
	if ((result = (char* )malloc(sizeof(char) * MAX_BUFF_LEN)) == NULL) { /* initialize by minimum size */
		fclose(fp);
		return NULL;
	}
	cache_len = MAX_BUFF_LEN;
	result[0] = '\0';
	
	while (!feof(fp)) {
		memset(buff, 0, MAX_BUFF_LEN);
		current_len = 0;
		fgets(buff, MAX_BUFF_LEN, fp);
		current_len = strlen(buff); 
		if (current_len <= 0) {
			break;
		}
		if ((total_len + current_len) > MAX_SHOW_LEN) {
			break;
		}
		if (ww_parse_line(buff, start_time, end_time, keywords) >= 0) {
			if ((total_len + current_len) > MAX_SHOW_LEN) {
				break;
			}
			total_len += current_len;
			if (total_len > cache_len) {
alloc_cache:	if ((result = (char* )realloc(result, cache_len + MAX_BUFF_LEN)) == NULL) {
					fclose(fp);
					free(result);
					printf("no memory\n");
					return NULL;
				}
				cache_len += MAX_BUFF_LEN;
				if (cache_len < total_len) {
					goto alloc_cache;
				}
			}
			strcat(result, buff);
		}
			
	}
	
	fclose(fp);
	
	return result;
}

