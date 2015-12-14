////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerPack.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/TimerPack.h"

#include "aos/aosReturnCode.h"
#include "aosUtil/Memory.h"
#include "alarm_c/alarm.h"
#include "aosUtil/List.h"
#include "KernelSimu/jiffies.h"

#ifdef __KERNEL__
#include <linux/module.h>
#endif

extern AosTimer_t * AosTimer_addTimer(unsigned int msec, unsigned long data, AosTimerFunc func);

AosTimerPack_t * AosTimerPack_init(AosTimerPack_t * pack, u32 freq_msec, AosTimerPackFunc func)
{
	int	i;
	memset(pack, 0, sizeof(AosTimerPack_t));
	pack->freq_msec = freq_msec;
	pack->func = func;
	//AOS_INIT_MUTEX(pack->lock);
	// 
	// Initialize vecs
	//
	for (i=0; i<=TVR_MASK; i++)
	{
		AOS_INIT_LIST_HEAD(&pack->tv1.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv2.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv3.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv4.vec[i]);
	}

	return pack;
}

AosTimerPack_t * AosTimerPack_create(u32 freq_msec, AosTimerPackFunc func)
{
	int	i;
	AosTimerPack_t * pack;
	if((pack = (AosTimerPack_t *)aos_malloc(sizeof(AosTimerPack_t)))==NULL)
	{
		//printk("malloc error");
		return NULL;
	}
	memset(pack, 0, sizeof(AosTimerPack_t));
	pack->freq_msec = freq_msec;
	pack->func = func;
	//AOS_INIT_MUTEX(pack->lock);
	// 
	// Initialize vecs
	//
	for (i=0; i<=TVR_MASK; i++)
	{
		AOS_INIT_LIST_HEAD(&pack->tv1.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv2.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv3.vec[i]);
		AOS_INIT_LIST_HEAD(&pack->tv4.vec[i]);
	}

	return pack;
}


int AosTimerPack_addInternal(AosTimerPack_t *pack, AosPackTimer_t *apptimer)
{
	// 
	// Calculate our tick
	//
	u32 expires = apptimer->expires;
	u32 idx = expires - pack->pack_tick;
	struct aos_list_head *vec;

	if (idx < 256) {
		int i = expires & 255;
		vec = pack->tv1.vec + i;
	} else if (idx < 1 << (8 + 8)) {
		int i = (expires >> 8) & 255;
		vec = pack->tv2.vec + i;
	} else if (idx < 1 << (8 + 2 * 8)) {
		int i = (expires >> (8 + 8)) & 255;
		vec = pack->tv3.vec + i;	
	} else {
		//
		// ???
		//
		int i = (expires >> (8 + 2 * 8)) & 255;
		vec = pack->tv4.vec + i;
	}

	// 
	// Add the timer to the end of the corresponding slot
	//
	//aos_lock(pack->lock);	
	aos_list_add_tail((struct aos_list_head *)&apptimer->entry, vec);
	//aos_unlock(pack->lock);
	/*
	AosPackTimer_t * ptr;
	int i=1;
	aos_list_for_each_entry(ptr, vec, entry)
	{
	    //printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
	    //printf("the add timer i=%d   entry+++++++:%x    expires:%d\n", i, &ptr->entry, ptr->expires);
		i++;
    }
	*/
	return 0;
}


static int cascade(AosTimerPack_t * pack, tvec_t *tv, int index)
{
	/* cascade all the timers from tv up one level */
	struct aos_list_head *head, *curr;

	head = tv->vec + index;
	curr = head->next;
	while (curr != head) {
		AosPackTimer_t *tmp;

		// tmp = aos_list_entry(curr, AosPackTimer_t, entry);
		tmp = (AosPackTimer_t *)curr;
		curr = curr->next;
		AosTimerPack_addInternal(pack, tmp);
	}

	AOS_INIT_LIST_HEAD(head);
	/*
	printf("cascade:index=%d-------",index);
	if(tv==&pack->tv2)
	{
		printf("cascade:tv2\n");
	}
	else if(tv==&pack->tv3)
	{
		printf("cascade:tv3\n");
	}
	else if(tv==&pack->tv4)
	{
		printf("cascade:tv4\n");
	}
	*/
	return index;
}


#define INDEX(N) (pack->pack_tick >> (8 + N * 8)) & 255


