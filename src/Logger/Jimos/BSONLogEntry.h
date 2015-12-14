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
#ifndef Aos_Logger_Jimos_BSONLogger_h
#define Aos_Logger_Jimos_BSONLogger_h

#include "BSON/Ptrs.h"
#include "Logger/Logger.h"
#include "Logger/LogEntry.h"
#include "Util/OmnNew.h"

class AosBSON;
class AosRundata;
class AosBSONLogger;

struct AosLogEntryBSON : public AosLogEntry
{
	OmnDefineRCObject;

private:
	AosRundataPtr	mRdata;
	AosBSONPtr		mBSON;
	AosBSON*		mBSONRaw;
	AosBSONLogger *	mLogger;
	int				mEnumFieldName;
	AosValueRslt	mLastValue;
	OmnString		mStrFieldName;

public:
	AosLogEntryBSON()
	:
	mRdata(0),
	mBSONRaw(0),
	mLogger(0),
	mNumericalFieldName(0)
	{
		mBSON = OmnNew AosBSON();
		mBSONRaw = mBSON.getPtr();
	}

	AosLogEntryBSON(AosRundata *rdata, AosBSONLogger *logger)
	:
	mRdata(rdata),
	mBSONRaw(0),
	mLogger(logger),
	mNumericalFieldName(0)
	{
		mBSON = OmnNew AosBSON();
		mBSONRaw = mBSON.getPtr();
	}

	void reset()
	{
		mRdata = 0;
		mBSONRaw->clear();
		mNumericalFieldName = AosEnumFieldName::eInvalidFieldName;
		mStrFieldName = "";
	}

	void
	set(AosRundata *rdata, AosBSONLogger *logger)
	{
		mRdata = rdata;
		mLogger = logger;
	}

	AosBuff *getBuffRaw() const;

	virtual AosLogEntryObj & operator << (const AosEnumFieldName::E name);
	virtual AosLogEntryObj & operator << (const u64 value);
	virtual AosLogEntryObj & operator << (const u32 value);
	virtual AosLogEntryObj & operator << (const u8 value);
	virtual AosLogEntryObj & operator << (const i64 value);
	virtual AosLogEntryObj & operator << (const i32 value);
	virtual AosLogEntryObj & operator << (const i8 value);
	virtual AosLogEntryObj & operator << (const float value);
	virtual AosLogEntryObj & operator << (const double value);
	virtual AosLogEntryObj & operator << (const OmnString &value);
	virtual AosLogEntryObj & operator << (const AosXmlTagPtr &value);
	virtual void operator << (const AosEndLogEntry &);
};
#endif



