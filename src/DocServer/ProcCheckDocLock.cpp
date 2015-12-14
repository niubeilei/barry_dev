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
// 02/08/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcCheckDocLock.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcCheckDocLock::AosProcCheckDocLock(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_CHECKDOCLOCK, AosDocSvrProcId::eCheckDocLock, regflag)
{
}


AosProcCheckDocLock::~AosProcCheckDocLock()
{
}


bool
AosProcCheckDocLock::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosCheckDocLock::check(...)
	aos_assert_rr(request, rdata, false);
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	OmnString type = request->getAttrStr(AOSTAG_ZKY_TYPE, "");
	aos_assert_r(type != "", false);

	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
		OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
		rdata->setSiteid(siteid);
	}
	bool result = AosDocSvr::getSelf()->procCheckLock(rdata, docid, type);

	OmnString contents = "<Contents><record ";
	contents << "result" << "=\"" << (result?"true":"false")<< "\" /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcCheckDocLock::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcCheckDocLock(false);
	return proc;
}

#endif
