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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataRecordObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"


AosDataRecordObjPtr AosDataRecordObj::smCreator;

	
AosDataRecordObj::AosDataRecordObj(const int version)
:
AosJimo(AosJimoType::eDataRecord, version)
{
}


AosDataRecordObjPtr
AosDataRecordObj::createDataRecordStatic(
		const AosXmlTagPtr &def, 
		const u64 task_docid,
		AosRundata *rdata AosMemoryCheckDecl)
{
	aos_assert_r(smCreator, 0);
	return smCreator->createDataRecord(def, task_docid, rdata AosMemoryCheckerFileLine);
}


// Chen Ding, 2013/12/25
bool 
AosDataRecordObj::isValid(AosRundata *rdata) 
{
	return true;
}

