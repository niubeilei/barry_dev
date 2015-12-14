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
#include "Accesses/AccessForbidden.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessForbidden::AosAccessForbidden()
:
AosAccess(AOSACCESSTYPE_FORBIDDEN)
{
}


bool 
AosAccessForbidden::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted,
		bool &denied)
{
	granted = false;
	denied = true;
	logDenied(rdata, sec_req, tag, AOS_DENYREASON_FORBIDDEN);
	return true;
}


AosJimoPtr
AosAccessForbidden::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessForbidden(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

