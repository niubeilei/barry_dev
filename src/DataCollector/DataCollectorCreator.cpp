////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DataCollector/DataCollectorCreator.h"

#include "SEInterfaces/DataCollectorObj.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "DataCollector/DataCollector.h"



bool 
AosDataCollectorCreator::registerDataCollector(
		const OmnString &name, 
		const AosDataCollectorObjPtr &collector)
{
	OmnNotImplementedYet;
	return false;
}


AosDataCollectorPtr 
AosDataCollectorCreator::createDataCollector(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosDataCollector(def, rdata);
	}
	catch(...)
	{
		OmnAlarm << "failed to create dataCollector" << enderr;
		return 0;	
	}
	OmnShouldNeverComeHere;
	return 0;
}


AosDataCollectorPtr 
AosDataCollectorCreator::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return 0;
}

	
bool
AosDataCollectorCreator::checkConfig(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	return AosDataCollector::checkConfigStatic(def, rdata);
}

#endif
