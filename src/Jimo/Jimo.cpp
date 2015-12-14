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
// 09/29/2012 Created by Chen Ding
// 2013/05/29 Renamed to AosJimo by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/Jimo.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"



AosJimo::AosJimo()
:
mJimoType(0),
mJimoVersion(-1)
{

}

AosJimo::AosJimo(
		const u32 type,
		const int version)
:
mJimoType(type),
mJimoVersion(version)
{
	mJimoType = type;
}


AosJimo::~AosJimo()
{
}


bool
AosJimo::serializeFrom(
		const AosRundataPtr &rdata, 
		AosBuff *buff)
{
	aos_assert_rr(buff, rdata, false);
	mJimoType = buff->getU32(0);
	mJimoVersion = buff->getInt(0);
	return true;
}


bool
AosJimo::serializeTo(
		const AosRundataPtr &rdata, 
		AosBuff *buff)
{
	aos_assert_rr(buff, rdata, false);
	buff->setU32(mJimoType);
	buff->setInt(mJimoVersion);
	return true;
}


bool 
AosJimo::config(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


bool 
AosJimo::queryInterface(
		const AosRundataPtr &rdata, 
		const char *interface_objid) const
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosJimo::registerMethod(
		const char *methodname, 
		AosJimoUtil::funcmap_t &map,
		const AosMethodId::E method_id,
		void *func)
{
	aos_assert_r(methodname, false);
	aos_assert_r(AosMethodId::isValid(method_id), false);
	map[methodname] = AosMethodDef(method_id, func, false);
	return true;
}


void *
AosJimo::getMethod(
		const AosRundataPtr &rdata, 
		const char *name, 
		AosMethodId::E &method_id)
{
	OmnShouldNeverComeHere;
	return 0;
}


void *
AosJimo::getMethod(
		const AosRundataPtr &rdata, 
		const char *name, 
		AosJimoUtil::funcmap_t &map, 
		AosMethodId::E &method_id)
{
	AosJimoUtil::funcmapitr_t itr = map.find(name);
	if (itr == map.end()) return 0;
	method_id = itr->second.method_id;
	return itr->second.func;
}


bool 
AosJimo::run(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker)
{
	AosXmlTagPtr jimo_doc;
	if (!config(rdata, worker, jimo_doc)) return false;
	return run(rdata);
}


bool 
AosJimo::run(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false; 
}


