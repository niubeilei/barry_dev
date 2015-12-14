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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondDriver.h"

#include "Conds/Condition.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"


bool 
AosCondDriver::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return AosCondition::evalCondStatic(def, rdata);
}


bool 
AosCondDriver::evalCond(
		const AosXmlTagPtr &def, 
		const OmnString &name, 
		const AosRundataPtr &rdata)
{
	return AosCondition::evalCondStatic(def, name, rdata);
}

