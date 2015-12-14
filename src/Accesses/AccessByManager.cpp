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
// This access determines the accesses based on whether the requester
// is a Direct Manager of the accessed. If yes, it is granted. Otherwise,
// it may reject it.
//
// Modification History:
// 11/08/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessByManager.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessByManager::AosAccessByManager()
:
AosAccess(AOSACCESSTYPE_BYMANAGER)
{
}


bool 
AosAccessByManager::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted,
		bool &denied)
{
	// This access checks whether the requester is the manager 
	// (either a supervisor or direct manager) of the owner of
	// the accessed. If yes, the access is granted.
	//
	// 1. User Roles
	//    This is a condition. If it is specified, the manager must
	//    belong to the roles listed here.
	// 2. User Groups
	//    This is a condition. If it is specified, the manager must
	//    belong to the groups listed here.
	// 3. Manager attribute name
	// 4. Role attribute name
	// 5. Group attribute name
	granted = false;
	denied = false;
	aos_assert_rr(tag, rdata, false);

	OmnNotImplementedYet;

	granted = false;
	denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
	if (denied)
	{
		AosSetError(rdata, "access_denied");
	}
	return true;
}


AosJimoPtr
AosAccessByManager::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessByManager(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

