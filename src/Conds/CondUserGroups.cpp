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
// This condition tests the requester belongs to one of the user groups
// specified by this condition.
//
// Modification History:
// 01/03/2012: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/CondUserGroups.h"

#include "DocSelector/DocSelector.h"
#include "Random/CommonValues.h"
#include "ValueSel/ValueSel.h"
#include "XmlUtil/XmlTag.h"


AosCondUserGroups::AosCondUserGroups(const bool reg)
:
AosCondition(AOSCONDTYPE_USERGROUPS, AosCondType::eUserGroups, reg)
{
}


AosCondUserGroups::~AosCondUserGroups()
{
}


bool 
AosCondUserGroups::evalCond(const AosXmlTagPtr &def, const AosRundataPtr &rdata)
{
	// This condition tests requester user groups to be one of the user
	// groups specified by this condition.
	// 	<cond type=AOSCONDITYPE_USERGROUP>group,group,...</cond>
	
	if (!def)
	{
		AosSetError(rdata, "missing_condition_def");
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	OmnString requester_groups = rdata->getRequesterUserGroups();
	if (requester_groups == "") return false;

	// Retrieve the groups
	OmnString groups = def->getNodeText();
	if (groups == "")
	{
		AosSetError(rdata, "missing_groups_in_cond:") << def->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool negate = def->getAttrBool(AOSTAG_NEGATE, false);
	if (negate) return !groups.hasCommonWords(requester_groups, ",");
	return groups.hasCommonWords(requester_groups, ",");
}


AosConditionObjPtr
AosCondUserGroups::clone()
{
	try
	{
		return OmnNew AosCondUserGroups(false);
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
AosCondUserGroups::getXmlStr(
		const OmnString &tagname,
		const int level,
		const OmnString &then_part, 
		const OmnString &else_part, 
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnString str = "<";
	OmnString tname = getTagname(tagname, rule);
	addHeader(str, tname, AOSCONDTYPE_USERGROUPS, rule);
	str << ">" << AosCommonValues::pickUserGroups(",");
	addThenElse(str, then_part, else_part, tname);
	return str;
}


OmnString
AosCondUserGroups::generateCommand(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}





