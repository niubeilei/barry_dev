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
#include "DocServer/ProcDocLock.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "DocLock/DocLock.h"
#include "XmlUtil/SeXmlParser.h"

AosProcDocLock::AosProcDocLock(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_DOCLOCK, AosDocSvrProcId::eDocLock, regflag)
{
}


AosProcDocLock::~AosProcDocLock()
{
}


bool
AosProcDocLock::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocLock::check(...)
	aos_assert_rr(request, rdata, false);
	u64 docid = request->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(docid, false);

	OmnString lock_type = request->getAttrStr(AOSTAG_LOCK_TYPE, "");
	aos_assert_r(lock_type != "", false);

	u64 lock_timer = request->getAttrU64(AOSTAG_LOCK_TIMER, 3000);

	u64 lockid = request->getAttrU64(AOSTAG_LOCK_DID, 0);

	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	    OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}

	bool rslt = AosDocSvr::getSelf()->procDocLock(rdata, trans, docid, lock_type, lock_timer, lockid);
	aos_assert_r(rslt, false);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcDocLock::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcDocLock(false);
	return proc;
}

#endif
