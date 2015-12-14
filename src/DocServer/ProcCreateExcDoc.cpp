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
// 08/01/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcCreateExcDoc.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcCreateExcDoc::AosProcCreateExcDoc(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CREATEEXCDOC, AosDocSvrProcId::eCreateExcDoc, regflag)
{
}


AosProcCreateExcDoc::~AosProcCreateExcDoc()
{
}


bool
AosProcCreateExcDoc::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocSvr::createExclusiveDoc(...)
	AosXmlTagPtr xml = request->getFirstChild("newxml");
	aos_assert_rr(xml, rdata, false);

	xml = xml ->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr newxml = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(newxml, rdata, false);

	u32 siteid = newxml->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	   OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	   rdata->setSiteid(siteid);
	}

	bool rslt = AosDocSvrSelf->createExclusiveDoc(newxml, rdata);
	if(!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCreateExcDoc::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCreateExcDoc(false);
	return proc;
}
#endif
