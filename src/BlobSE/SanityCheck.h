////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Zykie Networks, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2 Jul 2015 created by White
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_BlobSE_SanityCheck_H_
#define Aos_BlobSE_SanityCheck_H_
#include "alarm_c/alarm.h"

#ifdef SANITY_CHECK
extern bool gSanityCheck;
#define sanity_check(cond, ret_code, format, x...)		\
	if (!(cond) && gSanityCheck)						\
	{													\
		aos_alarm("sanity check failed, "format, ##x);	\
		AosAlarmBreakPoint(0);							\
		return (ret_code);								\
	}
#define sanity_check_no_ret(cond, format, x...)			\
	if (!(cond) && gSanityCheck)						\
	{													\
		aos_alarm("sanity check failed, "format, ##x);	\
		AosAlarmBreakPoint(0);							\
		return;											\
	}
#else
#define sanity_check(cond, ret_code, format, x...)
#define sanity_check_no_ret(cond, format, x...)
#endif

#endif /* Aos_BlobSE_SanityCheck_H_ */
