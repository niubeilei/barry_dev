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
// 09/02/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DataColComp/DataColComp.h"

#include "DataColComp/DataColCompMap.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"

static AosDataColCompPtr 	sgColComp[AosDataColCompType::eMax];
static OmnString		sgNames[AosDataColCompType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;

AosDataColComp::AosDataColComp(
		const OmnString &name, 
		const AosDataColCompType::E type,
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		 AosDataColCompPtr thisptr(this, false);   // bool false: true :  
		if (!registerColComp(name, thisptr))
		{
			OmnThrowException("Failed registering");
			return;
		}
	}
}

AosDataColComp::AosDataColComp(const AosDataColComp &rhs)
:
mType(rhs.mType)
{
}



AosDataColComp::~AosDataColComp()
{
}


bool
AosDataColComp::registerColComp(const OmnString &name, const AosDataColCompPtr  &b)
{
	sgLock.lock();
	if (!AosDataColCompType::isValid(b->mType))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data shuffler type: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgColComp[b->mType])
	{
		sgLock.unlock();
		OmnString errmsg = "Proc already registered: ";
		errmsg << b->mType;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgColComp[b->mType] = b;
	AosDataColCompType::addName(name, b->mType);
	sgLock.unlock();
	return true;
}


bool
AosDataColComp::init(const AosRundataPtr &rdata)
{
	static AosDataColCompMap 		lsDataColCompMap(true);

	sgInitLock.lock();
	sgInited = true;
	AosDataColCompType::check();
	sgInitLock.unlock();
	return true;
}

AosDataColCompPtr
AosDataColComp::createDataColComp(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);
	aos_assert_rr(def, rdata, 0);
	OmnString ss = def->getAttrStr(AOSTAG_TYPE, "");
	if (ss == "") ss = def->getAttrStr(AOSTAG_ZKY_TYPE, "");
	AosDataColCompType::E type = AosDataColCompType::toEnum(ss);
	aos_assert_rr(AosDataColCompType::isValid(type), rdata, 0);
	AosDataColCompPtr obj = sgColComp[type];
	if (!obj)
	{
		AosSetErrorU(rdata, "data_colcomp_not_registered") << ss << enderr;
		return 0;
	}

	return obj->clone(def, rdata);
}

