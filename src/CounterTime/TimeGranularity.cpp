////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 06/07/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterTime/TimeGranularity.h"

#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


extern AosTimeGranularityPtr sgTime[AosTimeGranularity::eMaxFormat];
static OmnMutex			  sgLock;


AosTimeGranularity::AosTimeGranularity(
		const AosTimeGranularity::TimeFormat format, 
		const bool regflag)
:
mTimeFormat(format)
{
	if (regflag)
	{
		AosTimeGranularityPtr thisPtr(this, false);
		registerProc(thisPtr);
	}
}


bool
AosTimeGranularity::registerProc(const AosTimeGranularityPtr &timePtr)
{
	sgLock.lock();
	if (timePtr->mTimeFormat <= eInvalid || timePtr->mTimeFormat >= eMaxFormat)
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect TimeFormat : " << timePtr->mTimeFormat << enderr;
		return false;
	}

	if (sgTime[timePtr->mTimeFormat])
	{
		OmnAlarm << "This timeFormat already registered: " << timePtr->mTimeFormat << enderr;
        sgLock.unlock();
        return false;

	}
	sgTime[timePtr->mTimeFormat] = timePtr;
	sgLock.unlock();
	return true;
}


AosTimeGranularityPtr
AosTimeGranularity::getProc(const TimeFormat formatType)
{
	if (formatType <= eInvalid) return 0;
	if (formatType >= eMaxFormat) return 0;
	return sgTime[formatType];
}


AosTimeGranularityPtr
AosTimeGranularity::getProc(const OmnString &type)
{
	TimeFormat formatType = toEnum(type);
	if (formatType <= eInvalid) return 0;
	if (formatType >= eMaxFormat) return 0;
	return sgTime[formatType];
}


bool
AosTimeGranularity::setEmptyContents(const AosRundataPtr &rdata)
{
	OmnString contents = "<contents total='0'></contents>";
    rdata->setContents(contents);
    rdata->setOk();
	return true;
}
#endif
