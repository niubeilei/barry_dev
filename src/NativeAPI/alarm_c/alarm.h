////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Alarm.h
// Description:
//   
//
// Modification History:
// 12/02/2007	Moved from aosUtil/Alarm.h by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_alarm_Alarm_h
#define Omn_alarm_Alarm_h

#include "alarm_c/alarm_ids.h"
#include "alarm_c/alarm_util.h"
#include "util_c/modules.h"




#ifndef aos_fatal
#define aos_fatal(format, x...) 								\
			aos_raise_alarm(__FILE__, __LINE__, 				\
			eAosAlarmLevel_Fatal, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x))
#endif

#ifndef aos_alarm
#define aos_alarm(format, x...) 								\
 	aos_raise_alarm(__FILE__, __LINE__, 						\
 			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x))
#endif

#ifndef aos_warn
#define aos_warn(format, x...) 									\
			aos_raise_alarm(__FILE__, __LINE__, 				\
			eAosAlarmLevel_Warn, eAosMD_Platform, 				\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x))
#endif 


#ifndef aos_not_implemented_yet
#define aos_not_implemented_yet aos_raise_alarm(				\
			__FILE__, __LINE__, 								\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_NotImplemented, 							\
			"Not implemented yet")
#endif

#ifndef aos_should_never_come_here
#define aos_should_never_come_here aos_raise_alarm(				\
			__FILE__, __LINE__, 								\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_ProgErr, 									\
			"Should never come here")
#endif

#ifndef aos_safety_guard_failed
#define aos_safety_guard_failed aos_raise_alarm(				\
			__FILE__, __LINE__, 								\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			"Safety Guard failed")
#endif

#ifndef aos_assert
#define aos_assert(cond)										\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		AosAlarmBreakPoint(0);									\
		return;													\
	}
#endif

#ifndef aos_assert_r
#define aos_assert_r(cond, returncode)							\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		AosAlarmBreakPoint(0);									\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_rl
#define aos_assert_rl(cond, lock, returncode)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		(lock)->unlock();											\
		AosAlarmBreakPoint(0);									\
		return (returncode);									\
	}
#endif


//Ice 2013/5/21
#ifndef aos_sleep_assert
#define aos_sleep_assert(cond)										\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		AosAlarmBreakPoint(1);								\
		return;													\
	}
#endif

#ifndef aos_sleep_assert_r
#define aos_sleep_assert_r(cond, returncode)							\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		AosAlarmBreakPoint(1);								\
		return (returncode);									\
	}
#endif

#ifndef aos_sleep_assert_rl
#define aos_sleep_assert_rl(cond, lock, returncode)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		(lock)->unlock();										\
		AosAlarmBreakPoint(1);								\
		return (returncode);									\
	}
#endif


// Brian Zhang 08/31/2012
#ifndef aos_assert_rf
#define aos_assert_rf(cond, file, returncode)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		(file)->closeFile();											\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_user_r1
#define aos_assert_user_r1(cond, rdata, errmsg_id, errmsg)						\
	if (!(cond))																\
	{																			\
		if (rdata)																\
		{																		\
			rdata->pushError((errmsg_id), "", (errmsg), true, __FILE__, __LINE__);	\
		}																		\
		aos_raise_alarm(__FILE__, __LINE__, 									\
			eAosAlarmLevel_Alarm, 												\
			eAosAlarm_Platform, eAosAlarm_General, "");							\
		AosAlarmBreakPoint(0);													\
		return;																	\
	}
#endif

#ifndef aos_assert_user_r2
#define aos_assert_user_r2(cond, rdata, errmsg_id, entity_id, errmsg)			\
	if (!(cond))																\
	{																			\
		if (rdata)																\
		{																		\
			rdata->pushError((errmsg_id), (entity_id), (errmsg), true, __FILE__, __LINE__);	\
		}																		\
		aos_raise_alarm(__FILE__, __LINE__, 									\
			eAosAlarmLevel_Alarm, 												\
			eAosAlarm_Platform, eAosAlarm_General, "");							\
		AosAlarmBreakPoint(0);													\
		return;																	\
	}
#endif

#ifndef aos_assert_rr
#define aos_assert_rr(cond, rdata, returncode)					\
	if (!(cond))												\
	{															\
		rdata->setError("assert_error", __FILE__, __LINE__);	\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		AosAlarmBreakPoint(0);									\
		return (returncode);									\
	}
#endif

