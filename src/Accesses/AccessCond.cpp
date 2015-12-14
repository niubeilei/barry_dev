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
// 01/19/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessCond.h"

#include "API/AosApiS.h"
#include "Conds/Condition.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


AosAccessCond::AosAccessCond()
:
AosAccess(AOSACCESSTYPE_CONDITION)
{
}


bool 
AosAccessCond::checkAccess(
		const AosRundataPtr &rdata,
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted, 
		bool &denied)
{
	// 	<access AOSTAG_TYPE=AOSACCESSTYPE_DOCATTR
	// 		AOSTAG_DENY_ACCESS="true|false">
	// 		<cond .../>
	// 		<cond .../>
	// 		...
	// 	</access>
	// In the current implementations, all <record>'s are ANDed.
	if (!tag)
	{
		AosSetErrorUser(rdata, "internal_error");
		denied = false;
		granted = false;
		return false;
	}

	if (AosCondition::evalCondsStatic(tag, eMaxConds, rdata))
	{
		logPassed(rdata, sec_req, tag);
		granted = true;
		denied = false;
		return true;
	}

	denied = tag->getAttrBool(AOSTAG_DENY_ACCESS, false);
	granted = false;
	if (denied)
	{
		logDenied(rdata, sec_req, tag, AOS_DENYREASON_COND_FAILED);
	}
	return true;
}


AosJimoPtr
AosAccessCond::cloneJimo() const
{
	try
	{
		return OmnNew AosAccessCond(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed cloning object" << enderr;
	}

	return 0;
}

