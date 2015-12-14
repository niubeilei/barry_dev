////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aos_poll_timer.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "linux_main.h"
#include "cavium_list.h"
#include "cavium.h"
#include "pending_list.h"
#include "command_que.h"
#include "aos_request.h"
#ifdef NPLUS
#include "soft_req_queue.h"
#endif

#define POLL_TIMER_INTERVAL 2// 10 millisecond

static Uint8 timer_created = 0;

static cavium_semaphore timer_sema;
volatile static int timer_kill;

wait_queue_head_t cavium_poll = { CAVIUM_SPIN_LOCK_UNLOCKED, {NULL, NULL}};
extern int dev_count;
extern cavium_device cavium_dev[];
extern cavium_spinlock_t ordered_list_lock;
extern cavium_spinlock_t unordered_list_lock;

static void aos_poll_timer_func( unsigned long arg );
static void aos_poll_tasklet_func( unsigned long arg );

static struct timer_list gtimer_poll;

DECLARE_TASKLET( gtasklet_poll, aos_poll_tasklet_func, 0 );

//
// Init poll thread
// 
int aos_init_poll_timer(void)
{
	printk("aos init cavium poll timer\n");
	cavium_sema_init( &timer_sema, 0 );
	timer_kill = 0;
	init_waitqueue_head( &cavium_poll );

	gtimer_poll.data = 0;
	gtimer_poll.function = aos_poll_timer_func;
 	gtimer_poll.expires = jiffies + POLL_TIMER_INTERVAL;	// 10 millisecond

	add_timer( &gtimer_poll );

	timer_created = 1;
   
	return 0;
}

//
// kill poll timer
//
void aos_kill_poll_timer(void)
{
   if ( timer_created ) 
   {
      timer_kill = 1;
      cavium_sema_down_interruptible( &timer_sema );
   }
   timer_kill = 0;
   timer_created = 0;
}

//
// Poll timer. Calls check_for_completion_callback
//
static void aos_poll_tasklet_func( unsigned long arg )
{
	int i,queue;
#ifdef NPLUS
	int ucode;
#endif

	for (i=0; i <dev_count; i++)
	{
		// Doorbell coalescing
#ifdef NPLUS
		for (ucode=0; ucode < MICROCODE_MAX;ucode++)
		{
			if((cavium_dev[i].microcode[ucode].code_type == CODE_TYPE_SPECIAL)
				&& likely((cavium_dev[i].microcode[ucode].core_id != (Uint8)(-1)))) 
			{
				move_srq_entries(&cavium_dev[i], ucode, 0);
			}
		}
#endif
		check_for_completion_callback(&cavium_dev[i]);
		
		for(queue=0;queue<MAX_N1_QUEUES;queue++)
		{
			lock_command_queue(&cavium_dev[i], queue);
			if(cavium_dev[i].door_bell_count[queue])
            {
	       		cavium_dbgprint("poll:hitting doorbell %ld\n", cavium_dev[i].door_bell_count[queue]);
               	ring_door_bell(&cavium_dev[i], queue, cavium_dev[i].door_bell_count[queue]);
               	cavium_dev[i].door_bell_count[queue]=0;
            }
            unlock_command_queue(&cavium_dev[i], queue);
         }
	 
	}// for all devices

	// handle pending request
	aos_handle_request();
	
}

static int aos_check_for_tasklet( void )
{
	int i,queue;
	
	// check for ordered queue
	cavium_spin_lock_softirqsave(&ordered_list_lock);
	if ( get_queue_head_ordered() != NULL )
	{
		cavium_spin_unlock_softirqrestore(&ordered_list_lock);
		return 1;   
	}
	cavium_spin_unlock_softirqrestore(&ordered_list_lock);
		
	// check for unordered queue
	cavium_spin_lock_softirqsave(&unordered_list_lock);
	if ( get_queue_head_unordered() != NULL )
	{
		cavium_spin_unlock_softirqrestore(&unordered_list_lock);
		return 1;   
	}
	cavium_spin_unlock_softirqrestore(&unordered_list_lock);
		
	for ( i = 0; i < dev_count; i++ )
	{
		for( queue = 0; queue < MAX_N1_QUEUES; queue++ )
		{
			lock_command_queue( &cavium_dev[i], queue );
			if( cavium_dev[i].door_bell_count[queue] )
            {
            	unlock_command_queue( &cavium_dev[i], queue );
				return 1;
            }
            unlock_command_queue( &cavium_dev[i], queue );
         }
	}
	
	return 0;
}
//
// Poll timer. Calls check_for_completion_callback
//
void aos_poll_timer_func( unsigned long arg )
{
	if ( timer_kill )
	{
		cavium_sema_up( &timer_sema );
		return;
	}
	else
	{
		gtimer_poll.expires += POLL_TIMER_INTERVAL; // 10  millisecond
		add_timer( &gtimer_poll );
	}

	if ( aos_check_for_tasklet() || !is_aos_req_queue_empty() )
	{
		tasklet_schedule( &gtasklet_poll );
	}
}


