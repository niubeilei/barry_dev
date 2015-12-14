////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: util.h
// Description:
//   
//
// Modification History:
// 2007-03-07 Created
////////////////////////////////////////////////////////////////////////////

#ifndef aos_wanCM_util_h
#define aos_wanCM_util_h

#include "rhcUtil/aosResMgrDefines.h"
#include "Util/String.h"

#include <pthread.h>

#define MAX_TIME_INTERVAL 120
#define MAX_IP_ADDR 16

struct wanConfigEntry
{
	OmnString ad_user;
	OmnString ad_passwd;
	OmnString dns1;
	OmnString dns2;
	OmnString ip;
	OmnString netmask;
	OmnString method;
	int       hc_time;
	OmnString hc_ip;
	OmnString status;
} ;
int wanInit();

typedef struct wanCM_thread_struct
{
	char param1[MAX_IP_ADDR];
	char param2[16];
}wanCM_thread_struct_t;

struct WanCM_App
{
	char m_dip[MAX_IP_ADDR];
	int m_interval;
	struct wanConfigEntry m_sgWan;	// Wan configuration value
	
	// something about thread
	BOOL 			m_switch_on ;	// = TRUE;
	pthread_cond_t 	m_thread_stopped;
	pthread_t 		m_thread;
	pthread_mutex_t m_mutex;	// global mutex lock!
	wanCM_thread_struct_t m_thread_input;
};

void reset_WanCM_App(struct WanCM_App * const ptrApp);

extern int wanPppoe(OmnString &username, OmnString &password, OmnString &dns1, OmnString &dns2);

extern int wanStatic(OmnString &ip, OmnString &netmask);

extern  int wanDhcp();

extern int wanStatusOff();

// Declare global variables
extern struct WanCM_App g_theWanCMApp;

//void Setsockopt(int, int, int, const void *, socklen_t);
//void Gettimeofday(struct timeval *, void *);
//struct addrinfo *Host_serv(const char *, const char *, int, int);
//int Socket(int, int, int);
//void sock_set_port(struct sockaddr *, socklen_t, int);
//void Sendto(int, const void *, size_t, int, const struct sockaddr *, socklen_t);
//void Bind(int, const struct sockaddr *, socklen_t);
int 
traceloop(char * remote_addr);

//int wanCM_thread_func(int argc, char **argv)
void wanCM_thread_func(void * ptr);

// start or kill the thread 
BOOL wanCM_start_thread();
BOOL wanCM_kill_thread();

#endif // aos_wanCM_util_h
