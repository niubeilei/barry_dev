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
//	String log entry assumes the logger is a file logger. This is a generic
//  string based log entry. Its log format is:
//
//		<File> <Line> <TimeTick> <Contents>   
//
// Modification History:
// 2013/03/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Logger/LogEntryDatalet.h"

#include "Alarm/Alarm.h"
#include "Logger/LoggerFile.h"
#include "Logger/LogListener.h"
#include "Util1/Time.h"




AosLogEntryDatalet::AosLogEntryDatalet(const int version)
:
AosLogEntry(version)
{
}


AosLogEntryDatalet::~AosLogEntryDatalet()
{
}


