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
#include "Accesses/AccessRoot.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessRoot::AosAccessRoot()
:
AosAccess(AOSACCESSTYPE_ROOT)
{
}


bool 
AosAccessRoot::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted, 
		bool &denied)
{
	granted = false;
	denied = false;
	aos_assert_rr(tag, rdata, false);
	if (sec_req.isRoot(rdata))
	{
		granted = true;
		return true;
	}

	granted = false;
	denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, true);
	if (denied)
	{
		AosSetError(rdata, "access_denied");
	}
	return true;
}


AosJimoPtr
AosAccessRoot::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessRoot(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

