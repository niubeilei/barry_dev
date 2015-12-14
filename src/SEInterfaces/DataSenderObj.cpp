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
#include "SEInterfaces/DataSenderObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosDataSenderObjPtr sgDataSender[AosDataSenderType::eMax+1];
static OmnMutex    	 		sgLock;


AosDataSenderObj::AosDataSenderObj(
		const OmnString &name, 
		const AosDataSenderType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosDataSenderObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerDataSender(thisptr);
	}
}

AosDataSenderObj::~AosDataSenderObj()
{
}


bool
AosDataSenderObj::registerDataSender(const AosDataSenderObjPtr &sender)
{
	AosDataSenderType::E type = sender->mType;
	if (type <= AosDataSenderType::eInvalid || type >= AosDataSenderType::eMax)
	{
		OmnAlarm << "Invalid data sender id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosDataSenderType::addName(sender->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgDataSender[type])
	{
		sgDataSender[type] = sender;
	}
	return true;
}


AosDataSenderObjPtr
AosDataSenderObj::getDataSenderStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosDataSenderType::E type = AosDataSenderType::toEnum(id);
	aos_assert_rr(AosDataSenderType::isValid(type), rdata, 0);
	
	return sgDataSender[type]->clone(item, rdata);
}


AosDataSenderObjPtr
AosDataSenderObj::getDataSenderStatic(
		const AosDataSenderType::E type, 
		const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosDataSenderType::isValid(type), rdata, 0);
	return sgDataSender[type];	
}

