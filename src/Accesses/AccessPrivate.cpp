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
#include "Accesses/AccessPrivate.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessPrivate::AosAccessPrivate()
:
AosAccess(AOSACCESSTYPE_PRIVATE)
{
}


bool 
AosAccessPrivate::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted, 
		bool &denied)
{
	granted = false;
	denied = false;
	aos_assert_rr(access_tag, rdata, false);

	// Check whether the user has logged in
	if (!sec_req.isUserLoggedInPublic(rdata))
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_LOGIN, 
				"Deny Reason", AOS_DENYREASON_NOT_LOGIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_LOGIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	if (sec_req.doesRequesterOwnAccessed(rdata))
	{
		logPassed(rdata, sec_req, access_tag);
		granted = true;
		denied = false;
		rdata->setOk();
		return true;
	}

	AosSetEntityError(rdata, AOS_DENYREASON_PRIVATE, 
			"Deny Reason", AOS_DENYREASON_PRIVATE) << enderr;
	logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_PRIVATE);
	granted = false;
	denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, true);
	return true;
}


AosJimoPtr
AosAccessPrivate::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessPrivate(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

