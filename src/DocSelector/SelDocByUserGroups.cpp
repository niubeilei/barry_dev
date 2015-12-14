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
// 01/04/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelDocByUserGroups.h"

#include "Rundata/Rundata.h"
#include "Util/String.h"
#include "ValueSel/ValueSel.h"
#include "ValueSel/ValueRslt.h"

AosDocSelDocByUserGroups::AosDocSelDocByUserGroups(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_DOC_BY_USER_GROUPS, AosDocSelectorType::eDocByUserGroups, reg)
{
}


AosDocSelDocByUserGroups::~AosDocSelDocByUserGroups()
{
}

AosXmlTagPtr 
AosDocSelDocByUserGroups::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This selector selects one doc based on user groups. If the requester is 
	// in one of the user groups specified in 'sdoc', it composes a string. It
	// then uses the string as the input to generate an objid.
	AOSLOG_ENTER_R(rdata, 0);
	if (!sdoc)
	{
		AosSetErrorUser(rdata, "missing_sdoc") << enderr;
		return 0;
	}

	// 1. Retrieve the user groups. 
	AosValueRslt value;
	bool rslt = AosValueSel::getValueStatic(value, sdoc, AOSTAG_USER_GROUPS, rdata);
	aos_assert_rr(rslt, rdata, 0);

	OmnString usergroups = value.getStr();
	if (usergroups == "" || !rslt)
	{
		AosSetError(rdata, "missing_usergroups") << ": " << sdoc->getAttrStr(AOSTAG_OBJID);
		return 0;
	}

	OmnString requester_groups = rdata->getRequesterUserGroups();
	if (requester_groups == "") 
	{
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	OmnString sep = sdoc->getAttrStr(AOSTAG_ENTRY_SEP, ",");
	if (sep == "") sep = ",";

	// Check whether the requester is in one of the groups
	if (!usergroups.hasCommonWords(requester_groups, sep)) 
	{
		// it is not
		AOSLOG_LEAVE(rdata);
		return 0;
	}

	// The requester is in one of the groups. 
	OmnNotImplementedYet;
	AOSLOG_LEAVE(rdata);
	return 0;
}


OmnString 
AosDocSelDocByUserGroups::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


