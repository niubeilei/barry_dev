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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/LogParserObj.h"

#include "API/AosApi.h"


AosLogParserObjPtr AosLogParserObj::smLogParserObj;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


AosLogParserObj::AosLogParserObj(const int version)
:
AosJimo(AosJimoType::eLogParser, version)
{
}


AosLogParserObj::~AosLogParserObj()
{
}


bool 
AosLogParserObj::createJimoDocStatic(AosRundata *rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosLogParser\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"_jimo_log_entry\">"
		<< "<versions>"
		<< 		"<version_1>libLogParserJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosLogParserObjPtr 
AosLogParserObj::getLogParserStatic(AosRundata *rdata)
{
	if (smLogParserObj) return smLogParserObj;

	sgLock.lock();
	if (smLogParserObj)
	{
		sgLock.unlock();
		return smLogParserObj;
	}

	if (!sgJimoDoc)
	{
		if (!createJimoDocStatic(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "logentryobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "logentryobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "logentryobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eLogParser)
	{
		AosSetErrorUser(rdata, "logentrybj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smLogParserObj = dynamic_cast<AosLogParserObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smLogParserObj)
	{
		AosSetError(rdata, "logentryobj_internal_error") << enderr;
		return 0;
	}

	return smLogParserObj;
}


AosLogParserObjPtr
AosLogParserObj::createLogParserStatic(
		const OmnString &type, 
		AosRundata *rdata)
{
	if (!smLogParserObj) getLogParserStatic(rdata);
	aos_assert_rr(smLogParserObj, rdata, 0);

	return smLogParserObj->createLogParser(type, rdata);
}

