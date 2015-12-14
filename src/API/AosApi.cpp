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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 08/13/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////

/*
#include "API/AosApi.h"

#include "Util/String.h"


bool AosAddU64ValueDoc(
		const u64 &iilid, 
		const u64 &value, 
		const u64 &docid, 
		const bool value_unique, 
		const bool docid_unique, 
		AosRundata *rdata)
{
	return AosIILClient::getSelf()->addU64ValueDoc(iilid, value, docid, value_unique, 
			docid_unique, rdata);
}


AosXmlTagPtr AosCreateDoc1(
		const AosXmlTagPtr &doc, 
		AosRundata *rdata)
{
	return AosDocClient::getSelf()->createDoc(doc, rdata);
}


u64 AosCreateIIL1(
		const AosIILType &iiltype,
		const bool isPersis,
		AosRundata *rdata)
{
	return AosIILClient::getSelf()->getIILId(iiltype, isPersis, rdata);
}


OmnString AosGetParentUserGroups(const AosXmlTagPtr &user_acct)
{
	aos_assert_r(user_acct, "");
	aos_assert_r(user_acct->getAttrStr(AOSTAG_OTYPE) == AOSOTYPE_USERACCT, "");
	return user_acct->getAttrStr(AOSTAG_GROUPS);
}


AosXmlTagPtr AosRetrieveDoc(
		const OmnString &objid,
		AosRundata *rdata)
{
	return AosDocClient::getSelf()->getDoc(rdata->getSiteid(), objid, rdata);
}
*/
