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
//
// Modification History:
// 2013/02/08 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapEngineObj.h"

#include "API/AosApi.h"
AosBitmapEngineObjPtr	AosBitmapEngineObj::smObject;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


int
AosBitmapEngineObj::getCubeId(const u64 &section_id)
{
	// For the time being, 'section_id' to 'physical_id' is mapped
	// by taking the remainder. In the future, we may want to 
	// let users determine.
	int num_cubes = AosGetNumCubes();
	aos_assert_r(num_cubes > 0, -1);
	return section_id % num_cubes;
}



AosBitmapEngineObj::AosBitmapEngineObj(const int version)
:
AosJimo(AosJimoType::eBitmapEngine, version)
{
}


static bool AosCreateBitmapEngineJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosBitmapEngine\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_bitmap_engine\">"
		<< "<versions>"
		<< 		"<version_1>libBitmapEngine.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosBitmapEngineObjPtr 
AosBitmapEngineObj::getBitmapEngine()
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
		if (!AosCreateBitmapEngineJimoDoc(rdata))
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

	if (jimo->getJimoType() != AosJimoType::eBitmapEngine)
	{
		OmnAlarm << "bitmapengineobj_invalid_jimo" << enderr;
		sgLock.unlock();
		return 0;
	}

	smObject = dynamic_cast<AosBitmapEngineObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smObject)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}

	return smObject;
}


