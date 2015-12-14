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
// 10/24/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessGroup.h"

#include "API/AosApiS.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessGroup::AosAccessGroup()
:
AosAccess(AOSACCESSTYPE_GROUP)
{
}


bool 
AosAccessGroup::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted, 
		bool &denied)
{
	// The accessed is an operation, and the operation access is defined
	// by 'tag', which is in the following form:
	// 	<access type="group"
	// 		AOSTAG_DENY_ACCESS="true|false">group,group,...</access>
	// This function checks whether the requester is in one of the groups
	// defined by 'tag'. If yes, the access is granted.
	if (!access_tag)
	{
		AosSetEntityError(rdata, "internal_error", "Deny Reason", "internal_error") << enderr;
		logDenied(rdata, sec_req, access_tag, "internal_error");
		granted = false;
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return false;
	}

	// Check whether the requester is in any group.
	OmnString requester_groups = sec_req.getRequesterUserGroups();
	if (requester_groups == "") 
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_IN_GROUPS, "Deny Reason", 
				AOS_DENYREASON_NOT_IN_GROUPS) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_IN_GROUPS);
		granted = false;
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	// Retrieve the allowed groups
	OmnString allowed_groups = access_tag->getNodeText();
	if (allowed_groups == "") 
	{
		// there are no groups. Normally this should not happen.
		AosSetEntityError(rdata, AOS_DENYREASON_MISSING_GROUPS, "Deny Reason", 
				AOS_DENYREASON_MISSING_GROUPS) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_MISSING_GROUPS);
		granted = false;
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	// Check whether they are in the same user domain
	if (!sec_req.isSameDomainPublic())
	{
		// The requester is not in the same domain as accessed. 
		// As far as this record is concerned, the access is denied.
		// But there can be multiple access records that are ORed. 
		// We need to check whether this record wants to deny 
		// the access.
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_SAME_USERDOMAIN, "Deny Reason", 
				AOS_DENYREASON_NOT_SAME_USERDOMAIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_SAME_USERDOMAIN);
		granted = false;
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	// Found same groups, the requester and accessed are in the same
	// user domain. It is the time to check whether the requester
	// and the accessed have common groups.
	if (!AosStrHasCommonWords(allowed_groups, requester_groups))
	{
		// No common groups
		AosSetEntityError(rdata, AOS_DENYREASON_NO_COMMON_GROUPS, "Deny Reason", 
				AOS_DENYREASON_NO_COMMON_GROUPS) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NO_COMMON_GROUPS);
		granted = false;
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);

	}

	// They do have common groups. 
	granted = true;
	denied = false;
	logPassed(rdata, sec_req, access_tag);
	return true;
}


AosJimoPtr
AosAccessGroup::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessGroup(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

