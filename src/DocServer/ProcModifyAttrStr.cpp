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
#include "DocServer/ProcModifyAttrStr.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcModifyAttrStr::AosProcModifyAttrStr(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_MODIFYATTRSTR, AosDocSvrProcId::eModifyAttrStr, regflag)
{
}


AosProcModifyAttrStr::~AosProcModifyAttrStr()
{
}


bool
AosProcModifyAttrStr::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::modifyAttrStr(...)
	//OmnString attrname = request->getAttrStr("attrname", "");
	//aos_assert_rr(attrname != "", rdata, false);

	//OmnString newvalue = request->getAttrStr("newvalue", "");
	//aos_assert_rr(newvalue != "", rdata, false);

	//OmnString oldvalue	= request->getAttrStr("oldvalue", "");
	//aos_assert_rr(oldvalue != "", rdata, false);

	//bool value_unique = (request->getAttrStr("value_unique") == "true");
	//bool docid_unique = (request->getAttrStr("docid_unique") == "true");
	//bool exist = (request->getAttrStr("exist") == "true");
	
	//OmnString fname = request->getAttrStr("fname", "");
	//int line = request->getAttrInt("line", 0);

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	u64 docid = doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);
	
	aos_assert_rr(docid == doc->getAttrU64(AOSTAG_DOCID, 0), rdata, false);
	bool rslt = AosDocSvrSelf->modifyAttrStr(rdata, doc, docid, true);
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
AosProcModifyAttrStr::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcModifyAttrStr(false);
	return proc;
}
#endif 
