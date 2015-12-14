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
// Modification History:
// 2014/11/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Logger_LoggerBSON_h
#define Aos_Logger_LoggerBSON_h

#include "BSON/Ptrs.h"
#include "Logger/Logger.h"
#include "Util/OmnNew.h"

class AosBSON;
class AosRundata;
class AosLoggerBSON;

class AosLoggerBSON : public AosLogger
{
	OmnDefineRCObject;

	struct LogEntry
	{
		AosRundata *	mRdata;
		AosBSONPtr		mBSON;
		AosBSON*		mBSONRaw;
		AosLoggerBSON *	mLogger;
		int				mFieldNameType;
		u32				mNumFieldName;
		OmnString		mStrFieldName;

		LogEntry()
		:
		mRdata(0),
		mBSONRaw(0),
		mLogger(0),
		mFieldNameType(0),
		mNumFieldName(0)
		{
			mBSON = OmnNew AosBSON();
			mBSONRaw = mBSON.getPtr();
		}

		void
		set(AosRundata *rdata, AosLoggerBSON *logger)
		{
			mRdata = rdata;
			mLogger = logger;
		}

		LogEntry & operator << (const u64 value);
		LogEntry & operator << (const u32 value);
		LogEntry & operator << (const u8 value);
		LogEntry & operator << (const i64 value);
		LogEntry & operator << (const i32 value);
		LogEntry & operator << (const float value);
		LogEntry & operator << (const double value);
		LogEntry & operator << (const OmnString & value);
		void operator << (const AosEndLogEntry &);
	};

public:
	AosLoggerBSON(const OmnString &type, const int version);
	~AosLoggerBSON();

};
#endif



