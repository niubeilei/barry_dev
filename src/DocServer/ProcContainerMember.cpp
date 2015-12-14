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
#include "DocServer/ProcContainerMember.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcContainerMember::AosProcContainerMember(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CONTAINERMEMBER, AosDocSvrProcId::eContainerMember, regflag)
{
}


AosProcContainerMember::~AosProcContainerMember()
{
}


bool
AosProcContainerMember::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request)
{
	// This function is the server side implementations of AosDocSvr::addContainerMember(...)
	OmnString container= request->getAttrStr("container", "");
	aos_assert_rr(container!="", rdata,  false);

	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	OmnString objid = request->getAttrStr(AOSTAG_OBJID, "");
	aos_assert_rr(objid != "", rdata, false);

	// Chen Ding, 08/30/2011
	OmnString sorted_attrs;
	AosXmlTagPtr sorted_attrs_tag = request->getFirstChild(AOSTAG_SORTEDATTRS);
	if (sorted_attrs_tag) sorted_attrs = sorted_attrs_tag->getNodeText();
	
	OmnString memberofs;
	AosXmlTagPtr memberofs_tag = request->getFirstChild(AOSTAG_MEMBEROFS);
	if (memberofs_tag) memberofs = memberofs_tag->getNodeText();

	AosXmlTagPtr xml = request->getFirstChild("origdocroot");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr origdocroot = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(origdocroot, rdata, false);

	aos_assert_rr(origdocroot->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);
	bool deleteok= false;
	bool rslt = false;// = AosDocSvrSelf->containerMember(rdata, container, origdocroot, 
			//objid, docid, deleteok, sorted_attrs, memberofs);
	if (deleteok)
	{
		if (!rslt)
		{
			rdata->setError();
			return true;
		}
	}
	OmnString contents = "<Contents><record ";
	contents << "result" << "=\"" << (deleteok?"true":"false")<< "\" "
		            << " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcContainerMember::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcContainerMember(false);
	return proc;
}
#endif
