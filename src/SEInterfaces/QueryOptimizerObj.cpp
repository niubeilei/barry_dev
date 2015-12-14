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
// 2013/12/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/QueryOptimizerObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataScannerObj.h"


AosQueryOptimizerObj::AosQueryOptimizerObj(const u32 version)
:
AosJimo(AosJimoType::eQueryOptimizer, version)
{
}


AosQueryOptimizerObj::~AosQueryOptimizerObj()
{
}


AosQueryOptimizerObjPtr 
AosQueryOptimizerObj::createQueryOptimizerStatic(
		AosRundata *rdata, 
		const AosXmlTagPtr &def)
{
	AosJimoPtr jimo = AosCreateJimo(rdata, def);
	if (!jimo) return 0;

	if (jimo->getJimoType() != AosJimoType::eQueryOptimizer)
	{
		AosSetErrorUser(rdata, "queryoptimizerobj_invalid") << enderr;
		return 0;
	}

	AosQueryOptimizerObjPtr dd = dynamic_cast<AosQueryOptimizerObj*>(jimo.getPtr());
	if (!dd)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}
	return dd;
}

