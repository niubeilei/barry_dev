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
// 12/27/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelByUserArgs.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "MultiLang/LangTermIds.h"
#include "Util/DynArray.h"
#include "Util/OmnNew.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosValueSelByUserArgs::AosValueSelByUserArgs(const bool reg)
:
AosValueSel(AOSACTOPRID_BYUSERARGS, AosValueSelType::eByUserArgs, reg)
{
}


AosValueSelByUserArgs::AosValueSelByUserArgs(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_BYUSERARGS, AosValueSelType::eByUserArgs, false)
{
}


AosValueSelByUserArgs::~AosValueSelByUserArgs()
{
}


bool
AosValueSelByUserArgs::run(
		AosValueRslt &value,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	//	<name AOSTAG_VALUE_TYPE=AOSACTOPRID_BYARGS 
	//		AOSTAG_ARGNAME="xxx"
	//		AOSTAG_DATA_TYPE ="xxx"/>
	
	value.reset();
	aos_assert_r(sdoc, false);
	AosDataType::E datatype = AosDataType::toEnum(sdoc->getAttrStr(AOSTAG_DATA_TYPE));
	if (!AosDataType::isValid(datatype))
	{
		datatype = AosDataType::eString;
	}

	if (datatype == AosDataType::eXmlDoc)
	{
		rdata->setError() << "Datatype incorrect: " << datatype;
		return false;
	}

	OmnString name = sdoc->getAttrStr(AOSTAG_ARGNAME);
	if (name == "")
	{
		AosSetError(rdata, AOSLT_MISSING_ARG_NAME);
		OmnAlarm << rdata->getErrmsg() << ":" << sdoc->toString() << enderr;
		return false;
	}

	OmnString vv = rdata->getUserArg(name);
	value.setStr(vv);
	return true;
}


OmnString 
AosValueSelByUserArgs::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelByUserArgs::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelByUserArgs(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorUser(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}

