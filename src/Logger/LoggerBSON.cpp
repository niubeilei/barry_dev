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
#if 0
#include "Logger/Jimos/LoggerBSON.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BSON/BSON.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

AosJimoPtr AosCreateJimoFunc_AosLoggerBSON_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosLoggerBSON(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosLoggerBSON::AosLoggerBSON(const int version)
:
AosLogger(version),
mLock(OmnNew OmnMutex()),
mEntryIdx(0)
{
	AosSeLogClientObjPtr client = AosSeLogClientObj::getSelf();
	if (!client)
	{
		OmnThrowException("Log Client not set");
		return;
	}
	mLogClient = client.getPtr();

	for (u32 i=0; i<eMaxEntries; i++)
	{
		mLogEntries.push_back(AosLogEntryBSON());
	}
}


AosLoggerBSON::~AosLoggerBSON()
{
}


bool
AosLoggerBSON::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


AosJimoPtr
AosLoggerBSON::clone() const
{
	return OmnNew AosLoggerBSON(*this);
}


AosLogEntryObj &
AosLoggerBSON::getLogEntry(AosRundata *rdata)
{
	static AosLogEntryBSON lsEntry;

	mLockRaw->lock();
	aos_assert_rl(mEntryIndex <= mLogEntries.size(), mLock, lsEntry);
	if (mEntryIdx >= mLogEntries.size()) mEntryIdx = 0;
	int idx = mEntryIdx++;
	mLockRaw->unlock();
	return mLogEntries[idx];
}


bool
AosLoggerBSON::closeEntry(AosRundata *rdata, AosLogEntryBSON *entry)
{
	return mLogClient->addLog(rdata, mLogName, entry->getBuffRaw());
}


/*
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
*/


bool
AosLoggerBSON::closeLogEntry(
		AosRundata *rdata, 
		const AosBSON *bson)
{
	mLogClient->addLog(rdata, mLogName, bson->getBuffRaw());		
}

#endif
