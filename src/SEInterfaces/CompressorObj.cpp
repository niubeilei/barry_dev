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
#include "SEInterfaces/CompressorObj.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Util/ValueRslt.h"
#include "Util/StrSplit.h"


extern AosCompressorObjPtr sgCompressor[AosCompressorType::eMax+1];
static OmnMutex    	 		sgLock;


AosCompressorObj::AosCompressorObj(
		const OmnString &name, 
		const AosCompressorType::E type, 
		const bool reg)
:
mName(name),
mType(type),
mIsTemplate(false)
{
	AosCompressorObjPtr thisptr(this, false);
	if (reg) 
	{
		mIsTemplate = true;
		registerCompressor(thisptr);
	}
}

AosCompressorObj::~AosCompressorObj()
{
}


bool
AosCompressorObj::registerCompressor(const AosCompressorObjPtr &compressor)
{
	AosCompressorType::E type = compressor->mType;
	if (type <= AosCompressorType::eInvalid || type >= AosCompressorType::eMax)
	{
		OmnAlarm << "Invalid data creator id: " << type << enderr;
		return false;
	}
	
	OmnString errmsg;
	bool rslt = AosCompressorType::addName(compressor->mName, type, errmsg);
	if (!rslt)
	{
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (!sgCompressor[type])
	{
		sgCompressor[type] = compressor;
	}
	return true;
}


AosCompressorObjPtr
AosCompressorObj::getCompressorStatic(const AosXmlTagPtr &item, const AosRundataPtr &rdata) 
{
	OmnString id = item->getAttrStr(AOSTAG_TYPE);
	aos_assert_rr(id != "", rdata, 0);
	
	AosCompressorType::E type = AosCompressorType::toEnum(id);
	aos_assert_rr(AosCompressorType::isValid(type), rdata, 0);
	
	return sgCompressor[type]->clone(item, rdata);
}


AosCompressorObjPtr
AosCompressorObj::getCompressorStatic(
		const AosCompressorType::E type, 
		const AosRundataPtr &rdata) 
{
	aos_assert_rr(AosCompressorType::isValid(type), rdata, 0);
	return sgCompressor[type];	
}

