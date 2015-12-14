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
// This condition tests the requester belongs to one of the user
// roles specified by this condition.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondUserRoles.h"

#include "DocSelector/DocSelector.h"
#include "Random/CommonValues.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondUserRoles::AosCondUserRoles(const bool reg)
:
AosCondition(AOSCONDTYPE_USERROLES, AosCondType::eUserRoles, reg)
{
}


AosCondUserRoles::~AosCondUserRoles()
{
}


bool 
AosCondUserRoles::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This condition tests requester user groups to be one of the user
	// groups specified by this condition.
	// 	<cond type=AOSCONDITYPE_USERROLES>role,role,...</cond>
	
	if (!def)
	{
		AosSetError(rdata, "missing_condition_def");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString requester_roles = rdata->getRequesterUserRoles();
	if (requester_roles == "") return false;

	// Retrieve the groups
	OmnString roles = def->getNodeText();
	if (roles == "")
	{
		AosSetError(rdata, "missing_roles_in_cond:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool negate = def->getAttrBool(AOSTAG_NEGATE, false);
	if (negate) return !roles.hasCommonWords(requester_roles, ",");
	return roles.hasCommonWords(requester_roles, ",");
}


AosConditionObjPtr
AosCondUserRoles::clone()
{
	try
	{
		return OmnNew AosCondUserRoles(false);
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
AosCondUserRoles::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part, 
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_USERROLES, rule);
	str << ">";
	OmnString userroles = AosCommonValues::pickUserRoles(",");
	str << userroles;
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondUserRoles::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}

