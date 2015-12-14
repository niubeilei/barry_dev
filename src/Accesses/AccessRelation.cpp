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
// 11/23/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessRelation.h"

#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessRelation::AosAccessRelation()
:
AosAccess(AOSACCESSTYPE_RELATION)
{
}


bool 
AosAccessRelation::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted, 
		bool &denied)
{
	granted = false;
	denied = false;
	OmnNotImplementedYet;
	return true;
}


AosJimoPtr
AosAccessRelation::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessRelation(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

