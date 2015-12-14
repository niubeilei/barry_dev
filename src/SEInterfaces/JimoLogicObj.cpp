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
#include "SEInterfaces/JimoLogicObj.h"

#include "API/AosApi.h"


AosJimoLogicObjPtr AosJimoLogicObj::smJimoLogicObj;

static AosXmlTagPtr			sgJimoDoc;
static OmnMutex				sgLock;


AosJimoLogicObj::AosJimoLogicObj(const int version)
:
AosJimo(AosJimoType::eJimoLogic, version)
{
}


AosJimoLogicObj::~AosJimoLogicObj()
{
}


bool 
AosJimoLogicObj::createJimoDocStatic(AosRundata *rdata)
{
	if (sgJimoDoc) return true;
	OmnString jimostr = "<jimo ";
	jimostr << AOSTAG_CURRENT_VERSION << "=\"1\" "
		<< "zky_classname=\"AosJimoLogic\" "
		<< "zky_otype=\"" << AOSOTYPE_JIMO << "\" "
		<< "zky_objid=\"data_logic_jimo\">"
		<< "<versions>"
		<< 		"<version_1>libJimoLogicJimos.so</version_1>"
		<< "</versions>"
		<< "</jimo>";

	sgJimoDoc = AosStr2Xml(rdata, jimostr AosMemoryCheckerArgs);
	aos_assert_r(sgJimoDoc, 0);
	return sgJimoDoc;
}


AosJimoLogicObjPtr 
AosJimoLogicObj::getJimoLogicStatic(AosRundata *rdata)
{
	if (smJimoLogicObj) return smJimoLogicObj;

	sgLock.lock();
	if (smJimoLogicObj)
	{
		sgLock.unlock();
		return smJimoLogicObj;
	}

	if (!sgJimoDoc)
	{
		if (!createJimoDocStatic(rdata))
		{
			sgLock.unlock();
			AosSetError(rdata, "datalogicobj_internal_error") << enderr;
			return 0;
		}

		if (!sgJimoDoc)
		{
			AosSetError(rdata, "datalogicobj_internal_error") << enderr;
			sgLock.unlock();
			return 0;
		}
	}

	AosJimoPtr jimo = AosCreateJimo(rdata, sgJimoDoc);
	if (!jimo)
	{
		AosSetError(rdata, "datalogicobj_internal_error") << enderr;
		sgLock.unlock();
		return 0;
	}

	if (jimo->getJimoType() != AosJimoType::eJimoLogic)
	{
		AosSetErrorUser(rdata, "datalogicbj_invalid_jimo") << enderr;
		sgLock.unlock();
		return 0;
	}

	smJimoLogicObj = dynamic_cast<AosJimoLogicObj*>(jimo.getPtr());
	sgLock.unlock();
	if (!smJimoLogicObj)
	{
		AosSetError(rdata, "datalogicobj_internal_error") << enderr;
		return 0;
	}

	return smJimoLogicObj;
}


AosJimoLogicObjPtr
AosJimoLogicObj::createJimoLogicStatic(
		const AosXmlTagPtr &def, 
		AosRundata *rdata)
{
	if (!smJimoLogicObj) getJimoLogicStatic(rdata);
	aos_assert_rr(smJimoLogicObj, rdata, 0);

	return smJimoLogicObj->createJimoLogic(def, rdata);
}

