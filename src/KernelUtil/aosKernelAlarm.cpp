////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosKernelAlarm.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelAlarm.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelUtil/KernelDef.h"
#include "KernelUtil/aosJiffies.h"
#include "Porting/LongTypes.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

int	aosDebugFlags[eAosMaxDebugFlag] = {0};

struct aosRatedErr
{
	OmnUint64		mLast;
	unsigned int 	mCount;
};

static struct aosRatedErr sgAosRatedErr[eAosErrR_lastRatedErr] = {{0, 0}};

#ifndef __KERNEL__
extern void printk(char *fmt, ...);
#endif

void aosInitRatedErr()
{
	int i;

	for (i=0; i<eAosErrR_lastRatedErr; i++)
	{
		sgAosRatedErr[i].mLast = 0;
		sgAosRatedErr[i].mCount = 0;
	}
}


int aosAlarm4Imp(const char *file, 
			      int line, 
				  int id,
				  int int1, 
				  int int2, 
				  const char *str1, 
				  const char *str2)
{
	if (str1 && str2)
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %d, %d, %s, %s\n", file, line, 
			id, int1, int2, str1, str2);
		return id;
	}

	if (str1)
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %d, %d, %s\n", file, line, 
			id, int1, int2, str1);
		return id;
	}

	aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %d, %d\n", file, line, 
		id, int1, int2);
	return id;
}


int aosAlarmRatedImp(const char *file, 
					  int line, 
					  int id,
					  unsigned int freq,
					  int int1, 
					  int int2, 
					  const char *str1, 
					  const char *str2)
{
	if (id <= eAosErrR_firstRatedErr || id >= eAosErrR_lastRatedErr)
	{
		id = eAosErrR_invalidRatedErrId;
	}

	sgAosRatedErr[id].mCount++;
	if ((unsigned int)jiffies - sgAosRatedErr[id].mLast >= freq)
	{
		if (str1 && str2)
		{
			aosPrintk("AOS-Rated-Alarm: <%s:%d>: ID=%d, Count=%d, %d, %d, %s, %s\n", 
				file, line, id, sgAosRatedErr[id].mCount, 
				int1, int2, str1, str2);
		} 
		else if (str1)
		{
			aosPrintk("AOS-Rated-Alarm: <%s:%d>: ID=%d, Count=%d, %d, %d, %s\n", 
				file, line, id, sgAosRatedErr[id].mCount, 
				int1, int2, str1);
		}
		else
		{
			aosPrintk("AOS-Rated-Alarm: <%s:%d>: ID=%d, Count=%d, %d, %d\n", 
				file, line, id, sgAosRatedErr[id].mCount, 
				int1, int2);
		}

		sgAosRatedErr[id].mLast = jiffies;
	}

	return id;
}


int aosAlarmIntImp(const char *file, 
					int line, 
					int id,
					int value)
{
	aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %d\n", file, line, id, value);
	return id;
}


int aosAlarmImp(const char *file, 
				int line, 
				int id)
{
	aosPrintk("AOS-Alarm: <%s:%d>: ID=%d\n", file, line, id);
	return id;
}


int aosAlarmInt2Imp(const char *file, 
					int line, 
					int id,
					int value1, 
					int value2)
{
	aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %d, %d\n", file, line, id, value1, value2);
	return id;
}


int aosAlarmStrImp(const char *file, 
					int line, 
					int id,
					const char *value)
{
	if (value)
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %s\n", file, line, id, value);
	}
	else
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d\n", file, line, id);
	}

	return id;
}


int aosAlarmStr2Imp(const char *file, 
					 int line, 
					 int id,
					 const char *value1, 
					 const char *value2)
{
	if (value1 && value2)
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %s, %s\n", file, line, id, value1, value2);
		return id;
	}

	if (value1)
	{
		aosPrintk("AOS-Alarm: <%s:%d>: ID=%d, %s\n", file, line, id, value1);
		return id;
	}

	aosPrintk("AOS-Alarm: <%s:%d>: ID=%d\n", file, line, id);

	return id;
}


int aosWarnIntImp(const char *file, 
				   int line, 
				   int id,
				   int value)
{
	aosPrintk("AOS-Warn: <%s:%d>: ID=%d, %d\n", file, line, id, value);
	return id;
}


int aosWarnInt2Imp(const char *file, 
				    int line, 
				    int id,
				    int value1, 
					int value2)
{
	aosPrintk("AOS-Warn: <%s:%d>: ID=%d, %d, %d\n", file, line, id, value1, value2);
	return id;
}


int aosWarnStrImp(const char *file, 
				   int line, 
				   int id,
				   const char *value)
{
	if (value)
	{
		aosPrintk("AOS-Warn: <%s:%d>: ID=%d, %s\n", file, line, id, value);
	}
	else
	{
		aosPrintk("AOS-Warn: <%s:%d>: ID=%d\n", file, line, id);
	}

	return id;
}


int aosWarnStr2Imp(const char *file, 
				    int line, 
				    int id,
				    const char *value1, 
					const char *value2)
{
	if (value1 && value2)
	{
		aosPrintk("AOS-Warn: <%s:%d>: ID=%d, %s, %s\n", file, line, id, value1, value2);
		return id;
	}

	if (value1)
	{
		aosPrintk("AOS-Warn: <%s:%d>: ID=%d, %s\n", file, line, id, value1);
		return id;
	}

	aosPrintk("AOS-Warn: <%s:%d>: ID=%d\n", file, line, id);

	return id;
}


void aosAssertFailed(const char *file, int line)
{
	aosPrintk("AOS Assert Failed: <%s:%d>\n", file, line);
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosAssertFailed );
EXPORT_SYMBOL( aosAlarmIntImp );
EXPORT_SYMBOL( aosAlarmImp );
#endif
