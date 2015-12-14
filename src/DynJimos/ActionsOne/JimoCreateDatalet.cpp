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
// 2013/06/10 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DynJimos/ActionsOne/JimoCreateDatalet.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <string>

static AosJimo::funcmap_t sgFuncMap;
static bool sgInited = false;
static OmnMutex sgLock;

AosJimoCreateDatalet::AosJimoCreateDatalet(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
:
AosJimo(rdata, worker_doc, jimo_doc)
{
	if (!init(rdata, worker_doc, jimo_doc))
	{
		OmnThrowException(rdata->getErrmsg());
	}
}


bool
AosJimoCreateDatalet::init(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	if (!sgInited) registerMethods();
	return config(rdata, worker_doc, jimo_doc);
}


bool
AosJimoCreateDatalet::registerMethods()
{
	sgLock.lock();
	if (sgInited)
	{
		sgLock.unlock();
		return true;
	}

	sgInited = true;
	sgLock.unlock();
	return true;
}


AosJimoCreateDatalet::AosJimoCreateDatalet(const AosJimoCreateDatalet &rhs)
:
AosJimo(rhs)
{

}

AosJimoCreateDatalet::~AosJimoCreateDatalet()
{
}

bool
AosJimoCreateDatalet::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc)
{
	return true;
}


OmnString
AosJimoCreateDatalet::toString() const
{
	return "";
}


bool 
AosJimoCreateDatalet::serializeTo(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	bool rslt = AosJimo::serializeTo(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


bool 
AosJimoCreateDatalet::serializeFrom(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	bool rslt = AosJimo::serializeFrom(rdata, buff);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoCreateDatalet::clone(const AosRundataPtr &rdata) const
{
	try
	{
		return OmnNew AosJimoCreateDatalet(*this);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_cloning_jimo") << toString() << enderr;
		return 0;
	}
}


bool 
AosJimoCreateDatalet::run(		
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &worker_doc)
{
	/*
	// This function creates a datalet. The command is defined in 'worker_doc':
	// 	<command 
	// 		...>
	// 		<objdef>
	// 			...
	// 		</objdef>
	// 		<setters>
	// 			<setter .../>
	// 			<setter .../>
	// 			...
	// 		</setters>
	// 	</command>
	aos_assert_rr(worker_doc, rdata, false);

	AosXmlTagPtr obj = worker_doc->getFirstChild(AOSTAG_OBJDEF);
	if (!obj)
	{
		AosSetErrorU(rdata, "missing_object_to_create") 
			<< worker_doc->toString() << enderr;
		return false;
	}

	obj = obj->getFirstChild();
	if (!obj)
	{
		AosSetErrorU(rdata, "object_is_empty") 
			<< worker_doc->toString() << enderr;
		return false;
	}

	AosXmlTagPtr setters_tag = worker_doc->getFirstChild(AOSTAG_SETTERS);
	if (setters_tag)
	{
		rdata->setWorkingDoc(obj);
		bool rslt = AosRunSetters(rdata, setters_tag);
		if (!rslt) return false;
	}
	
	*/
	OmnNotImplementedYet;
	return false;
}


void * 
AosJimoCreateDatalet::getMethod(
		const AosRundataPtr &rdata, 
		const OmnString &name, 
		AosMethodId::E &method_id)
{
	return AosJimo::getMethod(rdata, name, sgFuncMap, method_id);
}


