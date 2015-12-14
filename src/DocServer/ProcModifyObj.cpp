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

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcModifyObj.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"

AosProcModifyObj::AosProcModifyObj(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_MODIFYOBJ, AosDocSvrProcId::eModifyObj, regflag)
{
}


AosProcModifyObj::~AosProcModifyObj()
{
}


bool
AosProcModifyObj::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocSvr::modifyObj(...)

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr newxml = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(newxml, rdata, false);

	//bool synobj = (request->getAttrStr("synobj") == "true");
	//AosXmlTagPtr xml1 = request->getFirstChild("root");
	//aos_assert_rr(xml1, rdata, false);

	//xml1 = xml1->getFirstChild();
	//aos_assert_rr(xml1, rdata, false);

	//AosXmlParser parser1;
	//AosXmlTagPtr root = parser1.parse(xml1->toString(), "" AosMemoryCheckerArgs);
	//aos_assert_rr(root, rdata, false);

	//AosXmlTagPtr xml2 = request->getFirstChild("origdoc");
	//aos_assert_rr(xml2, rdata, false);

	//xml2 = xml2->getFirstChild();
	//aos_assert_rr(xml2, rdata, false);

	//AosXmlParser parser2;
	//AosXmlTagPtr origdoc = parser2.parse(xml2->toString(), "" AosMemoryCheckerArgs);
	//aos_assert_rr(origdoc, rdata, false);
	
	// Ketty 2012/11/30
	trans->setFinishLater();

	u64 newdid = newxml->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(newdid, rdata, false);

	u32 siteid = newxml->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
		OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
		rdata->setSiteid(siteid);
	}

	bool rslt = AosDocSvrSelf->modifyObj(rdata, newxml, newdid, trans->getTransId());
	if(!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcModifyObj::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcModifyObj(false);
	return proc;
}

#endif
