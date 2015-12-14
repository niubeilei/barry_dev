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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ConditionObj.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


bool AosEvalCond(
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	AosConditionObjPtr condition = AosConditionObj::getCondition();
	aos_assert_r(condition, false);
	return condition->evalCond(def, rdata);
}

