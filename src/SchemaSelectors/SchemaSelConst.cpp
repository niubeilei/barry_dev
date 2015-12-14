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
// 2013/07/29 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SchemaSelectors/SchemaSelConst.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SchemaSelectors/SchemaTypeNames.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosSchemaSelConst_0(
 		const AosRundataPtr &rdata, 
		const u32 version) 
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		return OmnNew AosSchemaSelConst(rdata, version);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosSchemaSelConst::AosSchemaSelConst(
		const AosRundataPtr &rdata,
		// const AosXmlTagPtr &jimo_doc, 	// Chen Ding, 2014/01/12
		const u32 version)
:
AosSchemaSelector(rdata, jimo_doc, "schema_sel_const", version, 
		AOS_SCHEMA_SELECTOR_TYPENAME_CONST)
{
}


AosSchemaSelConst::AosSchemaSelConst(const AosSchemaSelConst &rhs)
:
AosSchemaSelector(rhs)
{
}


AosSchemaSelConst::~AosSchemaSelConst()
{
}


bool
AosSchemaSelConst::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc)
{
	return true;
}


bool 
AosSchemaSelConst::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosSchemaSelector::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosSchemaSelConst::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosSchemaSelector::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosSchemaSelConst::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return true;
}


bool
AosSchemaSelConst::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	// registerMethod("next_value", sgFuncMap, 
	// 		AosMethodId::eAosRundata_Jimo_WorkerDoc,
	// 		(void *)AosSchemaSelConst_nextValue);

	sgInited = true;
	sgLock.unlock();
	return true;
}


OmnString
AosSchemaSelConst::toString() const
{
	return "";
}


AosJimoPtr 
AosSchemaSelConst::cloneJimo(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)  const
{
	try
	{
		return OmnNew AosSchemaSelConst(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating jimo" << enderr;
		return 0;
	}
}


bool 
AosSchemaSelConst::run(const AosRundataPtr &rdata) 
{
	OmnNotImplementedYet;
	return false;
}


void * 
AosSchemaSelConst::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}

