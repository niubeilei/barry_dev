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
#include "SEInterfaces/SorterObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosSorterObjPtr sgSorter[AosSorterType::eMax+1];
static OmnMutex    	 		sgLock;


AosSorterObj::AosSorterObj(
		const OmnString &name, 
		const AosSorterType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosSorterObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerSorter(thisptr);
	}
}

AosSorterObj::~AosSorterObj()
{
}


bool
AosSorterObj::registerSorter(const AosSorterObjPtr &sorter)
{
	AosSorterType::E type = sorter->mType;
	if (type <= AosSorterType::eInvalid || type >= AosSorterType::eMax)
	{
		OmnAlarm << "Invalid sorter id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	if (!sgSorter[type])
	{
		sgSorter[type] = sorter;
	}
	return true;
}


AosSorterObjPtr
AosSorterObj::getSorterStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosSorterType::E type = AosSorterType::toEnum(id);
	aos_assert_rr(AosSorterType::isValid(type), rdata, 0);
	
	return sgSorter[type]->clone(item, rdata);
}


AosSorterObjPtr
AosSorterObj::getSorterStatic(const AosSorterType::E type, const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosSorterType::isValid(type), rdata, 0);
	return sgSorter[type];	
}

