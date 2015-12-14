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
#include "DocServer/ProcBatchGetDocs.h"

#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "Util/Ptrs.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

AosProcBatchGetDocs::AosProcBatchGetDocs(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHGETDOCS, AosDocSvrProcId::eBatchGetDocs, regflag)
{
}


AosProcBatchGetDocs::~AosProcBatchGetDocs()
{
}


bool
AosProcBatchGetDocs::proc(const AosRundataPtr &rdata, const AosXmlTagPtr &request, const AosBigTransPtr &trans)
{
	// This function is the server side implementations of AosDocMgr::getDoc(...)
	u32 siteid = request->getAttrU32(AOSTAG_SITEID, 0);
	aos_assert_r(siteid != 0, false);
	if (siteid != rdata->getSiteid())
	{
	    OmnAlarm << "Siteid mismatch: " << siteid << ":" << rdata->getSiteid() << enderr;
	    rdata->setSiteid(siteid);
	}

	OmnString scanner_id = request->getAttrStr("zky_scanner_id", "");
	aos_assert_r(scanner_id != "", false);

	AosBuffPtr resp_buff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);

	bool rslt = AosDocSvr::getSelf()->batchGetDocs(scanner_id, resp_buff, rdata);
	if (!rslt)
	{
		rdata->setError();
		return true;
	}
	
	OmnString contents = "<Contents><record><docs><![BDATA[";
	contents << resp_buff->dataLen() << ":";
	contents.append(resp_buff->data(), resp_buff->dataLen());
	contents << "]]></docs></record></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchGetDocs::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchGetDocs(false);
	return proc;
}
#endif
