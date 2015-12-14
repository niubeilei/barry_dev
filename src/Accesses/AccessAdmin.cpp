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
#include "Accesses/AccessAdmin.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessAdmin::AosAccessAdmin()
:
AosAccess(AOSACCESSTYPE_ADMIN)
{
}


bool 
AosAccessAdmin::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted,
		bool &denied)
{
	// The tag format is:
	// 	<tag AOSTAG_DENY_ACCESS="true|false" type=AOSACCESSTYPE_ADMIN/>
	granted = false;
	denied = false;
	aos_assert_rr(tag, rdata, false);
	if (sec_req.isAdminPublic(rdata))
	{
		logPassed(rdata, sec_req, tag);
		rdata->setOk();
		granted = true;
		denied = false;
		return true;
	}

	granted = false;
	denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, true);
	if (denied)
	{
		logDenied(rdata, sec_req, tag, AOS_DENYREASON_NOT_ADMIN);
		AosSetErrorUser(rdata, AOS_DENYREASON_NOT_ADMIN);
	}
	return true;
}


AosJimoPtr
AosAccessAdmin::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessAdmin(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

