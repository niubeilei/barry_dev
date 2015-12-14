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
// 2015/03/25 Created by Arvin Jiang
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/JimoParserObj.h"

#include "API/AosApi.h"


//AosJimoParserObj *AosJimoParserObj::smJimoParserObj;

//static AosXmlTagPtr			sgJimoDoc;
//static OmnMutex				sgLock;


AosJimoParserObj::AosJimoParserObj()
{
}

AosJimoParserObj::AosJimoParserObj(const int version)
:
AosJimo(AosJimoType::eJimoParser, version)
{
	mJimoType = AosJimoType::eJimoParser;
}

/*
bool 
AosJimoParserObj::createJimoDocStatic(AosRundata *rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosJimoParser\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"jimo_parser_jimo\">"
		<< "<versions>"
		<< 		"<version_1>libJimoParserJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosJimoParserObj *
AosJimoParserObj::getJimoParserStatic(AosRundata *rdata)
{
	if (smJimoParserObj) return smJimoParserObj;

	sgLock.lock();
	if (smJimoParserObj)
	{
		sgLock.unlock();
		return smJimoParserObj;
	}

	if (!sgJimoDoc)
	{
		if (!createJimoDocStatic(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "JimoParserobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "JimoParserobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "JimoParserobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eJimoParser)
	{
		AosSetErrorUser(rdata, "JimoParserbj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smJimoParserObj = dynamic_cast<AosJimoParserObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smJimoParserObj)
	{
		AosSetError(rdata, "JimoParserobj_internal_error") << enderr;
		return 0;
	}

	return smJimoParserObj;
}


AosJimoParserObjPtr
AosJimoParserObj::createJimoParserStatic(
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	if (!smJimoParserObj) getJimoParserStatic(rdata);
	aos_assert_rr(smJimoParserObj, rdata, 0);

	return smJimoParserObj->createJimoParser(def, rdata);
}
*/
