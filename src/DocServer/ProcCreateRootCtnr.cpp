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
#include "DocServer/ProcCreateRootCtnr.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcCreateRootCtnr::AosProcCreateRootCtnr(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CREATEROOTCTNR, AosDocSvrProcId::eCreateRootCtnr, regflag)
{
}


AosProcCreateRootCtnr::~AosProcCreateRootCtnr()
{
}


bool
AosProcCreateRootCtnr::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::createRootCtnr(...)
	//
	//// Chen Ding, 08/30/2011
	//OmnString sorted_attrs;
	//AosXmlTagPtr sorted_attrs_tag = request->getFirstChild(AOSTAG_SORTEDATTRS);
	//if (sorted_attrs_tag) sorted_attrs = sorted_attrs_tag->getNodeText();

	//OmnString memberofs;
	//AosXmlTagPtr memberofs_tag = request->getFirstChild(AOSTAG_MEMBEROFS);
	//if (memberofs_tag) memberofs = memberofs_tag->getNodeText();

	AosXmlTagPtr docxml = request->getFirstChild("doc");
	aos_assert_rr(docxml, rdata, false);

	docxml = docxml->getFirstChild();
	aos_assert_rr(docxml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr dd = parser.parse(docxml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(dd, rdata, false);

	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	aos_assert_rr(docid == dd->getAttrU64(AOSTAG_DOCID,0), rdata, false);
	bool rslt = AosDocSvrSelf->createRootCtnr(docid, dd, rdata, true);
	//Process send resp;
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCreateRootCtnr::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCreateRootCtnr(false);
	return proc;
}
#endif
