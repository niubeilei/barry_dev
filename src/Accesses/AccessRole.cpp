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
#include "Accesses/AccessRole.h"

#include "API/AosApiS.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessRole::AosAccessRole()
:
AosAccess(AOSACCESSTYPE_ROLE)
{
}


bool 
AosAccessRole::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted, 
		bool &denied)
{
	// The accessed is an operation, and the operation access is defined
	// by 'tag', which is in the following form:
	// 	<access AOSTAG_TYPE="role" 
	// 		AOSTAG_DENY_ACCESS="true|false">role,role,...</access>
	// This function checks whether the requester is in one of the roles 
	// defined by 'tag'. If yes, the access is granted.
	if (!access_tag)
	{
		AosSetError(rdata, "internal_error");
		denied = false;
		granted = false;
		return false;
	}

	// Check whether the user has logged in
	if (!sec_req.isUserLoggedInPublic(rdata))
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_LOGIN, 
				"Deny Reason", AOS_DENYREASON_NOT_LOGIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_LOGIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	// Check whether the user is in the same user domain
	if (!sec_req.isSameDomainPublic())
	{
		// The requester is not in the same domain as accessed. 
		// As far as this record is concerned, the access is denied.
		// But there can be multiple access records that are ORed. 
		// We need to check whether this record wants to deny 
		// the access.
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_SAME_USERDOMAIN, 
				"Deny Reason", AOS_DENYREASON_NOT_SAME_USERDOMAIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_SAME_USERDOMAIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	OmnString requester_roles = sec_req.getRequesterUserRoles();
	if (requester_roles == "")
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NO_ROLES,
				"Deny Reason", AOS_DENYREASON_NO_ROLES) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NO_ROLES);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	OmnString opr_roles = access_tag->getNodeText();
	if (opr_roles == "") 
	{
		// there are no roles. Normally this should not happen.
		AosSetEntityError(rdata, AOS_DENYREASON_MISSING_ROLES,
				"Deny Reason", AOS_DENYREASON_MISSING_ROLES) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_MISSING_ROLES);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	// Found same roles, the requester and accessed are in the same
	// user domain. It is the time to check whether the requester
	// and the accessed have common roles.
	if (!AosStrHasCommonWords(opr_roles, requester_roles))
	{
		// They do have common roles. 
		AosSetEntityError(rdata, AOS_DENYREASON_NO_COMMON_ROLES,
				"Deny Reason", AOS_DENYREASON_NO_COMMON_ROLES) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NO_COMMON_ROLES);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	logPassed(rdata, sec_req, access_tag);
	granted = true;
	denied = false;
	return true;
}


AosJimoPtr
AosAccessRole::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessRole(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

