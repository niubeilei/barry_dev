////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: WebWall.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __WEB_WALL_H__
#define __WEB_WALL_H__


#define MAX_ACCESS_TIME_ENTRIES 5
#define MAX_DENYPAGE_PATH_LENGTH 256
#define MAX_DENYPAGE_LENGTH 256*1024

typedef enum web_wall_ctl{
	eWebWall_Ctl_Forward = 0,
	eWebWall_Ctl_Block = 1,
} web_wall_ctl_t;

typedef enum {
	BLACK_LIST_TYPE_URL = 0,
	BLACK_LIST_TYPE_DN,
	BLACK_LIST_TYPE_IP,
	BLACK_LIST_TYPE_MAX
} eblack_list_type;

typedef struct black_list_tab{
	unsigned char *key_word;
	eblack_list_type type;
	struct black_list_tab *next;
} black_list_tab_t;

typedef struct webwall_api_s{
	int (*is_enable)(void);
	int (*url_filter)(unsigned char* uri, int uri_len,unsigned char * host, int host_len);
	int (*time_control)(void);
	char* (*get_deny_page)(void);
}webwall_api_t;

typedef struct webwall_access_time{
	unsigned long start_time;
	unsigned long end_time;
} webwall_access_time_t;

extern struct aosUrlPtree *gblack_list;
extern struct aosUrlPtree *gblack_list_dn;
extern struct webwall_access_time gaccess_time[MAX_ACCESS_TIME_ENTRIES];
extern char* gwebwall_denypage;
extern char gwebwall_denypage_path[MAX_DENYPAGE_PATH_LENGTH];

extern int AosWebWall_isEnabled(void);
extern int AosWebWall_urlFilter( unsigned char *uri, int uri_len, unsigned char * host, int host_len );
extern char* AosWebWall_getDenyPage(void);
extern int AosWebWall_timeControl(void);
extern int AosWebWall_createDenyPage( char* body, int body_len );	
extern unsigned long AosAddrStrToLong( char* ip_input );
extern int AosLongToBinStr( unsigned long ip_addr, char* ip_keyword_out, int out_len );
extern int AosWebWall_ipKeywordCheck( char* ip_input, char* ip_keyword_out, int out_len );

#endif // #ifndef __WEB_WALL_H__

