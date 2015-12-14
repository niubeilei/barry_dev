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
// This condition tests requester is in the user domains specified by this condition.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondUserDomains.h"

#include "DocSelector/DocSelector.h"
#include "Random/CommonValues.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondUserDomains::AosCondUserDomains(const bool reg)
:
AosCondition(AOSCONDTYPE_USERDOMAINS, AosCondType::eUserDomains, reg)
{
}


AosCondUserDomains::~AosCondUserDomains()
{
}


bool 
AosCondUserDomains::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This condition tests requester user groups to be one of the user
	// groups specified by this condition.
	// 	<cond type=AOSCONDITYPE_USERDOMAIN>domain,...</cond>
	
	if (!def)
	{
		AosSetError(rdata, "missing_condition_def");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString requester_domains = rdata->getRequesterDomains();
	if (requester_domains == "") return false;

	// Retrieve the domains
	OmnString domains = def->getNodeText();
	if (domains == "")
	{
		AosSetError(rdata, "missing_domains_in_cond:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool negate = def->getAttrBool(AOSTAG_NEGATE, false);
	if (negate) return !domains.hasCommonWords(requester_domains, ",");
	return domains.hasCommonWords(requester_domains, ",");
}


AosConditionObjPtr
AosCondUserDomains::clone()
{
	try
	{
		return OmnNew AosCondUserDomains(false);
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
AosCondUserDomains::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part, 
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_USERDOMAINS, rule);
	str << ">" << AosCommonValues::pickUserDomains(",");
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondUserDomains::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}




