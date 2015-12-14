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
// 08/11/2011	Created by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcCreateDocByTemplate.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"


AosProcCreateDocByTemplate::AosProcCreateDocByTemplate(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CREATEDOCBYTEMP, AosDocSvrProcId::eCreateDocByTemplate, regflag)
{
}


AosProcCreateDocByTemplate::~AosProcCreateDocByTemplate()
{
}


bool
AosProcCreateDocByTemplate::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::createDocByTemplate(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	//// Chen Ding, 08/30/2011
	//OmnString sorted_attrs;
	//AosXmlTagPtr sorted_attrs_tag = request->getFirstChild(AOSTAG_SORTEDATTRS);
	//if (sorted_attrs_tag) sorted_attrs = sorted_attrs_tag->getNodeText();

	//OmnString memberofs;
	//AosXmlTagPtr memberofs_tag = request->getFirstChild(AOSTAG_MEMBEROFS);
	//if (memberofs_tag) memberofs = memberofs_tag->getNodeText();

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr tmpl = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(tmpl, rdata, false);
	
	bool rslt = AosDocSvrSelf->createDocByTemplate(rdata, tmpl, true);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCreateDocByTemplate::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCreateDocByTemplate(false);
	return proc;
}
#endif
