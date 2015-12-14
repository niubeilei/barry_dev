//////////////////////////////////////////////////////////////////////////
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
// 04/13/2009: Created by Sharon Shen
// 01/01/2013: Rewritten by Chen Ding
//////////////////////////////////////////////////////////////////////////
#ifndef AOS_LogUtil_LogEntryNameValue_h
#define AOS_LogUtil_LogEntryNameValue_h

#include "LogUtil/LogEntry.h"
#include "Util/Buff.h"


class AosLogEntryNameValue : public AosLogEntry
{
private:
	AosBuffPtr	mBuff;

public:
	AosLogEntryNameValue();
	~AosLogEntryNameValue();

	virtual bool serializeTo(
				const AosBuffPtr &buff, 
				const AosRundataPtr &rdata);

	virtual bool appendField(
				const AosBuffPtr &buff,
				const OmnString &name, 
				const OmnString &value);

	virtual bool appendField(
				const AosBuffPtr &buff,
				const OmnString &name, 
				const u64 &value);
};
#endif

