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
#include "Accesses/AccessUserEnum.h"

#include "API/AosApiS.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessUserEnum::AosAccessUserEnum()
:
AosAccess(AOSACCESSTYPE_USER_ENUM)
{
}


bool 
AosAccessUserEnum::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted,
		bool &denied)
{
	// The accessed is an operation, and the operation access is defined
	// by 'tag', which is in the following form:
	// 		<access type="group" 
	// 			AOSTAG_DENY_ACCESS="true|false">username,username,...
	// 		</access>
	// This function checks whether the requester is in the usernames
	// defined by 'tag'. If yes, the access is granted.
	if (!access_tag)
	{
		AosSetError(rdata, "internal_error");
		logDenied(rdata, sec_req, access_tag, "internal_error");
		denied = false;
		granted = false;
		return false;
	}

	OmnString username = sec_req.getRequesterUsername();
	if (username == "") 
	{
		// This is an error.
		AosSetError(rdata, "internal_error");
		logDenied(rdata, sec_req, access_tag, "internal_error");
		denied = false;
		granted = false;

	}

	// Check whether the requester has logged in
	if (!sec_req.isUserLoggedInPublic(rdata))
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_LOGIN, 
				"Deny Reason", AOS_DENYREASON_NOT_LOGIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_LOGIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	OmnString users = access_tag->getNodeText();
	if (users == "")
	{
		AosSetEntityError(rdata, AOS_DENYREASON_MISSING_USERS, 
				"Deny Reason", AOS_DENYREASON_MISSING_USERS) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_MISSING_USERS);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

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
		return false;
	}

	if (!AosStrHasCommonWords(users, username))
	{
		AosSetEntityError(rdata, AOS_DENYREASON_USER_NOT_LISTED, 
				"Deny Reason", AOS_DENYREASON_USER_NOT_LISTED) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_USER_NOT_LISTED);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	logPassed(rdata, sec_req, access_tag);
	granted = true;
	denied = false;
	return true;
}


AosJimoPtr
AosAccessUserEnum::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessUserEnum(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

