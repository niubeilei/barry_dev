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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ConditionObj.h"

#include "Rundata/Rundata.h"


AosConditionObjPtr AosConditionObj::smCondition;


AosConditionObjPtr 
AosConditionObj::getConditionStatic(
		const AosXmlTagPtr &conf, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(smCondition, rdata, 0);
	return smCondition->getCondition(conf, rdata);
}

