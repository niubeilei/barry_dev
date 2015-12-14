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
#include "Accesses/AccessPublic.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessPublic::AosAccessPublic()
:
AosAccess(AOSACCESSTYPE_PUBLIC)
{
}


bool 
AosAccessPublic::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &access_tag, 
		bool &granted, 
		bool &denied)
{
	logPassed(rdata, sec_req, access_tag);
	granted = true;
	denied = false;
	return true;
}


AosJimoPtr
AosAccessPublic::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessPublic(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

