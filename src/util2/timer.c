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
// 03/14/2008 Copied from Util1/Timer.cpp by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "util2/timer.h"

#include "alarm_c/alarm.h"
#include "net/udp.h"
#include "porting_c/mutex.h"
#include "porting_c/time_of_day.h"
#include "porting_c/addr.h"
#include "porting_c/socket.h"
#include "porting_c/select.h"
#include "thread_c/thread.h"
#include "util2/global_data.h"
#include "util_c/memory.h"
#include "util_c/tracer.h"
#include <stdio.h>
#include <stdlib.h>

static int sgTimerId = 10;
static aos_lock_t *sg_lock = 0;
static int sg_max_fd = 0;
static aos_thread_t *sg_thread = 0;
static u32 sg_local_addr = 0;
static u16 sg_port = 0;
static aos_udp_t *sg_reader = 0;
static aos_udp_t *sg_writer = 0;
static struct timeval sg_timeval;
static int sg_init_flag = 0;
static aos_list_head_t sg_timers;
static fd_set sg_read_fds;
static int sg_timer_min_usec = 10000;		// 10ms

// 
// Description:
//
// Returns:
// 1 if time1 > time2
// 0 if time1 == time2
// -1 if time1 < time2
int aos_time_comp( struct timeval *time1, struct timeval *time2)
{
	//
	// It compares two timeval
	//
    if (time1->tv_sec > time2->tv_sec) return 1;

    if (time1->tv_sec == time2->tv_sec)
	{
		if (time1->tv_usec > time2->tv_usec) return 1;
		if (time1->tv_usec == time2->tv_usec) return 0;
        return -1;
	}
 
    return -1;
}
 
 
// 
// Description:
// result = time1 + time2
//
int aos_time_add(
		struct timeval *result, 
		struct timeval *time1, 
		struct timeval *time2)
{
	aos_assert_r(result, -1);
	aos_assert_r(time1, -1);
	aos_assert_r(time2, -1);

    result->tv_usec = time1->tv_usec + time2->tv_usec;
    if (result->tv_usec >= 1000000)
    {
        result->tv_usec -= 1000000;
		result->tv_sec++;
    }
    result->tv_sec = time1->tv_sec + time2->tv_sec;
	return 0;
}


// 
// Description 
// result = time1 - time2
//
int aos_time_minus(
		struct timeval *result, 
		struct timeval *time1, 
		struct timeval *time2)
{
	aos_assert_r(result, -1);
	aos_assert_r(time1, -1);
	aos_assert_r(time2, -1);

    if (time2->tv_usec > time1->tv_usec)
    {
        result->tv_usec = time1->tv_usec + 1000000 - time2->tv_usec;
        result->tv_sec  = time1->tv_sec - time2->tv_sec - 1;
    }
    else
    {
        result->tv_usec = time1->tv_usec - time2->tv_usec;
        result->tv_sec  = time1->tv_sec  - time2->tv_sec;
    }
	return 0;
}
 

int aos_timer_create_sock()
{
	sg_reader = aos_udp_create(sg_local_addr, 0, 1);
	aos_assert_r(sg_reader, -1);
	aos_assert_r(!sg_reader->mf->connect((aos_conn_t *)sg_reader), -1);
	sg_port = sg_reader->local_used_port;
	sg_writer = aos_udp_create(sg_local_addr, 0, 1);
	aos_assert_r(sg_writer, -1);
	aos_assert_r(!sg_writer->mf->connect((aos_conn_t *)sg_writer), -1);
	return 0;
}


static int aos_timer_check_timer()
{
	//
    // This function checks all OmnTimers in the list. If any OmnTimer
    // expires, its function is called, and it is removed from
    // the list. We assume that the mutex is unlocked when this
    // function is called.
	//
    struct timeval crtTime;
    aos_gettimeofday(&crtTime);
    struct timeval timeLeft;
    struct timeval nextTimer;
    nextTimer.tv_sec = 180;
    nextTimer.tv_usec = 0;
	aos_list_head_t expired_timers;
	AOS_INIT_LIST_HEAD(&expired_timers);
 
    sg_timeval.tv_sec = 180;
    sg_timeval.tv_usec = 0;

    //
    // Collect all expired timers and put them in the list.
    //
	aos_lock(sg_lock);
	aos_trace("Check timer");
	
	aos_timer_t *timer, *tmp;
	aos_list_for_each_entry_safe(timer, tmp, &sg_timers, link)
    {
        if (aos_time_comp(&crtTime, &timer->end_time) >= 0)
        {
			// Timer expired
			aos_list_del(&timer->link);
			aos_list_add(&timer->link, &expired_timers);
			continue;
        }
        else
        {
			// Timer not expired yet. Calculate the next expired
			// timer. 
            aos_time_minus(&timeLeft, &timer->end_time, &crtTime);
            if (aos_time_comp(&nextTimer, &timeLeft) > 0)
            {
                nextTimer.tv_sec = timeLeft.tv_sec;
                nextTimer.tv_usec = timeLeft.tv_usec;
            }
        }
    }
 
	aos_unlock(sg_lock);

    //
    // Call those expired Timer's callback functions
    //
	aos_list_for_each_entry_safe(timer, tmp, &expired_timers, link)
    {
		aos_trace("Timer: %d expired", timer->timer_id);
		aos_assert_r(timer->callback, -1);
        aos_assert_r(!timer->callback(timer->timer_id, timer->user_data), -1);
    }
 
    //
    // Update the next OmnTimer time
    //
    if (aos_time_comp(&sg_timeval, &nextTimer) > 0)
    {
        sg_timeval.tv_sec = nextTimer.tv_sec;
        sg_timeval.tv_usec = nextTimer.tv_usec;
    }
 
	return 0;
}


