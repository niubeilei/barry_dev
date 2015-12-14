////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2012/11/12 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IILScanner/IILScanValue.h"

#include "IILScanner/IILScanValueAll.h"

static AosIILScanValuePtr	sgScanValues[AosIILScanValue::eInvalid];
static OmnMutex 			sgLock;
static OmnMutex 			sgInitLock;
static bool					sgInited = false;

	
AosIILScanValue::AosIILScanValue(
		const E type,
		const bool regist_flag)
:
mType(type)
{
	if(regist_flag) registerScanValue(type, this);	
}


AosIILScanValue::~AosIILScanValue()
{
}


bool
AosIILScanValue::registerScanValue(
		const AosIILScanValue::E type,
		AosIILScanValue * scan)
{
	sgLock.lock();
	if(sgScanValues[type])
	{
		OmnAlarm << "already regist, " << type << enderr;
		sgLock.unlock();
		return false;
	}
	sgScanValues[type] = scan;
	sgScanValues[type].setDelFlag(false);
	sgLock.unlock();
	return true;
}


void
AosIILScanValue::init()
{
	sgInitLock.lock();
	if(sgInited) 
	{
		sgInitLock.unlock();
		return;
	}
	
	static AosIILScanValueAll	sgAll(true);

	sgInited = true;
	sgInitLock.unlock();
}


AosIILScanValuePtr
AosIILScanValue::createIILScanValue(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	if(!sgInited) init();

	aos_assert_r(def, 0);
	AosIILScanValue::E type = to_enum(def->getAttrStr(AOSTAG_ZKY_TYPE));
	if(!sgScanValues[type])
	{
		OmnAlarm << "scanvalue is not inited:" << type << enderr;
		return 0;
	}

	AosIILScanValuePtr scan = sgScanValues[type];
	aos_assert_r(scan, 0);

	return scan->clone(def, rdata);
}


AosIILScanValue::E
AosIILScanValue::to_enum(const OmnString &type)
{
	if(type == AOSIILSCANVALUE_ALL) return eAll;
	if(type == AOSIILSCANVALUE_AVG) return eAvg;
	if(type == AOSIILSCANVALUE_FIRST) return eFirst;
	if(type == AOSIILSCANVALUE_LAST) return eLast;
	if(type == AOSIILSCANVALUE_MAX) return eMax;
	if(type == AOSIILSCANVALUE_MIN) return eMin;
	if(type == AOSIILSCANVALUE_SUM) return eSum;
	return eInvalid;
}


OmnString
AosIILScanValue::to_string(const AosIILScanValue::E type)
{
	switch(type)
	{
	case eAll : return AOSIILSCANVALUE_ALL;
	case eAvg : return AOSIILSCANVALUE_AVG;
	case eFirst : return AOSIILSCANVALUE_FIRST;
	case eLast : return AOSIILSCANVALUE_LAST;
	case eMax : return AOSIILSCANVALUE_MAX;
	case eMin : return AOSIILSCANVALUE_MIN;
	case eSum : return AOSIILSCANVALUE_SUM;
	default : break;
	}
	return AOSIILSCANVALUE_INVALID;
}

	
