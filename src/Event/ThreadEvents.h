////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 11/30/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_ThreadEvents_h
#define Aos_Event_ThreadEvents_h

#include "Event/EventId.h"
#include "Event/Event.h"


#ifdef AOS_SEMANTICS

#ifndef aos_se_thread_started
#define aos_se_thread_started(flag, logic_name, inst_name, inst, actor)\
	if (flag) aos_raise_event(								\
			AosEventId::eThreadStarted, 			\
			__FILE__, __LINE__, 					\
			logic_name,								\
			inst_name,								\
			inst_ptr,								\
			actor)
#endif

#ifndef aos_se_thread_finished
#define aos_se_thread_finished(flag, logic_name, inst_name, inst, actor) \
	if (flag) aos_raise_event(								\
			AosEventId::eThreadFinished, 			\
			__FILE__, __LINE__, 					\
			logic_name,								\
			inst_name,								\
			inst_ptr,								\
			actor)
#endif

#ifndef aos_se_thread_goto_sleep
#define aos_se_thread_goto_sleep(flag, logic_name, inst_name, inst, actor)\
	if (flag) aos_raise_event(						\
			AosEventId::eThreadGotoSleep, 			\
			__FILE__, __LINE__, 					\
			logic_name,								\
			inst_name,								\
			inst_ptr,								\
			actor)
#endif

#ifndef aos_se_thread_waken_up
#define aos_se_thread_waken_up(flag, logic_name, inst_name, inst, actor)\
	if (flag) aos_raise_event(						\
			AosEventId::eThreadWakenUp, 			\
			__FILE__, __LINE__, 					\
			logic_name,								\
			inst_name,								\
			inst_ptr,								\
			actor)
#endif

#ifndef aos_se_to_lock
#define aos_se_to_lock(flag, theLock, actor, classname, instname) 	\
	if (flag)														\
	{																\
		aos_raise_event(AosEventId::eToLock, __FILE__, __LINE__, 	\
			actor, classname, instname, theLock);					\
	}
#endif

#ifndef aos_se_locked
#define aos_se_locked(flag, theLock, actor, classname, instname) 	\
	if (flag)														\
	{																\
		aos_raise_event(AosEventId::eLocked, __FILE__, __LINE__, 	\
			actor, classname, instname, theLock);					\
	}
#endif

#ifndef aos_se_unlocked
#define aos_se_unlocked(flag, theLock, actor, classname, instname) 	\
	if (flag)														\
	{																\
		aos_raise_event(AosEventId::eUnlocked, __FILE__, __LINE__, 	\
			actor, classname, instname, theLock);					\
	}
#endif

#ifndef aos_se_to_unlock
#define aos_se_to_unlock(flag, theLock, actor, classname, instname) \
	if (flag)														\
		aos_raise_event(AosEventId::eToUnlock, __FILE__, __LINE__, 	\
			actor, classname, instname, theLock);
#endif


#else
// 
// Semantics is not turned on
//
#define aos_se_thread_started(flag, logic_name, inst_name, inst, actor)
#define aos_se_thread_finished(flag, logic_name, inst_name, inst, actor) 
#define aos_se_thread_goto_sleep(flag, logic_name, inst_name, inst, actor)
#define aos_se_thread_waken_up(flag, logic_name, inst_name, inst, actor)
#define aos_se_to_lock(flag, theLock, actor, classname, instname) 	
#define aos_se_locked(flag, theLock, actor, classname, instname) 	
#define aos_se_unlocked(flag, theLock, actor, classname, instname) 
#define aos_se_to_unlock(flag, theLock, actor, classname, instname) 

#endif // end of ifdef AOS_SEMANTICS

#endif
