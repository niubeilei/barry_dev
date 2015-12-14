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
// 2013/12/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Accesses/AccessToken.h"

#include "API/AosApiS.h"
#include "Conds/Condition.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "SEBase/SecReq.h"
#include "XmlUtil/XmlTag.h"


extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosAccessToken(const AosRundataPtr &rdata, const OmnString &version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosAccessToken(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetError(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}


AosAccessToken::AosAccessToken(const OmnString &version)
:
AosAccess("access_token", version)
{
}


AosAccessToken::~AosAccessToken()
{
}


bool
AosAccessToken::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &worker_doc,
		const AosXmlTagPtr &jimo_doc) 
{
	return true;
}


bool 
AosAccessToken::checkAccess(
		AosSecReq &sec_req, 
		const AosXmlTagPtr &tag, 
		bool &granted, 
		bool &denied,
		const AosRundataPtr &rdata)
{
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
AosAccessToken::cloneJimo()  const
{
	try
	{
		return OmnNew AosAccessToken(*this);
	}

	catch (...)
	{
		OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}

