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
// This condition always returns true.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondTrue.h"

#include "DocSelector/DocSelector.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondTrue::AosCondTrue(const bool reg)
:
AosCondition(AOSCONDTYPE_TRUE, AosCondType::eTrue, reg)
{
}


AosCondTrue::~AosCondTrue()
{
}


bool 
AosCondTrue::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	return true;
}

	
bool
AosCondTrue::evalCond(const AosValueRslt &value, const AosRundataPtr &rdata)
{
	bool isTrue = value.getBool();
	return isTrue == true;
}


AosConditionObjPtr
AosCondTrue::clone()
{
	try
	{
		return OmnNew AosCondTrue(false);
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to clone cond" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondTrue::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part, 
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_TRUE, rule);
	str << ">";
	addThenElse(str, then_part, else_part, tname);
	return str; 
}


OmnString
AosCondTrue::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}



