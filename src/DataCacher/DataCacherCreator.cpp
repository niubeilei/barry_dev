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
#include "DataCacher/DataCacherCreator.h"

#include "DataCacher/ScanCacher.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SEInterfaces/TaskDataObj.h"
#include "Thread/Mutex.h"
#include "DataCacher/DataCacherSimple.h"
// #include "DataCacher/DataCacherMulti.h"
#include "DataCacher/CacherFile.h"
#include "DataCacher/CacherVirtualFile.h"


static AosDataCacherObjPtr sgCachers[AosDataCacherType::eMax];
static OmnMutex			sgLock;
static OmnMutex			sgInitLock;
static bool				sgInited = false;


bool
AosDataCacherCreator::init(const AosRundataPtr &rdata)
{
	if (sgInited) return true;

 	static AosDataCacherObjPtr lsFileCacher = OmnNew AosCacherFile(true);
 	// static AosDataCacherObjPtr lsVirtualFileCacher = OmnNew AosCacherVirtualFile(true);

	sgInitLock.lock();
	AosDataCacherType::check();
	sgInited = true;
	sgInitLock.unlock();
	return true;
}


AosDataCacherObjPtr 
AosDataCacherCreator::createCacher(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	aos_assert_rr(def, rdata, 0);
	AosDataCacherType::E type = AosDataCacherType::toEnum(def->getAttrStr(AOSTAG_TYPE));
	aos_assert_rr(AosDataCacherType::isValid(type), rdata, 0);
	sgLock.lock();
	AosDataCacherObjPtr cacher = sgCachers[type];
	sgLock.unlock();
	//return cacher;
	return cacher->clone(def, rdata);
}


AosDataCacherObjPtr
AosDataCacherCreator::createCacher(
 		const AosDataScannerObjPtr &scanner,
 		const AosDataBlobObjPtr &blob, 
 		const AosRundataPtr &rdata)
{
 	return OmnNew AosScanCacher(scanner, blob, rdata); 
}


AosDataCacherObjPtr 
AosDataCacherCreator::serializeFrom(const AosBuffPtr &buff, const AosRundataPtr &rdata)
{
	if (!sgInited) init(rdata);

	AosDataCacherType::E type = (AosDataCacherType::E)buff->getU32(-1);
	aos_assert_rr(AosDataCacherType::isValid(type), rdata, 0);
	sgLock.lock();
	AosDataCacherObjPtr cacher = sgCachers[type]->clone();
	sgLock.unlock();
	cacher->serializeFrom(buff, rdata);
	return cacher;
}


bool
AosDataCacherCreator::registerCacher(const OmnString &name, const AosDataCacherObjPtr &cacher)
{
	sgLock.lock();
	if (!AosDataCacherType::isValid(cacher->getType()))
	{
		sgLock.unlock();
		OmnString errmsg = "Incorrect data cacher type: ";
		errmsg << cacher->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	if (sgCachers[cacher->getType()])
	{
		sgLock.unlock();
		OmnString errmsg = "Cacher already registered: ";
		errmsg << cacher->getType();
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgCachers[cacher->getType()] = cacher;
	sgLock.unlock();
	return true;
}

