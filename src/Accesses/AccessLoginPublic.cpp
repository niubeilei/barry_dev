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
#include "Accesses/AccessLoginPublic.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessLoginPublic::AosAccessLoginPublic()
:
AosAccess(AOSACCESSTYPE_LOGIN_PUBLIC)
{
}


bool 
AosAccessLoginPublic::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted, 
		bool &denied)
{
	granted = false;
	denied = false;
	aos_assert_rr(access_tag, rdata, false);
	if (!sec_req.isUserLoggedInPublic(rdata))
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_LOGIN, 
				"Deny Reason", AOS_DENYREASON_NOT_LOGIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_LOGIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	if (!sec_req.isSameDomainPublic())
	{
		AosSetEntityError(rdata, AOS_DENYREASON_NOT_SAME_USERDOMAIN, 
				"Deny Reason", AOS_DENYREASON_NOT_SAME_USERDOMAIN) << enderr;
		logDenied(rdata, sec_req, access_tag, AOS_DENYREASON_NOT_SAME_USERDOMAIN);
		denied = access_tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
		return true;
	}

	logPassed(rdata, sec_req, access_tag);
	granted = true;
	denied = false;
	rdata->setOk();
	return true;
}


AosJimoPtr
AosAccessLoginPublic::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessLoginPublic(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

