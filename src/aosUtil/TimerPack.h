////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimerPack.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AOS_TIMER_PACK_H
#define AOS_TIMER_PACK_H

#include "aosUtil/List.h"
#include "aosUtil/Mutex.h"
#include "KernelSimu/types.h"

#include "aosUtil/Timer.h"

//# define HZ		1000		/* Internal kernel timer frequency */
# define TVR_MASK		255

typedef struct tvec_s {
	struct aos_list_head vec[256];
} tvec_t;
	
typedef void (* AosTimerPackFunc)(struct aos_list_head * data_list);
typedef struct AosPackTimer AosPackTimer_t;
typedef struct AosTimerPack AosTimerPack_t;



struct AosTimerData
{
	struct aos_list_head list_data;
	void * data;
};
	
struct AosPackTimer
{
	struct aos_list_head entry;
	u32 				 expires;
	u32 				 data;
};
						
struct AosTimerPack
{
//	OmnMutexType lock;
	u32 pack_tick;
	//struct AosTimer *running_timer;
	tvec_t tv1;
	tvec_t tv2;
	tvec_t tv3;
	tvec_t tv4;
	u32 freq_msec;
	AosTimerPackFunc func;
	int stopFlag;               //1: stopped
};


AosTimerPack_t * AosTimerPack_init(AosTimerPack_t * pack, u32 freq_msec, AosTimerPackFunc func);
AosTimerPack_t * AosTimerPack_create(u32 freq_msec, AosTimerPackFunc func);
AosPackTimer_t * AosTimerPack_add(AosTimerPack_t * pack, u32 app_msec, u32 data);
AosPackTimer_t * AosPackTimer_init(AosTimerPack_t * pack, AosPackTimer_t * apptimer, u32 app_msec, u32 data);

int AosTimerPack_start(AosTimerPack_t * pack);
void AosTimerPack_run(unsigned long packu32);
int AosTimerPack_free(AosTimerPack_t * pack);
int AosTimerPack_del(AosTimerPack_t * pack, AosPackTimer_t * timer);
int AosTimerPack_stop(AosTimerPack_t * pack);
int AosTimerPack_addInternal(AosTimerPack_t *pack, AosPackTimer_t *apptimer);



#endif


