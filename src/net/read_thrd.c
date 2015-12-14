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
#include "net/read_thrd.h"

#include "alarm_c/alarm.h"
#include "net/conn.h"
#include "porting_c/mutex.h"
#include "porting_c/get_errno.h"
#include "porting_c/select.h"
#include "porting_c/sleep.h"
#include "thread_c/thread.h"
#include "util_c/memory.h"
#include "util_c/strutil.h"
#include "util_c/tracer.h"
#include <sys/time.h>

extern void aos_read_thrd_thread_func(aos_thread_t *thread);

aos_read_thrd_entry_t *chending;

static int aos_read_thrd_integrity_check(aos_read_thrd_t *rt) 
{
	aos_assert_r(rt, -1);
	aos_assert_r(rt->thread, -1);
	aos_assert_r(rt->magic == AOS_READ_THRD_MAGIC, -1);
	aos_assert_r(rt->lock, -1);
	return 1;
}


static int aos_read_thrd_set_name(
		aos_read_thrd_t *rt, 
		const char * const name)
{
	aos_assert_r(rt, -1);
	aos_assert_r(name, -1);

	if (rt->name) aos_free(rt->name);
	aos_assert_r(!aos_str_set(&rt->name, name, strlen(name)), -1);
	return 0;
}


static int aos_read_thrd_start(struct aos_read_thrd *rt)
{
	aos_assert_r(rt, -1);
	aos_assert_r(!rt->thread, -1);

	rt->thread = aos_thread_create(rt->name, 0,
			aos_read_thrd_thread_func, 
			rt, 1, 1, 0);
	aos_assert_r(rt->thread, -1);
	return 0;
}


void aos_read_thrd_thread_func(aos_thread_t *thread)
{
	int i;
	int is_eof;

	aos_assert(thread);
	aos_assert(thread->user_data);
	aos_read_thrd_t *self = (aos_read_thrd_t *)thread->user_data;
	aos_assert(self->magic == AOS_READ_THRD_MAGIC);
	aos_assert(self->mf->integrity_check(self) == 1);

	aos_trace("Start read thread");
	aos_conn_t *conns[AOS_READ_THRD_DEFAULT_MAX_CONNS];
	aos_conn_read_callback_t callbacks[AOS_READ_THRD_DEFAULT_MAX_CONNS];
	while (1)
	{
		// To select
		struct timeval timer;
		timer.tv_sec = 1;
		timer.tv_usec = 0;
		fd_set fds;
		aos_lock(self->lock);
	    fds = self->fds;
		aos_unlock(self->lock);
		int ret = aos_socket_select(self->max_fd, &fds, 0, 0, &timer);
		if (ret < 0)
		{
			int en = aos_get_errno();
			switch (en)
			{
			case EINTR:
			case EINVAL:
				 break;

			case EBADF:
				 aos_alarm("Bad connections found");
				 self->mf->check_conns(self);
				 break;

			default:
				 aos_alarm("Select error: %d", aos_get_errno());
				 aos_sleep(1);
				 break;
			}
			continue;
		}

		// Select successful
		aos_lock(self->lock);
		aos_read_thrd_entry_t *entry, *tmp;
		int idx = 0;
		aos_list_for_each_entry_safe(entry, tmp, &self->conns, link)
		{
			if (FD_ISSET(entry->conn->sock, &fds))
			{
				// There is something for the sock to read
				conns[idx] = entry->conn;
				callbacks[idx] = entry->callback;
				entry->conn->mf->hold(entry->conn);
				idx++;
			}
		}	
		aos_unlock(self->lock);

		// To actually read the connections
		for (i=0; i<idx; i++)
		{
			int is_conn_broken;
			ret = conns[i]->mf->read(
					conns[i], 0, 1000, 
					callbacks[i],
					&is_eof, 
					&is_conn_broken);
			if (ret == 0 && (is_eof || is_conn_broken))
			{
            	aos_trace("EOF received on: %d", 
						conns[i]->sock);
				aos_assert(!self->mf->del_conn(self, 
							conns[i]->sock));
				i--;
			}
			conns[i]->mf->put(conns[i]);
		}
	}
}


