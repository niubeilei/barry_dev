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
// 2013/03/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryReqObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

AosQueryReqObjPtr AosQueryReqObj::smQueryReqObj;
AosQueryReqObjPtr AosQueryReqObj::smQueryReqObjNew;

	
AosQueryReqObjPtr
AosQueryReqObj::createQueryStatic(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smQueryReqObj, 0);
	return smQueryReqObj->createQuery(def, rdata);
}


AosQueryReqObjPtr
AosQueryReqObj::createQueryStaticNew(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smQueryReqObj, 0);
	return smQueryReqObjNew->createQuery(def, rdata);
}
