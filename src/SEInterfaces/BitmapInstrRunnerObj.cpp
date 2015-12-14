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
// 2013/08/30 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapInstrRunnerObj.h"

#include "API/AosApiS.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/Ptrs.h"
#include "Rundata/Rundata.h"


static AosXmlTagPtr		sgJimoDoc;
static OmnMutex			sgLock;


AosBitmapInstrRunnerObj::AosBitmapInstrRunnerObj(const int version)
:
AosJimo(AosJimoType::eBitmapInstrRunner, version)
{
}


AosBitmapInstrRunnerObj::~AosBitmapInstrRunnerObj()
{
}

bool 
AosBitmapInstrRunnerObj::addTransStatic(const AosTransSendInstrsPtr &trans)
{
	static AosBitmapInstrRunnerObjPtr lsRunner;
	static OmnMutex lsLock;

	lsLock.lock();
	if (!lsRunner)
	{
		// create jimo
		AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
		if (!sgJimoDoc)
		{
			if (!AosCreateBitmapInstrRunnerJimoDoc(rdata))
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
	
		if (jimo->getJimoType() != AosJimoType::eBitmapInstrRunner)
		{
			OmnAlarm << "bitmapengineobj_invalid_jimo" << enderr;
			sgLock.unlock();
			return 0;
		}
	
		lsRunner = dynamic_cast<AosBitmapInstrRunnerObj*>(jimo.getPtr());
		
	}
	lsLock.unlock();

	return lsRunner->addTrans(trans);
}


bool 
AosBitmapInstrRunnerObj::AosCreateBitmapInstrRunnerJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosBitmapInstrRunner\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_bitmap_instr_runner\">"
		<< "<versions>"
		<< 		"<version_1>libBitmapEngine.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}
