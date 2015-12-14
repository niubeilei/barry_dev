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
// 07/06/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcBatchSendDocids.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosProcBatchSendDocids::AosProcBatchSendDocids(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHSENDDOCIDS, AosDocSvrProcId::eBatchSendDocids, regflag)
{
}


AosProcBatchSendDocids::~AosProcBatchSendDocids()
{
}


bool
AosProcBatchSendDocids::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)
	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	    OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}

	u32 client_id = ((trans->getTransId() >> 32 ) & 0x00ffffff);

	bool rslt = AosDocSvr::getSelf()->batchSendDocids(request, client_id, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchSendDocids::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchSendDocids(false);
	return proc;
}
#endif
