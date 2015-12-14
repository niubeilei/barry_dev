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
// 2013/01/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapStorageMgrObj.h"

#include "API/AosApi.h"


AosBitmapStorageMgrObjPtr AosBitmapStorageMgrObj::smObject;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


AosBitmapStorageMgrObj::AosBitmapStorageMgrObj(const int version)
:
AosJimo(AosJimoType::eBitmapStorageMgr, version)
{
}


static bool AosCreateBitmapStorageMgrJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosBitmapStorageMgr\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_bitmap_storage_mgr\">"
		<< "<versions>"
		<< 		"<ver_1>libBitmapEngine.so</ver_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosBitmapStorageMgrObjPtr 
AosBitmapStorageMgrObj::getObject()
{
	if (smObject) return smObject;

	sgLock.lock();
	if (smObject)
	{
		sgLock.unlock();
		return smObject;
	}

	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	if (!sgJimoDoc)
	{
		if (!AosCreateBitmapStorageMgrJimoDoc(rdata))
		{
			sgLock.unlock();
			OmnAlarm << "indexmgrobj_internal_error" << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			OmnAlarm << "indexmgrobj_internal_error" << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sgJimoDoc);
	if (!jimo)
	{
		OmnAlarm << "indexmgrobj_internal_error" << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eBitmapStorageMgr)
	{
		OmnAlarm << "indexmgrobj_invalid_jimo" << enderr;
		sgLock.unlock();
		return 0;
	}

	smObject = dynamic_cast<AosBitmapStorageMgrObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smObject)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}

	return smObject;
}