// Created By Andy Zhang
#ifndef aos_assert_use_err
#define aos_assert_use_err(cond, rdata, errmsg_id, errmsg, returncode) 			\
	if (!(cond))																\
	{																			\
		if (rdata)																\
		{																		\
			AosSetErrorUser(rdata, errmsg_id) << errmsg << enderr;				\
		}																		\
		aos_raise_alarm(__FILE__, __LINE__,               						\
			eAosAlarmLevel_Alarm,             									\
			eAosAlarm_Platform, eAosAlarm_General, "");    						\
			AosAlarmBreakPoint(0);                             					\
			return (returncode);                               					\
	}																				
#endif

#ifndef aos_assert_user_rr1
#define aos_assert_user_rr1(cond, rdata, errmsg_id, errmsg, returncode)					\
	if (!(cond))																\
	{																			\
		if (rdata)																\
		{																		\
			rdata->pushError((errmsg_id), "", (errmsg), true, __FILE__, __LINE__);	\
		}																		\
		aos_raise_alarm(__FILE__, __LINE__, 									\
			eAosAlarmLevel_Alarm, 												\
			eAosAlarm_Platform, eAosAlarm_General, "");							\
		AosAlarmBreakPoint(0);													\
		return (returncode);													\
	}
#endif

#ifndef aos_assert_user_rr2
#define aos_assert_user_rr2(cond, rdata, errmsg_id, entity_id, errmsg, returncode)		\
	if (!(cond))																\
	{																			\
		if (rdata)																\
		{																		\
			rdata->pushError((errmsg_id), (entity_id), (errmsg), true, __FILE__, __LINE__);	\
		}																		\
		aos_raise_alarm(__FILE__, __LINE__, 									\
			eAosAlarmLevel_Alarm, 												\
			eAosAlarm_Platform, eAosAlarm_General, "");							\
		AosAlarmBreakPoint(0);													\
		return (returncode);													\
	}
#endif

#ifndef aos_assert_rk
#define aos_assert_rk(cond, lock, returncode)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		(lock)->mMonitor->unlock(__FILE__, __LINE__);			\
		(lock)->mMonitor->finish(__FILE__, __LINE__);			\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_rb
#define aos_assert_rb(cond, lock, returncode)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		(lock)->unlock(__FILE__, __LINE__);						\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_l
#define aos_assert_l(cond, lock)								\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		lock->unlock();											\
		AosAlarmBreakPoint(0);									\
		return;													\
	}
#endif

#ifndef aos_assert_m
#define aos_assert_m(cond, returncode, format, x...)			\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x));					\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_re
#define aos_assert_re(cond, returncode, errmsg, em)				\
	if (!(cond))												\
	{															\
		errmsg = em;											\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, "");								\
		return (returncode);									\
	}
#endif

#ifndef aos_assert_gm
#define aos_assert_gm(cond, label, format, x...)				\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x));					\
		goto label;												\
	}
#endif

#ifndef aos_assert_rm
#define aos_assert_rm(cond, rc, format, x...)					\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x));					\
		return (rc);											\
	}
#endif

#ifndef aos_assert_g
#define aos_assert_g(cond, label)								\
	if (!(cond))												\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_General, "");								\
		goto label;												\
	}
#endif

#ifndef aos_assert_t
#define aos_assert_t(if_part, then_part, rc)					\
	if ((if_part) && !(then_part))								\
	{															\
		aos_raise_alarm(__FILE__, __LINE__,						\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, "");			\
		return (rc);											\
	}
#endif

#ifndef aos_assert_tm
#define aos_assert_tm(if_part, then_part, rc, format, x...)		\
	if ((if_part) && !(then_part))								\
	{															\
		aos_raise_alarm(__FILE__, __LINE__,						\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, 								\
			eAosAlarm_General, 									\
			aos_alarm_get_errmsg(format, ##x));					\
		return (rc);											\
	}
#endif

#ifndef aos_assert_int_range_r
#define aos_assert_int_range_r(value, min, max, rc)				\
	if ((value) < (min) || (value) > (max))						\
	{															\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosAlarm_Platform, eAosAlarm_General, 				\
			aos_alarm_get_errmsg("Value out of range: %d: [%d:%d]", \
				(value), (min), (max)));						\
		return (rc);											\
	}
#endif

#define aos_very_rare(format, x...)								\
		aos_raise_alarm(__FILE__, __LINE__, 					\
			eAosAlarmLevel_Alarm, 								\
			eAosMD_Platform, 									\
			eAosAlarm_VeryRare, 								\
			aos_alarm_get_errmsg(format, ##x));					\

#define aos_assert_raw(cond)									\
	if (!cond)													\
	{															\
		printf("************** Assert failed: %s:%d\n", 		\
				__FILE__, __LINE__);							\
	}

#endif // end of the header file



