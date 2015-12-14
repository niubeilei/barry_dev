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
#include "Logger/Jimos/BSONLogEntry.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BSON/BSON.h"
#include "Util/OmnNew.h"


AosLogEntryBSON::AosLogEntryBSON()
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


AosLogEntryBSON::~AosLogEntryBSON()
{
}


bool
AosLogEntryBSON::closeLogEntry(
		AosRundata *rdata, 
		const AosBSON *bson)
{
	mLogClient->addLog(rdata, mLogName, bson->getBuffRaw());		
}

u64 docid = 123;
OmnString objid = "abc";

logentry << AosEnumFieldName::eDocid  << docid
	<< AosEnumFieldName::eObjid << objid << endlog;

{
	eObjid: "abc"
}


AosLogEntryObj & 
AosLogEntryBSON::LogEntry::operator << (const AosEnumFieldName::E fname)
{
	mEnumFieldName = fname;
	mFieldNameType = 2;
	return *this;
}


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::LogEntry::operator << (const u64 value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::LogEntry::operator << (const u32 value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::LogEntry::operator << (const u8 value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::LogEntry::operator << (const i64 value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::operator << (const i32 value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::operator << (const float value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::operator << (const double value)
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


AosLogEntryBSON::LogEntry & 
AosLogEntryBSON::LogEntry::operator << (const OmnString & value)
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
AosLogEntryBSON::LogEntry::operator << (const AosEndLogEntry &endlog)
{
	mLogger->closeLogEntry(mRdata, mBSONRaw);
	reset();
}

