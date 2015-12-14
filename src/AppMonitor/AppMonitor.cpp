////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppMonitor.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppMonitor/AppMonitor.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util1/Timer.h"
#include "Util1/Ptrs.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"


OmnSingletonImpl(AosAppMonitorSingleton,
				 AosAppMonitor,
				 AosAppMonitorSelf,
				 "AosAppMonitor");

static OmnString sgSysPreInitFileName = "/usr/local/AOS/Bin/SysPreInit";
static OmnString sgSysPostInitFileName = "/usr/local/AOS/Bin/SysPostInit";
static OmnString sgSysInitConfFileName = "/usr/local/AOS/Data/SysInit.conf";


AosAppMonitor::AosAppMonitor()
{
}


bool
AosAppMonitor::start()
{
	const int safeguard = 100;

	// 
	// Run the script /usr/local/AOS/Bin/SysPreInit
	//
	system(sgSysPreInitFileName);

	// 
	// Open /usr/local/AOS/Bin/SysInit.conf
	//
	OmnFile theFile(sgSysInitConfFileName, OmnFile::eReadOnly);
	if (!theFile.isGood())
	{
		// 
		// Failed to open the file. Ignore it
		//
		system(sgSysPostInitFileName);
		return true;
	}

	int guard = 0;
	bool finished;
	bool found = false;
	OmnTimerObjPtr selfPtr(this, false);
	while (guard++ < safeguard)
	{
		OmnString line = theFile.getLine(finished);
		if (finished)
		{
			// 
			// Did not find the entry. Ignore it.
			//
			break;
		}

		// 
		// Check whether it is "sys_init_timer = ddd"
		//
		// if (it is "sys_init_timer = ddd")
		// {
			int second = 5;		// Testing purpose only
			cout << "To start timer" << endl;
			OmnTimer::getSelf()->startTimer("AppMonitor", second, 0, selfPtr, 0);
			return true;
		// }
	}

	if (!found)
	{
		system(sgSysPostInitFileName);
	}
		
	return true;
}


bool
AosAppMonitor::stop()
{
	return true;
}


AosAppMonitor::~AosAppMonitor()
{
}


OmnRslt
AosAppMonitor::config(const OmnXmlParserPtr &conf)
{
	if (!conf)
	{
		return true;
	}

	return true;
}


void        
AosAppMonitor::timeout(const int timerId,
                       const OmnString &timerName,
                       void *parm)
{
	system(sgSysPostInitFileName);
}


