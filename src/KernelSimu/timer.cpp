////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: timer.cpp
// Description:
//   
// Author: Tim
//
// Modification History:
// 	created: 01/06/2007
// 
////////////////////////////////////////////////////////////////////////////

#include <KernelSimu/timer.h>
#include <KernelSimu/spinlock.h>

// this is system timer list;
struct aos_list_head aos_timer_list_head = AOS_LIST_HEAD_INIT(aos_timer_list_head); 

void aos_timer_check_and_run(void)
{
	struct aos_list_head *cur, *next;
	struct timer_list *timer;
	
	aos_list_for_each_safe(cur, next, &aos_timer_list_head)
	{
		timer = aos_list_entry(cur, struct timer_list, entry);
		if (AOS_GET_JIFFIES() >= timer->expires)
		{
			aos_list_del(cur);
			timer->function(timer->data);
		}
	}
}

int del_timer(struct timer_list *timer)
{
	unsigned long flags;
	struct tvec_t_base_s *base;

	//check_timer(timer);

repeat:
 	base = timer->base;
	if (!base)
		return 0;
	spin_lock_irqsave(&base->lock, flags);
	if (base != timer->base) {
		spin_unlock_irqrestore(&base->lock, flags);
		goto repeat;
	}
	aos_list_del(&timer->entry);
	timer->base = NULL;
	spin_unlock_irqrestore(&base->lock, flags);

	return 1;
}
