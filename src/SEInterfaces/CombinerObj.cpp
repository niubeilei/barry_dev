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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/CombinerObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosCombinerObjPtr 	sgCombiner[AosCombinerType::eMax+1];
static OmnMutex    	 		sgLock;


AosCombinerObj::AosCombinerObj(
		const OmnString &name, 
		const AosCombinerType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosCombinerObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerCombiner(thisptr);
	}
}

AosCombinerObj::~AosCombinerObj()
{
}


bool
AosCombinerObj::registerCombiner(const AosCombinerObjPtr &combiner)
{
	AosCombinerType::E type = combiner->mType;
	if (type <= AosCombinerType::eInvalid || type >= AosCombinerType::eMax)
	{
		OmnAlarm << "Invalid action id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosCombinerType::addName(combiner->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgCombiner[type])
	{
		sgCombiner[type] = combiner;
	}
	return true;
}


AosCombinerObjPtr
AosCombinerObj::getCombinerStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosCombinerType::E type = AosCombinerType::toEnum(id);
	aos_assert_rr(AosCombinerType::isValid(type), rdata, 0);
	
	return sgCombiner[type]->clone(item, rdata);
}


AosCombinerObjPtr
AosCombinerObj::getCombinerStatic(const AosCombinerType::E type, const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosCombinerType::isValid(type), rdata, 0);
	return sgCombiner[type];	
}

