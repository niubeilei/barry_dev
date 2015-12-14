////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TraceEntry.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TraceEntry_TraceEntry_h
#define Omn_TraceEntry_TraceEntry_h

#include "aosUtil/Types.h"
#include "Tracer/Tracer.h"
#include <string>
#include <iostream>


class OmnIpAddr;

using namespace std;

class OmnTraceEntry
{
	OmnDefineRCObject;

private:
	enum 
	{
		eBufferLength = 2000,
	};

	int			mIndex;
	char		mData[eBufferLength];
	bool		mFinished;
	bool		mEnabled;
	// OmnString	mTimeStr;		// Chen Ding, 12/13/2012

public:
	OmnTraceEntry() 
	:
	mIndex(0),
	mFinished(false),
	mEnabled(true)
	{
	}

	OmnTraceEntry(const bool flag)
	:
	mIndex(0),
	mFinished(false),
	mEnabled(flag)
	{
	}

	virtual ~OmnTraceEntry() {}

	void	setFileLine(const char *file, const int line);
	OmnTraceEntry & operator << (const std::string &str);
	OmnTraceEntry & operator << (const OmnString &str)
	{
		return operator << (str.data());
	}
	OmnTraceEntry & operator << (const char *str);
	OmnTraceEntry & operator << (ostream & (*f)(ostream &outs));
	//OmnTraceEntry & operator << (const long unsigned int v);
	OmnTraceEntry & operator << (const char c);
	OmnTraceEntry & operator << (const void * constv);
	OmnTraceEntry & operator << (const unsigned int v);
	OmnTraceEntry & operator << (const int v);
	OmnTraceEntry & operator << (const float f);
	OmnTraceEntry & operator << (const double f);
	OmnTraceEntry & operator << (const int64_t ll);
	OmnTraceEntry & operator << (const u64 ll);
#ifndef __amd64__
	OmnTraceEntry & operator << (const unsigned long ll)
	{
		if (!mEnabled)
		{
			return *this;
		}

		if (mIndex + 16 < eBufferLength)
		{
			char buf[100];
			sprintf(buf, "%lu", ll);
			buf[15] = 0;
			strcpy(&mData[mIndex], buf);
			mIndex += strlen(buf);

			mData[mIndex] = 0;
		}
		return *this;
	}
#endif
	// Chen Ding, 05/26/2003
	// Chen Ding, 2013/01/27
	// static void setTick(const int tick);
};


#endif
