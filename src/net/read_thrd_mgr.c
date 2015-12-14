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
#include "net/read_thrd_mgr.h"

#include "alarm_c/alarm.h"
#include "net/read_thrd.h"
#include "util_c/memory.h"
#include "util_c/global_data.h"
#include "util_c/tracer.h"


static aos_read_thrd_mgr_t *sg_self = 0;
static int					sg_init_flag = 0;
static aos_lock_t *			sg_lock = 0;

int aos_rdthdmgr_init()
{
	aos_global_lock();
	if (sg_init_flag)
	{
		aos_global_unlock();
		return 0;
	}

	sg_self = aos_read_thrd_mgr_create();
	aos_assert_g(sg_self, cleanup);
	sg_lock = aos_malloc(sizeof(*sg_lock));
	aos_init_lock(sg_lock);
	sg_init_flag = 1;
	aos_global_unlock();
	return 0;

cleanup:
	aos_global_unlock();
	return -1;
}


int aos_rdthdmgr_add_conn(
		aos_conn_read_callback_t callback, 
		aos_conn_t *conn)
{
	if (!sg_init_flag) aos_assert_r(!aos_rdthdmgr_init(), -1);
	aos_assert_r(!sg_self->mf->add_conn(sg_self, 
			callback, conn), -1);
	return 0;
}


int aos_read_thrd_mgr_add_conn(
		struct aos_read_thrd_mgr *mgr,
		aos_conn_read_callback_t callback,
		struct aos_conn *conn) 
{
	int i;
	aos_assert_r(mgr, -1);
	aos_assert_r(conn, -1);
	aos_assert_r(callback, -1);
	aos_assert_r(mgr->lock, -1);

	aos_lock(mgr->lock);
	for (i=0; i<mgr->thrds_array.noe; i++)
	{
		if (!mgr->thrds[i]->mf->add_conn(mgr->thrds[i], 
					callback, conn))
		{
			// Added
			aos_unlock(mgr->lock);
			return 0;
		}
	}

	// Need to create a new thread. Check whether it can add
	aos_assert_g(mgr->thrds_array.noe < mgr->max_thrds, error);
	aos_read_thrd_t *thrd = aos_read_thrd_create();
	aos_assert_g(thrd, error);

	aos_assert_g(!thrd->mf->add_conn(thrd, callback, conn), error);
	aos_assert_g(!mgr->thrds_array.mf->add_element(&mgr->thrds_array, 
				1, (char **)&mgr->thrds), error);
	mgr->thrds[mgr->thrds_array.noe++] = thrd;
aos_trace("Connection added");
	aos_assert_g(!thrd->mf->start(thrd), error);
aos_trace("Read thread started");
	aos_unlock(mgr->lock);
	return 0;

error:
	aos_unlock(mgr->lock);
	return -1;
}


int aos_rdthdmgr_del_conn(aos_conn_t *conn)
{
	if (!sg_init_flag) aos_assert_r(!aos_rdthdmgr_init(), -1);
	aos_assert_r(!sg_self->mf->del_conn(sg_self, conn), -1);
	return 0;
}


int aos_read_thrd_mgr_del_conn(
		struct aos_read_thrd_mgr *mgr,
		struct aos_conn *conn)
{
	int i;
	aos_assert_r(mgr, -1);
	aos_assert_r(conn, -1);
	aos_assert_r(mgr->lock, -1);

	aos_lock(mgr->lock);
	for (i=0; i<mgr->thrds_array.noe; i++)
	{
		if (!mgr->thrds[i]->mf->del_conn(mgr->thrds[i], conn->sock))
		{
			// deleted 
			aos_unlock(mgr->lock);
			return 0;
		}
	}

	// 
	// Did not find
	//
	aos_unlock(mgr->lock);
	aos_alarm("To delete a connection from a read thread mgr,"
			" but not found!");
	return -1;
}


static aos_read_thrd_mgr_mf_t sg_mf = 
{
	aos_read_thrd_mgr_add_conn,
	aos_read_thrd_mgr_del_conn
};


int aos_read_thrd_mgr_init(aos_read_thrd_mgr_t *mgr)
{
	aos_assert_r(mgr, -1);
	mgr->group_size = AOS_READ_THRD_MGR_DEFAULT_GROUP_SIZE;
	mgr->max_thrds = AOS_READ_THRD_MGR_DEFAULT_MAX_THRDS;
	mgr->mf = &sg_mf;
	mgr->magic = AOS_READ_THRD_MGR_MAGIC;

	aos_assert_r(!aos_dyn_array_init(&mgr->thrds_array, 
			(char **)&mgr->thrds, 
			sizeof(aos_read_thrd_t *), 3, 
			AOS_READ_THRD_MGR_DEFAULT_MAX_THRDS), -1);
	if (!mgr->lock)
	{
		mgr->lock = aos_malloc(sizeof(aos_lock_t));
		aos_assert_r(mgr->lock, -1);
		aos_assert_r(!aos_init_lock(mgr->lock), -1);
	}
	return 0;
}


aos_read_thrd_mgr_t *aos_read_thrd_mgr_create()
{
	aos_read_thrd_mgr_t *mgr = aos_malloc(sizeof(aos_read_thrd_mgr_t));
	aos_assert_r(mgr, 0);
	memset(mgr, 0, sizeof(aos_read_thrd_mgr_t));
	aos_assert_g(!aos_read_thrd_mgr_init(mgr), cleanup);
	return mgr;

cleanup:
	aos_free(mgr);
	return 0;
}


