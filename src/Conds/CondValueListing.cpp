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
// This condition tests whether a user is in the user list specified
// by this condition.
// 	<cond type="xxx"
// 		AOSTAG_USERSOURCE=AOSVALUE_REQUESTER|AOSVALUE_ACCESSED|AOSVALUE_VALUE_SEL
// 		AOSTAG_SEPARATOR="xxx">
// 		<AOSTAG_USRLSTCOND_VALTAG .../>   The value selector that selects users
// 		<AOSTAG_USRLSTCOND_USRTAG .../>   The value selector that selects user 
// 	</cond>
// 		
//
// Modification History:
// 01/10/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondValueListing.h"

#include "DocSelector/DocSelector.h"
#include "Random/RandomUtil.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondValueListing::AosCondValueListing(const bool reg)
:
AosCondition(AOSCONDTYPE_USERLISTING, AosCondType::eUserListing, reg)
{
}


AosCondValueListing::~AosCondValueListing()
{
}


bool 
AosCondValueListing::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	if (!def)
	{
		AosSetError(rdata, "missing_condition_def");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Check user listing type
	OmnString sep = def->getAttrStr(AOSTAG_SEPARATOR, ",");

	// It is list username. 
	OmnString value;
	bool rslt = getValueThrSel(value, def, rdata);
	if (!rslt) return false;

	OmnString values;
	AosValueRslt vv;
	rslt = AosValueSel::getValueStatic(vv, def, AOSTAG_USRLSTCOND_VALTAG, rdata);
	if (!rslt) return false;
	values = vv.getStr();
	if (!rslt) return false;
	return values.hasCommonWords(value, sep);
}


AosConditionObjPtr
AosCondValueListing::clone()
{
	try
	{
		return OmnNew AosCondValueListing(false);
	}

	catch (...)
	{
		OmnAlarm << "internal_error" << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}


OmnString 
AosCondValueListing::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part,
		const OmnString &else_part,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	/*
	// 	<cond type="xxx"
	// 		AOSTAG_SEPARATOR="xxx">
	// 		<AOSTAG_USRLSTCOND_VALTAG .../>   The value selector that selects users
	// 		<AOSTAG_USRLSTCOND_USRTAG .../>   The value selector that selects user 
	// 	</cond>
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_USERLISTING, rule);

	switch (rule->percent(AOSTAG_SEPARATOR, 4))
	{
	case 0:
		 str << "\" " << AOSTAG_SEPARATOR << "=\"#";
		 break;

	case 1:
		 str << "\" " << AOSTAG_SEPARATOR << "=\"$";
		 break;

	case 2:
		 str << "\" " << AOSTAG_SEPARATOR << "=\",";
		 break;

	default:
		 str << "\"";
		 break;
	}
	str << ">";

	// Create the selector for the values
	OmnString vv = AosValueRandUtil::pickMultiValuesSelector(
			AOSTAG_USRLSTCOND_VALTAG, level-1, rule, rdata);
	aos_assert_r(vv != "", "");
	str << vv;

	OmnString dft_value = OmnRandom::word30();

	// Create the user selector
	vv = AosValueRandUtil::pickSingleValueSelector(
			AOSTAG_USRLSTCOND_USRTAG, level-1, dft_value, rule, rdata);
	aos_assert_r(vv != "", "");
	str << vv;
	addThenElse(str, then_part, else_part, tname);
	return str; 
	*/
	// Commented out by Chen Ding, 06/01/2012
	OmnNotImplementedYet;
	return "";
}


bool
AosCondValueListing::getValueThrSel(
		OmnString &value, 
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	aos_assert_rr(def, rdata, false);

	// It is through a value selector. The selector's name is 
	// in the tag.
	AosXmlTagPtr tag = def->getFirstChild(AOSTAG_USRLSTCOND_USRTAG);
	if (!tag)
	{
		AosSetError(rdata, "condition_error_002") << AOSTAG_USRLSTCOND_USRTAG;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	AosValueRslt vv;
	bool rslt = AosValueSel::getValueStatic(vv, tag, rdata);
	if (!rslt) return false;
	value = vv.getStr();
	return rslt;
}


OmnString
AosCondValueListing::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