static void timer_thread_func(aos_thread_t *thread)
{
	//
	// This is a thread function. It does not return until the state
	// becomes not ACTIVE
	//
    struct timeval *timeout = 0; 
    fd_set readfds;

	while (1)
	{
    	readfds = sg_read_fds;

   		if (sg_timeval.tv_sec > 0 || sg_timeval.tv_usec > 0)
   		{
     	    timeout = &sg_timeval;
   		}
   		else
		{
     	    timeout = 0;
		}

		int rslt = aos_socket_select(sg_max_fd, &readfds, 0, 0, timeout);
		aos_assert(rslt >= 0);
		if (rslt == 0)
		{
			// 
			// Timeout
			//
			continue;
		}
		else
		{
    		if (FD_ISSET(sg_reader->sock, &readfds))
    		{
      			char  dat[70];
				int length = 70;
      			aos_assert(aos_read_sock(sg_reader->sock, dat, 
						length) > 0);
			}
		}
		
    	aos_timer_check_timer();
    }

	aos_should_never_come_here;
}


int aos_timer_start()
{
	aos_assert_r(!aos_timer_create_sock(), -1);

    FD_ZERO(&sg_read_fds);
    FD_SET(sg_reader->sock, &sg_read_fds);
    sg_timeval.tv_sec = 0; 
    sg_timeval.tv_usec = 0; 
    sg_max_fd = sg_reader->sock+ 1;
	sg_thread = aos_thread_create("Timer", 0, timer_thread_func, 
			0, 1, 1, 0);
	aos_assert_r(sg_thread, -1);
	return 0;
}


int aos_timer_init()
{
	aos_global_lock();
	if (sg_init_flag)
	{
		aos_global_unlock();
		return 0;
	}

	aos_assert_r(!aos_lock_create(&sg_lock), -1);
	AOS_INIT_LIST_HEAD(&sg_timers);
	aos_assert_r(!aos_addr_to_u32("127.0.0.1", &sg_local_addr), -1);
	sg_init_flag = 1;
	aos_global_unlock();
	return 0;
}



int aos_timer_start_timer(
		const u32 msec, 
		u32 *timer_id,
		aos_timer_callback_t callback, 
		void *user_data)
{
	//
	// If timer starts successfully, it returns the timer ID. 
	// Otherwise, it returns -1. If 'timer_id' is null, it will
	// not return the timer id. If 'user_data' is not null, 
	// when the timer expires, it is passed back to the caller.
	//
	aos_assert_r(msec > 0, -1);
	aos_assert_r(callback, -1);
	int sec = (int)(msec / 1000);
	int usec = (int)(msec % 1000);

	aos_trace("Start timer: %d:%d", sec, usec);

    //
    // Make sure the OmnTimer is at least 10 msecs. 
    //
    if (sec == 0 && usec < sg_timer_min_usec)
    {
		aos_warn("Timer value too small: %d:%d", sec, usec);

		usec = sg_timer_min_usec;
    }

	aos_lock(sg_lock);

    //
    // Create the timer and insert it into the timer list 
    //
	aos_timer_t *new_timer = aos_malloc(sizeof(*new_timer));
	AOS_INIT_LIST_HEAD(&new_timer->link);
    struct timeval stopTime, crtTime;
    aos_gettimeofday(&crtTime);
    stopTime.tv_sec  = sec;
    stopTime.tv_usec = usec;
    new_timer->timer_id = sgTimerId++;
    aos_assert_r(!aos_time_add(&new_timer->end_time, &stopTime, &crtTime), -1);
    new_timer->callback = callback;
	new_timer->user_data = user_data;
	aos_list_add_tail(&new_timer->link, &sg_timers);

    //
    // Check whether the new Timer is sooner than the next OmnTimer.
    //
    if ((sg_timeval.tv_sec == 0 && sg_timeval.tv_usec == 0) ||
		aos_time_comp(&sg_timeval, &stopTime) > 0)
    {
		aos_assert_g(!sg_writer->mf->send_udp(
				sg_writer, "timer", 5, sg_local_addr, sg_port), cleanup);
    }

	aos_trace("Timer: %d started", new_timer->timer_id);
    aos_unlock(sg_lock);
    if (timer_id) *timer_id = new_timer->timer_id;
	return 0;

cleanup:
	aos_unlock(sg_lock);
	return -1;
}


// 
// Description
// It cancels the timer 'timer_id'. If found, it returns 1. 
// If not found, it returns 0. If errors, it returns -1.
//
int aos_timer_cancel_timer(const u32 timer_id)
{
	aos_trace("Cancel Timer: %d", timer_id);

	aos_timer_t *timer, *tmp;
	int found = 0;
	aos_lock(sg_lock);
	aos_list_for_each_entry_safe(timer, tmp, &sg_timers, link)
    {
        if (timer_id == timer->timer_id)
        {
			aos_list_del(&timer->link);
			aos_trace("Timer: %d cancelled", timer_id);
            found = 1;
			break;
        }
    }

	aos_unlock(sg_lock);
    return found;
}



