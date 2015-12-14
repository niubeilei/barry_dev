////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TraceEntry.cpp
// Description:
//   
//
// Modification History:
//		Add time in print out. 
////////////////////////////////////////////////////////////////////////////

#include "Tracer/TraceEntry.h"

#include "Porting/ThreadDef.h"
#include "Util/IpAddr.h"
#include "Util1/Time.h"

extern OmnTracer *	OmnTracerSelf;

// Chen Ding, 2013/01/27
// static unsigned int sgSecTick = 0;


void	
OmnTraceEntry::setFileLine(const char *file, const int line)
{
	if (!mEnabled)
	{
		return;
	}

	char buf[100];
	int length = strlen(file);
	mIndex = 1;
	mData[0] = '<';

	// Chen Ding, 2013/01/27. Add time
	//sprintf(buf, "%s ", OmnTime::getTimeCharStr());
	//strcpy(&mData[mIndex], buf);
	//mIndex += strlen(buf);

	// Add current thread ID and tick.
	sprintf(buf, "0x%08x", (unsigned int)OmnGetCurrentThreadId()); // Ken Lee, 2013/05/28
	//sprintf(buf, "%s 0x%x", OmnTracer::getSelf()->getTimeStr(), (unsigned int)OmnGetCurrentThreadId());
	buf[99] = 0;
	strcpy(&mData[mIndex], buf);
	mIndex += strlen(buf);
	mData[mIndex++] = ':';
	
	// Ken 2013/04/24
	OmnString time = OmnGetTime(AosLocale::eChina);  
	strncpy(&mData[mIndex], time.data(), time.length());
	mIndex += time.length();
	mData[mIndex++] = ':';

	strncpy(&mData[mIndex], file, length);
	mIndex += length;
	mData[mIndex++] = ':';

	sprintf(buf, "%d", line);
	buf[9] = 0;
	strcpy(&mData[mIndex], buf);
	mIndex += strlen(buf);

	mData[mIndex++] = '>';
	mData[mIndex++] = ' ';
	mData[mIndex] = 0;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (const std::string &str)
{
	*this << str.data();
	return *this;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (const char *str)
{
	if (!mEnabled || !str)
	{
		return *this;
	}

	int length = strlen(str);
	if (mIndex + length >= eBufferLength)
	{
		length = eBufferLength - mIndex - 2;
	}

	if(length < 0)length = 0;
	strncpy(&mData[mIndex], str, length);
	mIndex += length;
	mData[mIndex] = 0;

	return *this;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (ostream & (*f)(ostream &outs))
{
	if (!mEnabled)
	{
		return *this;
	}

	mData[mIndex++] = '\n';
	mData[mIndex++] = 0;
	mFinished = true;

	/*
	if (OmnTracerSelf)
	{
		OmnTracerSelf->writeTraceEntry(mData, mIndex);
	}*/

	mIndex = 0;
	mData[mIndex] = 0;
	return *this;
}


/*
OmnTraceEntry & 
OmnTraceEntry::operator << (const OmnIpAddr &addr)
{
	if (!mEnabled)
	{
		return *this;
	}

	OmnTraceEntry::operator << (addr.toString());
	return *this;
}
*/


OmnTraceEntry & 
OmnTraceEntry::operator << (const u64 v)
{
	if (!mEnabled)
	{
		return *this;
	}
	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%llu", (unsigned long long int)v);
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}
	return *this;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (const char c)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 1 < eBufferLength)
	{
		mData[mIndex++] = c;
		mData[mIndex] = 0;
	}
	return *this;
}

	
OmnTraceEntry & 
OmnTraceEntry::operator << (const void * const v)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%lx", (unsigned long)v);
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (const unsigned int v)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%u", v);
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}


OmnTraceEntry & 
OmnTraceEntry::operator << (const int v)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%d", v);
		// buf[9] = 0;
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}

/*
OmnTraceEntry & 
OmnTraceEntry::operator << (const long l)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 16 < eBufferLength)
	{
		char buf[100];
		sprintf(buf, "%ld", l);
		buf[15] = 0;
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}
*/


OmnTraceEntry & 
OmnTraceEntry::operator << (const float f)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%f", f);
		buf[19] = 0;
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}	


OmnTraceEntry & 
OmnTraceEntry::operator << (const double f)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[20];
		sprintf(buf, "%fL", f);
		buf[19] = 0;
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}	


// Chen Ding, 05/26/2003
// Chen Ding, 2013/01/27
// void
// OmnTraceEntry::setTick(const int tick)
// {
// 	sgSecTick = tick;
// }


OmnTraceEntry & 
OmnTraceEntry::operator << (const int64_t ll)
{
	if (!mEnabled)
	{
		return *this;
	}

	if (mIndex + 20 < eBufferLength)
	{
		char buf[50];
		// ll.toAscii(buf);
		aos_lltoa(ll, buf);
		strcpy(&mData[mIndex], buf);
		mIndex += strlen(buf);

		mData[mIndex] = 0;
	}

	return *this;
}


