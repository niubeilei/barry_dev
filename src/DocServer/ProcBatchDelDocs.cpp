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
// 03/22/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22

#if 0
#include "DocServer/ProcBatchDelDocs.h"

#include "API/AosApi.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "XmlUtil/SeXmlParser.h"

AosProcBatchDelDocs::AosProcBatchDelDocs(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHDELDOCS, AosDocSvrProcId::eBatchDelDocs, regflag)
{
}


AosProcBatchDelDocs::~AosProcBatchDelDocs()
{
}


bool
AosProcBatchDelDocs::proc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &request, 
		const AosBigTransPtr &trans)
{
	// The request is in the form:
	// 	<trans AOSTAG_VIRTUAL_ID="xxx"
	// 		AOSTAG_SIZEID="xxx"
	// 	</trans>
	OmnString  scanner_id = request->getAttrStr("zky_scanner_id", "");
	if (scanner_id == "")
	{
		AosSetErrorU(rdata, "invalid_scanner_id") << ": " << scanner_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt =  AosDocSvr::getSelf()->deleteBatchDocs(scanner_id, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "no_file_mgr") << ": " << scanner_id;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	aos_assert_rr(rslt, rdata, false);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchDelDocs::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchDelDocs(false);
	return proc;
}
# endif 
