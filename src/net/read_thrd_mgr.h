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
// 03/01/2008 : Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef aos_net_read_thread_mgr_h
#define aos_net_read_thread__mgrh

#include "net/conn.h"
#include "porting_c/mutex.h"
#include "util_c/dyn_array.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>


struct aos_read_thrd_mgr;
struct aos_conn;
struct aos_read_thrd;


#define AOS_READ_THRD_MGR_DEFAULT_GROUP_SIZE 30
#define AOS_READ_THRD_MGR_DEFAULT_MAX_THRDS  50
#define AOS_READ_THRD_MGR_MAGIC 23564256

typedef int (*aos_read_thrd_mgr_add_conn_t)(
		struct aos_read_thrd_mgr *mgr,
		aos_conn_read_callback_t callback,
		struct aos_conn *conn);

typedef int (*aos_read_thrd_mgr_del_conn_t)(
		struct aos_read_thrd_mgr *mgr,
		struct aos_conn *conn);


#define AOS_READ_THREAD_MGR_MEMFUNC_DECL				\
	aos_read_thrd_mgr_add_conn_t		add_conn;		\
	aos_read_thrd_mgr_del_conn_t		del_conn


#define AOS_READ_THREAD_MGR_MEMDATA_DECL				\
	int						group_size;					\
	struct aos_read_thrd **	thrds;						\
	aos_dyn_array_t			thrds_array;				\
	int						magic;						\
	aos_lock_t *			lock;						\
	int						max_thrds


typedef struct aos_read_thrd_mgr_mf
{
	AOS_READ_THREAD_MGR_MEMFUNC_DECL;
} aos_read_thrd_mgr_mf_t;

typedef struct aos_read_thrd_mgr
{
	aos_read_thrd_mgr_mf_t *mf;

	AOS_READ_THREAD_MGR_MEMDATA_DECL;
} aos_read_thrd_mgr_t;


#ifdef __cplusplus
extern "C" {
#endif

extern int aos_read_thrd_mgr_init(aos_read_thrd_mgr_t *rt);
extern aos_read_thrd_mgr_t *aos_read_thrd_mgr_create();

extern int aos_rdthdmgr_add_conn(
		aos_conn_read_callback_t callback, 
		struct aos_conn *conn);

extern int aos_rdthdmgr_del_conn(struct aos_conn *conn);

#ifdef __cplusplus
}
#endif


#endif


