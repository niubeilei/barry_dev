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
// 2013/01/27 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogUtil/XmlLog.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Util1/Time.h"


AosXmlLog::AosXmlLog()
{
	// The entry format is:
	// 	<entry time="xxx" timestamp="xxx">
	// 		...
	// 	</entry>
	mContents = "<xmllog time=\"";
	mContents << OmnTime::getTimeCharStr() << "\" timestamp=\""
		<< OmnGetTimestamp() << "\">";

	mEntry.reset();
}


AosXmlLog::~AosXmlLog()
{
}

