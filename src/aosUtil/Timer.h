////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_aosUtil_Timer_h
#define Aos_aosUtil_Timer_h

//#include <KernelSimu/timer.h>

typedef void (*AosTimerFunc)(unsigned long data);
typedef struct timer_list AosTimer_t;

extern AosTimer_t *AosTimer_addTimer(unsigned int msec, unsigned long data, AosTimerFunc func);
extern int AosTimer_modTimer(AosTimer_t *timer,unsigned int msec);
extern int AosTimer_delTimer(AosTimer_t *timer);

#ifndef __KERNEL__
#include "aosUtil/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"


class AosTimerUtilObj : public OmnTimerObj
{
	OmnDefineRCObject;

private:
	AosTimerFunc 	mFunc;

public:
	AosTimerUtilObj(AosTimerFunc func)
		:
	mFunc(func)
	{
	}

    virtual void        timeout(const int timerId,
						        const OmnString &timerName,
								void *parm);
};
#endif

#endif

