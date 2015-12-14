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
#include "DocServer/ProcSaveToFile.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"

AosProcSaveToFile::AosProcSaveToFile(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_SAVETOFILE, AosDocSvrProcId::eSaveToFile, regflag)
{
}


AosProcSaveToFile::~AosProcSaveToFile()
{
}


bool
AosProcSaveToFile::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocSvr::saveToFile(...)
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_rr(docid, rdata, false);

	AosXmlTagPtr xml = request->getFirstChild("doc");
	aos_assert_rr(xml, rdata, false);

	xml = xml->getFirstChild();
	aos_assert_rr(xml, rdata, false);

	AosXmlParser parser;
	AosXmlTagPtr doc = parser.parse(xml->toString(), "" AosMemoryCheckerArgs);
	aos_assert_rr(doc, rdata, false);

	aos_assert_rr(doc->getAttrU64(AOSTAG_DOCID, 0) == docid, rdata, false);

	// Ketty 2012/11/30
	trans->setFinishLater();

	u32 siteid = doc->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
		OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
		rdata->setSiteid(siteid);
	}

	bool rslt = AosDocSvrSelf->saveToFile(docid, doc, rdata, trans->getTransId());
	if (!rslt)
	{
		rdata->setError() << "Failed saving to file: " << rdata->getErrmsg();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return true;
	}
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcSaveToFile::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcSaveToFile(false);
	return proc;
}
#endif
