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
// 2015/01/12	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/UserAcctObj.h"

#include "SEBase/SecUtil.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEUtil/ValueDefs.h"
#include "XmlUtil/SeXmlUtil.h"


bool
AosUserAcctObj::verifySysAcct(
		const AosXmlTagPtr &acct,
		const AosRundataPtr &rdata)
{
	// This function should be called by AosSite to verify 'acct' is indeed
	// a system account. This is because system accounts created before
	// this release (09/08/2011) did not have the needed flags.
	bool modified = false;
	if (acct->getAttrStr(AOSTAG_SYSACCT) != AOSSTYPE_SYSACCT)
	{
		acct->setAttr(AOSTAG_SYSACCT, AOSSTYPE_SYSACCT);
		modified = true;
	}
	
	if (!AosXmlUtil::verifySysAcct(acct))
	{
		OmnString signature = AosSecUtil::signValue(AOSVALUE_SYSTEM_ACCOUNT);
		acct->setAttr(AOSTAG_SYSACCT_SIGNATURE, signature);
		modified = true;
	}

	if (!modified) return true;
	
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, false);
	docclient->modifyObj(acct, rdata);
	aos_assert_r(AosXmlUtil::verifySysAcct(acct), false);
	return true;
}


OmnString
AosUserAcctObj::getUsername(const u64 &userid, const AosRundataPtr &rdata)
{
	if (!userid) return "";
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, "");
	AosXmlTagPtr userdoc = docclient->getDocByDocid(userid, rdata);
	aos_assert_r(userdoc, "");
	return userdoc->getAttrStr(AOSTAG_USERNAME);
}



AosXmlTagPtr
AosUserAcctObj::getDomainDoc(const u64 &userid, const AosRundataPtr &rdata)
{
	// This function retrieves the doc of the user domain to which the user 'userid'
	// belongs. 
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	aos_assert_r(userid, 0);
	AosXmlTagPtr userdoc = docclient->getDocByDocid(userid, rdata);
	aos_assert_r(userdoc, 0);
	OmnString domain_objid = userdoc->getAttrStr(AOSTAG_PARENTC);
	aos_assert_r(domain_objid != "", 0);
	AosXmlTagPtr doc = docclient->getDocByObjid(domain_objid, rdata);
	aos_assert_r(doc, 0);
	return doc;
}