void AosTimerPack_run(unsigned long packu32)
{
	//AosTimer_t * packtimer; 
	AosTimerPack_t * pack = (AosTimerPack_t *)packu32;

	//
	// Check whether the pack is stopped
	//
	if(pack->stopFlag)
	{
		return ;
	}
	//	
	// Start the next timer
	//
	//packtimer = AosTimer_addTimer(pack->freq_msec, packu32, AosTimerPack_run);
	AosTimer_addTimer(pack->freq_msec, packu32, AosTimerPack_run);
	
	//aos_assert1(pack->func);
	
	struct aos_list_head work_list;
	AOS_INIT_LIST_HEAD(&work_list);

	//printf("To lock in run");
	//aos_lock(pack->lock);

	// 
	// Calculate the index
	//
	int index = pack->pack_tick & TVR_MASK;
	
	// 
	// Expand the slots, if needed
	//
	
	if (!index &&
		(!cascade(pack, &pack->tv2, INDEX(0))) &&
			!cascade(pack, &pack->tv3, INDEX(1))) 
	{
		cascade(pack, &pack->tv4, INDEX(2));
	}

	// 
	// Increment our tick
	//
	++pack->pack_tick; 
	//pack->pack_tick = pack->pack_tick + 10000; 


	// 
	// Move the list to 'work_list' and empty pack->tv1.vec + index
	//
	aos_list_splice_init(pack->tv1.vec + index, &work_list);
	
	
	//aos_unlock(pack->lock); 
	//printf("unlocked in run");

	// 
	// Call the callback
	//
	if (!aos_list_empty(&work_list))
	{
		pack->func(&work_list);
		
		/*
		AosPackTimer_t * ptr;
		AosPackTimer_t * tmpptr;
		aos_list_for_each_entry_safe(ptr, tmpptr, &work_list, entry)
		{
			aos_list_del(&ptr->entry);
			aos_free(ptr);
			//printf("the cur index:%d, pack_tick: %d\n",index, pack->pack_tick);
		}
		*/

	}
	return ;
}


int AosTimerPack_start(AosTimerPack_t * pack)
{
	pack->stopFlag = 0;

	//printf("this is the start jiffes:%llu\n",jiffies);
	pack->pack_tick = 0;
	//packtimer = AosTimer_addTimer(pack->freq_msec, 0, AosTimerPack_run);
	AosTimer_addTimer(pack->freq_msec, (unsigned long)pack, AosTimerPack_run);
	return 0;
}


int AosTimerPack_stop(AosTimerPack_t * pack)
{
	pack->stopFlag = 1;
	return 0;
}

AosPackTimer_t * AosPackTimer_init(AosTimerPack_t * pack, AosPackTimer_t * apptimer, u32 app_msec, u32 data)
{
	apptimer->expires = pack->pack_tick + app_msec / pack->freq_msec;
	apptimer->data = data;	
	return apptimer;
}


AosPackTimer_t *
AosTimerPack_add(AosTimerPack_t * pack, u32 app_msec, u32 data)
{
	AosPackTimer_t * apptimer;
	if((apptimer = (AosPackTimer_t *)aos_malloc(sizeof(AosPackTimer_t)))==NULL)
	{
		aos_alarm("Failed to allocate memory \n");
		return NULL;
	}

	memset(apptimer, 0, sizeof(AosPackTimer_t));
	apptimer->expires = pack->pack_tick + app_msec / pack->freq_msec;
	apptimer->data = data;	

	//aos_lock(pack->lock);	
	int ret = AosTimerPack_addInternal(pack, apptimer);
	//aos_unlock(pack->lock);	
	if (ret)
	{
		aos_free(apptimer);
		return NULL;
	}

	return apptimer;
}


int AosTimerPack_del(AosTimerPack_t * pack, AosPackTimer_t * timer)
{
	//printf("To lock in del");
	//aos_lock(pack->lock);	
	aos_list_del(&timer->entry);
	//aos_unlock(pack->lock);
	//printf("unlock in del");
	
	return 0;
}


int AosTimerPack_free(AosTimerPack_t * pack)
{
	aos_free(pack); 
	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( AosTimerPack_addInternal );
EXPORT_SYMBOL( AosTimerPack_init );
EXPORT_SYMBOL( AosPackTimer_init );
EXPORT_SYMBOL( AosTimerPack_start );
EXPORT_SYMBOL( AosTimerPack_stop );
#endif

