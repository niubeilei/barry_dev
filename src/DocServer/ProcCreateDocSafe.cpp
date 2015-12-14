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

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcCreateDocSafe.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"



AosProcCreateDocSafe::AosProcCreateDocSafe(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CREATEDOCSAFE, AosDocSvrProcId::eCreateDocSafe, regflag)
{
}


AosProcCreateDocSafe::~AosProcCreateDocSafe()
{
}


bool
AosProcCreateDocSafe::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocSvr::createDocSafe(...)
	bool savedocflag = request->getAttrBool("savedocfile", false);

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);	

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);	

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	   	OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}
	
	// Ketty 2012/11/30
	trans->setFinishLater();
	
	// Linda 2012/08/20 save Binary Doc
	bool rslt = false;
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME, "");
		AosBuffPtr buff = doc->getNodeTextBinaryUnCopy(nodename AosMemoryCheckerArgs);
		aos_assert_r(buff && buff->dataLen() >0, false);

		OmnString signature;
		rslt = AosDocSvr::getSelf()->createBinaryDoc(doc, buff, signature, rdata, trans->getTransId());
		aos_assert_r(rslt, false);
		if (nodename == "")
		{
			doc->removeNodeTexts();
		}
		else
		{
			doc->removeNode(nodename, false, false);
		}
		aos_assert_r(signature != "", false);
		//doc->setAttr(AOSTAG_BINARY_DOC_SIGNATURE, signature);
	}

	rslt = AosDocSvrSelf->createDocSafe(rdata, doc, savedocflag, trans->getTransId());
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCreateDocSafe::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCreateDocSafe(false);
	return proc;
}
#endif