static int aos_read_thrd_add_conn(
		struct aos_read_thrd *rt, 
		aos_conn_read_callback_t callback,
		aos_conn_t *conn) 
{
	aos_assert_r(rt, -1);
	aos_assert_r(conn, -1);
	aos_assert_r(conn->sock > 0, -1);
	aos_assert_r(callback, -1);

	aos_lock(rt->lock);
	aos_read_thrd_entry_t *entry = aos_malloc(sizeof(*entry));
	AOS_INIT_LIST_HEAD(&entry->link);
	conn->mf->hold(conn);
	entry->conn = conn;
	entry->callback = callback;
	aos_list_add_tail(&entry->link, &rt->conns);
	rt->num_conns++;

	FD_SET(conn->sock, &rt->fds);
	if (rt->max_fd < conn->sock + 1)
	{
		rt->max_fd = conn->sock + 1;
	}

	aos_unlock(rt->lock);
	return 0;
}


static int aos_read_thrd_set_fds(aos_read_thrd_t *rt)
{
	aos_assert_r(rt, -1);

	aos_lock(rt->lock);
	rt->max_fd = 0;
	aos_read_thrd_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &rt->conns, link)
	{
		if (rt->max_fd < entry->conn->sock + 1)
		{
			rt->max_fd = entry->conn->sock + 1;
		}
	}
	aos_unlock(rt->lock);
	return 0;
}


static int aos_read_thrd_del_conn(
		struct aos_read_thrd *rt, 
		const int sock) 
{
	aos_assert_r(rt, -1);

	aos_lock(rt->lock);
	aos_read_thrd_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &rt->conns, link)
	{
		if (entry->conn->sock == sock)
		{
			aos_conn_t *conn = entry->conn;
			conn->mf->put(conn);
			aos_list_del(&entry->link);
			aos_free(entry);
			rt->num_conns--;
			aos_unlock(rt->lock);
			return aos_read_thrd_set_fds(rt);
		}
	}
	return 0;
}


static int aos_read_thrd_check_conns(aos_read_thrd_t *rt)
{
    //
    // This function is called when a bad file descriptor is found.
    // All connections are saved in mConnections. This function simply
    // go over all the connections in mConnections. If a connection is
    // bad, it removes it.
    //
	aos_assert_r(rt, -1);

	aos_lock(rt->lock);
	aos_conn_t *conn;
	aos_read_thrd_entry_t *entry, *tmp;
	aos_list_for_each_entry_safe(entry, tmp, &rt->conns, link)
	{
		conn = entry->conn;
        if (conn->mf->is_conn_good(conn) != 1)
        {
            aos_trace("Bad connection found: %d", conn->sock);
			conn->mf->put(conn);
			aos_list_del(&entry->link);
        }
    }
	aos_unlock(rt->lock);

	return aos_read_thrd_set_fds(rt);
	return 0;
}


int aos_read_thrd_is_full(aos_read_thrd_t *rt)
{
	aos_assert_r(rt, -1);
	return (rt->num_conns >= rt->max_conns)?1:0;
}


static aos_read_thrd_mf_t sg_mf = 
{
	aos_read_thrd_set_name,
	aos_read_thrd_check_conns,
	aos_read_thrd_integrity_check,
	aos_read_thrd_start,
	aos_read_thrd_add_conn,
	aos_read_thrd_del_conn,
	aos_read_thrd_is_full
};


int aos_read_thrd_init(aos_read_thrd_t *rt)
{
	aos_assert_r(rt, -1);
	memset(rt, 0, sizeof(aos_read_thrd_t));
	rt->mf = &sg_mf;
	rt->max_conns = AOS_READ_THRD_DEFAULT_MAX_CONNS;
	AOS_INIT_LIST_HEAD(&rt->conns);
	rt->magic = AOS_READ_THRD_MAGIC;
	FD_ZERO(&rt->fds);
	rt->lock = aos_malloc(sizeof(aos_lock_t));
	aos_assert_r(rt->lock, -1);
	aos_assert_r(!aos_init_lock(rt->lock), -1);
	return 0;
}


aos_read_thrd_t *aos_read_thrd_create()
{
	aos_read_thrd_t *obj = aos_malloc(sizeof(aos_read_thrd_t));
	aos_assert_r(obj, 0);

	aos_assert_g(!aos_read_thrd_init(obj), cleanup);
	return obj;

cleanup:
	aos_free(obj);
	return 0;
}


