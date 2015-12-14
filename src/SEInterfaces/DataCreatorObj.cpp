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
#include "SEInterfaces/DataCreatorObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosDataCreatorObjPtr sgDataCreator[AosDataCreatorType::eMax+1];
static OmnMutex    	 		sgLock;


AosDataCreatorObj::AosDataCreatorObj(
		const OmnString &name, 
		const AosDataCreatorType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosDataCreatorObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerDataCreator(thisptr);
	}
}

AosDataCreatorObj::~AosDataCreatorObj()
{
}


bool
AosDataCreatorObj::registerDataCreator(const AosDataCreatorObjPtr &sorter)
{
	AosDataCreatorType::E type = sorter->mType;
	if (type <= AosDataCreatorType::eInvalid || type >= AosDataCreatorType::eMax)
	{
		OmnAlarm << "Invalid data creator id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosDataCreatorType::addName(sorter->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgDataCreator[type])
	{
		sgDataCreator[type] = sorter;
	}
	return true;
}


AosDataCreatorObjPtr
AosDataCreatorObj::getDataCreatorStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosDataCreatorType::E type = AosDataCreatorType::toEnum(id);
	aos_assert_rr(AosDataCreatorType::isValid(type), rdata, 0);
	
	return sgDataCreator[type]->clone(item, rdata);
}


AosDataCreatorObjPtr
AosDataCreatorObj::getDataCreatorStatic(
		const AosDataCreatorType::E type, 
		const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosDataCreatorType::isValid(type), rdata, 0);
	return sgDataCreator[type];	
}

