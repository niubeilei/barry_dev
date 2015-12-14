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
//////////////////////////////////////////////////////////////////////////
#include "LogUtil/LogEntry.h"

#include "alarm_c/alarm.h"
#include "util_c/strutil.h"
#include "Rundata/Rundata.h"

AosLogEntry::AosLogEntry()
{
}


AosLogEntry::~AosLogEntry()
{
}


AosRundataPtr
AosLogEntry::getRundata() const
{
	return mRundata;
}

