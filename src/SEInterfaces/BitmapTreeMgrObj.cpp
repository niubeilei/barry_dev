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
#include "SEInterfaces/BitmapTreeMgrObj.h"

#include "API/AosApi.h"
#include "Thread/Mutex.h"
#include "SEUtil/DocTags.h"



AosBitmapTreeMgrObjPtr AosBitmapTreeMgrObj::smObject;
static AosXmlTagPtr		sgJimoDoc;
static OmnMutex			sgLock;




AosBitmapTreeMgrObj::AosBitmapTreeMgrObj(const int version)
:
AosJimo(AosJimoType::eBitmapTreeMgr, version)
{
}


static bool AosCreateBitmapTreeMgrJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosBitmapTreeMgr\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_bitmap_tree_mgr\">"
		<< "<versions>"
		<< 		"<version_1>libBitmapTreeMgr.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosBitmapTreeMgrObjPtr 
AosBitmapTreeMgrObj::getObject()
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
		if (!AosCreateBitmapTreeMgrJimoDoc(rdata))
		{
			sgLock.unlock();
			OmnAlarm << "bitmapengineobj_internal_error" << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			OmnAlarm << "bitmapengineobj_internal_error" << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sgJimoDoc);
	if (!jimo)
	{
		OmnAlarm << "bitmapengineobj_internal_error" << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eBitmapTreeMgr)
	{
		OmnAlarm << "bitmapengineobj_invalid_jimo" << enderr;
		sgLock.unlock();
		return 0;
	}

	smObject = dynamic_cast<AosBitmapTreeMgrObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smObject)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}

	return smObject;
}


