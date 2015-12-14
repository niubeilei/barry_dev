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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcCreateObjByTemplate.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"


AosProcCreateObjByTemplate::AosProcCreateObjByTemplate(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CREATEOBJBYTEMP, AosDocSvrProcId::eCreateObjByTemplate, regflag)
{
}


AosProcCreateObjByTemplate::~AosProcCreateObjByTemplate()
{
}


bool
AosProcCreateObjByTemplate::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::createDoc(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr newxml = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(newxml, rdata, false);
	
	//OmnString sorted_attrs;
	//AosXmlTagPtr sorted_attrs_tag = request->getFirstChild(AOSTAG_SORTEDATTRS);
	//if (sorted_attrs_tag) sorted_attrs = sorted_attrs_tag->getNodeText();

	//OmnString memberofs;
	//AosXmlTagPtr memberofs_tag = request->getFirstChild(AOSTAG_MEMBEROFS);
	//if (memberofs_tag) memberofs = memberofs_tag->getNodeText();

	//AosXmlTagPtr xml1 = request->getFirstChild("root");
	//aos_assert_rr(xml1, rdata, false);

	//xml1 = xml1 ->getFirstChild();
	//aos_assert_rr(xml1, rdata, false);

	//AosXmlParser parser1;
	//AosXmlTagPtr root = parser1.parse(xml1->toString(), "" AosMemoryCheckerArgs);
	//aos_assert_rr(root, rdata, false);

	aos_assert_rr(newxml->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);

	bool rslt = AosDocSvrSelf->createObjByTemplate(rdata, newxml, true);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCreateObjByTemplate::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCreateObjByTemplate(false);
	return proc;
}
#endif
