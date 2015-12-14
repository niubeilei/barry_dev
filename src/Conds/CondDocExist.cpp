////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This condition tests requester user roles to be one of the user
// groups specified by this condition.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondDocExist.h"

#include "DocSelector/DocSelector.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondDocExist::AosCondDocExist(const bool reg)
:
AosCondition(AOSCONDTYPE_DOCEXIST, AosCondType::eDocExist, reg)
{
}


AosCondDocExist::~AosCondDocExist()
{
}


bool 
AosCondDocExist::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!def)
	{
		AosSetErrorUser(rdata, "cond_failed_001") << enderr;
		return false;
	}

	AosXmlTagPtr tt = def->getFirstChild(AOSTAG_VALUE_SELECTOR);
	if (!tt)
	{
		AosSetErrorUser(rdata, "cond_failed_002") << def->toString() << enderr;
		return false;
	}
	
	AosValueSelType::E type = AosValueSelType::toEnum(tt->getAttrStr(AOSTAG_VALUE_TYPE));
	if(type != AosValueSelType::eQuery)
	{
		AosSetErrorUser(rdata, "cond_failed_003") << def->toString() << enderr;
		return false;
	}

	AosValueRslt value;
	if (!AosValueSel::getValueStatic(value, tt, rdata))
	{
		AosSetErrorUser(rdata, "cond_failed_004") << def->toString() << enderr;
		return false;
	}
	
	//AosQueryRsltObjPtr rslt = value.getQueryRslt(rdata.getPtr());
	AosQueryRsltObjPtr rslt;
	if(!rslt)
	{
		AosSetErrorUser(rdata, "cond_failed_005") << def->toString() << enderr;
		return false;
	}
	
	int num = rslt->getNumDocs();
	bool needExist = def->getAttrBool(AOSTAG_NEEDEXIST, true);
	if(needExist)
	{
		if(num > 0) return true;
		return false;
	}
	else
	{
		if(num > 0) return false;
		return true;
	}
	
	return true;
}


AosConditionObjPtr
AosCondDocExist::clone()
{
	try
	{
		return OmnNew AosCondDocExist(false);
	}

	catch (...)
	{
		OmnAlarm << "Failed to clone cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondDocExist::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_DOCEXIST, rule);
	str << ">";
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondDocExist::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


