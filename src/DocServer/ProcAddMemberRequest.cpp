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
#include "DocServer/ProcAddMemberRequest.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcAddMemberRequest::AosProcAddMemberRequest(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_ADDADDMEMBERREQ, AosDocSvrProcId::eAddMemberRequest, regflag)
{
}


AosProcAddMemberRequest::~AosProcAddMemberRequest()
{
}


bool
AosProcAddMemberRequest::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::addAddMemberRequest(...)
	OmnString ctnr_objid = request->getAttrStr("ctnr_objid", "");
	aos_assert_rr(ctnr_objid !="", rdata,  false);

	u64 userid = request->getAttrU64(AOSTAG_USERID, 0);
	aos_assert_rr(userid, rdata, false);

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	bool rslt = false;// = AosDocSvrSelf->addAddMemberRequest(doc, ctnr_objid, userid, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	return true;
}


AosDocSvrProcPtr
AosProcAddMemberRequest::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcAddMemberRequest(false);
	return proc;
}
#endif
