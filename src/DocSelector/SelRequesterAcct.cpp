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
//
// Modification History:
// 01/06/2012	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DocSelector/SelRequesterAcct.h"

#include "SEInterfaces/DocClientObj.h"
#include "Rundata/Rundata.h"
#include "ValueSel/ValueSel.h"


AosDocSelRequesterAcct::AosDocSelRequesterAcct(const bool reg)
:
AosDocSelector(AOSDOCSELTYPE_REQUESTER_ACCT, AosDocSelectorType::eRequesterAcct, reg)
{
}


AosDocSelRequesterAcct::~AosDocSelRequesterAcct()
{
}


AosXmlTagPtr 
AosDocSelRequesterAcct::selectDoc(
		const AosXmlTagPtr &sdoc, 
		const AosRundataPtr &rdata)
{
	// This selector selects the requester's account.
	// AOSLOG_ENTER_R(rdata, 0);
	u64 userid = rdata->getUserid();
	if (!userid)
	{
		// AOSLOG_LEAVE(rdata);
		return 0;
	}

	AosXmlTagPtr userdoc = AosDocClientObj::getDocClient()->getDocByDocid(userid, rdata);
	// AOSLOG_LEAVE(rdata);
	return userdoc;
}


OmnString 
AosDocSelRequesterAcct::getXmlStr(
		const OmnString &tagname,
		const int level,
		const AosRandomRulePtr &rule,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return "";
}


