////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 02/24/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_net_read_thread_h
#define aos_net_read_thread_h

#include "net/conn.h"
#include "porting_c/mutex.h"
#include "util_c/list.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


#define AOS_READ_THRD_MAGIC 22346367
#define AOS_READ_THRD_DEFAULT_MAX_CONNS 50

struct aos_read_thrd;
struct aos_action;
struct aos_conn;

typedef int (*aos_read_thrd_set_name_t)(
		struct aos_read_thrd *rt, 
		const char * const name);

typedef int (*aos_read_thrd_check_conns_t)(
		struct aos_read_thrd *rt);

typedef int (*aos_read_thrd_integrity_check_t)(
		struct aos_read_thrd *rt);

typedef int (*aos_read_thrd_start_t)(
		struct aos_read_thrd *rt);

typedef int (*aos_read_thrd_add_conn_t)(
		struct aos_read_thrd *rt, 
		aos_conn_read_callback_t callback,
		struct aos_conn *conn); 

typedef int (*aos_read_thrd_del_conn_t)(
		struct aos_read_thrd *rt, 
		const int sock); 

typedef int (*aos_read_thrd_is_full_t)(struct aos_read_thrd *rt);

typedef struct aos_read_thrd_entry
{
	aos_list_head_t				link;
	aos_conn_read_callback_t	callback;
	struct aos_conn *			conn;
} aos_read_thrd_entry_t;


#define AOS_READ_THREAD_MEMFUNC_DECL						\
	aos_read_thrd_set_name_t			set_name;			\
	aos_read_thrd_check_conns_t			check_conns;		\
	aos_read_thrd_integrity_check_t		integrity_check;	\
	aos_read_thrd_start_t				start;				\
	aos_read_thrd_add_conn_t			add_conn;			\
	aos_read_thrd_del_conn_t			del_conn;			\
	aos_read_thrd_is_full_t				is_full


#define AOS_READ_THREAD_MEMDATA_DECL				\
	char *						name;				\
	struct aos_thread *			thread;				\
	aos_list_head_t				conns;				\
	int							magic;				\
	fd_set 						fds;				\
	int							max_fd;				\
	aos_lock_t *				lock;				\
	int							num_conns;			\
	int							max_conns



typedef struct aos_read_thrd_mf
{
	AOS_READ_THREAD_MEMFUNC_DECL;
} aos_read_thrd_mf_t;

typedef struct aos_read_thrd
{
	aos_read_thrd_mf_t *mf;

	AOS_READ_THREAD_MEMDATA_DECL;
} aos_read_thrd_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_read_thrd_init(aos_read_thrd_t *rt);
extern aos_read_thrd_t *aos_read_thrd_create();

#ifdef __cplusplus
}
#endif


#endif


