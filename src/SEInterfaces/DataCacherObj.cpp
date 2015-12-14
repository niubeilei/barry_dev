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
// 07/17/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataCacherObj.h"

#include "Rundata/Rundata.h"
#include "SEInterfaces/BuffData.h"


AosDataCacherCreatorObjPtr AosDataCacherObj::smCreator;

AosDataCacherObj::AosDataCacherObj(
		const OmnString &name, 
		const AosDataCacherType::E type, 
		const bool flag)
:
mType(type)
{
	if (flag)
	{
		AosDataCacherObjPtr thisptr(this, false);
		if (!AosDataCacherObj::registerDataCacher(name, thisptr))
		{
			OmnThrowException("failed_registering");
			return;
		}
	}
}


AosDataCacherObj::AosDataCacherObj()
{
}

AosDataCacherObj::~AosDataCacherObj()
{
}


AosDataCacherObjPtr 
AosDataCacherObj::createDataCacher(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->createCacher(def, rdata);
}
	

AosDataCacherObjPtr
AosDataCacherObj::createDataCacher(
 		const AosDataScannerObjPtr &scanner,
 		const AosDataBlobObjPtr &blob, 
 		const AosRundataPtr &rdata)
{
 	aos_assert_rr(smCreator, rdata, NULL);
 	return smCreator->createCacher(scanner, blob, rdata);
}


AosDataCacherObjPtr 
AosDataCacherObj::serializeFromStatic(const AosBuffPtr &def, const AosRundataPtr &rdata)
{
	aos_assert_rr(smCreator, rdata, NULL);
	return smCreator->serializeFrom(def, rdata);
}


bool
AosDataCacherObj::registerDataCacher(const OmnString &name, const AosDataCacherObjPtr &cacher)
{
	aos_assert_r(smCreator, false);
	return smCreator->registerCacher(name, cacher);
}

