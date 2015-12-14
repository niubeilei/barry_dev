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
#include "SEInterfaces/DataBlobObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


AosDataBlobObjPtr AosDataBlobObj::smDataBlobObj;


AosDataBlobObjPtr
AosDataBlobObj::createDataBlobStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smDataBlobObj, rdata, 0);
	return smDataBlobObj->createDataBlob(def, rdata);
}


AosDataBlobObjPtr
AosDataBlobObj::createDataBlobStatic(
		const AosDataRecordObjPtr &record,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smDataBlobObj, rdata, 0);
	return smDataBlobObj->createDataBlob(record, rdata);
}
	

AosDataBlobObjPtr
AosDataBlobObj::createDataBlobStatic(
		const OmnString &sep,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smDataBlobObj, rdata, 0);
	return smDataBlobObj->createDataBlob(sep, rdata);
}


AosDataBlobObjPtr 
AosDataBlobObj::serializeFromStatic(
		const AosBuffPtr &buff,
		const AosRundataPtr &rdata)
{
	//aos_assert_r(smDataBlobObj, NULL);
	AosDataBlobObjPtr datablob = smDataBlobObj->clone();
	datablob->serializeFrom(buff, rdata);
	return datablob;
}

