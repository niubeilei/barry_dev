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
#include "DocServer/ProcBatchFixedStart.h"

#include "API/AosApiG.h"
#include "DocServer/DocInfo2.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "StorageEngine/StorageEngineMgr.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/DocTypes.h"


AosProcBatchFixedStart::AosProcBatchFixedStart(const bool regflag)
:
AosDocSvrProc(AOSDOCSVRPROCNAME_BATCHFIXEDSTART, AosDocSvrProcId::eBatchFixedStart, regflag)
{

}


AosProcBatchFixedStart::~AosProcBatchFixedStart()
{
}


bool
AosProcBatchFixedStart::proc(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &request, 
		const AosBigTransPtr &trans)
{
	u64 job_id = request->getAttrU64(AOSTAG_JOB_DOCID, 0);
	if (!job_id)
	{
		AosSetErrorU(rdata, "invalid_job_id:") << job_id << enderr;
		OmnAlarm << rdata->getErrmsg() << enderr; 
		return false;                             
	}

	u32 job_serverid = request->getAttrU64(AOSTAG_JOBSERVERID, u32(-1)); 
	if (job_serverid == u32(-1))
	{
		AosSetErrorU(rdata, "invalid_job_serverid") << job_serverid << enderr;			
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	bool rslt = AosStorageEngineMgr::getSelf()->startData(job_id, job_serverid, rdata); 
	if (!rslt)
	{
		rdata->setError();
		return true;
	}

	OmnString contents = "<Contents><record ";
	contents << "rslt" << "=\"" << (rslt?"true":"false")<< "\" "
			    << " /></Contents>";
	rdata->setContents(contents);
	rdata->setOk();
	return true;
}


AosDocSvrProcPtr
AosProcBatchFixedStart::clone()
{
	AosDocSvrProcPtr proc = OmnNew AosProcBatchFixedStart(false);
	return proc;
}
#endif 
