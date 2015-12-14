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
#include "Logger/LogEntry.h"
#include "Util/OmnNew.h"

class AosBSON;
class AosRundata;
class AosLoggerBSON;

class AosLoggerBSON : public AosLogger
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxEntries = 100
	};

	OmnMutexPtr				mLock;
	vector<AosLogEntryObj>	mLogEntries;
	u32						mEntryIdx;
	AosSeLogClientObj *		mLogClient;

public:
	AosLoggerBSON(const int version);
	AosLoggerBSON(const AosLoggerBSON &rhs);
	~AosLoggerBSON();

	virtual bool config(const AosRundataPtr &rdata,
						const AosXmlTagPtr &worker_doc,
						const AosXmlTagPtr &jimo_doc);

	virtual AosJimoPtr cloneJimo() const;

	virtual AosLogEntryObj & getLogEntry(AosRundata *rdata);
	virtual bool closeEntry(AosRundata *rdata, AosLogEntryBSON *entry);
	virtual bool closeLogEntry(AosRundata *rdata, const AosBSON *bson);
};
#endif
