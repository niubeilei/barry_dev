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
// This is a utility to select docs.
//
// Modification History:
// 11/01/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#include "DataGenUtil/DataGenUtil.h"

#include "alarm_c/alarm.h"
#include "DataGenUtil/AllDataGenUtil.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"

extern AosDataGenUtilPtr	sgDataGenUtil[AosDataGenUtil::eMax+1];
static OmnMutex				sgLock;



AosDataGenUtil::AosDataGenUtil(
		const OmnString &name,
		const AosDataGenUtilType::E type, 
		const bool reg)
:
mType(type),
mLock(OmnNew OmnMutex())
{
	AosDataGenUtilPtr thisptr(this, false);
	if (reg) registerDataGen(name, thisptr);
}


AosDataGenUtil::~AosDataGenUtil()
{
}


bool
AosDataGenUtil::registerDataGen(const OmnString &name, const AosDataGenUtilPtr &uu)
{
	AosDataGenUtilType::E type = uu->getType();
	if (!AosDataGenUtilType::isValid(type))
	{
		OmnAlarm << "Invalid Data Gen id: " << type << enderr;
		return false;
	}

	mLock->lock();
	if (sgDataGenUtil[type])
	{
		mLock->unlock();
		OmnAlarm << "Data Gen already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgDataGenUtil[type] = uu;
	bool rslt = AosDataGenUtilType::addName(name, uu->mType);
	aos_assert_r(AosDataGenUtilType::toEnum(name), false);
	mLock->unlock();
	return rslt;
}


bool
AosDataGenUtil::nextValueStatic(
			AosValueRslt &value, 
			const AosXmlTagPtr &sdoc,
			const AosRundataPtr &rdata)
{
	OmnString id = sdoc->getAttrStr(AOSTAG_DATAGEN_TYPE1, "");
	aos_assert_rr(id != "", rdata, false);
	AosDataGenUtilType::E type = AosDataGenUtilType::toEnum(id);
	if (!AosDataGenUtilType::isValid(type))
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() << "Unrecognized DataGen Type: " << id << enderr;
		return false;
	}

	sgLock.lock();
	AosDataGenUtilPtr uu = sgDataGenUtil[type];
	sgLock.unlock();
	aos_assert_rr(uu, rdata, false);
	return uu->nextValue(value, sdoc, rdata);
}


AosDataGenUtilPtr
AosDataGenUtil::getDataGenUtilStatic(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(sdoc, rdata, 0);
	OmnString id = sdoc->getAttrStr(AOSTAG_DATAGEN_TYPE1, "");
	aos_assert_r(id != "", 0);
	AosDataGenUtilType::E type = AosDataGenUtilType::toEnum(id);
	if (!AosDataGenUtilType::isValid(type))
	{
		AosSetError(rdata, AosErrmsgId::eInternalError);
		OmnAlarm << rdata->getErrmsg() <<"Unrecognized DataGen Type: " << id << enderr;
		return 0;
	}

	sgLock.lock();
	AosDataGenUtilPtr uu = sgDataGenUtil[type];
	sgLock.unlock();
	aos_assert_r(uu, 0);
	AosDataGenUtilPtr datagen = uu->clone(sdoc, rdata);
	aos_assert_r(datagen, 0);
	return datagen;
}

