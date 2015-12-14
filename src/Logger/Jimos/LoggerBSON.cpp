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
// 2014/11/08 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "Logger/Jimos/LoggerBSON.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BSON/BSON.h"
#include "Util/OmnNew.h"


AosLoggerBSON::AosLoggerBSON(const int version)
:
AosLogger(version)
{
	AosSeLogClientObjPtr client = AosSeLogClientObj::getSelf();
	if (!client)
	{
		OmnThrowException("Log Client not set");
		return;
	}
	mLogClient = client.getPtr();
}


AosLoggerBSON::~AosLoggerBSON()
{
}


bool
AosLoggerBSON::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def)
{
	return true;
}


AosBSONPtr
AosLoggerBSON::createEntry()
{
	return OmnNew AosBSON();
}


bool
AosLoggerBSON::appendEntry(
		AosRundata *rdata, 
		const AosBSONPtr &entry)
{
	return mLogClient->addLog(rdata, mLogName, entry->getBuffRaw());
}


bool
AosLoggerBSON::appendLog(
		AosRundata *rdata, 
		const u32 field_name, 
		const OmnString &value)
{
	AosBSONPtr bson = OmnNew AosBSON();
	bson->appendFieldStr(field_name, value);
	return mLogClient->addLog(rdata, mLogName, bson->getBuffRaw());
}


AosLoggerBSON::LogEntry&
AosLoggerBSON::appendEntry(
		AosRundata *rdata, 
		const char *filename, 
		const int line)
{
	mLockRaw->lock();
	LogEntry entry = mLogEntries[mLogEntryIdx];
	mLogEntryIdx++;
	if (mLogEntryIdx >= mLogEntries.size()) mLogEntryIdx = 0;
	mLockRaw->unlock();

	AosBSON *bson = entry.getBSON();
	bson->appendFieldCharStr(AosFieldName::eFileName, filename);
	bson->appendFieldInt(AosFieldName::eLineNum, line);
	return entry;
}


bool
AosLoggerBSON::closeLogEntry(
		AosRundata *rdata, 
		const AosBSON *bson)
{
	mLogClient->addLog(rdata, mLogName, bson->getBuffRaw());		
}


AosLoggerBSON::LogEntry()
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


AosLoggerBSON::LogEntry & 
AosLoggerBSON::LogEntry::operator << (const u64 value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldU64(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldU64(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::LogEntry::operator << (const u32 value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldU32(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldU32(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::LogEntry::operator << (const u8 value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldU8(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldU8(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::LogEntry::operator << (const i64 value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldI64(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldI64(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::operator << (const i32 value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldI32(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldI32(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::operator << (const float value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldFloat(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldFloat(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::operator << (const double value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldDouble(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldDouble(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


AosLoggerBSON::LogEntry & 
AosLoggerBSON::LogEntry::operator << (const OmnString & value)
{
	switch (mFieldNameType)
	{
	case 1:
		 // String field name
		 mBSONRaw->appendFieldStr(mStrFieldName, value);
		 break;

	case 2:
		 // Numerical Field name
		 mBSONRaw->appendFieldStr(mNumFieldName, value);
		 break;

	default:
		 OmnAlarm << "Missing field name" << enderr;
		 return *this;
	}

	mFieldNameType = 0;
	return *this;
}


void
AosLoggerBSON::LogEntry::operator << (const AosEndLogEntry &)
{
	mLogger->closeLogEntry(mRdata, mBSONRaw);
}

