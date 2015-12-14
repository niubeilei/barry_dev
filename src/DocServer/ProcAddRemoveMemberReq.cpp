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
//
// Modification History:
// 07/29/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcAddRemoveMemberReq.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcAddRemoveMemberReq::AosProcAddRemoveMemberReq(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_ADDREMOVEMEMBERREQ, AosDocSvrProcId::eAddRemoveMemberReq, regflag)
{
}


AosProcAddRemoveMemberReq::~AosProcAddRemoveMemberReq()
{
}


bool
AosProcAddRemoveMemberReq::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::addRemoveMemberReq(...)
	OmnString ctnr_objid = request->getAttrStr("ctnr_objid", "");
	aos_assert_rr(ctnr_objid !="", rdata, false);

	// Chen Ding, 08/30/2011
	OmnString sorted_attrs;
	AosXmlTagPtr sorted_attrs_tag = request->getFirstChild(AOSTAG_SORTEDATTRS);
	if (sorted_attrs_tag) sorted_attrs = sorted_attrs_tag->getNodeText();

	OmnString memberofs;
	AosXmlTagPtr memberofs_tag = request->getFirstChild(AOSTAG_MEMBEROFS);
	if (memberofs_tag) memberofs = memberofs_tag->getNodeText();

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	bool rslt = false; // = AosDocSvrSelf->addRemoveMemberRequest(doc, ctnr_objid, sorted_attrs, memberofs, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcAddRemoveMemberReq::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcAddRemoveMemberReq(false);
	return proc;
}
#endif
