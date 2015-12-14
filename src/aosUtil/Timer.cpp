////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include "aosUtil/Timer.h"
#include "aosUtil/Memory.h"

#include <KernelSimu/kernel.h>
#include <KernelSimu/timer.h>
#include <KernelSimu/jiffies.h>
#include <KernelSimu/aosKernelDebug.h>

#include "aos/KernelEnum.h"

AosTimer_t *AosTimer_addTimer(unsigned int msec, unsigned long data, AosTimerFunc func)
{
	AosTimer_t *thetimer = (AosTimer_t *)aos_malloc_atomic(sizeof(AosTimer_t));
	init_timer(thetimer);
	thetimer->expires=jiffies+msec;
	thetimer->data=data;
	thetimer->function=func;
	mod_timer(thetimer, thetimer->expires);
		
	return thetimer;

}

int AosTimer_modTimer(AosTimer_t *timer,unsigned int msec)
{
	timer->expires=jiffies+msec;
	mod_timer(timer, timer->expires);
	//printk("<0>mod timer ok\n");
	return 0;
}


int AosTimer_delTimer(AosTimer_t *timer)
{

	unsigned long flags;
	tvec_base_t *base;

	if (timer->magic != TIMER_MAGIC)
	{
		static int whine_count;
		if (whine_count < 16) {
			whine_count++;
			printk("Uninitialised timer!\n");
			printk("This is just a warning.  Your computer is OK\n");
			printk("function=0x%p, data=0x%lx\n",
				timer->function, timer->data);
			dump_stack();
		}
		/*
		 * Now fix it up
		 */
		spin_lock_init(&timer->lock);
		timer->magic = TIMER_MAGIC;
	}
	
repeat:
 	base = timer->base;
	if (!base)
		return 0;
	spin_lock_irqsave(&base->lock, flags);
	if (base != timer->base) {
		spin_unlock_irqrestore(&base->lock, flags);
		goto repeat;
	}
	list_del(&timer->entry);
	timer->base = NULL;
	spin_unlock_irqrestore(&base->lock, flags);

	/*
	if(aos_free(timer)) 
	{
		printk("<0>del timer ok\n");
		return 0;
	}*/

	aos_free(timer);

	return 0;
}




/*
int inline mod_timer(struct timer_list *timer, unsigned long expires)
{
	BUG_ON(!timer->function);

	check_timer(timer);

	if (timer->expires == expires && timer_pending(timer))
			return 1;

	return __mod_timer(timer, expires);
}
*/
#else
#include "aosUtil/Timer.h"
#include "aosUtil/Ptrs.h"
#include "Util/OmnNew.h"
#include "Util1/Timer.h"

AosTimer_t *
AosTimer_addTimer(unsigned int msec, unsigned long data, AosTimerFunc func)
{
	AosTimerUtilObj * caller = OmnNew AosTimerUtilObj(func);	
	OmnTimer::getSelf()->startTimer("NoName", msec / 1000, 
				msec % 1000 * 1000, caller, (void *)data);
	return NULL;
}


void 
AosTimerUtilObj::timeout(const int timerId,
						 const OmnString &timerName,
						 void *parm)
{
	if (mFunc)
	{
		mFunc((unsigned long)parm);
	}
}  


int AosTimer_modTimer(AosTimer_t *timer,unsigned int msec)
{
	//timer->expires=jiffies+msec;
	//mod_timer(timer, timer->expires);
	//printk("<0>mod timer ok\n");
	return 0;
}


int AosTimer_delTimer(AosTimer_t *timer)
{
	//timer->expires=jiffies+msec;
	//mod_timer(timer, timer->expires);
	//printk("<0>mod timer ok\n");
	return 0;
}
#endif

