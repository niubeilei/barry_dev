#ifndef WEBWALL_LOG_H
#define WEBWALL_LOG_H

extern char* ww_get_log(int log_item);
extern char* ww_get_stat();
extern char* ww_search_log(struct tm* start_time, struct tm* end_time, char* keywords);

#endif

