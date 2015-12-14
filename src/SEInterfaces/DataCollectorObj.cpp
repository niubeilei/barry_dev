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
// 07/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataCollectorObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


AosDataCollectorCreatorObjPtr AosDataCollectorObj::smCreator;


AosDataCollectorObjPtr
AosDataCollectorObj::createDataCollectorStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, 0);
	return smCreator->createDataCollector(def, rdata);
}


AosDataCollectorObjPtr 
AosDataCollectorObj::serializeFromStatic(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	//aos_assert_r(smDataCollectorObj, NULL);
	//AosDataCollectorObjPtr datablob = smDataCollectorObj->clone();
	//datablob->serializeFrom(buff, rdata);
	//return datablob;
	return 0;
}

	
bool
AosDataCollectorObj::checkConfigStatic(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, false);
	return smCreator->checkConfig(def, rdata);
}

