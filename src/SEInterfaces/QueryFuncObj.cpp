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
// 2014/01/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryFuncObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"


AosQueryFuncObjPtr	AosQueryFuncObj::smObject;


AosQueryFuncObj::AosQueryFuncObj(const int version)
:
AosJimo(AosJimoType::eQueryFunc, version)
{
}


AosQueryFuncObj::~AosQueryFuncObj()
{
}


bool 
AosQueryFuncObj::evalStatic(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &def,
		AosValueRslt &value)
{
	aos_assert_rr(smObject, rdata, 0);
	aos_assert_rr(def, rdata, false);

	return smObject->pickJimo(rdata, def, value);
}


