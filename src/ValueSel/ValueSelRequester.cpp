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
// This value selector selects the requester. It can select one of the 
// following:
// 		1. The requester's user account doc
// 		2. The requester's cid
// 		3. The requester's user name
//		4. The requester's userid
// This is determined by sdoc's AOSTAG_GET_REQUESTER_TYPE attribute. If
// not specified, it defaults to AOSVALUE_USERID.
//
// Modification History:
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ValueSel/ValueSelRequester.h"

#include "Actions/ActUtil.h"
#include "Debug/Except.h"
#include "Debug/Debug.h"
#include "Util/OmnNew.h"
#include "Alarm/Alarm.h"
#include "Util/DynArray.h"
#include "Util/UtUtil.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"
#include "SEInterfaces/UserAcctObj.h"

#if 0
AosValueSelRequester::AosValueSelRequester(const bool reg)
:
AosValueSel(AOSACTOPRID_REQUESTER, AosValueSelType::eRequester, reg)
{
}


AosValueSelRequester::AosValueSelRequester(
        const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
:
AosValueSel(AOSACTOPRID_REQUESTER, AosValueSelType::eRequester, false)
{
}


AosValueSelRequester::~AosValueSelRequester()
{
}


bool
AosValueSelRequester::run(
		AosValueRslt    &valueRslt,
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(rdata, false);

	AosUserAcctObjPtr req_acct = rdata->getRequesterAcct();
	if (!req_acct)
	{
		valueRslt.reset();
		return true;
	}

	AosXmlTagPtr doc = req_acct->getDoc();
	if (!doc)
	{
		AosSetErrorUser(rdata, "missing_acct_doc") << enderr;
		valueRslt.reset();
		return false;
	}

	// Determine the requester type
	OmnString type = sdoc->getAttrStr(AOSTAG_GET_REQUESTER_TYPE);
	if (type == "")
	{
		// It defaults to get the requester's docid
		type = AOSVALUE_USERID;
	}

	if (type == AOSVALUE_REQUESTER_DOC)
	{
		// It is to get requester's doc
		valueRslt.setXml(rdata->getRequesterAcctDoc());
		return true;
	}

	if (type == AOSVALUE_CID)
	{
		// It is to get the requester's cloud id
		valueRslt.setStr(rdata->getRequesterCid());
		return true;
	}

	if (type == AOSVALUE_USERID)
	{
		// It is to get the requester's docid
		valueRslt.setU64(rdata->getRequesterUserid());
		return true;
	}

	if (type == AOSVALUE_USERNAME)
	{
		valueRslt.setStr(rdata->getRequesterUsername());
		return true;
	}

	AosSetError(rdata, "unrec_requester_type") << ": " << sdoc->toString() << enderr;
	return false;
}


OmnString 
AosValueSelRequester::getXmlStr(
		const OmnString &tagname, 
		const int level,
		const AosRandomRulePtr &rule, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


AosValueSelObjPtr
AosValueSelRequester::clone(const AosXmlTagPtr &sdoc, const AosRundataPtr &rdata)
{
	try
	{
		return OmnNew AosValueSelRequester(sdoc, rdata);
	}

	catch (...)
	{
		AosSetErrorU(rdata, "failed_creating_obj") << enderr;
		return 0;
	}
}






#endif
