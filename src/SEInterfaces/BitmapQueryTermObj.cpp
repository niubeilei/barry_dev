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
// 2014/01/28 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/BitmapQueryTermObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Thread/Mutex.h"



AosBitmapQueryTermObjPtr AosBitmapQueryTermObj::smObject;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;



AosBitmapQueryTermObj::AosBitmapQueryTermObj(const int version)
:
AosJimo(AosJimoType::eBitmapQueryTerm, version)
{
}


static bool AosCreateBitmapQueryTermJimoDoc(const AosRundataPtr &rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosBitmapQueryTermOr\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"" << AosObjid::composeJimoDocObjid("AosBitmapQueryTermOr") << "\">"
		<< "<versions>"
		<< 		"<version_1>libQueryEngine.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata.getPtr(), jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosBitmapQueryTermObjPtr 
AosBitmapQueryTermObj::getBitmapQueryTermObj(const AosRundataPtr &rdata)
{
	if (smObject) return smObject;

	sgLock.lock();
	if (smObject)
	{
		sgLock.unlock();
		return smObject;
	}

	if (!sgJimoDoc)
	{
		if (!AosCreateBitmapQueryTermJimoDoc(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata.getPtr(), sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "indexmgrobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eBitmapQueryTerm)
	{
		AosSetErrorUser(rdata, "indexmgrobj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smObject = dynamic_cast<AosBitmapQueryTermObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smObject)
	{
		AosSetError(rdata, "internal_error") << enderr;
		return 0;
	}

	return smObject;
}


AosBitmapQueryTermObjPtr 
AosBitmapQueryTermObj::createTermStatic( 
		const AosRundataPtr &rdata,
		const AosQueryCallerPtr &query_req, 
		const AosQueryTermObjPtr &term,
		const AosQueryReqObjPtr &req, 
		const bool check_paral)
{
	if (!smObject) getBitmapQueryTermObj(rdata);
	aos_assert_rr(smObject, rdata, 0);
	return smObject->createTerm(rdata, query_req, term, req, check_paral);
}

