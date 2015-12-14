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
#include "DocServer/ProcBatchSendStatus.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
//#include "TransUtil/XmlTrans.h"
#include "TransUtil/BigTrans.h"
#include "Util/Ptrs.h"
#include "XmlUtil/XmlTag.h"

AosProcBatchSendStatus::AosProcBatchSendStatus(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHSENDSTATUS, AosDocSvrProcId::eBatchSendStatus, regflag)
{
}


AosProcBatchSendStatus::~AosProcBatchSendStatus()
{
}


bool
AosProcBatchSendStatus::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
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

	OmnString contents = "<Contents><record ";
	contents << "result" << "=\"" << (rslt?"true":"false")<< "\" "
			    << " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchSendStatus::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchSendStatus(false);
	return proc;
}
#endif
