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
#include "DocServer/ProcGetDoc.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/XmlTag.h"

AosProcGetDoc::AosProcGetDoc(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_GETDOC, AosDocSvrProcId::eGetDoc, regflag)
{
}


AosProcGetDoc::~AosProcGetDoc()
{
}


bool
AosProcGetDoc::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	    OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}

	AosXmlTagPtr doc = AosDocSvr::getSelf()->getDoc(docid, rdata);
	if (!doc)
	{
		rdata->setError();
		return true;
	}
	aos_assert_rr(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);
	aos_assert_rr(doc->getAttrU32(AOSTAG_SITEID, 0) == siteid, rdata, false);

	//Linda 2012/08/21 get binary doc
	if (doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		doc = doc->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(doc, false);
	}

	bool need_binary = request->getAttrBool("need_binarydata", false);
	if (need_binary)
	{
		//aos_assert_r(doc->getAttrStr(AOSTAG_BINARY_DOC_SIGNATURE, "") != "", false);
		aos_assert_r(doc->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC, false);
		AosBuffPtr buff;
		bool rslt = AosDocSvr::getSelf()->retrieveBinaryDoc(doc, buff, rdata);
		if (!rslt)
		{
			rdata->setError();
			return true;
		}
		aos_assert_r(buff->dataLen() > 0, false);
		OmnString nodename = doc->getAttrStr(AOSTAG_BINARY_NODENAME);
		doc->setTextBinary(nodename, buff);
	}
	//doc->removeAttr(AOSTAG_BINARY_DOC_SIGNATURE);

	OmnString contents = "<Contents><record>";
	contents << "<doc>" << doc->toString() << "</doc>"
		<< "</record></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcGetDoc::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcGetDoc(false);
	return proc;
}
#endif
