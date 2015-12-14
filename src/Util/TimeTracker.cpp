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
// 2013/10/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/TimeTracker.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util/Buff.h"


OmnString 
AosTimeTracker::convertToAscii(
		const int fname_len, 
		const Unit unit, 
		const bool scientific_form) const
{
	// It converts to the following format:
	// 	[fname:line:name:time, 
	// 	 fname:line:name:time, 
	// 	 ...
	// 	 fname:line:name:time] 
	// File names and lines are optional. Time can be in microseconds, 
	// milli-seconds, or seconds. The numbers can be converted in 
	// scientific. If it is converted to scientific, the number will
	// be double quoted since it may contain commas. 
	OmnString str;
	for (u32 i=1; i<mEntries.size(); i++)
	{
		if (i > 1) str << ",";
		mEntries[i].toAscii(str, fname_len, unit, scientific_form); 
	}
	return str;
}


void
AosTimeTracker::pushTimestamp()
{
	mTimestamps.push(mCrtTime);
	mTimestamps.push(mCrtStart);
	mCrtTime = 0;
	mCrtStart = 0;
}


bool
AosTimeTracker::popTimestamp()
{
	aos_assert_r(mTimestamps.size() > 1, false);
	mCrtStart = mTimestamps.top();
	mTimestamps.pop();
	mCrtTime = mTimestamps.top();
	mTimestamps.pop();
	return true;
}


void
AosTimeTracker::append(
		const char *fname, 
		const int line, 
		const OmnString &name)
{
	if (mCrtTime == 0)
	{
		mCrtTime = OmnGetTimestamp();
		mCrtStart = mCrtTime;
		return;
	}

	u64 tt = OmnGetTimestamp();
	aos_assert(tt >= mCrtTime);
	u64 delta = tt - mCrtTime;
	mEntries.push_back(Entry(fname, line, name, delta));
	mCrtTime = tt;
}


bool
AosTimeTracker::append(const AosTimeTracker &times)
{
	const vector<Entry> &entries = times.mEntries;
	for (u32 i=0; i<entries.size(); i++)
	{
		mEntries.push_back(entries[i]);
	}
	return true;
}


void
AosTimeTracker::append(
		const char *fname, 
		const int line, 
		const OmnString &name, 
		const u64 time)
{
	mEntries.push_back(Entry(fname, line, name, time));
}


bool
AosTimeTracker::serializeToBuff(const AosBuffPtr &buff)
{
	buff->setU32(mEntries.size());
	for (u32 i=0; i<mEntries.size(); i++)
	{
		mEntries[i].serializeToBuff(buff);
	}
	return true;
}


bool
AosTimeTracker::serializeFromBuff(const AosBuffPtr &buff)
{
	mEntries.clear();
	u32 nn = buff->getU32(0);
	if (nn <= 0) return true;

	for (u32 i=0; i<nn; i++)
	{
		Entry entry;
		entry.serializeFromBuff(buff);
		mEntries.push_back(entry);
	}
	return true;
}


bool
AosTimeTracker::Entry::serializeToBuff(const AosBuffPtr &buff)
{
	buff->setOmnStr(fname);
	buff->setInt(line);
	buff->setOmnStr(name);
	buff->setU64(time);
	return true;
}


bool
AosTimeTracker::Entry::serializeFromBuff(const AosBuffPtr &buff)
{
	fname = buff->getOmnStr("");
	line = buff->getInt(0);
	name = buff->getOmnStr("");
	time = buff->getU64(0);
	return true;
}


bool
AosTimeTracker::Entry::toAscii(
		OmnString &str,
		const int fname_len, 
		const Unit unit,
		const bool scientific_form) const
{
	str << "[";
	if (fname_len > 0)
	{
		if (fname.length() < fname_len)
		{
			str << fname << ":" << line << ":";
		}
		else
		{
			const char *data = fname.data();
			str << &data[fname.length()-fname_len] << ":" << line << ":";
		}
	}

	str << name;
	u64 tt = time;
	switch (unit)
	{
	case eMicrosec:
		 break;

	case eMillisec:
		 tt = tt/1000;
		 break;

	case eSecond:
		 tt /= 1000000;
		 break;

	default:
		 OmnAlarm << "Unrecognized unit: " << unit << enderr;
		 return false;
	}

	if (scientific_form)
	{
		OmnString ss;
		ss << tt;
		ss.convertToScientific();
		str << ":\"" << ss << "\"]";
		return true;
	}

	str << ":" << tt << "]";
	return true;
}


bool
AosTimeTracker::setTotalTime(
		const char *fname, 
		const int line, 
		const OmnString &name)
{
	if (mCrtTime == 0 || mCrtStart == 0) return false;
	aos_assert_r(mCrtTime >= mCrtStart, false);

	u64 tt = OmnGetTimestamp();
	aos_assert_r(tt >= mCrtStart, false);
	u64 delta = tt - mCrtStart;
	mEntries.push_back(Entry(fname, line, name, delta));
	return true;
}
