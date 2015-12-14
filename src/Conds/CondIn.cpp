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
//
// Modification History:
// 12/24/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondIn.h"

#include "DocSelector/DocSelector.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondIn::AosCondIn(const bool reg)
:
AosCondition(AOSCONDTYPE_IN, AosCondType::eIn, reg)
{
}


AosCondIn::~AosCondIn()
{
}


bool 
AosCondIn::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// 	<cond type="in">
	// 		<zky_valueselector .../>
	// 		<zky_valueset .../>
	// 	</cond>
	
	if (!def)
	{
		rdata->setError() << "Missing def";
		return false;
	}

	// 1. Select value
	AosXmlTagPtr value_selector = def->getFirstChild(AOSTAG_VALUESELECTOR);
	if (!value_selector)
	{
		AosSetError(rdata, AosErrmsgId::eCondSyntaxError);
		OmnAlarm << rdata->getErrmsg() << def->toString() << enderr;
		return false;
	}

	AosValueRslt value;
	if (!AosValueSel::getValueStatic(value, value_selector, rdata))
	{
		AosSetError(rdata, AosErrmsgId::eCondSyntaxError);
		OmnAlarm << rdata->getErrmsg() << def->toString() << enderr;
		return false;
	}

	// 2. Evaluate the valueset
	AosXmlTagPtr valueset = def->getFirstChild(AOSTAG_VALUESET);
	if (!valueset)
	{
		rdata->setError() << "Missing valueset";
		return false;
	}

	if (!AosValueSel::getValueStatic(value, valueset, rdata))
	{
		return false;
	}

	rdata->setOk();
	return true;
}


AosConditionObjPtr
AosCondIn::clone()
{
	try
	{
		return OmnNew AosCondIn(false);
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
AosCondIn::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	/*
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_IN, rule);
	str << ">";

	if (level > 0)
	{
		int num = rand()%5;
		for (int i=0; i<num ;i++)
		{
			str << "<attr>";
	
			OmnString s1 = "valueselecotr";
			OmnString s2 = "valueset";

			s1 << i;
			str << AosValueRandUtil::pickValueSel("valuesel", level-1, rule, rdata);

			// Chen Ding, 01/17/2012
			// mChildRules.insert(make_pair(s1, childrule));

			s2 << i;
			str << AosValueRandUtil::pickValueSel("valuesel", level-1, rule, rdata);

			// Chen Ding, 01/17/2012
			// mChildRules.insert(make_pair(s2, childrule));
			str << "</attr>";
		}
	}
	str << ">";
	addThenElse(str, then_part, else_part, tname);
	return str;
	*/
	// Commented out by Chen Ding, 06/01/2012
	OmnNotImplementedYet;
	return "";
}


OmnString
AosCondIn::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

