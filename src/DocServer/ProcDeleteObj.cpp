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
#include "DocServer/ProcDeleteObj.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcDeleteObj::AosProcDeleteObj(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_DELETEOBJ, AosDocSvrProcId::eDeleteObj, regflag)
{
}


AosProcDeleteObj::~AosProcDeleteObj()
{
}


bool
AosProcDeleteObj::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocSvr::deleteObj(...)
	u64	docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	AosXmlTagPtr xml = request->getFirstChild("origdocroot");
	AosXmlTagPtr origdocroot;
	if (xml)
	{
		xml = xml->getFirstChild();
		aos_assert_rr(xml, rdata, false);

		AosXmlParser parser;
		origdocroot = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
		aos_assert_rr(origdocroot, rdata, false);

		u32 siteid = origdocroot->getAttrU32(AOSTAG_SITEID, 0);
		aos_assert_r(siteid != 0, false);
		if (siteid != rdata->getSiteid())
		{
			OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
			rdata->setSiteid(siteid);
		}
	}
	aos_assert_r(origdocroot, false);	
	
	// Ketty 2012/11/30
	trans->setFinishLater();

	// Linda, delete binary doc
	bool rslt = false;
	if (origdocroot->getAttrStr(AOSTAG_OTYPE, "") == AOSOTYPE_BINARYDOC)
	{
		rslt = AosDocSvr::getSelf()->deleteBinaryDoc(docid, rdata, trans->getTransId());	
		if (!rslt)
		{
			rdata->setError() << "Failed to remove binary doc!" << docid;
			OmnAlarm << rdata->getErrmsg() << enderr;
			return false;
		}
	}

	rslt = AosDocSvr::getSelf()->deleteObj(rdata, docid, trans->getTransId());
	//Process send resp
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcDeleteObj::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcDeleteObj(false);
	return proc;
}

#endif
