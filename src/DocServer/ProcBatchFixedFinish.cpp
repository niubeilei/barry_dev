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
// 2013/01/15 Create By Ken Lee
////////////////////////////////////////////////////////////////////////////

// Ketty 2013/03/22
#if 0
#include "DocServer/ProcBatchFixedFinish.h"

#include "API/AosApiG.h"
#include "DocServer/DocInfo2.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "TransUtil/BigTrans.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/DocTypes.h"
#include "SEInterfaces/StorageEngineObj.h"



AosProcBatchFixedFinish::AosProcBatchFixedFinish(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHFIXEDFINISH, AosDocSvrProcId::eBatchFixedFinish, regflag)
{

}


AosProcBatchFixedFinish::~AosProcBatchFixedFinish()
{
}


bool
AosProcBatchFixedFinish::proc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &request, 
		const AosBigTransPtr &trans)
{
	u64 job_id = request->getAttrU64(AOSTAG_JOB_DOCID, 0);
	if (!job_id)
	{
		AosSetErrorU(rdata, "invalid_job_id") << ": " << job_id;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;                             
	}

	u64 total_num_docs = request->getAttrU64("total_num_docs", 0);
	bool rslt = AosStorageEngineMgr::getSelf()->finishData(job_id, total_num_docs, rdata); 
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchFixedFinish::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchFixedFinish(false);
	return proc;
}
# endif
