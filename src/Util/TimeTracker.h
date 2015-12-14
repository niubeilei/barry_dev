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
#ifndef Aos_Util_TimeTracker_h
#define Aos_Util_TimeTracker_h

#include "Porting/TimeOfDay.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include <vector>
#include <stack>
using namespace std;

class AosTimeTracker
{
public:
	enum Unit
	{
		eInvalid, 

		eMicrosec,
		eMillisec,
		eSecond
	};

private:
	struct Entry
	{
		OmnString 	fname;
		int			line;
		OmnString	name;
		u32			time;

		Entry()
		:
		line(0),
		time(0)
		{
		}

		Entry(const char *fn, 
				const int ln, 
				const OmnString &nm, 
				const u32 tt)
		:
		fname(fn),
		line(ln),
		name(nm),
		time(tt)
		{
		}

		bool toAscii( OmnString &str,
					const int fname_len, 
					const Unit unit,
					const bool scientific_form) const;

		bool serializeToBuff(const AosBuffPtr &buff);
		bool serializeFromBuff(const AosBuffPtr &buff);
	};

	vector<Entry>	mEntries;
	stack<u64>		mTimestamps;
	u64				mCrtTime;
	u64				mCrtStart;

public:
	AosTimeTracker()
	:
	mCrtTime(0),
	mCrtStart(0)
	{
	}

	void reset() {clear();}
	void clear() 
	{
		mEntries.clear(); 
		while (mTimestamps.size() > 0) mTimestamps.pop();
		mCrtTime = 0;
		mCrtStart = 0;
	}
	bool append(const AosTimeTracker &times);
	void append(const char *fname, const int line, const OmnString &name);
	void append(const char *fname, 
				const int line, 
				const OmnString &name, 
				const u64 time);
	OmnString convertToAscii(
					const int fname_len,
					const Unit unit, 
					const bool scientific_form) const;
	bool serializeToBuff(const AosBuffPtr &buff);
	bool serializeFromBuff(const AosBuffPtr &buff);
	void pushTimestamp();
	bool popTimestamp();
	bool setTotalTime(
				const char *fname, 
				const int line, 
				const OmnString &name);
};

#endif

