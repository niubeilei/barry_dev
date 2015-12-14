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
// 07/23/2012 Created by Felicia
////////////////////////////////////////////////////////////////////////////
#include "DataSampler/DataSamplerCreator.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataSamplerObj.h"
#include "SEInterfaces/DataSamplerType.h"
#include "Thread/Mutex.h"
#include "Util/Buff.h"


static AosDataSamplerObjPtr	sgSamplers[AosDataSamplerType::eMax];
static OmnMutex				sgLock;
static bool					sgInited = false;

AosDataSamplerObjPtr 
AosDataSamplerCreator::createStrField(const AosRundataPtr &rdata)
{
	//AosDataSamplerObjPtr field = sgSamplers[AosDataSamplerType::eStr];
	//aos_assert_rr(field, rdata, 0);
	//return field->clone();
	return 0;
}


AosDataSamplerObjPtr 
AosDataSamplerCreator::serializeFrom(
		const AosBuffPtr &buff, 
		const AosRundataPtr &rdata)
{
	AosDataSamplerType::E type = (AosDataSamplerType::E)buff->getU32(0);
	if (!AosDataSamplerType::isValid(type))
	{
		AosSetErrorU(rdata, "invalid_data_field_type") << ": " << (int)type << enderr;
		return 0;
	}

	AosDataSamplerObjPtr field = sgSamplers[type];
	if (!field)
	{
		AosSetErrorU(rdata, "field_not_registered") << ": " << (int)type << enderr;
		return 0;
	}

	field = field->clone();
	if (!field->serializeFrom(buff, rdata)) return 0;
	return field;
}


AosDataSamplerObjPtr 
AosDataSamplerCreator::createDataSampler(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if(!sgInited) init();
	aos_assert_rr(def, rdata, 0);
	OmnString ss = def->getAttrStr(AOSTAG_TYPE);
	AosDataSamplerType::E type = AosDataSamplerType::toEnum(ss);
	aos_assert_rr(AosDataSamplerType::isValid(type), rdata, 0);
	AosDataSamplerObjPtr field = sgSamplers[type];
	if (!field)
	{
		AosSetErrorU(rdata, "data_field_not_registered") << ": " << ss;
		AosLogError(rdata);
		return 0;
	}

	return field->clone(def, rdata);
}


bool
AosDataSamplerCreator::init()
{
	if(sgInited)    return true;

	// static AosRecordVariable	lsAosRecordVariable(true);

	sgInited = true;
	return true;
}


bool
AosDataSamplerCreator::registerDataSampler(
		const OmnString &name,
		const AosDataSamplerObjPtr &field)
{
	sgLock.lock();
	if (!AosDataSamplerType::isValid(field->getType()))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect id: ";
		errmsg << field->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgSamplers[field->getType()])
	{
		sgLock.unlock();
		OmnString errmsg = "Already registered: ";
		errmsg << field->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgSamplers[field->getType()] = field;
	sgLock.unlock();
	return true;
}

