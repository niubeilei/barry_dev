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
#include "Conds/CondFalse.h"

#include "DocSelector/DocSelector.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondFalse::AosCondFalse(const bool reg)
:
AosCondition(AOSCONDTYPE_FALSE, AosCondType::eFalse, reg)
{
}


AosCondFalse::~AosCondFalse()
{
}


bool 
AosCondFalse::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return false;
}

	
bool
AosCondFalse::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	bool isFalse = value.getBool();
	return isFalse == false;
}


AosConditionObjPtr
AosCondFalse::clone()
{
	try
	{
		return OmnNew AosCondFalse(false);
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
AosCondFalse::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_FALSE, rule);
	str << ">";
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondFalse::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


