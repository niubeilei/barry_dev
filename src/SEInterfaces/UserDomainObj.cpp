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
// 2015/01/12	Created by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/UserDomainObj.h"

#include "DocUtil/DocProcUtil.h"
#include "XmlUtil/SeXmlUtil.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DocClientObj.h"
#include "SEInterfaces/UserAcctObj.h"


AosXmlTagPtr 
AosUserDomainObj::createUserOprArd(
		const u64 &userid, 
		const OmnString &username,
		const AosRundataPtr &rdata)
{
	AosDocClientObjPtr docclient = AosDocClientObj::getDocClient();
	aos_assert_r(docclient, 0);
	// This function creates a user operation access record. This is done
	// by copying the default user operation access record. 
	AosXmlTagPtr domain_doc = AosUserAcctObj::getDomainDoc(userid, rdata);
	if (!domain_doc)
	{
		// Failed retrieving the domain docid. This is an error.
		AosSetError(rdata, AOSLT_INVALID_USER);
		OmnAlarm << rdata->getErrmsg() << ": " << userid << enderr;
		return 0;
	}

	OmnString domain_docid = domain_doc->getAttrStr(AOSTAG_DOCID);
	OmnString dft_oprard_objid = AosObjid::composeDefaultUserOprArdObjid(domain_docid);
	if (dft_oprard_objid == "")
	{
		// Should never happen
		AosSetError(rdata, AOSLT_INTERNAL_ERROR);
		OmnAlarm << rdata->getErrmsg() << ": " << userid << ":" << domain_docid << enderr;
		return 0;
	}

	AosXmlTagPtr doc = docclient->getDocByObjid(dft_oprard_objid, rdata);
	if (!doc)
	{
		// The doc is not there yet. Need to create it.
		AosSetError(rdata, AOSLT_NO_DEFAULT_USEROPRARD);
		return 0;
	}

	OmnString user_oprard_objid = AosObjid::composeUserOprArdObjid(userid);
	if (user_oprard_objid == "")
	{
		// Should never happen
		AosSetError(rdata, AOSLT_INTERNAL_ERROR);
		OmnAlarm << rdata->getErrmsg() << ": " << userid << enderr;
		return 0;
	}
	AosXmlUtil::removeMetaAttrs(doc);
	doc->setAttr(AOSTAG_OBJID, user_oprard_objid);
	doc->setAttr(AOSTAG_USERID, userid);
	doc->setAttr(AOSTAG_USERNAME, username);

	// Create the doc
	OmnString docstr = doc->toString();
	AosXmlTagPtr user_opr = docclient->createDocSafe1(rdata, docstr, 
			rdata->getCid(), "", true, false, false, false, false, false);
	if (!user_opr)
	{
		// Should never happen
		OmnAlarm << "Failed creating the doc: " << docstr << enderr;
		return 0;
	}

	return user_opr;
}


