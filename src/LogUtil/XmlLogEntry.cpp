////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet LogEntryering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet LogEntryering, Inc. or its derivatives
//	
// Description:
//
// Modification History:
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogUtil/XmlLogEntry.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util1/Time.h"


AosXmlLogEntry::AosXmlLogEntry()
{
	reset();
}


AosXmlLogEntry::~AosXmlLogEntry()
{
}


void
AosXmlLogEntry::reset()
{
	// The entry format is:
	// 	<entry time="xxx" timestamp="xxx" thread="xxx">
	// 		...
	// 	</entry>
	mContents = "<entry time=\"";
	mContents << OmnTime::getTimeCharStr() << "\" timestamp=\""
		<< OmnGetTimestamp() << "\" thread=\""
		<< OmnGetCurrentThreadId() << "\">";
}
