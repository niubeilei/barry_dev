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
#include "SEInterfaces/JQLParserObj.h"

#include "API/AosApi.h"


AosJQLParserObjPtr AosJQLParserObj::smJQLParserObj;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


AosJQLParserObj::AosJQLParserObj(const int version)
:
AosJimo(AosJimoType::eJQLParser, version)
{
}


AosJQLParserObj::~AosJQLParserObj()
{
}


bool 
AosJQLParserObj::createJimoDocStatic(AosRundata *rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosJQLParserWrapper\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"jql_parser_jimo\">"
		<< "<versions>"
		<< 		"<version_1>libJQLParserJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosJQLParserObj*
AosJQLParserObj::getJQLParserStatic(AosRundata *rdata)
{
	if (smJQLParserObj) return smJQLParserObj.getPtrNoLock();

	sgLock.lock();
	if (smJQLParserObj)
	{
		sgLock.unlock();
		return smJQLParserObj.getPtrNoLock();
	}

	if (!sgJimoDoc)
	{
		if (!createJimoDocStatic(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "jqlparserobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "jqlparserobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "jqlparserobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eJQLParser)
	{
		AosSetErrorUser(rdata, "jqlparserbj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smJQLParserObj = dynamic_cast<AosJQLParserObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smJQLParserObj)
	{
		AosSetError(rdata, "jqlparserobj_internal_error") << enderr;
		return 0;
	}

	return smJQLParserObj.getPtrNoLock();
}
