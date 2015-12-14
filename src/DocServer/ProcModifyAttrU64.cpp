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
// 08/04/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "DocServer/ProcModifyAttrU64.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcModifyAttrU64::AosProcModifyAttrU64(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_MODIFYATTRU64, AosDocSvrProcId::eModifyAttrU64, regflag)
{
}


AosProcModifyAttrU64::~AosProcModifyAttrU64()
{
}


bool
AosProcModifyAttrU64::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::modifyAttrU64(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	OmnString attrname = request->getAttrStr("attrname", "");
	aos_assert_rr(attrname != "", rdata, false);

	u64 newvalue = request->getAttrU64("newvalue", 0);
	aos_assert_rr(newvalue, rdata, false);

	u64 oldvalue = request->getAttrU64("oldvalue",0);
	aos_assert_rr(oldvalue, rdata, false);

	bool value_unique = (request->getAttrStr("value_unique") == "true");
	bool docid_unique = (request->getAttrStr("docid_unique") == "true");
	bool exist = (request->getAttrStr("exist") == "true");

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "");
	aos_assert_rr(doc, rdata, false);
	
	aos_assert_rr(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);
	bool rslt = false;// AosDocSvrSelf->modifyAttrU64(rdata, doc, docid, 
			//attrname, newvalue, oldvalue, value_unique, docid_unique, exist, true);
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
AosProcModifyAttrU64::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcModifyAttrU64(false);
	return proc;
}

#endif
